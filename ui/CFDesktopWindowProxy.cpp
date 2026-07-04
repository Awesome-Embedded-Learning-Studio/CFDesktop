#include "CFDesktopWindowProxy.h"
#include "CFDesktop.h"
#include "CFDesktopEntity.h"
#include "CFDesktopProxy.h"
#include <QWidget>

namespace cf::desktop {

CFDesktopWindowProxy::CFDesktopWindowProxy()
    : CFDesktopProxy(CFDesktopEntity::instance().desktop_widget()) {}

void CFDesktopWindowProxy::set_window_display_strategy(DesktopDisplayStrategy s) {
    s_ = std::move(s);
}

bool CFDesktopWindowProxy::activate_window_display_strategy() {
    if (!s_) {
        return false;
    }
    QWidget* desktop_widget = qobject_cast<QWidget*>(desktop_.Get());

    if (!desktop_widget) {
        return false;
    }

    return s_->action(desktop_widget);
}

bool CFDesktopWindowProxy::show_desktop() {
    desktop_->show();
    return true; // OK, desktop can safely show
}

} // namespace cf::desktop