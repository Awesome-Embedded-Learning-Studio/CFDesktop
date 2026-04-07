# 基础工具库

基础工具库是 CFDesktop 最底层的模块，提供了一套零 Qt 依赖的跨平台工具。选择把这些工具独立出来，是因为上层 UI 框架可能需要在不同的上下文中复用——比如单元测试里不想链接整个 Qt 库，或者某些底层模块只需要一个轻量的错误处理机制。

这些工具大多是标准库特性的"降级实现"或"便利封装"。项目基于 C++17，但我们需要 C++20 的 `std::span` 和 C++23 的 `std::expected`，所以自己实现了一份。另外也有一些实战中总结的实用工具，比如 RAII 风格的资源管理。

## 错误处理

`expected<T, E>` 是函数式错误处理的模板，用返回值显式表示可能的失败，而不是靠异常。选择这个方案主要考虑嵌入式环境通常禁用异常，而且显式的错误类型让调用者更清楚怎么处理。

```cpp
#include "base/expected/expected.hpp"

enum class ErrorCode { InvalidInput, NotFound };

cf::expected<int, ErrorCode> parseNumber(std::string str) {
    if (str.empty()) {
        return cf::unexpected(ErrorCode::InvalidInput);
    }
    return std::stoi(str);
}

// 使用时
auto result = parseNumber("42");
if (result.has_value()) {
    std::cout << "值: " << result.value() << std::endl;
} else {
    // 根据 error() 的值决定怎么恢复
}
```

## 容器视图

`span<T>` 提供对连续序列的非拥有视图，避免不必要的拷贝。函数参数用 `span` 替代 `const vector&`，调用方可以用数组、`vector`、`array` 等任何容器传入，灵活性高很多。

```cpp
#include "base/span/span.h"

void process(cf::span<const int> data);

std::vector<int> vec = {1, 2, 3};
int arr[] = {1, 2, 3};

process(vec);  // OK
process(arr);  // 也 OK
```

## 资源管理

`ScopeGuard` 确保 RAII 风格的清理操作在作用域结束时执行，即使中途抛出异常。这在管理 C 接口资源时特别有用——比如打开的文件句柄、分配的内存，或者需要手动释放的 COM 对象。

```cpp
#include "base/scope_guard/scope_guard.hpp"

{
    FILE* f = fopen("data.txt", "r");
    cf::ScopeGuard guard([&f]() { fclose(f); });

    // 使用文件，无论中间发生什么，离开作用域都会自动关闭
}
```

## 懒加载初始化

`CallOnceInit<T>` 是线程安全的懒加载模板，用 `std::call_once` 确保只初始化一次。我们用它来缓存系统信息——CPU 型号、内存大小这些东西不会变，每次都查太浪费。

```cpp
#include "base/helpers/once_init.hpp"

class CPUInfoCache : public cf::CallOnceInit<CPUInfo> {
protected:
    bool init_resources() override {
        resource.model = queryModel();
        resource.arch = queryArch();
        return true;
    }
};

// 首次调用 get_resources() 时才执行初始化
auto& info = cache.get_resources();
```

⚠️ `force_reinit()` 不是线程安全的，如果需要在运行时重新初始化，记得自己加锁。

## Linux 文件解析

`proc_parser` 提供了一套解析 `/proc` 和 `/sys` 伪文件系统的工具。Linux 下查硬件信息基本都要读这些文件，格式是固定的但处理起来很繁琐。这套工具用 `string_view` 避免拷贝，而且不抛异常。

```cpp
#include "base/linux/proc_parser.h"

// 从 cpuinfo 行中提取字段
std::string_view model = cf::parse_cpuinfo_field(line, "model name");

// 直接读单个数字值的文件
auto freq = cf::read_uint32_file("/sys/devices/system/cpu/cpu0/cpufreq/max_freq");
```

## 弱引用

`WeakPtr<T>` 是一套非拥有的弱引用机制，和 `std::weak_ptr` 有本质区别。假设对象有唯一的拥有者，弱引用只是一个"取票凭证"——拥有者销毁后，所有凭证自动失效。配合 `WeakPtrFactory<T>` 使用，为对象提供弱引用支持。

```cpp
#include "base/weak_ptr/weak_ptr_factory.h"

class ThemeManager {
private:
    cf::WeakPtrFactory<ThemeManager> weak_factory_{this};  // 最后一个成员

public:
    cf::WeakPtr<ThemeManager> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }
};

// 使用方
auto weak = manager.GetWeakPtr();
if (weak) {
    weak->ApplyTheme();  // 安全访问
}
```

## 平台检测

宏定义系统提供编译时的平台和架构检测。`CFDESKTOP_OS_WINDOWS`、`CFDESKTOP_OS_LINUX` 等宏在预处理阶段就知道目标平台，实现条件编译。

```cpp
#include "base/macros.h"

#if defined(CFDESKTOP_OS_WINDOWS)
    // Windows 特定代码
#elif defined(CFDESKTOP_OS_LINUX)
    // Linux 特定代码
#endif
```

## 相关文档

- [expected 详解](./expected.md)
- [span 详解](./span.md)
- [ScopeGuard 详解](./scope_guard.md)
- [weak_ptr 详解](./weak_ptr.md)
- [weak_ptr_factory 详解](./weak_ptr_factory.md)
- [macros 详解](./macros.md)
- [system_judge 详解](./macro/system_judge.md)
- [CallOnceInit 详解](./once_init.md)
- [proc_parser 详解](../linux/proc_parser/)
