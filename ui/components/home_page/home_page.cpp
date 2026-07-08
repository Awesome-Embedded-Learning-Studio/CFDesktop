/**
 * @file    home_page.cpp
 * @brief   Implementation of the HomePage screen.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "home_page.h"

#include "analog_clock_widget.h"
#include "card_stack_widget.h"
#include "cpu_card.h"
#include "date_card.h"
#include "digital_time_widget.h"
#include "disk_card.h"
#include "home_card_manager.h"
#include "memory_card.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Outer margin around the home page, in pixels.
inline constexpr int kOuterMargin = 24;
/// @brief Spacing between the left/right columns, in pixels.
inline constexpr int kColumnSpacing = 24;
} // namespace

HomePage::HomePage(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);

    auto* outer = new QHBoxLayout(this);
    outer->setContentsMargins(kOuterMargin, kOuterMargin, kOuterMargin, kOuterMargin);
    outer->setSpacing(kColumnSpacing);

    // Left column: analog clock (5) above digital time (2).
    auto* left = new QWidget(this);
    auto* left_layout = new QVBoxLayout(left);
    left_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->setSpacing(kColumnSpacing);
    analog_clock_ = new AnalogClockWidget(left);
    digital_time_ = new DigitalTimeWidget(left);
    left_layout->addWidget(analog_clock_, 5);
    left_layout->addWidget(digital_time_, 2);

    // Right column: card stack (3) above a placeholder app-grid area (1).
    auto* right = new QWidget(this);
    auto* right_layout = new QVBoxLayout(right);
    right_layout->setContentsMargins(0, 0, 0, 0);
    right_layout->setSpacing(kColumnSpacing);
    card_stack_ = new CardStackWidget(right);
    auto* placeholder_grid = new QWidget(right); // Step B: app card grid.
    placeholder_grid->setAttribute(Qt::WA_TranslucentBackground);
    right_layout->addWidget(card_stack_, 3);
    right_layout->addWidget(placeholder_grid, 1);

    outer->addWidget(left);
    outer->addWidget(right);
    outer->setStretchFactor(left, 1);
    outer->setStretchFactor(right, 1);

    card_manager_ = std::make_unique<HomeCardManager>(card_stack_);
    card_manager_->installCard(new DateCard(card_stack_));
    card_manager_->installCard(new MemoryCard(card_stack_));
    card_manager_->installCard(new CpuCard(card_stack_));
    card_manager_->installCard(new DiskCard(card_stack_));
}

HomeCardManager* HomePage::cardManager() const {
    return card_manager_.get();
}

} // namespace cf::desktop::desktop_component
