# CFDesktop 当前状态

> 最后更新: 2026-05-22

## 基本信息

| 项目 | 当前值 |
|------|--------|
| 版本 | 0.18.0 |
| 语言标准 | C++23 |
| UI 框架 | Qt 6.8.x |
| 构建系统 | CMake + Docker build helpers |
| 文档系统 | VitePress |
| 当前开发分支 | develop |

## 当前基线

- 本地 `out/build_develop/test` 可发现 47 个 CTest 测试。
- 最近一次本地验证结果: 47/47 通过。
- VitePress 文档构建命令 `pnpm build` 可通过。
- 自动生成的 `document/api/**` 暂不纳入 VitePress 主站，避免旧 Doxybook2 生成页影响构建。

## 当前主线

短期建议优先推进“可见可用桌面闭环”：

1. 状态栏
2. 任务栏/导航栏
3. 应用启动器
4. 窗口管理可见联动

HWTier、InputManager、RenderBackend、Wayland/EGLFS 后端仍然重要，但当前不应阻塞桌面可演示版本。

## 已知需要收敛的地方

- 历史文档中仍有 `0.13.1`、C++17、MkDocs 等旧描述。
- `document/todo/done/` 是历史状态归档，不应作为当前事实源。
- API 自动文档二期需要重新选择发布方式：Doxygen HTML 独立发布，或修复 Doxybook2 Markdown 链接后再纳入主站。
