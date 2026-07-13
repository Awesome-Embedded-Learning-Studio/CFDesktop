/**
 * @file    page_stack_widget.cpp
 * @brief   Implementation of the horizontal-swipe PageStackWidget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "page_stack_widget.h"

#include <QCoreApplication>
#include <QEasingCurve>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWidget>

#include <algorithm>

namespace cf::desktop::desktop_component {

namespace {
constexpr int kSwipeIdleWatchdogMs = 650;
} // namespace

/// @brief Opaque raised overlay that paints two cached page snapshots translated
///        by a horizontal offset. Replaces per-frame full-tree repaints with two
///        pixmap blits during a swipe. No Q_OBJECT (paintEvent override only).
class SwipeOverlay : public QWidget {
  public:
    explicit SwipeOverlay(QWidget* parent) : QWidget(parent) {
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        hide();
    }

    void setSnapshots(const QPixmap& background, const QPixmap& current, const QPixmap& other,
                      bool other_is_next) {
        snap_background_ = background;
        snap_current_ = current;
        snap_other_ = other;
        other_is_next_ = other_is_next;
    }

    void setOffset(int dx) {
        offset_ = dx;
        update();
    }

  protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter p(this);
        if (!snap_background_.isNull()) {
            p.drawPixmap(0, 0, snap_background_);
        } else {
            p.fillRect(rect(), palette().window());
        }
        // Current page slides with the drag; the neighbor enters from the side.
        p.drawPixmap(offset_, 0, snap_current_);
        const int neighbor_x = offset_ + (other_is_next_ ? width() : -width());
        p.drawPixmap(neighbor_x, 0, snap_other_);
    }

  private:
    QPixmap snap_background_;
    QPixmap snap_current_;
    QPixmap snap_other_;
    int offset_{0};
    bool other_is_next_{false};
};

/// @brief Renders @p w and its child tree into a transparent page-layer pixmap.
QPixmap renderPageLayer(QWidget* w) {
    QPixmap pix(w->size());
    pix.fill(Qt::transparent);
    w->render(&pix);
    return pix;
}

bool hasUsableSnapshot(const QVector<QPixmap>& cache, int idx, const QSize& size) {
    return idx >= 0 && idx < cache.size() && !cache[idx].isNull() && cache[idx].size() == size;
}

int swipeDirectionSign(bool other_is_next) {
    return other_is_next ? -1 : 1;
}

int clampSwipeOffset(int dx, bool other_is_next, int width) {
    if (other_is_next) {
        return std::clamp(dx, -width, 0);
    }
    return std::clamp(dx, 0, width);
}

void flushWidgetNow(QWidget* widget) {
    if (widget == nullptr) {
        return;
    }
    widget->update();
    if (QWidget* top = widget->window()) {
        top->update();
        QCoreApplication::sendPostedEvents(top, QEvent::UpdateRequest);
        top->repaint();
    }
    widget->repaint();
}

PageStackWidget::PageStackWidget(QWidget* parent)
    : QStackedWidget(parent), overlay_(new SwipeOverlay(this)),
      swipe_anim_(new QPropertyAnimation(this, "swipeOffset", this)) {
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(false);
    swipe_anim_->setEasingCurve(QEasingCurve::OutCubic);
}

void PageStackWidget::setAnimationDuration(int duration_ms) {
    animation_duration_ = duration_ms;
}

void PageStackWidget::setBackgroundProvider(std::function<QImage()> provider) {
    background_provider_ = std::move(provider);
    update();
}

int PageStackWidget::animationDuration() const {
    return animation_duration_;
}

void PageStackWidget::slideToPage(int new_index) {
    // Moving to a higher index flips the next page in from the right (to_left).
    slideToPage(new_index, currentIndex() < new_index);
}

void PageStackWidget::toNextPage() {
    if (currentIndex() < count() - 1) {
        slideToPage(currentIndex() + 1, true);
    }
}

void PageStackWidget::toPrevPage() {
    if (currentIndex() > 0) {
        slideToPage(currentIndex() - 1, false);
    }
}

void PageStackWidget::slideToPage(int new_index, bool to_left) {
    if (new_index < 0 || new_index >= count() || new_index == currentIndex()) {
        return;
    }
    beginSwipe(to_left);
    animateSwipeTo(to_left ? -width() : width(), new_index);
}

void PageStackWidget::mousePressEvent(QMouseEvent* event) {
    if (dragging_ || swiping_) {
        // Resistive touchscreens (tslib/evdev) can emit spurious re-press events
        // mid-drag from pressure jitter. Ignore them so they do not reset the
        // gesture origin and snap the page offset back to ~0 mid-swipe.
        if (swiping_) {
            retouch_during_swipe_ = true;
            const int dx = event->pos().x() - start_pos_.x();
            const int directed = dx * swipeDirectionSign(other_is_next_);
            if (directed > swipe_peak_extent_) {
                swipe_peak_extent_ = directed;
            }
            armSwipeIdleWatchdog();
        }
        event->accept();
        return;
    }
    start_pos_ = event->pos();
    dragging_ = true;
    retouch_during_swipe_ = false;
    swipe_peak_extent_ = 0;
    grabMouse();
    event->accept();
}

void PageStackWidget::mouseMoveEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        return;
    }
    const int dx = event->pos().x() - start_pos_.x();
    if (qAbs(dx) < 5) {
        return;
    }
    const int index = currentIndex();
    const bool to_next = (dx < 0 && index < count() - 1);
    const bool to_prev = (dx > 0 && index > 0);
    if (!to_next && !to_prev) {
        return; // no neighbor in the dragged direction
    }
    if (!swiping_) {
        beginSwipe(to_next);
    }
    setSwipeOffset(dx);
}

void PageStackWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        releaseMouse();
        return;
    }
    dragging_ = false;
    if (!swiping_) {
        ++swipe_idle_generation_;
        releaseMouse();
        return; // drag never moved far enough to start a swipe
    }

    const int move_dx = swipe_offset_;
    const int release_dx = event->pos().x() - start_pos_.x();
    const int sign = swipeDirectionSign(other_is_next_);
    const int move_extent = std::max(0, move_dx * sign);
    const int release_extent = std::max(0, release_dx * sign);
    const int extent = std::max({move_extent, release_extent, swipe_peak_extent_});
    const int dx = sign * extent;
    if (dx != swipe_offset_) {
        setSwipeOffset(dx);
    }
    ++swipe_idle_generation_;
    // width/6 (~17%) — a resistive touchscreen's natural swipe rarely crosses
    // width/4, so a lower threshold avoids a frustrating "must swipe twice"
    // bounce on the first attempt.
    const int threshold = width() / 6;
    const int idx = currentIndex();

    const bool retouch_commit = retouch_during_swipe_ && extent > 0;

    if ((extent > threshold || (other_is_next_ && retouch_commit)) && dx < 0
        && idx < count() - 1) {
        animateSwipeTo(-width(), idx + 1); // commit to next page
    } else if ((extent > threshold || (!other_is_next_ && retouch_commit)) && dx > 0
               && idx > 0) {
        animateSwipeTo(width(), idx - 1); // commit to previous page
    } else {
        animateSwipeTo(0, idx); // bounce back to current page
    }
}

void PageStackWidget::beginSwipe(bool to_next) {
    if (swiping_) {
        return;
    }
    const int idx = currentIndex();
    const int other_idx = to_next ? idx + 1 : idx - 1;
    if (other_idx < 0 || other_idx >= count()) {
        return;
    }

    QWidget* cur = currentWidget();
    QWidget* other = widget(other_idx);
    const int cur_idx = currentIndex();
    // Ensure the neighbor is laid out at the current page size before grabbing
    // (only needed for the cold-render fallback below).
    other->setGeometry(QRect(QPoint(0, 0), cur->size()));

    if (snap_cache_.size() < count()) {
        snap_cache_.resize(count());
    }
    // Always refresh the currently visible page layer before hiding it. A later
    // swipe back to this page must not cold-render a hidden, complex widget tree
    // on linuxfb; that path can produce stale or incomplete pixels.
    const QPixmap current_layer = renderPageLayer(cur);
    snap_cache_[cur_idx] = current_layer;

    // Prefer a size-valid warm cache for the neighbor. Fall back only before a
    // page has ever been visible and captured.
    const QPixmap other_layer = hasUsableSnapshot(snap_cache_, other_idx, cur->size())
                                    ? snap_cache_[other_idx]
                                    : renderPageLayer(other);

    // Hide the live current page AFTER capturing it; the page stack and overlay
    // paint the wallpaper backdrop explicitly before drawing transparent page
    // layers, so old linuxfb pixels cannot leak through page holes.
    cur->setVisible(false);
    const QPixmap background = backgroundSnapshot();

    overlay_->setGeometry(QRect(QPoint(0, 0), cur->size()));
    overlay_->setSnapshots(background, current_layer, other_layer, to_next);
    overlay_->setOffset(0);
    overlay_->show();
    overlay_->raise();

    swiping_ = true;
    other_is_next_ = to_next;
    swipe_offset_ = 0;
    swipe_peak_extent_ = 0;
    retouch_during_swipe_ = false;
    source_index_ = cur_idx;
    target_index_ = other_idx;
    armSwipeIdleWatchdog();
}

void PageStackWidget::setSwipeOffset(int dx) {
    if (swiping_) {
        dx = clampSwipeOffset(dx, other_is_next_, width());
        const int directed = dx * swipeDirectionSign(other_is_next_);
        if (directed > swipe_peak_extent_) {
            swipe_peak_extent_ = directed;
        }
    }
    swipe_offset_ = dx;
    overlay_->setOffset(dx);
    if (swiping_) {
        armSwipeIdleWatchdog();
    }
}

void PageStackWidget::animateSwipeTo(int target_offset, int new_index) {
    target_index_ = new_index;
    swipe_anim_->stop();
    swipe_anim_->setDuration(animation_duration_);
    swipe_anim_->setStartValue(swipe_offset_);
    swipe_anim_->setEndValue(target_offset);
    disconnect(swipe_anim_, nullptr, this, nullptr);
    connect(swipe_anim_, &QPropertyAnimation::finished, this, [this, target_offset]() {
        setSwipeOffset(target_offset);
        overlay_->repaint();
        endSwipe(target_index_);
    });
    swipe_anim_->start();
}

void PageStackWidget::endSwipe(int new_index) {
    const int source_index = source_index_;
    releaseMouse();
    swiping_ = false;
    swipe_offset_ = 0;
    swipe_peak_extent_ = 0;
    retouch_during_swipe_ = false;
    source_index_ = -1;
    target_index_ = -1;
    if (new_index >= 0 && new_index < count()) {
        // Normalize every page explicitly. During snapshot swipes beginSwipe()
        // hides the source page outside QStackedWidget's normal transition path;
        // relying on setCurrentIndex() alone can leave a previously hidden page
        // or the old icon page with stale visible/raise state on linuxfb.
        setCurrentIndex(new_index);
        for (int i = 0; i < count(); ++i) {
            QWidget* page = widget(i);
            page->move(QPoint(0, 0));
            page->resize(size());
            page->setVisible(i == new_index);
        }
        widget(new_index)->raise();
        overlay_->raise();
        overlay_->show();
        overlay_->repaint();
        QTimer::singleShot(0, this, [this, new_index]() {
            if (!swiping_ && currentIndex() == new_index) {
                overlay_->hide();
                flushWidgetNow(widget(new_index));
                flushWidgetNow(this);
            }
        });
        // Re-capture this page's snapshot now that it is visible, so the next
        // swipe away reads a warm pixmap from the cache instead of rendering it
        // cold on the gesture path. Deferred so the gesture has already settled.
        QTimer::singleShot(80, this, [this, new_index]() {
            if (!swiping_ && currentIndex() == new_index) {
                refreshSnapshot(new_index);
            }
        });
    } else if (source_index >= 0 && source_index < count()) {
        setCurrentIndex(source_index);
        widget(source_index)->setVisible(true);
        overlay_->hide();
    } else {
        overlay_->hide();
    }
}

void PageStackWidget::armSwipeIdleWatchdog() {
    const int generation = ++swipe_idle_generation_;
    QTimer::singleShot(kSwipeIdleWatchdogMs, this, [this, generation]() {
        if (generation != swipe_idle_generation_ || !dragging_ || !swiping_) {
            return;
        }

        dragging_ = false;
        const int idx = currentIndex();
        const int target =
            other_is_next_ ? ((idx < count() - 1) ? idx + 1 : idx)
                           : ((idx > 0) ? idx - 1 : idx);
        ++swipe_idle_generation_;
        animateSwipeTo(other_is_next_ ? -width() : width(), target);
    });
}

void PageStackWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    const QPixmap background = backgroundSnapshot();
    if (!background.isNull()) {
        p.drawPixmap(0, 0, background);
    } else {
        p.fillRect(rect(), palette().window());
    }
    QStackedWidget::paintEvent(event);
}

void PageStackWidget::refreshSnapshot(int idx) {
    if (idx < 0 || idx >= count()) {
        return;
    }
    if (idx != currentIndex() || swiping_ || !widget(idx)->isVisible()) {
        return;
    }
    if (snap_cache_.size() < count()) {
        snap_cache_.resize(count());
    }
    snap_cache_[idx] = renderPageLayer(widget(idx));
}

QPixmap PageStackWidget::backgroundSnapshot() const {
    QPixmap pix(size());
    pix.fill(palette().window().color());

    const QImage image = background_provider_ ? background_provider_() : QImage{};
    if (image.isNull()) {
        return pix;
    }

    QPainter p(&pix);
    if (image.size() == size()) {
        p.drawImage(rect(), image);
    } else if (parentWidget() != nullptr && image.size() == parentWidget()->size()) {
        p.drawImage(rect(), image, geometry());
    } else {
        p.drawImage(rect(), image);
    }
    return pix;
}

void PageStackWidget::showEvent(QShowEvent* event) {
    QStackedWidget::showEvent(event);
    // Capture the initially-visible page so the first swipe away reads a warm
    // snapshot (deferred until the widget is laid out and painted).
    if (count() > 0) {
        QTimer::singleShot(0, this, [this]() {
            if (count() > 0) {
                refreshSnapshot(currentIndex());
            }
        });
    }
}

void PageStackWidget::resizeEvent(QResizeEvent* event) {
    QStackedWidget::resizeEvent(event);
    // The display geometry can change after first show (the boot-time probe
    // transitions 640x480 -> 1024x600). Cached snapshots captured at the old
    // size would render as a small/wrong page during a swipe, so invalidate the
    // cache and re-capture the current page at the new size.
    snap_cache_.fill(QPixmap());
    if (count() > 0) {
        QTimer::singleShot(0, this, [this]() {
            if (count() > 0) {
                refreshSnapshot(currentIndex());
            }
        });
    }
}

} // namespace cf::desktop::desktop_component
