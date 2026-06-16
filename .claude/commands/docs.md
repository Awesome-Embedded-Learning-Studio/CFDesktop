# /docs — 文档审查与补全

审查指定模块的文档准确性，补全缺失文档。

## 触发方式

- `/docs <模块路径>` — 审查模块文档
- 用户说"审查/改进 XXX 模块文档"

## 三大文档源

1. **Doxygen 源码注释** — 在 `.h`/`.hpp` 文件中的 `/** */` 或 `///` 注释
2. **MkDocs 文档** — `document/HandBook/` 和 `document/design_stage/` 目录
3. **模块 README** — 各模块目录下的 README 文件

## 审查流程

### Step 1: 确定审查范围

- 用户指定模块、文件或目录
- 列出头文件和源文件清单
- 识别对应的文档文件（`document/HandBook/` 下）

### Step 2: Doxygen 合规性检查

参考规范：`document/DOXYGEN_REQUEST.md`（权威标准）

对每个头文件检查：

#### 文件级检查
- `@file` 头部存在且路径正确
- `@brief`, `@author`, `@date`, `@version`, `@since`, `@ingroup` 标签完整
- 行宽 ≤ 100 字符

#### 类型注释检查
- 每个公共 `class`/`struct`/`enum` 前有 `@brief`
- `@details` 描述生命周期和所有权（如有）
- `@ingroup` 模块归属正确

#### 函数注释检查
- `@brief` 使用第三人称现在时
- `@param` 包含方向标记 `[in]/[out]/[in,out]`
- `@param` 顺序与函数签名一致
- 非 void 函数有 `@return`，void 函数**没有** `@return`
- `@throws`, `@note`, `@warning`, `@since`, `@ingroup` 标签存在
- 模板参数有 `@tparam`

#### 风格一致性检查
- 同一文件内统一使用 `/** */` 或 `///`
- 行宽 ≤ 100 字符
- 无第一人称（"we", "I", "our"）

### Step 3: 代码 vs 文档准确性

对每个已文档化的 API：
1. `@param` 名称是否匹配函数签名
2. `@return` 是否匹配实际返回类型
3. `@throws` 是否匹配实际异常行为
4. 描述的行为是否匹配实现
5. 标记所有 `@note FIXME` 条目

### Step 4: MkDocs 文档检查

1. 验证 `document/` 文件引用的路径是否存在
2. 架构描述是否匹配当前代码结构
3. 文档中的代码示例是否可编译且为最新
4. `document/design_stage/` 的完成度描述是否准确

### Step 5: 输出报告 (中文)

```markdown
# 文档审查报告: <module_name>

## Doxygen 合规性
### 合规文件
- `path/to/file.h` — 完全合规

### 违规文件
- `path/to/file.h` — 具体违规项:
  1. 缺少文件级 @file 头
  2. 函数 foo() 缺少 @param 方向标记
  3. ...

### 缺失文档的公共 API
- `Class::method()` — 完全无文档
- `Class::anotherMethod()` — 仅有 @brief，缺少标签

## 文档准确性
### 不一致列表
| 文件 | API | 文档描述 | 实际行为 |
|------|-----|----------|----------|

## MkDocs 文档状态
- 过时内容: ...
- 缺失页面: ...
- 路径失效引用: ...

## 修复优先级
1. **[高]** ...
2. **[中]** ...
3. **[低]** ...
```

## 集成现有基建

- Doxygen 修复遵循 `AGENT.md` 的 5 步流程
- 验证运行 `python3 scripts/doxygen/lint.py`
- MkDocs 导航结构参考 `mkdocs.yml`
