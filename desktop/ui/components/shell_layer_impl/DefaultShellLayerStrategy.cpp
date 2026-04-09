#include "DefaultShellLayerStrategy.h"
#include "WidgetShellLayer.h"
#include "cflog.h"
#include "qt_format.h"

namespace cf::desktop {
DefaultShellLayerStrategy::DefaultShellLayerStrategy() {
    log::info("Using DefaultShellStrategy");
}

DefaultShellLayerStrategy::~DefaultShellLayerStrategy() = default;

void DefaultShellLayerStrategy::activate(WeakPtr<IShellLayer> layer, WeakPtr<WindowManager> wm) {
    log::trace("DefaultShellStrategy activated");
    layer_ = layer;
    window_manager_ = wm;
}

void DefaultShellLayerStrategy::deactivate() {
    log::trace("DefaultShellStrategy deactivated");
    layer_.Reset();
    window_manager_.Reset();
}

void DefaultShellLayerStrategy::onGeometryChanged(const QRect& r) {
    log::traceftag("DefaultShellStrategy", "Geometry changed: QRect({}, {}, {}, {})", r.x(), r.y(),
                   r.width(), r.height());
    // The WidgetShellLayer::onAvailableGeometryChanged() already calls
    // setGeometry() and update(), so no additional action needed here.
}

} // namespace cf::desktop
