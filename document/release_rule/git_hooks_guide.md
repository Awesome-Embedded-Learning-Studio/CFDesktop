# Git Hooks 使用指南

本项目配置了 Git hooks，在本地进行代码质量检查和构建验证，确保远程 main 分支始终保持可构建状态。

## 目录

- [快速开始](#快速开始)
- [安装钩子](#安装钩子)
- [钩子说明](#钩子说明)
- [分支策略](#分支策略)
- [验证级别](#验证级别)
- [常见问题](#常见问题)
- [卸载钩子](#卸载钩子)

---

## 快速开始

### 安装

**Linux/macOS:**
```bash
bash scripts/release/hooks/install_hooks.sh
```

**Windows (PowerShell):**
```powershell
.\scripts\release\hooks\install_hooks.ps1
```

### 验证安装

```bash
# Linux/macOS
ls -la .git/hooks/pre-commit .git/hooks/pre-push

# Windows
dir .git\hooks\pre-commit.*
```

---

## 安装钩子

### 前置要求

| 组件 | 要求 | 说明 |
|------|------|------|
| Git | >= 2.0 | 用于执行钩子脚本 |
| Docker | 最新版 | 用于 pre-push 构建验证 |
| clang-format | 可选 | 用于 pre-commit 格式检查 |

### 安装步骤

1. **运行安装脚本**

   安装脚本会自动：
   - 检查是否在 Git 仓库中
   - 备份现有的自定义钩子（如果有）
   - 复制钩子到 `.git/hooks/` 目录
   - 设置执行权限

2. **确认安装成功**

   ```bash
   cat .git/hooks/pre-commit | head -n 5
   ```

   应该看到钩子脚本的开头部分。

---

## 钩子说明

### pre-commit

**触发时机**: 每次 `git commit` 时

**适用分支**: 所有分支

**检查内容**:
1. 空白字符检查（尾随空格、混合缩进等）
2. C++ 代码格式检查（需要 clang-format）

**失败行为**: 阻止提交

**绕过方法**:
```bash
git commit --no-verify -m "message"
```

---

### pre-push

**触发时机**: 每次 `git push` 时

**适用分支**: 仅 `main` 和 `release/*` 分支

**检查内容**: Docker 容器内完整构建 + 测试

**失败行为**: 阻止推送，远程仓库保持干净

**绕过方法**:
```bash
git push --no-verify
```

---

## 分支策略

| 分支类型 | 命名规则 | pre-commit | pre-push | 推送到远程 |
|----------|----------|-----------|----------|-----------|
| **main** | - | ✅ | ✅ (FastBuild) | ✅ |
| **release** | `release/x.y` | ✅ | ✅ (自动检测) | ✅ |
| **feat** | 任意 | ✅ | ❌ | ❌ |

**工作流程**:

```
1. 创建 feat 分支开发
   $ git checkout -b feat/my-feature
   # ... 编码 ...
   $ git commit -m "feat: xxx"  # 触发 pre-commit

2. 合并到 main
   $ git checkout main
   $ git merge feat/my-feature
   $ git push                  # 触发 pre-push 验证

3. 验证失败时回退
   $ git reset --hard origin/main
```

---

## 验证级别

### main 分支

默认验证级别：**X64 FastBuild + Tests**

```bash
# 等价于执行
docker_start.sh --verify --fast-build --arch amd64
```

### release 分支

根据版本号变化自动检测验证级别：

| 版本变化 | 验证级别 | 执行命令 |
|----------|----------|----------|
| **Major** (x.y.z → x+1.0.0) | X64 + ARM64 完整构建 | `docker_start.sh --verify --arch amd64` + `--arch arm64` |
| **Minor** (x.y.z → x.y+1.0) | X64 完整构建 | `docker_start.sh --verify --arch amd64` |
| **Patch** (x.y.z → x.y.z+1) | X64 FastBuild + Tests | `docker_start.sh --verify --fast-build --arch amd64` |

**示例**:

```
远程 main: 1.2.3

推送 release/1.2.4 (标签 1.2.4)
  → Patch 变化 → X64 FastBuild + Tests

推送 release/1.3.0 (标签 1.3.0)
  → Minor 变化 → X64 完整构建

推送 release/2.0.0 (标签 2.0.0)
  → Major 变化 → X64 + ARM64 完整构建
```

---

## 常见问题

### Q: pre-push 验证太慢，可以跳过吗？

**A**: 可以使用 `--no-verify` 跳过，但不推荐：

```bash
git push --no-verify
```

更好的做法是确保本地代码通过测试后再推送。

---

### Q: Docker daemon 未运行怎么办？

**A**: 启动 Docker Desktop 或 Docker 服务：

```bash
# Windows/macOS
# 启动 Docker Desktop 应用程序

# Linux
sudo systemctl start docker
sudo systemctl enable docker
```

---

### Q: clang-format 未安装会怎样？

**A**: pre-commit 会跳过格式检查并显示警告：

```
⚠ clang-format 未安装，跳过格式检查
```

不影响提交，但建议安装：

```bash
# Ubuntu/Debian
sudo apt install clang-format

# macOS
brew install clang-format

# Windows
# 通过 LLVM 或包管理器安装
```

---

### Q: 如何验证钩子是否正常工作？

**A**: 测试 pre-commit：

```bash
# 制造格式错误
echo "int x;" >> src/some_file.cpp
git add src/some_file.cpp
git commit -m "test"
# 应该被阻止
```

测试 pre-push：

```bash
git checkout main
# 合并一些更改
git push
# 应该触发 Docker 验证
```

---

### Q: 验证失败后如何回退？

**A**: 使用以下命令回退本地 main 分支：

```bash
git fetch origin
git reset --hard origin/main
```

**注意**: 这会丢弃本地所有未推送的更改，请确保已备份重要内容。

---

### Q: release 分支的版本号从哪里获取？

**A**: 钩子通过以下方式获取版本号：

1. **本地版本**: 使用 `git describe --tags --abbrev=0` 获取最近的标签
2. **远程版本**: 使用 `git fetch` 获取远程 main 分支的最近标签

**确保在推送 release 分支前已创建版本标签**：

```bash
git tag 1.2.4
git push origin release/1.2
```

---

### Q: ARM64 构建需要很长时间？

**A**: 是的，在 x86_64 主机上，ARM64 构建使用 QEMU 仿真，速度较慢。

**优化建议**:
- 仅在 Major 版本发布时运行 ARM64 验证
- 考虑使用 ARM64 runner（GitHub Actions 自托管）

---

## 卸载钩子

### 完全卸载

```bash
# Linux/macOS
rm .git/hooks/pre-commit .git/hooks/pre-push

# Windows
Remove-Item .git\hooks\pre-commit, .git\hooks\pre-push
```

### 重新安装

直接运行安装脚本即可，会自动覆盖现有钩子：

```bash
# Linux/macOS
bash scripts/release/hooks/install_hooks.sh

# Windows
.\scripts\release\hooks\install_hooks.ps1
```

---

## 文件结构

```
scripts/release/hooks/
├── install_hooks.sh       # Linux/macOS 安装脚本
├── install_hooks.ps1      # Windows 安装脚本
├── version_utils.sh       # 版本检测辅助函数
├── pre-commit.sample      # pre-commit 钩子模板
└── pre-push.sample        # pre-push 钩子模板

document/release_rule/
└── git_hooks_guide.md     # 本文档
```

安装后：
```
.git/hooks/
├── pre-commit             # 从 pre-commit.sample 复制
└── pre-push               # 从 pre-push.sample 复制
```

---

*文档版本: v1.0 | 更新日期: 2026-03-07*
