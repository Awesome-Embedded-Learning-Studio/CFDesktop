/**
 * @file    crash_report.h
 * @brief   Crash report data model and JSON serialization.
 *
 * Defines the CrashReport struct assembled on the next boot from a raw signal
 * snapshot. The signal handler itself writes only a minimal async-signal-safe
 * @c .pending file; this struct folds that snapshot with the logger tail into
 * a finalized JSON report. Symbol resolution is deferred to Phase 2, so
 * raw_frames holds bare addresses.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cf::crash {

/// @brief Default number of trailing log lines captured into a report.
inline constexpr std::size_t kDefaultTailLogLines{50};

/// @brief Default maximum number of finalized reports kept on disk.
inline constexpr std::size_t kDefaultMaxReports{20};

/**
 * @brief  One symbol-resolved stack frame.
 *
 * Produced by the symbolizer (addr2line on Linux) from a bare address: the
 * demangled function name, source file, and line. Holds "??"/"0" when the
 * address could not be resolved.
 *
 * @ingroup crash
 */
struct ResolvedFrame {
    /// @brief Demangled function name (or "??").
    std::string function;

    /// @brief Source file path (or "??").
    std::string file;

    /// @brief Source line number as a string (or "0").
    std::string line;
};

/**
 * @brief  A finalized crash report.
 *
 * Aggregates the raw signal snapshot (timestamp/pid/signal/raw stack
 * addresses) with the tail of the logger file, ready for JSON storage.
 *
 * @note   Plain struct; serialize via toJson().
 * @warning None
 * @since  0.19.0
 * @ingroup crash
 */
struct CrashReport {
    /// @brief Wall-clock time of the crash, in seconds since the epoch.
    std::int64_t timestamp{0};

    /// @brief Process id of the crashed shell.
    std::int64_t pid{0};

    /// @brief Signal number (e.g. SIGSEGV=11). 0 when no signal applies.
    int signal{0};

    /// @brief Human-readable signal name ("SIGSEGV", "SIGABRT", ...).
    std::string signal_name;

    /// @brief Bare stack frame addresses as hex strings.
    std::vector<std::string> raw_frames;

    /// @brief Symbol-resolved frames (function/file/line); empty until finalize.
    std::vector<ResolvedFrame> resolved_frames;

    /// @brief Tail of the logger file captured at finalize time.
    std::vector<std::string> last_logs;

    /**
     * @brief  Serializes the report to a pretty multi-line JSON string.
     *
     * @return     JSON text (UTF-8); array elements are one per line.
     * @throws     None
     * @note       Strings are JSON-escaped.
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash
     */
    std::string toJson() const;
};

/**
 * @brief  Maps a POSIX signal number to its canonical name.
 *
 * @param[in]  signo  Signal number.
 * @return     "SIGSEGV" etc., or "UNKNOWN" if unrecognized.
 * @throws     None
 * @note       None
 * @warning    None
 * @since      0.19.0
 * @ingroup    crash
 */
std::string signalName(int signo);

} // namespace cf::crash
