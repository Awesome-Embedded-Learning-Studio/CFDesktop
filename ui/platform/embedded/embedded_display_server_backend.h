/**
 * @file    embedded_display_server_backend.h
 * @brief   DirectRender IDisplayServerBackend for embedded framebuffer targets.
 *
 * EmbeddedDisplayServerBackend is the display server backend used when
 * CFDesktop renders directly to the framebuffer (Qt linuxfb / EGLFS) without
 * a windowing system. It reports the DirectRender role, exposes the primary
 * screen as the single output, and provides a NullWindowBackend so the shell
 * boots standalone.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#pragma once

#include "../../components/IDisplayServerBackend.h"
#include "null_window_backend.h"

#include <memory>

namespace cf::desktop::backend::embedded {

/**
 * @brief  DirectRender display server backend for embedded framebuffer targets.
 *
 * Operates without an external window manager: the Qt platform plugin
 * (linuxfb/EGLFS) owns the framebuffer, and this backend only reports the
 * primary output geometry and a no-op window backend.
 *
 * @ingroup platform_embedded
 */
class EmbeddedDisplayServerBackend : public IDisplayServerBackend {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the embedded DirectRender backend.
     * @param[in] parent  Optional Qt parent.
     * @throws            None
     * @note              None
     * @warning           None
     * @since             0.19.0
     * @ingroup           platform_embedded
     */
    explicit EmbeddedDisplayServerBackend(QObject* parent = nullptr);

    /**
     * @brief  Destroys the embedded DirectRender backend.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup platform_embedded
     */
    ~EmbeddedDisplayServerBackend() override;

    /**
     * @brief  Returns the DirectRender role.
     * @return     DisplayServerRole::DirectRender.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    DisplayServerRole role() const override;

    /**
     * @brief  Returns capabilities for a standalone framebuffer target.
     * @return     DisplayServerCapabilities with no external-window or
     *             protocol support.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    DisplayServerCapabilities capabilities() const override;

    /**
     * @brief  Initializes the backend (the QPA plugin owns the framebuffer).
     * @param[in] argc  Argument count from the application entry point (unused).
     * @param[in] argv  Argument vector from the application entry point (unused).
     * @return          True unconditionally; initialization cannot fail.
     * @throws          None
     * @note            The Qt platform plugin (linuxfb/EGLFS) is selected via
     *                  QT_QPA_PLATFORM before the QApplication is created.
     * @warning         None
     * @since           0.19.0
     * @ingroup         platform_embedded
     */
    bool initialize(int argc, char** argv) override;

    /**
     * @brief  Releases resources held by the backend.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup platform_embedded
     */
    void shutdown() override;

    /**
     * @brief  Runs the backend event loop.
     * @return     Always zero; the Qt event loop is driven by the desktop
     *             session via QApplication::exec().
     * @throws     None
     * @note       Not invoked on the Linux boot path.
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    int runEventLoop() override;

    /**
     * @brief  Returns the no-op window backend.
     * @return     Weak pointer to the NullWindowBackend, or empty before
     *             initialization.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    aex::WeakPtr<IWindowBackend> windowBackend() override;

    /**
     * @brief  Returns the primary screen geometry as the single output.
     * @return     List containing the primary screen rectangle, or empty when
     *             no screen is available yet.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    QList<QRect> outputs() const override;

  private:
    /// No-op window backend. Ownership: this.
    std::unique_ptr<NullWindowBackend> window_backend_;
    /// Whether initialize() has succeeded.
    bool initialized_{false};
};

} // namespace cf::desktop::backend::embedded
