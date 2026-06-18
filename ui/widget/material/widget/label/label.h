/**
 * @file    ui/widget/material/widget/label/label.h
 * @brief   Material Design 3 Label widget.
 *
 * Implements Material Design 3 label with support for multiple typography
 * styles (Display, Headline, Title, Body, Label), color variants (OnSurface,
 * OnSurfaceVariant, Primary, Secondary, etc.), and theme integration.
 *
 * @author  CFDesktop Team
 * @date    2026-03-01
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "aex/weak_ptr/weak_ptr.h"
#include "base/color.h"
#include "export.h"

#include <QLabel>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Typography style for Material Design 3 labels.
 *
 * @details Maps to Material Design 3 Type Scale with 15 styles across
 *          5 categories: Display, Headline, Title, Body, Label.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class TypographyStyle {
    // Display styles (57sp, 45sp, 36sp) - for hero content
    DisplayLarge,
    DisplayMedium,
    DisplaySmall,

    // Headline styles (32sp, 28sp, 24sp) - for app bar text
    HeadlineLarge,
    HeadlineMedium,
    HeadlineSmall,

    // Title styles (22sp, 16sp, 14sp) - for section headings
    TitleLarge,
    TitleMedium,
    TitleSmall,

    // Body styles (16sp, 14sp, 12sp) - for main content
    BodyLarge,
    BodyMedium,
    BodySmall,

    // Label styles (14sp, 12sp, 11sp) - for secondary info
    LabelLarge,
    LabelMedium,
    LabelSmall
};

/**
 * @brief  Color variant for Material Design 3 labels.
 *
 * @details Controls which color token is used for text rendering.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class LabelColorVariant {
    OnSurface,        ///< Default on-surface color
    OnSurfaceVariant, ///< Variant on-surface color
    Primary,          ///< Primary brand color
    OnPrimary,        ///< Text on primary color
    Secondary,        ///< Secondary brand color
    OnSecondary,      ///< Text on secondary color
    Error,            ///< Error color
    OnError,          ///< Text on error color
    InverseSurface,   ///< Inverted surface color
    InverseOnSurface  ///< Text on inverted surface
};

/**
 * @brief  Material Design 3 Label widget.
 *
 * @details Implements Material Design 3 label with:
 *          - 15 typography styles from MD3 Type Scale
 *          - 9 color variants for semantic coloring
 *          - Theme-aware color and font resolution
 *          - Disabled state with opacity
 *          - Automatic theme change handling
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT Label : public QLabel {
    Q_OBJECT
    Q_PROPERTY(TypographyStyle typographyStyle READ typographyStyle WRITE setTypographyStyle)
    Q_PROPERTY(LabelColorVariant colorVariant READ colorVariant WRITE setColorVariant)
    Q_PROPERTY(bool autoHiding READ autoHiding WRITE setAutoHiding)

  public:
    /**
     * @brief  Constructor with text and style.
     *
     * @param[in]     text Label text content.
     * @param[in]     style Typography style.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Default style is BodyMedium.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit Label(const QString& text = QString(),
                   TypographyStyle style = TypographyStyle::BodyMedium, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~Label() override;

    /**
     * @brief  Gets the typography style.
     *
     * @return        Current typography style.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TypographyStyle typographyStyle() const;

    /**
     * @brief  Sets the typography style.
     *
     * @param[in]     style Typography style to apply.
     *
     * @throws        None
     * @note          Updates font and triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setTypographyStyle(TypographyStyle style);

    /**
     * @brief  Gets the color variant.
     *
     * @return        Current color variant.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    LabelColorVariant colorVariant() const;

    /**
     * @brief  Sets the color variant.
     *
     * @param[in]     variant Color variant to apply.
     *
     * @throws        None
     * @note          Updates text color and triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setColorVariant(LabelColorVariant variant);

    /**
     * @brief  Gets whether auto-hiding is enabled.
     *
     * @return        true if label hides when empty, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool autoHiding() const;

    /**
     * @brief  Sets whether auto-hiding is enabled.
     *
     * @param[in]     enabled true to hide when text is empty, false otherwise.
     *
     * @throws        None
     * @note          When enabled, label becomes hidden when text is empty.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setAutoHiding(bool enabled);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the label.
     *
     * @throws        None
     * @note          Based on text, font, and word wrap setting.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets the minimum recommended size.
     *
     * @return        Minimum recommended size.
     *
     * @throws        None
     * @note          Ensures minimum touch target size for interactive labels.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  protected:
    /**
     * @brief  Paints the label.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Renders text with theme-aware color and font.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles widget state change event.
     *
     * @param[in]     event Change event.
     *
     * @throws        None
     * @note          Updates appearance based on state changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void changeEvent(QEvent* event) override;

  private:
    /**
     * @brief  Updates the label's appearance.
     *
     * @throws        None
     * @note          Refreshes font, color, and auto-hiding state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void updateAppearance();

    /**
     * @brief  Gets the text color based on current variant.
     *
     * @return        Color for text rendering.
     *
     * @throws        None
     * @note          Respects enabled state and theme.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    CFColor textColor() const;

    /**
     * @brief  Gets the font for current typography style.
     *
     * @return        Font matching the current style.
     *
     * @throws        None
     * @note          Queries theme for typography token.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QFont typographyFont() const;

    /**
     * @brief  Gets the typography token name for a style.
     *
     * @param[in]     style Typography style.
     * @return        Token name string.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    static const char* typographyTokenName(TypographyStyle style);

    TypographyStyle typographyStyle_;
    LabelColorVariant colorVariant_;
    bool autoHiding_;
    mutable CFColor cachedColor_;  ///< Cached color to avoid repeated theme queries
    mutable bool colorCacheValid_; ///< Whether the cached color is valid
};

} // namespace cf::ui::widget::material
