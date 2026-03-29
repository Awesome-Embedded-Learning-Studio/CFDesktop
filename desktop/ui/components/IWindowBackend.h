/**
 * @file    IWindowBackend.h
 * @brief   Abstract interface for window backends.
 *
 * IWindowBackend defines how windows are created, destroyed, and
 * tracked. Concrete implementations adapt to each platform (Win32,
 * X11, Wayland, internal QWidget).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once
#include "../render/backend_capabilities.h"
#include "IWindow.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include <QObject>

namespace cf::desktop {

/**
 * @brief  Abstract interface for window creation and tracking.
 *
 * Concrete backends implement this to manage windows on each
 * platform. The backend notifies the shell when windows appear
 * or disappear.
 *
 * @ingroup components
 */
class IWindowBackend : public QObject {
    Q_OBJECT
  public:
    explicit IWindowBackend(QObject* parent = nullptr) : QObject(parent) {}

    virtual ~IWindowBackend() = default;

    virtual WeakPtr<IWindow> createWindow(const QString& appId) = 0;

    /**
     * @brief  Destroys the given window and releases resources.
     *
     * @param[in]  window  Weak reference to the window to destroy.
     */
    virtual void destroyWindow(WeakPtr<IWindow> window) = 0;

    /// Referenced windows held by the backend.
    virtual QList<WeakPtr<IWindow>> windows() const = 0;

    /**
     * @brief  Queries the capabilities of this window backend.
     *
     * Allows the shell and window manager to adapt their behavior based on
     * what the backend supports (e.g. disabling multi-window features on
     * linuxfb).
     *
     * @return BackendCapabilities describing the backend's abilities.
     * @since  0.11
     */
    virtual render::BackendCapabilities capabilities() const = 0;

    /**
     * @brief  Returns a weak reference to this backend.
     *
     * @return A WeakPtr referencing this IWindowBackend instance.
     * @since  0.11
     */
    WeakPtr<IWindowBackend> make_weak() const { return weak_ptr_factory_.GetWeakPtr(); }

  signals:
    /**
     * @brief  Emitted when the backend creates a new window.
     *
     * @param[in]  ref  Weak reference to the newly created window.
     */
    void window_came(WeakPtr<IWindow> ref);

    /**
     * @brief  Emitted when a window is destroyed and about to be released.
     *
     * @param[in]  ref  Weak reference to the destroyed window.
     */
    void window_gone(WeakPtr<IWindow> ref);

  private:
    mutable WeakPtrFactory<IWindowBackend> weak_ptr_factory_{const_cast<IWindowBackend*>(this)};
};
} // namespace cf::desktop
