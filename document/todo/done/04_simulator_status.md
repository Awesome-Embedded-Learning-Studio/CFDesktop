---
title: "Phase 4: 多平台模拟器 - 状态文档"
description: "模块ID: Phase 4，状态: <img src=\"x-data:image/svg+xml,%"
---

# Phase 4: 多平台模拟器 - 状态文档

> **模块ID**: Phase 4
> **状态**: <img src="x-data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16'%3E%3Crect width='16' height='16' fill='none' stroke='gray' stroke-width='1'/%3E%3C/svg%3E" width="16" height="16"> 未开始
> **总体进度**: 0%
> **最后更新**: 2026-03-11

---

# 04 - Multi-Platform Simulator Reference

## Module Overview

The Multi-Platform Simulator module provides cross-platform device simulation capabilities for CFDesktop, enabling developers to test the application on various device configurations without physical hardware. This module is essential for embedded device development and cross-platform UI testing.

**Module Purpose:**
- Simulate different hardware configurations (memory, CPU, display)
- Provide device frame visualization (phone, tablet, embedded terminal)
- Enable touch input visualization for touch-enabled interfaces
- Support hardware tier selection for performance testing
- DPI injection for testing different screen densities

---

## Current Implementation Status

**Completion: 0%**

This module is currently **NOT IMPLEMENTED**. No files exist for the simulator functionality.

---

## Required Components

### 1. SimulatorWindow (Main Window)
**Status:** Not Implemented

**Purpose:** Main simulator window hosting device frames and controls.

**Required Features:**
- Device frame container
- Configuration panel (device type, resolution, DPI)
- Runtime controls (start, stop, reset)
- Screenshot capture
- Performance monitoring overlay

**Key Files to Create:**
- `ui/simulator/simulator_window.h`
- `ui/simulator/simulator_window.cpp`

---

### 2. DeviceFrame (Device Shell)
**Status:** Not Implemented

**Purpose:** Visual frame representing physical device外观.

**Required Features:**
- Predefined device frames (phone, tablet, embedded terminal)
- Custom frame support
- Screen area clipping
- Device-specific bezels and buttons
- Status bar simulation

**Key Files to Create:**
- `ui/simulator/device/device_frame.h`
- `ui/simulator/device/device_frame.cpp`
- `ui/simulator/device/frame_presets.h` (preset definitions)

---

### 3. TouchVisualizer
**Status:** Not Implemented

**Purpose:** Visualize touch events for debugging touch interfaces.

**Required Features:**
- Touch point rendering (circles at touch locations)
- Multi-touch support (up to 10 points)
- Touch path/trail visualization
- Tap duration indication
- Gesture recognition display

**Key Files to Create:**
- `ui/simulator/visualizer/touch_visualizer.h`
- `ui/simulator/visualizer/touch_visualizer.cpp`

---

### 4. HWTierSelector (Hardware Tier Selector)
**Status:** Not Implemented

**Purpose:** Select hardware performance tiers for testing.

**Required Features:**
- Tier presets (Low, Medium, High, Ultra)
- Custom hardware configuration
- Memory limit simulation
- CPU throttling simulation
- Storage speed simulation

**Key Files to Create:**
- `ui/simulator/config/hw_tier_selector.h`
- `ui/simulator/config/hw_tier_selector.cpp`
- `ui/simulator/config/hw_profile.h` (profile definitions)

---

### 5. DPI Injector
**Status:** Not Implemented

**Purpose:** Inject custom DPI values for testing high/low density displays.

**Required Features:**
- DPI value override
- Scaling factor simulation
- Font DPI injection
- Coordinate system transformation

**Key Files to Create:**
- `ui/simulator/injector/dpi_injector.h`
- `ui/simulator/injector/dpi_injector.cpp`

---

### 6. Hardware Mock
**Status:** Not Implemented

**Purpose:** Mock hardware APIs for embedded simulation.

**Required Features:**
- Memory query mock
- CPU info mock
- Storage mock
- Network condition simulation
- Sensor mock (accelerometer, gyroscope)

**Key Files to Create:**
- `ui/simulator/mock/hardware_mock.h`
- `ui/simulator/mock/hardware_mock.cpp`
- `ui/simulator/mock/system_info_mock.h`

---

## Implementation Priority

### Phase 1 - Core Structure (P0)
1. **SimulatorWindow** - Basic window with container
2. **DeviceFrame** - Simple frame with screen area
3. **HWTierSelector** - Basic tier selection UI

### Phase 2 - Visualization (P1)
4. **TouchVisualizer** - Touch event rendering
5. **DPI Injector** - DPI override capability

### Phase 3 - Advanced Features (P2)
6. **HardwareMock** - Full hardware API mocking
7. **Performance Profiling** - FPS, memory monitoring
8. **Screenshot/Recording** - Capture capabilities

---

## Key File Paths

| Component | Header Path | Source Path |
|-----------|-------------|-------------|
| SimulatorWindow | `ui/simulator/simulator_window.h` | `ui/simulator/simulator_window.cpp` |
| DeviceFrame | `ui/simulator/device/device_frame.h` | `ui/simulator/device/device_frame.cpp` |
| TouchVisualizer | `ui/simulator/visualizer/touch_visualizer.h` | `ui/simulator/visualizer/touch_visualizer.cpp` |
| HWTierSelector | `ui/simulator/config/hw_tier_selector.h` | `ui/simulator/config/hw_tier_selector.cpp` |
| DPI Injector | `ui/simulator/injector/dpi_injector.h` | `ui/simulator/injector/dpi_injector.cpp` |
| HardwareMock | `ui/simulator/mock/hardware_mock.h` | `ui/simulator/mock/hardware_mock.cpp` |

---

## Design Considerations

### Qt Integration
- Use `QWidget` as base for all simulator components
- Leverage `QGraphicsView` for device frame rendering
- Use `QTouchEvent` for touch simulation

### Platform Support
- Windows: Full support
- Linux: Full support
- macOS: Full support (with limitations on touch)

### Performance
- Simulator should not impact application performance significantly
- Mock operations should be lightweight
- Visualization should use hardware acceleration when available

---

## Next Steps

1. **Create directory structure:**
   ```
   ui/simulator/
   ├── simulator_window.h/cpp
   ├── device/
   ├── visualizer/
   ├── config/
   ├── injector/
   └── mock/
   ```

2. **Implement SimulatorWindow** - Start with basic container widget

3. **Add CMakeLists.txt entries** for new simulator module

4. **Integrate with existing Application class** - Add simulator launcher

5. **Write unit tests** for each component

---

## Related Documents

- `01_architecture_ref.md` - Overall architecture
- `05_testing_ref.md` - Testing infrastructure
- `10_ui_embedding_ref.md` - UI embedding for embedded systems

---

*Last Updated: 2026-03-05*
*Status: Not Implemented*
