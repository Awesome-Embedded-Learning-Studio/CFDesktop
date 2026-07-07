/**
 * @file    window_info.h
 * @brief   Tracked-window data model for WindowManager.
 *
 * WindowInfo captures the observed state of an external window (title, pid,
 * geometry, lifecycle state) so the shell can reason about windows without
 * owning them. WindowState models the lifecycle the tracker needs to drive
 * minimize/maximize/restore transitions and taskbar linkage.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.2
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include <QDateTime>
#include <QRect>
#include <QString>

namespace cf::desktop {

/**
 * @brief  Lifecycle state of a tracked window.
 *
 * Normal, Minimized, Maximized, and Closed are exercised by the WindowManager
 * state machine today; Closing and Fullscreen are reserved for future work
 * (close-animation and full-screen window modes).
 *
 * @ingroup components
 */
enum class WindowState {
    Normal,     ///< Visible, resting state.
    Minimized,  ///< Hidden to the taskbar.
    Maximized,  ///< Filling the work area.
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
    QString icon_hint;                      ///< Hint for icon lookup (app_id/path; may be empty).
    qint64 pid{0};                          ///< Owning process id (0 if unknown).
    QRect geometry;                         ///< Last observed geometry (device px).
    WindowState state{WindowState::Normal}; ///< Lifecycle state.
    int z_index{0};                         ///< Last observed stacking order (0 = bottom).
    bool is_always_on_top{false};           ///< Whether the window is pinned on top.
    QDateTime created_at;                   ///< When the window was first observed.
};

} // namespace cf::desktop
