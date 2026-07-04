/**
 * @file    window_manager_test.cpp
 * @brief   GoogleTest unit tests for the WindowManager state machine.
 *
 * Drives minimize/maximize/restore through a FakeWindow (no backend, no
 * display) and asserts: the IWindow operation is dispatched (counter
 * incremented), the stored WindowInfo state transitions correctly, the
 * transition table rejects invalid moves, queries behave on known and unknown
 * ids, and QObject::destroyed emits the Closed transition before the entry is
 * erased. When Qt6::Test is available, signal emission is also verified with
 * QSignalSpy.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-05
 * @version 0.1
 * @since   0.21
 * @ingroup components
 */

#include "WindowManager.h"
#include "fake_window.h"
#include "window_info.h"

#include <gtest/gtest.h>

#ifdef QT_TEST_AVAILABLE
#    include <QSignalSpy>
#endif

#include <memory>

// WindowState is an enum class; QSignalSpy needs a metatype to capture it.
Q_DECLARE_METATYPE(cf::desktop::WindowState)

namespace {

using cf::desktop::IWindow;
using cf::desktop::win_id_t;
using cf::desktop::WindowInfo;
using cf::desktop::WindowManager;
using cf::desktop::WindowState;
using cf::desktop::test::FakeWindow;

/// @brief Owns a WindowManager plus one registered FakeWindow for a test.
struct FakeFixture {
    WindowManager wm;
    std::unique_ptr<FakeWindow> window;

    FakeFixture() {
        window = std::make_unique<FakeWindow>("win-1", "Title", QRect(10, 20, 800, 600), 4242);
        wm.registerWindowForTest(window->make_weak());
    }
};

} // namespace

// ── Registration populates the data model ──────────────────────────────────

TEST(WindowManager, Register_PopulatesWindowInfo) {
    FakeFixture f;
    const WindowInfo info = f.wm.getWindowInfo("win-1");
    EXPECT_EQ(info.window_id, "win-1");
    EXPECT_EQ(info.title, "Title");
    EXPECT_EQ(info.pid, 4242);
    EXPECT_EQ(info.state, WindowState::Normal);
    EXPECT_TRUE(info.created_at.isValid());
}

// ── Happy-path state transitions ───────────────────────────────────────────

TEST(WindowManager, Minimize_Normal_DispatchesAndUpdatesState) {
    FakeFixture f;
    EXPECT_TRUE(f.wm.minimizeWindow("win-1"));
    EXPECT_EQ(f.window->minimize_calls, 1);
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Minimized);
}

TEST(WindowManager, Maximize_Normal_DispatchesAndUpdatesState) {
    FakeFixture f;
    EXPECT_TRUE(f.wm.maximizeWindow("win-1"));
    EXPECT_EQ(f.window->maximize_calls, 1);
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Maximized);
}

TEST(WindowManager, Restore_Minimized_DispatchesAndUpdatesState) {
    FakeFixture f;
    ASSERT_TRUE(f.wm.minimizeWindow("win-1"));
    EXPECT_TRUE(f.wm.restoreWindow("win-1"));
    EXPECT_EQ(f.window->restore_calls, 1);
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Normal);
}

// ── Invalid transitions are rejected, no dispatch, no state change ─────────

TEST(WindowManager, Minimize_OnMinimized_Rejected_NoDispatch) {
    FakeFixture f;
    ASSERT_TRUE(f.wm.minimizeWindow("win-1"));
    EXPECT_FALSE(f.wm.minimizeWindow("win-1")); // already Minimized
    EXPECT_EQ(f.window->minimize_calls, 1);     // not dispatched again
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Minimized);
}

TEST(WindowManager, Maximize_FromMinimized_Rejected_NoDispatch) {
    FakeFixture f;
    ASSERT_TRUE(f.wm.minimizeWindow("win-1"));
    EXPECT_FALSE(f.wm.maximizeWindow("win-1")); // must restore first
    EXPECT_EQ(f.window->maximize_calls, 0);
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Minimized);
}

TEST(WindowManager, StateOp_OnUnknownId_ReturnsFalse) {
    FakeFixture f;
    EXPECT_FALSE(f.wm.minimizeWindow("nope"));
    EXPECT_FALSE(f.wm.maximizeWindow("nope"));
    EXPECT_FALSE(f.wm.restoreWindow("nope"));
    EXPECT_EQ(f.window->minimize_calls, 0);
}

// ── Queries ────────────────────────────────────────────────────────────────

TEST(WindowManager, GetWindowInfo_UnknownId_ReturnsClosed) {
    FakeFixture f;
    const auto gone = f.wm.getWindowInfo("missing");
    EXPECT_EQ(gone.state, WindowState::Closed);
    EXPECT_TRUE(gone.window_id.isEmpty());
}

TEST(WindowManager, FindWindowByPid_ReturnsIdOrNullopt) {
    FakeFixture f;
    EXPECT_EQ(f.wm.findWindowByPid(4242), std::optional<win_id_t>("win-1"));
    EXPECT_FALSE(f.wm.findWindowByPid(9999).has_value());
    EXPECT_FALSE(f.wm.findWindowByPid(0).has_value()); // 0 is never matched
}

TEST(WindowManager, GetAllWindowInfos_ReturnsAllRegistered) {
    WindowManager wm;
    FakeWindow a{"a", "A", QRect(0, 0, 100, 100), 10};
    FakeWindow b{"b", "B", QRect(0, 0, 100, 100), 20};
    wm.registerWindowForTest(a.make_weak());
    wm.registerWindowForTest(b.make_weak());
    const auto all = wm.getAllWindowInfos();
    EXPECT_EQ(all.size(), 2);
}

// ── Destroyed observation emits Closed then disappears ─────────────────────

TEST(WindowManager, Destroyed_EmitsClosedTransitionThenErases) {
    FakeFixture f;
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Normal);
    f.window.reset(); // destroy the FakeWindow -> QObject::destroyed fires
    EXPECT_EQ(f.wm.getWindowInfo("win-1").state, WindowState::Closed);
    EXPECT_FALSE(f.wm.findWindowByPid(4242).has_value());
}

// ── Signal emission (only when Qt6::Test is available) ─────────────────────

#ifdef QT_TEST_AVAILABLE
TEST(WindowManager, Minimize_EmitsWindowStateChangedAndInfoUpdated) {
    qRegisterMetaType<cf::desktop::WindowState>("cf::desktop::WindowState");
    FakeFixture f;
    QSignalSpy state_changed(&f.wm, &WindowManager::windowStateChanged);
    QSignalSpy info_updated(&f.wm, &WindowManager::windowInfoUpdated);
    ASSERT_TRUE(f.wm.minimizeWindow("win-1"));
    EXPECT_EQ(state_changed.count(), 1);
    EXPECT_EQ(state_changed.takeFirst().at(1).value<cf::desktop::WindowState>(),
              WindowState::Minimized);
    EXPECT_EQ(info_updated.count(), 1);
}

TEST(WindowManager, RejectedTransition_EmitsNothing) {
    qRegisterMetaType<cf::desktop::WindowState>("cf::desktop::WindowState");
    FakeFixture f;
    QSignalSpy state_changed(&f.wm, &WindowManager::windowStateChanged);
    EXPECT_FALSE(f.wm.minimizeWindow("missing"));
    EXPECT_EQ(state_changed.count(), 0);
}

TEST(WindowManager, Destroyed_EmitsClosedSignal) {
    qRegisterMetaType<cf::desktop::WindowState>("cf::desktop::WindowState");
    FakeFixture f;
    QSignalSpy state_changed(&f.wm, &WindowManager::windowStateChanged);
    QSignalSpy disappeared(&f.wm, &WindowManager::windowDisappeared);
    f.window.reset();
    EXPECT_EQ(state_changed.count(), 1);
    EXPECT_EQ(state_changed.takeFirst().at(1).value<cf::desktop::WindowState>(),
              WindowState::Closed);
    EXPECT_EQ(disappeared.count(), 1);
    EXPECT_EQ(disappeared.takeFirst().at(0).value<qint64>(), 4242);
}
#endif