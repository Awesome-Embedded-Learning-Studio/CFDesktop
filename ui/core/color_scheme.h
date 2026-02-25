/**
 * @file color_scheme.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief color_scheme provides the color
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <QColor>
namespace cf::ui::core {
struct ICFColorScheme {
    virtual ~ICFColorScheme() = default;
    virtual QColor& queryExpectedColor(const char* name) = 0;
};
} // namespace cf::ui::core
