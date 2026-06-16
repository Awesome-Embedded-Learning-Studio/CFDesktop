---
title: Logger Flush 丢失唤醒死锁
description: 异步日志 logger_concurrency_test 在 clang/CI 下极端偶然卡死的根因定位与修复——flush_completed_cv_ 丢失唤醒（主因）与 MpscQueue readPos_ 数据竞争（次因）
---

# Logger Flush 丢失唤醒死锁 -- 排查与修复记录

## 背景

`logger_concurrency_test` 在 clang 构建（尤其 CI 高负载）下**极端偶然**地卡死：CTest 输出停在 `Start 11: logger_concurrency_test`，进程永不返回。直接怀疑死锁，但常规手段碰壁——

- 隔离运行该二进制 **23 次全部通过**（fast 配置：2 线程 × 50 条，队列 65536 永不满）；
- 单核 `taskset`、填满队列的 stress 配置也无法在几次内复现；
- 静态读代码看不出任何 AB-BA 锁环。

一句话定性：**不是确定性死锁，是时序敏感的罕见竞态**。这类 bug 不能靠"多跑几次碰运气"定位，必须换工具。

## 排查方法论（可复用）

定位罕见并发 bug 的三件套，按顺序用：

1. **ThreadSanitizer（TSan）** —— 不依赖时序，基于 happens-before **确定性**报数据竞争与锁环。先确认"确有并发缺陷"，再缩小范围。本例 TSan 当场抓到一处 `readPos_` 数据竞争，并（因 TSan 改变线程调度）顺带暴露了卡死。

2. **脱离重依赖的最小探针** —— logger 源码不依赖 Qt（`LogRecord` 是纯 `std`），于是用 `clang++` 直接编译 3 个 `.cpp` + 一个压测驱动，绕过整个 CMake/Qt 构建链。压测驱动刻意制造最坏组合：多生产者 + 一个**紧循环调 `flush_sync()`** 的 flusher 线程。复现率从"23 次不卡"提升到"约 1/3 卡死"。

3. **SIGABRT 看门狗 + gdb 作父进程** —— 罕见卡死无法 `gdb -p` 附加（ptrace_scope=1、无 sudo）。解法：探针内置看门狗线程，8 秒无进展则 `raise(SIGABRT)`；在 gdb 下 `run` 因信号自动中断，随后的 `-ex "thread apply all bt"` 便能在**卡死现场**抓全部线程栈。

最后用**逐事件插桩**（flush_sync 入口/出口、worker 完成事件、worker 心跳）打印到 stderr，在卡死时读末尾几行，精确还原控制流。

## 根因：`flush_completed_cv_` 丢失唤醒

`AsyncPostQueue::flush_sync()` 用 token 机制等待 flush 完成：

```cpp
// flush_sync() —— 唯一无超时的阻塞点
uint64_t my_token = flush_token_.fetch_add(1, acq_rel) + 1;
flush_requested_.store(true, release);
cv_.notify_one();
std::unique_lock lock(flush_completed_mu_);
flush_completed_cv_.wait(lock, [my_token] {            // 无 timeout
    return flush_completed_.load(acquire) >= my_token || !running_;
});
```

修复前，worker 完成 flush 时**未持锁**就 notify：

```cpp
// worker_loop（修复前）
flush_completed_.store(current_token, release);
flush_completed_cv_.notify_all();   // ← 未持有 flush_completed_mu_
```

致命窗口：flusher 已判定 predicate 为 false（`completed < my_token`）、尚未进入 futex 期间，worker 的 `notify_all` 正好飞过 —— 没人在 futex 里，通知**丢失**。flusher 随即进入 futex 永久阻塞，而该 `wait` **没有超时兜底**。

逐事件追踪抓到的现场（token 627）：

```
[F-in ] tk=627 (token=627 comp=626)   ← flusher 进入 wait（626>=627 false）
[W-done] comp=627 (flag was 1)         ← worker 已置 completed=627 并 notify_all（未持锁）
[W-beat] qsize=0 fr=0   …8 秒…         ← 再无 [F-out] tk=627，flusher 永久卡死
```

**指纹**：`flush_token_ == flush_completed_`（token 已完成）却仍卡在 `wait`。这看似自相矛盾——predicate 明明为真——但 `wait(lock, pred)` 只在**被唤醒或伪唤醒时**才重判 predicate；notify 已丢，永远不重判。worker 之所以之后 `fr=0`，是因为 flusher 卡死、发不出 #628。

> 为何 clang/CI 更易触发：纯粹是该窗口的时序敏感，CI 高负载让 worker 与 flusher 的调度更容易撞进这个窗口，非编译器 bug。

## 修复：持锁 notify 关闭窗口

`pthread_cond_wait` 的语义保证"**原子地释放用户锁 + 登记为等待者**"。因此，只要 notify 方**持有同一把锁**，waiter 要么还没进 `wait`（之后取锁、重判 predicate 为真即返回），要么已在 futex（被这次 notify 唤醒、重判为真返回）。两个分支都不会丢。

关键决策：

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| worker/stop 持 `flush_completed_mu_` 做 `store + notify_all` | 关闭丢失唤醒窗口，根治 | 给 `flush_completed_cv_.wait` 加超时：只是用轮询掩盖竞态（违背项目"no silent fallbacks"），且仍可能漏判 |
| notify 在锁内（而非解锁后） | `pthread_cond_wait` 原子解锁+登记语义要求 notify 持锁才能保证不丢；flush 是低频路径，唤醒后短暂等锁的开销可忽略 | 解锁后 notify：仍残留更小的丢唤醒窗口 |
| 不改 worker 唤醒路径（`cv_`/`wakeMu_`） | worker 用 `wait_for(10ms)` 有超时自愈，丢唤醒最多 10ms 延迟，非死锁 | 给 `submit` 热路径加 `wakeMu_` 锁竞争：得不偿失 |

代码见 [async_queue.cpp](../../desktop/base/logger/src/async_queue/async_queue.cpp) 的 `worker_loop()` 与 `stop()`。

## 次因：`MpscQueue::readPos_` 数据竞争

TSan 顺带抓到的独立缺陷：`readPos_` 原为**非原子** `size_type`（Vyukov 队列的单消费者私有位），却被生产者经 `size()` 读取（`submit()` 的 drop 策略用它）。worker 写、生产者读，无同步 → 数据竞争（UB）。x86-64 上对齐 8 字节不会撕裂，故不致死锁，只影响 drop 判定的准确性，但是真实的并发 UB。

修复：`readPos_` 改为 `std::atomic<size_type>`，消费者 `relaxed` 读 / `release` 推进，生产者 `size()`/`empty()` 用 `acquire` 读。代码见 [mpsc_queue.hpp](../../base/include/base/lockfree/mpsc_queue.hpp)。

## 验证

| 验证项 | 修复前 | 修复后 |
|--------|--------|--------|
| 独立探针（clang `-O2`）猛跑 | ~1/3 卡死 | 80 次零卡死 |
| TSan 探针 | 数据竞争 + 卡死 | **完全干净**，三次全到 `ALL DONE` |
| 项目 4 个 logger 测试 ×6 | — | 全通过 |
| `mpsc_queue_test` ×8 | — | 全通过 |
| clang `-Werror` 语法检查 | — | 通过 |

两处改动共 +45/−21 行，均在并发核心，附详细注释说明"为何必须持锁 notify"。

## 可复用的经验

- **罕见竞态别靠多跑**：先用 TSan 拿确定性的 happens-before 证据，再缩小范围。
- **绕过重依赖做最小探针**：能脱离 Qt/CMake 直接 `clang++` 编译的子系统，复现迭代快一个数量级。
- **看门狗 + gdb 父进程**是无 sudo/受限 ptrace 环境下抓"卡死现场全栈"的通用套路。
- **`condition_variable` notify 不持锁**是经典坑：带 predicate 的 `wait` 能挡住"notify 先于 wait 进入"的常见情况，但挡不住"notify 落在 pred 判定与 futex 进入之间"的窗口——只要 `wait` 无超时，这个窗口就是永久死锁。持锁 notify 是根治。
- **`token == completed` 却卡在 wait** 是丢失唤醒的典型指纹：值已更新，但唤醒已丢，predicate 不再被重判。
