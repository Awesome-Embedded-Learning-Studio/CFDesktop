/**
 * @file    crash_signal_test.cpp
 * @brief   End-to-end test: a forked child arms the handler and crashes.
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
#include <filesystem>
#include <gtest/gtest.h>

#if defined(__linux__)
#    include <sys/wait.h>
#    include <unistd.h>
#endif

namespace fs = std::filesystem;

#if defined(__linux__)
TEST(CrashSignal, HandlerWritesPendingOnSegv) {
    const auto dir = crash_test::uniqueDir("cfcrash_signal");

    const pid_t pid = ::fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        // Child: arm the handler, then crash. Handler writes .pending and
        // _exit(128+SIGSEGV) -- the child never returns from raise().
        cf::crash::CrashHandler::instance().install(dir.string());
        ::raise(SIGSEGV);
        ::_exit(0);
    }

    int status = 0;
    ASSERT_GT(::waitpid(pid, &status, 0), -1);
    ASSERT_TRUE(WIFEXITED(status)) << "child did not exit via _exit";
    EXPECT_EQ(WEXITSTATUS(status), 128 + SIGSEGV);

    bool found_pending = false;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".pending") {
            found_pending = true;
        }
    }
    EXPECT_TRUE(found_pending) << "crash handler did not write a .pending file";

    fs::remove_all(dir);
}
#else
TEST(CrashSignal, DisabledOnNonLinux) {
    GTEST_SKIP() << "fork+SIGSEGV end-to-end test is Linux-only (Phase 1 focus)";
}
#endif
