# Phase 8: 测试体系详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 8 - 测试体系 |
| 预计周期 | 贯穿全程 |
| 依赖阶段 | 所有阶段 |

---

## 一、阶段目标

### 1.1 核心目标
建立完整的测试体系，确保代码质量和系统稳定性。测试体系覆盖单元测试、集成测试、UI 自动化测试和跨平台 CI。

### 1.2 具体交付物
- [ ] 单元测试框架和测试用例
- [ ] 集成测试套件
- [ ] UI 自动化测试
- [ ] CI/CD 流水线配置
- [ ] 测试覆盖率报告
- [ ] 性能测试基准

---

## 二、测试体系架构

### 2.1 测试金字塔

```
                    /\
                   /  \
                  / E2E \
                 / 测试  \
                /________\
               /          \
              /            \
             /  UI 自动化   \
            /    测试        \
           /__________________\
          /                    \
         /     集成测试          \
        /________________________\
       /                          \
      /      单元测试              \
     /______________________________\
    /       70% 单元                \
   /        20% 集成                 \
  /         10% UI                    \
 /____________________________________\
```

### 2.2 测试目录结构

```
tests/
├── unit/                           # 单元测试
│   ├── base/
│   │   ├── test_hardware_probe.cpp
│   │   ├── test_theme_engine.cpp
│   │   ├── test_animation_manager.cpp
│   │   ├── test_dpi_manager.cpp
│   │   ├── test_config_store.cpp
│   │   └── test_logger.cpp
│   │
│   ├── sdk/
│   │   ├── test_widgets.cpp
│   │   ├── test_system_api.cpp
│   │   └── test_app_lifecycle.cpp
│   │
│   └── shell/
│       ├── test_launcher.cpp
│       ├── test_window_manager.cpp
│       └── test_status_bar.cpp
│
├── integration/                    # 集成测试
│   ├── test_base_integration.cpp
│   ├── test_sdk_integration.cpp
│   └── test_shell_integration.cpp
│
├── ui/                             # UI 自动化测试
│   ├── test_launcher_ui.cpp
│   ├── test_window_manager_ui.cpp
│   └── test_app_switching.cpp
│
├── performance/                    # 性能测试
│   ├── test_startup_time.cpp
│   ├── test_animation_fps.cpp
│   ├── test_memory_usage.cpp
│   └── benchmarks/
│       ├── benchmark_theme_load.cpp
│       └── benchmark_input_processing.cpp
│
├── mock/                           # Mock 数据
│   ├── proc/
│   │   ├── cpuinfo_imx6ull
│   │   ├── cpuinfo_rk3568
│   │   ├── meminfo_512mb
│   │   └── meminfo_1gb
│   └── devices/
│       └── dri/
│           └── card0
│
├── fixtures/                       # 测试夹具
│   ├── test_app.h
│   ├── test_widget.h
│   └── test_theme.h
│
└── CMakeLists.txt
```

---

## 三、单元测试

### 3.1 测试框架配置

**文件**: `tests/CMakeLists.txt`

```cmake
# 启用测试
enable_testing()

# Qt6 Test 包
find_package(Qt6 REQUIRED COMPONENTS Test)

# 添加测试子目录
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(ui)
add_subdirectory(performance)
```

### 3.2 基础测试模板

```cpp
// tests/unit/base/test_hardware_probe.cpp

#include <QtTest>
#include <CFDesktop/Base/HardwareProbe/HardwareProbe.h>

class TestHardwareProbe : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();        // 测试套件开始前执行一次
    void cleanupTestCase();     // 测试套件结束后执行一次
    void init();                // 每个测试用例前执行
    void cleanup();             // 每个测试用例后执行

    // CPU 检测测试
    void testDetectCPU_IMX6ULL();
    void testDetectCPU_RK3568();
    void testDetectCPU_RK3588();
    void testDetectCPU_X86_64();

    // 档位计算测试
    void testCalculateTier_LowEnd();
    void testCalculateTier_MidRange();
    void testCalculateTier_HighEnd();

    // Mock 数据测试
    void testWithMockData();
    void testWithInvalidData();
};

void TestHardwareProbe::initTestCase() {
    // 设置测试环境
    qputenv("CFDESKTOP_TEST_MODE", "1");
}

void TestHardwareProbe::testDetectCPU_IMX6ULL() {
    // 设置 Mock 数据路径
    qputenv("CFDESKTOP_MOCK_CPUINFO",
            "/path/to/mock/proc/cpuinfo_imx6ull");

    HardwareProbe probe;
    HardwareInfo info = probe.probe();

    // 验证 CPU 信息
    QCOMPARE(info.cpu.cores, 1);
    QCOMPARE(info.cpu.architecture, QString("armv7l"));
    QVERIFY(info.cpu.features.contains("neon"));

    // 验证档位
    QCOMPARE(info.tier, HWTier::Low);
}

QTEST_MAIN(TestHardwareProbe)
#include "test_hardware_probe.moc"
```

### 3.3 Mock 系统设计

```cpp
// tests/fixtures/test_helpers.h

namespace CFDesktop::Testing {

/**
 * @brief Mock 系统调用
 */
class MockSystem {
public:
    /**
     * @brief Mock /proc/cpuinfo
     */
    static void mockCpuinfo(const QString& mockFile);

    /**
     * @brief Mock /proc/meminfo
     */
    static void mockMeminfo(const QString& mockFile);

    /**
     * @brief Mock /sys/class/net/*
     */
    static void mockNetwork(const QList<NetworkInterface>& interfaces);

    /**
     * @brief 清除所有 Mock
     */
    static void clearAll();

    /**
     * @brief 设置模拟屏幕参数
     */
    static void mockScreen(int dpi, qreal dpr, const QSize& size);
};

} // namespace CFDesktop::Testing
```

### 3.4 关键单元测试用例清单

#### HardwareProbe 测试用例

| 测试用例 | 描述 | 预期结果 |
|----------|------|----------|
| `testDetectCPU_IMX6ULL` | IMX6ULL CPU 检测 | 识别为 Low 档 |
| `testDetectCPU_RK3568` | RK3568 CPU 检测 | 识别为 Mid 档 |
| `testDetectCPU_RK3588` | RK3588 CPU 检测 | 识别为 High 档 |
| `testDetectGPU_WithDRM` | DRM 设备检测 | 正确检测 GPU |
| `testDetectGPU_NoDRM` | 无 DRM 设备 | 返回无 GPU |
| `testDetectMemory_512MB` | 512MB 内存检测 | 正确报告内存 |
| `testCalculateTier_ScoreBased` | 评分计算 | 档位正确 |
| `testUserOverride` | 用户配置覆盖 | 使用用户档位 |
| `testCustomScript` | 自定义脚本执行 | 脚本结果生效 |

#### ThemeEngine 测试用例

| 测试用例 | 描述 | 预期结果 |
|----------|------|----------|
| `testLoadDefaultTheme` | 加载默认主题 | 成功加载 |
| `testThemeVariableAccess` | 变量访问 | 返回正确值 |
| `testThemeInheritance` | 主题继承 | 继承生效 |
| `testQSSProcessing` | QSS 变量替换 | 变量被替换 |
| `testThemeHotReload` | 热重载 | 实时生效 |
| `testTierBasedFallback` | 档位降级 | Low 档禁用特效 |

#### AnimationManager 测试用例

| 测试用例 | 描述 | 预期结果 |
|----------|------|----------|
| `testFadeInAnimation` | 淡入动画 | 正常播放 |
| `testLowTierNoAnimation` | Low 档 | 动画时长为 0 |
| `testParallelGroup` | 并行动画组 | 同时播放 |
| `testSequentialGroup` | 串行动画组 | 顺序播放 |
| `testAnimationLifecycle` | 动画生命周期 | 状态正确 |

#### DPIManager 测试用例

| 测试用例 | 描述 | 预期结果 |
|----------|------|----------|
| `testDPConversion` | dp 转 px | 转换正确 |
| `testSPConversion` | sp 转 px | 考虑字体缩放 |
| `testInjection` | 参数注入 | 注入值生效 |
| `testHighDPI` | 高 DPI | 缩放正确 |

---

## 四、集成测试

### 4.1 集成测试框架

```cpp
// tests/integration/test_base_integration.cpp

#include <QtTest>
#include <CFDesktop/Base/HardwareProbe/HardwareProbe.h>
#include <CFDesktop/Base/ThemeEngine/ThemeEngine.h>
#include <CFDesktop/Base/AnimationManager/AnimationManager.h>

class TestBaseIntegration : public QObject {
    Q_OBJECT

private slots:
    void testHardwareProbeToTheme();
    void testHardwareProbeToAnimation();
    void testThemeToWidget();
    void testFullBaseStack();
};

void TestBaseIntegration::testHardwareProbeToTheme() {
    // 1. 硬件检测
    HardwareProbe probe;
    probe.setMockData(createLowEndMockInfo());
    HardwareInfo info = probe.probe();

    // 2. 主题引擎应响应档位
    ThemeEngine* theme = ThemeEngine::instance();
    theme->loadTheme("/path/to/default/theme");
    QColor shadowColor = theme->color("shadow");

    // Low 档应该禁用阴影 (透明或无色)
    if (info.tier == HWTier::Low) {
        QVERIFY(!shadowColor.alpha() > 0);
    }
}

void TestBaseIntegration::testFullBaseStack() {
    // 完整的 Base 库集成测试
    // 模拟真实启动流程
}
```

### 4.2 测试应用框架

```cpp
// tests/fixtures/test_app.h

namespace CFDesktop::Testing {

/**
 * @brief 测试用应用程序
 *
 * 提供独立的测试环境，不影响主应用。
 */
class TestApplication : public QApplication {
public:
    TestApplication(int& argc, char** argv);

    /**
     * @brief 使用测试配置初始化
     */
    void initializeWithTestConfig();

    /**
     * @brief 创建测试窗口
     */
    QWidget* createTestWindow();

    /**
     * @brief 模拟用户输入
     */
    void simulateKeyPress(int key);
    void simulateMouseClick(const QPoint& pos);
    void simulateTouch(const QPointF& pos);

    /**
     * @brief 等待条件满足
     */
    bool waitFor(std::function<bool()> condition, int timeout = 5000);

private:
    void setupTestEnvironment();
    void cleanupTestEnvironment();
};

} // namespace CFDesktop::Testing
```

---

## 五、UI 自动化测试

### 5.1 QTest UI 测试

```cpp
// tests/ui/test_launcher_ui.cpp

#include <QtTest>
#include <CFDesktop/Shell/Launcher/LauncherWindow.h>

class TestLauncherUI : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testLauncherDisplay();
    void testAppIconClick();
    void testSwipeGesture();
    void testAppLaunch();

private:
    LauncherWindow* m_launcher;
};

void TestLauncherUI::testAppIconClick() {
    // 查找应用图标
    QList<AppIconWidget*> icons = m_launcher->findChildren<AppIconWidget*>();
    QVERIFY(icons.size() > 0);

    AppIconWidget* firstIcon = icons.first();

    // 模拟点击
    QTest::mouseClick(firstIcon, Qt::LeftButton);

    // 验证应用启动
    QSignalSpy spy(firstIcon, &AppIconWidget::launched);
    QVERIFY(spy.wait(1000));
}

void TestLauncherUI::testSwipeGesture() {
    // 模拟滑动手势
    QWidget* viewport = m_launcher->viewport();

    QPoint startPos(400, 240);
    QPoint endPos(100, 240);

    QTest::mousePress(viewport, Qt::LeftButton, startPos);
    QTest::mouseMove(viewport, endPos);
    QTest::mouseRelease(viewport, Qt::LeftButton, endPos);

    // 验证页面切换
    QTest::qWait(500);
    QCOMPARE(m_launcher->currentPage(), 1);
}
```

### 5.2 可访问性测试

```cpp
void TestAccessibility::testWidgetNavigation() {
    // 验证焦点导航
    QWidget* widget1 = m_window->findChild<QWidget*>("widget1");
    QWidget* widget2 = m_window->findChild<QWidget*>("widget2");

    widget1->setFocus();
    QVERIFY(widget1->hasFocus());

    // 模拟 Tab 键
    QTest::keyClick(m_window, Qt::Key_Tab);
    QVERIFY(widget2->hasFocus());
}

void TestAccessibility::testScreenReader() {
    // 验证可访问性接口
    QAccessibleInterface* iface = QAccessible::queryAccessibleInterface(m_button);

    QCOMPARE(iface->role(), QAccessible::Role::PushButton);
    QVERIFY(!iface->text(QAccessible::Name).isEmpty());
}
```

---

## 六、性能测试

### 6.1 基准测试框架

```cpp
// tests/performance/benchmarks/benchmark_theme_load.cpp

#include <QtTest>
#include <CFDesktop/Base/ThemeEngine/ThemeEngine.h>
#include <QElapsedTimer>

class BenchmarkThemeLoad : public QObject {
    Q_OBJECT

private slots:
    void testThemeLoadPerformance();
    void testVariableLookupPerformance();
    void testQSSProcessingPerformance();
};

void BenchmarkThemeLoad::testThemeLoadPerformance() {
    ThemeEngine* engine = ThemeEngine::instance();
    QString themePath = "/path/to/default/theme";

    QElapsedTimer timer;
    timer.start();

    engine->loadTheme(themePath);

    qint64 elapsed = timer.elapsed();

    // 加载时间应 < 100ms
    QVERIFY(elapsed < 100);

    qDebug() << "Theme load time:" << elapsed << "ms";
}

void BenchmarkThemeLoad::testVariableLookupPerformance() {
    ThemeEngine* engine = ThemeEngine::instance();
    engine->loadTheme("/path/to/default/theme");

    const int iterations = 10000;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
        engine->color("primary");
        engine->size("normal");
        engine->font("body");
    }

    qint64 elapsed = timer.elapsed();
    qreal avgTime = static_cast<qreal>(elapsed) / iterations;

    // 平均查找时间应 < 0.01ms
    QVERIFY(avgTime < 0.01);

    qDebug() << "Average variable lookup time:" << avgTime * 1000 << "μs";
}
```

### 6.2 内存使用测试

```cpp
// tests/performance/test_memory_usage.cpp

class TestMemoryUsage : public QObject {
    Q_OBJECT

private slots:
    void testThemeEngineMemory();
    void testAnimationManagerMemory();
    void testMultipleWindowsMemory();

private:
    qint64 getCurrentMemoryUsage();
};

void TestMemoryUsage::testThemeEngineMemory() {
    qint64 before = getCurrentMemoryUsage();

    ThemeEngine* engine = ThemeEngine::instance();
    engine->loadTheme("/path/to/default/theme");

    qint64 after = getCurrentMemoryUsage();
    qint64 used = after - before;

    // 内存使用应 < 50MB
    QVERIFY(used < 50 * 1024 * 1024);

    qDebug() << "Theme engine memory usage:" << used / 1024 << "KB";
}

qint64 TestMemoryUsage::getCurrentMemoryUsage() {
    // Linux: 读取 /proc/self/status
    QFile file("/proc/self/status");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("VmRSS:")) {
                QString value = line.section(':', 1).section('k', 0, 0).trimmed();
                return value.toLongLong() * 1024;
            }
        }
    }
    return -1;
}
```

### 6.3 性能基准

| 模块 | 指标 | 目标值 | 测试方法 |
|------|------|--------|----------|
| 主题加载 | 加载时间 | < 100ms | 计时测试 |
| 变量查询 | 平均时间 | < 0.01ms | 循环测试 |
| 动画启动 | 启动延迟 | < 16ms | 计时测试 |
| 动画帧率 | 稳定性 | 60fps | 帧率测试 |
| 输入响应 | 延迟 | < 16ms | 事件测试 |
| 内存使用 | 基础占用 | < 100MB | 内存测试 |
| 启动时间 | 冷启动 | < 2s | 计时测试 |

---

## 七、CI/CD 配置

### 7.1 GitHub Actions 配置

**文件**: `.github/workflows/test.yml`

```yaml
name: Test Suite

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    container: ghcr.io/cfdesktop/builder-linux:latest

    steps:
      - uses: actions/checkout@v3

      - name: Configure
        run: |
          cmake -B build \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DENABLE_COVERAGE=ON

      - name: Build
        run: cmake --build build --parallel

      - name: Run Unit Tests
        run: |
          cd build
          ctest --output-on-failure \
            --rerun-failed \
            -j$(nproc)

      - name: Generate Coverage
        run: |
          cd build
          gcovr --xml-pretty --exclude-unreachable-branches \
            --print-summary -o coverage.xml

      - name: Upload Coverage
        uses: codecov/codecov-action@v3
        with:
          files: ./build/coverage.xml

  integration-tests:
    runs-on: ubuntu-latest
    needs: unit-tests

    steps:
      - uses: actions/checkout@v3

      - name: Build
        run: |
          cmake -B build -DBUILD_TESTING=ON
          cmake --build build

      - name: Run Integration Tests
        run: |
          cd build
          ctest -R integration --output-on-failure

  ui-tests:
    runs-on: ubuntu-latest
    needs: unit-tests

    steps:
      - uses: actions/checkout@v3

      - name: Build
        run: |
          cmake -B build -DBUILD_TESTING=ON
          cmake --build build

      - name: Setup Virtual Display
        run: |
          Xvfb :99 -screen 0 1024x768x24 &
          export DISPLAY=:99

      - name: Run UI Tests
        run: |
          cd build
          ctest -R ui --output-on-failure

  performance-tests:
    runs-on: ubuntu-latest
    needs: unit-tests

    steps:
      - uses: actions/checkout@v3

      - name: Build
        run: |
          cmake -B build -DBUILD_TESTING=ON
          cmake --build build --release

      - name: Run Performance Tests
        run: |
          cd build
          ctest -R performance --output-on-failure

      - name: Upload Results
        uses: actions/upload-artifact@v3
        with:
          name: performance-results
          path: build/testing/performance/
```

### 7.2 代码覆盖率要求

| 模块 | 最低覆盖率 | 目标覆盖率 |
|------|-----------|-----------|
| HardwareProbe | 90% | 95% |
| ThemeEngine | 85% | 90% |
| AnimationManager | 85% | 90% |
| DPIManager | 85% | 90% |
| ConfigStore | 90% | 95% |
| Logger | 85% | 90% |
| InputManager | 80% | 85% |
| **整体** | **85%** | **90%** |

---

## 八、测试数据管理

### 8.1 Mock 数据文件

**文件**: `tests/mock/proc/cpuinfo_imx6ull`

```
processor       : 0
model name      : Freescale i.MX6 UltraLite 528 MHz
BogoMIPS        : 264.00
Features        : half thumb fastmult vfp edsp neon vfpv3 tls vfpd32
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x0
CPU part        : 0xc07
CPU revision    : 5

Hardware        : Freescale i.MX6 UltraLite 528 MHz
Revision        : 0000
Serial          : 0000000000000000
```

**文件**: `tests/mock/proc/meminfo_512mb`

```
MemTotal:         524288 kB
MemFree:          262144 kB
MemAvailable:     393216 kB
Buffers:           16384 kB
Cached:            65536 kB
SwapCached:            0 kB
Active:           131072 kB
Inactive:         104857 kB
SwapTotal:             0 kB
SwapFree:              0 kB
```

### 8.2 测试夹具

```cpp
// tests/fixtures/test_theme.h

namespace CFDesktop::Testing {

/**
 * @brief 测试主题生成器
 *
 * 快速创建测试用主题。
 */
class TestThemeBuilder {
public:
    TestThemeBuilder& addColor(const QString& name, const QColor& color);
    TestThemeBuilder& addSize(const QString& name, int size);
    TestThemeBuilder& addFont(const QString& name, const QFont& font);
    TestThemeBuilder& addStyle(const QString& name, const QString& qss);

    Theme build() const;
    void save(const QString& path) const;

private:
    Theme m_theme;
};

/**
 * @brief 便捷函数
 */
inline Theme createDefaultTestTheme() {
    return TestThemeBuilder()
        .addColor("primary", QColor("#2196F3"))
        .addColor("background", QColor("#FFFFFF"))
        .addSize("normal", 8)
        .addFont("body", QFont("Arial", 10))
        .build();
}

} // namespace CFDesktop::Testing
```

---

## 九、详细任务清单

### 9.1 基础设施 (Week 1)

- [ ] 配置 CMake 测试框架
- [ ] 创建测试目录结构
- [ ] 实现 Mock 系统
- [ ] 创建测试应用框架
- [ ] 配置 CI 基础流水线

### 9.2 单元测试 (Week 2-4)

- [ ] HardwareProbe 测试套件
- [ ] ThemeEngine 测试套件
- [ ] AnimationManager 测试套件
- [ ] DPIManager 测试套件
- [ ] ConfigStore 测试套件
- [ ] Logger 测试套件
- [ ] InputManager 测试套件

### 9.3 集成测试 (Week 5)

- [ ] Base 库集成测试
- [ ] SDK 集成测试
- [ ] Shell 集成测试

### 9.4 UI 测试 (Week 6)

- [ ] Launcher UI 测试
- [ ] WindowManager UI 测试
- [ ] 应用切换测试

### 9.5 性能测试 (Week 7)

- [ ] 基准测试框架
- [ ] 启动时间测试
- [ ] 内存使用测试
- [ ] 动画性能测试
- [ ] 建立性能基准

---

## 十、验收标准

### 10.1 测试覆盖率
- [ ] 整体代码覆盖率 > 85%
- [ ] 核心模块覆盖率 > 90%
- [ ] 关键路径覆盖率 100%

### 10.2 CI/CD
- [ ] 所有测试自动运行
- [ ] 测试失败阻止合并
- [ ] 覆盖率报告自动生成

### 10.3 性能
- [ ] 所有性能测试达标
- [ ] 无内存泄漏
- [ ] 无明显性能退化

---

## 十一、测试最佳实践

### 11.1 命名约定

- 测试文件: `test_<module_name>.cpp`
- 测试类: `Test<ClassName>`
- 测试用例: `test<FunctionName>_<Scenario>`

### 11.2 断言选择

| 场景 | 使用 |
|------|------|
| 简单比较 | `QVERIFY2`, `QCOMPARE` |
| 性能测试 | `QBenchmark` |
| 异常测试 | `QEXPECT_FAIL` |
| 跳过测试 | `QSKIP` |

### 11.3 测试隔离

```cpp
void TestExample::init() {
    // 每个测试用例前：重置状态
    m_testObject = new TestClass();
}

void TestExample::cleanup() {
    // 每个测试用例后：清理资源
    delete m_testObject;
}
```

---

## 十二、常见问题解决

| 问题 | 解决方案 |
|------|----------|
| Qt 测试找不到组件 | 设置 `QT_QPA_PLATFORM=offscreen` |
| CI 中 UI 测试失败 | 使用 `Xvfb` 虚拟显示 |
| Mock 数据路径错误 | 使用 `QFINDTESTDATA` |
| 测试相互影响 | 确保正确使用 `init()`/`cleanup()` |

---

## 十三、下一步行动

测试体系应与其他阶段并行开发：
- Phase 0 完成 CI 基础设施
- Phase 1-2 同步编写单元测试
- Phase 4-5 同步编写集成和 UI 测试
