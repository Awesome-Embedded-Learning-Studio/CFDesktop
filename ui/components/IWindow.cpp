#include "IWindow.h"

namespace cf::desktop {
IWindow::IWindow(QObject* parent) : QObject{parent}, weak_ptr_factory_(this) {}
IWindow::~IWindow() {}
} // namespace cf::desktop
