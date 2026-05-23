/**
 * @file    example_hardware_tier.cpp
 * @brief   Demonstrates hardware tier assessment on the current machine.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "system/hardware_tier/hardware_tier.h"

#include <iostream>

using namespace cf;

int main() {
    auto assessment = assessHardware();
    if (!assessment.has_value()) {
        std::cerr << "Failed to assess hardware tier, error: "
                  << static_cast<int>(assessment.error()) << "\n";
        return 1;
    }

    const auto& a = *assessment;

    std::cout << "========== Hardware Tier Assessment ==========\n";
    std::cout << "CPU Score:     " << a.cpu.value << " / 100\n";
    std::cout << "GPU Score:     " << a.gpu.value << " / 100\n";
    std::cout << "Memory Score:  " << a.memory.value << " / 100\n";
    std::cout << "Display Score: " << a.display.value << " / 100\n";
    std::cout << "\n";
    std::cout << "Overall Tier:  " << hardwareTierLevelToString(a.tier) << "\n";

    if (a.is_overridden) {
        std::cout << "  (Overridden: " << a.override_reason << ")\n";
    }

    auto caps = getHardwareTierCapabilities();
    if (caps.has_value()) {
        const auto& c = *caps;
        std::cout << "\n========== Capability Flags ==========\n";
        std::cout << "OpenGL:          " << (c.use_opengl ? "Yes" : "No") << "\n";
        std::cout << "Software Render: " << (c.use_software_render ? "Yes" : "No") << "\n";
        std::cout << "Animation:       "
                  << (c.enable_animation           ? "Full"
                      : c.enable_partial_animation ? "Partial"
                                                   : "None")
                  << "\n";
        std::cout << "Hardware Decode: " << (c.use_hardware_decode ? "Yes" : "No") << "\n";
        std::cout << "EGLFS:           " << (c.use_eglfs ? "Yes" : "No") << "\n";
        std::cout << "LinuxFB:         " << (c.use_linuxfb ? "Yes" : "No") << "\n";
    }

    return 0;
}
