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
#include "cfmaterial_fonttype.h"
#include "cfmaterial_motion.h"
#include "cfmaterial_radius_scale.h"
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
    // Primary colors
    scheme.setColor("md.primary", QColor(ColorDefs::primary));
    scheme.setColor("md.onPrimary", QColor(ColorDefs::onPrimary));
    scheme.setColor("md.primaryContainer", QColor(ColorDefs::primaryContainer));
    scheme.setColor("md.onPrimaryContainer", QColor(ColorDefs::onPrimaryContainer));

    // Secondary colors
    scheme.setColor("md.secondary", QColor(ColorDefs::secondary));
    scheme.setColor("md.onSecondary", QColor(ColorDefs::onSecondary));
    scheme.setColor("md.secondaryContainer", QColor(ColorDefs::secondaryContainer));
    scheme.setColor("md.onSecondaryContainer", QColor(ColorDefs::onSecondaryContainer));

    // Tertiary colors
    scheme.setColor("md.tertiary", QColor(ColorDefs::tertiary));
    scheme.setColor("md.onTertiary", QColor(ColorDefs::onTertiary));
    scheme.setColor("md.tertiaryContainer", QColor(ColorDefs::tertiaryContainer));
    scheme.setColor("md.onTertiaryContainer", QColor(ColorDefs::onTertiaryContainer));

    // Error colors
    scheme.setColor("md.error", QColor(ColorDefs::error));
    scheme.setColor("md.onError", QColor(ColorDefs::onError));
    scheme.setColor("md.errorContainer", QColor(ColorDefs::errorContainer));
    scheme.setColor("md.onErrorContainer", QColor(ColorDefs::onErrorContainer));

    // Surface colors
    scheme.setColor("md.background", QColor(ColorDefs::background));
    scheme.setColor("md.onBackground", QColor(ColorDefs::onBackground));
    scheme.setColor("md.surface", QColor(ColorDefs::surface));
    scheme.setColor("md.onSurface", QColor(ColorDefs::onSurface));
    scheme.setColor("md.surfaceVariant", QColor(ColorDefs::surfaceVariant));
    scheme.setColor("md.onSurfaceVariant", QColor(ColorDefs::onSurfaceVariant));
    scheme.setColor("md.outline", QColor(ColorDefs::outline));
    scheme.setColor("md.outlineVariant", QColor(ColorDefs::outlineVariant));

    // Utility colors
    scheme.setColor("md.shadow", QColor(ColorDefs::shadow));
    scheme.setColor("md.scrim", QColor(ColorDefs::scrim));
    scheme.setColor("md.inverseSurface", QColor(ColorDefs::inverseSurface));
    scheme.setColor("md.inverseOnSurface", QColor(ColorDefs::inverseOnSurface));
    scheme.setColor("md.inversePrimary", QColor(ColorDefs::inversePrimary));
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
        return ::aex::unexpected(
            MaterialSchemeError{MaterialSchemeError::Kind::InvalidJson,
                                "Failed to parse JSON: " + parseError.errorString().toStdString()});
    }

    if (!doc.isObject()) {
        return ::aex::unexpected(MaterialSchemeError{MaterialSchemeError::Kind::InvalidJson,
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
                return ::aex::unexpected(MaterialSchemeError{
                    MaterialSchemeError::Kind::InvalidColorFormat,
                    "Invalid color format for " + key + ": " + colorStr.toStdString()});
            }
            scheme.setColor(registryKey, QColor(colorStr));
        }
    }

    return scheme;
}

MaterialColorScheme fromKeyColor(cf::ui::base::CFColor keyColor, bool isDark) {
    using ::cf::ui::base::tonalPalette;

    MaterialColorScheme scheme;

    // Generate tonal palette from key color
    QList<cf::ui::base::CFColor> tonal = tonalPalette(keyColor);

    // Helper to get color from tonal palette by index
    auto getTone = [&tonal](int index) -> cf::ui::base::CFColor {
        if (index >= 0 && index < tonal.size()) {
            return tonal[index];
        }
        return cf::ui::base::CFColor("#808080"); // Fallback gray
    };

    if (!isDark) {
        // Light scheme generation
        scheme.setColor("md.primary", getTone(4).native_color());
        scheme.setColor("md.onPrimary", QColor("#FFFFFF"));
        scheme.setColor("md.primaryContainer", getTone(9).native_color());
        scheme.setColor("md.onPrimaryContainer", getTone(0).native_color());

        // Secondary: Use complementary hue
        cf::ui::base::CFColor secondaryKey(std::fmod(keyColor.hue() + 60.0f, 360.0f),
                                           keyColor.chroma() * 0.8f, keyColor.tone());
        QList<cf::ui::base::CFColor> secondaryPalette = tonalPalette(secondaryKey);
        scheme.setColor("md.secondary", secondaryPalette[5].native_color());
        scheme.setColor("md.onSecondary", QColor("#FFFFFF"));
        scheme.setColor("md.secondaryContainer", secondaryPalette[9].native_color());
        scheme.setColor("md.onSecondaryContainer", secondaryPalette[0].native_color());

        // Tertiary: Use complementary hue
        cf::ui::base::CFColor tertiaryKey(std::fmod(keyColor.hue() + 120.0f, 360.0f),
                                          keyColor.chroma() * 0.6f, keyColor.tone());
        QList<cf::ui::base::CFColor> tertiaryPalette = tonalPalette(tertiaryKey);
        scheme.setColor("md.tertiary", tertiaryPalette[5].native_color());
        scheme.setColor("md.onTertiary", QColor("#FFFFFF"));
        scheme.setColor("md.tertiaryContainer", tertiaryPalette[9].native_color());
        scheme.setColor("md.onTertiaryContainer", tertiaryPalette[0].native_color());

        // Error colors (fixed red-based)
        scheme.setColor("md.error", QColor("#B3261E"));
        scheme.setColor("md.onError", QColor("#FFFFFF"));
        scheme.setColor("md.errorContainer", QColor("#F9DEDC"));
        scheme.setColor("md.onErrorContainer", QColor("#410E0B"));

        // Surface colors
        scheme.setColor("md.background", QColor("#FFFBFE"));
        scheme.setColor("md.onBackground", QColor("#1C1B1F"));
        scheme.setColor("md.surface", QColor("#FFFBFE"));
        scheme.setColor("md.onSurface", QColor("#1C1B1F"));
        scheme.setColor("md.surfaceVariant", getTone(11).native_color());
        scheme.setColor("md.onSurfaceVariant", getTone(2).native_color());
        scheme.setColor("md.outline", getTone(7).native_color());
        scheme.setColor("md.outlineVariant", getTone(10).native_color());

        // Utility colors
        scheme.setColor("md.shadow", QColor("#000000"));
        scheme.setColor("md.scrim", QColor("#000000"));
        scheme.setColor("md.inverseSurface", QColor("#313033"));
        scheme.setColor("md.inverseOnSurface", QColor("#F4EFF4"));
        scheme.setColor("md.inversePrimary", getTone(8).native_color());
    } else {
        // Dark scheme generation
        scheme.setColor("md.primary", getTone(8).native_color());
        scheme.setColor("md.onPrimary", getTone(0).native_color());
        scheme.setColor("md.primaryContainer", getTone(3).native_color());
        scheme.setColor("md.onPrimaryContainer", getTone(11).native_color());

        // Secondary
        cf::ui::base::CFColor secondaryKey(std::fmod(keyColor.hue() + 60.0f, 360.0f),
                                           keyColor.chroma() * 0.8f, keyColor.tone());
        QList<cf::ui::base::CFColor> secondaryPalette = tonalPalette(secondaryKey);
        scheme.setColor("md.secondary", secondaryPalette[8].native_color());
        scheme.setColor("md.onSecondary", secondaryPalette[0].native_color());
        scheme.setColor("md.secondaryContainer", secondaryPalette[3].native_color());
        scheme.setColor("md.onSecondaryContainer", secondaryPalette[11].native_color());

        // Tertiary
        cf::ui::base::CFColor tertiaryKey(std::fmod(keyColor.hue() + 120.0f, 360.0f),
                                          keyColor.chroma() * 0.6f, keyColor.tone());
        QList<cf::ui::base::CFColor> tertiaryPalette = tonalPalette(tertiaryKey);
        scheme.setColor("md.tertiary", tertiaryPalette[8].native_color());
        scheme.setColor("md.onTertiary", tertiaryPalette[0].native_color());
        scheme.setColor("md.tertiaryContainer", tertiaryPalette[3].native_color());
        scheme.setColor("md.onTertiaryContainer", tertiaryPalette[11].native_color());

        // Error colors (fixed red-based for dark)
        scheme.setColor("md.error", QColor("#F2B8B5"));
        scheme.setColor("md.onError", QColor("#601410"));
        scheme.setColor("md.errorContainer", QColor("#8C1D18"));
        scheme.setColor("md.onErrorContainer", QColor("#F9DEDC"));

        // Surface colors
        scheme.setColor("md.background", QColor("#1C1B1F"));
        scheme.setColor("md.onBackground", QColor("#E6E1E5"));
        scheme.setColor("md.surface", QColor("#1C1B1F"));
        scheme.setColor("md.onSurface", QColor("#E6E1E5"));
        scheme.setColor("md.surfaceVariant", getTone(2).native_color());
        scheme.setColor("md.onSurfaceVariant", getTone(10).native_color());
        scheme.setColor("md.outline", getTone(5).native_color());
        scheme.setColor("md.outlineVariant", getTone(2).native_color());

        // Utility colors
        scheme.setColor("md.shadow", QColor("#000000"));
        scheme.setColor("md.scrim", QColor("#000000"));
        scheme.setColor("md.inverseSurface", QColor("#E6E1E5"));
        scheme.setColor("md.inverseOnSurface", QColor("#313033"));
        scheme.setColor("md.inversePrimary", getTone(4).native_color());
    }

    return scheme;
}

QByteArray toJson(const MaterialColorScheme& scheme) {
    QJsonObject lightScheme;

    // Helper to get color string from scheme
    auto getColorString = [&scheme](const char* key) -> QString {
        QColor color = scheme.queryColor(key);
        return color.isValid() ? color.name() : "#000000";
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

    QJsonObject root;
    root["schemes"] = schemes;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented);
}

// =============================================================================
// Typography Factory Functions
// =============================================================================

MaterialTypography defaultTypography() {
    return MaterialTypography();
}

// =============================================================================
// Radius Scale Factory Functions
// =============================================================================

MaterialRadiusScale defaultRadiusScale() {
    return MaterialRadiusScale();
}

// =============================================================================
// Motion Factory Functions
// =============================================================================

MaterialMotionScheme motion() {
    return MaterialMotionScheme();
}

} // namespace cf::ui::core::material

// =============================================================================
// MaterialFactory Class Implementation
// =============================================================================

#include "../token/theme_name/material_theme_name.h"
#include "cfmaterial_theme.h"
#include "material_factory_class.h"

namespace cf::ui::core {

std::unique_ptr<ICFTheme> MaterialFactory::fromName(const char* name) {
    using namespace token::literals;

    // Check for light theme
    if (std::strcmp(name, MATERIAL_THEME_LIGHT) == 0) {
        auto color_scheme = std::make_unique<MaterialColorScheme>(material::light());
        auto font_type = std::make_unique<MaterialTypography>(material::defaultTypography());
        auto radius_scale = std::make_unique<MaterialRadiusScale>(material::defaultRadiusScale());
        auto motion_spec = std::make_unique<MaterialMotionScheme>(material::motion());

        return std::unique_ptr<ICFTheme>(
            new MaterialTheme(std::move(color_scheme), std::move(font_type),
                              std::move(radius_scale), std::move(motion_spec)));
    }

    // Check for dark theme
    if (std::strcmp(name, MATERIAL_THEME_DARK) == 0) {
        auto color_scheme = std::make_unique<MaterialColorScheme>(material::dark());
        auto font_type = std::make_unique<MaterialTypography>(material::defaultTypography());
        auto radius_scale = std::make_unique<MaterialRadiusScale>(material::defaultRadiusScale());
        auto motion_spec = std::make_unique<MaterialMotionScheme>(material::motion());

        return std::unique_ptr<ICFTheme>(
            new MaterialTheme(std::move(color_scheme), std::move(font_type),
                              std::move(radius_scale), std::move(motion_spec)));
    }

    // Unknown theme name
    return nullptr;
}

std::unique_ptr<ICFTheme> MaterialFactory::fromJson(const QByteArray& json) {
    // Try to parse as light theme first, then dark if needed
    auto lightResult = material::fromJson(json, false);
    if (lightResult) {
        auto color_scheme = std::make_unique<MaterialColorScheme>(std::move(*lightResult));
        auto font_type = std::make_unique<MaterialTypography>(material::defaultTypography());
        auto radius_scale = std::make_unique<MaterialRadiusScale>(material::defaultRadiusScale());
        auto motion_spec = std::make_unique<MaterialMotionScheme>(material::motion());

        return std::unique_ptr<ICFTheme>(
            new MaterialTheme(std::move(color_scheme), std::move(font_type),
                              std::move(radius_scale), std::move(motion_spec)));
    }

    // If light parsing failed, try dark
    auto darkResult = material::fromJson(json, true);
    if (darkResult) {
        auto color_scheme = std::make_unique<MaterialColorScheme>(std::move(*darkResult));
        auto font_type = std::make_unique<MaterialTypography>(material::defaultTypography());
        auto radius_scale = std::make_unique<MaterialRadiusScale>(material::defaultRadiusScale());
        auto motion_spec = std::make_unique<MaterialMotionScheme>(material::motion());

        return std::unique_ptr<ICFTheme>(
            new MaterialTheme(std::move(color_scheme), std::move(font_type),
                              std::move(radius_scale), std::move(motion_spec)));
    }

    // Parsing failed for both light and dark
    return nullptr;
}

QByteArray MaterialFactory::toJson(ICFTheme* raw_theme) {
    if (!raw_theme) {
        return QByteArray();
    }

    // Get the color scheme from the theme
    auto& color_scheme = raw_theme->color_scheme();

    // Try to cast to MaterialColorScheme
    auto* material_scheme = dynamic_cast<MaterialColorScheme*>(&color_scheme);
    if (material_scheme) {
        return material::toJson(*material_scheme);
    }

    // Not a MaterialColorScheme, return empty JSON
    return QByteArray();
}

} // namespace cf::ui::core
