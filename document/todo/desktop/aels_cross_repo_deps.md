---
title: AELS 跨仓依赖登记
description: CFDesktop 消费的 AELS 组织独立仓（含拟新建）的跨仓依赖与接口契约
---

# AELS 跨仓依赖登记

> **创建日期**: 2026-06-29
> **用途**: 登记 CFDesktop 消费的 AELS 组织独立仓（含拟新建）的依赖关系与接口契约。
> 这些能力横跨多板可复用、解耦于桌面框架内部，按 QuarkWidgets / aex 范式独立成仓。
> **原则**: 桌面 shell 只持有薄接口，真实后端在独立仓；不在 shell 里耦合系统栈。

## 背景：AELS 组织已有可复用仓（不重造）

| 仓 | 作用 | CFDesktop 关系 |
|----|------|----------------|
| **QuarkWidgets** | 跨项目统一 Qt 组件基座（MD3 五层 pipeline） | 已通过 submodule `third_party/QuarkWidgets/` 消费（CMake 目标 `QuarkWidgets::quarkwidgets`） |
| **aex** | 轻量 C++ 扩展库 | 已通过 submodule `third_party/aex/` 消费 |
| **miniwget** | 现代 C++ mini wget | aels-apprepo 下载层复用 |
| **imx-forge / rk-forge** | 板级 uboot/kernel/rootfs 补丁 | aels-ota 双 rootfs A/B 切换承载于此 |
| **lightroot** | 现代化 BuildRoot（根文件系统构建） | 与 OTA/部署同层 |
| **edgecv** | OpenCV 封装（desktop + embedded） | 相机/图像应用可选依赖 |
| **bareline** | 嵌入式+PC CLI 交互框架 | 终端 App 的 pty 层可借鉴 |
| **CF-Gallery** | 摄影静物壁纸静态资源包（CC BY-NC-ND 4.0，VitePress 收录站） | **非构建依赖**：运行时动态发现——资源包安装到 `<desktop_active_root>/Wallpapers/<pack>/`（默认 `~/desktop/Wallpapers/`）即被壁纸层递归扫描消费。CFDesktop 零编译期耦合（**无 submodule、无 `#ifdef`、无 CMake option**）；图片绝不进二进制，「开关」=是否安装了该资源包。 |

---

## 一、拟新建仓清单（按优先级）

### 1. `aels-i18n`（最优先）

> **为什么独立**：CJK 字体子集化 + fontconfig fallback 是命脉级共用能力（Noto CJK 动辄 20MB+，超 i.MX6ULL 的 eMMC/RAM 预算），横切 CFDesktop + CCIMXDesktop + QuarkWidgets 三方。

**职责范围**:
- Qt i18n 流程：`tr()` 收词 → `lrelease` 生成 `.qm` → `QTranslator` 加载/语言切换 → 运行时切语言重刷
- locale 子系统：语言/区域/时区/数字日期格式（`QLocale` 接线）
- **CJK 字体子集化工具链**（`fonttools pyftsubset`）：按实际用字裁剪字体包
- fontconfig 精简配置 Profile + fallback 链

**接口契约（CFDesktop 消费侧）**:
- CFDesktop 只做设置 App 的「语言/字体」入口接线；引擎本体在本仓
- 桌面通过 MD3 typography token 的 fontFamily 查询消费字体（已在用）
- 消除 CCIMXDesktop ~45 处硬编码 CJK 字面量（迁移到 `tr()`）

**6ULL 取舍**：字体必须子集化；优先点阵/嵌入式字体；locale 切换零成本。

---

### 2. `aels-power-thermal`

> **为什么独立**：电源/热管理横跨 i.MX6ULL / i.MX8 / RK3399 / PC 多板，内核是「读 sysfs → profile 推理 → 写 sysfs」，与具体桌面壳解耦。塞进 CFDesktop 会把框架与板级 sysfs 绑死。

**职责范围**:
- 统一接口：`IPowerManager` / `IThermalManager` / `IBrightness`
- 策略链：按 profile（suspend / dim / 降频 / AC-电池分档）决策
- sysfs 后端实现：`/sys/class/backlight/`、`/sys/class/power_supply/`、`/sys/class/thermal/`、cpufreq governor

**接口契约（CFDesktop 消费侧）**:
- 控制中心亮度滑条 → `IBrightness::set(value)`
- 设置 App display 面板 → 背光/缩放
- 锁屏息屏超时 → `IPowerManager` 联动
- 会话电源动作（注销/重启/关机）执行层 → 本仓（Linux/嵌入式走 `reboot(2)`/`poweroff(2)` syscall + sync，**不依赖 systemd**）

**移植来源**：CCIMXDesktop `builtin/core/backlight/` 全家（`BacklightController` + `BacklightControllerImpl` 抽象基类 + `ArmPlatformBacklightController` sysfs 实现 + `PesudoLightController` 桩 + Singleton），几乎零改动提升为通用框架（剥离 `desktop_settings.h`/`ARM_BUILD` 宏依赖）。

---

### 3. `aels-network`

> **为什么独立**：WiFi 扫描/连接/热点需 wpa_supplicant / NetworkManager / connman 桥接，与桌面三层框架内核无关且横跨两板。真后端不该塞 shell（违反框架纯净），又比 utils 重。

**职责范围**:
- 统一接口：扫描 / 连接 / 断开 / 热点 / 状态 / IP-DNS 配置
- 平台分 backend：PC 走 NetworkManager（D-Bus）、6ULL 走 `wpa_cli` 命令行、有线静态配置

**接口契约（CFDesktop 消费侧）**:
- 状态栏 WiFi 角标 → 连通性三态（online/local/disconnected）
- 控制中心 WiFi 瓦片 → 开关 + 扫描列表
- 设置 App 网络面板 → 连接管理 / IP 配置

**移植来源**：CCIMXDesktop `netability_scanner`（连通性三态监测，6ULL 已实战验证）重构进状态子模块；诊断工具 NetHelper 作 extern 策展应用。wpa_supplicant/nm 桥接后端需新写。

**6ULL 取舍**：多为有线/静态 IP 工业场景，WiFi 非刚需；若需 WiFi 用 USB 模块 + `wpa_cli`；热点 NAT 转发在单核 528MHz 偏吃力，建议 P2/defer。

---

### 4. `aels-imagedocs`

> **为什么独立**：看图/画板/PDF 三应用与宿主桌面零耦合（仅 link Qt Core/Widgets），独立成仓既服务 CFDesktop 也给未来 Qt 桌面复用。

**职责范围**:
- 看图器（缩放/旋转/格式）
- 画板（`QGraphicsScene`）
- **mupdf-Qt 适配层**（`mupdf_adapter`，最有价值资产）+ PDF 阅读器

**接口契约（CFDesktop 消费侧）**:
- launcher 注册 + 文件管理器 MIME「打开方式」调用
- HWTier 友好渲染降级（大图走 PXP、PDF 单页渲染、预渲染当前页±1）

**移植来源**：CCIMXDesktop `builtin/app/ImageWalker`（看图）+ `builtin/app/SimpleDrawer`（画板）+ `extern_app/pdfReader`（含 `mupdf_adapter`），剥离 CCIMX 顶层 CMake，补 `cf::` 命名/导出。

**6ULL 取舍**：看图/画板纯软件渲染可行；PDF 软解单核 A7 较慢但可行（mupdf 比 poppler 轻）；需交叉编译 mupdf，256~512MB DDR 下限制并发渲染页数。

---

### 5. `aels-apprepo` + `aels-ota`

> **为什么独立**：应用商店前端 + manifest + 校验/安装/回滚，横跨两桌面+多板。OTA 的 A/B 双 rootfs 切换属板级 BSP，**extend 进 imx-forge/rk-forge**（同层），不塞桌面仓。

**`aels-apprepo` 职责**:
- 应用清单（manifest）协议与仓库元数据解析
- 下载（底层复用组织已有 **miniwget**）
- 校验/签名（sha256 / ed25519）
- 安装器（原子替换 / 依赖解析 / 回滚）
- 商店 UI（浏览/搜索/详情/进度，6ULL 列表+进度条，禁大图动效）

**`aels-ota` 职责**（调度层）:
- OTA 触发与回滚调度
- **双 rootfs A/B 切换 + bootloader 通信 → extend 进 imx-forge / rk-forge**（bootcount / upgrade_confirm），不重造板级基础设施

**接口契约（CFDesktop 消费侧）**:
- CFDesktop 只实现薄 `IAppRegistry` 接口对接（launcher 动态应用列表、安装事件）
- 包安装/OTA 完成事件 → 通知中心（走 IPC）

**6ULL 取舍**：应用商店可行（远程装/升级小工具是真实需求），走单 `.so`/包原子替换，不做 Flatpak 重栈；系统 OTA 走 U-Boot 双 rootfs（升级失败可回滚）；签名校验对工业设备可降级为受控局域源 + sha256。

---

## 二、跨域共享原语（非独立仓，归 CFDesktop core 或 aex）

| 原语 | 归属 | 消费方 |
|------|------|--------|
| `IHardware`（`ISensorReader` / `IBacklightController` / `IBatteryMonitor` / `IGpioPin` / `ISerialPort`） | **CFDesktop `base/`**（接口先行 + HWTier capability flag） | aels-power-thermal、aels-network 共享；shell 面向接口编程 |
| `ISecretStore`（加密键值存储，密钥派生 + 文件加密） | **utils/aex 或独立小库** | 锁屏凭证 / 未来密钥环 / 账户密码 / 网络密码档案——**别在锁屏域重造** |

> `IHardware` 接口在 CFDesktop core 定义，真实 sysfs/dev 实现可由 aels-power-thermal 等仓提供适配器。串口**工具本体**归 extern（见 [13_widget_apps.md](13_widget_apps.md) 补漏小节），`ISerialPort` 接口归 core。

---

## 三、依赖时序（推进顺序）

```
第 0 波（可见桌面骨架，不依赖任何新仓）
第 2 波 IPC 落地（见 06_infrastructure.md）
        ↓
第 5 波（嵌入式 pivot 前置）：
  aels-i18n（CJK 子集化）  ←─ 必须先于任何 CJK 文案应用移植
  aels-power-thermal       ←─ 接控制中心亮度/锁屏息屏/会话电源
  aels-network             ←─ 接状态栏 WiFi 角标/控制中心瓦片
        ↓
第 7 波（桌面本体稳定后）：
  aels-imagedocs（看图/画板/PDF）
  aels-apprepo + aels-ota（应用商店 + OTA A/B）
```

**铁律**：
- `aels-i18n` 的 CJK 字体子集化**必须先于**第 7 波的中文应用移植，否则字体一上就爆 i.MX6ULL 闪存。
- `IHardware` 接口要先于 aels-power-thermal / aels-network 设计，三者共享硬件能力模型。
- 所有新仓在 CFDesktop 侧只暴露薄接口，shell 不直接持有后端实现。

---

## 四、相关文档

- 总体规划：[summary.md](summary.md)
- 基础设施（IPC）：[06_infrastructure.md](06_infrastructure.md)
- 主题/设置/锁屏（消费 power/i18n）：[12_theme_settings_lockscreen.md](12_theme_settings_lockscreen.md)
- 通知/控制中心（消费 power/network）：[11_notification_control.md](11_notification_control.md)
- Widget/应用（消费 imagedocs/network）：[13_widget_apps.md](13_widget_apps.md)
- 四层性质分类法：[index.md](index.md)

---

*创建: 2026-06-29*
