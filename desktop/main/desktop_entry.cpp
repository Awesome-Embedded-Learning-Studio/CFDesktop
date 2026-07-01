#include "desktop_entry.h"
#include "init/init.h"
#include "ui/CFDesktopEntity.h"
#include "ui/CFDesktopWindowProxy.h"

#include <QDir>
#include <QLockFile>
#include <QString>

namespace cf::desktop {

bool acquireSingleInstanceLock() {
    // QLockFile (QtCore only -- no QtNetwork/D-Bus, suits the 6ULL rule). The
    // static instance lives for the whole process and auto-unlocks on exit; a
    // stale lock left by a crash is reclaimed because QLockFile verifies the
    // owning PID.
    static QLockFile lock(QDir::tempPath() + QStringLiteral("/cfdesktop-shell.lock"));
    lock.setStaleLockTime(0);
    return lock.tryLock(100); // false if another live shell holds the lock
}

DesktopExitResult boot_desktop() {
    /* Desktop Placing here, for easy access handle... by manual :) */
    // Make A Instance Here
    auto& instance [[maybe_unused]] = CFDesktopEntity::instance();
    /* We might lately pass the entry to each components */
    instance.run_init();

    CFDesktopWindowProxy window_proxy;
    window_proxy.show_desktop();

    /* Ok, Can run off */
    init_session::ReleaseStageInitOldResources();

    // Oh, quit normally
    return DesktopExitResult::NORMAL_QUIT;
}

} // namespace cf::desktop
