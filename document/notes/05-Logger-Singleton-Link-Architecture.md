# Logger 单实例链接架构

本文档记录 CFDesktop 项目中 `cflogger`（日志系统）的单实例保证方案，包括 CMake 链接策略、`INTERFACE` 头文件库的引入、以及 DLL 导出/导入机制的设计决策。

---

## 1. 问题背景

### 1.1 项目架构

CFDesktop 采用"多静态库 → 单一共享库"架构：

```
多个 STATIC library (cflogger, cfbase, CFDesktopMain, CFDesktopUi, ...)
        ↓ --whole-archive
CFDesktop_shared (SHARED / DLL)
        ↓
CFDesktop.exe
```

所有静态库通过 `--whole-archive` 合并进一个 DLL，EXE 只链接这个 DLL。

### 1.2 Logger 单例问题

`cflogger` 内部使用单例模式（`Logger::instance()`）。如果同一份 `.cpp` 被编译进多个模块，会产生**多个独立的单例实例**，导致：

- 日志输出丢失（模块 A 初始化的 Sink，模块 B 看不到）
- 全局状态不一致（日志级别、格式器各自独立）
- 静态初始化顺序问题（Static Initialization Order Fiasco）

### 1.3 原始问题：cflogger 被到处 PUBLIC 链接

修改前，`cflogger` 被多个模块通过 `PUBLIC` 关键字链接：

| 目标 | 链接方式 | 问题 |
|------|---------|------|
| `CFDesktopMain` | PRIVATE | 不传播，OK |
| `CFDesktopLog` | **PUBLIC** | 向所有消费者传播 |
| `CFDesktopEarlySession` | **PUBLIC** | 向所有消费者传播 |
| `CFDesktopPathSettings` | **PUBLIC** | 向所有消费者传播 |
| `cf_desktop_ui_platform` | **PUBLIC** | 向所有消费者传播 |
| `cf_desktop_ui_widget_init_session` | **PUBLIC** | 向所有消费者传播 |

`PUBLIC` 链接会在编译期将 `cflogger` 的 include 目录和链接需求传播给所有下游消费者，造成不必要的耦合。虽然在当前 `--whole-archive` 方案下不会产生多实例（因为所有静态库最终打包进同一个 DLL），但这种传播增加了维护复杂度，且容易在架构调整时引入问题。

---

## 2. 解决方案：INTERFACE 头文件库

### 2.1 核心思想

引入一个 `cflogger_headers`（INTERFACE library），让 DLL 内部的模块**只拿头文件**，不链接静态库实体。唯一实际链接 `cflogger` 静态库的地方是 `CFDesktop_shared`（通过 `--whole-archive`）和 `CFDesktopMain`（PRIVATE，因为直接使用了内部类）。

### 2.2 实现代码

在 `desktop/base/logger/CMakeLists.txt` 中新增：

```cmake
# Header-only interface for internal modules that only need cflog.h
# Modules inside CFDesktop_shared should use this instead of linking cflogger
# to avoid duplicating the static lib symbols — only CFDesktop_shared links the real library.
add_library(cflogger_headers INTERFACE)
target_include_directories(cflogger_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(cflogger_headers INTERFACE cfbase)
```

### 2.3 为什么需要 `cflogger_headers` 而不是直接用 `cflogger`

| 方式 | 效果 |
|------|------|
| `target_link_libraries(X PUBLIC cflogger)` | X 的消费者也会链接 `libcflogger.a`（传播） |
| `target_link_libraries(X PRIVATE cflogger)` | 只有 X 链接 `libcflogger.a`（不传播），但 X 的 `.a` 里会包含对 cflogger 符号的引用 |
| `target_link_libraries(X PUBLIC cflogger_headers)` | X 及其消费者只获得 `cflog.h` 头文件路径，不链接任何 `.a` |

`INTERFACE` 库本身没有编译产物（没有 `.a` / `.o`），它只是一个**编译期元数据载体**，传递 include 目录和编译定义。

---

## 3. 最终链接拓扑

### 3.1 完整依赖图

```
cflogger (STATIC) ──────────────────────────────┐
  ├─ cflog.cpp, cflog_impl.cpp                  │
  ├─ console_sink.cpp, file_sink.cpp            │
  ├─ console_formatter.cpp, file_formatter.cpp  │
  └─ async_queue.cpp                            │
                                                │ --whole-archive
cflogger_headers (INTERFACE)                    ├────────────────→ CFDesktop_shared (DLL)
  ├─ include dirs only                          │
  └─ links: cfbase                              │
      ↑                                         │
      │ use (headers only)                      │
      ├─ CFDesktopLog                           │
      ├─ CFDesktopEarlySession                  │
      ├─ CFDesktopPathSettings                  │
      ├─ cf_desktop_ui_platform                 │
      └─ cf_desktop_ui_widget_init_session      │
                                                │
CFDesktopMain (STATIC) ──PRIVATE──→ cflogger ───┘
  └─ 直接使用了 FileSink, ConsoleSink 等内部类
      这些类没有 CFLOG_API 标记，不能通过 DLL 导入
```

### 3.2 各模块的角色

| 模块 | 如何使用 cflogger | 原因 |
|------|------------------|------|
| `CFDesktop_shared` | `--whole-archive` 链接 `cflogger` | 唯一的实际链接点，保证 DLL 内单实例 |
| `CFDesktopMain` | PRIVATE 链接 `cflogger` | `logger_stage.cpp` 直接实例化 `FileSink`/`ConsoleSink`，这些内部类没有 `CFLOG_API`，必须从静态库解析 |
| `CFDesktopLog` | PUBLIC 链接 `cflogger_headers` | 只调用 `CFLOG_INFO()` 等公开 API |
| `CFDesktopEarlySession` | PUBLIC 链接 `cflogger_headers` | 只调用公开 API |
| `CFDesktopPathSettings` | PUBLIC 链接 `cflogger_headers` | 只调用公开 API |
| `cf_desktop_ui_platform` | PUBLIC 链接 `cflogger_headers` | 只调用公开 API |
| `cf_desktop_ui_widget_init_session` | PUBLIC 链接 `cflogger_headers` | 只调用公开 API |
| Test / Example | `CFLOG_STATIC_BUILD` + 链接 `CFDesktop::logger` | 独立编译，不走 DLL 路径 |

---

## 4. DLL 导出/导入机制

### 4.1 `CFLOG_API` 宏定义

定义在 `desktop/base/logger/include/cflog/cflog_export.h`：

```cpp
#if defined(_WIN32) || defined(_MSC_VER)
    #ifdef CFLOG_STATIC_BUILD
        #define CFLOG_API                          // 测试/示例：无修饰
    #elif defined(CFLOG_BUILDING)
        #define CFLOG_API __declspec(dllexport)    // 构建 cflogger 时：导出
    #else
        #define CFLOG_API __declspec(dllimport)    // 消费者：导入
    #endif
#else
    #define CFLOG_API __attribute__((visibility("default")))  // Linux：可见
#endif
```

### 4.2 标记规则

| 标记了 `CFLOG_API` 的 | 用途 |
|----------------------|------|
| `Logger` 类 (`cflog.hpp`) | 核心单例，EXE 通过 DLL 导入使用 |
| `trace()`, `debug()`, `info()`, `warning()`, `error()` 全局函数 | 公开日志 API |
| `set_level()`, `flush()` | 运行时配置 API |

| 没有标记 `CFLOG_API` 的 | 原因 |
|------------------------|------|
| `ConsoleSink`, `FileSink` | 内部实现类，仅供 `CFDesktopMain` 的 `logger_stage.cpp` 使用 |
| `ConsoleFormatter`, `FileFormatter`, `AsciiColorFormatter` | 内部实现类 |
| `AsyncQueue` | 内部实现类 |

### 4.3 为什么 `CFDesktopMain` 不能用 `cflogger_headers`

如果 `CFDesktopMain` 改用 `cflogger_headers`，链接 `CFDesktop.exe` 时会报 `undefined symbol`：

```
ld.lld: error: undefined symbol: cf::log::FileSink::FileSink(...)
ld.lld: error: undefined symbol: vtable for cf::log::ConsoleSink
```

因为 `FileSink`、`ConsoleSink` 没有被 `CFLOG_API` 标记导出，EXE 无法通过 DLL 导入表找到它们。`CFDesktopMain` 必须 PRIVATE 链接 `cflogger` 静态库，让链接器直接从 `.a` 中解析这些符号。

---

## 5. LNK4217 警告修复：`CFLOG_STATIC_BUILD` 与 DLL 内部解析

### 5.1 问题现象

编译 `CFDesktop_shared.dll` 时，lld 链接器产生大量 LNK4217 警告：

```
ld.lld: warning: libCFDesktopMain.a(init_chain.cpp.obj):
    locally defined symbol imported:
    cf::log::Logger::instance()
    (defined in libcflogger.a(cflog.cpp.obj)) [LNK4217]
```

涉及 `.a` 文件：`libCFDesktopMain.a`、`libCFDesktopUi.a`、`libcf_desktop_ui_platform.a`。

### 5.2 根因分析

LLD 警告的含义是：**某 `.obj` 文件通过 `__declspec(dllimport)` 引用了一个符号，但该符号在同一 DLL 内有本地定义。**

具体链路：

```
cflog_export.h 中的宏展开逻辑：

  CFLOG_BUILDING      → __declspec(dllexport)   ← 仅 cflogger 自身编译时
  CFLOG_STATIC_BUILD  → (空)                     ← 测试/示例用
  默认                 → __declspec(dllimport)   ← 其他所有消费者
```

DLL 内的静态库（如 `CFDesktopMain`）编译时：
- 没有定义 `CFLOG_BUILDING`（那是 cflogger 自己的）
- 没有定义 `CFLOG_STATIC_BUILD`（之前没加）
- 所以 `CFLOG_API` = `__declspec(dllimport)`

但链接时，这些 `.obj` 文件和 `libcflogger.a` 被 `--whole-archive` 打进**同一个 DLL**。链接器发现：这些 `dllimport` 引用的符号其实就在本地。LLD 对此发出警告。

> **这不是多实例问题**——链接器确实使用了本地定义，运行时行为正确。
> 但警告噪音大，且暗示链接意图不清晰。

### 5.3 修复方案

给所有会打入 `CFDesktop_shared` 的静态库目标添加 `CFLOG_STATIC_BUILD` **PRIVATE** 编译定义：

```cmake
target_compile_definitions(CFDesktopMain PRIVATE CFLOG_STATIC_BUILD)
target_compile_definitions(CFDesktopUi PRIVATE CFLOG_STATIC_BUILD)
target_compile_definitions(cf_desktop_ui_platform PRIVATE CFLOG_STATIC_BUILD)
target_compile_definitions(CFDesktopLog PRIVATE CFLOG_STATIC_BUILD)
target_compile_definitions(cf_desktop_ui_widget_init_session PRIVATE CFLOG_STATIC_BUILD)
```

关键：**必须是 PRIVATE**，不能是 PUBLIC 或 INTERFACE。

- `PRIVATE` → 只作用于该目标的 `.cpp` 文件，不传播给消费者
- 如果用 PUBLIC → EXE 也会得到 `CFLOG_STATIC_BUILD`，导致 `CFLOG_API` = 空，EXE 不再 `dllimport`，链接失败

### 5.4 修复后的宏展开

| 编译目标 | 宏定义 | `CFLOG_API` 展开 | 效果 |
|---------|--------|-----------------|------|
| `cflogger`（静态库） | `CFLOG_BUILDING` | `__declspec(dllexport)` | 符号从 DLL 导出 |
| `CFDesktopMain`（静态库） | `CFLOG_STATIC_BUILD` | *(空)* | 本地引用，无 dllimport |
| `CFDesktopUi`（静态库） | `CFLOG_STATIC_BUILD` | *(空)* | 本地引用，无 dllimport |
| `cf_desktop_ui_platform`（静态库） | `CFLOG_STATIC_BUILD` | *(空)* | 本地引用，无 dllimport |
| `CFDesktop.exe` | *(无)* | `__declspec(dllimport)` | 从 DLL 导入，正确 |

### 5.5 判断哪些目标需要添加

原则：**所有 `.obj` 文件会通过 `--whole-archive` 进入 DLL、且 `#include` 了 `cflog.h` 的静态库目标。**

可通过编译输出中的 LNK4217 警告定位（警告里提到的 `.a` 文件对应的目标）。

不链接 cflogger 的目标（如 `cf_desktop_render`、`cfbase`）无需添加。

---

## 6. 单实例保证原理

### 6.1 在 DLL 架构下

```
cflogger.cpp 编译一次 → libcflogger.a (静态库)
                              ↓
                    --whole-archive 合并
                              ↓
              CFDesktop_shared.dll (唯一包含 Logger 实现的模块)

CFDesktop.exe 通过 dllimport 访问 Logger::instance()
CFDesktopMain.a 通过 PRIVATE 链接直接解析内部类符号
其他 .a 通过 cflogger_headers 只拿到头文件，不链接 .a
```

`--whole-archive` 确保即使没有直接引用的符号也被保留在 DLL 中，避免链接器丢弃"未使用"的 Logger 实现。

### 6.2 验证方法

在代码中打印 Logger 实例地址，确认所有模块使用同一个：

```cpp
printf("Logger: %p\n", &Logger::instance());
```

如果在 DLL 中调用和 EXE 中调用得到的地址相同，说明单实例保证生效。

---

## 7. CMake 关键决策记录

### 7.1 为什么不用 OBJECT library

```cmake
# 可选方案（未采用）
add_library(cflog_obj OBJECT cflog.cpp ...)
target_sources(CFDesktopMain PRIVATE $<TARGET_OBJECTS:cflog_obj>)
```

OBJECT library 要求所有使用 cflogger 的模块都从同一个 OBJECT target 获取 `.o` 文件，这在当前多模块架构中引入更多管理复杂度。当前的 `STATIC + --whole-archive` 方案已经足够保证单实例。

### 7.2 为什么不用 header-only 单例

```cpp
// 危险！不要这样做
inline Logger& instance() {
    static Logger x;  // 每个 TU 可能独立实例化
    return x;
}
```

`inline` 函数中的 `static` 局部变量在 C++17 前的行为是 UB（多个 TU 可能各自实例化），即使在 C++17 后也有跨动态库的复杂性。始终让 `.cpp` 文件只编译一次是最安全的做法。

### 7.3 未来如果去掉 DLL

如果将来改为全静态单进程架构（无 DLL），修改量极小：

1. `CFDesktop_shared` 从 `SHARED` 改为 `STATIC`
2. `CFDesktop.exe` 直接链接所有静态库（不需要 `--whole-archive`）
3. 移除 `CFLOG_API` 宏（或全部定义为空）
4. `cflogger_headers` 方案完全不需要改

---

## 8. 修改的文件清单

| 文件 | 改动内容 |
|------|---------|
| `desktop/base/logger/CMakeLists.txt` | 新增 `cflogger_headers` INTERFACE target |
| `desktop/main/log/CMakeLists.txt` | `cflogger` → `cflogger_headers`；添加 `CFLOG_STATIC_BUILD` PRIVATE |
| `desktop/main/early_session/CMakeLists.txt` | `cflogger` → `cflogger_headers` |
| `desktop/main/path/CMakeLists.txt` | `cflogger` → `cflogger_headers` |
| `desktop/main/CMakeLists.txt` | 添加 `CFLOG_STATIC_BUILD` PRIVATE；保持 `cflogger` PRIVATE |
| `desktop/ui/CMakeLists.txt` | 添加 `CFLOG_STATIC_BUILD` PRIVATE |
| `desktop/ui/platform/CMakeLists.txt` | `cflogger` → `cflogger_headers`；添加 `CFLOG_STATIC_BUILD` PRIVATE |
| `desktop/ui/widget/init_session/CMakeLists.txt` | `cflogger` → `cflogger_headers`；添加 `CFLOG_STATIC_BUILD` PRIVATE |

---

## 9. 扩展建议

### 9.1 未来新模块使用 cflogger 的规则

- 如果只调用 `CFLOG_INFO()` 等公开 API → 链接 `cflogger_headers`
- 如果直接实例化 `FileSink`/`ConsoleSink` 等内部类 → PRIVATE 链接 `cflogger`（仅限 DLL 内部模块）
- 测试/示例 → `CFLOG_STATIC_BUILD` + `CFDesktop::logger`
- **新模块如果会进入 `CFDesktop_shared` DLL** → 必须添加 `CFLOG_STATIC_BUILD` PRIVATE（参见第 5 节）

### 9.2 适用于其他核心系统

同样的模式可以用于事件系统、配置系统等需要单实例保证的核心模块：

```cmake
# 事件系统示例
add_library(cfevent STATIC ...)
add_library(cfevent_headers INTERFACE ...)
target_include_directories(cfevent_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
```

**原则：`.cpp` 只编译一次，`.h` 可以到处传。**

---

*本文档编写于 2026-03-30，基于 CFDesktop 项目 feat/windows_backend 分支的实际架构改动。*
