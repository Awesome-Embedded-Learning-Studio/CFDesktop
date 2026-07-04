#include "console_signal_stage.h"

#ifdef _WIN32
#    include <QApplication>
#    include <windows.h>
#endif

namespace cf::desktop::early_stage {

#ifdef _WIN32
/**
 * @brief  Console Ctrl+C handler — asks Qt to quit gracefully.
 *
 * The default handler calls ExitProcess(), which runs static destructors
 * on the console-control thread (not the GUI thread).  QObject timers
 * and signals must be destroyed on the thread that owns them, so we
 * instead post QApplication::quit() and return TRUE to suppress the
 * default termination behaviour.
 */
static BOOL WINAPI consoleCtrlHandler(DWORD ctrlType) {
    if (ctrlType == CTRL_C_EVENT || ctrlType == CTRL_BREAK_EVENT) {
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return TRUE; // handled — do NOT call ExitProcess
    }
    return FALSE; // let the default handler deal with CLOSE / LOGOFF etc.
}
#endif

ConsoleSignalStage::BootResult ConsoleSignalStage::run_session() {
#ifdef _WIN32
    SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);
#endif
    return BootResult::OK;
}

} // namespace cf::desktop::early_stage
