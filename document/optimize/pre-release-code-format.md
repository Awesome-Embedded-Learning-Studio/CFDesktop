# CFDesktop 性能优化和架构优化计划

## Context

CFDesktop 是一个基于 Qt6 和 C++17 的桌面应用程序框架，实现了完整的 Material Design 3 UI 组件库。项目采用模块化架构设计，代码质量较高，但仍存在一些性能和架构层面的优化空间。

本计划旨在在 pre-release 阶段对项目进行系统的性能和架构优化，提升代码质量、运行效率和可维护性。

---

## 当前架构概览

```
CFDesktop/
├── base/                    # 基础库模块
│   ├── include/            # 头文件（expected、WeakPtr、哈希等）
│   └── system/             # 系统信息抽象层（CPU/Memory）
├── ui/                     # UI框架模块
│   ├── base/              # 基础工具（颜色、几何、缓动）
│   ├── core/              # 核心引擎（主题、动效、令牌）
│   ├── components/        # 通用组件（动画、状态机）
│   └── widget/            # Material控件（按钮、文本框等）
├── example/               # 示例程序
├── test/                  # GoogleTest单元测试
└── scripts/               # 构建和工具脚本
```

**技术栈：** Qt6 + C++17 + CMake + GoogleTest

---

## 优化计划

### 一、性能优化

#### 1.1 系统信息查询优化

**问题文件：**
- `base/system/memory/private/linux_impl/cached_memory.cpp`
- `base/system/cpu/private/linux_impl/cpu_profile.cpp`
- `base/system/memory/private/win_impl/process_memory.cpp`

**问题分析：**
- Linux平台频繁读取 `/proc/meminfo`、`/proc/stat` 等系统文件
- CPU使用率计算需要阻塞100ms进行两次采样
- 缓存机制存在但缺乏过期策略

**优化方案：**
1. 实现智能缓存策略（TTL过期 + LRU淘汰）
2. 将CPU使用率计算移到后台线程
3. 使用 `QTimer` 替代 `std::this_thread::sleep_for`
4. 添加查询频率限制机制

**预期收益：** 减少系统调用次数，降低CPU占用

---

#### 1.2 动画系统性能优化

**问题文件：**
- `ui/widget/material/base/ripple_helper.cpp`
- `ui/widget/material/base/state_machine.cpp`
- `ui/components/animation_factory_manager.h`

**问题分析：**
- 波纹效果每次触发都遍历所有波纹实例
- 状态变更频繁触发UI重绘
- 动画工厂缓存可能导致内存占用增加

**优化方案：**
1. 实现脏矩形机制，只重绘变化区域
2. 批量更新波纹状态，减少重绘次数
3. 使用 `QTimer::singleShot` 合并高频更新
4. 添加动画实例缓存上限

**预期收益：** 减少UI重绘开销，提升动画流畅度

---

#### 1.3 内存管理优化

**问题文件：**
- `ui/widget/material/base/state_machine.cpp`
- `base/include/base/weak_ptr/weak_ptr.h`

**问题分析：**
- 动画信号槽连接可能在对象销毁时未正确断开
- 定时器可能未在对象销毁时清除
- WeakPtr 生命周期管理需要加强

**优化方案：**
1. 在析构函数中确保所有动画正确清理
2. 使用 RAII 模式管理定时器和信号连接
3. 添加内存使用统计和分析工具
4. 考虑实现对象池减少频繁创建销毁

**预期收益：** 减少内存泄漏风险，稳定内存占用

---

### 二、架构优化

#### 2.1 模块依赖优化

**当前状态：** 依赖关系清晰 (`example → ui → base`)

**优化方案：**
1. 进一步减少头文件依赖，使用前向声明
2. 将 Pimpl 惯用法应用到更多组件
3. 评估 `ui/widget` 和 `ui/components` 的边界是否清晰

**预期收益：** 减少编译时间，提高模块独立性

---

#### 2.2 错误处理标准化

**当前状态：** 使用 `cf::expected<T, E>` 进行错误处理

**优化方案：**
1. 建立统一的错误码体系
2. 为 `expected` 添加更多实用方法
3. 考虑添加错误上下文追踪功能

**预期收益：** 提高错误处理一致性和调试效率

---

#### 2.3 Scripts 脚本重构

**问题目录：** `scripts/`

**问题分析：**
- scripts 目录下存在大量重复代码
- 缺乏统一的脚本框架和工具函数库
- 各脚本之间复用性差

**优化方案：**
1. 提取公共脚本函数库（`scripts/lib/`）
2. 统一脚本参数解析方式
3. 规范化脚本输出格式和日志
4. 添加脚本单元测试

**预期收益：** 减少代码重复，提高脚本可维护性

---

#### 2.4 测试覆盖率提升

**当前状态：** 有 GoogleTest 框架，但测试覆盖不足

**优化方案：**
1. 添加代码覆盖率统计工具（如 gcov/lcov）
2. 为核心组件补充边界条件测试
3. 添加性能基准测试
4. 集成静态分析工具（clang-tidy）
5. 补充 widgets 和 components 的交互测试
6. 添加跨平台兼容性测试用例

**预期收益：** 提高代码质量和可靠性，确保跨平台兼容

---

### 三、构建和发布优化

#### 3.1 构建配置优化

**问题分析：** Debug 构建使用 `-O0`，开发体验较差

**优化方案：**
1. 添加 `DebugOptimized` 构建类型（`-Og` 优化级别）
2. 优化链接器参数

---

#### 3.2 CI/CD 增强

**优化方案：**
1. 添加性能回归检测
2. 集成静态分析和代码质量检查
3. 自动化发布流程

---

## 优先级排序

| 优先级 | 优化项 | 预期工作量 | 影响范围 |
|--------|--------|-----------|----------|
| P0 | 系统信息查询缓存优化 | 中 | 全局性能 |
| P0 | 动画系统脏矩形机制 | 高 | UI性能 |
| P0 | 测试覆盖率提升 | 高 | 可靠性 |
| P1 | 内存泄漏风险修复 | 中 | 稳定性 |
| P1 | Scripts 脚本重构 | 中 | 可维护性 |
| P2 | 错误处理标准化 | 低 | 代码质量 |
| P3 | 构建配置优化 | 低 | 开发体验 |

---

## 关键文件清单

### 需要修改的文件

**性能优化：**
- `base/system/memory/private/linux_impl/cached_memory.cpp` - 缓存策略
- `base/system/cpu/private/linux_impl/cpu_profile.cpp` - CPU查询异步化
- `ui/widget/material/base/ripple_helper.cpp` - 波纹批量更新
- `ui/widget/material/base/state_machine.cpp` - 状态更新合并

**架构优化：**
- `base/include/base/expected/expected.hpp` - 错误处理增强
- `ui/core/theme_manager.cpp` - 单例模式优化
- 各组件头文件 - 前向声明优化

**Scripts 重构：**
- `scripts/` 下所有脚本文件 - 提取公共函数库

### 需要新增的文件

- `base/include/base/cache/lru_cache.h` - LRU缓存实现
- `ui/base/paint_optimizer.h` - 绘制优化器
- `test/performance/` - 性能基准测试目录

---

## 验证计划

1. **编译验证：** 确保所有构建类型编译通过
2. **单元测试：** 运行完整测试套件，确保无回归
3. **性能测试：** 运行基准测试，对比优化前后数据
4. **内存检测：** 使用 Valgrind/ASan 检测内存问题
5. **UI测试：** 手动验证动画流畅度和响应速度

---

## 实施建议

1. 分阶段实施，优先完成 P0 级别优化
2. 每项优化后进行测试验证
3. 保持向后兼容性，避免破坏现有API
4. 及时更新相关文档和示例
