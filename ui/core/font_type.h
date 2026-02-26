#pragma once
#include "export.h"
#include <QFont>

namespace cf::ui::core {
struct CF_UI_EXPORT IFontType {
    virtual ~IFontType() = default;
    virtual QFont queryTargetFont(const char* name) = 0;
};
} // namespace cf::ui::core
