#pragma once

namespace cf::ui::core {
struct IRadiusScale {
    virtual ~IRadiusScale() = default;
    virtual float queryRadiusScale(const char* name) = 0;
};
} // namespace cf::ui::core
