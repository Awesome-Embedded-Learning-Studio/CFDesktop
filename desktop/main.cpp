#include "desktop_entry.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication cf_desktop_app(argc, argv);
    return cf::desktop::run_desktop_session();
}
