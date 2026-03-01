/**
 * @file ThemeSidebar.cpp
 * @brief Left sidebar with tab items for navigation - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "ThemeSidebar.h"
#include "ui/core/theme.h"
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::gallery {

ThemeSidebar::ThemeSidebar(QWidget* parent) : QWidget(parent) {
    setMinimumWidth(200);
    setMaximumWidth(280);

    // Initialize tabs with icons + text
    tabs_ = {{"🎨", "色彩方案", "Color Scheme"},
             {"⚡", "动效规范", "Motion Spec"},
             {"🔲", "圆角规范", "Radius Scale"},
             {"🔤", "字体规范", "Typography"}};

    // Default colors (light theme)
    backgroundColor_ = QColor(245, 245, 245);
    activeColor_ = QColor(103, 80, 164);
    hoverColor_ = QColor(230, 230, 235);
    textColor_ = QColor(60, 60, 60);
    iconColor_ = QColor(60, 60, 60);
    borderColor_ = QColor(220, 220, 220);

    setMouseTracking(true);
}

void ThemeSidebar::setActiveIndex(int index) {
    if (index >= 0 && index < tabs_.size() && index != activeIndex_) {
        activeIndex_ = index;
        update();
    }
}

void ThemeSidebar::applyTheme(const cf::ui::core::ICFTheme& theme) {
    auto& colorScheme = theme.color_scheme();

    // Get colors from theme
    backgroundColor_ = colorScheme.queryColor("md.surface");
    auto primaryColor = colorScheme.queryColor("md.primary");
    auto onSurfaceColor = colorScheme.queryColor("md.onSurface");
    auto surfaceVariantColor = colorScheme.queryColor("md.surfaceVariant");

    activeColor_ = primaryColor;
    hoverColor_ = surfaceVariantColor;
    textColor_ = onSurfaceColor;
    iconColor_ = onSurfaceColor;
    borderColor_ = colorScheme.queryColor("md.outlineVariant");

    update();
}

void ThemeSidebar::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect();

    // Background
    painter.fillRect(r, backgroundColor_);

    // Border line on right side
    painter.setPen(QPen(borderColor_, 1));
    painter.drawLine(r.right() - 1, r.top(), r.right() - 1, r.bottom());

    // Draw tabs
    for (int i = 0; i < tabs_.size(); ++i) {
        QRect tabRect = this->tabRect(i);
        bool isActive = (i == activeIndex_);
        bool isHovered = (i == hoverIndex_);

        // Tab background
        if (isActive) {
            // Active indicator on left edge
            QPainterPath indicatorPath;
            qreal indicatorWidth = 4;
            indicatorPath.addRoundedRect(
                QRectF(tabRect.left(), tabRect.top() + 8, indicatorWidth, tabRect.height() - 16), 2,
                2);
            painter.fillPath(indicatorPath, activeColor_);

            // Active background
            QPainterPath bgPath;
            bgPath.addRoundedRect(tabRect.adjusted(4, 4, -4, -4), 8, 8);
            painter.fillPath(bgPath, hoverColor_);
        } else if (isHovered) {
            QPainterPath bgPath;
            bgPath.addRoundedRect(tabRect.adjusted(4, 4, -4, -4), 8, 8);
            painter.fillPath(bgPath, hoverColor_);
        }

        // Icon and text
        const TabItem& tab = tabs_[i];

        // Icon
        QFont iconFont("Segoe UI Emoji", 20);
        painter.setFont(iconFont);
        painter.setPen(iconColor_);

        QRectF iconRect(tabRect.left() + 20, tabRect.center().y() - 12, 32, 32);
        painter.drawText(iconRect, Qt::AlignCenter, tab.icon);

        // Text label
        QFont textFont("Segoe UI", 11, isActive ? QFont::Bold : QFont::Normal);
        painter.setFont(textFont);
        painter.setPen(isActive ? activeColor_ : textColor_);

        QRectF textRect(iconRect.right() + 12, tabRect.top() + 10,
                        tabRect.width() - iconRect.width() - 40, tabRect.height() - 20);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, tab.label);
    }
}

void ThemeSidebar::mousePressEvent(QMouseEvent* event) {
    int index = tabAtPosition(event->pos());
    if (index >= 0) {
        setActiveIndex(index);
        emit tabClicked(index);
    }
}

void ThemeSidebar::enterEvent(QEnterEvent*) {
    // Track mouse for hover effects
}

void ThemeSidebar::leaveEvent(QEvent*) {
    if (hoverIndex_ >= 0) {
        hoverIndex_ = -1;
        update();
    }
}

void ThemeSidebar::mouseMoveEvent(QMouseEvent* event) {
    int index = tabAtPosition(event->pos());
    if (index != hoverIndex_) {
        hoverIndex_ = index;
        update();
    }
}

int ThemeSidebar::tabAtPosition(const QPoint& pos) const {
    for (int i = 0; i < tabs_.size(); ++i) {
        if (tabRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

QRect ThemeSidebar::tabRect(int index) const {
    if (index < 0 || index >= tabs_.size()) {
        return QRect();
    }

    int tabHeight = 60;
    int topMargin = 20;
    int spacing = 8;

    int y = topMargin + index * (tabHeight + spacing);

    return QRect(0, y, width(), tabHeight);
}

} // namespace cf::ui::gallery
