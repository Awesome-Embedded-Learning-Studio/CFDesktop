/**
 * @file    crash_handler.h
 * @brief   Process-wide crash handler: signal capture and report finalization.
 *
 * On install(), registers POSIX signal handlers (Linux) that write a minimal
 * async-signal-safe raw snapshot (a @c .pending file in the crashes dir) and
 * _exit().
 * The next boot calls finalizePendingReports() to fold each @c .pending (plus
 * the tail of the logger file) into a finalized @c *.json report, then prunes
 * to a retention cap. Symbol resolution and a crash-reporter UI are Phase 2.
 *
 * Not a QObject: the signal path is pure C and must not touch Qt.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#pragma once

#include <cstddef>
#include <string>

#include "cfcrash/crash_report.h"

namespace cf::crash {

/**
 * @brief  Process-wide singleton crash handler.
 *
 * @note   The signal handler is async-signal-safe; install/finalize are not.
 * @warning None
 * @since  0.19.0
 * @ingroup crash
 */
class CrashHandler {
  public:
    /**
     * @brief  Returns the process-wide singleton instance.
     *
     * @return     Reference to the singleton CrashHandler.
     * @throws     None
     * @note       Created on first access; lives until process exit.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    static CrashHandler& instance();

    /**
     * @brief  Registers crash signal handlers and prepares the crashes dir.
     *
     * @param[in]  crashes_dir  Directory for @c .pending / @c .json reports.
     * @param[in]  logger_path  This run's logger file; written into each
     *                          @c .pending so finalize() tails the crashed
     *                          run's own log. Empty to omit.
     * @return     True if handlers registered; false on failure (best-effort).
     * @throws     None
     * @note       Creates crashes_dir if missing.
     * @warning    Call after QCoreApplication exists so the dir is resolvable.
     * @since      0.19.0
     * @ingroup    crash
     */
    bool install(const std::string& crashes_dir, const std::string& logger_path = "");

    /**
     * @brief  Restores default signal disposition (test-only).
     *
     * @throws     None
     * @note       No-op if install() was not called.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    void uninstall();

    /**
     * @brief  Folds each @c .pending snapshot into a finalized @c .json report.
     *
     * @param[in]  logger_path  Logger file whose tail becomes last_logs.
     * @param[in]  tail_lines   Number of trailing log lines to capture.
     * @return     Count of reports finalized.
     * @throws     None
     * @note       Deletes each @c .pending after writing its @c .json; prunes.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    std::size_t finalizePendingReports(const std::string& logger_path,
                                       std::size_t tail_lines = kDefaultTailLogLines,
                                       const std::string& exe_path = "");

    /**
     * @brief  Deletes oldest @c .json reports beyond the retention cap.
     *
     * @param[in]  max_keep  Maximum reports to keep.
     * @return     Count of reports deleted.
     * @throws     None
     * @note       Sorts by modification time; youngest kept.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    std::size_t pruneReports(std::size_t max_keep = kDefaultMaxReports);

    /**
     * @brief  Sets the crashes directory without installing handlers.
     *
     * @param[in]  crashes_dir  Directory for reports.
     * @return     None
     * @throws     None
     * @note       For finalize/prune tests that do not want handlers armed.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    void setCrashesDir(const std::string& crashes_dir);

  private:
    /**
     * @brief  Constructs the crash handler.
     *
     * @throws     None
     * @note       Private; access via instance().
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    CrashHandler();

    /// @brief Directory where @c .pending / @c .json reports are stored.
    std::string crashes_dir_;

    /// @brief True once signal handlers have been armed.
    bool installed_{false};
};

} // namespace cf::crash
