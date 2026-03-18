/**
 * @file    ui/base/color.h
 * @brief   Enhanced color representation with HCT color space support.
 *
 * Provides the CFColor class which extends QColor with HCT (Hue-Chroma-Tone)
 * color space for Material Design 3 color system compatibility.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-02-23
 * @version 0.1
 * @since   0.1
 * @ingroup ui
 */

#pragma once
#include <QColor>
#include <QString>

namespace cf::ui::base {

/**
 * @brief  Enhanced color class with HCT color space support.
 *
 * Extends QColor with HCT (Hue-Chroma-Tone) color space, enabling
 * Material Design 3 color theming and dynamic color schemes.
 * Maintains both the native QColor representation and cached HCT values.
 *
 * @note    Not thread-safe unless externally synchronized.
 * @ingroup ui
 *
 * @code
 * // Create from hex string
 * CFColor color("#FF0000");
 *
 * // Create from HCT values
 * CFColor blue(240.0f, 80.0f, 50.0f);
 *
 * // Access HCT components
 * float h = color.hue();
 * float c = color.chroma();
 * float t = color.tone();
 * @endcode
 */
class CFColor {
  public:
    /**
     * @brief  Default constructor.
     *
     * Creates a black color with HCT values (0, 0, 0).
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CFColor() : internal_color(Qt::black) {}

    /**
     * @brief  Constructs from RGB values.
     *
     * Creates a CFColor from RGB integer values (0-255 range),
     * automatically computing and caching the HCT values.
     *
     * @param[in] r  Red component (0-255).
     * @param[in] g  Green component (0-255).
     * @param[in] b  Blue component (0-255).
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CFColor(int r, int g, int b);

    /**
     * @brief  Constructs from a QColor.
     *
     * Creates a CFColor from a native QColor, automatically computing
     * and caching the HCT values.
     *
     * @param[in] native  Source QColor.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CFColor(const QColor& native);

    /**
     * @brief  Constructs from a hexadecimal color string.
     *
     * Parses a hex string in "#RRGGBB" or "#AARRGGBB" format and creates
     * a CFColor. Invalid formats default to black.
     *
     * @param[in] hex  Hex color string. Valid formats: "#RRGGBB", "#AARRGGBB".
     *
     * @throws     None.
     *
     * @note       Explicit to avoid ambiguity with QColor(const char*).
     *
     * @warning    Invalid hex strings produce a black color.
     *
     * @since      0.1
     * @ingroup    ui
     */
    explicit CFColor(const QString& hex);

    /**
     * @brief  Constructs from a C-string hexadecimal color.
     *
     * Parses a hex string in "#RRGGBB" or "#AARRGGBB" format.
     *
     * @param[in] hex  Hex color string. Valid formats: "#RRGGBB", "#AARRGGBB".
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Invalid hex strings produce a black color.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CFColor(const char* hex);

    /**
     * @brief  Constructs from HCT color space values.
     *
     * Creates a color from Hue-Chroma-Tone values, automatically converting
     * to RGB and caching the result. Values are clamped to valid ranges.
     *
     * @param[in] hue    Hue component in degrees. Valid range: [0.0, 360.0].
     * @param[in] chroma Chroma component (color intensity). Valid range: [0.0, 150.0].
     * @param[in] tone   Tone component (lightness). Valid range: [0.0, 100.0].
     *
     * @throws     None.
     *
     * @note       Values outside valid ranges are clamped.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CFColor(float hue, float chroma, float tone);

    /**
     * @brief  Computes the WCAG relative luminance.
     *
     * Returns the relative luminance according to WCAG 2.1 specification,
     * used for contrast ratio calculations. Range: [0.0, 1.0].
     *
     * @return        Relative luminance in range [0.0, 1.0].
     * @throws       None
     * @note         Uses linear RGB conversion with gamma correction.
     * @warning      None
     * @since        0.1
     */
    float relativeLuminance() const;

    /**
     * @brief  Returns the hue component.
     *
     * @return        Hue in degrees, range [0.0, 360.0].
     * @throws       None
     * @note         None
     * @warning      None
     * @since        0.1
     */
    float hue() const;

    /**
     * @brief  Returns the chroma component.
     *
     * @return        Chroma (color intensity), range [0.0, 150.0].
     * @throws       None
     * @note         None
     * @warning      None
     * @since        0.1
     */
    float chroma() const;

    /**
     * @brief  Returns the tone component.
     *
     * @return        Tone (lightness), range [0.0, 100.0].
     * @throws       None
     * @note         None
     * @warning      None
     * @since        0.1
     */
    float tone() const;

    /**
     * @brief  Returns the native QColor representation.
     *
     * @return        Native QColor. Ownership: observer.
     * @throws       None
     * @note         The returned QColor is a copy, not a reference.
     * @warning      None
     * @since        0.1
     */
    QColor native_color() const { return internal_color; }

    /**
     * @brief  Copy constructor.
     *
     * @param[in] other Source color.
     * @throws       None
     * @note         None
     * @warning      None
     * @since        0.1
     */
    CFColor(const CFColor& other) = default;

    /**
     * @brief  Copy assignment operator.
     *
     * Copies both the native QColor and the cached HCT values to ensure
     * consistency after assignment.
     *
     * @param[in] other Source color.
     * @return        Reference to this color.
     * @throws       None
     * @note         Self-assignment is safely handled.
     * @warning      None
     * @since        0.1
     */
    CFColor& operator=(const CFColor& other) {
        if (this != &other) {
            internal_color = other.internal_color;
            m_hue = other.m_hue;
            m_chroma = other.m_chroma;
            m_tone = other.m_tone;
        }
        return *this;
    }

  private:
    QColor internal_color; ///< Native QColor representation.

    // Cached HCT values (computed from RGB in constructors)
    float m_hue = 0.0f;    ///< Cached hue component. Range: [0.0, 360.0].
    float m_chroma = 0.0f; ///< Cached chroma component. Range: [0.0, 150.0].
    float m_tone = 0.0f;   ///< Cached tone component. Range: [0.0, 100.0].
};

} // namespace cf::ui::base
