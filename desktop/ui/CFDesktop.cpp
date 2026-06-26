/**
 * @file CFDesktop.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Welcomes Center!
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "CFDesktop.h"
#include "CFDesktopEntity.h"
#include "cflog.h"
#include "components/PanelManager.h"

#include <QMoveEvent>
#include <QResizeEvent>

namespace cf::desktop {

CFDesktop::CFDesktop(CFDesktopEntity* entity) : weak_ptr_factory_(this) {
    log::traceftag("CFDesktop Self", "Creating CFDesktop...");
};

void CFDesktop::register_desktop_resources(InitResources& resources) {
    log::traceftag("CFDesktop Self", "Register Desktop Resources from remote");
    panel_manager_ = resources.panel_manager_;
    shell_layer_ = resources.shell_layer_;
}

CFDesktop::~CFDesktop() {
    log::traceftag("CFDesktop Self", "Destroying Self");
};

void CFDesktop::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (panel_manager_) {
        log::trace("CFDesktop Resized, Panel Manager handle the layouts, calling relayout...");
        panel_manager_->relayout();
    }
}

void CFDesktop::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    // A pure position change (drag) does not alter the local panel layout, so
    // relayout/availableGeometryChanged will not fire — emit geometryChanged so
    // screen-coordinate dependents (window placement) re-evaluate.
    emit geometryChanged();
}

bool CFDesktop::component_available(const DesktopComponent d) const noexcept {
    switch (d) {
        case DesktopComponent::Common:
            return panel_manager_ != nullptr && shell_layer_ != nullptr;
        case DesktopComponent::PanelManager:
            return panel_manager_ != nullptr;
        case DesktopComponent::ShellLayer:
            return shell_layer_ != nullptr;
    }
    return false;
}

} // namespace cf::desktop
