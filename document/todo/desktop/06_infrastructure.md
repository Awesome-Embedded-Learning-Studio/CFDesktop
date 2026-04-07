# Phase 6: 基础设施补全 TODO

> **状态**: 🚧 部分完成 (~50%)
> **预计周期**: 4~5 周
> **依赖阶段**: Phase 1, Phase 2, Phase 3
> **已完成归档**: [done/06_infrastructure_status.md](../done/06_infrastructure_status.md)

---

## 已完成模块

> GPU 检测器、Network 检测器、ConfigStore、Logger 已完成。
> 详细状态请参考: [done/06_infrastructure_status.md](../done/06_infrastructure_status.md)

---

## 待实现任务

### HWTier 分级系统

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

#### Day 1-2: 崩溃捕获核心
- [ ] 创建 CrashHandler 类
  - [ ] 单例模式
  - [ ] 应用启动时注册
- [ ] 实现信号捕获（Linux）
  - [ ] `SIGSEGV`（段错误）
  - [ ] `SIGABRT`（中止）
  - [ ] `SIGFPE`（浮点异常）
  - [ ] `SIGBUS`（总线错误）
  - [ ] `SIGILL`（非法指令）
- [ ] 实现异常捕获（Windows）
  - [ ] `SetUnhandledExceptionFilter`
  - [ ] `std::set_terminate`
- [ ] 实现堆栈回溯
  - [ ] Linux: `backtrace()` / `backtrace_symbols()`
  - [ ] Windows: `StackWalk64`
  - [ ] 符号解析（addr2line 集成）
- [ ] 编写单元测试

#### Day 3: 崩溃信息存储
- [ ] 定义 CrashReport 结构
  - [ ] `timestamp`（崩溃时间）
  - [ ] `processName`（进程名称）
  - [ ] `processId`（进程 ID）
  - [ ] `signal`（崩溃信号）
  - [ ] `stackTrace`（堆栈回溯）
  - [ ] `registers`（寄存器状态）
  - [ ] `lastLogs`（最近 50 条日志）
  - [ ] `systemInfo`（系统信息/HWTier）
- [ ] 实现崩溃报告存储
  - [ ] 存储路径 `~/.cache/CFDesktop/crashes/`
  - [ ] JSON 格式序列化
  - [ ] Mini dump 支持（Windows）
- [ ] 实现崩溃历史管理
  - [ ] 最多保留 20 个报告
  - [ ] 按时间排序
  - [ ] 清理过期报告（30 天）
- [ ] 编写单元测试

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

---

## 三、验收标准

### 功能验收
- [ ] GPU 检测在 Linux/Windows 均可正常工作
- [ ] HWTier 在目标板卡上正确判定档位
- [ ] ConfigStore 三层存储和变更监听正常
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
- [ ] `ui/desktop/infrastructure/gpu_detector.h`
- [ ] `ui/desktop/infrastructure/hw_tier.h`
- [ ] `ui/desktop/infrastructure/capability_policy.h`
- [ ] `ui/desktop/infrastructure/config_store.h`
- [ ] `ui/desktop/infrastructure/config_watcher.h`
- [ ] `ui/desktop/infrastructure/logger.h`
- [ ] `ui/desktop/infrastructure/log_message.h`
- [ ] `ui/desktop/infrastructure/log_sink.h`
- [ ] `ui/desktop/infrastructure/console_sink.h`
- [ ] `ui/desktop/infrastructure/file_sink.h`
- [ ] `ui/desktop/infrastructure/crash_handler.h`
- [ ] `ui/desktop/infrastructure/crash_report.h`
- [ ] `ui/desktop/infrastructure/crash_reporter.h`
- [ ] `ui/desktop/infrastructure/watchdog.h`
- [ ] `ui/desktop/infrastructure/ipc_message.h`
- [ ] `ui/desktop/infrastructure/ipc_client.h`
- [ ] `ui/desktop/infrastructure/ipc_server.h`
- [ ] `ui/desktop/infrastructure/service_locator.h`

### 源文件
- [ ] `src/desktop/infrastructure/gpu_detector.cpp`
- [ ] `src/desktop/infrastructure/hw_tier.cpp`
- [ ] `src/desktop/infrastructure/capability_policy.cpp`
- [ ] `src/desktop/infrastructure/config_store.cpp`
- [ ] `src/desktop/infrastructure/config_watcher.cpp`
- [ ] `src/desktop/infrastructure/logger.cpp`
- [ ] `src/desktop/infrastructure/console_sink.cpp`
- [ ] `src/desktop/infrastructure/file_sink.cpp`
- [ ] `src/desktop/infrastructure/crash_handler.cpp`
- [ ] `src/desktop/infrastructure/crash_report.cpp`
- [ ] `src/desktop/infrastructure/crash_reporter.cpp`
- [ ] `src/desktop/infrastructure/watchdog.cpp`
- [ ] `src/desktop/infrastructure/ipc_client.cpp`
- [ ] `src/desktop/infrastructure/ipc_server.cpp`
- [ ] `src/desktop/infrastructure/service_locator.cpp`

### 平台特定实现
- [ ] `src/desktop/infrastructure/platform/gpu_detector_linux.cpp`
- [ ] `src/desktop/infrastructure/platform/gpu_detector_windows.cpp`
- [ ] `src/desktop/infrastructure/platform/crash_handler_linux.cpp`
- [ ] `src/desktop/infrastructure/platform/crash_handler_windows.cpp`
- [ ] `src/desktop/infrastructure/platform/stack_trace_linux.cpp`
- [ ] `src/desktop/infrastructure/platform/stack_trace_windows.cpp`

### 独立程序
- [ ] `src/tools/crash_reporter/main.cpp` （崩溃报告弹窗程序）

### 测试文件
- [ ] `tests/unit/desktop/infrastructure/test_gpu_detector.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_hw_tier.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_capability_policy.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_config_store.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_logger.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_crash_handler.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_crash_report.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_watchdog.cpp`
- [ ] `tests/unit/desktop/infrastructure/test_ipc.cpp`
- [ ] `tests/integration/desktop/infrastructure/test_crash_recovery.cpp`

### Mock 数据
- [ ] `tests/mock/dri/card0_renderD128` （RK3568 GPU 信息）
- [ ] `tests/mock/dri/card0` （无 GPU 场景）

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase A 节
- 依赖: [硬件探针状态](../done/01_hardware_probe_status.md), [Base库状态](../done/02_base_library_status.md)
- CMake 配置: `../../cmake/` （参考现有模块集成方式）

---

*最后更新: 2026-03-12*
