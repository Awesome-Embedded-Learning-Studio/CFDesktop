/**
 * @file    null_window_backend.h
 * @brief   No-op IWindowBackend for embedded DirectRender mode.
 *
 * In DirectRender mode CFDesktop renders its own shell and does not manage
 * external application windows. NullWindowBackend satisfies the IWindowBackend
 * contract with empty implementations so the shell boots without a windowing
 * system.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#pragma once

#include "../../components/IWindowBackend.h"

namespace cf::desktop::backend::embedded {

/**
 * @brief  No-op window backend for embedded DirectRender mode.
 *
 * Reports no managed windows and limited capabilities. createWindow() returns
 * an empty reference and no signals are emitted.
 *
 * @ingroup platform_embedded
 */
class NullWindowBackend : public IWindowBackend {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the null window backend.
     * @param[in] parent  Optional Qt parent.
     * @throws            None
     * @note              None
     * @warning           None
     * @since             0.19.0
     * @ingroup           platform_embedded
     */
    explicit NullWindowBackend(QObject* parent = nullptr);

    /**
     * @brief  Destroys the null window backend.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup platform_embedded
     */
    ~NullWindowBackend() override;

    /**
     * @brief  Returns an empty reference; no windows are created.
     * @param[in] appId  Logical identifier of the application (unused).
     * @return           Empty weak reference (no window is created).
     * @throws           None
     * @note             None
     * @warning          None
     * @since            0.19.0
     * @ingroup          platform_embedded
     */
    aex::WeakPtr<IWindow> createWindow(const QString& appId) override;

    /**
     * @brief  No-op destruction; nothing is tracked.
     * @param[in] window  Weak reference to the window (unused).
     * @throws            None
     * @note              None
     * @warning           None
     * @since             0.19.0
     * @ingroup           platform_embedded
     */
    void destroyWindow(aex::WeakPtr<IWindow> window) override;

    /**
     * @brief  Returns an empty list; no windows are tracked.
     * @return     Empty list of weak window references.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    QList<aex::WeakPtr<IWindow>> windows() const override;

    /**
     * @brief  Returns limited capabilities (no multi-window, no GPU).
     * @return     BackendCapabilities describing the embedded framebuffer.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    render::BackendCapabilities capabilities() const override;
};

} // namespace cf::desktop::backend::embedded
