#include "desktop_entry.h"

#include <QDir>
#include <QLockFile>
#include <QString>
#include <QtGlobal>

#include "cfipc/ipc_client.h"
#include "ui/widget/material/application/material_application.h"

namespace {
/// @brief Tries to become the single desktop shell instance.
///
/// Acquires a QLockFile ($TMPDIR/cfdesktop-shell.lock) held for the whole
/// process; a stale lock from a crashed shell is reclaimed because QLockFile
/// verifies the owning PID. QtCore-only (no QtNetwork/D-Bus, suits the 6ULL
/// rule). Lives in main.cpp's anonymous namespace so it stays in the exe
/// target and never crosses the CFDesktop_shared dll boundary (no export
/// headaches on MSVC).
bool acquireSingleInstanceLock() {
    static QLockFile lock(QDir::tempPath() + QStringLiteral("/cfdesktop-shell.lock"));
    lock.setStaleLockTime(0);
    return lock.tryLock(100); // false if another live shell holds the lock
}
} // namespace

int main(int argc, char* argv[]) {
    // MaterialApplication registers the MD3 themes (light/dark) into ThemeManager
    // on construction, so panels and widgets resolve real theme tokens at runtime.
    qw::widget::material::MaterialApplication cf_desktop_app(argc, argv);

    // Refuse to start a second desktop shell -- two fullscreen shells on one
    // screen fight over geometry and WindowManagers cross-track each other's
    // windows.
    if (!acquireSingleInstanceLock()) {
        // Ask the running shell to bring itself to front, then exit. Best-effort:
        // if no listener is around (crashed mid-boot) send fails silently.
        cf::ipc::IPCClient::send(QDir::tempPath() + QStringLiteral("/cfdesktop-shell.ipc"),
                                 QStringLiteral("raise"));
        return 0;
    }

    return cf::desktop::run_desktop_session();
}
