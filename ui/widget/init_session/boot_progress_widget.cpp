#include "boot_progress_widget.h"
#include <QGuiApplication>
#include <QScreen>

namespace cf::desktop::init_session {
BootProgressWidget::BootProgressWidget(QWidget* parent) : QWidget(parent) {}

BootProgressWidget::~BootProgressWidget() {}

void BootProgressWidget::setInitialDisplayStatus() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int width = screenGeometry.width() / 2;
        int height = screenGeometry.height() / 2;
        setFixedSize(width, height);
        move(screenGeometry.center() - rect().center());
    }
}

} // namespace cf::desktop::init_session
