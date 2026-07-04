/**
 * @file  desktop_run_session.cpp
 * @brief Single DLL entry point — compiled directly into CFDesktop_shared.dll.
 *
 * This file is a source of the CFDesktop_shared target (not CFDesktopMain),
 * so CFDESKTOP_EXPORTS is defined and run_desktop_session() is properly
 * exported from the DLL.
 */

#include "cflog.h"
#include "init/init.h"
#include "main/desktop_entry.h"
#include "ui/CFDesktopEntity.h"
#include <QApplication>
#include <QDebug>

namespace cf::desktop {

int run_desktop_session() {
    qDebug() << "Qt Application is Boot OK, About to run Early Boot...";

    /* Run Stages */
    init_session::RunEarlyInit();
    init_session::RunStageInit();

    /* Boot Ups, All can enter the Desktop Issues */
    boot_desktop();

    cf::log::traceftag("Main", "Boot Finished, Ready to launch everything!");
    auto quit_code = QApplication::exec();

    /* Explicit cleanup BEFORE QApplication dies — QWidgets must not outlive it */
    CFDesktopEntity::release(); // destroy entity (and its QWidgets) while QApplication is alive
    cf::log::Logger::instance().flush_sync(); // flush all pending logs before static destruction

    /* After our quit, we may need to do some deinits */
    qDebug() << "Desktop Dismissed with code: " << quit_code << ", Goodbye!";

    return quit_code;
}

} // namespace cf::desktop
