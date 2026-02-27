/**
 * @file ToastWidget.cpp
 * @brief Toast notification widget implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "ToastWidget.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

namespace cf::ui::gallery {

ToastWidget::ToastWidget(const QString& message, QWidget* parent)
    : QWidget(parent), message_(message) {
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ToastWidget::show(int durationMs) {
    QFont font("Segoe UI", 10);
    QFontMetrics fm(font);
    int padding = 20;
    int textWidth = fm.horizontalAdvance(message_);
    int width = textWidth + padding * 2;
    int height = fm.height() + padding * 2;

    setFixedSize(width, height);

    if (parentWidget()) {
        QPoint pos = parentWidget()->mapToGlobal(parentWidget()->rect().bottomLeft());
        int x = pos.x() + (parentWidget()->width() - width) / 2;
        int y = pos.y() - height - 20;
        move(x, y);
    }

    QWidget::show();

    QTimer::singleShot(durationMs, this, [this]() {
        QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            hide();
            deleteLater();
        });
        anim->start(QPropertyAnimation::DeleteWhenStopped);
    });
}

void ToastWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect();

    // Background with rounded corners
    QPainterPath bgPath;
    bgPath.addRoundedRect(r, 8, 8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(60, 60, 60, 230));
    painter.drawPath(bgPath);

    // Text
    QFont font("Segoe UI", 10);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(r, Qt::AlignCenter, message_);
}

} // namespace cf::ui::gallery
