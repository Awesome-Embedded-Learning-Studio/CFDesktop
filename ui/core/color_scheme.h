/**
 * @file color_scheme.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Interface for color scheme queries.
 * @version 0.1
 * @date 2026-02-25
 * @since 0.1
 * @ingroup ui_core
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "export.h"
#include <QColor>

namespace cf::ui::core {

/**
 * @brief  Interface for color scheme queries.
 *
 * Defines the contract for color scheme implementations that provide
 * color values by name. Used by Material Design 3 color system
 * implementations.
 *
 * @note       Implementations are expected to be thread-safe if used
 *             across multiple threads.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    ui_core
 */
struct CF_UI_EXPORT ICFColorScheme {
    /**
     * @brief  Virtual destructor.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui_core
     */
    virtual ~ICFColorScheme() = default;

    /**
     * @brief  Queries a color by name.
     *
     * @param[in] name Color token name (e.g., "md.primary").
     *
     * @return     Reference to the QColor associated with the name.
     *
     * @throws     None.
     *
     * @note       The returned reference is valid for the lifetime of
     *             the color scheme object.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui_core
     */
    virtual QColor& queryExpectedColor(const char* name) = 0;

    /**
     * @brief  Convenience wrapper for queryExpectedColor.
     *
     * @param[in] name Color token name (e.g., "md.primary").
     *
     * @return     Copy of the QColor associated with the name.
     *
     * @throws     None.
     *
     * @note       Returns a copy rather than a reference for convenience.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui_core
     */
    QColor queryColor(const char* name) const {
        return const_cast<ICFColorScheme*>(this)->queryExpectedColor(name);
    }
};

} // namespace cf::ui::core
