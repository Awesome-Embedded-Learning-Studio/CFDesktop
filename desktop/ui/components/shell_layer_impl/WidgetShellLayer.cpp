#include "WidgetShellLayer.h"
#include "cflog.h"
#include "qt_format.h"

#include <QPainter>

namespace cf::desktop {
WidgetShellLayer::WidgetShellLayer(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WidgetAttribute::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
}

WidgetShellLayer::~WidgetShellLayer() {
    if (strategy_) {
        strategy_->deactivate();
        strategy_.reset();
    }
}

void WidgetShellLayer::setStrategy(std::unique_ptr<IShellLayerStrategy> strategy) {
    if (strategy_) {
        log::infoftag("WidgetShellLayer", "Replacing existing strategy");
        strategy_->deactivate();
    }
    strategy_ = std::move(strategy);
    if (strategy_) {
        // Activate with our weak ref; WindowManager not available at this layer
        strategy_->activate(GetWeak(), WeakPtr<WindowManager>{});
    }
}

QRect WidgetShellLayer::geometry() const {
    return QWidget::geometry();
}

void WidgetShellLayer::onAvailableGeometryChanged(const QRect& rect) {
    log::traceftag("WidgetShellLayer", "Available geometry changed: QRect({}, {}, {}, {})",
                   rect.x(), rect.y(), rect.width(), rect.height());
    setGeometry(rect);
    if (strategy_) {
        strategy_->onGeometryChanged(rect);
    }
    update();
}

void WidgetShellLayer::paintEvent(QPaintEvent*) {
    QPainter p(this);
    if (strategy_) {
        QImage bg = strategy_->currentBackgroundImage();
        if (!bg.isNull()) {
            p.drawImage(0, 0, bg);
            return;
        }
        p.fillRect(rect(), strategy_->backgroundColor());
        return;
    }
    p.fillRect(rect(), QColor(0x1c, 0x1b, 0x1f));
}

} // namespace cf::desktop
