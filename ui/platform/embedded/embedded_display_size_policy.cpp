/**
 * @file    embedded_display_size_policy.cpp
 * @brief   Embedded framebuffer display size policy implementation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#include "embedded_display_size_policy.h"

#include <Qt>
#include <QWidget>

namespace cf::desktop::platform_strategy::embedded {

EmbeddedDisplaySizePolicy::EmbeddedDisplaySizePolicy() = default;

EmbeddedDisplaySizePolicy::~EmbeddedDisplaySizePolicy() = default;

const char* EmbeddedDisplaySizePolicy::name() const noexcept {
    return "Embedded linuxfb Size Policy";
}

bool EmbeddedDisplaySizePolicy::action(QWidget* widget_data) {
    if (widget_data == nullptr) {
        return false;
    }
    // Embedded targets render directly to the framebuffer; drop any window
    // decorations that a compositor would otherwise provide.
    widget_data->setWindowFlag(Qt::FramelessWindowHint, true);
    return true;
}

DesktopBehaviors EmbeddedDisplaySizePolicy::query() const {
    return DesktopBehaviorFlag::Fullscreen | DesktopBehaviorFlag::Frameless;
}

} // namespace cf::desktop::platform_strategy::embedded
