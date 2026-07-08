/**
 * @file    home_card_manager.cpp
 * @brief   Implementation of the HomeCardManager.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "home_card_manager.h"

#include <QStackedWidget>
#include <QWidget>

namespace cf::desktop::desktop_component {

HomeCardManager::HomeCardManager(QStackedWidget* stack, QObject* parent)
    : QObject(parent), stack_(stack) {}

void HomeCardManager::installCard(QWidget* card) {
    if (stack_ == nullptr || card == nullptr) {
        return;
    }
    card->setParent(stack_);
    stack_->addWidget(card);
}

void HomeCardManager::removeCard(QWidget* card) {
    if (stack_ == nullptr || card == nullptr) {
        return;
    }
    stack_->removeWidget(card);
}

int HomeCardManager::cardCount() const {
    return stack_ != nullptr ? stack_->count() : 0;
}

} // namespace cf::desktop::desktop_component
