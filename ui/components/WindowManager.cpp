#include "WindowManager.h"
#include "IWindowBackend.h"

#include <QDateTime>

namespace cf::desktop {

WindowManager::WindowManager(QObject* parent) : QObject(parent) {}

void WindowManager::setBackend(aex::WeakPtr<IWindowBackend> backend) {
    window_backend_ = std::move(backend);
    if (auto* backend_raw = window_backend_.Get()) {
        connect(backend_raw, &IWindowBackend::window_came, this, &WindowManager::onWindowCame);
    }
}

aex::WeakPtr<IWindow> WindowManager::create_window(const win_id_t& win_id) {
    if (windows_.find(win_id) != windows_.end())
        return nullptr;

    if (!window_backend_)
        return nullptr;

    auto window = window_backend_->createWindow(win_id);
    if (!window)
        return nullptr;

    windows_[win_id] = window;
    return window;
}

aex::WeakPtr<IWindow> WindowManager::find_window(const win_id_t& win_id) const {
    auto it = windows_.find(win_id);
    if (it == windows_.end())
        return nullptr;

    if (!it->second)
        return nullptr;

    return it->second;
}

bool WindowManager::request_close_window(aex::WeakPtr<IWindow> window) {
    if (!window)
        return false;
    window->requestClose();
    return true;
}

bool WindowManager::raise_a_window(aex::WeakPtr<IWindow> window) {
    if (!window)
        return false;
    window->raise();
    return true;
}

void WindowManager::onWindowCame(aex::WeakPtr<IWindow> window) {
    if (!window) {
        return;
    }
    auto* window_raw = window.Get();
    if (window_raw == nullptr) {
        return;
    }
    const win_id_t id = window_raw->windowID();
    WindowInfo info;
    info.window_id = id;
    info.title = window_raw->title();
    info.pid = window_raw->pid();
    info.geometry = window_raw->geometry();
    info.state = WindowState::Normal;
    info.created_at = QDateTime::currentDateTime();
    // icon_hint / z_index / is_always_on_top keep their defaults (empty/0/
    // false); the tracker does not observe them today.
    window_infos_[id] = info;
    // Also index the weak reference so find_window / request_close_window /
    // raise_a_window / the state machine can resolve externally-arrived
    // windows (not just those from create_window).
    windows_[id] = std::move(window);

    // The backend emits window_gone after the IWindow object is already
    // destroyed (its weak reference expires), so that signal cannot carry the
    // id. Watch QObject::destroyed instead: it fires while the entry is still
    // identifiable via the captured id.
    connect(window_raw, &QObject::destroyed, this, [this, id, pid = info.pid]() {
        // Emit the Closed transition BEFORE erasing so subscribers to
        // windowStateChanged / windowInfoUpdated observe the final state. After
        // erase, getWindowInfo(id) returns a Closed sentinel (state == Closed).
        auto it = window_infos_.find(id);
        if (it != window_infos_.end()) {
            it->second.state = WindowState::Closed;
            emit windowStateChanged(id, WindowState::Closed);
            emit windowInfoUpdated(id, it->second);
            window_infos_.erase(it);
        }
        windows_.erase(id);
        emit windowDisappeared(pid);
    });

    emit windowAppeared(info.pid);
    emit windowInfoUpdated(id, info);
}

// ── State machine ──────────────────────────────────────────────────────────

bool WindowManager::isValidTransition(WindowState from, WindowState to) {
    if (from == to) {
        return false;
    }
    // Closed is terminal; Closing only resolves to Closed.
    if (from == WindowState::Closed) {
        return false;
    }
    if (from == WindowState::Closing) {
        return to == WindowState::Closed;
    }
    switch (to) {
        case WindowState::Minimized:
        case WindowState::Maximized:
            // Only reachable from the resting Normal state.
            return from == WindowState::Normal;
        case WindowState::Normal:
            // Restore from Minimized or Maximized.
            return from == WindowState::Minimized || from == WindowState::Maximized;
        case WindowState::Closing:
            // Any live state may request a close.
            return true;
        case WindowState::Fullscreen:
            // Out of scope for this slice.
            return false;
        case WindowState::Closed:
            // Closed is only reached via the destroyed observation, not here.
            return false;
    }
    return false;
}

bool WindowManager::applyState(const win_id_t& id, WindowState to, void (IWindow::*op)()) {
    auto it = window_infos_.find(id);
    if (it == window_infos_.end()) {
        return false;
    }
    if (!isValidTransition(it->second.state, to)) {
        return false;
    }
    // Re-fetch the live window and re-check: it may have been torn down
    // between the caller's lookup and here (Qt direct connection keeps this
    // same-thread, so in practice the window cannot vanish mid-call, but the
    // guard is cheap).
    auto* raw = find_window(id).Get();
    if (raw == nullptr) {
        return false;
    }
    (raw->*op)();
    it->second.state = to;
    emit windowStateChanged(id, to);
    emit windowInfoUpdated(id, it->second);
    return true;
}

bool WindowManager::minimizeWindow(const win_id_t& id) {
    return applyState(id, WindowState::Minimized, &IWindow::minimize);
}

bool WindowManager::maximizeWindow(const win_id_t& id) {
    return applyState(id, WindowState::Maximized, &IWindow::maximize);
}

bool WindowManager::restoreWindow(const win_id_t& id) {
    return applyState(id, WindowState::Normal, &IWindow::restore);
}

// ── Queries ────────────────────────────────────────────────────────────────

WindowInfo WindowManager::getWindowInfo(const win_id_t& id) const {
    const auto it = window_infos_.find(id);
    if (it != window_infos_.end()) {
        return it->second;
    }
    // Unknown id (e.g. window already gone): report the terminal state so
    // callers can treat absence uniformly.
    WindowInfo gone;
    gone.state = WindowState::Closed;
    return gone;
}

QList<WindowInfo> WindowManager::getAllWindowInfos() const {
    QList<WindowInfo> out;
    out.reserve(static_cast<int>(window_infos_.size()));
    for (const auto& kv : window_infos_) {
        out.append(kv.second);
    }
    return out;
}

std::optional<win_id_t> WindowManager::findWindowByPid(qint64 pid) const {
    if (pid == 0) {
        return std::nullopt;
    }
    for (const auto& kv : window_infos_) {
        if (kv.second.pid == pid) {
            return kv.first;
        }
    }
    return std::nullopt;
}

void WindowManager::registerWindowForTest(aex::WeakPtr<IWindow> window) {
    onWindowCame(std::move(window));
}

} // namespace cf::desktop
