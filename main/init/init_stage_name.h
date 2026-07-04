/**
 * @file desktop/main/init/init_stage_name.h
 * @brief Defines compile-time constant names for initialization stages.
 *
 * @author CFDesktop Team
 * @ingroup init_session
 */

#pragma once

namespace cf::desktop::init_session {
static constexpr const char* CONFIG_PASS_STAGE = "Early Transform";
static constexpr const char* DESKTOP_BACKBONE_SETUP = "Desktop Backbone setup";
static constexpr const char* DESKTOP_CONFIG_INIT = "Desktop Config Init";
} // namespace cf::desktop::init_session
