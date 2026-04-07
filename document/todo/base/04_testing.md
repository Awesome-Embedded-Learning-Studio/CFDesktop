# Phase 8: 测试体系 TODO

> **状态**: 🚧 进行中
> **预计周期**: 贯穿全程
> **依赖阶段**: 所有阶段
> **目标交付物**: 单元测试框架、集成测试、UI 自动化、CI/CD 配置
> **完成归档**: [document/todo/done/05_testing_status.md](../done/05_testing_status.md)


---

## 一、阶段目标

### 核心目标
建立完整的测试体系，确保代码质量和系统稳定性。测试体系覆盖单元测试、集成测试、UI 自动化测试和跨平台 CI。

### 具体交付物
- [ ] 单元测试框架和测试用例
- [ ] 集成测试套件
- [ ] UI 自动化测试
- [ ] CI/CD 流水线配置
- [ ] 测试覆盖率报告
- [ ] 性能测试基准

---

## 二、测试金字塔

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

---

## 三、待实现测试

### AnimationManager 测试套件
- [ ] `test_animation_manager.cpp`
  - [ ] 淡入淡出测试
  - [ ] 滑动动画测试
  - [ ] 缩放动画测试
  - [ ] 旋转动画测试
  - [ ] 并行动画组测试
  - [ ] 串行动画组测试
  - [ ] Low 档降级测试

### ThemeEngine 测试套件
- [ ] `test_theme_engine.cpp`
  - [ ] 主题加载测试
  - [ ] 变量访问测试
  - [ ] 主题继承测试
  - [ ] QSS 处理测试
  - [ ] 热重载测试
  - [ ] 档位降级测试

### DPIManager 测试套件
- [ ] `test_dpi_manager.cpp`
  - [ ] dp 转换测试
  - [ ] sp 转换测试
  - [ ] 参数注入测试
  - [ ] 高 DPI 测试

### ConfigStore 测试套件
- [ ] `test_config_store.cpp`
  - [ ] 存储测试
  - [ ] 读取测试
  - [ ] 层级测试
  - [ ] 监听测试
  - [ ] 持久化测试

### Logger 测试套件
- [ ] `test_logger.cpp`
  - [ ] 输出测试
  - [ ] 等级过滤测试
  - [ ] 标签过滤测试
  - [ ] 文件输出测试
  - [ ] 并发测试

---

## 四、集成测试

### Base 库集成测试
- [ ] `test_base_integration.cpp`
  - [ ] Hardware → Theme 集成
  - [ ] Hardware → Animation 集成
  - [ ] Theme → Widget 集成
  - [ ] 完整 Base 栈测试

### SDK 集成测试
- [ ] `test_sdk_integration.cpp`
  - [ ] Widget 加载测试
  - [ ] API 调用测试
  - [ ] 生命周期测试

### Shell 集成测试
- [ ] `test_shell_integration.cpp`
  - [ ] Launcher 启动测试
  - [ ] WindowManager 测试
  - [ ] 应用切换测试

---

## 五、UI 测试

### Launcher UI 测试
- [ ] `test_launcher_ui.cpp`
  - [ ] 显示测试
  - [ ] 图标点击测试
  - [ ] 滑动手势测试
  - [ ] 应用启动测试

### WindowManager UI 测试
- [ ] `test_window_manager_ui.cpp`
  - [ ] 窗口创建测试
  - [ ] 窗口切换测试
  - [ ] 窗口关闭测试

---

## 六、性能测试

### 启动时间测试
- [ ] `test_startup_time.cpp`
  - [ ] 冷启动测试
  - [ ] 热启动测试
  - [ ] 目标: < 2s

### 内存使用测试
- [ ] `test_memory_usage.cpp`
  - [ ] 基础占用测试
  - [ ] 峰值测试
  - [ ] 泄漏检测
  - [ ] 目标: < 100MB

### 动画性能测试
- [ ] `test_animation_fps.cpp`
  - [ ] 帧率测试
  - [ ] 目标: 稳定 60fps
  - [ ] 掉帧检测

---

## 七、验收标准

### 测试覆盖率
- [ ] 整体代码覆盖率 > 85%
- [ ] 核心模块覆盖率 > 90%
- [ ] 关键路径覆盖率 100%

### CI/CD
- [ ] 所有测试自动运行
- [ ] 测试失败阻止合并
- [ ] 覆盖率报告自动生成

### 性能
- [ ] 所有性能测试达标
- [ ] 无内存泄漏
- [ ] 无明显性能退化

---

## 八、文件清单

### 测试配置
- [x] `test/CMakeLists.txt` (已有)
- [ ] `.github/workflows/test.yml`

### 单元测试
- [ ] `tests/unit/base/test_hardware_probe.cpp`
- [ ] `tests/unit/base/test_theme_engine.cpp`
- [ ] `tests/unit/base/test_animation_manager.cpp`
- [ ] `tests/unit/base/test_dpi_manager.cpp`
- [ ] `tests/unit/base/test_config_store.cpp`
- [ ] `tests/unit/base/test_logger.cpp`
- [ ] `tests/unit/base/test_input_manager.cpp`

### 集成测试
- [ ] `tests/integration/test_base_integration.cpp`
- [ ] `tests/integration/test_sdk_integration.cpp`
- [ ] `tests/integration/test_shell_integration.cpp`

### UI 测试
- [ ] `tests/ui/test_launcher_ui.cpp`
- [ ] `tests/ui/test_window_manager_ui.cpp`
- [ ] `tests/ui/test_app_switching.cpp`
- [ ] `tests/ui/test_accessibility.cpp`

### 性能测试
- [ ] `tests/performance/test_startup_time.cpp`
- [ ] `tests/performance/test_animation_fps.cpp`
- [ ] `tests/performance/test_memory_usage.cpp`
- [ ] `tests/performance/benchmarks/benchmark_theme_load.cpp`
- [ ] `tests/performance/benchmarks/benchmark_input_processing.cpp`

### Mock 数据
- [ ] `tests/mock/proc/cpuinfo_imx6ull`
- [ ] `tests/mock/proc/cpuinfo_rk3568`
- [ ] `tests/mock/proc/cpuinfo_rk3588`
- [ ] `tests/mock/proc/meminfo_512mb`
- [ ] `tests/mock/proc/meminfo_1gb`
- [ ] `tests/mock/proc/meminfo_4gb`

### 测试夹具
- [ ] `tests/fixtures/test_app.h`
- [ ] `tests/fixtures/test_widget.h`
- [ ] `tests/fixtures/test_theme.h`
- [ ] `tests/fixtures/test_helpers.h`

---

## 九、相关文档

- 设计文档: [../../design_stage/05_phase8_testing.md](../../design_stage/05_phase8_testing.md)
- 依赖: 所有其他阶段文档
- 测试状态: [../done/05_testing_status.md](../done/05_testing_status.md)

---

*最后更新: 2026-03-30*
