/**
 * @file    animation.cpp
 * @brief   Implementation file for ICFAbstractAnimation to enable MOC processing
 *
 * This file exists to trigger Qt's Meta-Object Compiler (MOC) for the
 * ICFAbstractAnimation class which uses Q_OBJECT.
 */

#include "animation.h"

namespace cf::ui::components {

ICFAbstractAnimation::ICFAbstractAnimation(QObject* parent) : QObject(parent) {
    driven_internal_timer = new QTimer(this);
}

} // namespace cf::ui::components
