#include "WindowManager.h"
#include "IWindowBackend.h"

namespace cf::desktop {

WindowManager::WindowManager(QObject* parent) : QObject(parent) {}

void WindowManager::setBackend(WeakPtr<IWindowBackend> backend) {
    window_backend_ = std::move(backend);
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

} // namespace cf::desktop
