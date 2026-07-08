/**
 * @file    disk_card.cpp
 * @brief   Implementation of the DiskCard.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "disk_card.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QTimer>

#include <filesystem>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
inline constexpr double kCornerRadius = 20.0;
inline constexpr int kBarHeight = 16;
inline constexpr int kMargin = 24;
inline constexpr double kBytesPerGB = 1024.0 * 1024.0 * 1024.0;
} // namespace

DiskCard::DiskCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    refresh();
    applyTheme();
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this]() { applyTheme(); });

    timer_ = new QTimer(this);
    timer_->setTimerType(Qt::CoarseTimer);
    connect(timer_, &QTimer::timeout, this, [this]() { refresh(); });
    timer_->start(10000);
}

void DiskCard::refresh() {
    std::error_code ec;
    const auto space = std::filesystem::space("/", ec);
    if (ec) {
        update();
        return;
    }
    const quint64 total = space.capacity;
    const quint64 avail = space.available;
    const quint64 used = (total > avail) ? total - avail : 0;
    pct_ = (total > 0) ? static_cast<int>((used * 100U) / total) : 0;
    detail_text_ = QStringLiteral("%1 / %2 GB")
                       .arg(static_cast<double>(used) / kBytesPerGB, 0, 'f', 0)
                       .arg(static_cast<double>(total) / kBytesPerGB, 0, 'f', 0);
    update();
}

void DiskCard::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        track_color_ = cs.queryColor(SURFACE_VARIANT);
        fill_color_ = cs.queryColor(PRIMARY);
        label_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        value_color_ = cs.queryColor(ON_SURFACE);
        detail_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        label_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_LABEL_MEDIUM);
        value_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_DISPLAY_SMALL);
        detail_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_BODY_SMALL);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
        track_color_ = QColor(0xE7, 0xE0, 0xEC);
        fill_color_ = QColor(0x6B, 0x53, 0x95);
        label_color_ = QColor(0x49, 0x45, 0x4E);
        value_color_ = QColor(0x1C, 0x1B, 0x1F);
        detail_color_ = QColor(0x49, 0x45, 0x4E);
        label_font_.setPointSize(12);
        value_font_.setPointSize(28);
        value_font_.setBold(true);
        detail_font_.setPointSize(11);
    }
    update();
}

void DiskCard::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(surface, surface_color_);

    p.setPen(label_color_);
    p.setFont(label_font_);
    p.drawText(QRect(kMargin, kMargin, width() - 2 * kMargin, 24), Qt::AlignLeft | Qt::AlignVCenter,
               QStringLiteral("Disk"));
    p.setPen(value_color_);
    p.setFont(value_font_);
    p.drawText(QRect(kMargin, kMargin, width() - 2 * kMargin, 24),
               Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("%1%").arg(pct_));

    const int bar_y = kMargin + 36;
    const int bar_w = width() - 2 * kMargin;
    const QRectF bar_rect(kMargin, bar_y, bar_w, kBarHeight);
    p.setPen(Qt::NoPen);
    p.setBrush(track_color_);
    p.drawRoundedRect(bar_rect, kBarHeight / 2.0, kBarHeight / 2.0);
    if (pct_ > 0) {
        const QRectF fill_rect(kMargin, bar_y, bar_w * pct_ / 100.0, kBarHeight);
        p.setBrush(fill_color_);
        p.drawRoundedRect(fill_rect, kBarHeight / 2.0, kBarHeight / 2.0);
    }

    p.setPen(detail_color_);
    p.setFont(detail_font_);
    p.drawText(QRect(kMargin, bar_y + kBarHeight + 8, width() - 2 * kMargin, 20),
               Qt::AlignLeft | Qt::AlignVCenter, detail_text_);
}

} // namespace cf::desktop::desktop_component
