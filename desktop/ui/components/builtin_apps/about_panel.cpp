/**
 * @file    about_panel.cpp
 * @brief   In-process "About CFDesktop" panel implementation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "about_panel.h"

#include <QGuiApplication>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

#include <algorithm>

namespace cf::desktop::desktop_component {

namespace {
constexpr qreal kCornerRadius = 16.0; ///< Card corner radius (px).
constexpr qreal kWidthRatio = 0.42;   ///< Card width as a fraction of available.
constexpr qreal kHeightRatio = 0.36;  ///< Card height as a fraction of available.
constexpr int kMinWidth = 360;        ///< Minimum card width (px).
constexpr int kMaxWidth = 560;        ///< Maximum card width (px).
constexpr int kMinHeight = 220;       ///< Minimum card height (px).
constexpr int kMaxHeight = 340;       ///< Maximum card height (px).
} // namespace

AboutPanel::AboutPanel(QWidget* parent)
    : QWidget(parent), version_text_(QStringLiteral("CFDesktop v0.19.0")),
      target_text_(QStringLiteral("Running on i.MX6ULL  (Qt linuxfb)")) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
    // Stay hidden until popup(). As a child of the desktop, Qt would otherwise
    // auto-show this widget when the desktop becomes visible.
    hide();
}

AboutPanel::~AboutPanel() = default;

void AboutPanel::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }

    const int w = std::clamp(static_cast<int>(avail.width() * kWidthRatio), kMinWidth, kMaxWidth);
    const int h =
        std::clamp(static_cast<int>(avail.height() * kHeightRatio), kMinHeight, kMaxHeight);
    const int x = avail.center().x() - w / 2;
    const int y = avail.center().y() - h / 2;

    setGeometry(x, y, w, h);
    show();
    raise();
}

void AboutPanel::hidePanel() {
    hide();
}

QString AboutPanel::appId() const {
    return QStringLiteral("about");
}

QString AboutPanel::displayName() const {
    return QStringLiteral("About");
}

void AboutPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Hardcoded Material-ish palette; this demo panel does not track the theme.
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    const QColor text_color(0x1C, 0x1B, 0x1F);
    const QColor hint_color(0x49, 0x45, 0x4E);

    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(surface, surface_color);

    // Version (title), centered in the upper portion.
    QFont title_font = font();
    title_font.setPointSize(18);
    title_font.setBold(true);
    p.setFont(title_font);
    p.setPen(text_color);
    p.drawText(QRect(0, 0, width(), height() * 2 / 3), Qt::AlignCenter, version_text_);

    // Target line + dismiss hint, centered in the lower portion.
    QFont body_font = font();
    body_font.setPointSize(11);
    p.setFont(body_font);
    p.setPen(hint_color);
    p.drawText(QRect(0, height() / 2, width(), height() / 4), Qt::AlignCenter, target_text_);
    p.drawText(QRect(0, height() * 3 / 4, width(), height() / 6), Qt::AlignCenter,
               QStringLiteral("(tap to close)"));
}

void AboutPanel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        hidePanel();
    }
    QWidget::mouseReleaseEvent(event);
}

} // namespace cf::desktop::desktop_component
