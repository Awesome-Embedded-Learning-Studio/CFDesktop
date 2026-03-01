#include "theme_manager.h"

#include <QtCore/qtextstream.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qapplication.h>
#include <stdexcept>

namespace cf::ui::core {

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

const ICFTheme& ThemeManager::theme(const std::string& name) const {
    auto it = factories_.find(name);
    if (it == factories_.end()) {
        // Return a pointer indicator for "not found" - caller should check factories_ first
        // Or we can throw an exception
        throw std::runtime_error("Theme factory not found: " + name);
    }

    // Create or get cached theme instance
    auto cache_it = theme_cache_.find(name);
    if (cache_it != theme_cache_.end()) {
        return *cache_it->second;
    }

    // const_cast is safe here as we're just populating the cache
    auto& factory = it->second;
    auto theme_ptr = factory->fromName(name.c_str());
    if (!theme_ptr) {
        throw std::runtime_error("Failed to create theme: " + name);
    }

    auto& non_const = const_cast<ThemeManager*>(this)->theme_cache_[name];
    non_const = std::move(theme_ptr);
    return *non_const;
}

bool ThemeManager::insert_one(const std::string& name, InstallerMaker make_one) {
    if (factories_.find(name) != factories_.end()) {
        return false; // Already exists
    }
    auto factory = make_one();
    if (!factory) {
        return false;
    }
    factories_[name] = std::move(factory);
    return true;
}

void ThemeManager::remove_one(const std::string& name) {
    factories_.erase(name);
    theme_cache_.erase(name);
}

void ThemeManager::install_widget(QWidget* w) {
    if (!w)
        return;
    installed_widget.insert(w);
}

void ThemeManager::remove_widget(QWidget* w) {
    installed_widget.erase(w);
}

void ThemeManager::setThemeTo(const std::string& name, bool doBroadcast) {
    auto it = factories_.find(name);
    if (it == factories_.end()) {
        return; // Theme not found
    }

    // Get or create the theme
    auto cache_it = theme_cache_.find(name);
    if (cache_it == theme_cache_.end()) {
        auto theme_ptr = it->second->fromName(name.c_str());
        if (!theme_ptr) {
            return; // Failed to create theme
        }
        cache_it = theme_cache_.emplace(name, std::move(theme_ptr)).first;
    }

    current_theme_name_ = name;
    // Emit signal
    if (doBroadcast) {
        emit themeChanged(*cache_it->second);
    }
}

const std::string& ThemeManager::currentThemeName() const {
    return current_theme_name_;
}
} // namespace cf::ui::core
