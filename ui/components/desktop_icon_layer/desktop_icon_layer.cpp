/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_icon_layer.cpp
 * @brief   Implementation of DesktopIconLayer.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "desktop_icon_layer.h"

#include "cflog.h"
#include "launcher/launcher_tile.h"

#include <QGridLayout>
#include <QPaintEvent>

#include <algorithm>

namespace cf::desktop::desktop_component {

namespace {
/// Tag for DesktopIconLayer log lines.
constexpr const char* kLogTag = "DesktopIconLayer";
/// Tile widget edge length (px). Mirrors LauncherTile::kCellSize in
/// launcher_tile.cpp; if that changes, update here too.
constexpr int kCellSize = 96;
/// Gap between adjacent tiles (px).
constexpr int kGridSpacing = 16;
/// Inner padding between the central area edge and the tile grid (px).
constexpr int kMargin = 24;
/// Upper bound on columns so very wide screens do not stretch the grid.
constexpr int kMaxColumns = 8;
} // namespace

GridDimensions computeGridDimensions(const QSize& available, int app_count) {
    GridDimensions result;
    // QSize(0,0) is technically isValid() (width/height >= 0) but is an empty
    // area — treat both invalid and empty as "no grid", else the floor-to-one
    // column logic below would return 1 column for a zero-size desktop.
    if (!available.isValid() || available.isEmpty() || app_count <= 0) {
        return result;
    }

    const int stride = kCellSize + kGridSpacing;
    // +kGridSpacing: the last column/row does not need a trailing gap.
    const int usable_w = available.width() - 2 * kMargin + kGridSpacing;
    const int usable_h = available.height() - 2 * kMargin + kGridSpacing;

    int cols = (usable_w > 0) ? (usable_w / stride) : 0;
    if (cols < 1) {
        cols = 1; // Even a very narrow desktop shows at least one column.
    }
    if (cols > kMaxColumns) {
        cols = kMaxColumns;
    }

    const int rows = (usable_h > 0) ? (usable_h / stride) : 0;
    const int capacity = cols * rows;
    const int shown = std::min(app_count, capacity);

    result.columns = cols;
    result.rows = rows;
    result.shown = shown;
    return result;
}

DesktopIconLayer::DesktopIconLayer(QWidget* parent) : QWidget(parent) {
    // The container must not paint a background (the wallpaper shows through)
    // and must not consume mouse events on empty cells (clicks between tiles
    // fall through to the wallpaper). Each LauncherTile clears the attribute
    // explicitly in rebuildGrid() so it still receives its own clicks.
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);

    grid_ = new QGridLayout(this);
    grid_->setSpacing(kGridSpacing);
    grid_->setContentsMargins(kMargin, kMargin, kMargin, kMargin);
    grid_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

DesktopIconLayer::~DesktopIconLayer() = default;

void DesktopIconLayer::setApps(const QList<AppEntry>& apps) {
    apps_ = apps;
    rebuildGrid();
}

void DesktopIconLayer::onAvailableGeometryChanged(const QRect& available) {
    if (!available.isValid() || available.isEmpty()) {
        return; // Avoid flashing tiles at (0,0) before the first relayout.
    }
    if (available == last_available_) {
        return; // No change: column count stays the same.
    }
    last_available_ = available;
    setGeometry(available);
    rebuildGrid();
}

void DesktopIconLayer::paintEvent(QPaintEvent* /*event*/) {
    // Transparent passthrough; LauncherTile children paint themselves.
}

void DesktopIconLayer::rebuildGrid() {
    qDeleteAll(tiles_);
    tiles_.clear();

    const auto dims = computeGridDimensions(last_available_.size(), static_cast<int>(apps_.size()));
    if (dims.shown <= 0) {
        return; // No valid geometry yet, or nothing to show.
    }

    int row = 0;
    int col = 0;
    for (int i = 0; i < dims.shown; ++i) {
        auto* tile = new LauncherTile(apps_[i], this);
        // Defensive: the container is transparent-for-mouse; ensure each tile
        // is not, so its hover/click still work under every Qt version.
        tile->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        connect(tile, &LauncherTile::clicked, this, &DesktopIconLayer::appClicked);
        grid_->addWidget(tile, row, col);
        tiles_.append(tile);
        ++col;
        if (col >= dims.columns) {
            col = 0;
            ++row;
        }
    }

    if (apps_.size() > dims.shown) {
        cf::log::warningftag(kLogTag, "{} apps but only {} fit on desktop ({}x{}); {} hidden",
                             apps_.size(), dims.shown, dims.columns, dims.rows,
                             apps_.size() - dims.shown);
    }
}

} // namespace cf::desktop::desktop_component
