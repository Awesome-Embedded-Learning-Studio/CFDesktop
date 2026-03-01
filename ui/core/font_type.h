/**
 * @file    ui/core/font_type.h
 * @brief   Defines the IFontType interface for querying font styles.
 *
 * Provides an abstract interface for querying QFont objects by name.
 *
 * @author  Charliechen114514
 * @date    2026-02-26
 * @version N/A
 * @since   N/A
 * @ingroup ui_core
 */

#pragma once
#include "export.h"
#include <QFont>

namespace cf::ui::core {

/**
 * @brief  Abstract interface for querying font styles by name.
 *
 * Defines the contract for font type providers that return QFont
 * objects based on style names.
 *
 * @ingroup ui_core
 *
 * @code
 * IFontType* fontType = getFontType();
 * QFont font = fontType->queryTargetFont("bodyLarge");
 * @endcode
 */
struct CF_UI_EXPORT IFontType {
    /// @brief Virtual destructor.
    virtual ~IFontType() = default;

    /**
     * @brief  Query a font style by name.
     *
     * @param[in] name Font style name (e.g., "bodyLarge", "headlineMedium").
     * @return         QFont object corresponding to the requested style.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          N/A
     * @ingroup        ui_core
     */
    virtual QFont queryTargetFont(const char* name) = 0;
};

} // namespace cf::ui::core
