/**
 * @file    date_card.cpp
 * @brief   Implementation of the DateCard.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "date_card.h"

#include <QDate>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Blue gradient QSS (ported from CCIMX DateShowCard).
inline constexpr const char* kCardQss = R"(
    #DateCardWidget {
        border-radius: 20px;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #4A90E2, stop:1 #1E3C72);
    }
    #DateCardWidget QLabel { color: white; }
    #title_label { font-size: 32px; }
    #day_label { font-size: 48px; font-weight: bold; }
    #full_text_label { font-size: 16px; }
)";
} // namespace

DateCard::DateCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground); // required for QSS background on a QWidget
    setObjectName("DateCardWidget");
    setStyleSheet(kCardQss);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(6);

    title_label_ = new QLabel("DATE", this);
    title_label_->setObjectName("title_label");
    day_label_ = new QLabel(this);
    day_label_->setObjectName("day_label");
    full_text_label_ = new QLabel(this);
    full_text_label_->setObjectName("full_text_label");

    layout->addWidget(title_label_);
    layout->addWidget(day_label_);
    layout->addWidget(full_text_label_);
    layout->addStretch();

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(16);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 160));
    setGraphicsEffect(shadow);

    refresh();
}

void DateCard::refresh() {
    const QDate today = QDate::currentDate();
    day_label_->setText(today.toString("dd"));
    full_text_label_->setText(today.toString("yyyy MMM dd ddd"));
}

} // namespace cf::desktop::desktop_component
