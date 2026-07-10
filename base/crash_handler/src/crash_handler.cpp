/**
 * @file    crash_handler.cpp
 * @brief   Implementation of the CrashHandler singleton (normal-context half).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_handler.h"

#include "cfcrash/crash_report.h"
#include "cfcrash/symbolizer.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace cf::crash {

// Platform-specific signal arming lives in crash_signal_handler.cpp.
bool installSignalHandlers(const std::string& dir, const std::string& logger_path);
void uninstallSignalHandlers();

namespace {

namespace fs = std::filesystem;

/// @brief Reads the trailing tail_lines lines of a file.
std::vector<std::string> tailLines(const fs::path& path, std::size_t tail_lines) {
    std::vector<std::string> all;
    std::ifstream in(path);
    if (!in) {
        return all;
    }
    std::string line;
    while (std::getline(in, line)) {
        all.push_back(std::move(line));
    }
    if (all.size() <= tail_lines) {
        return all;
    }
    return {all.end() - static_cast<std::ptrdiff_t>(tail_lines), all.end()};
}

/// @brief Parses one .pending raw snapshot; returns the file stem on success.
/// @param[out] pending_logger  The crashed run's logger path (may be empty).
bool parsePending(const fs::path& path, CrashReport& out, std::string& stem,
                  std::string& pending_logger) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::string header;
    if (!std::getline(in, header) || header != "CFDESKTOP_CRASH_V1") {
        return false;
    }
    enum class Section { kFields, kFrames } section = Section::kFields;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (section == Section::kFrames) {
            if (line.starts_with("0x")) {
                out.raw_frames.push_back(line);
            }
            continue;
        }
        if (line.starts_with("frames=")) {
            section = Section::kFrames;
            continue;
        }
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string key = line.substr(0, eq);
        const std::string val = line.substr(eq + 1);
        if (key == "signal") {
            out.signal = std::atoi(val.c_str());
        } else if (key == "signal_name") {
            out.signal_name = val;
        } else if (key == "pid") {
            out.pid = std::atoll(val.c_str());
        } else if (key == "timestamp") {
            out.timestamp = std::atoll(val.c_str());
        } else if (key == "logger") {
            pending_logger = val;
        }
    }
    if (out.signal_name.empty()) {
        out.signal_name = signalName(out.signal);
    }
    stem = path.stem().string();
    return true;
}

} // namespace

CrashHandler& CrashHandler::instance() {
    static CrashHandler handler;
    return handler;
}

CrashHandler::CrashHandler() = default;

void CrashHandler::setCrashesDir(const std::string& crashes_dir) {
    crashes_dir_ = crashes_dir;
    std::error_code ec;
    fs::create_directories(crashes_dir_, ec);
}

bool CrashHandler::install(const std::string& crashes_dir, const std::string& logger_path) {
    setCrashesDir(crashes_dir);
    const bool ok = installSignalHandlers(crashes_dir, logger_path);
    installed_ = ok;
    return ok;
}

void CrashHandler::uninstall() {
    uninstallSignalHandlers();
    installed_ = false;
}

std::size_t CrashHandler::finalizePendingReports(const std::string& logger_path,
                                                 std::size_t tail_lines,
                                                 const std::string& exe_path) {
    if (crashes_dir_.empty()) {
        return 0;
    }

    std::size_t count = 0;
    std::error_code ec;
    for (auto& entry : fs::directory_iterator(crashes_dir_, ec)) {
        if (ec) {
            break;
        }
        if (!entry.is_regular_file() || entry.path().extension() != ".pending") {
            continue;
        }
        CrashReport report;
        std::string stem;
        std::string pending_logger;
        if (!parsePending(entry.path(), report, stem, pending_logger)) {
            continue;
        }
        // Prefer the crashed run's own logger path (captured in the .pending);
        // fall back to the caller-supplied path for snapshots without one.
        const std::string effective_logger = pending_logger.empty() ? logger_path : pending_logger;
        report.last_logs = tailLines(effective_logger, tail_lines);
        // Phase 2: symbolize raw addresses against the running executable.
        if (!exe_path.empty()) {
            report.resolved_frames = resolveFrames(report.raw_frames, exe_path);
        }

        const auto out_path = fs::path(crashes_dir_) / (stem + ".json");
        std::ofstream of(out_path);
        if (of) {
            of << report.toJson();
            of.close();
            ++count;
        }
        fs::remove(entry.path(), ec);
    }
    if (count > 0) {
        pruneReports();
    }
    return count;
}

std::size_t CrashHandler::pruneReports(std::size_t max_keep) {
    if (crashes_dir_.empty()) {
        return 0;
    }
    std::vector<std::pair<fs::file_time_type, fs::path>> reports;
    std::error_code ec;
    for (auto& entry : fs::directory_iterator(crashes_dir_, ec)) {
        if (ec) {
            break;
        }
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }
        reports.emplace_back(entry.last_write_time(ec), entry.path());
    }
    if (reports.size() <= max_keep) {
        return 0;
    }
    std::sort(reports.begin(), reports.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    const std::size_t to_remove = reports.size() - max_keep;
    std::size_t removed = 0;
    for (std::size_t i = 0; i < to_remove; ++i) {
        if (fs::remove(reports[i].second, ec)) {
            ++removed;
        }
    }
    return removed;
}

} // namespace cf::crash
