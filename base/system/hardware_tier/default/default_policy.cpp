/**
 * @file    default_policy.cpp
 * @brief   Default policy implementation.
 *
 * Maps HardwareTierLevel to HardwareTierCapabilities based on the
 * design document tier definitions.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "../default_factories.h"

namespace cf {

class DefaultPolicy : public IHardwarePolicy {
  public:
    HardwareTierCapabilities deriveCapabilities(HardwareTierLevel level,
                                                const HardwareTierAssessment& assessment) override {
        HardwareTierCapabilities caps;

        switch (level) {
            case HardwareTierLevel::High:
                caps.use_opengl = true;
                caps.enable_animation = true;
                caps.use_hardware_decode = true;
                caps.use_eglfs = true;
                break;

            case HardwareTierLevel::Mid:
                caps.use_opengl = true;
                caps.enable_partial_animation = true;
                caps.use_hardware_decode = true;
                caps.use_eglfs = (assessment.gpu.value >= 40);
                caps.use_linuxfb = !caps.use_eglfs;
                break;

            case HardwareTierLevel::Low:
                caps.use_software_render = true;
                caps.use_hardware_decode = false;
                caps.use_linuxfb = true;
                break;

            case HardwareTierLevel::Unknown:
            default:
                caps.use_software_render = true;
                caps.use_linuxfb = true;
                break;
        }

        return caps;
    }
};

std::unique_ptr<IHardwarePolicy> makeDefaultPolicy() {
    return std::make_unique<DefaultPolicy>();
}

} // namespace cf
