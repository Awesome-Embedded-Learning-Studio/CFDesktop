/**
 * @file    backend_capabilities.h
 * @brief   Defines rendering backend capability flags and query structure.
 *
 * BackendCapabilities allows the desktop shell to query what the current
 * rendering backend supports at runtime, enabling graceful degradation
 * when running on limited hardware (e.g. linuxfb vs GPU-accelerated).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup render
 */

#pragma once

#include <cstdint>

namespace cf::desktop::render {

/**
 * @brief  Describes the capabilities of a rendering backend.
 *
 * Each field can be queried at runtime so that the shell can adapt its
 * behavior — for example, disabling transparency when the backend does
 * not support it, or falling back to software rendering when there is no
 * GPU acceleration.
 *
 * @note   Default values assume a capable desktop backend; limited
 *         backends (linuxfb) should set fields to false.
 * @ingroup render
 */
struct BackendCapabilities {
    /// Whether the backend can create multiple independent windows.
    bool supportsMultiWindow = true;

    /// Whether the backend supports per-pixel alpha blending.
    bool supportsTransparency = true;

    /// Whether GPU-accelerated rendering (OpenGL / Vulkan) is available.
    bool hasHardwareAcceleration = true;

    /// Whether the backend can synchronize to the display refresh rate.
    bool supportsVSync = true;

    /// Whether the backend can capture the screen content for screenshots.
    bool supportsScreenshot = true;

    /// Maximum texture dimension supported by the GPU (0 if unknown).
    int maxTextureSize = 4096;
};

} // namespace cf::desktop::render
