/**
 * @file material_factory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Scheme Factory Implementation
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "material_factory.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "base/color.h"
#include "base/color_helper.h"
#include "token/material_scheme/cfmaterial_token_literals.h"

// Type aliases to avoid namespace lookup issues
using CFColor = ::cf::ui::base::CFColor;

namespace cf::ui::core::material {

namespace detail {

// =============================================================================
// Material Design 3 Default Color Values
// =============================================================================

/**
 * @brief Material Design 3 Light Theme Colors (Baseline Purple)
 *
 * Official MD3 baseline colors for light theme.
 * Source: https://m3.material.io/styles/color/the-color-system/tokens
 */
struct LightColors {
    static constexpr const char* primary = "#6750A4";
    static constexpr const char* onPrimary = "#FFFFFF";
    static constexpr const char* primaryContainer = "#EADDFF";
    static constexpr const char* onPrimaryContainer = "#21005D";

    static constexpr const char* secondary = "#625B71";
    static constexpr const char* onSecondary = "#FFFFFF";
    static constexpr const char* secondaryContainer = "#E8DEF8";
    static constexpr const char* onSecondaryContainer = "#1D192B";

    static constexpr const char* tertiary = "#7D5260";
    static constexpr const char* onTertiary = "#FFFFFF";
    static constexpr const char* tertiaryContainer = "#FFD8E4";
    static constexpr const char* onTertiaryContainer = "#31111D";

    static constexpr const char* error = "#B3261E";
    static constexpr const char* onError = "#FFFFFF";
    static constexpr const char* errorContainer = "#F9DEDC";
    static constexpr const char* onErrorContainer = "#410E0B";

    static constexpr const char* background = "#FFFBFE";
    static constexpr const char* onBackground = "#1C1B1F";
    static constexpr const char* surface = "#FFFBFE";
    static constexpr const char* onSurface = "#1C1B1F";
    static constexpr const char* surfaceVariant = "#E7E0EC";
    static constexpr const char* onSurfaceVariant = "#49454F";
    static constexpr const char* outline = "#79747E";
    static constexpr const char* outlineVariant = "#CAC4D0";

    static constexpr const char* shadow = "#000000";
    static constexpr const char* scrim = "#000000";
    static constexpr const char* inverseSurface = "#313033";
    static constexpr const char* inverseOnSurface = "#F4EFF4";
    static constexpr const char* inversePrimary = "#D0BCFF";
};

/**
 * @brief Material Design 3 Dark Theme Colors (Baseline Purple)
 *
 * Official MD3 baseline colors for dark theme.
 */
struct DarkColors {
    static constexpr const char* primary = "#D0BCFF";
    static constexpr const char* onPrimary = "#381E72";
    static constexpr const char* primaryContainer = "#4F378B";
    static constexpr const char* onPrimaryContainer = "#EADDFF";

    static constexpr const char* secondary = "#CCC2DC";
    static constexpr const char* onSecondary = "#332D41";
    static constexpr const char* secondaryContainer = "#4A4458";
    static constexpr const char* onSecondaryContainer = "#E8DEF8";

    static constexpr const char* tertiary = "#EFB8C8";
    static constexpr const char* onTertiary = "#492532";
    static constexpr const char* tertiaryContainer = "#633B48";
    static constexpr const char* onTertiaryContainer = "#FFD8E4";

    static constexpr const char* error = "#F2B8B5";
    static constexpr const char* onError = "#601410";
    static constexpr const char* errorContainer = "#8C1D18";
    static constexpr const char* onErrorContainer = "#F9DEDC";

    static constexpr const char* background = "#1C1B1F";
    static constexpr const char* onBackground = "#E6E1E5";
    static constexpr const char* surface = "#1C1B1F";
    static constexpr const char* onSurface = "#E6E1E5";
    static constexpr const char* surfaceVariant = "#49454F";
    static constexpr const char* onSurfaceVariant = "#CAC4D0";
    static constexpr const char* outline = "#938F99";
    static constexpr const char* outlineVariant = "#49454F";

    static constexpr const char* shadow = "#000000";
    static constexpr const char* scrim = "#000000";
    static constexpr const char* inverseSurface = "#E6E1E5";
    static constexpr const char* inverseOnSurface = "#313033";
    static constexpr const char* inversePrimary = "#6750A4";
};

// =============================================================================
// Color Registration Helpers
// =============================================================================

template <typename ColorDefs> inline void registerAllColors(MaterialColorScheme& scheme) {
    auto& r = scheme.registry();
    namespace literals = ::cf::ui::core::token::literals;
    using CFColor = ::cf::ui::base::CFColor;

    // Primary colors
    r.register_dynamic<CFColor>(literals::PRIMARY, CFColor(ColorDefs::primary));
    r.register_dynamic<CFColor>(literals::ON_PRIMARY, CFColor(ColorDefs::onPrimary));
    r.register_dynamic<CFColor>(literals::PRIMARY_CONTAINER, CFColor(ColorDefs::primaryContainer));
    r.register_dynamic<CFColor>(literals::ON_PRIMARY_CONTAINER,
                                CFColor(ColorDefs::onPrimaryContainer));

    // Secondary colors
    r.register_dynamic<CFColor>(literals::SECONDARY, CFColor(ColorDefs::secondary));
    r.register_dynamic<CFColor>(literals::ON_SECONDARY, CFColor(ColorDefs::onSecondary));
    r.register_dynamic<CFColor>(literals::SECONDARY_CONTAINER,
                                CFColor(ColorDefs::secondaryContainer));
    r.register_dynamic<CFColor>(literals::ON_SECONDARY_CONTAINER,
                                CFColor(ColorDefs::onSecondaryContainer));

    // Tertiary colors
    r.register_dynamic<CFColor>(literals::TERTIARY, CFColor(ColorDefs::tertiary));
    r.register_dynamic<CFColor>(literals::ON_TERTIARY, CFColor(ColorDefs::onTertiary));
    r.register_dynamic<CFColor>(literals::TERTIARY_CONTAINER,
                                CFColor(ColorDefs::tertiaryContainer));
    r.register_dynamic<CFColor>(literals::ON_TERTIARY_CONTAINER,
                                CFColor(ColorDefs::onTertiaryContainer));

    // Error colors
    r.register_dynamic<CFColor>(literals::ERROR, CFColor(ColorDefs::error));
    r.register_dynamic<CFColor>(literals::ON_ERROR, CFColor(ColorDefs::onError));
    r.register_dynamic<CFColor>(literals::ERROR_CONTAINER, CFColor(ColorDefs::errorContainer));
    r.register_dynamic<CFColor>(literals::ON_ERROR_CONTAINER, CFColor(ColorDefs::onErrorContainer));

    // Surface colors
    r.register_dynamic<CFColor>(literals::BACKGROUND, CFColor(ColorDefs::background));
    r.register_dynamic<CFColor>(literals::ON_BACKGROUND, CFColor(ColorDefs::onBackground));
    r.register_dynamic<CFColor>(literals::SURFACE, CFColor(ColorDefs::surface));
    r.register_dynamic<CFColor>(literals::ON_SURFACE, CFColor(ColorDefs::onSurface));
    r.register_dynamic<CFColor>(literals::SURFACE_VARIANT, CFColor(ColorDefs::surfaceVariant));
    r.register_dynamic<CFColor>(literals::ON_SURFACE_VARIANT, CFColor(ColorDefs::onSurfaceVariant));
    r.register_dynamic<CFColor>(literals::OUTLINE, CFColor(ColorDefs::outline));
    r.register_dynamic<CFColor>(literals::OUTLINE_VARIANT, CFColor(ColorDefs::outlineVariant));

    // Utility colors
    r.register_dynamic<CFColor>(literals::SHADOW, CFColor(ColorDefs::shadow));
    r.register_dynamic<CFColor>(literals::SCRIM, CFColor(ColorDefs::scrim));
    r.register_dynamic<CFColor>(literals::INVERSE_SURFACE, CFColor(ColorDefs::inverseSurface));
    r.register_dynamic<CFColor>(literals::INVERSE_ON_SURFACE, CFColor(ColorDefs::inverseOnSurface));
    r.register_dynamic<CFColor>(literals::INVERSE_PRIMARY, CFColor(ColorDefs::inversePrimary));
}

} // namespace detail

// =============================================================================
// Factory Function Implementations
// =============================================================================

MaterialColorScheme light() {
    MaterialColorScheme scheme;
    detail::registerAllColors<detail::LightColors>(scheme);
    return scheme;
}

MaterialColorScheme dark() {
    MaterialColorScheme scheme;
    detail::registerAllColors<detail::DarkColors>(scheme);
    return scheme;
}

Result fromJson(const QByteArray& json, bool isDark) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        return ::cf::unexpected(
            MaterialSchemeError{MaterialSchemeError::Kind::InvalidJson,
                                "Failed to parse JSON: " + parseError.errorString().toStdString()});
    }

    if (!doc.isObject()) {
        return ::cf::unexpected(MaterialSchemeError{MaterialSchemeError::Kind::InvalidJson,
                                                    "Root element must be an object"});
    }

    QJsonObject root = doc.object();

    // Support both direct color values and nested "schemes" structure
    QJsonObject colors;
    if (root.contains("schemes")) {
        QJsonObject schemes = root["schemes"].toObject();
        QString schemeKey = isDark ? "dark" : "light";
        colors = schemes[schemeKey].toObject();
    } else {
        colors = root;
    }

    MaterialColorScheme scheme;
    auto& r = scheme.registry();

    // Map of MD3 color names to registry keys
    const std::unordered_map<std::string, std::string> colorMapping = {
        {"primary", "md.primary"},
        {"onPrimary", "md.onPrimary"},
        {"primaryContainer", "md.primaryContainer"},
        {"onPrimaryContainer", "md.onPrimaryContainer"},
        {"secondary", "md.secondary"},
        {"onSecondary", "md.onSecondary"},
        {"secondaryContainer", "md.secondaryContainer"},
        {"onSecondaryContainer", "md.onSecondaryContainer"},
        {"tertiary", "md.tertiary"},
        {"onTertiary", "md.onTertiary"},
        {"tertiaryContainer", "md.tertiaryContainer"},
        {"onTertiaryContainer", "md.onTertiaryContainer"},
        {"error", "md.error"},
        {"onError", "md.onError"},
        {"errorContainer", "md.errorContainer"},
        {"onErrorContainer", "md.onErrorContainer"},
        {"background", "md.background"},
        {"onBackground", "md.onBackground"},
        {"surface", "md.surface"},
        {"onSurface", "md.onSurface"},
        {"surfaceVariant", "md.surfaceVariant"},
        {"onSurfaceVariant", "md.onSurfaceVariant"},
        {"outline", "md.outline"},
        {"outlineVariant", "md.outlineVariant"},
        {"shadow", "md.shadow"},
        {"scrim", "md.scrim"},
        {"inverseSurface", "md.inverseSurface"},
        {"inverseOnSurface", "md.inverseOnSurface"},
        {"inversePrimary", "md.inversePrimary"}};

    // Parse and register colors
    for (const auto& [key, registryKey] : colorMapping) {
        QString qKey = QString::fromStdString(key);
        if (colors.contains(qKey)) {
            QString colorStr = colors[qKey].toString();
            if (!colorStr.startsWith('#')) {
                return ::cf::unexpected(MaterialSchemeError{
                    MaterialSchemeError::Kind::InvalidColorFormat,
                    "Invalid color format for " + key + ": " + colorStr.toStdString()});
            }
            r.register_dynamic<CFColor>(registryKey, CFColor(colorStr));
        }
    }

    return scheme;
}

MaterialColorScheme fromKeyColor(CFColor keyColor, bool isDark) {
    using ::cf::ui::base::tonalPalette;

    MaterialColorScheme scheme;
    auto& r = scheme.registry();

    // Generate tonal palette from key color
    QList<CFColor> tonal = tonalPalette(keyColor);

    // Helper to get color from tonal palette by index
    auto getTone = [&tonal](int index) -> CFColor {
        // tonalPalette returns 13 colors: 0, 10, 20, ..., 90, 95, 99, 100
        // Map index 0-12 to these tones
        if (index >= 0 && index < tonal.size()) {
            return tonal[index];
        }
        return CFColor("#808080"); // Fallback gray
    };

    if (!isDark) {
        // Light scheme generation
        r.register_dynamic<CFColor>("md.primary", getTone(4)); // Tone 40
        r.register_dynamic<CFColor>("md.onPrimary", CFColor("#FFFFFF"));
        r.register_dynamic<CFColor>("md.primaryContainer", getTone(9));   // Tone 90
        r.register_dynamic<CFColor>("md.onPrimaryContainer", getTone(0)); // Tone 0

        // Secondary: Use complementary hue
        CFColor secondaryKey(std::fmod(keyColor.hue() + 60.0f, 360.0f), keyColor.chroma() * 0.8f,
                             keyColor.tone());
        QList<CFColor> secondaryPalette = tonalPalette(secondaryKey);
        r.register_dynamic<CFColor>("md.secondary", secondaryPalette[5]);
        r.register_dynamic<CFColor>("md.onSecondary", CFColor("#FFFFFF"));
        r.register_dynamic<CFColor>("md.secondaryContainer", secondaryPalette[9]);
        r.register_dynamic<CFColor>("md.onSecondaryContainer", secondaryPalette[0]);

        // Tertiary: Use complementary hue
        CFColor tertiaryKey(std::fmod(keyColor.hue() + 120.0f, 360.0f), keyColor.chroma() * 0.6f,
                            keyColor.tone());
        QList<CFColor> tertiaryPalette = tonalPalette(tertiaryKey);
        r.register_dynamic<CFColor>("md.tertiary", tertiaryPalette[5]);
        r.register_dynamic<CFColor>("md.onTertiary", CFColor("#FFFFFF"));
        r.register_dynamic<CFColor>("md.tertiaryContainer", tertiaryPalette[9]);
        r.register_dynamic<CFColor>("md.onTertiaryContainer", tertiaryPalette[0]);

        // Error colors (fixed red-based)
        r.register_dynamic<CFColor>("md.error", CFColor("#B3261E"));
        r.register_dynamic<CFColor>("md.onError", CFColor("#FFFFFF"));
        r.register_dynamic<CFColor>("md.errorContainer", CFColor("#F9DEDC"));
        r.register_dynamic<CFColor>("md.onErrorContainer", CFColor("#410E0B"));

        // Surface colors
        r.register_dynamic<CFColor>("md.background", CFColor("#FFFBFE"));
        r.register_dynamic<CFColor>("md.onBackground", CFColor("#1C1B1F"));
        r.register_dynamic<CFColor>("md.surface", CFColor("#FFFBFE"));
        r.register_dynamic<CFColor>("md.onSurface", CFColor("#1C1B1F"));
        r.register_dynamic<CFColor>("md.surfaceVariant", getTone(11));
        r.register_dynamic<CFColor>("md.onSurfaceVariant", getTone(2));
        r.register_dynamic<CFColor>("md.outline", getTone(7));
        r.register_dynamic<CFColor>("md.outlineVariant", getTone(10));

        // Utility colors
        r.register_dynamic<CFColor>("md.shadow", CFColor("#000000"));
        r.register_dynamic<CFColor>("md.scrim", CFColor("#000000"));
        r.register_dynamic<CFColor>("md.inverseSurface", CFColor("#313033"));
        r.register_dynamic<CFColor>("md.inverseOnSurface", CFColor("#F4EFF4"));
        r.register_dynamic<CFColor>("md.inversePrimary", getTone(8));
    } else {
        // Dark scheme generation
        r.register_dynamic<CFColor>("md.primary", getTone(8));             // Tone 80
        r.register_dynamic<CFColor>("md.onPrimary", getTone(0));           // Tone 0
        r.register_dynamic<CFColor>("md.primaryContainer", getTone(3));    // Tone 30
        r.register_dynamic<CFColor>("md.onPrimaryContainer", getTone(11)); // Tone 95

        // Secondary
        CFColor secondaryKey(std::fmod(keyColor.hue() + 60.0f, 360.0f), keyColor.chroma() * 0.8f,
                             keyColor.tone());
        QList<CFColor> secondaryPalette = tonalPalette(secondaryKey);
        r.register_dynamic<CFColor>("md.secondary", secondaryPalette[8]);
        r.register_dynamic<CFColor>("md.onSecondary", secondaryPalette[0]);
        r.register_dynamic<CFColor>("md.secondaryContainer", secondaryPalette[3]);
        r.register_dynamic<CFColor>("md.onSecondaryContainer", secondaryPalette[11]);

        // Tertiary
        CFColor tertiaryKey(std::fmod(keyColor.hue() + 120.0f, 360.0f), keyColor.chroma() * 0.6f,
                            keyColor.tone());
        QList<CFColor> tertiaryPalette = tonalPalette(tertiaryKey);
        r.register_dynamic<CFColor>("md.tertiary", tertiaryPalette[8]);
        r.register_dynamic<CFColor>("md.onTertiary", tertiaryPalette[0]);
        r.register_dynamic<CFColor>("md.tertiaryContainer", tertiaryPalette[3]);
        r.register_dynamic<CFColor>("md.onTertiaryContainer", tertiaryPalette[11]);

        // Error colors (fixed red-based for dark)
        r.register_dynamic<CFColor>("md.error", CFColor("#F2B8B5"));
        r.register_dynamic<CFColor>("md.onError", CFColor("#601410"));
        r.register_dynamic<CFColor>("md.errorContainer", CFColor("#8C1D18"));
        r.register_dynamic<CFColor>("md.onErrorContainer", CFColor("#F9DEDC"));

        // Surface colors
        r.register_dynamic<CFColor>("md.background", CFColor("#1C1B1F"));
        r.register_dynamic<CFColor>("md.onBackground", CFColor("#E6E1E5"));
        r.register_dynamic<CFColor>("md.surface", CFColor("#1C1B1F"));
        r.register_dynamic<CFColor>("md.onSurface", CFColor("#E6E1E5"));
        r.register_dynamic<CFColor>("md.surfaceVariant", getTone(2));
        r.register_dynamic<CFColor>("md.onSurfaceVariant", getTone(10));
        r.register_dynamic<CFColor>("md.outline", getTone(5));
        r.register_dynamic<CFColor>("md.outlineVariant", getTone(2));

        // Utility colors
        r.register_dynamic<CFColor>("md.shadow", CFColor("#000000"));
        r.register_dynamic<CFColor>("md.scrim", CFColor("#000000"));
        r.register_dynamic<CFColor>("md.inverseSurface", CFColor("#E6E1E5"));
        r.register_dynamic<CFColor>("md.inverseOnSurface", CFColor("#313033"));
        r.register_dynamic<CFColor>("md.inversePrimary", getTone(4));
    }

    return scheme;
}

QByteArray toJson(const MaterialColorScheme& scheme) {
    QJsonObject root;
    QJsonObject lightScheme;

    auto& r = scheme.registry();

    // Helper to get color string from registry
    auto getColorString = [&r](const char* key) -> QString {
        auto result = r.get_dynamic_const<::cf::ui::base::CFColor>(key);
        if (result && *result) {
            return (*result)->native_color().name();
        }
        return "#000000";
    };

    // Build scheme object
    lightScheme["primary"] = getColorString("md.primary");
    lightScheme["onPrimary"] = getColorString("md.onPrimary");
    lightScheme["primaryContainer"] = getColorString("md.primaryContainer");
    lightScheme["onPrimaryContainer"] = getColorString("md.onPrimaryContainer");
    lightScheme["secondary"] = getColorString("md.secondary");
    lightScheme["onSecondary"] = getColorString("md.onSecondary");
    lightScheme["secondaryContainer"] = getColorString("md.secondaryContainer");
    lightScheme["onSecondaryContainer"] = getColorString("md.onSecondaryContainer");
    lightScheme["tertiary"] = getColorString("md.tertiary");
    lightScheme["onTertiary"] = getColorString("md.onTertiary");
    lightScheme["tertiaryContainer"] = getColorString("md.tertiaryContainer");
    lightScheme["onTertiaryContainer"] = getColorString("md.onTertiaryContainer");
    lightScheme["error"] = getColorString("md.error");
    lightScheme["onError"] = getColorString("md.onError");
    lightScheme["errorContainer"] = getColorString("md.errorContainer");
    lightScheme["onErrorContainer"] = getColorString("md.onErrorContainer");
    lightScheme["background"] = getColorString("md.background");
    lightScheme["onBackground"] = getColorString("md.onBackground");
    lightScheme["surface"] = getColorString("md.surface");
    lightScheme["onSurface"] = getColorString("md.onSurface");
    lightScheme["surfaceVariant"] = getColorString("md.surfaceVariant");
    lightScheme["onSurfaceVariant"] = getColorString("md.onSurfaceVariant");
    lightScheme["outline"] = getColorString("md.outline");
    lightScheme["outlineVariant"] = getColorString("md.outlineVariant");
    lightScheme["shadow"] = getColorString("md.shadow");
    lightScheme["scrim"] = getColorString("md.scrim");
    lightScheme["inverseSurface"] = getColorString("md.inverseSurface");
    lightScheme["inverseOnSurface"] = getColorString("md.inverseOnSurface");
    lightScheme["inversePrimary"] = getColorString("md.inversePrimary");

    QJsonObject schemes;
    schemes["light"] = lightScheme;

    root["schemes"] = schemes;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented);
}
} // namespace cf::ui::core::material
