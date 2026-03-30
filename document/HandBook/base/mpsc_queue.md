# MpscQueue - 无锁多生产者单消费者队列

`cf::lockfree::MpscQueue` 是一个固定容量的环形缓冲区无锁队列，专为多生产者单消费者（MPSC）场景设计。它的典型应用场景是高性能日志系统、事件分发和任务队列。

## 为什么需要 MPSC 队列

很多系统都有一个"收集-处理"的模式：多个线程产生数据，一个线程负责消费和处理。比如日志系统——多个工作线程往里写日志，一个专门的线程负责格式化和写入文件。如果用互斥锁保护队列，每次入队出队都要加锁解锁，在高并发下会成为瓶颈。

无锁队列通过原子操作替代互斥锁，避免了线程阻塞。生产者之间通过 `fetch_add` 原子操作协调写入位置，不需要互斥。消费者是单线程的，读取操作天然不需要同步。

## 基本用法

### 单个元素操作

```cpp
#include "base/lockfree/mpsc_queue.hpp"

cf::lockfree::MpscQueue<int, 1024> queue;

// 生产者线程（可以有多个）
queue.tryPush(42);
queue.tryPush(100);

// 消费者线程（只有一个）
int value;
if (queue.tryPop(value)) {
    std::cout << "Got: " << value << std::endl;  // Got: 42
}
```

### 批量操作

批量操作比逐个操作更高效，因为分摊了原子操作的开销：

```cpp
// 批量入队
std::vector<int> data = {1, 2, 3, 4, 5};
cf::span<int> data_span(data.data(), data.size());
size_t pushed = queue.tryPushBatch(data_span);

// 批量出队
int buffer[32];
size_t popped = queue.tryPopBatch(buffer, 32);
for (size_t i = 0; i < popped; ++i) {
    process(buffer[i]);
}
```

### 容量查询

```cpp
// 容量（编译期常量）
constexpr size_t cap = cf::lockfree::MpscQueue<int, 1024>::capacity();  // 1024

// 当前大小（近似值，并发环境下可能过时）
size_t approx_size = queue.size();

// 是否为空（近似值）
bool empty = queue.empty();
```

## 容量要求

容量必须是 2 的幂。这是因为队列内部使用 `pos & (Capacity - 1)` 来计算环形索引，位与运算比取模快得多。

```cpp
cf::lockfree::MpscQueue<int, 1024> q1;   // OK: 1024 = 2^10
cf::lockfree::MpscQueue<int, 2048> q2;   // OK: 2048 = 2^11
cf::lockfree::MpscQueue<int, 1000> q3;   // 编译错误：static_assert 失败
```

选择容量时应该考虑最坏情况下的生产速率和消费速率之差。如果生产者短暂 burst 产生了很多数据，队列需要有足够的缓冲空间。

## 实现原理

### 序列号机制

队列的每个槽位（Cell）都有一个序列号。序列号不是简单的索引，而是一个递增的"版本号"：

- 槽位可用条件：`sequence == pos`（生产者写入）
- 槽位可读条件：`sequence == pos + 1`（消费者读取）
- 消费后释放条件：`sequence = pos + Capacity`（标记为可用）

这个机制避免了 ABA 问题——两个不同的轮次不会被混淆。

### 生产者流程

1. `fetch_add` 原子递增写位置，获得自己的位置 `pos`
2. 自旋等待目标槽位的序列号等于 `pos`
3. 写入数据，设置序列号为 `pos + 1`

多生产者之间通过 `fetch_add` 保证了位置的独占性。如果某个生产者拿到位置后卡住了（还没写完），其他生产者会自旋等待那个槽位。

### 消费者流程

1. 读取读位置对应的槽位
2. 检查序列号是否等于 `readPos + 1`（有新数据）
3. 如果是，取出数据，设置序列号为 `readPos + Capacity`
4. 如果不是，队列为空

消费者只有一个线程，不需要原子操作。

### 自旋等待

当队列满时，`tryPush` 会自旋等待（busy-wait）。在 x86 上使用 `pause` 指令降低功耗：

```cpp
while (seq != pos) {
#if defined(__x86_64__) || defined(_M_X64)
    __builtin_ia32_pause();  // x86 pause
#else
    volatile int dummy = 0;  // 通用回退
#endif
    seq = cell->sequence.load(std::memory_order_acquire);
}
```

这意味着如果消费者跟不上，生产者线程会被阻塞在自旋中。如果你的场景可能出现持续的生产过剩，需要在上层做背压控制。

## 内存布局

```cpp
struct Cell {
    std::atomic<size_type> sequence;              // 序列号
    alignas(alignof(T)) unsigned char storage[sizeof(T)];  // 原始存储
};
```

每个槽位的存储是 `alignas(T)` 的原始字节数组，通过 placement new 构造对象。这样避免了不必要的默认构造，也支持没有默认构造函数的类型。

队列末尾有 padding 防止 false sharing：

```cpp
char padding_[64 - sizeof(readPos_) - sizeof(writePos_) - sizeof(buffer_) % 64];
```

## 线程安全

| 操作 | 线程安全 |
|------|---------|
| tryPush | 是，多生产者可并发调用 |
| tryPop | 否，仅单消费者可调用 |
| tryPushBatch | 是，多生产者可并发调用 |
| tryPopBatch | 否，仅单消费者可调用 |
| empty / size | 近似值，不保证一致性 |
| 析构 | 仅消费者线程应析构队列 |

### 内存序

- 生产者写入后使用 `release` 语义发布数据
- 消费者使用 `acquire` 语义读取数据
- `fetch_add` 使用 `relaxed` 语义（只需要原子递增，不需要排序）

这保证了生产者写入的数据在消费者读取时是可见的。

## 限制和注意事项

1. **容量固定**：编译期确定，不能动态调整。如果需要更大的队列，只能改模板参数重新编译。

2. **仅支持移动语义**：元素类型必须可移动构造和可移动赋值。入队操作会移动元素。

3. **不可复制**：队列本身不可复制也不可移动，通常作为全局或静态对象使用。

4. **生产者阻塞**：队列满时生产者会自旋，不会返回失败。如果需要非阻塞语义，需要在调用前检查 `size()`。

5. **析构必须在消费者线程**：队列析构时会 drain 剩余元素，这调用了 `tryPop`，所以析构必须和消费者在同一线程。

## 相关文档

- [Singleton - 单例模式](./singleton.md)
- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
