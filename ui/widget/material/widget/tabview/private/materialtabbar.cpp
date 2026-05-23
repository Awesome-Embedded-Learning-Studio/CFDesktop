/**
 * @file materialtabbar.cpp
 * @brief Material Design 3 TabBar Implementation
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @ingroup ui_widget_material
 */

#include "materialtabbar.h"
#include "../tabview.h"

#include "application_support/application.h"
#include "base/device_pixel.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

namespace {
constexpr int DEFAULT_TAB_HEIGHT_DP = 48;
constexpr int DEFAULT_TAB_MIN_WIDTH_DP = 120;
constexpr int INDICATOR_HEIGHT_DP = 3;

inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
}
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(117, 117, 117);
}
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
}
} // namespace

// ============================================================================
// MaterialTabBar Implementation
// ============================================================================

MaterialTabBar::MaterialTabBar(TabView* parent)
    : QTabBar(parent), m_tabView(parent), m_tabHeightDp(DEFAULT_TAB_HEIGHT_DP),
      m_tabMinWidthDp(DEFAULT_TAB_MIN_WIDTH_DP), m_showIndicator(true), m_indicatorPosition(0.0f),
      m_indicatorTargetPosition(0.0f), m_lastIndex(-1), m_hoveredIndex(-1), m_pressedIndex(-1),
      m_closeButtonHoveredIndex(-1), m_stateMachine(nullptr), m_focusIndicator(nullptr) {

    setDrawBase(false);
    setDocumentMode(true);
    setExpanding(false);
    setUsesScrollButtons(true);
    setElideMode(Qt::ElideRight);
    setMouseTracking(true);

    m_animationFactory =
        cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));
    connect(this, &QTabBar::currentChanged, this, &MaterialTabBar::animateIndicatorTo);
}

MaterialTabBar::~MaterialTabBar() {
    // Components are parented to this, Qt will delete them
}

void MaterialTabBar::setTabHeightDp(int height) {
    if (m_tabHeightDp != height) {
        m_tabHeightDp = height;
        updateGeometry();
    }
}

void MaterialTabBar::setTabMinWidthDp(int width) {
    if (m_tabMinWidthDp != width) {
        m_tabMinWidthDp = width;
        updateGeometry();
    }
}

void MaterialTabBar::setShowIndicator(bool show) {
    if (m_showIndicator != show) {
        m_showIndicator = show;
        update();
    }
}

void MaterialTabBar::setTabCloseable(int index, bool closeable) {
    if (closeable) {
        m_closeableTabs.insert(index);
    } else {
        m_closeableTabs.remove(index);
    }
    updateGeometry();
    update();
}

bool MaterialTabBar::isTabCloseable(int index) const {
    return m_closeableTabs.contains(index);
}

QSize MaterialTabBar::tabSizeHint(int index) const {
    Q_UNUSED(index)

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float minWidth = helper.dpToPx(static_cast<float>(m_tabMinWidthDp));
    float height = helper.dpToPx(static_cast<float>(m_tabHeightDp));

    return QSize(static_cast<int>(std::ceil(minWidth)), static_cast<int>(std::ceil(height)));
}

void MaterialTabBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), backgroundColor().native_color());

    for (int i = 0; i < count(); ++i) {
        drawTab(p, i);
    }

    if (m_showIndicator) {
        drawIndicator(p);
    }
}

void MaterialTabBar::drawTab(QPainter& p, int index) {
    QRect tabRect = this->tabRect(index);
    if (tabRect.isEmpty()) {
        return;
    }

    drawTabStateLayer(p, tabRect, index);
    drawTabContent(p, tabRect, index);

    if (index == currentIndex() && hasFocus() && m_focusIndicator) {
        QPainterPath shape;
        shape.addRoundedRect(tabRect.adjusted(2, 2, -2, -2), 4, 4);
        m_focusIndicator->paint(&p, shape, focusIndicatorColor());
    }
}

void MaterialTabBar::drawTabStateLayer(QPainter& p, const QRect& tabRect, int index) {
    if (!isEnabled() || !m_stateMachine) {
        return;
    }

    bool isHovered = (index == m_hoveredIndex);
    bool isPressed = (index == m_pressedIndex);
    bool isSelected = (index == currentIndex());

    if (!isHovered && !isPressed && !isSelected) {
        return;
    }

    float opacity = isPressed ? 0.12f : (isHovered ? 0.08f : 0.08f);

    if (opacity <= 0.0f) {
        return;
    }

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);
    p.fillRect(tabRect, color);
}

void MaterialTabBar::drawTabContent(QPainter& p, const QRect& tabRect, int index) {
    CFColor textColor = (index == currentIndex()) ? selectedTabTextColor() : tabTextColor();

    if (!isEnabled()) {
        QColor color = textColor.native_color();
        color.setAlphaF(0.38f);
        p.setPen(color);
    } else {
        p.setPen(textColor.native_color());
    }

    QFont font = tabFont();
    p.setFont(font);

    QString text = this->tabText(index);
    QRect textRect = tabRect.adjusted(16, 0, -16, 0);

    QIcon icon = this->tabIcon(index);
    if (!icon.isNull()) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float iconSize = helper.dpToPx(24.0f);
        float iconGap = helper.dpToPx(8.0f);

        QRect iconRect(textRect.left(), textRect.center().y() - iconSize / 2,
                       static_cast<int>(iconSize), static_cast<int>(iconSize));
        icon.paint(&p, iconRect);
        textRect.setLeft(iconRect.right() + static_cast<int>(iconGap));
    }

    // Handle closeable tabs - reserve space for close button
    if (isTabCloseable(index)) {
        QRect closeRect = closeButtonRect(index);
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float closeGap = helper.dpToPx(8.0f);
        textRect.setRight(closeRect.left() - static_cast<int>(closeGap));
        drawCloseIcon(p, closeRect, index);
    }

    QString elidedText = fontMetrics().elidedText(text, Qt::ElideRight, textRect.width());
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
}

void MaterialTabBar::drawIndicator(QPainter& p) {
    if (count() == 0) {
        return;
    }

    updateIndicatorPosition();

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float indicatorHeight = helper.dpToPx(static_cast<float>(INDICATOR_HEIGHT_DP));
    int currentIndexVal = currentIndex();

    if (currentIndexVal < 0 || currentIndexVal >= count()) {
        return;
    }

    QRect currentTabRect = tabRect(currentIndexVal);
    float indicatorWidth = currentTabRect.width() - 32;
    float indicatorX = currentTabRect.left() + 16;
    float indicatorY = height() - indicatorHeight;

    QRectF indicatorRect(indicatorX, indicatorY, indicatorWidth, indicatorHeight);

    float radius = indicatorHeight / 2.0f;
    QPainterPath path;
    path.addRoundedRect(indicatorRect, radius, radius);
    p.fillPath(path, indicatorColor().native_color());
}

void MaterialTabBar::animateIndicatorTo(int index) {
    if (index < 0 || index >= count()) {
        return;
    }

    int oldIndex = m_lastIndex;
    m_lastIndex = index;

    if (!m_animationFactory || !m_animationFactory->isAllEnabled()) {
        m_indicatorPosition = static_cast<float>(tabRect(index).left());
        update();
        return;
    }

    m_indicatorPosition = static_cast<float>(tabRect(index).left());
    update();
}

void MaterialTabBar::updateIndicatorPosition() {
    if (!m_indicatorAnimation || !m_indicatorAnimation.Get()) {
        int current = currentIndex();
        if (current >= 0 && current < count()) {
            m_indicatorPosition = static_cast<float>(tabRect(current).left());
        }
    }
}

QRect MaterialTabBar::closeButtonRect(int index) const {
    if (index < 0 || index >= count()) {
        return QRect();
    }

    QRect tabRect = this->tabRect(index);
    if (tabRect.isEmpty()) {
        return QRect();
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float closeSize = helper.dpToPx(18.0f);
    float rightMargin = helper.dpToPx(8.0f);

    int closeX = tabRect.right() - static_cast<int>(closeSize) - static_cast<int>(rightMargin);
    int closeY = tabRect.center().y() - static_cast<int>(closeSize / 2);

    return QRect(closeX, closeY, static_cast<int>(closeSize), static_cast<int>(closeSize));
}

bool MaterialTabBar::isOverCloseButton(int index, const QPoint& pos) const {
    if (!isTabCloseable(index)) {
        return false;
    }
    QRect closeRect = closeButtonRect(index);
    return closeRect.contains(pos);
}

void MaterialTabBar::updateCloseButtonHover(const QPoint& pos) {
    int newCloseHoverIndex = -1;

    for (int i = 0; i < count(); ++i) {
        if (isOverCloseButton(i, pos)) {
            newCloseHoverIndex = i;
            break;
        }
    }

    if (newCloseHoverIndex != m_closeButtonHoveredIndex) {
        m_closeButtonHoveredIndex = newCloseHoverIndex;
        update();
    }
}

void MaterialTabBar::drawCloseIcon(QPainter& p, const QRect& closeRect, int index) {
    bool isHovered = (index == m_closeButtonHoveredIndex);
    bool isSelected = (index == currentIndex());

    CFColor baseColor = isSelected ? selectedTabTextColor() : tabTextColor();
    QColor color = baseColor.native_color();

    if (isHovered) {
        color.setAlphaF(0.87f);
    } else {
        color.setAlphaF(0.6f);
    }

    p.setPen(QPen(color, 1.5));

    float pad = closeRect.width() * 0.25f;

    QPointF p1(closeRect.left() + pad, closeRect.top() + pad);
    QPointF p2(closeRect.right() - pad, closeRect.bottom() - pad);

    QPointF p3(closeRect.right() - pad, closeRect.top() + pad);
    QPointF p4(closeRect.left() + pad, closeRect.bottom() - pad);

    p.drawLine(p1, p2);
    p.drawLine(p3, p4);
}

void MaterialTabBar::enterEvent(QEnterEvent* event) {
    QTabBar::enterEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onHoverEnter();
    }
    update();
}

void MaterialTabBar::leaveEvent(QEvent* event) {
    QTabBar::leaveEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onHoverLeave();
    }
    m_hoveredIndex = -1;
    m_closeButtonHoveredIndex = -1;
    update();
}

void MaterialTabBar::mouseMoveEvent(QMouseEvent* event) {
    int index = tabAt(event->pos());
    if (index != m_hoveredIndex) {
        m_hoveredIndex = index;
        update();
    }

    // Track close button hover
    updateCloseButtonHover(event->pos());

    QTabBar::mouseMoveEvent(event);
}

void MaterialTabBar::mousePressEvent(QMouseEvent* event) {
    int index = tabAt(event->pos());
    if (index >= 0) {
        m_pressedIndex = index;
        if (m_stateMachine) {
            m_stateMachine->onPress(event->pos());
        }
    }
    QTabBar::mousePressEvent(event);
    update();
}

void MaterialTabBar::mouseReleaseEvent(QMouseEvent* event) {
    m_pressedIndex = -1;
    if (m_stateMachine) {
        m_stateMachine->onRelease();
    }

    // Check if close button was clicked
    int index = tabAt(event->pos());
    if (index >= 0 && isOverCloseButton(index, event->pos())) {
        emit tabCloseRequested(index);
        update();
        return; // Don't process as tab click
    }

    QTabBar::mouseReleaseEvent(event);
    update();
}

void MaterialTabBar::focusInEvent(QFocusEvent* event) {
    QTabBar::focusInEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusIn();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusIn();
    }
    update();
}

void MaterialTabBar::focusOutEvent(QFocusEvent* event) {
    QTabBar::focusOutEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusOut();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusOut();
    }
    update();
}

void MaterialTabBar::changeEvent(QEvent* event) {
    QTabBar::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        if (m_stateMachine) {
            isEnabled() ? m_stateMachine->onEnable() : m_stateMachine->onDisable();
        }
        update();
    }
}

CFColor MaterialTabBar::backgroundColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurface();
    }
    try {
        const auto& theme = app->currentTheme();
        return CFColor(theme.color_scheme().queryColor(SURFACE));
    } catch (...) {
        return fallbackSurface();
    }
}

CFColor MaterialTabBar::tabTextColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurfaceVariant();
    }
    try {
        const auto& theme = app->currentTheme();
        return CFColor(theme.color_scheme().queryColor(ON_SURFACE_VARIANT));
    } catch (...) {
        return fallbackOnSurfaceVariant();
    }
}

CFColor MaterialTabBar::selectedTabTextColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackPrimary();
    }
    try {
        const auto& theme = app->currentTheme();
        return CFColor(theme.color_scheme().queryColor(PRIMARY));
    } catch (...) {
        return fallbackPrimary();
    }
}

CFColor MaterialTabBar::indicatorColor() const {
    return selectedTabTextColor();
}

CFColor MaterialTabBar::stateLayerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurfaceVariant();
    }
    try {
        const auto& theme = app->currentTheme();
        return CFColor(theme.color_scheme().queryColor(ON_SURFACE));
    } catch (...) {
        return fallbackOnSurfaceVariant();
    }
}

CFColor MaterialTabBar::focusIndicatorColor() const {
    return selectedTabTextColor();
}

QFont MaterialTabBar::tabFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QTabBar::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }
    try {
        const auto& theme = app->currentTheme();
        return theme.font_type().queryTargetFont("labelLarge");
    } catch (...) {
        QFont font = QTabBar::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }
}

} // namespace cf::ui::widget::material
