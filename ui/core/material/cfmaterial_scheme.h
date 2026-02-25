/**
 * @file cfmaterial_scheme.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Scheme with EmbeddedTokenRegistry
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <QByteArray>
#include <QColor>
#include <string>
#include <unordered_map>

#include "base/color.h"
#include "base/hash/constexpr_fnv1a.hpp"
#include "color_scheme.h"
#include "token.hpp"
#include "token/material_scheme/cfmaterial_token_literals.h"
#include "../../export.h"

namespace cf::ui::core {

using CFColor = cf::ui::base::CFColor;

// =============================================================================
// Color Token Type Aliases - Material Color System
// =============================================================================
namespace tokens {
using namespace cf::ui::core::token::literals;

// Primary color tokens
using PrimaryToken = StaticToken<CFColor, cf::hash::fnv1a64(PRIMARY)>;
using OnPrimaryToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_PRIMARY)>;
using PrimaryContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(PRIMARY_CONTAINER)>;
using OnPrimaryContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_PRIMARY_CONTAINER)>;

// Secondary color tokens
using SecondaryToken = StaticToken<CFColor, cf::hash::fnv1a64(SECONDARY)>;
using OnSecondaryToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_SECONDARY)>;
using SecondaryContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(SECONDARY_CONTAINER)>;
using OnSecondaryContainerToken =
    StaticToken<CFColor, cf::hash::fnv1a64(ON_SECONDARY_CONTAINER)>;

// Tertiary color tokens
using TertiaryToken = StaticToken<CFColor, cf::hash::fnv1a64(TERTIARY)>;
using OnTertiaryToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_TERTIARY)>;
using TertiaryContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(TERTIARY_CONTAINER)>;
using OnTertiaryContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_TERTIARY_CONTAINER)>;

// Error color tokens
using ErrorToken = StaticToken<CFColor, cf::hash::fnv1a64(ERROR)>;
using OnErrorToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_ERROR)>;
using ErrorContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(ERROR_CONTAINER)>;
using OnErrorContainerToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_ERROR_CONTAINER)>;

// Surface color tokens
using BackgroundToken = StaticToken<CFColor, cf::hash::fnv1a64(BACKGROUND)>;
using OnBackgroundToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_BACKGROUND)>;
using SurfaceToken = StaticToken<CFColor, cf::hash::fnv1a64(SURFACE)>;
using OnSurfaceToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_SURFACE)>;
using SurfaceVariantToken = StaticToken<CFColor, cf::hash::fnv1a64(SURFACE_VARIANT)>;
using OnSurfaceVariantToken = StaticToken<CFColor, cf::hash::fnv1a64(ON_SURFACE_VARIANT)>;
using OutlineToken = StaticToken<CFColor, cf::hash::fnv1a64(OUTLINE)>;
using OutlineVariantToken = StaticToken<CFColor, cf::hash::fnv1a64(OUTLINE_VARIANT)>;

// Utility color tokens
using ShadowToken = StaticToken<CFColor, cf::hash::fnv1a64(SHADOW)>;
using ScrimToken = StaticToken<CFColor, cf::hash::fnv1a64(SCRIM)>;
using InverseSurfaceToken = StaticToken<CFColor, cf::hash::fnv1a64(INVERSE_SURFACE)>;
using InverseOnSurfaceToken = StaticToken<CFColor, cf::hash::fnv1a64(INVERSE_ON_SURFACE)>;
using InversePrimaryToken = StaticToken<CFColor, cf::hash::fnv1a64(INVERSE_PRIMARY)>;

} // namespace tokens

// =============================================================================
// Color Group Structs - Material Color System
// =============================================================================

/**
 * @brief Primary color group.
 *
 * Primary colors are used for key components throughout the UI,
 * such as prominent buttons, active states, and tonal surfaces.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct PrimaryColors {
    tokens::PrimaryToken primary;
    tokens::OnPrimaryToken onPrimary;
    tokens::PrimaryContainerToken primaryContainer;
    tokens::OnPrimaryContainerToken onPrimaryContainer;
};

/**
 * @brief Secondary color group.
 *
 * Secondary colors provide alternative ways to distinguish components.
 * They are used for less prominent components and for accents.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct SecondaryColors {
    tokens::SecondaryToken secondary;
    tokens::OnSecondaryToken onSecondary;
    tokens::SecondaryContainerToken secondaryContainer;
    tokens::OnSecondaryContainerToken onSecondaryContainer;
};

/**
 * @brief Tertiary color group.
 *
 * Tertiary colors are used to derive contrasting accents that can be used
 * for balance and to express unique branding.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct TertiaryColors {
    tokens::TertiaryToken tertiary;
    tokens::OnTertiaryToken onTertiary;
    tokens::TertiaryContainerToken tertiaryContainer;
    tokens::OnTertiaryContainerToken onTertiaryContainer;
};

/**
 * @brief Error color group.
 *
 * Error colors are used for error states, destructive actions,
 * and validation feedback.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct ErrorColors {
    tokens::ErrorToken error;
    tokens::OnErrorToken onError;
    tokens::ErrorContainerToken errorContainer;
    tokens::OnErrorContainerToken onErrorContainer;
};

/**
 * @brief Background and Surface color group.
 *
 * These colors define the base surfaces of the UI, including
 * backgrounds, surfaces, and their variants.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct SurfaceColors {
    tokens::BackgroundToken background;
    tokens::OnBackgroundToken onBackground;
    tokens::SurfaceToken surface;
    tokens::OnSurfaceToken onSurface;
    tokens::SurfaceVariantToken surfaceVariant;
    tokens::OnSurfaceVariantToken onSurfaceVariant;
    tokens::OutlineToken outline;
    tokens::OutlineVariantToken outlineVariant;
};

/**
 * @brief Utility color group.
 *
 * Utility colors support elevated surfaces and inverse states
 * for special UI scenarios.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct UtilityColors {
    tokens::ShadowToken shadow;
    tokens::ScrimToken scrim;
    tokens::InverseSurfaceToken inverseSurface;
    tokens::InverseOnSurfaceToken inverseOnSurface;
    tokens::InversePrimaryToken inversePrimary;
};

// =============================================================================
// Material Color Scheme
// =============================================================================

/**
 * @brief Material Design 3 Color Scheme with EmbeddedTokenRegistry.
 *
 * Implements the complete Material Design 3 color system with 26 color tokens.
 * Colors are stored in an embedded registry for independent scheme instances.
 *
 * Factory functions are available in the cf::ui::core::material namespace.
 *
 * @since 0.1
 * @ingroup ui_core
 *
 * @code
 * #include "material_factory.hpp"
 *
 * // Create a light theme
 * auto lightScheme = cf::ui::core::material::light();
 *
 * // Query colors by name
 * QColor primary = lightScheme.queryExpectedColor("md.primary");
 * @endcode
 */
class CF_UI_EXPORT MaterialColorScheme : public ICFColorScheme {
  public:
    MaterialColorScheme();
    ~MaterialColorScheme() override = default;

    // Non-copyable, movable
    MaterialColorScheme(const MaterialColorScheme&) = delete;
    MaterialColorScheme& operator=(const MaterialColorScheme&) = delete;
    MaterialColorScheme(MaterialColorScheme&&) noexcept = default;
    MaterialColorScheme& operator=(MaterialColorScheme&&) noexcept = default;

    /**
     * @brief Query a color by name.
     *
     * @param name Color token name (e.g., "md.primary").
     * @return Reference to the QColor.
     *
     * @since 0.1
     */
    QColor& queryExpectedColor(const char* name) override;

    /**
     * @brief Query a color by name (const overload).
     *
     * @param name Color token name.
     * @return Copy of the QColor.
     *
     * @since 0.1
     */
    QColor queryColor(const char* name) const;

    /**
     * @brief Access the embedded token registry.
     *
     * @return Reference to the EmbeddedTokenRegistry.
     *
     * @since 0.1
     */
    EmbeddedTokenRegistry& registry() { return registry_; }
    const EmbeddedTokenRegistry& registry() const { return registry_; }

    // Color group accessors - return structs with token types
    [[nodiscard]] PrimaryColors primary() const { return {}; }
    [[nodiscard]] SecondaryColors secondary() const { return {}; }
    [[nodiscard]] TertiaryColors tertiary() const { return {}; }
    [[nodiscard]] ErrorColors error() const { return {}; }
    [[nodiscard]] SurfaceColors surface() const { return {}; }
    [[nodiscard]] UtilityColors utility() const { return {}; }

  private:
    EmbeddedTokenRegistry registry_;
    mutable std::unordered_map<std::string, QColor> color_cache_;
};

} // namespace cf::ui::core
