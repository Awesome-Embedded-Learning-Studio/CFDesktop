/**
 * @file    desktop/ui/components/launcher/app_icon_resolver.h
 * @brief   Resolves an AppEntry icon_path to a renderable pixmap.
 *
 * AppEntry::icon_path carries different value shapes depending on its source:
 * manifest apps use an absolute filesystem path, builtin masks use a Qt
 * resource ":/..." path, and freedesktop .desktop entries use an icon-theme
 * name (e.g. "firefox", "utilities-terminal"). resolve_app_icon() turns any
 * of those into a QPixmap with one call, returning a null pixmap when nothing
 * loads so callers fall back to drawing the application's initial letter.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "app_entry.h"

#include <QPixmap>
#include <QSize>

namespace cf::desktop::desktop_component {

/**
 * @brief   Resolves @p entry's icon to a pixmap sized for @p size.
 *
 * Resolution chain, in order:
 * 1. Absolute filesystem path or Qt resource (":/...") -> QPixmap{path}.
 * 2. Otherwise the value is treated as a freedesktop icon-theme name and
 *    resolved via QIcon::fromTheme().
 * 3. A null pixmap is returned when nothing loads; the caller should draw the
 *    application's initial letter as the fallback.
 *
 * The returned pixmap is scaled (aspect ratio kept, smooth filtering) when
 * loaded from a path. Theme lookups return the theme's pixmap at @p size
 * directly (QIcon already picks the best-resolution variant).
 *
 * @param[in] entry  The application whose icon_path is resolved.
 * @param[in] size   Target icon edge length in device pixels.
 *
 * @return  The resolved pixmap, or a null pixmap when entry.icon_path is
 *          empty or no source loads.
 *
 * @throws  None.
 * @note    QIcon::fromTheme requires an installed icon theme (hicolor,
 *          adwaita, ...). On minimal targets without one, the null pixmap is
 *          returned and the caller's letter fallback applies — graceful
 *          degradation, not a hard dependency.
 * @since   0.20
 * @ingroup components
 */
QPixmap resolve_app_icon(const AppEntry& entry, QSize size);

} // namespace cf::desktop::desktop_component
