---
title: "span - 容器视图"
description: 是 C++20 引入的容器视图类，提供对连续序列的非拥有访问。我们提供了一份 backport 实现
---

# span - 容器视图

`span<T>` 是 C++20 引入的容器视图类，提供对连续序列的非拥有访问。我们提供了一份 backport 实现，确保在所有目标平台上可用。核心思想很简单——只持有指针和长度，不管理数据生命周期——这使得 `span` 可以零拷贝地"切分"任何连续容器。

## 为什么需要 span

考虑一个函数需要接收整型数据的场景：

```cpp
// 只能接受 vector
void process(const std::vector<int>& data);

// 只能接受 C 数组（但会退化成指针，丢失长度）
void process(int* data, size_t size);
```text

第一种限制了调用方必须用 `vector`，第二种需要手动传长度而且容易出错。用 `span` 就没有这些问题：

```cpp
// 可以接受任何容器
void process(cf::span<const int> data);

std::vector<int> vec = {1, 2, 3};
std::array<int, 3> arr = {1, 2, 3};
int c_arr[] = {1, 2, 3};

process(vec);   // OK
process(arr);   // OK
process(c_arr); // OK
```text

## 构造方式

`span` 可以从任何连续容器构造，编译器会自动推导大小：

```cpp
#include "base/span/span.h"

// 从 C 数组构造
int arr[] = {1, 2, 3, 4, 5};
cf::span<int> s1 = arr;  // 自动推导长度

// 从 vector 构造
std::vector<int> vec = {1, 2, 3};
cf::span<int> s2 = vec;

// 从 array 构造
std::array<int, 4> arr2 = {1, 2, 3, 4};
cf::span<int> s3 = arr2;

// 手动指定指针和长度
cf::span<int> s4(vec.data(), vec.size());
```text

## 元素访问

访问元素的方式和标准容器一样，支持 `operator[]`、`front()`、`back()`：

```cpp
cf::span<int> s = /* ... */;

int first = s[0];           // 下标访问
int first2 = s.front();     // 首元素
int last = s.back();        // 末元素
int* ptr = s.data();        // 底层指针
```text

⚠️ `operator[]` 不做边界检查，越界访问是未定义行为。如果需要安全检查，标准库提供了 `at()` 方法，但我们的实现里为了性能省略了。

## 切片操作

`span` 最强大的功能是切片——可以轻松获取子视图而不拷贝数据：

```cpp
cf::span<int> s = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 前三个元素
auto first_three = s.first(3);   // {1, 2, 3}

// 后三个元素
auto last_three = s.last(3);     // {8, 9, 10}

// 中间的切片
auto middle = s.subspan(2, 4);   // {3, 4, 5, 6}

// 从位置 2 到末尾
auto tail = s.subspan(2);        // {3, 4, 5, 6, 7, 8, 9, 10}
```text

切片返回的新 `span` 仍指向原始数据，只是起始位置和长度不同。这意味着切片操作是 O(1) 的，没有任何拷贝开销。

## 函数参数

用 `span` 做函数参数是最常见的使用场景，特别是处理二进制数据或网络协议时：

```cpp
// 写一个处理网络包的函数
void process_packet(cf::span<const uint8_t> packet) {
    if (packet.size() < 4) return;  // 包头至少 4 字节

    auto header = packet.first(4);
    auto payload = packet.subspan(4);

    // 处理...
}

// 调用方可以传入任何缓冲区
std::vector<uint8_t> buffer = read_from_socket();
process_packet(buffer);

uint8_t stack_buf[256];
size_t received = recv(sock, stack_buf, 256, 0);
process_packet(cf::span<uint8_t>(stack_buf, received));
```text

## const 正确性

`span<const T>` 表示只读视图，`span<T>` 表示可写视图。选择正确的类型可以避免意外修改：

```cpp
void read_only(cf::span<const int> data);
void read_write(cf::span<int> data);

const std::vector<int> vec = {1, 2, 3};

read_only(vec);   // OK
read_write(vec);  // 编译错误
```text

## 生命周期陷阱

`span` 不拥有数据，只是一个"窗口"。如果底层数据被销毁，`span` 就会变成悬空引用：

```cpp
cf::span<int> get_bad_span() {
    std::vector<int> vec = {1, 2, 3};
    return vec;  // 警告：vec 会被销毁，返回的 span 悬空
}

// 正确的做法
cf::span<const int> get_good_span(const std::vector<int>& vec) {
    return vec;  // OK，调用方保证 vec 有效
}
```text

这个坑在异步代码里特别容易出现——如果在一个线程里创建 `span`，另一个线程里使用，必须确保原始数据的生命周期足够长。

## 相关文档

- [expected - 错误处理](./expected.md)
- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
