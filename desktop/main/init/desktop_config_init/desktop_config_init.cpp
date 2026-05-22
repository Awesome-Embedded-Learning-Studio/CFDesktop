#include "desktop_config_init.h"

#include "cfconfig.hpp"
#include "cfconfig/cfconfig_path_provider.h"
#include "cflog.h"
#include "desktop_settings.h"
#include "file_op.h"
#include "init_settings.h"

#include <QFile>

namespace cf::desktop::init_session {

DesktopConfigInitStage::StageResult DesktopConfigInitStage::run_session() {
    // 1. Read config_folder from early settings stored in InitInfoHandle
    const auto& root_pos = InitInfoHandle::instance().root_position();
    if (root_pos.isEmpty()) {
        return StageResult::critical_failed("DesktopConfigInit", "No root position available");
    }

    // Compute absolute config folder path: app_runtime_dir() + config_folder
    const auto runtime = base::filesystem::app_runtime_dir();

    // Read config_folder from early settings via InitInfoHandle
    QString config_folder_rel = InitInfoHandle::instance().config_folder();
    if (config_folder_rel.isEmpty()) {
        // When Empty, Desktop should be sucked
        return StageResult::critical_failed("DesktopConfigInit",
                                            "Config Folder is empty, this is not expected!");
    }

    QString config_folder_abs = base::filesystem::concat_filepath(runtime, config_folder_rel);

    // 2. Ensure directory exists
    if (!base::filesystem::exsited(config_folder_abs)) {
        if (!base::filesystem::create_anyway(config_folder_abs)) {
            return StageResult::critical_failed(
                "DesktopConfigInit", "Failed to create config directory: " + config_folder_abs);
        }
    }

    log::infoftag("DesktopConfigInit", "Config folder: {}", config_folder_abs.toStdString());

    // 3. Create default wallpaper.json if not exists
    QString wallpaper_path = config_folder_abs + "/wallpaper.json";
    if (!QFile::exists(wallpaper_path)) {
        QFile file(wallpaper_path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(early_stage::WALLPAPER_CONFIG_TEMPLATE);
            file.close();
            log::infoftag("DesktopConfigInit", "Created default wallpaper.json");
        }
    }

    // 4. Initialize ConfigStore with path provider pointing to config folder
    auto provider = std::make_shared<cf::config::DesktopConfigStorePathProvider>(config_folder_abs);
    cf::config::ConfigStore::instance().initialize(provider);

    log::infoftag("DesktopConfigInit", "ConfigStore initialized with app_dir: {}",
                  config_folder_abs.toStdString());

    return StageResult::ok("Desktop config initialized");
}

} // namespace cf::desktop::init_session
