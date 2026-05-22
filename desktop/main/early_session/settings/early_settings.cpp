#include "settings/early_settings.h"
#include "cffundamental/log_helper.h"
#include "file_op.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace cf::desktop::early_settings {

EarlySettings::~EarlySettings() {}

EarlySettings::EarlySettings(const QString& early_config_path) : filepath_(early_config_path) {
    QFile file(early_config_path);
    if (!file.open(QIODevice::ReadOnly)) {
        valid_ = false;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        valid_ = false;
        return;
    }

    early_settings_obj_ = std::make_unique<QJsonObject>(doc.object());
    valid_ = true;
}

bool EarlySettings::valid() const {
    return valid_;
}

QString EarlySettings::loadFrom() const {
    return filepath_;
}

QString EarlySettings::get_boot_logger_path() const {
    if (!valid() || !early_settings_obj_) {
        return {};
    }

    if (!this_session_logfile_path.isEmpty()) {
        return this_session_logfile_path;
    }

    const auto log_file_rev = base::filesystem::concat_filepath(
        (*early_settings_obj_)["logger"].toObject()["dirent"].toString(),
        logger_helper::log_filename());
    const auto log_file =
        base::filesystem::concat_filepath(base::filesystem::app_runtime_dir(), log_file_rev);

    this_session_logfile_path = log_file;
    return log_file;
}

QString EarlySettings::get_desktop_root() const {
    if (!valid() || !early_settings_obj_) {
        return {};
    }

    if (!desktop_root.isEmpty()) {
        return desktop_root;
    }

    desktop_root = (*early_settings_obj_)["desktop"].toObject()["root"].toString();
    return desktop_root;
}

QString EarlySettings::get_config_folder() const {
    if (!valid() || !early_settings_obj_) {
        return {};
    }

    if (!config_folder_cache.isEmpty()) {
        return config_folder_cache;
    }

    config_folder_cache =
        (*early_settings_obj_)["desktop"].toObject()["config_folder"].toString("settings/desktop/");
    return config_folder_cache;
}

std::unique_ptr<QJsonObject> EarlySettings::unlock_early_settings() {
    return std::move(early_settings_obj_);
}

} // namespace cf::desktop::early_settings
