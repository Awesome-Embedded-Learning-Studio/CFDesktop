/**
 * @file early_welcome_impl.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Default EarlyWelcome Sessions
 * @version 0.1
 * @date 2026-03-19
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "../init_chain/init_stage.h"

namespace cf::desktop::early_stage {

/**
 * @brief  Default early welcome stage implementation.
 *
 * Provides a welcome message during the early boot sequence.
 *
 * @note           None
 * @warning        None
 * @since          0.13.0
 * @ingroup        early_session
 */
class EarlyWelcomeImpl : public IEarlyStage {
  public:
    EarlyWelcomeImpl() = default;
    ~EarlyWelcomeImpl() = default;

    /**
     * @brief  Returns the name of this stage.
     *
     * @return     Stage name as a string view.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    std::string_view name() const override { return "Desktop Early Boots"; }

    /**
     * @brief  Returns the expected position in the boot sequence.
     *
     * @return     Empty optional (no fixed position).
     * @throws     None
     * @note       This stage has no fixed boot order.
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return {}; }

    /**
     * @brief  Runs the welcome session.
     *
     * @return     Result of the boot operation.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    BootResult run_session() override;

  private:
    /**
     * @brief  Generates the welcome message string.
     *
     * @return     The welcome message as a string.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    std::string factorize_bootWelcomeWithString() const;
};
} // namespace cf::desktop::early_stage
