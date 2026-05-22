#include "init_settings.h"
#include "early_handle/early_handle.h"
#include <QJsonObject>

namespace cf::desktop::init_session {

InitInfoHandle::~InitInfoHandle() {
    delete early_settings_;
    early_settings_ = nullptr;
}

void InitInfoHandle::passedEarlyBootInfo(early_stage::EarlyHandle& early_handle) {
    auto earlies = early_handle.unlockEarlySettings();
    early_settings_ = earlies->unlock_early_settings().release();
    /* Releases the Early settings... */
}

void InitInfoHandle::setBootWidget(QWidget* widget) {
    boot_widget_ = widget;
}

QWidget* InitInfoHandle::unlockBootWidget() {
    QWidget* p = boot_widget_;
    boot_widget_ = nullptr;
    return p;
}

QString InitInfoHandle::root_position() const {
    if (!early_settings_) {
        return {};
    }
    return (*early_settings_)["desktop"].toObject()["root"].toString();
}

QString InitInfoHandle::config_folder() const {
    if (!early_settings_) {
        return {};
    }
    return (*early_settings_)["desktop"].toObject()["config_folder"].toString("settings/desktop/");
}

} // namespace cf::desktop::init_session
