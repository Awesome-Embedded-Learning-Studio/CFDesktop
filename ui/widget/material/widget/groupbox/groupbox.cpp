/**
 * @file groupbox.cpp
 * @brief Material Design 3 GroupBox Implementation
 *
 * Implements a Material Design 3 group box with rounded corners,
 * optional elevation shadows, and theme-aware colors.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "groupbox.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "cfmaterial_animation_factory.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/elevation_controller.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGroupBox>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Constructor / Destructor
// ============================================================================

GroupBox::GroupBox(QWidget* parent) : QGroupBox(parent), m_cornerRadius(-1.0f), m_hasBorder(true) {
    // Get animation factory from Application
    m_animationFactory =
        aex::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Initialize behavior components
    m_elevation = new MdElevationController(m_animationFactory, this);

    // Set initial elevation (default: level 1 for subtle depth)
    m_elevation->setElevation(1);

    // Enable custom drawing without automatic clipping
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_TranslucentBackground, false);

    // Disable automatic background drawing to avoid double-draw issues
    setAttribute(Qt::WA_StyledBackground, false);

    // Check if title exists
    m_hasTitle = !title().isEmpty();

    // Set initial content margins to ensure proper layout
    updateContentMargins();
}

GroupBox::GroupBox(const QString& title, QWidget* parent) : GroupBox(parent) {
    setTitle(title);
    m_hasTitle = !title.isEmpty();
    // Update margins since we now have a title
    updateContentMargins();
}

GroupBox::~GroupBox() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

int GroupBox::elevation() const {
    return m_elevation ? m_elevation->elevation() : 0;
}

void GroupBox::setElevation(int level) {
    if (m_elevation) {
        m_elevation->setElevation(level);
        updateContentMargins(); // Shadow margin changed, update layout
        update();
    }
}

float GroupBox::cornerRadius() const {
    // If corner radius is not explicitly set (negative), use default value
    if (m_cornerRadius < 0.0f) {
        // Use Material Design ShapeSmall (8dp) as default
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        return helper.dpToPx(8.0f);
    }
    return m_cornerRadius;
}

void GroupBox::setCornerRadius(float radius) {
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        update();
    }
}

bool GroupBox::hasBorder() const {
    return m_hasBorder;
}

void GroupBox::setHasBorder(bool hasBorder) {
    if (m_hasBorder != hasBorder) {
        m_hasBorder = hasBorder;
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize GroupBox::sizeHint() const {
    // QGroupBox's sizeHint already includes our contentsMargins,
    // which we've set to include shadow and title space in updateContentMargins()
    return QGroupBox::sizeHint();
}

QSize GroupBox::minimumSizeHint() const {
    // QGroupBox's minimumSizeHint already includes our contentsMargins
    return QGroupBox::minimumSizeHint();
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
} // Surface
inline CFColor fallbackSurfaceVariant() {
    return CFColor(231, 224, 236);
} // Surface Variant
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
} // namespace

CFColor GroupBox::surfaceColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(SURFACE));
    } catch (...) {
        return fallbackSurface();
    }
}

CFColor GroupBox::outlineColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOutline();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(OUTLINE));
    } catch (...) {
        return fallbackOutline();
    }
}

CFColor GroupBox::titleColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_SURFACE));
    } catch (...) {
        return fallbackOnSurface();
    }
}

QFont GroupBox::titleFont() const {
    auto* app = Application::instance();
    if (!app) {
        // Fallback to system font with reasonable size
        QFont font = QGroupBox::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("titleSmall");
    } catch (...) {
        QFont font = QGroupBox::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }
}

// ============================================================================
// Child Event
// ============================================================================

void GroupBox::childEvent(QChildEvent* event) {
    QGroupBox::childEvent(event);
    // Update when children change to ensure proper redraw
    if (event->type() == QChildEvent::ChildAdded || event->type() == QChildEvent::ChildRemoved) {
        updateGeometry();
        update();
    }
}

// ============================================================================
// Change Event
// ============================================================================

void GroupBox::changeEvent(QEvent* event) {
    QGroupBox::changeEvent(event);
    if (event->type() == QEvent::WindowTitleChange) {
        m_hasTitle = !title().isEmpty();
        updateContentMargins();
        update();
    }
}

// ============================================================================
// Resize Event
// ============================================================================

void GroupBox::resizeEvent(QResizeEvent* event) {
    QGroupBox::resizeEvent(event);
    // Update content margins on resize to ensure proper layout
    updateContentMargins();
}

// ============================================================================
// Layout Helpers
// ============================================================================

void GroupBox::updateContentMargins() {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate shadow margin (extra space needed for shadow rendering)
    QMarginsF shadowMargin = this->shadowMargin();
    int shadowLeft = int(std::ceil(shadowMargin.left()));
    int shadowTop = int(std::ceil(shadowMargin.top()));
    int shadowRight = int(std::ceil(shadowMargin.right()));
    int shadowBottom = int(std::ceil(shadowMargin.bottom()));

    // Calculate title area height (space needed for title at the top)
    int titleTopSpace = 0;
    if (m_hasTitle) {
        QFont font = titleFont();
        QFontMetrics fm(font);
        float titleTopPadding = helper.dpToPx(8.0f); // Distance from top of contentRect
        float titleHeight = fm.height();
        float titleBottomGap = helper.dpToPx(4.0f); // Extra gap below title
        titleTopSpace = int(std::ceil(titleTopPadding + titleHeight + titleBottomGap));
    }

    // Additional padding to keep content away from rounded corners
    // This prevents child widgets from drawing into the corner curve area
    float radius = cornerRadius();
    int cornerAvoidance = int(std::ceil(radius * 0.6f)); // Keep 60% of radius as clearance

    // Standard content padding
    int contentPadding = int(std::ceil(helper.dpToPx(16.0f)));

    // Set content margins:
    // - Left/Right: shadow margin + corner avoidance + standard content padding
    // - Top: shadow margin + title space + standard content padding
    // - Bottom: shadow margin + standard content padding
    setContentsMargins(
        shadowLeft + cornerAvoidance + contentPadding, shadowTop + titleTopSpace + contentPadding,
        shadowRight + cornerAvoidance + contentPadding, shadowBottom + contentPadding);
}

// ============================================================================
// Paint Event
// ============================================================================

void GroupBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Calculate content area (inset to make room for shadow)
    QMarginsF margin = shadowMargin();
    QRectF contentRect =
        QRectF(rect()).adjusted(margin.left(), margin.top(), -margin.right(), -margin.bottom());

    // Create shape path (rounded corners) from contentRect
    QPainterPath shape = roundedRect(contentRect, cornerRadius());

    // Step 1: Draw shadow (outside contentRect)
    drawShadow(p, contentRect, shape);

    // Step 2: Draw background
    drawBackground(p, shape);

    // Step 3: Draw border (with title gap if has title)
    if (m_hasBorder) {
        drawBorder(p, contentRect, shape);
    }

    // Step 4: Draw title (straddling the top border)
    if (m_hasTitle) {
        drawTitle(p, contentRect);
    }
}

// ============================================================================
// Drawing Helpers
// ============================================================================

// Calculate shadow margin (extra space needed for shadow)
QMarginsF GroupBox::shadowMargin() const {
    if (!m_elevation || m_elevation->elevation() <= 0) {
        return QMarginsF(0, 0, 0, 0);
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    // According to elevation level, calculate margin dynamically
    // Level 1: blur=2dp, offset=1dp, max offset ~1.5dp
    // Reserve margin = offset + blur/2
    int level = m_elevation->elevation();
    float margin = helper.dpToPx(1.0f + level * 1.5f);
    return QMarginsF(margin, margin, margin, margin);
}

void GroupBox::drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape) {
    if (m_elevation && m_elevation->elevation() > 0) {
        m_elevation->paintShadow(&p, shape);
    }
}

void GroupBox::drawBackground(QPainter& p, const QPainterPath& shape) {
    CFColor bg = surfaceColor();

    // Group box background is typically surface color
    p.fillPath(shape, bg.native_color());
}

void GroupBox::drawBorder(QPainter& p, const QRectF& contentRect, const QPainterPath& shape) {
    QColor color = outlineColor().native_color();
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();

    // Use 0.5px inset for border (QPen strokes are center-aligned)
    float inset = 0.5f;

    // Create inset path for border
    QRectF shapeBounds = shape.boundingRect();
    QRectF insetRect = shapeBounds.adjusted(inset, inset, -inset, -inset);
    float adjustedRadius = std::max(0.0f, cornerRadius() - inset);
    QPainterPath insetShape = roundedRect(insetRect, adjustedRadius);

    QPen pen(color, 1.0);  // 1px width
    pen.setCosmetic(true); // Keep consistent 1px width across DPI
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    // If has title, we need to clear the border where title sits
    if (m_hasTitle) {
        QRectF tArea = titleArea();
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float titleGapExtend = helper.dpToPx(6.0f);

        QRectF gapRect = tArea.adjusted(-titleGapExtend, -helper.dpToPx(2.0f), titleGapExtend,
                                        helper.dpToPx(2.0f));

        QRegion fullRegion(rect());
        QRegion gapRegion(gapRect.toRect());
        p.setClipRegion(fullRegion - gapRegion);

        p.drawPath(insetShape);

        p.setClipping(false); // 恢复
    } else {
        p.drawPath(insetShape);
    }

    p.restore();
}

void GroupBox::drawTitle(QPainter& p, const QRectF& contentRect) {
    QString titleText = title();
    if (titleText.isEmpty()) {
        return;
    }

    // Get title font and color
    QFont font = titleFont();
    QFontMetrics fm(font);
    CFColor titleC = titleColor();

    p.save();

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float titlePadding = helper.dpToPx(16.0f);
    float titleTopPadding = helper.dpToPx(8.0f); // Distance from top of contentRect
    float titleHeight = fm.height();

    // Title is positioned fully inside the contentRect, at the top
    // The border and background start below the title area
    float titleY = contentRect.top() + titleTopPadding;

    QRectF textRect(contentRect.left() + titlePadding, titleY,
                    contentRect.width() - titlePadding * 2, titleHeight);

    // Draw title text
    p.setFont(font);
    QColor textColor = titleC.native_color();
    if (!isEnabled()) {
        textColor.setAlphaF(0.38f);
    }
    p.setPen(textColor);

    p.drawText(textRect, Qt::AlignLeft | Qt::AlignTop, titleText);

    p.restore();
}

// ============================================================================
// Title Area Calculation
// ============================================================================

QRectF GroupBox::titleArea() const {
    QString titleText = title();
    if (titleText.isEmpty()) {
        return QRectF();
    }

    QFont font = titleFont();
    QFontMetrics fm(font);
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float titlePadding = helper.dpToPx(16.0f);
    float titleTopPadding = helper.dpToPx(8.0f);
    float titleTextWidth = fm.horizontalAdvance(titleText);
    float titleHeight = fm.height();

    QMarginsF margin = shadowMargin();
    QRectF contentRect =
        QRectF(rect()).adjusted(margin.left(), margin.top(), -margin.right(), -margin.bottom());

    // Title area matches drawTitle() position (inside contentRect at top)
    float titleY = contentRect.top() + titleTopPadding;

    return QRectF(contentRect.left() + titlePadding, titleY, titleTextWidth, titleHeight);
}

} // namespace cf::ui::widget::material
