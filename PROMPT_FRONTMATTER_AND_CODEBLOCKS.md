# CFDesktop 文档收尾：Frontmatter 补充 + 代码块语言标签统一

## 项目背景

CFDesktop 是一个 C++23/Qt6/CMake 跨平台桌面框架，文档站使用 VitePress 构建，源文件在 `document/` 目录下。此前已完成一轮文档一致性修复（英文→中文翻译、C++17 过时引用修正、占位内容清理、重复目录删除、遗留 MkDocs CSS 删除）。

现在需要完成两项收尾工作。

---

## 任务一：为所有非占位 .md 文件补充 VitePress YAML frontmatter

### 规则

每个 `.md` 文件**开头**必须添加如下 frontmatter（如果没有的话）：

```yaml
---
title: 文档标题
description: 一句话描述页面内容
---
```

**要求：**
- `title`：取文件中第一个 `#` 标题的内容（去掉 markdown 标记），中文标题保持中文
- `description`：根据文件内容写一句简短中文描述（20-50 字），包含英文技术术语
- 如果文件已有 frontmatter，**不要重复添加**，检查是否缺少 `title` 或 `description`，补齐即可
- `index.md` 文件通常标题是目录名描述，也需要加
- **跳过** `document/api/` 目录（这是 Doxygen 自动生成的，不在 VitePress 构建范围内）
- `---` 是 YAML frontmatter 边界标记，和文件正文中的 `---`（水平分割线）不冲突——frontmatter 的 `---` 必须出现在文件的**最开始**两行

### 判断是否已有 frontmatter

如果文件以 `---` 开头（第一行就是 `---`），则已有 frontmatter，检查并补齐字段。
如果文件以 `#` 或其他内容开头，则没有 frontmatter，需要添加。

### 示例

修改前：
```markdown
# 快速开始指南

> 30 分钟内搭建 CFDesktop 开发环境
```

修改后：
```markdown
---
title: 快速开始指南
description: 30 分钟搭建 CFDesktop 开发环境，涵盖克隆仓库、Docker 构建、运行示例和测试
---

# 快速开始指南

> 30 分钟内搭建 CFDesktop 开发环境
```

### 需要处理的目录

```
document/development/     (10 个文件)
document/design_stage/    (10 个文件)
document/HandBook/        (~150 个文件，排除 api/ 子目录)
document/notes/           (7 个文件)
document/ci/              (5 个文件)
document/todo/            (~37 个文件)
document/optimize/        (2 个文件)
document/release_rule/    (2 个文件)
document/scripts/         (~59 个文件)
document/status/          (2 个文件)
document/index.md         (首页)
```

**跳过：** `document/api/` 目录（自动生成，不在 VitePress 范围内）

---

## 任务二：统一代码块语言标签

### 问题

文档中有 336 处代码块使用了无语言标签的 ` ``` ` 开关，导致 VitePress 无法正确进行语法高亮。

### 规则

将所有无语言标签的代码块根据内容加上正确的标签：

| 内容类型 | 标签 |
|---------|------|
| C++ 代码 | `cpp` |
| CMake 代码 | `cmake` |
| Bash/Shell 命令 | `bash` |
| PowerShell 命令 | `powershell` |
| INI 配置 | `ini` |
| JSON | `json` |
| YAML | `yaml` |
| 纯文本输出/目录树 | `text` |
| Diff | `diff` |
| Python | `python` |

**判断方法：**
- 看代码块内容判断语言
- 目录树结构（有 `├──`、`└──` 等）用 `text`
- 终端命令输出（有 `$` 提示符）用 `bash`
- 纯文本日志/输出用 `text`
- 如果实在无法判断，用 `text`

### 示例

修改前：
````
```
cmake_minimum_required(VERSION 3.16)
project(CFDesktop)
```
````

修改后：
````
```cmake
cmake_minimum_required(VERSION 3.16)
project(CFDesktop)
```
````

### 注意

- **只修改无标签的代码块**（即开头的 ` ``` ` 后面什么都没有的）
- 已有语言标签的代码块（如 ` ```cpp `、` ```bash `）**不要动**
- 代码块内的内容**不要修改**，只添加语言标签

---

## 文档约定

- 文档语言：中文 + 英文技术术语
- 项目版本：0.18.0
- C++ 标准：C++23（不是 C++17）
- Qt 版本：6.8.3
- CMake 最低版本：3.16
- 三层架构：base/ → ui/ → desktop/（严格单向依赖）

## 验证

完成所有修改后，运行以下验证：

```bash
# 检查是否还有无标签的代码块（应该大幅减少）
grep -rn '^```$' document/ --include='*.md' | grep -v 'document/api/' | wc -l

# 检查所有非 api 目录的 .md 文件是否都有 frontmatter
find document/ -name '*.md' -not -path 'document/api/*' | while read f; do
  head -1 "$f" | grep -q '^---$' || echo "MISSING FRONTMATTER: $f"
done

# 检查文档站能否正常构建
cd /home/charliechen/CFDesktop && pnpm build
```
