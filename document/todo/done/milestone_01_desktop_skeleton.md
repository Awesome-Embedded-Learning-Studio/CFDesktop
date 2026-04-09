# Milestone 1: 桌面骨架可见

> **状态**: ✅ 已完成 (2026-04-09)
> **预计周期**: 1-2 天
> **前置依赖**: 无 (当前基础设施已就绪)
> **目标**: 运行后不再是空白，能看到一个带背景的全屏桌面窗口

---

## 一、阶段目标

启动 CFDesktop 后，ShellLayer 渲染出桌面背景（纯色或壁纸），PanelManager 的可用区域计算正确，桌面主 Widget 正确显示。

**完成标志**: `./scripts/run.sh` 启动后看到全屏非空白的桌面画面。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `CFDesktop` (主 QWidget) | `desktop/ui/CFDesktop.h` | ✅ 已创建，`register_desktop_resources()` 可注入 PanelManager 和 ShellLayer |
| `CFDesktopEntity` (生命周期) | `desktop/ui/CFDesktopEntity.h/.cpp` | ✅ 单例，`run_init()` 已接入 DisplayBackend |
| `ShellLayer` | `desktop/ui/components/ShellLayer.h` | ✅ 接口+声明完整，**缺 .cpp 实现文件** |
| `IShellLayer` | `desktop/ui/components/IShellLayer.h` | ✅ `setStrategy()` + `geometry()` 接口 |
| `IShellLayerStrategy` | `desktop/ui/components/IShellLayerStrategy.h` | ✅ `activate(layer, wm)` + `deactivate()` + `onGeometryChanged()` |
| `PanelManager` | `desktop/ui/components/PanelManager.h/.cpp` | ✅ 完整实现，`registerPanel()` / `availableGeometry()` |
| `IPanel` | `desktop/ui/components/IPanel.h` | ✅ `position()` / `priority()` / `preferredSize()` / `widget()` |
| `SimpleBootWidget` | `desktop/ui/widget/init_session/simple_boot_widget.h/.cpp` | ✅ 启动画面已完整 |

### 关键缺口

1. **ShellLayer.cpp 不存在** — 只有 `.h` 声明，没有实现
2. **CFDesktop 没有 paintEvent** — 桌面主窗口不会画任何东西
3. **没有壁纸/背景渲染逻辑** — IShellLayerStrategy 没有具体实现
4. **CFDesktopEntity::run_init() 没有创建 ShellLayer** — 只做了 DisplayBackend 初始化
5. **CFDesktop 没有被 show()** — 桌面主 Widget 可能从未显示

---

## 三、待实现任务

### Day 1: ShellLayer 实现与桌面背景渲染

#### Step 1: 创建 ShellLayer.cpp
- [x] 创建文件 `desktop/ui/components/ShellLayer.cpp`
- [x] 实现构造函数 `ShellLayer(QWidget* parent)`
  - 设置 `setAttribute(Qt::WA_OpaquePaintEvent)` 减少闪烁
  - 设置 `setAutoFillBackground(false)`
  - 接受 parent 的全尺寸 geometry
- [x] 实现 `setStrategy(std::unique_ptr<IShellLayerStrategy> strategy)`
  - 若已有旧 strategy，先调用 `deactivate()`
  - 存储新 strategy
  - 调用 `activate(this->GetWeak(), wm_weak)` 激活
- [x] 实现 `geometry()` — 返回 `QWidget::geometry()`
- [x] 实现 `onAvailableGeometryChanged(const QRect& rect)`
  - 调用 `setGeometry(rect)` 调整 ShellLayer 尺寸
  - 转发给 `strategy_->onGeometryChanged(rect)`

**参考已有接口**:
- `ShellLayer.h:35` — 类声明
- `IShellLayer.h:37` — 接口定义
- `IShellLayerStrategy.h:33` — 策略接口

#### Step 2: 创建 DefaultShellStrategy (桌面背景策略)
- [x] 创建文件 `desktop/ui/strategy/default_shell_strategy.h/.cpp`
- [x] 实现 `IShellLayerStrategy` 接口
- [x] `activate()` 中：
  - 持有 ShellLayer 的 weak 引用
  - 在 ShellLayer 上创建背景子 Widget 或直接绘制
- [x] `onGeometryChanged()` 中：
  - 更新背景区域尺寸
  - 触发重绘
- [x] `deactivate()` 中：清理资源

#### Step 3: 为 ShellLayer 添加背景绘制
- [x] 方案 A (推荐): 在 ShellLayer 的 `paintEvent()` 中绘制纯色背景
  ```cpp
  void ShellLayer::paintEvent(QPaintEvent*) {
      QPainter p(this);
      auto* theme = ThemeManager::instance().currentTheme();
      p.fillRect(rect(), theme->colorScheme().surface());
  }
  ```
- [x] 方案 B: 壁纸图片渲染
  - 在 `DefaultShellStrategy::activate()` 中加载壁纸 QPixmap
  - 提供 `WallpaperLayer` 子 Widget 或在 paintEvent 中 `drawPixmap()`
- [x] 选择配色：使用 ThemeManager 的 `colorScheme().surface()` 作为默认背景色

**可复用**: `ui/core/theme_manager.h` (ThemeManager 单例)、`ui/core/color_scheme.h` (颜色方案)

#### Step 4: 修改 CFDesktopEntity::run_init() 连接所有组件
- [x] 在 `CFDesktopEntity::run_init()` 中（文件 `desktop/ui/CFDesktopEntity.cpp:45`）:
  ```cpp
  // 1. 创建 PanelManager
  auto* panel_mgr = new PanelManager(desktop_entity_, desktop_entity_);

  // 2. 创建 ShellLayer
  auto* shell = new ShellLayer(desktop_entity_);

  // 3. 注入到 CFDesktop
  CFDesktop::InitResources res;
  res.panel_manager_ = panel_mgr;
  res.shell_layer_ = shell;
  desktop_entity_->register_desktop_resources(res);

  // 4. 设置 ShellLayer 策略
  shell->setStrategy(std::make_unique<DefaultShellStrategy>());

  // 5. 连接 PanelManager geometry 变化到 ShellLayer
  connect(panel_mgr, &PanelManager::availableGeometryChanged,
          shell, &ShellLayer::onAvailableGeometryChanged);

  // 6. 显示
  desktop_entity_->showFullScreen();
  ```

#### Step 5: 确保 CFDesktop 正确接收 geometry
- [x] 在 `CFDesktop::register_desktop_resources()` 中（文件 `desktop/ui/CFDesktop.cpp:21`）:
  - 验证 `panel_manager_` 和 `shell_layer_` 非空
  - 触发 PanelManager 初始布局计算

#### Step 6: 更新 CMakeLists.txt
- [x] 在 `desktop/ui/components/CMakeLists.txt` 中添加 `ShellLayer.cpp`
- [x] 如新建 strategy 目录，确保 CMake 能发现新文件

---

### Day 2: 验证与调试

#### Step 7: 构建并运行
- [x] `cmake --build build` 确认编译通过
- [x] 运行后验证：
  - [x] 桌面全屏显示
  - [x] 背景非空白（纯色或壁纸）
  - [x] 关闭后无 crash / 内存泄漏

#### Step 8: 调试布局
- [x] 在 PanelManager::relayout() 中添加 debug log 输出 `availableGeometry()`
- [x] 确认 ShellLayer 的 geometry 正确跟随屏幕大小
- [x] 确认后续注册 Panel 后 availableGeometry 会正确缩小

---

## 四、关键文件清单

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/CFDesktopEntity.cpp` | `run_init()` 中创建 ShellLayer、PanelManager、设置策略、show |
| `desktop/ui/components/CMakeLists.txt` | 添加 ShellLayer.cpp |

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/components/ShellLayer.cpp` | ShellLayer 实现 |
| `desktop/ui/strategy/default_shell_strategy.h` | 默认 Shell 策略头文件 |
| `desktop/ui/strategy/default_shell_strategy.cpp` | 默认 Shell 策略实现 |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `desktop/ui/CFDesktop.h:76-81` | `InitResources` 结构体 |
| `desktop/ui/CFDesktop.cpp:21-25` | `register_desktop_resources()` |
| `desktop/ui/components/PanelManager.h` | PanelManager 接口 |
| `desktop/ui/components/IShellLayerStrategy.h` | 策略接口 |
| `desktop/ui/widget/init_session/simple_boot_widget.cpp` | QPainter 绘制参考 |
| `ui/core/theme_manager.h` | ThemeManager 单例 |

---

## 五、验收标准

- [x] `./scripts/run.sh` 后看到全屏非空白桌面
- [x] 桌面背景色跟随 ThemeManager 的 surface 色值
- [x] PanelManager 的 `availableGeometry()` 返回正确值 (初始为全屏)
- [x] 后续注册 Panel 后 `availableGeometry()` 会自动缩小
- [x] 关闭后无内存泄漏

---

*最后更新: 2026-03-31*
