#include "init_chain.h"
#include "cflog.h"
#include "init_stage.h"
#include <QApplication>
#include <QDebug>
#include <qlogging.h>

namespace cf::desktop::early_stage {

EarlyInitRunner::~EarlyInitRunner() {
    if (!boot_finished_) {
        execute();
    }
}

void EarlyInitRunner::execute_with_no_log_enabled() {
    /* force sync */
    qDebug() << "Runs With no log... boot as fast as possible!";
    for (auto& first_one : first_stages_) {
        auto stage = first_one();
        if (auto staging_request = stage->atExpectedStageBootup()) {
            if (staging_request != stage_at_current) {
                qWarning() << "Stage: " << stage->name() << " expected bootup stage mismatch!"
                           << "Expected:" << staging_request << "Current:" << stage_at_current;
            }
        }

        auto result = stage->run_session();
        switch (result) {
            case IEarlyStage::BootResult::CRITICAL_FAILED: {
                qCritical() << "Fatal Error: When booting stage" << stage->name()
                            << ", something sucks!";
                direct_quit_app();
            } break;
            case IEarlyStage::BootResult::FAILED: {
                qWarning()
                    << "Warning: " << stage->name()
                    << " failed but not critical... Booting of this session might be unreliable...";
                stage_at_current++;
            } break;
            case IEarlyStage::BootResult::OK: {
                qDebug() << "OK Boot " << stage->name() << "Finished!";
                stage_at_current++;
            } break;
        }
    }

    first_stages_.clear(); // clear the stage
    qDebug() << "First stage boot finished! Logger Can be avaible!";
}

void EarlyInitRunner::execute_with_log() {
    using namespace cf::log;

    for (auto& first_one : early_stages_) {
        auto stage = first_one();
        if (auto staging_request = stage->atExpectedStageBootup()) {
            if (staging_request != static_cast<unsigned int>(stage_at_current)) {
                auto expected = *staging_request;
                auto current = static_cast<unsigned int>(stage_at_current);
                warningftag("EarlyBoot",
                            "Stage: {} expected bootup stage mismatch! Expected: {} Current: {}",
                            stage->name(), expected, current);
            }
        }

        auto result = stage->run_session();
        switch (result) {
            case IEarlyStage::BootResult::CRITICAL_FAILED: {
                errorftag("EarlyBoot", "Fatal Error: When booting stage {}, something sucks!",
                          stage->name());
                direct_quit_app();
            } break;
            case IEarlyStage::BootResult::FAILED: {
                warningftag("EarlyBoot",
                            "Warning: {} failed but not critical... Booting of this session might "
                            "be unreliable...",
                            stage->name());
            } break;
            case IEarlyStage::BootResult::OK: {
                infoftag("EarlyBoot", "OK Boot {} Finished!", stage->name());
            } break;
        }
    }

    early_stages_.clear(); // clear the stage
    infoftag("EarlyBoot", "First stage boot finished! Logger Can be available!");
}

void EarlyInitRunner::direct_quit_app() {
    qApp->exit(-1);
}

void EarlyInitRunner::execute() {
    execute_with_no_log_enabled();
    execute_with_log();
    boot_finished_ = true;
}

} // namespace cf::desktop::early_stage