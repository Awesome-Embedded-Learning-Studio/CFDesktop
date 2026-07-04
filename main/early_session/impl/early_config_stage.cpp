#include "early_config_stage.h"
#include "desktop_settings.h"
#include "early_handle/early_handle.h"
#include "file_op.h"
#include "init_chain/init_stage.h"
#include "settings/early_settings.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <memory>
#include <qlogging.h>

namespace cf::desktop::early_stage {
EarlyConfigStage::EarlyConfigStage() : IEarlyStage(true) {
    policy = UnexpectedStageBootSolution::SHUTDOWN;
}

EarlyConfigStage::BootResult EarlyConfigStage::run_session() {
    QString early_config_full_path = base::filesystem::concat_filepath(
        base::filesystem::app_runtime_dir(), early_stage::EARLY_SETTINGS);
    qDebug() << "About to Check config in: " << early_config_full_path;
    // Assume We write here
    if (!base::filesystem::exsited(early_config_full_path)) {
        base::filesystem::create_anyway(early_config_full_path);
        QFile file(early_config_full_path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(early_stage::EARLY_SETTINGS_TEMPLATE);
            file.close();
        } else {
            qCritical() << "Can not perform Early writtings in " << early_config_full_path;
            return BootResult::CRITICAL_FAILED;
        }
    }

    // OK, Register to Early Handles
    EarlyHandle::instance().setEarlySettingsHandle(
        std::make_unique<early_settings::EarlySettings>(early_config_full_path));

    qDebug() << "OK, Finished settings up, early config file path at: " << early_config_full_path;
    return BootResult::OK;
}

} // namespace cf::desktop::early_stage