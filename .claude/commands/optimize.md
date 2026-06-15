# /optimize — 代码优化

基于 C++23 和现代 C++ 工程实践，对代码进行逼近零开销的小改动优化。

## 触发方式

- `/optimize <文件/函数>` — 优化指定代码
- Review 输出对接 — 用户基于审查报告请求优化

## 硬约束 (MUST)

- 每项优化改动 **1-5 行**
- **禁止**改变公共 API 签名（破坏 ABI）
- **禁止**改变行为（只做优化）
- **禁止**大规模重构
- **必须**通过 `-Wall -Wextra -Wpedantic` 编译
- **必须**通过现有测试
- 每文件每轮最多 **10 项优化**
- 不确定时加 `@note FIXME` 而非猜测

## 优化流程

### Step 1: 读取目标代码

- 读取指定文件完整内容
- 读取相关头文件和测试文件
- 理解类层次和使用上下文

### Step 2: 优化检查清单

#### 2.1 constexpr 提升
- 编译期可计算的函数 → `constexpr`
- 魔数 → `constexpr` 常量
- 类型特征和辅助工具 → `constexpr` 优先于模板元编程
- `if constexpr` 替代运行时分支（类型分发场景）

#### 2.2 移动语义
- 按值返回 vs 显式 `std::move`（NRVO 场景不要 move，会阻碍优化）
- Sink 参数用按值传递 + `std::move` 进成员
- 构造函数中用 `std::move` 初始化成员
- **不要**对返回局部变量 `std::move`（阻碍 RVO/NRVO）

#### 2.3 零开销抽象
- 编译期可确定的多态 → 模板参数替代虚分派
- 小函数隐式内联（头文件定义）
- CRTP 替代虚分派（静态多态）
- 固定大小容器 → `std::array` 替代 `std::vector`

#### 2.4 [[nodiscard]] 标注
- 所有 const getter
- 返回所有权的工厂函数
- 返回 `expected<T>` 或错误码的函数
- 忽略返回值会导致 bug 的函数

#### 2.5 智能指针优化
- 独占所有权 → `std::unique_ptr`（零开销）
- 共享所有权 → `std::shared_ptr`（仅在必要时）
- `std::make_unique` / `std::make_shared` 优于裸 `new`
- 观察者模式 → 裸指针/引用，不用智能指针

#### 2.6 Qt 特定优化
- `QString` → 使用 `QStringView` / `qsizetype`
- Signal/Slot → 新式连接 (`&Class::method`)
- 避免 `QString::fromStdString` 不必要转换
- 像素操作 → `QImage` 优于 `QPixmap`
- `Q_FOREACH` → 范围 for 循环

### Step 3: 输出格式 (中文)

```markdown
# 优化建议: <file_name>

## 优化 1: <标题>
- **类别**: constexpr/移动语义/零开销/[[nodiscard]]/智能指针/Qt
- **当前代码**:
  ```cpp
  // file:line
  ```
- **优化后代码**:
  ```cpp
  // 修改后
  ```
- **原理**: ...
- **影响范围**: 低/中
- **验证方法**: 编译 + 运行测试

## 优化 2: ...

## 总结
- 优化项数: N
- 预估性能提升: ...
- 风险等级: 整体低/中
```
