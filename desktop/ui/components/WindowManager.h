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
#include "base/weak_ptr/weak_ptr.h"

#include <QHash>
#include <QObject>
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
    void setBackend(WeakPtr<IWindowBackend> backend);

    /**
     * @brief  Creates and registers a new window with the given ID.
     *
     * If a window with @p win_id already exists, returns nullptr.
     * Otherwise delegates to the backend, registers the result, and
     * returns a weak reference to the new window.
     *
     * @param[in]  win_id  Unique identifier for the new window.
     *
     * @return WeakPtr to the created window, or nullptr if duplicate.
     */
    WeakPtr<IWindow> create_window(const win_id_t& win_id);

    /**
     * @brief  Finds a tracked window by its unique ID.
     *
     * @param[in]  win_id  The window identifier to look up.
     *
     * @return WeakPtr to the window, or nullptr if not found / expired.
     */
    WeakPtr<IWindow> find_window(const win_id_t& win_id) const;

    /**
     * @brief  Requests a graceful close for the given window.
     *
     * @param[in]  window  Weak reference to the target window.
     *
     * @return True if the close request was dispatched, false otherwise.
     */
    bool request_close_window(WeakPtr<IWindow> window);

    /**
     * @brief  Raises the given window to the top of the stacking order.
     *
     * @param[in]  window  Weak reference to the target window.
     *
     * @return True if the raise request was dispatched, false otherwise.
     */
    bool raise_a_window(WeakPtr<IWindow> window);

  private:
    /// Weak reference to the window backend. Ownership: external.
    WeakPtr<IWindowBackend> window_backend_{nullptr};
    /// Tracked windows keyed by window ID (weak references only). Ownership: backend.
    std::unordered_map<win_id_t, WeakPtr<IWindow>, QStringHash> windows_;
};
} // namespace cf::desktop
