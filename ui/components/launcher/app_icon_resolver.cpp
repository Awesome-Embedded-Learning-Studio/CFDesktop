/**
 * @file    desktop/ui/components/launcher/app_icon_resolver.cpp
 * @brief   Implementation of resolve_app_icon.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "app_icon_resolver.h"

#include <QIcon>
#include <QLatin1Char>
#include <QPixmap>
#include <Qt>

namespace cf::desktop::desktop_component {

QPixmap resolve_app_icon(const AppEntry& entry, QSize size) {
    const QString& path = entry.icon_path;
    if (path.isEmpty()) {
        return {};
    }

    // 1. Absolute filesystem path or Qt resource (manifest apps, builtin masks).
    //    A path-like value that fails to load is unrecoverable — do not fall
    //    through to the theme lookup, which would only mismatch on a path
    //    string and waste a lookup.
    if (path.startsWith(QLatin1Char('/')) || path.startsWith(QLatin1Char(':'))) {
        QPixmap pm(path);
        if (!pm.isNull()) {
            return pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        return {};
    }

    // 2. Otherwise treat icon_path as a freedesktop icon-theme name (the
    //    .desktop Icon= case). Returns a null icon when no theme is installed
    //    or the name is unknown — caller falls back to the initial letter.
    const QIcon theme_icon = QIcon::fromTheme(path);
    if (!theme_icon.isNull()) {
        QPixmap pm = theme_icon.pixmap(size);
        if (!pm.isNull()) {
            return pm;
        }
    }

    return {};
}

} // namespace cf::desktop::desktop_component
