/**
 * @file    WindowDefine.h
 * @brief   Common type aliases for window identifiers.
 *
 * Defines the window identifier type used across the display server
 * and window management components.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup components
 */
#pragma once
#include <QString>

namespace cf::desktop {
/// Window identifier type — unique per backend implementation.
using win_id_t = QString;
} // namespace cf::desktop