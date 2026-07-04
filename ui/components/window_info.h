/**
 * @file    window_info.h
 * @brief   Tracked-window data model for WindowManager.
 *
 * WindowInfo captures the observed state of an external window (title, pid,
 * geometry, lifecycle state) so the shell can reason about windows without
 * owning them. WindowState models the minimal lifecycle the tracker needs.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.1
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include <QRect>
#include <QString>

namespace cf::desktop {

/**
 * @brief  Lifecycle state of a tracked window.
 *
 * Only Normal and Closed are exercised by the tracker today; the remaining
 * values are reserved for future window-management work.
 *
 * @ingroup components
 */
enum class WindowState {
    Normal,     ///< Visible, resting state.
    Minimized,  ///< Hidden to the taskbar (reserved).
    Maximized,  ///< Filling the work area (reserved).
    Fullscreen, ///< Full-screen (reserved).
    Closing,    ///< Close requested, not yet gone (reserved).
    Closed      ///< Window is gone.
};

/**
 * @brief  Snapshot of an observed external window.
 *
 * @ingroup components
 */
struct WindowInfo {
    QString window_id;                      ///< Platform window identifier (cf IWindow::windowID).
    QString title;                          ///< Last observed window title.
    qint64 pid{0};                          ///< Owning process id (0 if unknown).
    QRect geometry;                         ///< Last observed geometry (device px).
    WindowState state{WindowState::Normal}; ///< Lifecycle state.
};

} // namespace cf::desktop
