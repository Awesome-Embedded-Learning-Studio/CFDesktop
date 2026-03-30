# hash (FNV-1a) - 编译期哈希

`cf::hash` 命名空间提供了 FNV-1a（Fowler-Noll-Vo 1a）哈希算法的 `constexpr` 实现，支持在编译期计算字符串的哈希值。FNV-1a 以其实现简单、分布均匀和极低的碰撞率而闻名，非常适合用于字符串标识符的快速比较。

## 为什么需要编译期哈希

在运行时用 `std::string` 做键查找需要字符串比较，开销不小。如果能把字符串在编译期转换成整数哈希值，运行时就只需要做整数比较——这在高性能场景下（比如消息分发、事件系统、Token 解析）非常有用。

```cpp
// 运行时字符串比较：O(n) 字符逐个比较
if (token == "START") { /* ... */ }
if (token == "STOP")  { /* ... */ }

// 编译期哈希 + 运行时整数比较：O(1)
using namespace cf::hash;
switch (fnv1a64(token)) {
    case fnv1a64("START"): /* ... */ break;
    case fnv1a64("STOP"):  /* ... */ break;
}
```

编译器会在编译期算出 `fnv1a64("START")` 的值，switch 语句变成了一组整数比较，非常高效。

## 基本用法

### 64 位哈希

```cpp
#include "base/hash/constexpr_fnv1a.hpp"

using namespace cf::hash;

// 编译期计算
constexpr uint64_t h1 = fnv1a64("HelloWorld");
constexpr uint64_t h2 = fnv1a64("GoodbyeWorld");
static_assert(h1 != h2, "Different strings must produce different hashes");

// 运行时使用 std::string_view
std::string_view sv = "some_string";
uint64_t h3 = fnv1a64(sv);
```

### 32 位哈希

```cpp
// 编译期
constexpr uint32_t h32 = fnv1a32("TokenA");

// 运行时
uint32_t h = fnv1a32(std::string_view("dynamic"));
```

32 位版本适用于内存受限的场景，但碰撞概率比 64 位高。如果哈希表不大（几千个条目以内），32 位通常够用。

### 自定义种子

```cpp
// 使用自定义种子，用于哈希表的不同桶或其他需要不同分布的场景
constexpr uint64_t h1 = fnv1a64("data");                        // 默认种子
constexpr uint64_t h2 = fnv1a64("data", 12345678ULL);           // 自定义种子
// h1 != h2，相同输入不同种子产生不同哈希
```

### 用户自定义字面量

```cpp
using namespace cf::hash;

// "_hash" 后缀，编译期计算
constexpr uint64_t h = "MyToken"_hash;
static_assert(h == fnv1a64("MyToken"), "UDL must match function call");
```

`_hash` 字面量让代码更简洁，特别是在 switch-case 里：

```cpp
std::string_view cmd = read_command();
switch (fnv1a64(cmd)) {
    case "open"_hash:   open_file();   break;
    case "close"_hash:  close_file();  break;
    case "save"_hash:   save_file();   break;
    default:            unknown_cmd(); break;
}
```

## FNV-1a 算法

FNV-1a 的核心逻辑极其简单：

```
hash = offset_basis
for each byte in input:
    hash = hash XOR byte
    hash = hash * prime
return hash
```

参数值：

| 参数 | 32 位 | 64 位 |
|------|--------|--------|
| offset_basis | 2166136261 | 14695981039346656037 |
| prime | 16777619 | 1099511628211 |

FNV-1a 和 FNV-1 的区别在于 XOR 和乘法的顺序：FNV-1a 先 XOR 再乘，而 FNV-1 先乘再 XOR。1a 变体对于短字符串的分布更好，这也是我们选择它的原因。

## 性能考虑

FNV-1a 是纯整数运算，每次迭代只有一次 XOR 和一次乘法。对于 N 字符的字符串：

- 时间复杂度：O(N)
- 空间复杂度：O(1)
- 无内存分配
- 无分支（除了循环条件）

在 x86-64 上，64 位 FNV-1a 处理速度约为 1-2 字节/时钟周期。对于大多数标识符字符串（几十字节以内），哈希计算时间可以忽略不计。

编译期计算则完全免费——结果会被编译器内联为常量。

## 碰撞处理

FNV-1a 虽然碰撞率很低，但毕竟是非加密哈希，碰撞是存在的。实际使用中需要注意：

1. **编译期常量之间不会碰撞**：如果你只用 `static_assert` 检查过的常量做 switch-case，碰撞会在编译期被发现。

2. **运行时输入需要二次验证**：如果用哈希值做查找键，当哈希匹配时，应该再用字符串比较确认：

```cpp
auto it = hashmap.find(fnv1a64(key));
if (it != hashmap.end()) {
    // 哈希匹配，但可能是碰撞
    if (it->second.actual_key == key) {
        // 确认是真正的匹配
    }
}
```

3. **不要用于安全场景**：FNV-1a 不是加密哈希，不应该用于密码存储、完整性校验等安全场景。

## 线程安全

所有 `fnv1a64` 和 `fnv1a32` 函数都是纯函数（无副作用），完全线程安全。`constexpr` 常量在编译期求值，不存在并发问题。

## 相关文档

- [expected - 错误处理](./expected.md)
- [基础工具类概述](./overview.md)
