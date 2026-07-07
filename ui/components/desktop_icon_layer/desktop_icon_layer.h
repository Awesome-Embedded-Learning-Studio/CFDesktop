/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_icon_layer.h
 * @brief   Desktop shortcut icon grid sitting on the wallpaper.
 *
 * DesktopIconLayer renders the merged application list (builtin panels,
 * discovered manifests, and XDG .desktop entries) as a grid of LauncherTile
 * shortcuts directly on the desktop background, above the wallpaper and below
 * the status/task bars. It is a plain child widget of the desktop surface
 * (not an IShellLayer, not an IPanel): it consumes PanelManager's central
 * availableGeometry() like the launcher popup does, and forwards tile clicks
 * as appClicked(app_id) so the same launch_app path drives desktop, taskbar,
 * and launcher entries.
 *
 * The container paints nothing so the wallpaper shows through. It does NOT set
 * WA_TransparentForMouseEvents (that starves child widgets of mouse events on
 * several Qt versions); tiles receive their own clicks, and empty-cell clicks
 * propagate to the desktop surface.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "app_entry.h"

#include <QList>
#include <QRect>
#include <QSize>
#include <QString>
#include <QWidget>

class QGridLayout;

namespace cf::desktop::desktop_component {

class LauncherTile;

/**
 * @brief  Result of computing how many tiles fit a given desktop area.
 *
 * Pure value type returned by computeGridDimensions(); exposed in the header
 * so the layout math is unit-testable without instantiating a QWidget.
 *
 * @ingroup components
 */
struct GridDimensions {
    int columns{0}; ///< Tile columns that fit the available width (1..kMaxColumns).
    int rows{0};    ///< Tile rows that fit the available height.
    int shown{0};   ///< Entries actually displayed after cap-and-truncate.
};

/**
 * @brief   Computes how many tiles fit the available area and how many to show.
 *
 * The desktop background never scrolls, so when the merged app list is larger
 * than what fits on screen the visible count is capped to columns*rows and the
 * caller logs the truncation. The math is extracted here as a free function so
 * it can be exercised by unit tests without a QWidget.
 *
 * @param[in] available   The central desktop area available for icons.
 * @param[in] app_count   Number of applications that want a tile.
 *
 * @return  GridDimensions with columns/rows clamped to layout constants and
 *          shown capped to the on-screen capacity. A zero result is returned
 *          for invalid geometry or non-positive count.
 *
 * @throws  None.
 * @note    Layout constants (cell size, spacing, margins, max columns) are
 *          defined in the .cpp; this function is the single source of truth
 *          for grid sizing.
 * @since   0.20
 * @ingroup components
 */
GridDimensions computeGridDimensions(const QSize& available, int app_count);

/**
 * @brief  Grid of application shortcut tiles on the desktop background.
 *
 * Parents LauncherTile instances in a QGridLayout aligned to the top-left of
 * the central desktop area. Reuses LauncherTile unchanged (it already emits
 * clicked(app_id)); this layer owns the grid container, geometry, and the
 * passthrough behavior for empty cells.
 *
 * @ingroup components
 */
class DesktopIconLayer final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the icon layer.
     *
     * Sets up the transparent, mouse-passthrough container with an empty
     * grid layout. No tiles are created until setApps() + a valid geometry
     * arrive.
     *
     * @param[in] parent  Owning widget (the desktop surface).
     *
     * @throws  None.
     * @note    Must be constructed after the wallpaper shell layer and before
     *          the status/task bars so creation order stacks it between them.
     * @since   0.20
     * @ingroup components
     */
    explicit DesktopIconLayer(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the layer.
     *
     * Qt parent ownership releases the tiles.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    ~DesktopIconLayer() override;

    /**
     * @brief   Replaces the application list backing the grid.
     *
     * Same contract as CenteredTaskbar::setApps / AppLauncher::setApps: stores
     * the merged list and rebuilds the tiles. Tiles are not created until a
     * valid geometry has been received via onAvailableGeometryChanged().
     *
     * @param[in] apps  The applications to show as desktop shortcuts.
     *
     * @throws  None.
     * @note    Replaces any previously shown tiles.
     * @since   0.20
     * @ingroup components
     */
    void setApps(const QList<AppEntry>& apps);

    /**
     * @brief   Updates this layer's geometry from the desktop's central area.
     *
     * Wire to PanelManager::availableGeometryChanged. Invalid or empty rects
     * are ignored (tiles would otherwise flash at 0,0 before relayout). The
     * grid is rebuilt because column count may change with width.
     *
     * @param[in] available  The central rectangle between the bars.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void onAvailableGeometryChanged(const QRect& available);

  signals:
    /**
     * @brief  Emitted when a desktop tile is clicked.
     *
     * Connect to the same launch_app handler used by the taskbar and launcher
     * so all three entry points share one dispatch path.
     *
     * @param[in] app_id  The clicked application identifier.
     *
     * @since 0.20
     * @ingroup components
     */
    void appClicked(const QString& app_id);

  protected:
    /**
     * @brief   Paints nothing.
     *
     * The container is a transparent passthrough; LauncherTile children paint
     * themselves.
     *
     * @param[in] event  The paint event descriptor (unused).
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// @brief Clears existing tiles and lays them out for the current geometry.
    void rebuildGrid();

    QGridLayout* grid_{nullptr}; ///< Tile grid. Ownership: this widget.
    QList<LauncherTile*> tiles_; ///< Current tiles. Ownership: Qt parented.
    QList<AppEntry> apps_;       ///< Backing application list.
    QRect last_available_;       ///< Last valid central geometry received.
};

} // namespace cf::desktop::desktop_component
