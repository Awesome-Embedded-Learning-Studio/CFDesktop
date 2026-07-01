---
title: "Milestone 7: 第三方 App 平台"
description: "预计周期: 分阶段演进;前置依赖: Milestone 4 双态地基"
---

# Milestone 7: 第三方 App 平台

> **状态**: 📋 规划中(未启动)
> **前置依赖**: [Milestone 4: 应用启动器](milestone_04_app_launcher.md) 双态地基已落地(LaunchKind/IBuiltinPanel/BuiltinPanelRegistry/auto 裁决)
> **目标**: 让外部贡献者/独立仓按规范写 App,运行时被 CFDesktop 发现并集成为一等公民

---

## 一、定位与跨硬件取舍

第三方 App **只能走独立进程**(`LaunchKind::DetachedProcess`)——进程内 builtin 是 shell 维护者的特权(源码编译期链进 shell 二进制),第三方代码不应进 shell 进程(ABI 耦合 + 崩溃 + 安全)。

跨硬件谱系下的现实:

| 档位 | 第三方独立进程 App 的处境 |
|------|---------------------------|
| i.MX6ULL(Low) | 每个 ~30-50MB,shell 自己在内存红线上挣扎——**第三方基本跑不动/不该跑**。6ULL App 生态靠内置 builtin |
| RK3568(Mid) | 勉强能开少量,需克制 |
| RK3588/上位机(High) | **第三方主战场**,内存宽裕,独立进程随便开 |

**结论**:本里程碑是 **RK/上位机阶段**的特性,不阻塞 6ULL North Star。但 manifest 规范与发现路径**现在就要前瞻预留**(内置 App 用同一份 manifest,定了后面是"开放"而非"重做")。

---

## 二、分阶段实现

### Phase 1 — manifest 规范扩展

`app.json` 在现有 4 字段(app_id/display_name/exec/launch_kind)基础上加:

| 字段 | 必要性 | 用途 |
|------|--------|------|
| `icon` | 建议 | 网格图标(calculator 现缺,补) |
| `version` | 建议 | App 版本 |
| `min_shell_version` | 第三方特需 | shell 版本低于此值不加载 + 记日志(对齐 no-silent-fallback) |
| `single_instance` | 第三方特需 | 第二实例启动时 shell 把已有窗口拉前台 |
| `categories` | 可选 | 网格分组 |

**刻意不做** capabilities/permissions 沙箱:第三方是独立进程,跑在用户权限下跟 shell 平级,OS 级权限本就是它自己的事。沙箱是 OS 层(cgroup/seccomp),不是桌面 shell 现阶段该背的。

### Phase 2 — 多路径发现

现 [app_discoverer.cpp](../../../desktop/ui/components/launcher/app_discoverer.cpp) 只扫 `<bin>/../apps/`。加 XDG 语义路径:

| 路径 | 用途 | 优先级 |
|------|------|--------|
| `<bin>/../apps/`(内置) | 主仓自带 | 最高 |
| `~/.local/share/cfdesktop/apps/` | 单用户安装 | 中 |
| `/usr/share/cfdesktop/apps/` | 发行版打包 | 低 |

`app_id` 冲突时高优先级覆盖。可选吃一层系统 `.desktop`(让 CFDesktop 自动发现 firefox 等),作为兼容层。

### Phase 3 — cfapp SDK

第三方独立进程 App 想跟 shell 集成(主题跟随/读配置/单实例/任务栏指示),需 Runtime SDK。**全部走 QLocalSocket**(对齐 6ULL 铁律:文件/socket IPC,不引 D-Bus)。

| 能力 | 干什么 |
|------|--------|
| Theme bridge | shell 推当前主题 token,App 用 QuarkWidgets 应用 |
| Single-instance | 启动握手,已有实例则拉前台 |
| Launch handshake | App 注册("id=X,pid=Y,开窗了")→ 任务栏运行指示(补 `AppEntry::is_running`) |
| Config bridge | 读写 `cfdesktop` 命名空间共享配置 |

**cfapp 依赖边界**:只依赖 aex + QuarkWidgets + Qt,**不碰 cfui/cfbase/desktop**。**可选**:第三方不链接 cfapp 也能跑(失去集成),平缓接入曲线。

**位置**:独立 lib(主仓 `sdk/cfapp/` 或独立仓),实现时定。是 [[desktop-buildout-handoff]] 提及的"desktop-extension-tools"的演进。

### Phase 4 — 最低限度安全

manifest 解析加防御:字段长度上限、exec 路径不逃逸 apps 目录、JSON 体积上限、启动失败可见(已有)。更深沙箱是 OS 层,不背。

---

## 三、依赖与架构债

- **依赖 milestone_04 双态地基**:LaunchKind/Registry/auto 裁决已就位,本里程碑在其上扩展。
- **架构债(待还)**:milestone_04 步骤 7 让 desktop 编译期引用 `apps/calculator` 源文件 + parser lib(desktop→apps 反向依赖,违反分层 spirit)。本里程碑 **Phase 3 前置任务**:把 calculator 核心(parser+panel)迁到中立层(新 shared lib 或 `third_party`),desktop 与 apps exe 都向下依赖它,彻底解耦。迁移完成后 `grep -r '#include.*apps/' desktop/` 应为空。

---

## 四、验收标准(完成时)

- [ ] manifest 规范文档化,calculator manifest 补 icon
- [ ] 多路径发现:用户级/系统级 App 被发现
- [ ] cfapp SDK:主题跟随 + 单实例 + 任务栏指示(可选接入)
- [ ] calculator 核心迁中立层,desktop→apps 依赖清零
- [ ] 安全:manifest 解析防御 + 失败可见

---

*创建: 2026-07-01(随 milestone_04 builtin 正式化一并规划,未实现)*
