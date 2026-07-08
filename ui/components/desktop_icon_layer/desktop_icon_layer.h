/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_icon_layer.h
 * @brief   User-managed desktop shortcut grid on the wallpaper.
 *
 * DesktopIconLayer renders a user-managed set of DesktopShortcut values (each
 * an app_id placed at a grid cell) as LauncherTile widgets on the desktop
 * background, above the wallpaper and below the status/task bars. It is a plain
 * child widget of the desktop surface (not an IShellLayer, not an IPanel) and
 * consumes PanelManager's central availableGeometry() like the launcher popup.
 *
 * Tiles are long-press-draggable (target: i.MX6ULL resistive touch). A drag
 * shows a floating ghost (grabbed pixmap) over a dimmed source tile; dropping
 * on a grid cell swaps positions, dropping outside the layer removes the
 * shortcut. Every layout change emits shortcutsChanged() so the caller can
 * persist via DesktopShortcutStore.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "app_entry.h"
#include "desktop_shortcut.h"

#include <QList>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QWidget>

class QGridLayout;
class QLabel;

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
    int columns{0}; ///< Tile columns that fit the available width (>=1 when valid).
    int rows{0};    ///< Tile rows that fit the available height.
    int shown{0};   ///< Capacity (columns*rows) capped to the app count.
    int cell{0};    ///< Tile edge length chosen for this area (px), clamped to [48,96].
};

/**
 * @brief   Computes how many tiles fit the available area.
 *
 * Extracted as a free function so the grid math is unit-testable without a
 * QWidget. Layout constants live in the .cpp.
 *
 * @param[in] available   The central desktop area available for icons.
 * @param[in] app_count   Number of applications that want a tile.
 *
 * @return  GridDimensions; columns/rows for the chosen tile edge, shown = capacity
 *          capped to the app count. The cell field is the largest tile edge in
 *          [48,96] whose columns*rows still fits app_count (96 when nothing needs
 *          to shrink, 48 when even that truncates); 0 for invalid input or when
 *          the area is too short to fit even one row.
 *
 * @throws  None.
 * @since   0.20
 * @ingroup components
 */
GridDimensions computeGridDimensions(const QSize& available, int app_count);

/**
 * @brief  Grid of user-placed application shortcut tiles on the wallpaper.
 *
 * Parents LauncherTile instances in a QGridLayout, one per DesktopShortcut at
 * the shortcut's (col, row). Tiles use TileContext::Desktop so a long-press
 * enters drag mode (rearrange / remove); clicks forward as appClicked().
 *
 * @ingroup components
 */
class DesktopIconLayer final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the icon layer.
     *
     * Sets up the transparent container with an empty grid. No tiles are
     * created until setShortcuts() + a valid geometry arrive.
     *
     * @param[in] parent  Owning widget (the desktop surface).
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    explicit DesktopIconLayer(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the layer.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    ~DesktopIconLayer() override;

    /**
     * @brief   Loads the shortcut layout and app registry into the grid.
     *
     * @param[in] shortcuts  User-placed shortcuts (app_id + cell).
     * @param[in] registry   Full app list for app_id -> AppEntry resolution.
     *
     * @throws  None.
     * @note    Rebuilds tiles; shortcuts whose app_id is not in the registry
     *          or whose cell is outside the grid are skipped with a warning.
     * @since   0.20
     * @ingroup components
     */
    void setShortcuts(const QList<DesktopShortcut>& shortcuts, const QList<AppEntry>& registry);

    /**
     * @brief   Adds one shortcut at the first free cell (launcher "send here").
     *
     * @param[in] app_id  Application to pin. No-op if already on the desktop or
     *                    not in the registry.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void addShortcut(const QString& app_id);

    /**
     * @brief   Updates this layer's geometry from the desktop's central area.
     *
     * Wire to PanelManager::availableGeometryChanged. Invalid/empty rects are
     * ignored (tiles would flash at 0,0 before relayout).
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
     * @brief  Emitted when a desktop tile is tapped (launched).
     *
     * @param[in] app_id  The clicked application identifier.
     *
     * @since 0.20
     * @ingroup components
     */
    void appClicked(const QString& app_id);

    /**
     * @brief  Emitted whenever the shortcut layout changes (add / remove / swap).
     *
     * Connect to DesktopShortcutStore::save to persist.
     *
     * @param[in] shortcuts  The new shortcut layout.
     *
     * @since 0.20
     * @ingroup components
     */
    void shortcutsChanged(const QList<DesktopShortcut>& shortcuts);

  protected:
    /**
     * @brief   Paints only the drag snap-highlight (transparent otherwise).
     *
     * @param[in] event  The paint event descriptor (unused).
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// @brief Clears existing tiles and lays them out by shortcut cell.
    void rebuildGrid();
    /// @brief Creates the floating ghost when a tile's long-press fires.
    void onTileLongPressed(const QString& app_id);
    /// @brief Moves the ghost and updates the snap target cell.
    void onTileDragMoved(const QPoint& global_pos);
    /// @brief Commits the drop: swap, remove, persist, rebuild.
    void onTileDragEnded(const QPoint& global_pos);

    LauncherTile* tileForApp(const QString& app_id) const;
    const AppEntry* findEntry(const QString& app_id) const;
    QPoint cellAt(const QPoint& layer_pos) const; ///< (col,row) or (-1,-1) if out.
    QPoint firstFreeCell() const;

    QGridLayout* grid_{nullptr};       ///< Tile grid. Ownership: this widget.
    QList<LauncherTile*> tiles_;       ///< Current tiles. Ownership: Qt parented.
    QList<DesktopShortcut> shortcuts_; ///< User-managed layout.
    QList<AppEntry> registry_;         ///< App registry for app_id resolution.
    QRect last_available_;             ///< Last valid central geometry.

    QString dragging_app_id_;         ///< app_id being dragged ("" = none).
    QPoint drag_origin_cell_{-1, -1}; ///< Cell the drag started from.
    QPoint drag_target_cell_{-1, -1}; ///< Current snap target (or (-1,-1)).
    QLabel* drag_ghost_{nullptr};     ///< Floating pixmap overlay. Ownership: this.
};

} // namespace cf::desktop::desktop_component
