---
title: "Phase 6: 基础设施补全 TODO"
description: "状态: 🚧 部分完成 (~50%)，预计周期: 4~5 周"
---

# Phase 6: 基础设施补全 TODO

> **状态**: 🚧 部分完成 (~50%)
> **预计周期**: 4~5 周
> **依赖阶段**: Phase 1, Phase 2（Phase 3 输入层**非硬依赖**——IPC/CrashHandler 不依赖输入抽象；输入层可见桌面闭环后按需推进，见 [current.md](../../status/current.md)）
> **已完成归档**: [done/SUMMARY.md)

---

## 已完成模块

> GPU 检测器、Network 检测器、ConfigStore、Logger 已完成。
> 详细状态请参考: [done/SUMMARY.md)

---

## 待实现任务

### HWTier 分级系统

> **状态订正（2026-06-26）**：HWTier **检测 + 评分 + 档位覆写已完成**（Phase 1，见 `base/system/hardware_tier/` 的 `IHardwareCollector/Scorer/Assessor/Policy`，以及 [`current.md`](../../status/current.md)）。下列 Day 1-2 任务实际已完成；**CapabilityPolicy 策略引擎**（Day 5，档位→动效/渲染/内存降级并接入 Shell）在 demo / 可见桌面路线**延后**（开发期按 High Tier），待 embedded/production pivot 再做。本节「必做」语义针对 embedded 上线，不阻塞当前 demo 路线。

#### Day 1-2: HWTier 分级系统
- [ ] 定义 HWTier 枚举
  - [ ] `enum class Tier { Low, Mid, High, Unknown }`
- [ ] 实现 TierCalculator 评分算法
  - [ ] CPU 评分规则（核心数/频率/架构）
  - [ ] GPU 评分规则（是否有GPU/OpenGL版本）
  - [ ] 内存评分规则（总量/可用量）
- [ ] 定义档位阈值配置
  - [ ] Low: 0-60 分（无GPU/<=512MB RAM）
  - [ ] Mid: 61-120 分（弱GPU/1-2GB RAM）
  - [ ] High: 121+ 分（独立GPU/>=4GB RAM）
- [ ] 实现档位覆写机制
  - [ ] 配置文件强制指定
  - [ ] 环境变量 `CFDESKTOP_HW_TIER`
- [ ] 验证各板卡档位判定
  - [ ] IMX6ULL → Low
  - [ ] RK3568 → Mid
  - [ ] RK3588 → High

#### Day 5: CapabilityPolicy 策略引擎
- [ ] 定义策略结构体
  - [ ] `AnimationPolicy`（动效开关/并发数）
  - [ ] `RenderingPolicy`（阴影/模糊/合成层数）
  - [ ] `MemoryPolicy`（缓存大小）
- [ ] 实现档位默认策略
  - [ ] Low 档策略（关闭动效/简化阴影）
  - [ ] Mid 档策略（基础动效/限制并发）
  - [ ] High 档策略（全量动效/模糊效果）
- [ ] 实现 CapabilityPolicy 单例
  - [ ] `currentTier()` 查询
  - [ ] `getAnimationPolicy()` 获取
  - [ ] `getRenderingPolicy()` 获取

---

### CrashHandler 崩溃处理与自动重启

> **Phase 1 已落地（2026-07-08，feat/shell-foundation）**：信号捕获 + async-signal-safe raw 快照 + 下次启动 finalize 成 JSON 报告（含 logger tail 的 lastLogs）+ 20 份保留。落 `base/crash_handler/`（STATIC lib `cfcrash`，不 link Qt），early_session `CrashHandlerStage`（Stage 4，Logger 之后）注册。**订正**：报告路径跟 logger 同根 `<exe_dir>/crashes/`（非 `~/.cache/...`）；lastLogs 不在 handler 内取（logger 异步线程，UB），改为 defer finalize。符号解析 / CrashReporter 弹窗 / Watchdog 属 Phase 2。

#### Day 1-2: 崩溃捕获核心 — ✅ Phase 1 完成（Linux）
- [x] 创建 CrashHandler 类（`base/crash_handler/include/cfcrash/crash_handler.h`）
  - [x] 单例模式（`CrashHandler::instance()`）
  - [x] 应用启动时注册（`CrashHandlerStage`，early_session Stage 4）
- [x] 实现信号捕获（Linux，`sigaction` + `sigaltstack` + `SA_ONSTACK`/`SA_RESETHAND`）
  - [x] `SIGSEGV`（段错误）
  - [x] `SIGABRT`（中止）
  - [x] `SIGFPE`（浮点异常）
  - [x] `SIGBUS`（总线错误）
  - [x] `SIGILL`（非法指令）
- [ ] 实现异常捕获（Windows）— Phase 1 占位 stub，完整版 Phase 2
  - [ ] `SetUnhandledExceptionFilter`
  - [ ] `std::set_terminate`
- [x] 实现堆栈回溯（Linux `backtrace()` 写**裸地址**到 `.pending`）
  - [ ] `backtrace_symbols()` + addr2line 符号解析 — **defer Phase 2**（handler 内非 async-signal-safe）
  - [ ] Windows: `StackWalk64` — Phase 2
- [x] 编写单元测试（`test/crash/` 4 测：report 序列化 / retention / finalize 组装 / fork+SIGSEGV 端到端）

#### Day 3: 崩溃信息存储 — ✅ Phase 1 完成
- [x] 定义 CrashReport 结构（`crash_report.h`：timestamp / pid / signal / signal_name / raw_frames / last_logs）
  - [ ] processName / registers / systemInfo（HWTier）字段 — Phase 2 增项
- [x] 实现崩溃报告存储
  - [x] 存储路径 `<exe_dir>/crashes/`（跟 logger 同根，**订正**早期 `~/.cache/...`）
  - [x] JSON 格式序列化（`CrashReport::toJson()`）
  - [ ] Mini dump 支持（Windows）— Phase 2
- [x] 实现崩溃历史管理
  - [x] 最多保留 20 个报告（`pruneReports`）
  - [x] 按 mtime 排序删最旧
  - [ ] 清理过期报告（30 天）— Phase 2（可选）
- [x] 编写单元测试

#### Day 4: CrashReporter 崩溃报告弹窗
- [ ] 创建 CrashReporter 独立进程
  - [ ] 轻量级 Qt 程序
  - [ ] 不依赖主进程
- [ ] 实现崩溃提示界面
  - [ ] 友好的错误提示
  - [ ] 崩溃时间/次数
  - [ ] 崩溃摘要
- [ ] 实现操作选项
  - [ ] "重新启动" 按钮
  - [ ] "查看详情" 按钮
  - [ ] "发送报告" 按钮（可选）
  - [ ] "关闭应用" 按钮
- [ ] 实现详情查看
  - [ ] 堆栈回溯显示
  - [ ] 系统信息显示
  - [ ] 复制到剪贴板
- [ ] 实现自动重启逻辑
  - [ ] 检测未处理的崩溃报告
  - [ ] 启动时显示 CrashReporter
  - [ ] 用户确认后自动重启主进程
- [ ] 实现报告发送（可选）
  - [ ] HTTP 上传接口
  - [ ] 配置服务器地址
  - [ ] 压缩报告数据
- [ ] 编写单元测试

#### Day 5: 自动重启与守护进程
- [ ] 实现 Watchdog 守护进程
  - [ ] 监控主进程健康
  - [ ] 心跳检测
  - [ ] 异常时重启
- [ ] 实现重启策略
  - [ ] 首次崩溃：立即重启
  - [ ] 连续崩溃：延迟重启（指数退避）
  - [ ] 连续 3 次崩溃：停止自动重启，仅显示报告
- [ ] 实现重启状态保持
  - [ ] 保存当前应用状态（可选）
  - [ ] 重启后恢复
- [ ] 实现崩溃统计
  - [ ] 记录崩溃频率
  - [ ] 检测崩溃模式
- [ ] 编写集成测试

---

### IPC 基础层

#### Day 1-2: 消息序列化
- [ ] 定义 IPC 消息格式
  - [ ] `IPCMessage` 基类
  - [ ] `type` 字段（消息类型）
  - [ ] `payload` 字段（JSON 序列化）
- [ ] 实现消息注册表
  - [ ] `IPCMessageRegistry` 单例
  - [ ] `register<MessageType>()` 注册
  - [ ] `create(type)` 工厂方法

#### Day 3-4: QLocalSocket 封装
- [ ] 创建 IPCClient 类
  - [ ] 连接管理
  - [ ] 异步发送/接收
  - [ ] 重连机制
- [ ] 创建 IPCServer 类
  - [ ] 监听指定 socket
  - [ ] 客户端连接管理
  - [ ] 消息路由
- [ ] 实现 ServiceLocator
  - [ ] 服务注册/发现
  - [ ] 服务健康检查

#### Day 5: IPC 测试
- [ ] 编写集成测试
  - [ ] 客户端-服务器通信测试
  - [ ] 多客户端并发测试
  - [ ] 服务发现测试

### 引导启动序列（补漏：原 Phase 6 未覆盖，2026-06-29）

> DAG Init Chain（开机有序装配）已落地，但**面向用户的"首次启动引导/向导"序列**未规划——从内核接管屏幕到 shell 就绪这段（early frame buffer 显示 / splash / 启动进度 / 品牌 OEM 标识 / **启动失败的诊断回退**到 recovery shell / 与 init 握手）。嵌入式开机体验高度依赖它。

- [ ] 早期帧缓冲显示（图形栈就绪前的最小画面）
- [ ] 启动进度 UI（复用 `desktop/ui/widget/init_session/` 的 `boot_progress_widget` / `simple_boot_widget`）
- [ ] 品牌 / OEM 标识展示
- [ ] 启动失败诊断回退（无图形时进 recovery / 文本提示）
- [ ] 与 systemd / init 握手（嵌入式 init 不一定是 systemd）

> 参照资产：CCIMXDesktop `app_wrapper` + `splash_window` + `pagesetuper` 的引导链范式。

---

## 三、验收标准

### 功能验收
- [ ] GPU 检测在 Linux/Windows 均可正常工作
- [ ] HWTier 在目标板卡上正确判定档位
- [ ] ConfigStore 四层存储和变更监听正常
- [ ] Logger 多 Sink 并发安全，日志轮转正常
- [ ] CrashHandler 可捕获所有常见崩溃信号
- [ ] CrashReporter 弹窗友好展示崩溃信息
- [ ] 自动重启功能在崩溃后正常工作
- [ ] 连续崩溃检测和防护正常
- [ ] IPC 可跨进程通信

### 性能验收
- [ ] 硬件检测耗时 < 500ms
- [ ] 配置读写延迟 < 1ms
- [ ] 日志写入性能影响 < 1%
- [ ] 崩溃捕获延迟 < 100ms
- [ ] 崩溃报告生成 < 1s
- [ ] 自动重启延迟 < 3s

### 代码质量
- [ ] 单元测试覆盖率 > 85%
- [ ] 符合项目代码规范
- [ ] API 文档完整

---

## 四、文件清单（待实现）

### 头文件
> 📌 **路径核对（2026-06-29）**：base 级基础设施落 `desktop/base/infrastructure/`（与 [CRASH_HANDLE_STEP.md](CRASH_HANDLE_STEP.md) 一致，符合三层架构，**不在 `ui/`**）。

**已完成（移出待实现清单，标实际位置）**：
- [x] `base/system/gpu/` — GPU 检测器（已完成）
- [x] `base/system/hardware_tier/` — HWTier 检测/评分/Assessor/Policy（已完成；仅 CapabilityPolicy 策略引擎延后，见下）
- [x] `desktop/base/config_manager/` — ConfigStore 4 层 + 变更监听（已完成）
- [x] `desktop/base/logger/` — Logger + log_message/log_sink/console_sink/file_sink（已完成）

**待实现（落 `desktop/base/infrastructure/`）**：
- [ ] `desktop/base/infrastructure/capability_policy.h`（CapabilityPolicy 策略引擎，demo 路线延后）
- [ ] `desktop/base/infrastructure/crash_handler.h`
- [ ] `desktop/base/infrastructure/crash_report.h`
- [ ] `desktop/base/infrastructure/crash_reporter.h`
- [ ] `desktop/base/infrastructure/watchdog.h`
- [ ] `desktop/base/infrastructure/ipc_message.h`
- [ ] `desktop/base/infrastructure/ipc_client.h`
- [ ] `desktop/base/infrastructure/ipc_server.h`
- [ ] `desktop/base/infrastructure/service_locator.h`

### 源文件
- [x] GPU/HWTier/ConfigStore/Logger 源文件已完成（见上述实际目录）
- [ ] `desktop/base/infrastructure/capability_policy.cpp`（延后）
- [ ] `desktop/base/infrastructure/crash_handler.cpp`
- [ ] `desktop/base/infrastructure/crash_report.cpp`
- [ ] `desktop/base/infrastructure/crash_reporter.cpp`
- [ ] `desktop/base/infrastructure/watchdog.cpp`
- [ ] `desktop/base/infrastructure/ipc_client.cpp`
- [ ] `desktop/base/infrastructure/ipc_server.cpp`
- [ ] `desktop/base/infrastructure/service_locator.cpp`

### 平台特定实现
- [x] GPU/HWTier/Logger 的平台实现已完成
- [ ] `desktop/base/infrastructure/platform/crash_handler_linux.cpp`
- [ ] `desktop/base/infrastructure/platform/crash_handler_windows.cpp`
- [ ] `desktop/base/infrastructure/platform/stack_trace_linux.cpp`
- [ ] `desktop/base/infrastructure/platform/stack_trace_windows.cpp`

### 独立程序
- [ ] `src/tools/crash_reporter/main.cpp` （崩溃报告弹窗程序）

### 测试文件
- [x] `test_gpu_detector` / `test_hw_tier` / `test_config_store` / `test_logger` — 已完成（见 [done/SUMMARY.md](../done/SUMMARY.md)）
- [ ] `tests/unit/desktop/infrastructure/test_capability_policy.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_crash_handler.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_crash_report.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_watchdog.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_ipc.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_service_locator.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_ipc.cpp`
- [ ] `tests/integration/desktop/infrastructure/test_crash_recovery.cpp`

### Mock 数据
- [ ] `tests/mock/dri/card0_renderD128` （RK3568 GPU 信息）
- [ ] `tests/mock/dri/card0` （无 GPU 场景）

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase A 节
- 依赖: [硬件探针状态](../done/SUMMARY.md), [Base库状态](../done/SUMMARY.md)
- CMake 配置: `../../cmake/` （参考现有模块集成方式）

---

*最后更新: 2026-03-12*
