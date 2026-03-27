#include "cflog.h"
#include "desktop_entry.h"
#include "init/init.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    QApplication cf_desktop_app(argc, argv);

    qDebug() << "Qt Application is Boot OK, About to run Early Boot...";

    /* Run Stages */
    cf::desktop::init_session::RunEarlyInit();
    cf::desktop::init_session::RunStageInit();

    /* Boot Ups, All can enter the Desktop Issues */
    cf::desktop::boot_desktop();

    cf::log::traceftag("Main", "Boot Finished, Ready to launch everything!");
    auto quit_code = cf_desktop_app.exec();

    /* After our quit, we may need to do some deinits */
    qDebug() << "Desktop Dismissed with code: " << quit_code << ", Goodbye!";
}
