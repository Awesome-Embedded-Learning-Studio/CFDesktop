# span - 容器视图

## 简介

`span<T>` 是 C++20 中引入的容器视图类，CFDesktop 在 C++17 环境下提供了兼容实现。`span` 提供了对连续序列的非拥有视图，可以避免不必要的内存拷贝。

## 头文件

```cpp
#include "base/span/span.h"
```

## 特性

- **零开销**：不拥有数据，只持有指针和大小
- **通用性**：支持 C 数组、std::vector、std::array
- **安全**：提供边界检查的方法
- **便捷**：支持子视图操作

## 基本用法

### 构造函数

```cpp
// 从指针和大小构造
int arr[] = {1, 2, 3, 4, 5};
cf::span<int> s1(arr, 5);

// 从 C 数组构造（自动推导大小）
cf::span<int> s2 = arr;

// 从 std::vector 构造
std::vector<int> vec = {1, 2, 3};
cf::span<int> s3 = vec;

// 从 std::array 构造
std::array<int, 4> arr2 = {1, 2, 3, 4};
cf::span<int> s4 = arr2;
```

### 元素访问

```cpp
cf::span<int> s = /* ... */;

// 使用 operator[]
int first = s[0];
int last = s[s.size() - 1];

// 使用 front/back
int first2 = s.front();
int last2 = s.back();

// 使用 data()
int* ptr = s.data();
```

### 子视图操作

```cpp
cf::span<int> s = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 前 3 个元素
auto first_three = s.first(3);  // {1, 2, 3}

// 后 3 个元素
auto last_three = s.last(3);    // {8, 9, 10}

// 中间的子视图
auto middle = s.subspan(2, 4);  // {3, 4, 5, 6}

// 从位置 2 到末尾
auto from_two = s.subspan(2);   // {3, 4, 5, 6, 7, 8, 9, 10}
```

## 使用场景

### 1. 函数参数

使用 `span` 可以避免函数参数的拷贝，同时保持灵活性：

```cpp
// 传统方式：只能接受 vector
void process1(const std::vector<int>& data);

// 使用 span：可以接受任何容器
void process2(cf::span<const int> data);

// 调用
std::vector<int> vec = {1, 2, 3};
std::array<int, 3> arr = {1, 2, 3};
int c_arr[] = {1, 2, 3};

process2(vec);   // OK
process2(arr);   // OK
process2(c_arr); // OK
```

### 2. 避免拷贝

```cpp
std::vector<std::string> strings = {"hello", "world"};

// 创建 string_view 的 span
cf::span<const std::string> view = strings;

// 或者创建 string_view 的 span（需要额外转换）
```

### 3. 切片操作

```cpp
std::vector<int> data(100);

// 处理前半部分
cf::span<int> first_half = cf::span(data).first(50);
process_chunk(first_half);

// 处理后半部分
cf::span<int> second_half = cf::span(data).subspan(50);
process_chunk(second_half);
```

## 注意事项

1. **生命周期**：`span` 只是视图，不拥有数据。确保底层数据在 `span` 使用期间保持有效。

2. **边界检查**：`operator[]` 不进行边界检查，使用时需要确保索引有效。

3. **const 正确性**：使用 `span<const T>` 来表示只读视图。

## 完整示例

```cpp
#include "base/span/span.h"
#include <iostream>
#include <vector>

void print_span(cf::span<const int> s) {
    for (auto val : s) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    cf::span<const int> s = vec;
    std::cout << "完整: ";
    print_span(s);

    std::cout << "前3个: ";
    print_span(s.first(3));

    std::cout << "后2个: ";
    print_span(s.last(2));

    return 0;
}
```

## 相关文档

- [expected - 错误处理](./expected.md)
- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
