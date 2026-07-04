/**
 * @file    WindowManager.h
 * @brief   Tracks windows created by the window backend.
 *
 * WindowManager records windows created by the IWindowBackend and
 * provides operations for closing and raising windows. It holds only
 * weak references — it never owns the window objects.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once
#include "IWindow.h"
#include "aex/weak_ptr/weak_ptr.h"
#include "window_info.h"

#include <QHash>
#include <QList>
#include <QObject>
#include <optional>
#include <unordered_map>

namespace cf::desktop {
class IWindowBackend;

/// Hash functor for QString, enables std::unordered_map<win_id_t, ...>.
struct QStringHash {
    std::size_t operator()(const QString& s) const noexcept { return qHash(s); }
};
/**
 * @brief  Records and manages windows created by the backend.
 *
 * @note   WindowManager never owns windows. It holds only weak
 *         references to windows created by the IWindowBackend.
 *
 * @ingroup components
 */
class WindowManager : public QObject {
    Q_OBJECT
  public:
    explicit WindowManager(QObject* parent = nullptr);

    /**
     * @brief  Sets the window backend used for window operations.
     *
     * @param[in]  backend  Weak reference to the window backend.
     */
    void setBackend(aex::WeakPtr<IWindowBackend> backend);

    /**
     * @brief  Creates and registers a new window with the given ID.
     *
     * If a window with @p win_id already exists, returns nullptr.
     * Otherwise delegates to the backend, registers the result, and
     * returns a weak reference to the new window.
     *
     * @param[in]  win_id  Unique identifier for the new window.
     *
     * @return aex::WeakPtr to the created window, or nullptr if duplicate.
     */
    aex::WeakPtr<IWindow> create_window(const win_id_t& win_id);

    /**
     * @brief  Finds a tracked window by its unique ID.
     *
     * @param[in]  win_id  The window identifier to look up.
     *
     * @return aex::WeakPtr to the window, or nullptr if not found / expired.
     */
    aex::WeakPtr<IWindow> find_window(const win_id_t& win_id) const;

    /**
     * @brief  Requests a graceful close for the given window.
     *
     * @param[in]  window  Weak reference to the target window.
     *
     * @return True if the close request was dispatched, false otherwise.
     */
    bool request_close_window(aex::WeakPtr<IWindow> window);

    /**
     * @brief  Raises the given window to the top of the stacking order.
     *
     * @param[in]  window  Weak reference to the target window.
     *
     * @return True if the raise request was dispatched, false otherwise.
     */
    bool raise_a_window(aex::WeakPtr<IWindow> window);

    // ── Window state machine (minimize / maximize / restore) ──────────────

    /**
     * @brief  Minimizes (iconifies) the tracked window with the given id.
     *
     * Validates the state transition (Normal -> Minimized), dispatches
     * IWindow::minimize on the live window, updates the stored WindowInfo, and
     * emits windowStateChanged + windowInfoUpdated.
     *
     * @param[in]  id  The window identifier.
     *
     * @return True on success; false when the id is unknown, the window is
     *         gone, or the transition is invalid.
     *
     * @throws None
     * @note   No-op signal-wise when the transition is rejected.
     * @since  0.21
     */
    bool minimizeWindow(const win_id_t& id);

    /**
     * @brief  Maximizes the tracked window with the given id.
     *
     * Validates the state transition (Normal -> Maximized), dispatches
     * IWindow::maximize, updates the stored WindowInfo, and emits
     * windowStateChanged + windowInfoUpdated.
     *
     * @param[in]  id  The window identifier.
     *
     * @return True on success; false when the id is unknown, the window is
     *         gone, or the transition is invalid.
     *
     * @throws None
     * @note   No-op signal-wise when the transition is rejected.
     * @since  0.21
     */
    bool maximizeWindow(const win_id_t& id);

    /**
     * @brief  Restores the tracked window (Minimized/Maximized -> Normal).
     *
     * Validates the state transition, dispatches IWindow::restore, updates the
     * stored WindowInfo, and emits windowStateChanged + windowInfoUpdated.
     *
     * @param[in]  id  The window identifier.
     *
     * @return True on success; false when the id is unknown, the window is
     *         gone, or the transition is invalid.
     *
     * @throws None
     * @note   No-op signal-wise when the transition is rejected.
     * @since  0.21
     */
    bool restoreWindow(const win_id_t& id);

    // ── Window queries ────────────────────────────────────────────────────

    /**
     * @brief  Returns the stored WindowInfo for the given id.
     *
     * @param[in]  id  The window identifier.
     *
     * @return A copy of the stored WindowInfo, or a WindowInfo with state ==
     *         Closed when the id is unknown (e.g. the window is gone).
     *
     * @throws None
     * @since  0.21
     */
    WindowInfo getWindowInfo(const win_id_t& id) const;

    /**
     * @brief  Returns snapshots of all currently tracked windows.
     *
     * @return A list of WindowInfo for every live tracked window.
     *
     * @throws None
     * @since  0.21
     */
    QList<WindowInfo> getAllWindowInfos() const;

    /**
     * @brief  Finds the tracked window id whose owning pid matches @p pid.
     *
     * @param[in]  pid  The process id to look up.
     *
     * @return The first matching window id, or std::nullopt when no tracked
     *         window has that pid.
     *
     * @throws None
     * @note   Returns the first match when several windows share a pid.
     * @since  0.21
     */
    std::optional<win_id_t> findWindowByPid(qint64 pid) const;

    /**
     * @brief  Registers a window directly (test hook).
     *
     * Forwards to the same path used when the backend reports window_came, so
     * tests can drive the state machine with a fake IWindow and no real
     * backend or display.
     *
     * @param[in]  window  Weak reference to the window to register.
     *
     * @throws None
     * @note   Test-only; production code registers windows via the backend.
     * @since  0.21
     */
    void registerWindowForTest(aex::WeakPtr<IWindow> window);

  signals:
    /**
     * @brief  Emitted when a tracked window appears.
     *
     * @param[in] pid  Owning process id (0 if unknown).
     */
    void windowAppeared(qint64 pid);

    /**
     * @brief  Emitted when a tracked window disappears.
     *
     * @param[in] pid  Owning process id captured at appearance.
     */
    void windowDisappeared(qint64 pid);

    /**
     * @brief  Emitted after a window's state changes.
     *
     * @param[in] id         The window identifier.
     * @param[in] newState   The new lifecycle state.
     *
     * @since 0.21
     */
    void windowStateChanged(const win_id_t& id, WindowState newState);

    /**
     * @brief  Emitted whenever a tracked WindowInfo is set or updated.
     *
     * @param[in] id    The window identifier.
     * @param[in] info  The updated window snapshot.
     *
     * @since 0.21
     */
    void windowInfoUpdated(const win_id_t& id, const WindowInfo& info);

  private:
    /// @brief Records a newly appeared window and watches its destruction.
    void onWindowCame(aex::WeakPtr<IWindow> window);

    /// @brief Returns whether a state transition is permitted by the model.
    static bool isValidTransition(WindowState from, WindowState to);

    /// @brief Drives a state transition end-to-end (validate, dispatch, emit).
    bool applyState(const win_id_t& id, WindowState to, void (IWindow::*op)());

    /// Weak reference to the window backend. Ownership: external.
    aex::WeakPtr<IWindowBackend> window_backend_{nullptr};
    /// Tracked windows keyed by window ID (weak references only). Ownership: backend.
    std::unordered_map<win_id_t, aex::WeakPtr<IWindow>, QStringHash> windows_;
    /// Observed WindowInfo keyed by window ID.
    std::unordered_map<win_id_t, WindowInfo, QStringHash> window_infos_;
};
} // namespace cf::desktop
