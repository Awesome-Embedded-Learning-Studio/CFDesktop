/**
 * @file    animation_factory_manager.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Factory Manager Interface Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implementation file for ICFAnimationManagerFactory interface.
 * This file is required for Qt's MOC (Meta-Object Compiler) to generate
 * the meta-object code (vtable, staticMetaObject, etc.) for interfaces
 * with Q_OBJECT macro.
 */

#include "animation_factory_manager.h"

namespace cf::ui::components {

ICFAnimationManagerFactory::ICFAnimationManagerFactory(QObject* parent) : QObject(parent) {}

} // namespace cf::ui::components
