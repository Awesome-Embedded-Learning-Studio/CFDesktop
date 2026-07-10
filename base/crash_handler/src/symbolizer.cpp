/**
 * @file    symbolizer.cpp
 * @brief   addr2line-based symbol resolution for crash frames.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/symbolizer.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace cf::crash {

std::vector<ResolvedFrame> parseAddr2LineOutput(const std::vector<std::string>& lines,
                                                std::size_t frame_count) {
    std::vector<ResolvedFrame> out;
    out.reserve(frame_count);
    for (std::size_t i = 0; i < frame_count; ++i) {
        const std::size_t func_idx = i * 2;
        const std::size_t file_idx = i * 2 + 1;
        if (file_idx >= lines.size()) {
            break; // addr2line produced fewer lines than expected
        }
        ResolvedFrame frame;
        frame.function = lines[func_idx];
        const std::string& fl = lines[file_idx];
        // addr2line prints "path/file.cpp:123" (or "??:0"); split on the last ':'.
        const auto colon = fl.rfind(':');
        if (colon != std::string::npos) {
            frame.file = fl.substr(0, colon);
            frame.line = fl.substr(colon + 1);
        } else {
            frame.file = fl;
        }
        out.push_back(std::move(frame));
    }
    return out;
}

#ifdef __linux__
std::vector<ResolvedFrame> resolveFrames(const std::vector<std::string>& raw_frames,
                                         const std::string& exe_path) {
    if (raw_frames.empty() || exe_path.empty()) {
        return {};
    }
    // One addr2line invocation for all addresses; -f prints function then
    // file:line per address, -C demangles. exe_path is quoted; paths with
    // spaces would still need shell-safe escaping but applicationFilePath()
    // does not contain spaces in practice.
    std::string cmd = "addr2line -e \"";
    cmd += exe_path;
    cmd += "\" -f -C";
    for (const auto& addr : raw_frames) {
        cmd += ' ';
        cmd += addr;
    }
    cmd += " 2>/dev/null";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        return {};
    }
    std::array<char, 512> buf{};
    std::vector<std::string> lines;
    while (std::fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr) {
        std::string line(buf.data());
        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }
    pclose(pipe);
    return parseAddr2LineOutput(lines, raw_frames.size());
}
#else
std::vector<ResolvedFrame> resolveFrames(const std::vector<std::string>& /*raw_frames*/,
                                         const std::string& /*exe_path*/) {
    // Non-Linux (Windows): SetUnhandledExceptionFilter + dbghelp SymFromAddr
    // land in a later phase. Returning empty leaves resolved_frames blank,
    // which the reporter renders as raw addresses.
    return {};
}
#endif

} // namespace cf::crash
