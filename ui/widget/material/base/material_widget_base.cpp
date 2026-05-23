/**
 * @file    material_widget_base.cpp
 * @brief   Common Material Design widget behavior composition helper.
 *
 * @ingroup ui_widget_material_base
 */

#include "material_widget_base.h"

#include "application_support/application.h"

namespace cf::ui::widget::material::base {

MaterialWidgetBase::MaterialWidgetBase(QWidget* owner, const Config& config) : m_owner(owner) {
    auto factory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    m_stateMachine = new StateMachine(factory, owner);

    if (config.useRipple) {
        m_ripple = new RippleHelper(factory, owner);
        m_ripple->setMode(config.rippleMode);
        connect(m_ripple, &RippleHelper::repaintNeeded, owner,
                static_cast<void (QWidget::*)()>(&QWidget::update));
    }

    if (config.useElevation) {
        m_elevation = new MdElevationController(factory, owner);
        m_elevation->setElevation(config.initialElevation);
        connect(m_elevation, &MdElevationController::pressOffsetChanged, owner,
                static_cast<void (QWidget::*)()>(&QWidget::update));
    }

    if (config.useFocusIndicator) {
        m_focusIndicator = new MdFocusIndicator(factory, owner);
    }

    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, owner,
            static_cast<void (QWidget::*)()>(&QWidget::update));
}

void MaterialWidgetBase::onEnterEvent() {
    m_stateMachine->onHoverEnter();
    m_owner->update();
}

void MaterialWidgetBase::onLeaveEvent() {
    m_stateMachine->onHoverLeave();
    if (m_ripple)
        m_ripple->onCancel();
    m_owner->update();
}

void MaterialWidgetBase::onMousePress(const QPoint& pos, const QRectF& bounds) {
    m_stateMachine->onPress(pos);
    if (m_ripple)
        m_ripple->onPress(pos, bounds);
    if (m_elevation)
        m_elevation->setPressed(true);
    m_owner->update();
}

void MaterialWidgetBase::onMouseRelease() {
    m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    if (m_elevation)
        m_elevation->setPressed(false);
    m_owner->update();
}

void MaterialWidgetBase::onFocusIn() {
    m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    m_owner->update();
}

void MaterialWidgetBase::onFocusOut() {
    m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    m_owner->update();
}

void MaterialWidgetBase::onEnabledChange(bool enabled) {
    if (enabled) {
        m_stateMachine->onEnable();
    } else {
        m_stateMachine->onDisable();
    }
    m_owner->update();
}

} // namespace cf::ui::widget::material::base
