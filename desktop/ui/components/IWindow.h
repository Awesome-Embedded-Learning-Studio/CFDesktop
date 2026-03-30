/**
 * @file    IWindow.h
 * @brief   Platform-agnostic window interface.
 *
 * IWindow defines the core window contract used by WindowManager and
 * shell components. Concrete implementations wrap native handles (e.g.
 * HWND on Windows, Window on X11) or internal QWidget windows.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once
#include "WindowDefine.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include <QObject>

namespace cf::desktop {

/**
 * @brief  Platform-agnostic window abstraction.
 *
 * Provides a uniform interface for querying window properties and
 * requesting close. Concrete backends implement this for each
 * platform (Win32, X11, Wayland, internal QWidget).
 *
 * @ingroup components
 */
class IWindow : public QObject {
    Q_OBJECT
  public:
    explicit IWindow(QObject* parent = nullptr);
    ~IWindow();
    /**
     * @brief  Returns the unique identifier of this window.
     *
     * @return A platform-specific window identifier string.
     */
    virtual win_id_t windowID() const = 0;

    /**
     * @brief  Returns the current window title.
     *
     * @return The window title as a Unicode string.
     */
    virtual QString title() const = 0;

    /**
     * @brief  Returns the current window geometry in device pixels.
     *
     * @return The window rectangle (position and size).
     */
    virtual QRect geometry() const = 0;

    /**
     * @brief  Moves and resizes the window.
     *
     * @param[in]  r  The new geometry in device pixels.
     */
    virtual void set_geometry(const QRect& r) = 0;

    /**
     * @brief  Requests the window to close gracefully.
     */
    virtual void requestClose() = 0;

    /**
     * @brief  Raises this window to the top of the stacking order.
     */
    virtual void raise() = 0;

    /**
     * @brief  Creates a weak pointer to this window.
     *
     * @return A WeakPtr that references this IWindow instance.
     */
    WeakPtr<IWindow> make_weak() const { return weak_ptr_factory_.GetWeakPtr(); }

  signals:

    /**
     * @brief  Emitted when the window requests to be closed.
     */
    void closeRequested();

    /**
     * @brief  Emitted when the window title changes.
     *
     * @param[in]  prev_title  The previous title.
     * @param[in]  new_title   The new title.
     */
    void titleChanged(const QString& prev_title, const QString& new_title);

    /**
     * @brief  Emitted when the window geometry changes.
     *
     * @param[in]  old_rect  The previous geometry.
     * @param[in]  new_rect  The new geometry.
     */
    void geometryChanged(const QRect& old_rect, const QRect& new_rect);

  private:
    /// Factory for creating weak pointers. Ownership: this instance.
    WeakPtrFactory<IWindow> weak_ptr_factory_;
};
} // namespace cf::desktop
