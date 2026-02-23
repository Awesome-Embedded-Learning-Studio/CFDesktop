# CPU 信息查询示例

## 简介

本文档展示了如何使用 CFDesktop CPU 模块查询各种 CPU 信息。示例代码基于 `example/base/system/example_cpu_info.cpp`。

## 完整示例代码

```cpp
#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_bonus.h"
#include "system/cpu/cfcpu_profile.h"
#include <iostream>

int main() {
    using namespace cf;

    // ===== 1. 查询基础信息 =====
    auto cpuInfo = getCPUInfo();
    if (!cpuInfo.has_value()) {
        std::cerr << "无法获取 CPU 基础信息" << std::endl;
        return 1;
    }

    std::cout << "=== CPU 基础信息 ===" << std::endl;
    std::cout << "  型号: " << std::string(cpuInfo->model) << std::endl;
    std::cout << "  架构: " << std::string(cpuInfo->arch) << std::endl;
    std::cout << "  制造商: " << std::string(cpuInfo->manufacturer) << std::endl;

    // ===== 2. 查询性能信息 =====
    auto profileInfo = getCPUProfileInfo();
    if (!profileInfo.has_value()) {
        std::cerr << "无法获取 CPU 性能信息" << std::endl;
        return 1;
    }

    std::cout << "\n=== CPU 性能信息 ===" << std::endl;
    std::cout << "  逻辑线程: " << profileInfo->logical_cnt << std::endl;
    std::cout << "  物理核心: " << profileInfo->physical_cnt << std::endl;
    std::cout << "  当前频率: " << profileInfo->current_frequecy << " MHz" << std::endl;
    std::cout << "  最大频率: " << profileInfo->max_frequency << " MHz" << std::endl;
    std::cout << "  使用率: " << profileInfo->cpu_usage_percentage << "%" << std::endl;

    // ===== 3. 查询扩展信息 =====
    auto bonusInfo = getCPUBonusInfo();
    if (!bonusInfo.has_value()) {
        std::cerr << "无法获取 CPU 扩展信息" << std::endl;
        return 1;
    }

    std::cout << "\n=== CPU 扩展信息 ===" << std::endl;

    // 打印 CPU 特性
    std::cout << "  特性: ";
    for (const auto& feature : bonusInfo->features) {
        std::cout << std::string(feature) << " ";
    }
    std::cout << std::endl;

    // 打印缓存大小
    std::cout << "  缓存: ";
    for (const auto& cache : bonusInfo->cache_size) {
        std::cout << cache << "KB ";
    }
    std::cout << std::endl;

    // 打印大小核信息
    if (bonusInfo->has_big_little) {
        std::cout << "  大小核架构: 是" << std::endl;
        std::cout << "    大核: " << bonusInfo->big_core_count << std::endl;
        std::cout << "    小核: " << bonusInfo->little_core_count << std::endl;
    } else {
        std::cout << "  大小核架构: 否" << std::endl;
    }

    // 打印温度
    if (bonusInfo->temperature.has_value()) {
        std::cout << "  温度: " << *bonusInfo->temperature << "°C" << std::endl;
    } else {
        std::cout << "  温度: 不可用" << std::endl;
    }

    return 0;
}
```

## 编译和运行

### 编译

```bash
cd /home/charliechen/project/QtProjects/CFDesktop
./scripts/build_helpers/linux_fast_develop_build.sh
```

### 运行

```bash
./out/build_develop/example/base/system/example_cpu_info
```

## 示例输出

在 x86_64 Linux 系统上的典型输出：

```
=== CPU 基础信息 ===
  型号: Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz
  架构: x86_64
  制造商: GenuineIntel

=== CPU 性能信息 ===
  逻辑线程: 12
  物理核心: 6
  当前频率: 2600 MHz
  最大频率: 4500 MHz
  使用率: 15.5%

=== CPU 扩展信息 ===
  特性: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx avx512f avx512dq rdseed adx smap clflushopt clwb avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_act_window hwp_epp hwp_pkg_req

  缓存: 32KB 256KB 12288KB
  大小核架构: 否
  温度: 不可用
```

在 ARM Linux 系统上的典型输出：

```
=== CPU 基础信息 ===
  型号: ARM Cortex-A76
  架构: aarch64
  制造商: ARM

=== CPU 性能信息 ===
  逻辑线程: 8
  物理核心: 8
  当前频率: 1800 MHz
  最大频率: 2400 MHz
  使用率: 8.2%

=== CPU 扩展信息 ===
  特性: fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm jscvt fcma lrcpc dcpop sha3 sm3 sm4 asimddp sha512 sve asimdfhm dit uscat ilrcpc flagm ssbs sb paca pacg dcpodp sve2 sveaes svepmull svebitperm svesha3 svesm4 flagm2 frint

  缓存: 64KB 512KB 4096KB
  大小核架构: 否
  温度: 45°C
```

## 错误处理

所有 API 都返回 `expected<T, E>` 类型，需要检查是否有值：

```cpp
auto result = cf::getCPUInfo();

if (!result.has_value()) {
    // 处理错误
    switch (result.error()) {
        case cf::CPUInfoErrorType::CPU_QUERY_NOERROR:
            break;
        case cf::CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED:
            std::cerr << "查询失败" << std::endl;
            break;
    }
    return 1;
}

// 使用结果
auto info = result.value();
```

## 最佳实践

1. **检查返回值**：始终检查 `has_value()` 或使用 `operator->` 直接访问

2. **避免存储视图**：`CPUInfoView` 和 `CPUBonusInfoView` 中的 `string_view` 仅在缓存有效时可用

3. **强制刷新**：需要更新信息时使用 `force_refresh = true`

```cpp
// 首次查询
auto info1 = cf::getCPUInfo();

// 稍后刷新查询
auto info2 = cf::getCPUInfo(true);
```

## 相关文档

- [基础信息 API](../api/system/cpu/cfcpu.md)
- [性能信息 API](../api/system/cpu/cfcpu_profile.md)
- [扩展信息 API](../api/system/cpu/cfcpu_bonus.md)
