# /testing — 测试覆盖建议

为新增或修改的代码建议测试用例，参考现有测试模式。

## 触发方式

- `/testing <模块路径>` — 建议测试用例
- `/next-step` 或 `/optimize` 衔接

## 工作流程

### Step 1: 分析目标代码

1. 读取源文件，识别公共 API 接口：
   - 公共方法（public methods）
   - 信号和槽（signals/slots）
   - 枚举和常量
   - 模板函数/类

2. 识别边界情况：
   - 空输入、零值、最大值
   - 无效输入、越界访问
   - 资源限制（内存、文件句柄）

3. 识别线程安全问题：
   - 锁自由数据结构
   - 异步操作
   - 共享状态访问

### Step 2: 检查现有测试覆盖

1. 在 `test/<module>/<component>/` 查找已有测试文件
2. 读取已有测试，理解当前覆盖范围
3. 识别未覆盖的代码路径

### Step 3: 参考现有测试模式

**文件模式** (`test/<module>/<component>/<component>_test.cpp`)：
```cpp
/**
 * @file    <component>_test.cpp
 * @brief   Test coverage for <Component>.
 */
#include <gtest/gtest.h>
#include "<component>.h"

TEST(<Component>Suite, BasicFunctionality) {
    // Arrange
    // Act
    // Assert
}
```

**CMake 模式** (`test/<module>/CMakeLists.txt`)：
```cmake
add_gtest_executable(<component>_test
    <component>/<component>_test.cpp
    LINK_LIBRARIES <target> GTest::gtest GTest::gtest_main
    LABELS "module;unit;component"
)
```

### Step 4: 测试建议分级

#### P0 — 必须有 (Critical Path)
- 构造函数/析构函数基本功能
- 主要用例（happy path）
- 错误处理（无效输入）

#### P1 — 应该有 (Boundary)
- 空输入、零值、最大值
- Off-by-one 边界
- 资源限制

#### P2 — 可选有 (Integration/Regression)
- Qt 框架交互（Signal/Slot）
- 跨组件交互
- 线程安全测试
- 性能回归测试

### Step 5: 输出格式 (中文)

```markdown
# 测试建议: <module/component>

## 现有测试状态
- 已有测试文件: (列出或 "无")
- 覆盖率评估: X%
- 未覆盖的关键路径: ...

## 建议新增测试

### P0 — 必须
```cpp
// TEST(<Component>Suite, ConstructorInitializesCorrectly)
TEST(<Component>Suite, ConstructorInitializesCorrectly) {
    // Arrange & Act
    <Component> obj{/* params */};
    // Assert
    EXPECT_EQ(obj.<getter>(), expected);
}
```

### P1 — 应该
(测试用例骨架)

### P2 — 可选
(测试用例骨架)

## 测试文件位置
- 建议路径: `test/<module>/<component>/<component>_test.cpp`
- CMakeLists.txt 修改:
```cmake
add_gtest_executable(<component>_test ...)
```

## 预估工作量
- P0 测试数量: N
- P1 测试数量: N
- P2 测试数量: N
- 预估耗时: X minutes
```

## 模块特定注意事项

- **base/ 工具库**: header-only，链接 `cfbase_headers` + `GTest`
- **ui/ 控件**: 需要 `QApplication`，链接 `cfui` + `Qt6::Widgets` + `Qt6::Gui`
- **desktop/**: 需要完整 `CFDesktop_shared` + `Qt6::Widgets`
- **Signal/Slot 测试**: 需要 `Qt6::Test`，使用 `QSignalSpy`
