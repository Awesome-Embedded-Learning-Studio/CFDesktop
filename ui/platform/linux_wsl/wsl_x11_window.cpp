/**
 * @file    wsl_x11_window.cpp
 * @brief   Implementation of WSLX11Window — IWindow adapter for xcb_window_t.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#include "wsl_x11_window.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "cflog.h"

#    include <QRect>
#    include <QString>
#    include <cstring>
#    include <vector>

namespace cf::desktop::backend::wsl {

WSLX11Window::WSLX11Window(xcb_connection_t* conn, xcb_window_t root, xcb_window_t win,
                           const XcbAtoms& atoms, QObject* parent)
    : IWindow(parent), conn_(conn), root_(root), window_(win), atoms_(atoms) {
    pid_ = readPid();
}

WSLX11Window::~WSLX11Window() = default;

// ── IWindow interface ──────────────────────────────────────

win_id_t WSLX11Window::windowID() const {
    return QString("0x%1").arg(static_cast<uint32_t>(window_), 8, 16, QLatin1Char('0'));
}

QString WSLX11Window::title() const {
    if (!conn_ || window_ == XCB_WINDOW_NONE) {
        return {};
    }

    // Try _NET_WM_NAME (UTF-8) first
    if (atoms_.net_wm_name != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(conn_, 0, window_, atoms_.net_wm_name, XCB_ATOM_ANY, 0, 1024);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) > 0) {
            const char* data = static_cast<const char*>(xcb_get_property_value(reply));
            int len = xcb_get_property_value_length(reply);
            QString result = QString::fromUtf8(data, len);
            free(reply);
            return result;
        }
        free(reply);
    }

    // Fallback to WM_NAME (STRING / COMPOUND_TEXT)
    if (atoms_.wm_name != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(conn_, 0, window_, atoms_.wm_name, XCB_ATOM_ANY, 0, 1024);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) > 0) {
            const char* data = static_cast<const char*>(xcb_get_property_value(reply));
            int len = xcb_get_property_value_length(reply);
            QString result = QString::fromLatin1(data, len);
            free(reply);
            return result;
        }
        free(reply);
    }

    return {};
}

QRect WSLX11Window::geometry() const {
    if (!conn_ || window_ == XCB_WINDOW_NONE) {
        return {};
    }

    xcb_get_geometry_cookie_t geo_cookie = xcb_get_geometry(conn_, window_);
    xcb_get_geometry_reply_t* geo = xcb_get_geometry_reply(conn_, geo_cookie, nullptr);
    if (!geo) {
        return {};
    }

    // Translate to root-relative coordinates
    xcb_translate_coordinates_cookie_t trans_cookie =
        xcb_translate_coordinates(conn_, window_, root_, 0, 0);
    xcb_translate_coordinates_reply_t* trans =
        xcb_translate_coordinates_reply(conn_, trans_cookie, nullptr);

    QRect result;
    if (trans) {
        result = QRect(trans->dst_x, trans->dst_y, geo->width, geo->height);
        free(trans);
    } else {
        // Fallback: use parent-relative coordinates
        result = QRect(geo->x, geo->y, geo->width, geo->height);
    }

    free(geo);
    return result;
}

void WSLX11Window::set_geometry(const QRect& r) {
    if (!conn_ || window_ == XCB_WINDOW_NONE) {
        return;
    }

    const uint32_t values[] = {
        static_cast<uint32_t>(r.x()),
        static_cast<uint32_t>(r.y()),
        static_cast<uint32_t>(r.width()),
        static_cast<uint32_t>(r.height()),
    };
    xcb_configure_window(conn_, window_,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
                             XCB_CONFIG_WINDOW_HEIGHT,
                         values);
    xcb_flush(conn_);
}

void WSLX11Window::requestClose() {
    if (!conn_ || window_ == XCB_WINDOW_NONE) {
        return;
    }

    // Check if the window supports WM_DELETE_WINDOW protocol
    bool supports_delete = false;
    if (atoms_.wm_protocols != XCB_ATOM_NONE && atoms_.wm_delete_window != XCB_ATOM_NONE) {
        xcb_get_property_cookie_t cookie =
            xcb_get_property(conn_, 0, window_, atoms_.wm_protocols, XCB_ATOM_ATOM, 0, 1024);
        xcb_get_property_reply_t* reply = xcb_get_property_reply(conn_, cookie, nullptr);
        if (reply && reply->type != XCB_ATOM_NONE) {
            xcb_atom_t* atoms = static_cast<xcb_atom_t*>(xcb_get_property_value(reply));
            int count = xcb_get_property_value_length(reply) / sizeof(xcb_atom_t);
            for (int i = 0; i < count; ++i) {
                if (atoms[i] == atoms_.wm_delete_window) {
                    supports_delete = true;
                    break;
                }
            }
        }
        free(reply);
    }

    if (supports_delete) {
        // Send WM_DELETE_WINDOW ClientMessage
        xcb_client_message_event_t event;
        std::memset(&event, 0, sizeof(event));
        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = window_;
        event.type = atoms_.wm_protocols;
        event.format = 32;
        event.data.data32[0] = atoms_.wm_delete_window;
        event.data.data32[1] = XCB_CURRENT_TIME;

        xcb_send_event(conn_, 0, window_, XCB_EVENT_MASK_NO_EVENT,
                       reinterpret_cast<const char*>(&event));
        xcb_flush(conn_);
    } else {
        // Forceful kill
        xcb_kill_client(conn_, window_);
        xcb_flush(conn_);
    }
}

void WSLX11Window::raise() {
    if (!conn_ || window_ == XCB_WINDOW_NONE) {
        return;
    }

    const uint32_t values[] = {XCB_STACK_MODE_ABOVE};
    xcb_configure_window(conn_, window_, XCB_CONFIG_WINDOW_STACK_MODE, values);
    xcb_flush(conn_);
}

qint64 WSLX11Window::pid() const {
    return pid_;
}

qint64 WSLX11Window::readPid() const {
    if (!conn_ || window_ == XCB_WINDOW_NONE || atoms_.net_wm_pid == XCB_ATOM_NONE) {
        return 0;
    }
    xcb_get_property_cookie_t cookie =
        xcb_get_property(conn_, 0, window_, atoms_.net_wm_pid, XCB_ATOM_CARDINAL, 0, 1);
    xcb_get_property_reply_t* reply = xcb_get_property_reply(conn_, cookie, nullptr);
    qint64 pid = 0;
    if (reply && reply->type != XCB_ATOM_NONE && xcb_get_property_value_length(reply) >= 4) {
        pid = static_cast<qint64>(*static_cast<uint32_t*>(xcb_get_property_value(reply)));
    }
    free(reply);
    return pid;
}

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
