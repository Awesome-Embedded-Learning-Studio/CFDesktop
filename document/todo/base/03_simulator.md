# Phase 6: 多平台模拟器 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 2~3 周
> **依赖阶段**: Phase 0, Phase 2, Phase 3
> **目标交付物**: 模拟器窗口、设备外壳、触摸可视化、档位选择器

---

## 一、阶段目标

### 核心目标
在 Windows/Ubuntu 上通过 Qt Creator 还原真实嵌入式设备的 UI 效果，实现"所见即所得"的开发体验。

### 具体交付物
- [ ] `SimulatorWindow` 模拟器主窗口
- [ ] `DeviceFrame` 设备外壳渲染
- [ ] `TouchVisualizer` 触摸可视化
- [ ] `HWTierSelector` 硬件档位选择器
- [ ] `ResolutionPreset` 分辨率预设管理
- [ ] 独立可执行文件 `cfdesktop-sim`

---

## 二、Week 1: 基础框架

### Day 1-2: 主窗口与设备外壳
- [ ] 创建 SimulatorWindow 类
  - [ ] QMainWindow 继承
  - [ ] 布局管理
  - [ ] 菜单栏
- [ ] 实现 DeviceFrame 基础绘制
  - [ ] QWidget 实现
  - [ ] 屏幕容器
  - [ ] 绘制事件
- [ ] 实现矢量外壳绘制
  - [ ] 圆角矩形
  - [ ] 阴影效果
  - [ ] 硬件按键
- [ ] 支持图片外壳
  - [ ] 图片加载
  - [ ] 缩放适配
  - [ ] 透明度支持

### Day 3: 设备配置
- [ ] 定义 DeviceProfile 结构
  - [ ] 屏幕参数
  - [ ] 外壳参数
  - [ ] 硬件能力
- [ ] 实现 JSON 序列化
  - [ ] `fromJson()`
  - [ ] `toJson()`
- [ ] 创建预设配置文件
  - [ ] `imx6ull-4.3.json`
  - [ ] `imx6ull-7.0.json`
  - [ ] `rk3568-7.0.json`
  - [ ] `rk3568-10.1.json`
  - [ ] `rk3588-10.1.json`
  - [ ] `generic-1080p.json`
- [ ] 实现配置加载
  - [ ] 目录扫描
  - [ ] 默认配置

### Day 4: 控制面板
- [ ] 创建 ControlPanel 类
  - [ ] QWidget 布局
  - [ ] 控件排列
- [ ] 实现设备选择器
  - [ ] QComboBox
  - [ ] 配置列表
- [ ] 实现分辨率选择器
  - [ ] 预设分辨率
  - [ ] 自定义分辨率
- [ ] 实现档位选择器
  - [ ] HWTierSelector
  - [ ] 单选按钮组

### Day 5: 集成测试
- [ ] 集成各模块
  - [ ] 信号连接
  - [ ] 状态同步
- [ ] 基本功能测试
  - [ ] 配置加载
  - [ ] 窗口显示
  - [ ] 控件响应

---

## 三、Week 2: 注入与模拟

### Day 1-2: DPI 注入
- [ ] 实现 DPI 注入器
  - [ ] DPIInjector 类
  - [ ] 注入接口
- [ ] 修改 DPIManager 支持注入
  - [ ] 添加注入检查
  - [ ] 优先级处理
- [ ] 测试不同分辨率
  - [ ] 480×272
  - [ ] 800×480
  - [ ] 1024×600
  - [ ] 1920×1200

### Day 3: 硬件 Mock
- [ ] 实现硬件信息 Mock
  - [ ] HardwareMock 类
  - [ ] 数据注入
- [ ] 修改 HardwareProbe 支持 Mock
  - [ ] 测试模式检测
  - [ ] Mock 数据优先
- [ ] 测试档位切换
  - [ ] Low → Mid → High
  - [ ] 策略验证

### Day 4: 输入模拟
- [ ] 实现输入模拟器
  - [ ] InputSimulator 类
  - [ ] 事件过滤器
- [ ] 鼠标转触摸
  - [ ] QTouchEvent 转换
  - [ ] 多点模拟
- [ ] 键盘转按键
  - [ ] Qt Key 映射
  - [ ] 动作触发
- [ ] 滚轮转旋钮
  - [ ] QWheelEvent 转换
  - [ ] 步数计算

### Day 5: 触摸可视化
- [ ] 实现 TouchVisualizer
  - [ ] QWidget 覆盖层
  - [ ] 绘制事件
- [ ] 涟漪动画
  - [ ] QPropertyAnimation
  - [ ] 透明度渐变
- [ ] 多点触摸支持
  - [ ] 触摸点列表
  - [ ] 独立动画

---

## 四、Week 3: 完善与优化

### Day 1-2: UI 完善
- [ ] 完善设备外壳
  - [ ] 更多预设图片
  - [ ] SVG 矢量支持
- [ ] 添加更多预设
  - [ ] 手机类型
  - [ ] 平板类型
  - [ ] 自定义尺寸
- [ ] 美化控制面板
  - [ ] 图标支持
  - [ ] 工具提示

### Day 3: 高级功能
- [ ] 截图功能
  - [ ] 整窗口截图
  - [ ] 仅内容区域
  - [ ] 保存对话框
- [ ] 录屏功能
  - ] GIF 录制
  - [ ] 帧率配置
- [ ] 性能监控
  - [ ] FPS 显示
  - [ ] 内存显示
  - [ ] CPU 占用

### Day 4: 文档
- [ ] 使用说明
  - [ ] 快速开始
  - [ ] 功能介绍
  - [ ] 常见问题
- [ ] API 文档
  - [ ] 类接口
  - [ ] 使用示例
- [ ] 示例代码
  - [ ] 加载 Shell UI
  - [ ] 自定义配置

### Day 5: 测试
- [ ] 跨平台测试
  - [ ] Windows 10/11
  - [ ] Ubuntu 20.04+
  - [ ] macOS 12+
- [ ] 性能测试
  - [ ] 启动时间
  - [ ] 帧率稳定性
- [ ] 修复 Bug
  - [ ] 已知问题
  - [ ] 边界情况

---

## 五、验收标准

### 功能验收
- [ ] 能正确显示各种分辨率
- [ ] 档位切换生效
- [ ] 触摸模拟正常
- [ ] 截图功能正常

### 性能验收
- [ ] 启动时间 < 2 秒
- [ ] 帧率稳定 60fps

### 兼容性验收
- [ ] Windows 10/11 正常运行
- [ ] Ubuntu 20.04+ 正常运行
- [ ] macOS 12+ 正常运行

---

## 六、文件清单

### 核心类
- [ ] `include/CFDesktop/Simulator/SimulatorWindow.h`
- [ ] `include/CFDesktop/Simulator/DeviceFrame.h`
- [ ] `include/CFDesktop/Simulator/DeviceProfile.h`
- [ ] `include/CFDesktop/Simulator/TouchVisualizer.h`
- [ ] `include/CFDesktop/Simulator/ControlPanel.h`
- [ ] `include/CFDesktop/Simulator/HWTierSelector.h`
- [ ] `include/CFDesktop/Simulator/ResolutionPreset.h`
- [ ] `include/CFDesktop/Simulator/SimulatorInjection.h`

### 实现
- [ ] `src/simulator/SimulatorWindow.cpp`
- [ ] `src/simulator/DeviceFrame.cpp`
- [ ] `src/simulator/TouchVisualizer.cpp`
- [ ] `src/simulator/ControlPanel.cpp`
- [ ] `src/simulator/HWTierSelector.cpp`
- [ ] `src/simulator/ResolutionPreset.cpp`

### 注入
- [ ] `src/simulator/injection/DPIInjector.cpp`
- [ ] `src/simulator/injection/HardwareMock.cpp`
- [ ] `src/simulator/injection/InputSimulator.cpp`

### 资源
- [ ] `assets/simulator/devices/phone-generic.png`
- [ ] `assets/simulator/devices/tablet-10inch.png`
- [ ] `assets/simulator/devices/panel-7inch.png`
- [ ] `assets/simulator/devices/panel-10inch.png`
- [ ] `assets/simulator/profiles/imx6ull-4.3.json`
- [ ] `assets/simulator/profiles/imx6ull-7.0.json`
- [ ] `assets/simulator/profiles/rk3568-7.0.json`
- [ ] `assets/simulator/profiles/rk3568-10.1.json`
- [ ] `assets/simulator/profiles/rk3588-10.1.json`
- [ ] `assets/simulator/profiles/generic-1080p.json`
- [ ] `assets/simulator/effects/touch-ripple.png`

### 测试
- [ ] `tests/unit/simulator/test_device_profile.cpp`
- [ ] `tests/unit/simulator/test_device_frame.cpp`
- [ ] `tests/unit/simulator/test_touch_visualizer.cpp`

---

## 七、相关文档

- 设计文档: [../../design_stage/04_phase6_simulator.md](../../design_stage/04_phase6_simulator.md)
- 依赖: [工程骨架状态](../done/00_project_skeleton_status.md), [02_input_layer.md](02_input_layer.md)

---

*最后更新: 2026-03-05*
