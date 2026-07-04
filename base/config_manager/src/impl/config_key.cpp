#include "cfconfig_key.h"
#include <ranges>

namespace {
bool default_policy(const std::string_view s) {
    return !s.empty() &&
           std::ranges::empty(s | std::ranges::views::filter([](char c) {
                                  return !std::isalnum(static_cast<unsigned char>(c)) && c != '_' &&
                                         c != '.';
                              }));
}
} // namespace

namespace cf::config {

bool KeyHelper::fromKeyViewToKey(const KeyView& kv, Key& k) {
    if (!default_policy(kv.group) || !default_policy(kv.key)) {
        return false;
    }

    k.full_key = std::string(kv.group) + "." + std::string(kv.key);
    k.full_description = k.full_key;
    return true;
}

KeyView KeyHelper::fromKeyToKeyView(const Key& k) {
    auto pos = k.full_key.rfind('.');
    if (pos == std::string::npos) {
        return KeyView{.group = "", .key = k.full_key};
    }
    return KeyView{.group = std::string_view(k.full_key).substr(0, pos),
                   .key = std::string_view(k.full_key).substr(pos + 1)};
}

} // namespace cf::config
