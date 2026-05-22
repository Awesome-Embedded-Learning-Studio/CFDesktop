---
title: Token 系统设计——字符串字面量的编译时魔法
description: 在上一篇文章里，我们讲了主题系统的整体架构：ICFTheme 接口定义了主题包含什么，ThemeFa
---

# Token 系统设计——字符串字面量的编译时魔法

在上一篇文章里，我们讲了主题系统的整体架构：ICFTheme 接口定义了主题包含什么，ThemeFactory 负责创建主题，ThemeManager 管理多个主题并处理切换。

但有一个问题我们没深入聊：控件怎么访问主题里的数据？直接用字符串查询吗？比如 `theme->getColor("md.primary")`？

这方案能用，但有几个问题：字符串拼接容易出错、编译期无法检查、运行时查找有性能开销。Material Design 3 有 26 个标准颜色角色，还有各种字体、圆角、动画参数，如果全靠字符串查询，代码里会到处是"魔法字符串"。

于是我们设计了 Token 系统。

## Token 系统的动机

Token 系统的核心思想是：把"字符串标识符"和"编译时类型安全"结合起来。

理想情况下，我们希望这样写：

```cpp
// 定义一个 token
using PrimaryToken = StaticToken<QColor, "md.primary"_hash>;

// 注册 token
TokenRegistry::get().register_token<PrimaryToken>(QColor("#6200EE"));

// 使用 token
auto result = PrimaryToken::get();
if (result) {
    QColor color = *result;
}
```text

这样有几个好处：

1. **类型安全**：PrimaryToken 明确关联了 QColor 类型，编译器会检查
2. **零运行时开销**：Hash 是编译时常量，查找用 `unordered_map`
3. **可重构**：如果 token 名字改了，编译器会报错（虽然要手动改 hash）

## StaticToken：编译时类型安全

StaticToken 是一个模板类，它的两个模板参数分别是值类型 `T` 和名字的哈希值 `Hash`：

```cpp
template <typename T, uint64_t Hash> class StaticToken {
public:
    using value_type = T;
    static constexpr uint64_t hash_value = Hash;

    // 删除所有构造函数——这只是一个类型封装，不能实例化
    StaticToken() = delete;
    StaticToken(const StaticToken&) = delete;

    // 静态方法获取值
    static cf::expected<T*, TokenError> get();
};
```text

注意这里的设计技巧：StaticToken 的所有构造函数都被删除了。这意味着你不能创建 StaticToken 的实例——它只是一个"类型"，用来承载编译时信息。

实际使用时，你用的是 `StaticToken<QColor, Hash>::get()` 这个静态方法。

## constexpr 哈希

为了在编译时计算字符串的哈希，我们用了 FNV-1a 64 位哈希算法，它可以写成 `constexpr` 函数：

```cpp
constexpr uint64_t fnv1a64(std::string_view str) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

// 用户定义字面量（可选）
constexpr uint64_t operator""_hash(const char* str, size_t len) {
    return fnv1a64(std::string_view(str, len));
}
```text

这样你就可以在编译时计算字符串的哈希：

```cpp
constexpr uint64_t PRIMARY_HASH = fnv1a64("md.primary");  // 编译时常量
using PrimaryToken = StaticToken<QColor, PRIMARY_HASH>;
```text

## TokenRegistry：运行时存储

StaticToken 只是一个"类型"，真正的数据存在 TokenRegistry 里：

```cpp
class TokenRegistry {
public:
    static TokenRegistry& get();

    // 注册静态 token
    template <typename TokenToken, typename... Args>
    Result<void> register_token(Args&&... args);

    // 获取静态 token
    template <typename TokenToken>
    Result<typename TokenToken::value_type*> get();

    // 动态 token（运行时字符串）
    template <typename T>
    Result<void> register_dynamic(std::string_view name, Args&&... args);

    template <typename T>
    Result<T*> get_dynamic(std::string_view name);

private:
    mutable std::shared_mutex registry_mutex_;
    std::unordered_map<uint64_t, detail::TokenSlot> slot_map_;
};
```text

TokenRegistry 是一个单例，内部用 `unordered_map<uint64_t, TokenSlot>` 存储数据，键是哈希值。

## 类型擦除与 std::any

TokenSlot 使用 `std::any` 进行类型擦除：

```cpp
struct TokenSlot {
    std::unique_ptr<std::any> data;  // 类型擦除的值
    const std::type_info* type_info; // 用于类型检查
    std::string name;                // 调试用
};
```text

注册时，我们创建一个 `std::any` 存储 `T` 类型的值，同时保存 `typeid(T)` 用于后续的类型检查。

获取时，我们先检查 `type_info` 是否匹配，然后用 `std::any_cast` 提取值：

```cpp
template <typename TokenToken>
auto TokenRegistry::get() -> Result<typename TokenToken::value_type*> {
    using T = typename TokenToken::value_type;
    constexpr uint64_t hash = TokenToken::hash_value;

    std::shared_lock<std::shared_mutex> lock(registry_mutex_);

    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(TokenError{TokenError::Kind::NotFound, "..."});
    }

    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(TokenError{TokenError::Kind::TypeMismatch, "..."});
    }

    return std::any_cast<T>(slot->data.get());
}
```text

## 线程安全设计

TokenRegistry 使用 `std::shared_mutex` 来支持多读单写：

```cpp
// 读操作（get）：共享锁
std::shared_lock<std::shared_mutex> lock(registry_mutex_);

// 写操作（register_token）：独占锁
std::unique_lock<std::shared_mutex> lock(registry_mutex_);
```text

这个设计适合"读多写少"的场景——token 注册通常发生在初始化阶段，之后大部分时候都是读取。

`shared_mutex` 的好处是：多个读操作可以并发进行，只有在写操作时才需要独占访问。

## constexpr 字面量定义

Material Design 3 的 26 个颜色 Token 被定义为 `constexpr` 字面量：

```cpp
namespace cf::ui::core::token::literals {

inline constexpr const char PRIMARY[] = "md.primary";
inline constexpr const char ON_PRIMARY[] = "md.onPrimary";
inline constexpr const char PRIMARY_CONTAINER[] = "md.primaryContainer";
// ... 共 26 个

inline constexpr const char* const ALL_TOKENS[] = {
    PRIMARY, ON_PRIMARY, PRIMARY_CONTAINER, ON_PRIMARY_CONTAINER,
    // ...
};
inline constexpr size_t TOKEN_COUNT = 26;

} // namespace cf::ui::core::token::literals
```text

这些 `constexpr` 字面量可以在编译时使用，而且避免了字符串字面量的重复。

## ICFColorScheme 的 Token 访问

ICFColorScheme 接口的 `queryExpectedColor` 方法就是用 Token 系统实现的：

```cpp
struct ICFColorScheme {
    virtual QColor& queryExpectedColor(const char* name) = 0;

    QColor queryColor(const char* name) const {
        return const_cast<ICFColorScheme*>(this)->queryExpectedColor(name);
    }
};
```text

MaterialColorScheme 的实现内部会用 TokenRegistry 来查找颜色。虽然这里还是用了字符串参数，但内部实现可以复用 Token 系统，保持一致性。

## DynamicToken：运行时类型擦除

除了 StaticToken，我们还支持 DynamicToken——完全运行时的 token：

```cpp
// 注册
TokenRegistry::get().register_dynamic<QColor>("custom.color", QColor("#FF0000"));

// 使用
auto result = TokenRegistry::get().get_dynamic<QColor>("custom.color");
if (result) {
    QColor color = *result;
}
```yaml

DynamicToken 用的是运行时字符串查找，没有编译时检查，但更加灵活。适合插件系统、用户自定义主题等场景。

## 三层 Token 结构

在完整的 Material Design 3 体系中，Token 分为三层：

1. **Reference Token**：Material Design 原始规范的定义，如 `md.primary`
2. **System Token**：内部系统级 Token，如 `sys.color.primary`
3. **Component Token**：组件级 Token，如 `button.filled.container`

这种分层让系统更加灵活：组件可以定义自己的 Token，而系统 Token 可以被多个组件复用。

## 总结

Token 系统是一个"编译时类型安全 + 运行时灵活查找"的混合方案。StaticToken 提供零开销的编译时检查，DynamicToken 提供运行时灵活性，两者共享同一个 TokenRegistry 存储。

有了 Token 系统，我们就可以规范地访问主题数据了。但 Token 只是"标识符"，实际的值怎么来？如何从一个种子颜色生成完整的 Material 主题？

接下来，我们进入颜色方案的具体实现。

---

**相关文档**

- [主题系统架构设计](./01-theme-system-design.md)——主题系统的整体设计
- [颜色方案实现](./03-color-scheme.md)——从种子颜色到完整主题
- [HCT 色彩空间实战](../layer-1-math-utility/02-color-system-hct.md)——Layer 1 的颜色数学基础
