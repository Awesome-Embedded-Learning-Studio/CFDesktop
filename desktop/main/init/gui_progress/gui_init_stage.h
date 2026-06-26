/**
 * @file    desktop/main/init/gui_progress/gui_init_stage.h
 * @brief   GUI boot stage that displays a logo and progress widget during initialization.
 *
 * Provides the visual boot progress interface using BootProgressWidget, connecting
 * to the initialization session chain for real-time stage updates.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup desktop_init
 */

#pragma once
#include "init_stage.h"

namespace cf::desktop::init_session {

/**
 * @brief  GUI initialization stage that displays the boot progress widget.
 *
 * Shows a visual progress interface during system initialization, connecting
 * to the InitSessionChain to display real-time updates as stages execute.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @ingroup desktop_init
 *
 * @code
 * GuiLogoBootStage stage;
 * auto result = stage.run_session();
 * @endcode
 */
class GuiLogoBootStage : public IInitStage {
  public:
    ~GuiLogoBootStage() override;

    /**
     * @brief  Returns the name identifier of this stage.
     *
     * Provides a human-readable name for logging and debugging purposes.
     *
     * @return     Name string view "Gui Logo Progress Boot".
     * @throws     None
     * @note       This function is noexcept and may be evaluated at compile time.
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_init
     */
    std::string_view name() const noexcept override { return "Gui Logo Progress Boot"; }

    /**
     * @brief  Returns empty list of stage dependencies.
     *
     * This stage has no prerequisites and does not request any stages to run
     * before it.
     *
     * @return     Empty vector of weak pointers to IInitStage.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_init
     */
    std::vector<aex::WeakPtr<IInitStage>> request_before_actions_init() const override {
        return {};
    }

    /**
     * @brief  Executes the GUI boot progress initialization.
     *
     * Creates and configures the BootProgressWidget, connects it to the
     * InitSessionChain for stage progress updates, and displays the widget.
     *
     * @return     StageResult with status code and message (OK on success,
     *             CriticalFailed if chain reference is invalid).
     * @throws     None
     * @note       Currently uses BootstrapStyle::Simple for the widget.
     * @warning    Returns CriticalFailed if InitSessionChain reference is null.
     * @since      N/A
     * @ingroup    desktop_init
     */
    StageResult run_session() override;
};

} // namespace cf::desktop::init_session
