# CFDesktop 前期阶段设计文档总览

## 文档目录

本目录包含 CFDesktop 项目前期阶段的详细设计文档。

### 文档列表

| 文档 | 阶段 | 内容概要 |
|------|------|----------|
| [00_phase0_project_skeleton.md](00_phase0_project_skeleton.md) | Phase 0 | 工程骨架搭建 - CMake 构建系统、目录结构、CI/CD 配置 |
| [01_phase1_hardware_probe.md](01_phase1_hardware_probe.md) | Phase 1 | 硬件探针与能力分级 - CPU/GPU/内存检测、HWTier 档位判定 |
| [02_phase2_base_library.md](02_phase2_base_library.md) | Phase 2 | Base 库核心 - 主题引擎、动画管理、DPI 适配、配置中心、日志系统 |
| [03_phase3_input_layer.md](03_phase3_input_layer.md) | Phase 3 | 输入抽象层 - 触摸/按键/旋钮处理、手势识别、焦点导航 |
| [04_phase6_simulator.md](04_phase6_simulator.md) | Phase 6 | 多平台模拟器 - PC 端模拟器、设备外壳、触摸可视化、参数注入 |
| [05_phase8_testing.md](05_phase8_testing.md) | Phase 8 | 测试体系 - 单元测试、集成测试、UI 测试、CI/CD |

---

## 快速导航

### 按角色查找

#### 开发者
- **新手上路**: 从 [Phase 0](00_phase0_project_skeleton.md) 开始，了解项目结构
- **基础设施**: [Phase 1](01_phase1_hardware_probe.md) + [Phase 2](02_phase2_base_library.md)
- **UI 开发**: [Phase 3](03_phase3_input_layer.md)
- **调试工具**: [Phase 6](04_phase6_simulator.md)

#### 测试工程师
- **测试框架**: [Phase 8](05_phase8_testing.md)
- **单元测试**: 各 Phase 文档中的"单元测试"章节

#### 项目经理
- **时间线**: 查看 BLUEPRINT.md 中的里程碑时间线
- **任务分解**: 各 Phase 文档中的"详细任务清单"章节

### 按模块查找

| 模块 | 文档 |
|------|------|
| 目录结构 | [Phase 0 - 二、目录结构设计](00_phase0_project_skeleton.md#二目录结构设计) |
| 构建系统 | [Phase 0 - 三、CMake 构建系统设计](00_phase0_project_skeleton.md#三cmake-构建系统设计) |
| 硬件检测 | [Phase 1 - 五、检测逻辑详细设计](01_phase1_hardware_probe.md#五检测逻辑详细设计) |
| 主题引擎 | [Phase 2 - 三、主题引擎](02_phase2_base_library.md#三主题引擎-themeengine) |
| 动画管理 | [Phase 2 - 四、动画管理器](02_phase2_base_library.md#四动画管理器-animationmanager) |
| DPI 适配 | [Phase 2 - 五、DPI 管理器](02_phase2_base_library.md#五dpi-管理器-dpimanager) |
| 配置系统 | [Phase 2 - 六、配置中心](02_phase2_base_library.md#六配置中心-configstore) |
| 日志系统 | [Phase 2 - 七、日志系统](02_phase2_base_library.md#七日志系统-logger) |
| 输入处理 | [Phase 3](03_phase3_input_layer.md) |
| 模拟器 | [Phase 6](04_phase6_simulator.md) |
| 测试 | [Phase 8](05_phase8_testing.md) |

---

## 使用建议

### 阅读顺序
1. 先阅读 [BLUEPRINT.md](../../BLUEPRINT.md) 了解整体规划
2. 阅读 [Phase 0](00_phase0_project_skeleton.md) 了解工程结构
3. 根据开发任务阅读对应 Phase 文档

### 开发流程
1. **环境搭建**: 参考 Phase 0 创建项目骨架
2. **基础模块**: 按 Phase 1 → Phase 2 → Phase 3 顺序开发
3. **Shell UI**: 参考 BLUEPRINT.md Phase 4
4. **SDK 层**: 参考 BLUEPRINT.md Phase 5
5. **模拟器**: 参考 Phase 6
6. **测试**: 参考 Phase 8，贯穿全程

### 文档更新
- 设计文档会随开发进展更新
- 变更记录请查看各文档的版本信息
- 有问题请及时反馈

---

## 项目里程碑

| 里程碑 | 时间 | 交付物 |
|--------|------|--------|
| M0 | Week 2 | 工程骨架 + CI 跑通 |
| M1 | Week 5 | 硬件探针 + 三档能力分级 |
| M2 | Week 9 | Base 库 + 主题引擎 + 输入抽象 |
| M3 | Week 15 | Shell UI 主体可用 |
| M4 | Week 18 | SDK 导出 + 示例应用 |
| M5 | Week 21 | 模拟器可用 |
| M6 | Week 23 | 应用商店基础 + 完整 CI/CD |

---

## 联系方式

如有问题或建议，请通过以下方式联系：
- GitHub Issues: [项目 Issues 页面]
- 技术讨论: [项目 Discussions 页面]

---

*最后更新: 2026-02-20*
