/**
 * @file    wsl_x11_window_backend.cpp
 * @brief   Implementation of WSLX11WindowBackend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#include "wsl_x11_window_backend.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "cflog.h"
#    include "qt_format.h"

#    include <QRect>
#    include <QString>
#    include <cstring>
#    include <unistd.h>
#    include <xcb/xcb.h>

namespace cf::desktop::backend::wsl {

// ──────────────────────────────────────────────────────────
//  Construction / Destruction
// ──────────────────────────────────────────────────────────

WSLX11WindowBackend::WSLX11WindowBackend(QObject* parent) : IWindowBackend(parent) {}

WSLX11WindowBackend::~WSLX11WindowBackend() {
    stopTracking();
}

// ──────────────────────────────────────────────────────────
//  Tracking lifecycle
// ──────────────────────────────────────────────────────────

bool WSLX11WindowBackend::startTracking(xcb_connection_t* conn, xcb_window_t root) {
    if (!conn) {
        cf::log::errorftag("WSLWindowBackend", "XCB connection is null");
        return false;
    }

    xcb_conn_ = conn;
    root_window_ = root;

    // Intern commonly used atoms
    if (!internAtoms()) {
        cf::log::errorftag("WSLWindowBackend", "Failed to intern X11 atoms");
        xcb_conn_ = nullptr;
        root_window_ = XCB_WINDOW_NONE;
        return false;
    }

    // Select SubstructureNotifyMask on the root window to receive
    // CreateNotify / DestroyNotify for child windows.
    const uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_void_cookie_t cookie =
        xcb_change_window_attributes_checked(conn, root, XCB_CW_EVENT_MASK, &mask);
    xcb_generic_error_t* error = xcb_request_check(conn, cookie);
    if (error) {
        cf::log::errorftag("WSLWindowBackend",
                           "Failed to set SubstructureNotifyMask on root window "
                           "(error={})",
                           static_cast<int>(error->error_code));
        free(error);
        xcb_conn_ = nullptr;
        root_window_ = XCB_WINDOW_NONE;
        return false;
    }

    // Integrate XCB events into Qt event loop
    int fd = xcb_get_file_descriptor(conn);
    if (fd < 0) {
        cf::log::errorftag("WSLWindowBackend", "Failed to get XCB file descriptor");
        xcb_conn_ = nullptr;
        root_window_ = XCB_WINDOW_NONE;
        return false;
    }

    notifier_ = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier_, &QSocketNotifier::activated, this, &WSLX11WindowBackend::processXcbEvents);

    // Scan existing windows via xcb_query_tree
    xcb_query_tree_cookie_t tree_cookie = xcb_query_tree(conn, root);
    xcb_query_tree_reply_t* tree = xcb_query_tree_reply(conn, tree_cookie, nullptr);
    if (tree) {
        int num_children = xcb_query_tree_children_length(tree);
        xcb_window_t* children = xcb_query_tree_children(tree);
        for (int i = 0; i < num_children; ++i) {
            xcb_window_t child = children[i];
            if (shouldTrackWindow(child)) {
                registerWindow(child);
            }
        }
        free(tree);
    }

    xcb_flush(conn);

    cf::log::traceftag("WSLWindowBackend", "Tracking started, {} windows discovered",
                       tracked_windows_.size());
    return true;
}

void WSLX11WindowBackend::stopTracking() {
    if (notifier_) {
        delete notifier_;
        notifier_ = nullptr;
    }

    tracked_windows_.clear();

    xcb_conn_ = nullptr;
    root_window_ = XCB_WINDOW_NONE;
}

// ──────────────────────────────────────────────────────────
//  IWindowBackend interface
// ──────────────────────────────────────────────────────────

WeakPtr<IWindow> WSLX11WindowBackend::createWindow(const QString& appId) {
    Q_UNUSED(appId);
    cf::log::warningftag("WSLWindowBackend", "createWindow() not supported in X11 tracking mode");
    return {};
}

void WSLX11WindowBackend::destroyWindow(WeakPtr<IWindow> window) {
    if (!window) {
        return;
    }
    auto* ww = dynamic_cast<WSLX11Window*>(window.Get());
    if (!ww) {
        return;
    }
    unregisterWindow(ww->nativeHandle());
}

QList<WeakPtr<IWindow>> WSLX11WindowBackend::windows() const {
    QList<WeakPtr<IWindow>> result;
    result.reserve(static_cast<int>(tracked_windows_.size()));
    for (const auto& [win, ptr] : tracked_windows_) {
        result.append(ptr->make_weak());
    }
    return result;
}

render::BackendCapabilities WSLX11WindowBackend::capabilities() const {
    return render::BackendCapabilities{
        .supportsMultiWindow = true,
        .supportsTransparency = true,
        .hasHardwareAcceleration = true,
        .supportsVSync = false,
        .supportsScreenshot = true,
        .maxTextureSize = 4096,
    };
}

// ──────────────────────────────────────────────────────────
//  Atom interning
// ──────────────────────────────────────────────────────────

xcb_atom_t WSLX11WindowBackend::internAtom(const char* name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xcb_conn_, 0, std::strlen(name), name);
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(xcb_conn_, cookie, nullptr);
    if (!reply) {
        return XCB_ATOM_NONE;
    }
    xcb_atom_t atom = reply->atom;
    free(reply);
    return atom;
}

bool WSLX11WindowBackend::internAtoms() {
    atoms_.net_wm_name = internAtom("_NET_WM_NAME");
    atoms_.wm_name = internAtom("WM_NAME");
    atoms_.net_wm_pid = internAtom("_NET_WM_PID");
    atoms_.wm_protocols = internAtom("WM_PROTOCOLS");
    atoms_.wm_delete_window = internAtom("WM_DELETE_WINDOW");
    atoms_.net_wm_window_type = internAtom("_NET_WM_WINDOW_TYPE");
    atoms_.net_wm_window_type_dock = internAtom("_NET_WM_WINDOW_TYPE_DOCK");
    atoms_.net_wm_window_type_desktop = internAtom("_NET_WM_WINDOW_TYPE_DESKTOP");
    atoms_.utf8_string = internAtom("UTF8_STRING");

    cf::log::traceftag("WSLWindowBackend", "Atoms interned: _NET_WM_NAME={}, WM_NAME={}",
                       static_cast<uint32_t>(atoms_.net_wm_name),
                       static_cast<uint32_t>(atoms_.wm_name));
    return atoms_.net_wm_name != XCB_ATOM_NONE || atoms_.wm_name != XCB_ATOM_NONE;
}

// ──────────────────────────────────────────────────────────
//  XCB event processing
// ──────────────────────────────────────────────────────────

void WSLX11WindowBackend::processXcbEvents() {
    if (!xcb_conn_) {
        return;
    }

    while (xcb_generic_event_t* event = xcb_poll_for_event(xcb_conn_)) {
        const uint8_t response_type = event->response_type & ~0x80;

        switch (response_type) {
            case XCB_CREATE_NOTIFY: {
                auto* create = reinterpret_cast<xcb_create_notify_event_t*>(event);
                if (!create->override_redirect && shouldTrackWindow(create->window)) {
                    registerWindow(create->window);
                }
                break;
            }

            case XCB_DESTROY_NOTIFY: {
                auto* destroy = reinterpret_cast<xcb_destroy_notify_event_t*>(event);
                unregisterWindow(destroy->window);
                break;
            }

            case XCB_MAP_NOTIFY: {
                auto* map = reinterpret_cast<xcb_map_notify_event_t*>(event);
                if (!map->override_redirect && shouldTrackWindow(map->window)) {
                    registerWindow(map->window);
                }
                break;
            }

            case XCB_UNMAP_NOTIFY: {
                auto* unmap = reinterpret_cast<xcb_unmap_notify_event_t*>(event);
                // Don't unregister on unmap — window still exists, just hidden.
                // We'll unregister on DestroyNotify.
                Q_UNUSED(unmap);
                break;
            }

            case XCB_CONFIGURE_NOTIFY: {
                auto* cfg = reinterpret_cast<xcb_configure_notify_event_t*>(event);
                auto it = tracked_windows_.find(cfg->window);
                if (it != tracked_windows_.end()) {
                    // Emit geometry changed signal via IWindow
                    QRect old_rect = it->second->geometry();
                    QRect new_rect(cfg->x, cfg->y, cfg->width, cfg->height);
                    if (old_rect != new_rect) {
                        emit it->second->geometryChanged(old_rect, new_rect);
                    }
                }
                break;
            }

            case XCB_PROPERTY_NOTIFY: {
                auto* prop = reinterpret_cast<xcb_property_notify_event_t*>(event);
                auto it = tracked_windows_.find(prop->window);
                if (it != tracked_windows_.end()) {
                    if (prop->atom == atoms_.net_wm_name || prop->atom == atoms_.wm_name) {
                        QString old_title = it->second->title();
                        // Title will be re-queried on next title() call
                        emit it->second->titleChanged(old_title, old_title);
                    }
                }
                break;
            }

            default:
                break;
        }

        free(event);
    }
}

// ──────────────────────────────────────────────────────────
//  Window filtering
// ──────────────────────────────────────────────────────────

bool WSLX11WindowBackend::shouldTrackWindow(xcb_window_t win) {
    if (win == XCB_WINDOW_NONE || win == root_window_) {
        return false;
    }

    // Avoid duplicates
    if (tracked_windows_.count(win)) {
        return false;
    }

    // 1. Check window attributes — must be viewable and not override-redirect
    xcb_get_window_attributes_cookie_t attr_cookie = xcb_get_window_attributes(xcb_conn_, win);
    xcb_get_window_attributes_reply_t* attr =
        xcb_get_window_attributes_reply(xcb_conn_, attr_cookie, nullptr);
    if (!attr) {
        return false;
    }

    bool viewable = (attr->map_state == XCB_MAP_STATE_VIEWABLE);
    bool override_redirect = attr->override_redirect;
    free(attr);

    if (!viewable || override_redirect) {
        return false;
    }

    // 2. Must have a title
    bool has_title = false;
    if (atoms_.net_wm_name != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(xcb_conn_, 0, win, atoms_.net_wm_name, XCB_ATOM_ANY, 0, 256);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(xcb_conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) > 0) {
            has_title = true;
        }
        free(reply);
    }
    if (!has_title && atoms_.wm_name != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(xcb_conn_, 0, win, atoms_.wm_name, XCB_ATOM_ANY, 0, 256);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(xcb_conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) > 0) {
            has_title = true;
        }
        free(reply);
    }
    if (!has_title) {
        return false;
    }

    // 3. Skip our own process windows
    if (atoms_.net_wm_pid != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(xcb_conn_, 0, win, atoms_.net_wm_pid, XCB_ATOM_CARDINAL, 0, 1);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(xcb_conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) >= 4) {
            uint32_t pid = *static_cast<uint32_t*>(xcb_get_property_value(reply));
            if (pid == static_cast<uint32_t>(getpid())) {
                free(reply);
                return false;
            }
        }
        free(reply);
    }

    // 4. Skip dock and desktop windows
    if (atoms_.net_wm_window_type != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(xcb_conn_, 0, win, atoms_.net_wm_window_type, XCB_ATOM_ATOM, 0, 16);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(xcb_conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE) {
            xcb_atom_t* types = static_cast<xcb_atom_t*>(xcb_get_property_value(reply));
            int count = xcb_get_property_value_length(reply) / sizeof(xcb_atom_t);
            for (int i = 0; i < count; ++i) {
                if (types[i] == atoms_.net_wm_window_type_dock ||
                    types[i] == atoms_.net_wm_window_type_desktop) {
                    free(reply);
                    return false;
                }
            }
        }
        free(reply);
    }

    return true;
}

// ──────────────────────────────────────────────────────────
//  Registration helpers
// ──────────────────────────────────────────────────────────

void WSLX11WindowBackend::registerWindow(xcb_window_t win) {
    if (tracked_windows_.count(win)) {
        return;
    }

    auto wsl_win = std::make_unique<WSLX11Window>(xcb_conn_, root_window_, win, atoms_, this);
    auto weak = wsl_win->make_weak();
    const QString title = wsl_win->title();

    tracked_windows_[win] = std::move(wsl_win);

    cf::log::debugftag("WSLWindowBackend", "Window appeared: win=0x{:08x} title='{}'",
                       static_cast<uint32_t>(win), title.toStdString());

    emit window_came(weak);
}

void WSLX11WindowBackend::unregisterWindow(xcb_window_t win) {
    auto it = tracked_windows_.find(win);
    if (it == tracked_windows_.end()) {
        return;
    }

    auto weak = it->second->make_weak();

    cf::log::debugftag("WSLWindowBackend", "Window gone: win=0x{:08x}", static_cast<uint32_t>(win));

    tracked_windows_.erase(it);

    emit window_gone(weak);
}

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
