/**
 * @file    render_backend_factory.h
 * @brief   Factory for creating the appropriate RenderBackend instance.
 *
 * Selects and instantiates the correct RenderBackend implementation
 * based on the current platform and runtime environment.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup render
 */

#pragma once

#include "render_backend.h"
#include <memory>

namespace cf::desktop::render {

/**
 * @brief  Factory that creates a RenderBackend suitable for the current
 *         platform and environment.
 *
 * The selection logic follows this priority:
 * 1. Environment variable CFDESKTOP_RENDER_BACKEND (force override)
 * 2. Platform detection via Qt platform name and system probes
 * 3. Default: platform-specific default backend
 *
 * @note   The returned backend is created but **not initialized**.
 *         The caller must call initialize() before use.
 * @ingroup render
 */
class RenderBackendFactory {
  public:
    /**
     * @brief  Creates a RenderBackend appropriate for the current environment.
     *
     * @return A unique_ptr to the created backend, or nullptr if no suitable
     *         backend is available.
     */
    static std::unique_ptr<RenderBackend> create();

    /**
     * @brief  Creates a specific RenderBackend by name.
     *
     * @param[in] backendName  One of: "eglfs", "linuxfb", "windows",
     *                         "wayland", "x11".
     * @return A unique_ptr to the created backend, or nullptr if the
     *         name is unrecognized.
     */
    static std::unique_ptr<RenderBackend> createByName(const char* backendName);
};

} // namespace cf::desktop::render
