/**
 * @file    windows_window_backend.cpp
 * @brief   Implementation of WindowsWindowBackend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#include "windows_window_backend.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    ifdef ERROR
#        undef ERROR
#    endif
#    include "cflog.h"
#    include "qt_format.h"

#    include <QString>
#    include <psapi.h>

namespace cf::desktop::backend::windows {

// ──────────────────────────────────────────────────────────
//  Global state for the static WinEvent callback
// ──────────────────────────────────────────────────────────

namespace {
/// Pointer to the single active backend instance.
/// Safe because: (1) only one backend exists, (2) all access
/// is on the GUI thread (WINEVENT_OUTOFCONTEXT guarantees this).
WindowsWindowBackend* g_active_backend = nullptr;

/**
 * @brief  WinEvent callback — routes OS notifications to the backend.
 *
 * Called by Windows on the thread that installed the hook (the GUI
 * thread, because we use WINEVENT_OUTOFCONTEXT).
 */
void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event, HWND hwnd, LONG idObject,
                           LONG idChild, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/) {
    // We only care about window-level events, not sub-objects.
    if (idObject != OBJID_WINDOW) {
        return;
    }
    if (!g_active_backend) {
        return;
    }

    switch (event) {
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_CREATE:
            g_active_backend->onExternalWindowShown(hwnd);
            break;

        case EVENT_OBJECT_DESTROY:
            g_active_backend->onExternalWindowDestroyed(hwnd);
            break;

        default:
            break;
    }
}

/**
 * @brief  Callback for EnumWindows — scans existing top-level windows.
 */
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* backend = reinterpret_cast<WindowsWindowBackend*>(lParam);
    backend->onExternalWindowShown(hwnd);
    return TRUE; // continue enumeration
}

} // anonymous namespace

// ──────────────────────────────────────────────────────────
//  Construction / Destruction
// ──────────────────────────────────────────────────────────

WindowsWindowBackend::WindowsWindowBackend(QObject* parent) : IWindowBackend(parent) {}

WindowsWindowBackend::~WindowsWindowBackend() {
    stopTracking();
}

// ──────────────────────────────────────────────────────────
//  Tracking lifecycle
// ──────────────────────────────────────────────────────────

bool WindowsWindowBackend::startTracking() {
    if (g_active_backend) {
        log::warningftag("WinWindowBackend", "Another backend is already active; skipping");
        return false;
    }

    g_active_backend = this;

    // Hook: window show / destroy (all processes, out-of-context)
    hook_show_destroy_ = SetWinEventHook(EVENT_OBJECT_CREATE,  // 0x8000
                                         EVENT_OBJECT_SHOW,    // 0x8002,
                                         nullptr,              // no DLL
                                         WinEventProc,         // callback
                                         0,                    // all processes
                                         0,                    // all threads
                                         WINEVENT_OUTOFCONTEXT // called on our thread
    );

    if (!hook_show_destroy_) {
        log::errorftag("WinWindowBackend", "SetWinEventHook (show/destroy) failed: {}",
                       GetLastError());
        g_active_backend = nullptr;
        return false;
    }

    // Enumerate windows that are already open
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));

    log::traceftag("WinWindowBackend", "Tracking started, {} windows discovered",
                   tracked_windows_.size());
    return true;
}

void WindowsWindowBackend::stopTracking() {
    if (hook_show_destroy_) {
        UnhookWinEvent(hook_show_destroy_);
        hook_show_destroy_ = nullptr;
    }
    if (hook_name_change_) {
        UnhookWinEvent(hook_name_change_);
        hook_name_change_ = nullptr;
    }

    tracked_windows_.clear();

    if (g_active_backend == this) {
        g_active_backend = nullptr;
    }
}

// ──────────────────────────────────────────────────────────
//  IWindowBackend interface
// ──────────────────────────────────────────────────────────

WeakPtr<IWindow> WindowsWindowBackend::createWindow(const QString& appId) {
    // For the pseudo-desktop, internal window creation is not yet
    // implemented.  External windows are discovered via hooks.
    Q_UNUSED(appId);
    log::warningftag("WinWindowBackend", "createWindow() not yet supported in pseudo-desktop mode");
    return {};
}

void WindowsWindowBackend::destroyWindow(WeakPtr<IWindow> window) {
    // For external windows, "destroy" means stop tracking —
    // we don't actually close the window.
    if (!window) {
        return;
    }
    auto* ww = dynamic_cast<WindowsWindow*>(window.Get());
    if (!ww) {
        return;
    }
    unregisterWindow(ww->nativeHandle());
}

QList<WeakPtr<IWindow>> WindowsWindowBackend::windows() const {
    QList<WeakPtr<IWindow>> result;
    result.reserve(static_cast<int>(tracked_windows_.size()));
    for (const auto& [hwnd, win] : tracked_windows_) {
        result.append(win->make_weak());
    }
    return result;
}

render::BackendCapabilities WindowsWindowBackend::capabilities() const {
    return render::BackendCapabilities{
        .supportsMultiWindow = true,
        .supportsTransparency = true,
        .hasHardwareAcceleration = true,
        .supportsVSync = true,
        .supportsScreenshot = true,
        .maxTextureSize = 8192,
    };
}

// ──────────────────────────────────────────────────────────
//  Hook event handlers
// ──────────────────────────────────────────────────────────

void WindowsWindowBackend::onExternalWindowShown(HWND hwnd) {
    if (!shouldTrackWindow(hwnd)) {
        return;
    }
    // Avoid duplicates
    if (tracked_windows_.count(hwnd)) {
        return;
    }
    registerWindow(hwnd);
}

void WindowsWindowBackend::onExternalWindowDestroyed(HWND hwnd) {
    unregisterWindow(hwnd);
}

// ──────────────────────────────────────────────────────────
//  Internal helpers
// ──────────────────────────────────────────────────────────

bool WindowsWindowBackend::shouldTrackWindow(HWND hwnd) {
    // 1. Must be a valid window
    if (!IsWindow(hwnd)) {
        return false;
    }

    // 2. Must be visible
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    // 3. Must be a top-level window (no owner or parent)
    //    GetAncestor with GA_ROOT returns the root ancestor.
    //    If the root isn't itself, it's a child.
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    // 4. Must have a title (skip invisible tool windows, popups, etc.)
    if (GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }

    // 5. Skip our own process windows
    DWORD winPid = 0;
    GetWindowThreadProcessId(hwnd, &winPid);
    if (winPid == GetCurrentProcessId()) {
        return false;
    }

    // 6. Skip windows with WS_EX_TOOLWINDOW but no visible frame
    //    (these are typically helper / notification windows)
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    // 7. Skip the desktop ("Program Manager") and taskbar ("Shell_TrayWnd")
    wchar_t className[256] = {};
    GetClassNameW(hwnd, className, 256);
    if (wcscmp(className, L"Progman") == 0 ||       // Desktop
        wcscmp(className, L"Shell_TrayWnd") == 0 || // Taskbar
        wcscmp(className, L"WorkerW") == 0 ||       // Desktop wallpaper worker
        wcscmp(className, L"IME") == 0) {           // Input method
        return false;
    }

    return true;
}

void WindowsWindowBackend::registerWindow(HWND hwnd) {
    auto win = std::make_unique<WindowsWindow>(hwnd, this);
    auto weak = win->make_weak();
    const QString title = win->title();

    tracked_windows_[hwnd] = std::move(win);

    log::debugftag("WinWindowBackend", "Window appeared: hwnd=0x{:x} title='{}'",
                   reinterpret_cast<std::uintptr_t>(hwnd), title);

    emit window_came(weak);
}

void WindowsWindowBackend::unregisterWindow(HWND hwnd) {
    auto it = tracked_windows_.find(hwnd);
    if (it == tracked_windows_.end()) {
        return;
    }

    auto weak = it->second->make_weak();

    log::debugftag("WinWindowBackend", "Window gone: hwnd=0x{:x}",
                   reinterpret_cast<std::uintptr_t>(hwnd));

    tracked_windows_.erase(it);

    emit window_gone(weak);
}

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
