#include "PanelManager.h"
#include "IPanel.h"

#include <QWidget>
#include <algorithm>
namespace cf::desktop {

PanelManager::PanelManager(QWidget* host, QObject* parent) : QObject{parent}, host_(host) {}
PanelManager::~PanelManager() = default;

PanelManager::RegisterFeedback PanelManager::registerPanel(WeakPtr<IPanel> panel) {
    if (!panel) {
        return RegisterFeedback::InvalidPanel;
    }

    auto it = std::find(panels.cbegin(), panels.cend(), panel);
    /* Already Registered */
    if (it != panels.cend()) {
        return RegisterFeedback::DuplicatePanel;
    }

    return RegisterFeedback::OK;
}

PanelManager::UnRegisterFeedback PanelManager::unregisterPanel(WeakPtr<IPanel> panel) {
    if (!panel) {
        return UnRegisterFeedback::UnknownPanel;
    }

    auto it = std::find(panels.begin(), panels.end(), panel);
    if (it == panels.end()) {
        return UnRegisterFeedback::UnknownPanel;
    }

    panels.erase(it);
    return UnRegisterFeedback::OK;
}

const QRect PanelManager::availableGeometry() const {
    return available_geometry_;
}

void PanelManager::relayout() {
    if (!host_) {
        available_geometry_ = QRect();
        return;
    }

    QRect available = host_->rect();

    // ── Collect panels by edge (skip invalid / zero-size) ──
    std::vector<WeakPtr<IPanel>> tops, bottoms, lefts, rights;
    for (const auto& p : panels) {
        if (!p || !p->widget() || p->preferredSize() <= 0)
            continue;
        switch (p->position()) {
            case PanelPosition::Top:
                tops.push_back(p);
                break;
            case PanelPosition::Bottom:
                bottoms.push_back(p);
                break;
            case PanelPosition::Left:
                lefts.push_back(p);
                break;
            case PanelPosition::Right:
                rights.push_back(p);
                break;
        }
    }

    // Higher priority → placed first (outermost on that edge)
    auto byPriority = [](const WeakPtr<IPanel>& a, const WeakPtr<IPanel>& b) {
        return a->priority() > b->priority();
    };
    std::sort(tops.begin(), tops.end(), byPriority);
    std::sort(bottoms.begin(), bottoms.end(), byPriority);
    std::sort(lefts.begin(), lefts.end(), byPriority);
    std::sort(rights.begin(), rights.end(), byPriority);

    // ── Top: stack downward from top edge ──
    for (const auto& p : tops) {
        int h = p->preferredSize();
        p->widget()->setGeometry(available.left(), available.top(), available.width(), h);
        available.setTop(available.top() + h);
    }

    // ── Bottom: stack upward from bottom edge ──
    for (const auto& p : bottoms) {
        int h = p->preferredSize();
        int y = available.bottom() - h + 1;
        p->widget()->setGeometry(available.left(), y, available.width(), h);
        available.setBottom(y - 1);
    }

    // ── Left: between top and bottom, stack rightward ──
    for (const auto& p : lefts) {
        int w = p->preferredSize();
        p->widget()->setGeometry(available.left(), available.top(), w, available.height());
        available.setLeft(available.left() + w);
    }

    // ── Right: between top and bottom, stack leftward ──
    for (const auto& p : rights) {
        int w = p->preferredSize();
        int x = available.right() - w + 1;
        p->widget()->setGeometry(x, available.top(), w, available.height());
        available.setRight(x - 1);
    }

    // Emit only when the geometry actually changed
    if (available_geometry_ != available) {
        available_geometry_ = available;
        emit availableGeometryChanged(available_geometry_);
    }
}

} // namespace cf::desktop
