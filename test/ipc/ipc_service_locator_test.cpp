/**
 * @file    ipc_service_locator_test.cpp
 * @brief   Unit tests for the ServiceLocator typed registry.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/service_locator.h"

#include <gtest/gtest.h>
#include <memory>

namespace {
struct Foo {
    int v;
};
} // namespace

TEST(ServiceLocator, RoundTripsRegisteredInstance) {
    auto& loc = cf::ipc::ServiceLocator::instance();
    loc.clear();
    loc.registerService<Foo>("foo", std::make_shared<Foo>(Foo{42}));

    const auto got = loc.resolve<Foo>("foo");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->v, 42);
}

TEST(ServiceLocator, ReturnsNullForUnknownName) {
    auto& loc = cf::ipc::ServiceLocator::instance();
    loc.clear();
    EXPECT_EQ(loc.resolve<Foo>("missing"), nullptr);
}

TEST(ServiceLocator, ReRegisterReplacesInstance) {
    auto& loc = cf::ipc::ServiceLocator::instance();
    loc.clear();
    loc.registerService<Foo>("foo", std::make_shared<Foo>(Foo{1}));
    loc.registerService<Foo>("foo", std::make_shared<Foo>(Foo{99}));
    EXPECT_EQ(loc.resolve<Foo>("foo")->v, 99);
}
