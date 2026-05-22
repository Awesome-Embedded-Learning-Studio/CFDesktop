/**
 * @file desktop/main/init/desktop_config_init/desktop_config_init.h
 * @brief Desktop component configuration initialization stage.
 *
 * Initializes ConfigStore with the desktop config folder path read from
 * early settings, and creates default component config files if needed.
 *
 * @ingroup init_session
 */
#pragma once

#include "init_stage.h"
#include "init_stage_name.h"

namespace cf::desktop::init_session {

/**
 * @brief  Desktop component configuration initialization stage.
 *
 * Initializes the ConfigStore with desktop-specific paths and creates
 * default component configuration files when needed.
 *
 * @note   None
 * @since  N/A
 * @ingroup init_session
 */
class DesktopConfigInitStage : public IInitStage {
  public:
    std::string_view name() const noexcept override { return DESKTOP_CONFIG_INIT; }

    /**
     * @brief  Executes the configuration initialization.
     *
     * Reads the desktop config folder path from early settings, then
     * initializes the ConfigStore and creates default config files.
     *
     * @return     StageResult indicating success or failure.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup init_session
     */
    StageResult run_session() override;
};

} // namespace cf::desktop::init_session
