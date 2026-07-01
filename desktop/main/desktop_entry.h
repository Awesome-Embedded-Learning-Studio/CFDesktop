/**
 * @file    desktop/desktop_entry.h
 * @brief   Desktop entry point and exit result definitions.
 *
 * Provides the main entry point for booting the desktop session and defines
 * the possible exit result codes.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

#include "export.h"

namespace cf::desktop {

/**
 * @brief  Exit result codes for the desktop session.
 * @ingroup none
 */
enum class DesktopExitResult {
    NORMAL_QUIT ///< Normal exit without errors.
};

/**
 * @brief  Boots the desktop session and initializes the desktop environment.
 *
 * Creates the desktop entity instance, initializes the session, displays the
 * desktop window, and releases early initialization resources.
 *
 * @return              Exit result code indicating the termination status.
 * @throws              None
 * @note                This function starts the main desktop event loop.
 * @warning             None
 * @since               N/A
 * @ingroup             none
 */
DesktopExitResult boot_desktop();

/**
 * @brief  Single entry point for the entire desktop session.
 *
 * Runs all initialization stages, boots the desktop, enters the Qt event loop,
 * and returns the exit code. The EXE only needs to create a QApplication and
 * call this function.
 *
 * @return  Exit code from the Qt event loop.
 */
CF_DESKTOP_EXPORT int run_desktop_session();

/**
 * @brief  Tries to become the single desktop shell instance.
 *
 * Acquires a lock file (<tt>$TMPDIR/cfdesktop-shell.lock</tt>) via QLockFile;
 * if a live shell already holds it, this returns false so the caller can exit
 * cleanly. A stale lock from a crashed shell is reclaimed because QLockFile
 * verifies the owning PID. Uses QLockFile (QtCore only -- no D-Bus, suits the
 * 6ULL rule).
 *
 * @return              true if this process is the sole shell; false if another
 *                      instance is already running (caller should exit).
 * @throws              None
 * @since               0.20
 * @ingroup             none
 */
CF_DESKTOP_EXPORT bool acquireSingleInstanceLock();

} // namespace cf::desktop
