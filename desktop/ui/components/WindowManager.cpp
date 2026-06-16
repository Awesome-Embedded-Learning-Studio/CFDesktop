#include "WindowManager.h"
#include "IWindowBackend.h"

namespace cf::desktop {

WindowManager::WindowManager(QObject* parent) : QObject(parent) {}

void WindowManager::setBackend(WeakPtr<IWindowBackend> backend) {
    window_backend_ = std::move(backend);
    if (auto* backend_raw = window_backend_.Get()) {
        connect(backend_raw, &IWindowBackend::window_came, this, &WindowManager::onWindowCame);
    }
}

WeakPtr<IWindow> WindowManager::create_window(const win_id_t& win_id) {
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

WeakPtr<IWindow> WindowManager::find_window(const win_id_t& win_id) const {
    auto it = windows_.find(win_id);
    if (it == windows_.end())
        return nullptr;

    if (!it->second)
        return nullptr;

    return it->second;
}

bool WindowManager::request_close_window(WeakPtr<IWindow> window) {
    if (!window)
        return false;
    window->requestClose();
    return true;
}

bool WindowManager::raise_a_window(WeakPtr<IWindow> window) {
    if (!window)
        return false;
    window->raise();
    return true;
}

void WindowManager::onWindowCame(WeakPtr<IWindow> window) {
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
    window_infos_[id] = info;

    // The backend emits window_gone after the IWindow object is already
    // destroyed (its weak reference expires), so that signal cannot carry the
    // id. Watch QObject::destroyed instead: it fires while the entry is still
    // identifiable via the captured id.
    connect(window_raw, &QObject::destroyed, this, [this, id, pid = info.pid]() {
        window_infos_.erase(id);
        emit windowDisappeared(pid);
    });

    emit windowAppeared(info.pid);
}

} // namespace cf::desktop
