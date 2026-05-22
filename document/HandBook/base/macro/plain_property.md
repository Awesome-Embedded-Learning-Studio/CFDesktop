---
title: "plainproperty - 简单属性宏"
description: 是一个用来自动生成 getter 和 setter 方法的宏，减少手写样板代码的重复劳动。我们经常需
---

# plain_property - 简单属性宏

`CF_PLAIN_PROPERTY` 是一个用来自动生成 getter 和 setter 方法的宏，减少手写样板代码的重复劳动。我们经常需要一些简单的数据持有类，它们只包含一些带默认值的成员变量和对应的访问器——这种代码写多了既无聊又容易出错，用宏来生成是更务实的选择。

## 基本用法

在类中使用 `CF_PLAIN_PROPERTY` 宏时，它会展开为一个私有成员变量和三个公共方法：

```cpp
#include "base/macro/plain_property.h"

class WidgetConfig {
    // 生成: int width{800};
    //       int& get_width();
    //       const int& get_width_const() const;
    //       void set_width(const int&);
    CF_PLAIN_PROPERTY(int, width, 800)

    // 生成: std::string title{"Untitled"};
    //       ...
    CF_PLAIN_PROPERTY(std::string, title, "Untitled")

public:
    // 其他成员方法...
};
```text

## 宏展开结果

`CF_PLAIN_PROPERTY(val_type, val_name, default_value)` 展开后的代码如下：

```cpp
public:
    val_type& get_##val_name() {
        return val_name;
    }
    const val_type& get_##val_name##_const() const {
        return val_name;
    }
    void set_##val_name(const val_type& v) {
        val_name = v;
    }

private:
    val_type val_name{default_value};
```text

注意成员变量直接在 `private` 区域声明，而访问器在 `public` 区域。这是因为宏本身包含了访问修饰符，所以使用时不需要额外考虑这些。

## 使用示例

```cpp
#include "base/macro/plain_property.h"
#include <string>
#include <iostream>

class AppConfig {
    CF_PLAIN_PROPERTY(int, max_connections, 10)
    CF_PLAIN_PROPERTY(std::string, host, "localhost")
    CF_PLAIN_PROPERTY(bool, debug_mode, false)
};

int main() {
    AppConfig config;

    // 使用 setter 修改值
    config.set_max_connections(100);
    config.set_host("192.168.1.1");
    config.set_debug_mode(true);

    // 使用 getter 读取值
    std::cout << "Host: " << config.get_host() << std::endl;
    std::cout << "Max connections: " << config.get_max_connections() << std::endl;

    // const getter 可以在 const 上下文调用
    const AppConfig& const_config = config;
    std::cout << "Debug: " << std::boolalpha << (const_config.get_debug_mode_const()) << std::endl;

    return 0;
}
```text

## 方法命名

宏生成的三个方法遵循特定命名规则：

- `get_<name>()`：返回非 const 引用，允许修改
- `get_<name>_const()`：返回 const 引用，用于只读访问
- `set_<name>(const T&)`：设置新值

两个 getter 的区别在于 const 正确性。非 const 对象可以调用任何一个，const 对象只能调用 `_const` 版本。这个设计虽然有点冗余，但避免了在模板代码里处理 const 重载的麻烦。

## 设计动机

选择用宏而不是 C++ 的属性语法（如 `property` 关键字）是因为后者不是标准 C++ 的一部分，只有 MSVC 支持非标准的扩展。用宏虽然可读性稍差，但能保证跨编译器兼容。

另一个考虑是代码生成的一致性。手写 getter/setter 时容易漏掉 const 版本，或者命名不统一。宏强制了一种固定模式，减少了人为错误的可能性。

## 注意事项

⚠️ setter 使用 const 引用传参，对于基础类型（int、bool 等）来说不是最高效的选择。如果你对性能非常敏感，或者这些属性在热路径上被频繁修改，可能需要手写专门的版本。

⚠️ 宏展开会插入访问修饰符，如果在类的中间使用，会改变后续成员的访问级别。建议把所有 `CF_PLAIN_PROPERTY` 调用集中在类定义的开头或结尾，避免和手写的 public/private 区域混在一起：

```cpp
// 好的做法：宏集中使用
class Good {
    CF_PLAIN_PROPERTY(int, x, 0)
    CF_PLAIN_PROPERTY(int, y, 0)

public:
    void method();

private:
    int internal_;
};

// 不好的做法：宏和手写访问修饰符混用
class Bad {
public:
    void method();
    CF_PLAIN_PROPERTY(int, x, 0)  // 会插入 public:/private:，破坏原有结构
};
```text

## 适用场景

`CF_PLAIN_PROPERTY` 适合用于简单的配置类、数据传输对象（DTO）或状态持有类。如果属性需要额外的验证逻辑、触发通知或线程安全保证，还是手写实现更合适。

## 相关文档

- [system_judge - 系统判断宏](./system_judge.md)
- [macros - 宏定义系统](../macros.md)
