/**
 * @file    symbolizer.h
 * @brief   Symbol resolution for raw crash stack frames.
 *
 * Phase 2: converts the bare addresses captured by the signal handler into
 * (function, file, line) triples via addr2line on Linux. Non-Linux targets
 * get a stub (Windows dbghelp is deferred). resolveFrames() runs in a normal
 * context (next boot), never in a signal handler, so it may spawn a subprocess.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#pragma once

#include "cfcrash/crash_report.h"

#include <cstddef>
#include <string>
#include <vector>

namespace cf::crash {

/**
 * @brief  Resolves bare frame addresses against the executable.
 *
 * @param[in]  raw_frames  Hex address strings (e.g. "0x7f...").
 * @param[in]  exe_path    Path to the executable the addresses belong to.
 * @return     One ResolvedFrame per input address; empty on non-Linux or when
 *             exe_path is empty or addr2line is unavailable.
 * @throws     None
 * @note       Linux only; spawns addr2line via popen.
 * @warning    Not async-signal-safe; call from a normal context only.
 * @since      0.19.0
 * @ingroup    crash
 */
std::vector<ResolvedFrame> resolveFrames(const std::vector<std::string>& raw_frames,
                                         const std::string& exe_path);

/**
 * @brief  Parses addr2line output lines into resolved frames.
 *
 * addr2line prints two lines per address (function, then file:line). This
 * helper is split out so it can be unit-tested without spawning the tool.
 *
 * @param[in]  lines        Output lines from addr2line.
 * @param[in]  frame_count  Number of addresses that were queried.
 * @return     Up to frame_count ResolvedFrames; fewer if lines run out.
 * @throws     None
 * @note       None
 * @warning    None
 * @since      0.19.0
 * @ingroup    crash
 */
std::vector<ResolvedFrame> parseAddr2LineOutput(const std::vector<std::string>& lines,
                                                std::size_t frame_count);

} // namespace cf::crash
