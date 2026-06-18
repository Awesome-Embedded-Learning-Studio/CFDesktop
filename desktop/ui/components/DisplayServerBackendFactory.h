/**
 * @file    DisplayServerBackendFactory.h
 * @brief   Factory for creating platform-specific display server backends.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#pragma once
#include "IDisplayServerBackend.h"
#include "aex/factory/registered_factory.hpp"

namespace cf::desktop {

/**
 * @brief Factory for creating platform-specific display server backends.
 *
 * Inherits from aex::StaticRegisteredFactory<IDisplayServerBackend>, which
 * delegates creation to a std::function registered at startup by the
 * platform-specific module.
 *
 * Usage:
 *   auto backend = DisplayServerBackendFactory::instance().make_unique();
 */
class DisplayServerBackendFactory : public aex::StaticRegisteredFactory<IDisplayServerBackend> {};

} // namespace cf::desktop
