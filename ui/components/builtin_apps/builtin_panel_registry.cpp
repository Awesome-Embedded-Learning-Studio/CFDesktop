/**
 * @file    builtin_panel_registry.cpp
 * @brief   Implementation of BuiltinPanelRegistry.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "builtin_panel_registry.h"

#include "cflog.h"

namespace cf::desktop::desktop_component {

namespace {
/// Tag for BuiltinPanelRegistry log lines.
constexpr const char* kLogTag = "BuiltinPanelRegistry";
} // namespace

BuiltinPanelRegistry& BuiltinPanelRegistry::instance() {
    static BuiltinPanelRegistry registry;
    return registry;
}

void BuiltinPanelRegistry::registerPanel(IBuiltinPanel* panel) {
    if (panel == nullptr) {
        cf::log::warningftag(kLogTag, "registerPanel ignored null panel");
        return;
    }
    const QString id = panel->appId();
    if (panels_.contains(id)) {
        cf::log::warningftag(kLogTag, "Duplicate builtin panel id '{}' overwritten",
                             id.toStdString());
    }
    panels_.insert(id, panel);
}

IBuiltinPanel* BuiltinPanelRegistry::find(const QString& id) const {
    const auto it = panels_.constFind(id);
    if (it == panels_.constEnd()) {
        cf::log::warningftag(kLogTag, "No builtin panel for id '{}'", id.toStdString());
        return nullptr;
    }
    return it.value();
}

bool BuiltinPanelRegistry::contains(const QString& id) const {
    return panels_.contains(id);
}

std::vector<IBuiltinPanel*> BuiltinPanelRegistry::all() const {
    std::vector<IBuiltinPanel*> result;
    result.reserve(static_cast<std::size_t>(panels_.size()));
    for (IBuiltinPanel* panel : panels_) {
        result.push_back(panel);
    }
    return result;
}

void BuiltinPanelRegistry::clear() {
    panels_.clear();
}

} // namespace cf::desktop::desktop_component
