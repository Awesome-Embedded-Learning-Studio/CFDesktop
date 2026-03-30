/**
 * @file    WindowDefine.h
 * @brief   Common type aliases for window identifiers.
 *
 * Defines the window identifier type used across the display server
 * and window management components.
 *
 * @author  CFDesktop Team
 * @date    2026-03-27
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup components
 */
#pragma once
#include <QString>

namespace cf::desktop {
/// Window identifier type — unique per backend implementation.
using win_id_t = QString;
} // namespace cf::desktop