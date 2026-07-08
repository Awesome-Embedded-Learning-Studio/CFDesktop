/**
 * @file    crash_report.cpp
 * @brief   Implementation of CrashReport JSON serialization.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_report.h"

#include <csignal>
#include <format>
#include <string>
#include <string_view>

namespace cf::crash {
namespace {

/// @brief Escapes a string for embedding inside a JSON string literal.
std::string escapeJson(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                out.push_back(c);
                break;
        }
    }
    return out;
}

/// @brief Renders a string vector as a pretty JSON array (one element/line).
std::string toJsonArray(const std::vector<std::string>& v) {
    if (v.empty()) {
        return "[]";
    }
    std::string out = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        out += "\n    \"";
        out += escapeJson(v[i]);
        out += '"';
        out += (i + 1 == v.size()) ? "" : ",";
    }
    out += "\n  ]";
    return out;
}

} // namespace

std::string CrashReport::toJson() const {
    std::string out = "{\n";
    out += std::format("  \"timestamp\": {},\n", timestamp);
    out += std::format("  \"pid\": {},\n", pid);
    out += std::format("  \"signal\": {},\n", signal);
    out += std::format("  \"signal_name\": \"{}\",\n", escapeJson(signal_name));
    out += "  \"raw_frames\": ";
    out += toJsonArray(raw_frames);
    out += ",\n  \"last_logs\": ";
    out += toJsonArray(last_logs);
    out += '\n';
    out += "}\n";
    return out;
}

std::string signalName(int signo) {
    switch (signo) {
        case SIGSEGV:
            return "SIGSEGV";
        case SIGABRT:
            return "SIGABRT";
        case SIGFPE:
            return "SIGFPE";
        case SIGILL:
            return "SIGILL";
#ifdef SIGBUS
        case SIGBUS:
            return "SIGBUS";
#endif
        default:
            return "UNKNOWN";
    }
}

} // namespace cf::crash
