/**
 * @file    notification_card_widget.cpp
 * @brief   Implementation of NotificationCardWidget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#include "notification_card_widget.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kCardWidth = 320; ///< Fixed card width (px).
constexpr int kCardHeight = 76; ///< Fixed card height (px).
constexpr int kPadX = 14;       ///< Horizontal inner padding (px).
constexpr int kPadY = 12;       ///< Vertical inner padding (px).
constexpr qreal kRadius = 12.0; ///< Corner radius (px).
constexpr int kCloseSize = 22;  ///< Dismiss hotspot edge (px).
} // namespace

NotificationCardWidget::NotificationCardWidget(QWidget* parent) : QWidget(parent) {
    setAutoFillBackground(false);
    setFixedSize(kCardWidth, kCardHeight);
    // Dismiss hotspot sits in the top-right corner.
    close_rect_ = QRect(width() - kCloseSize - kPadX / 2, kPadY / 2, kCloseSize, kCloseSize);
    // Follow live theme switches; ThemeManager is the canonical source.
    try {
        connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
                [this](const qw::core::ICFTheme&) { applyTheme(); });
    } catch (...) {
        // No theme registered yet; applyTheme() falls back below.
    }
    applyTheme();
}

void NotificationCardWidget::setNotification(const Notification& notification) {
    notif_ = notification;
    update();
}

Notification NotificationCardWidget::notification() const {
    return notif_;
}

QSize NotificationCardWidget::sizeHint() const {
    return {kCardWidth, kCardHeight};
}

void NotificationCardWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Rounded card surface.
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kRadius, kRadius);
    p.fillPath(surface, surface_color_);

    // Dismiss "x" hotspot, drawn at the top-right corner.
    p.setPen(QPen(close_color_, 1.6));
    const int m = 6;
    p.drawLine(close_rect_.left() + m, close_rect_.top() + m, close_rect_.right() - m,
               close_rect_.bottom() - m);
    p.drawLine(close_rect_.right() - m, close_rect_.top() + m, close_rect_.left() + m,
               close_rect_.bottom() - m);

    // Text columns leave room for the dismiss hotspot on the right.
    const int text_right = close_rect_.left() - kPadX / 2;

    QFont title_font = font();
    title_font.setBold(true);
    title_font.setPixelSize(15);
    p.setFont(title_font);
    p.setPen(title_color_);
    p.drawText(QRect(kPadX, kPadY, text_right - kPadX, 22), Qt::AlignVCenter | Qt::AlignLeft,
               notif_.title.isEmpty() ? QStringLiteral("Notification") : notif_.title);

    QFont body_font = font();
    body_font.setPixelSize(13);
    p.setFont(body_font);
    p.setPen(message_color_);
    p.drawText(QRect(kPadX, kPadY + 24, text_right - kPadX, 28),
               Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, notif_.message);

    // App id + time meta line.
    QFont meta_font = font();
    meta_font.setPixelSize(11);
    p.setFont(meta_font);
    p.setPen(meta_color_);
    const QString time_str = QDateTime::fromMSecsSinceEpoch(notif_.timestamp).toString("HH:mm");
    const QString meta = notif_.app_id.isEmpty() ? time_str : (notif_.app_id + "  " + time_str);
    p.drawText(QRect(kPadX, height() - kPadY - 14, text_right - kPadX, 14),
               Qt::AlignVCenter | Qt::AlignLeft, meta);
}

void NotificationCardWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && close_rect_.contains(event->pos())) {
        pressed_on_close_ = true;
        return;
    }
    QWidget::mousePressEvent(event);
}

void NotificationCardWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (pressed_on_close_ && event->button() == Qt::LeftButton &&
        close_rect_.contains(event->pos())) {
        pressed_on_close_ = false;
        emit dismissRequested(notif_.id);
        return;
    }
    pressed_on_close_ = false;
    QWidget::mouseReleaseEvent(event);
}

void NotificationCardWidget::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE_VARIANT);
        title_color_ = cs.queryColor(ON_SURFACE);
        message_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        meta_color_ = cs.queryColor(OUTLINE);
        close_color_ = cs.queryColor(ON_SURFACE_VARIANT);
    } catch (...) {
        // Fallback palette (mirrors MD3 light) when no theme is registered.
        surface_color_ = QColor(0xF3, 0xED, 0xF7);
        title_color_ = QColor(0x1C, 0x1B, 0x1F);
        message_color_ = QColor(0x49, 0x45, 0x4E);
        meta_color_ = QColor(0x79, 0x74, 0x7E);
        close_color_ = QColor(0x49, 0x45, 0x4E);
    }
    update();
}

} // namespace cf::desktop::desktop_component
