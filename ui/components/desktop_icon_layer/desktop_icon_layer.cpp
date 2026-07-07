/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_icon_layer.cpp
 * @brief   Implementation of DesktopIconLayer.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#include "desktop_icon_layer.h"

#include "cflog.h"
#include "launcher/launcher_tile.h"

#include <QGridLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QTimer>

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

/// Index of the shortcut with @p app_id, or -1.
int indexOfApp(const QList<DesktopShortcut>& list, const QString& app_id) {
    for (int i = 0; i < list.size(); ++i) {
        if (list[i].app_id == app_id) {
            return i;
        }
    }
    return -1;
}

/// Index of the shortcut occupying @p cell (col in x, row in y), or -1.
int indexOfCell(const QList<DesktopShortcut>& list, const QPoint& cell) {
    for (int i = 0; i < list.size(); ++i) {
        if (list[i].col == cell.x() && list[i].row == cell.y()) {
            return i;
        }
    }
    return -1;
}
} // namespace

GridDimensions computeGridDimensions(const QSize& available, int app_count) {
    GridDimensions result;
    if (!available.isValid() || available.isEmpty() || app_count <= 0) {
        return result;
    }

    const int stride = kCellSize + kGridSpacing;
    const int usable_w = available.width() - 2 * kMargin + kGridSpacing;
    const int usable_h = available.height() - 2 * kMargin + kGridSpacing;

    int cols = (usable_w > 0) ? (usable_w / stride) : 0;
    if (cols < 1) {
        cols = 1;
    }
    // No artificial column cap: the grid fills the available width. (A prior
    // kMaxColumns=8 cap left wide displays half-empty horizontally.)

    const int rows = (usable_h > 0) ? (usable_h / stride) : 0;
    const int capacity = cols * rows;
    result.columns = cols;
    result.rows = rows;
    result.shown = std::min(app_count, capacity);
    return result;
}

DesktopIconLayer::DesktopIconLayer(QWidget* parent) : QWidget(parent) {
    // No background paint (wallpaper shows through). Deliberately NOT
    // WA_TransparentForMouseEvents: that starves child widgets of mouse events
    // under several Qt versions / on WSLg, so LauncherTile would not get the
    // long-press that starts a drag.
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);

    grid_ = new QGridLayout(this);
    grid_->setSpacing(kGridSpacing);
    grid_->setContentsMargins(kMargin, kMargin, kMargin, kMargin);
    grid_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

DesktopIconLayer::~DesktopIconLayer() = default;

void DesktopIconLayer::setShortcuts(const QList<DesktopShortcut>& shortcuts,
                                    const QList<AppEntry>& registry) {
    shortcuts_ = shortcuts;
    registry_ = registry;
    rebuildGrid();
}

void DesktopIconLayer::addShortcut(const QString& app_id) {
    if (app_id.isEmpty()) {
        return;
    }
    if (indexOfApp(shortcuts_, app_id) >= 0) {
        cf::log::infoftag(kLogTag, "app {} already on desktop; addShortcut no-op",
                          app_id.toStdString());
        return;
    }
    if (findEntry(app_id) == nullptr) {
        cf::log::warningftag(kLogTag, "addShortcut: app {} not in registry; skipped",
                             app_id.toStdString());
        return;
    }
    const QPoint cell = firstFreeCell();
    DesktopShortcut s;
    s.app_id = app_id;
    s.col = cell.x();
    s.row = cell.y();
    shortcuts_.append(s);
    emit shortcutsChanged(shortcuts_);
    // Defer rebuild: addShortcut runs inside the launcher tile's mouseRelease
    // handler, and rebuildGrid's qDeleteAll would delete a widget mid-event —
    // singleShot(0) runs once control returns to the event loop, where it's safe.
    QTimer::singleShot(0, this, [this]() { rebuildGrid(); });
}

void DesktopIconLayer::onAvailableGeometryChanged(const QRect& available) {
    if (!available.isValid() || available.isEmpty()) {
        return;
    }
    if (available == last_available_) {
        return;
    }
    last_available_ = available;
    setGeometry(available);
    rebuildGrid();
}

void DesktopIconLayer::paintEvent(QPaintEvent* /*event*/) {
    // Transparent by default; only draw the drag snap-highlight when active.
    if (dragging_app_id_.isEmpty() || drag_target_cell_.x() < 0) {
        return;
    }
    const int stride = kCellSize + kGridSpacing;
    const QRect cell_rect(kMargin + drag_target_cell_.x() * stride,
                          kMargin + drag_target_cell_.y() * stride, kCellSize, kCellSize);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(QColor(0x6F, 0x5B, 0xA4, 200)); // MD3-ish primary, semi-transparent.
    pen.setWidthF(2.5);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(cell_rect, 12, 12);
}

void DesktopIconLayer::rebuildGrid() {
    // removeWidget BEFORE delete: deleting a widget leaves a stale item in the
    // QGridLayout until the destroyed signal is processed (asynchronous). The
    // next addWidget then lands in a layout that still references dead widgets,
    // and every new tile ends up at pos (0,0) — the data layer is correct but
    // nothing is visually where shortcuts_ says. Removing synchronously keeps
    // the layout clean so the new addWidget lands in the right cell.
    for (auto* tile : tiles_) {
        grid_->removeWidget(tile);
        delete tile;
    }
    tiles_.clear();

    const auto dims =
        computeGridDimensions(last_available_.size(), static_cast<int>(shortcuts_.size()));
    if (dims.columns <= 0 || dims.rows <= 0) {
        return; // No valid geometry yet.
    }
    // Force EVERY row/column (including empty ones) to a full cell size. Without
    // this, QGridLayout collapses rows/cols that hold no widget to zero, so the
    // grid occupies only the rows that have tiles instead of cols*stride x
    // rows*stride. A tile at row 5 with rows 2-4 empty then renders visually at
    // row 2 — cellAt()'s (margin + row*stride) math no longer matches where the
    // widget actually is, and the tile looks like it "bounced back" or "flew".
    // Clear row/col minimums from a previous (possibly larger) grid first:
    // setRowMinimumHeight only updates the rows we name, so rows beyond the new
    // capacity would keep their old 96px minimum and the grid would stay
    // oversized after a window shrink, flinging tiles out of place.
    for (int r = 0; r < grid_->rowCount(); ++r) {
        grid_->setRowMinimumHeight(r, 0);
    }
    for (int c = 0; c < grid_->columnCount(); ++c) {
        grid_->setColumnMinimumWidth(c, 0);
    }
    for (int r = 0; r < dims.rows; ++r) {
        grid_->setRowMinimumHeight(r, kCellSize);
    }
    for (int c = 0; c < dims.columns; ++c) {
        grid_->setColumnMinimumWidth(c, kCellSize);
    }
    for (const auto& s : shortcuts_) {
        if (s.col < 0 || s.col >= dims.columns || s.row < 0 || s.row >= dims.rows) {
            cf::log::warningftag(kLogTag, "shortcut {} at ({},{}) outside grid ({}x{}); skipped",
                                 s.app_id.toStdString(), s.col, s.row, dims.columns, dims.rows);
            continue;
        }
        const AppEntry* entry = findEntry(s.app_id);
        if (entry == nullptr) {
            cf::log::warningftag(kLogTag, "shortcut {} resolves to no app entry; skipped",
                                 s.app_id.toStdString());
            continue;
        }
        auto* tile = new LauncherTile(*entry, this);
        tile->setContext(cf::desktop::desktop_component::TileContext::Desktop);
        connect(tile, &LauncherTile::clicked, this, &DesktopIconLayer::appClicked);
        connect(tile, &LauncherTile::longPressed, this, &DesktopIconLayer::onTileLongPressed);
        connect(tile, &LauncherTile::dragMoved, this, &DesktopIconLayer::onTileDragMoved);
        connect(tile, &LauncherTile::dragEnded, this, &DesktopIconLayer::onTileDragEnded);
        grid_->addWidget(tile, s.row, s.col);
        tiles_.append(tile);
    }
    grid_->activate(); // Force reflow after bulk add/remove — without this the
                       // new widgets can stay at stale positions after a drag.
}

LauncherTile* DesktopIconLayer::tileForApp(const QString& app_id) const {
    for (auto* tile : tiles_) {
        if (tile->appId() == app_id) {
            return tile;
        }
    }
    return nullptr;
}

const AppEntry* DesktopIconLayer::findEntry(const QString& app_id) const {
    for (const auto& entry : registry_) {
        if (entry.app_id == app_id) {
            return &entry;
        }
    }
    return nullptr;
}

QPoint DesktopIconLayer::cellAt(const QPoint& layer_pos) const {
    // Outside the layer entirely → (-1,-1) = "drop to delete".
    if (!rect().contains(layer_pos)) {
        return {-1, -1};
    }
    const auto dims =
        computeGridDimensions(last_available_.size(), static_cast<int>(shortcuts_.size()));
    if (dims.columns <= 0 || dims.rows <= 0) {
        return {-1, -1};
    }
    const int stride = kCellSize + kGridSpacing;
    const int col = std::clamp((layer_pos.x() - kMargin) / stride, 0, dims.columns - 1);
    const int row = std::clamp((layer_pos.y() - kMargin) / stride, 0, dims.rows - 1);
    return {col, row};
}

QPoint DesktopIconLayer::firstFreeCell() const {
    const auto dims =
        computeGridDimensions(last_available_.size(), static_cast<int>(shortcuts_.size()));
    for (int r = 0; r < dims.rows; ++r) {
        for (int c = 0; c < dims.columns; ++c) {
            bool taken = false;
            for (const auto& s : shortcuts_) {
                if (s.col == c && s.row == r) {
                    taken = true;
                    break;
                }
            }
            if (!taken) {
                return {c, r};
            }
        }
    }
    return {0, 0}; // Grid full: overlap (rebuildGrid stacks at the same cell).
}

void DesktopIconLayer::onTileLongPressed(const QString& app_id) {
    auto* tile = tileForApp(app_id);
    if (tile == nullptr) {
        return;
    }
    dragging_app_id_ = app_id;
    const int idx = indexOfApp(shortcuts_, app_id);
    drag_origin_cell_ =
        (idx >= 0) ? QPoint(shortcuts_[idx].col, shortcuts_[idx].row) : QPoint(-1, -1);
    drag_target_cell_ = drag_origin_cell_;
    // Floating ghost = grabbed pixmap of the tile, follows the finger. It is
    // transparent for mouse events so the source tile keeps receiving moves.
    const QPixmap pm = tile->grab();
    drag_ghost_ = new QLabel(this);
    drag_ghost_->setPixmap(pm);
    drag_ghost_->setScaledContents(false);
    drag_ghost_->setAttribute(Qt::WA_TransparentForMouseEvents);
    drag_ghost_->setGeometry(QRect(tile->pos(), tile->size()));
    drag_ghost_->show();
    drag_ghost_->raise();
    update();
}

void DesktopIconLayer::onTileDragMoved(const QPoint& global_pos) {
    if (dragging_app_id_.isEmpty() || drag_ghost_ == nullptr) {
        return;
    }
    const QPoint layer_pos = mapFromGlobal(global_pos);
    const QSize sz = drag_ghost_->size();
    drag_ghost_->move(layer_pos - QPoint(sz.width() / 2, sz.height() / 2));
    drag_target_cell_ = cellAt(layer_pos);
    update();
}

void DesktopIconLayer::onTileDragEnded(const QPoint& global_pos) {
    if (dragging_app_id_.isEmpty()) {
        return;
    }
    const QPoint layer_pos = mapFromGlobal(global_pos);
    const QPoint target = cellAt(layer_pos);
    cf::log::infoftag(kLogTag, "dragEnd: target=({},{}) origin=({},{})", target.x(), target.y(),
                      drag_origin_cell_.x(), drag_origin_cell_.y());

    bool changed = false;
    QList<DesktopShortcut> updated = shortcuts_;
    if (target.x() < 0 || target.y() < 0) {
        // Dropped outside the layer → remove the shortcut.
        updated.erase(
            std::remove_if(updated.begin(), updated.end(),
                           [&](const DesktopShortcut& s) { return s.app_id == dragging_app_id_; }),
            updated.end());
        changed = true;
        cf::log::infoftag(kLogTag, "removed shortcut {} (dragged off desktop)",
                          dragging_app_id_.toStdString());
    } else if (target != drag_origin_cell_) {
        const int drag_idx = indexOfApp(updated, dragging_app_id_);
        const int tgt_idx = indexOfCell(updated, target);
        if (drag_idx >= 0 && tgt_idx >= 0) {
            // Swap the two shortcuts' positions.
            std::swap(updated[drag_idx].col, updated[tgt_idx].col);
            std::swap(updated[drag_idx].row, updated[tgt_idx].row);
            changed = true;
        } else if (drag_idx >= 0) {
            // Target cell empty: just move.
            updated[drag_idx].col = target.x();
            updated[drag_idx].row = target.y();
            changed = true;
        }
    }

    delete drag_ghost_;
    drag_ghost_ = nullptr;
    dragging_app_id_.clear();
    drag_origin_cell_ = {-1, -1};
    drag_target_cell_ = {-1, -1};

    if (changed) {
        shortcuts_ = std::move(updated);
        emit shortcutsChanged(shortcuts_);
        // Defer the rebuild to after this mouseRelease handler returns:
        // rebuildGrid uses qDeleteAll on the tiles, and the drag-source tile is
        // the very widget receiving this event — deleting it mid-event hits
        // Qt's deferred-delete trap and the new positions never visually land
        // (the tile "bounces back"). singleShot(0) runs once the event loop
        // regains control, where deleting any widget is safe.
        QTimer::singleShot(0, this, [this]() {
            cf::log::infoftag(kLogTag, "deferred rebuildGrid firing");
            rebuildGrid();
        });
    }
    update();
}

} // namespace cf::desktop::desktop_component
