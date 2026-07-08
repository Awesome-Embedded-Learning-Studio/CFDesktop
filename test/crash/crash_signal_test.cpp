/**
 * @file    crash_signal_test.cpp
 * @brief   End-to-end tests: forked children arm the handler then crash for
 *          real (raised signal, null-pointer deref, abort, stack overflow).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_handler.h"
#include "crash_test_util.h"

#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <gtest/gtest.h>

#if defined(__linux__)
#    include <sys/wait.h>
#    include <unistd.h>
#endif

namespace fs = std::filesystem;

#if defined(__linux__)
namespace {
/// @brief Infinite recursion that touches a stack buffer each frame so the
///        compiler cannot tail-call it into a loop. Used to exhaust the stack
///        and prove sigaltstack keeps the handler runnable.
[[noreturn]] void crashRecurse() {
    volatile char buf[8192];
    buf[0] = static_cast<char>('x'); // touch + defeat tail-call optimization
    (void)buf;
    crashRecurse();
}

/// @brief Forks a child that arms the handler, runs @p crash (which must not
///        return), and verifies the child exits 128+@p signo and leaves a
///        @c .pending snapshot. Asserts the handler _exit()ed rather than the
///        process being raw-killed by the signal (which would show as
///        WIFSIGNALED) -- the latter would mean the handler never ran.
void expectCrashCaught(std::function<void()> crash, int signo) {
    const auto dir = crash_test::uniqueDir("cfcrash_signal");

    const pid_t pid = ::fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        // Child: arm the handler, then trigger the fault. The handler writes
        // .pending and _exit(128+signo) -- the child never returns normally.
        cf::crash::CrashHandler::instance().install(dir.string());
        crash();
        ::_exit(0); // unreachable if crash() faults
    }

    int status = 0;
    ASSERT_GT(::waitpid(pid, &status, 0), -1);
    ASSERT_TRUE(WIFEXITED(status)) << "handler should _exit(); raw WIFSIGNALED means it never ran";
    EXPECT_EQ(WEXITSTATUS(status), 128 + signo);

    bool found_pending = false;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".pending") {
            found_pending = true;
        }
    }
    EXPECT_TRUE(found_pending) << "crash handler did not write a .pending file";

    fs::remove_all(dir);
}
} // namespace

TEST(CrashSignal, RaisedSignalIsCaught) {
    expectCrashCaught([] { ::raise(SIGSEGV); }, SIGSEGV);
}

TEST(CrashSignal, NullPointerDereferenceIsCaught) {
    expectCrashCaught(
        [] {
            // Real MMU fault: write through a null pointer. volatile keeps the
            // store from being optimized away as dead UB.
            volatile int* p = nullptr;
            *p = 42;
        },
        SIGSEGV);
}

TEST(CrashSignal, AbortCallIsCaught) {
    expectCrashCaught([] { std::abort(); }, SIGABRT);
}

TEST(CrashSignal, StackOverflowIsCaughtViaAltStack) {
    // Stack exhaustion -> SIGSEGV with the main stack full. Without
    // sigaltstack the handler could not run and the child would be raw-killed
    // (WIFSIGNALED). Passing here proves the alternate stack works.
    expectCrashCaught([] { crashRecurse(); }, SIGSEGV);
}
#else
TEST(CrashSignal, DisabledOnNonLinux) {
    GTEST_SKIP() << "fork-based crash tests are Linux-only (Phase 1 focus)";
}
#endif
