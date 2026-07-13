/**
 * @file    crash_signal_handler.cpp
 * @brief   Async-signal-safe crash signal handler (Linux) + Win32 stub.
 *
 * @warning The Linux handler runs in signal context. It may only call
 *          async-signal-safe functions: write/open/close/backtrace/time/
 *          getpid/sigaction/_exit, plus the arithmetic helpers below. Do NOT
 *          add Qt, malloc, stdio, std::string, or any locking here -- doing so
 *          is undefined behavior (the CI triple-compiler matrix is what
 *          reliably catches such regressions).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#if defined(__unix__) || defined(__linux__)
#    include <csignal>
#    include <ctime>
#    include <execinfo.h>
#    include <fcntl.h>
#    include <unistd.h>
#endif

namespace cf::crash {
// Defined here, declared by crash_handler.cpp via forward declaration.
bool installSignalHandlers(const std::string& dir, const std::string& logger_path);
void uninstallSignalHandlers();
} // namespace cf::crash

namespace {
#if defined(__unix__) || defined(__linux__)

/// @brief Capacity of the captured path buffers (async-signal-safe reads).
inline constexpr std::size_t kPathCap = 512;

/// @brief Crashes dir captured at install() time; read by the handler.
char g_crashes_dir[kPathCap] = {};

/// @brief Whether g_crashes_dir has been populated.
bool g_dir_set = false;

/// @brief Logger file path captured at install() time; written into the
///        .pending so finalize() can tail the crashed run's own log.
char g_logger_path[kPathCap] = {};

/// @brief Whether g_logger_path has been populated.
bool g_logger_set = false;

/// @brief Alternate stack size: 64 KiB stays clear of glibc 2.34+ runtime
/// SIGSTKSZ surprises and gives backtrace() headroom.
inline constexpr std::size_t kAltStackSize = 1U << 16;

/// @brief Maximum stack frames captured per crash.
inline constexpr int kMaxFrames = 64;

/// @brief Writes a buffer fully, tolerating partial writes. Async-signal-safe.
void writeAll(int fd, const char* data, std::size_t len) {
    while (len > 0) {
        ssize_t n = ::write(fd, data, len);
        if (n <= 0) {
            return;
        }
        data += n;
        len -= static_cast<std::size_t>(n);
    }
}

/// @brief Writes a C string. Async-signal-safe.
void writeStr(int fd, const char* s) {
    writeAll(fd, s, std::strlen(s));
}

/// @brief Writes an unsigned decimal. Async-signal-safe (pure arithmetic).
void writeDecimal(int fd, unsigned long v) {
    char buf[24];
    char* p = buf + sizeof(buf);
    if (v == 0) {
        *--p = '0';
    }
    while (v > 0) {
        *--p = static_cast<char>('0' + (v % 10U));
        v /= 10U;
    }
    writeAll(fd, p, static_cast<std::size_t>(buf + sizeof(buf) - p));
}

/// @brief Writes a pointer as lowercase hex with an 0x prefix. Async-signal-safe.
void writeHexPtr(int fd, std::uintptr_t v) {
    writeStr(fd, "0x");
    if (v == 0) {
        writeStr(fd, "0");
        return;
    }
    char buf[2 * sizeof(std::uintptr_t)];
    char* p = buf + sizeof(buf);
    while (v > 0) {
        unsigned d = static_cast<unsigned>(v & 0xFU);
        *--p = static_cast<char>(d < 10U ? ('0' + d) : ('a' + (d - 10U)));
        v >>= 4;
    }
    writeAll(fd, p, static_cast<std::size_t>(buf + sizeof(buf) - p));
}

/// @brief Maps a signal number to its name. Async-signal-safe (no allocation).
const char* signalNameAS(int signo) {
    switch (signo) {
        case SIGSEGV:
            return "SIGSEGV";
        case SIGABRT:
            return "SIGABRT";
        case SIGFPE:
            return "SIGFPE";
        case SIGBUS:
            return "SIGBUS";
        case SIGILL:
            return "SIGILL";
        default:
            return "UNKNOWN";
    }
}

/// @brief Appends a C string to a fixed buffer. Async-signal-safe.
void appendChars(char* dst, std::size_t cap, std::size_t& i, const char* src) {
    for (std::size_t k = 0; src[k] != '\0' && i + 1U < cap; ++k) {
        dst[i++] = src[k];
    }
}

/// @brief Appends an unsigned decimal to a fixed buffer. Async-signal-safe.
void appendDecimal(char* dst, std::size_t cap, std::size_t& i, unsigned long v) {
    char buf[24];
    char* p = buf + sizeof(buf);
    if (v == 0) {
        *--p = '0';
    }
    while (v > 0) {
        *--p = static_cast<char>('0' + (v % 10U));
        v /= 10U;
    }
    while (p < buf + sizeof(buf) && i + 1U < cap) {
        dst[i++] = *p++;
    }
}

/// @brief The crash signal handler. Async-signal-safe; writes a .pending file.
extern "C" void cfCrashHandleSignal(int signo) {
    // Restore the default disposition so a fault inside the handler cannot
    // recurse.
    struct sigaction sa{};
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    ::sigaction(signo, &sa, nullptr);

    char path[kPathCap + 96];
    std::size_t i = 0;
    if (g_dir_set) {
        appendChars(path, sizeof(path), i, g_crashes_dir);
    }
    appendChars(path, sizeof(path), i, "/cfdesktop-");
    appendDecimal(path, sizeof(path), i, static_cast<unsigned long>(::getpid()));
    appendChars(path, sizeof(path), i, "-");
    appendDecimal(path, sizeof(path), i, static_cast<unsigned long>(signo));
    appendChars(path, sizeof(path), i, ".pending");
    path[i] = '\0';

    // 0644 (not 0600) so the dump is readable over NFS by the dev host for
    // offline symbolication. Contains no secrets — just frames + /proc/maps.
    int fd = ::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        writeStr(fd, "CFDESKTOP_CRASH_V1\n");

        writeStr(fd, "signal=");
        writeDecimal(fd, static_cast<unsigned long>(signo));
        writeStr(fd, "\n");

        writeStr(fd, "signal_name=");
        writeStr(fd, signalNameAS(signo));
        writeStr(fd, "\n");

        writeStr(fd, "pid=");
        writeDecimal(fd, static_cast<unsigned long>(::getpid()));
        writeStr(fd, "\n");

        writeStr(fd, "timestamp=");
        writeDecimal(fd, static_cast<unsigned long>(::time(nullptr)));
        writeStr(fd, "\n");

        // Record this run's logger file so finalize() on the next boot tails
        // the crashed run's own log (log_filename() is per-launch).
        if (g_logger_set) {
            writeStr(fd, "logger=");
            writeStr(fd, g_logger_path);
            writeStr(fd, "\n");
        }

        void* frames[kMaxFrames];
        int n = ::backtrace(frames, kMaxFrames);
        writeStr(fd, "frames=");
        writeDecimal(fd, static_cast<unsigned long>(n));
        writeStr(fd, "\n");
        for (int k = 0; k < n; ++k) {
            writeHexPtr(fd, reinterpret_cast<std::uintptr_t>(frames[k]));
            writeStr(fd, "\n");
        }

        // Dump /proc/self/maps so the frame addresses above can be symbolicated
        // offline: each line gives a shared object's [start] load address, so
        // offset_in_lib = frame_addr - start, then addr2line resolves to the
        // function/source line. Async-signal-safe (open/read/write/close only).
        writeStr(fd, "maps=\n");
        int mfd = ::open("/proc/self/maps", O_RDONLY | O_CLOEXEC);
        if (mfd >= 0) {
            char mbuf[768];
            ssize_t got = 0;
            while ((got = ::read(mfd, mbuf, sizeof(mbuf))) > 0) {
                ssize_t put = 0;
                while (put < got) {
                    ssize_t w = ::write(fd, mbuf + put, static_cast<size_t>(got - put));
                    if (w <= 0) {
                        break;
                    }
                    put += w;
                }
                if (put < got) {
                    break; // output file write error; stop copying
                }
            }
            ::close(mfd);
        }
        ::close(fd);
    }

    ::_exit(128 + signo);
}

/// @brief Signals armed by install().
int g_armed_signals[] = {SIGSEGV, SIGABRT, SIGFPE, SIGBUS, SIGILL};

/// @brief Alternate stack storage; allocated once, leaked (process lifetime).
char* g_altstack_mem = nullptr;

#endif // defined(__unix__) || defined(__linux__)
} // namespace

namespace cf::crash {

#if defined(__unix__) || defined(__linux__)
bool installSignalHandlers(const std::string& dir, const std::string& logger_path) {
    std::strncpy(g_crashes_dir, dir.c_str(), sizeof(g_crashes_dir) - 1U);
    g_crashes_dir[sizeof(g_crashes_dir) - 1U] = '\0';
    g_dir_set = true;
    std::strncpy(g_logger_path, logger_path.c_str(), sizeof(g_logger_path) - 1U);
    g_logger_path[sizeof(g_logger_path) - 1U] = '\0';
    g_logger_set = !logger_path.empty();

    // Allocate an alternate stack so a stack overflow (SIGSEGV) still has room
    // to run the handler. Leaked on purpose: the handler needs it until exit.
    g_altstack_mem = static_cast<char*>(std::malloc(kAltStackSize));
    if (g_altstack_mem != nullptr) {
        stack_t ss{};
        ss.ss_sp = g_altstack_mem;
        ss.ss_size = kAltStackSize;
        ss.ss_flags = 0;
        ::sigaltstack(&ss, nullptr);
    }

    struct sigaction sa{};
    sa.sa_handler = cfCrashHandleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND | SA_ONSTACK;
    for (int s : g_armed_signals) {
        ::sigaction(s, &sa, nullptr);
    }
    return true;
}

void uninstallSignalHandlers() {
    for (int s : g_armed_signals) {
        struct sigaction sa{};
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        ::sigaction(s, &sa, nullptr);
    }
}
#else
bool installSignalHandlers(const std::string&, const std::string&) {
    // Phase 1 targets Linux. SetUnhandledExceptionFilter / std::set_terminate
    // land here in a later phase.
    return false;
}

void uninstallSignalHandlers() {}
#endif

} // namespace cf::crash
