/**
 * @file    render_backend.h
 * @brief   Abstract interface for platform-specific rendering backends.
 *
 * RenderBackend encapsulates the low-level rendering hardware
 * initialization and lifecycle. Concrete implementations wrap the
 * platform's native rendering API (EGL/OpenGL ES on embedded Linux,
 * Win32 + D3D/OpenGL on Windows, etc.).
 *
 * The shell does not draw through RenderBackend directly — Qt RHI or
 * QPainter is still used for actual rendering. RenderBackend's job is to
 * set up the rendering surface, provide capability information, and
 * manage the native display handle.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup render
 */

#pragma once

#include "backend_capabilities.h"
#include <QSize>

namespace cf::desktop::render {

/**
 * @brief  Abstract rendering backend interface.
 *
 * Provides lifecycle management and capability queries for the
 * platform-specific rendering hardware. Each supported platform
 * (EGLFS, Windows, Wayland, X11) provides its own implementation.
 *
 * @note   Implementations must be safe to construct but should not
 *         access hardware until initialize() is called.
 * @ingroup render
 */
class RenderBackend {
  public:
    virtual ~RenderBackend() = default;

    /**
     * @brief  Initializes the rendering backend and hardware resources.
     *
     * Must be called exactly once before any other method.
     *
     * @return True if initialization succeeded, false on failure.
     */
    virtual bool initialize() = 0;

    /**
     * @brief  Releases all hardware resources held by the backend.
     *
     * Safe to call even if initialize() was never called or failed.
     */
    virtual void shutdown() = 0;

    /**
     * @brief  Queries the capabilities of this rendering backend.
     *
     * @return BackendCapabilities describing what the backend supports.
     */
    virtual BackendCapabilities capabilities() const = 0;

    /**
     * @brief  Returns the current screen/ output size in pixels.
     *
     * On multi-output setups this returns the primary output size.
     *
     * @return Screen dimensions in device pixels.
     */
    virtual QSize screenSize() const = 0;

    /**
     * @brief  Returns the platform-native display handle.
     *
     * The type depends on the platform:
     * - Linux EGLFS: EGLDisplay
     * - Windows:     HWND or device context
     * - Wayland:     wl_display*
     * - X11:         Display*
     *
     * @return Opaque native handle, or nullptr if not initialized.
     */
    virtual void* nativeHandle() const = 0;
};

} // namespace cf::desktop::render
