---
title: "CrashHandler 崩溃处理与自动重启 - 基建就绪度评估"
description: "评估日期：2026-03-30，整体基建就绪度约 60%，核心崩溃捕获可直接开始，完整功能（Cras"
---

# CrashHandler 崩溃处理与自动重启 - 基建就绪度评估

> 评估日期：2026-03-30

> **更新（2026-07-08）：Phase 1 已落地** 于 `feat/shell-foundation`（`base/crash_handler/`，lib `cfcrash` + early_session `CrashHandlerStage` + `test/crash/` 4 单测）。实施时对本评估做了三处订正：
> 1. **Logger `flush_sync()` 并非信号安全** — logger 是异步后台线程，signal handler 内调用即 UB。Phase 1 改为 **defer-to-finalize**：handler 只写 async-signal-safe 裸快照（`.pending`：pid/signal/裸地址），下次启动 `CrashHandlerStage` 时 tail logger 日志文件组装 `lastLogs`（参 breakpad/crashpad 同路）。
> 2. **落地路径不是 `desktop/base/infrastructure/`**（重组前结构），而是 `base/crash_handler/`（07 月 base 平铺重组后）。
> 3. **报告路径不是 `~/.cache/CFDesktop/crashes/`**，而是 `<exe_dir>/crashes/` — 跟 logger 同根（`app_runtime_dir() == QCoreApplication::applicationDirPath()`），finalize tail 日志与报告同目录树。
> 详见 [06_infrastructure.md](06_infrastructure.md) CrashHandler 段（Phase 1 完成项已勾选，Phase 2 项保留）。

## 结论：部分就绪，建议分阶段实施

整体**可复用相邻基建**就绪度约 **60%**（指 Logger/ConfigStore/Platform/System/InitChain/ScopeGuard 等可复用底座）；**崩溃捕获功能本身为 0%**。Phase 1（信号捕获 + backtrace + JSON 落盘，零依赖）可直接开始；Phase 2（CrashReporter 独立进程 + Watchdog）依赖 IPC 基建（0%，见 [06_infrastructure.md](06_infrastructure.md)），须与 IPC 同批落地。

---

## 已完成的基建（可直接复用）

| 组件 | 状态 | 复用点 |
|------|------|--------|
| **Logger** | 100% | 异步 MPSC 队列，`flush_sync()` 支持信号安全 flush，可抓取崩溃前最后 50 条日志 |
| **ConfigStore** | 100% | 4 层存储，可存储崩溃处理器配置（开关、最大报告数等） |
| **Platform 抽象** | 100% | 清晰的 linux_wsl / windows 分层，工厂模式，新加平台特定代码路径很自然 |
| **System 检测** | 100% | CPU/GPU/Memory/Network 信息可直接填入 CrashReport |
| **Init Chain** | 100% | DAG 初始化链，CrashHandler 可作为 early_session stage 注册 |
| **ScopeGuard** | 100% | RAII 工具，可用于信号处理器中的资源清理 |

## 未完成的基建（阻塞项）

| 组件 | 状态 | 影响范围 |
|------|------|----------|
| **IPC** | 0% | 阻塞 Phase 2：CrashReporter 独立进程通信、Watchdog 心跳都需要 IPC |
| **HWTier** | ✅ 已完成 | systemInfo 字段可直接取（`base/system/hardware_tier/`）；仅 CapabilityPolicy 策略引擎延后，不影响崩溃报告 |

## 现有信号处理代码

- `desktop/main/early_session/impl/console_signal_stage.cpp` — 仅处理 Windows Ctrl+C/Ctrl+Break，走 `QApplication::quit()` 优雅退出
- 无 SIGSEGV/SIGABRT 处理
- 无栈回溯捕获
- 无崩溃报告生成

---

## 分阶段实施策略

### Phase 1 — 崩溃捕获核心（无外部依赖，立即可做）

预计工期：~3 天

1. **CrashHandler 类设计**
   - Singleton 模式，注册到 early_session init chain
   - Linux：`sigaction()` 捕获 SIGSEGV / SIGABRT / SIGFPE / SIGBUS / SIGILL
   - Windows：`SetUnhandledExceptionFilter()` + `std::set_terminate`

2. **栈回溯**
   - Linux：`backtrace()` / `backtrace_symbols()`，可选 addr2line 符号解析
   - Windows：`StackWalk64`

3. **CrashReport 结构体 + 存储**
   - 字段：timestamp, processName, processId, signal, stackTrace, lastLogs, systemInfo
   - JSON 序列化，存储路径 `~/.cache/CFDesktop/crashes/`
   - 崩溃历史管理：最多保留 20 份报告，30 天过期清理

4. **集成到 init chain**
   - 作为 early_session stage 在应用启动最早期注册
   - 参考 `console_signal_stage.cpp` 的注册方式

### Phase 2 — CrashReporter + Watchdog（依赖 IPC 基建）

预计工期：~2 天（IPC 完成后）

1. **CrashReporter 独立进程**
   - 轻量 Qt 程序，主进程崩溃后启动
   - 崩溃通知 UI（友好错误信息、崩溃时间/次数、操作按钮）
   - 详情查看（栈回溯、系统信息、复制到剪贴板）

2. **Watchdog 守护进程**
   - 监控主进程健康状态，心跳检测
   - 崩溃后自动重启，指数退避策略
   - 连续 3 次崩溃停止自动重启，仅显示报告

---

## 关键实现文件路径

| 用途 | 路径 |
|------|------|
| CrashHandler 接口 | `desktop/base/infrastructure/` (新建) |
| Linux 平台实现 | `desktop/base/infrastructure/platform/linux/` (新建) |
| Windows 平台实现 | `desktop/base/infrastructure/platform/windows/` (新建) |
| 注册入口 | `desktop/main/early_session/` (已有 init chain) |
| 现有信号处理参考 | `desktop/main/early_session/impl/console_signal_stage.cpp` |
| CrashReporter 进程 | `tools/crash_reporter/` (新建，Phase 2) |

## 验证方式

**Phase 1 验证：**
1. 单元测试：`raise(SIGSEGV)` / `raise(SIGABRT)` 触发崩溃，验证报告生成
2. 集成测试：在 desktop boot 过程中注入崩溃，验证 init chain 集成
3. 手动验证：运行 CFDesktop，`kill -SIGSEGV <pid>`，检查 `~/.cache/CFDesktop/crashes/` 输出

**Phase 2 验证：**
1. 模拟主进程崩溃，验证 CrashReporter 自动弹出
2. 测试 Watchdog 自动重启 + 指数退避策略
3. 测试连续崩溃后停止自动重启的行为
