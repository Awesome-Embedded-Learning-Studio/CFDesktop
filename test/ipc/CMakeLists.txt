# IPC module unit tests using GoogleTest
include(add_gtest_executable)

log_info("ipc_tests" "Configured IPC unit tests:")

add_gtest_executable(
    TEST_NAME      ipc_message_test
    SOURCE_FILE    ipc_message_test.cpp
    LINK_LIBRARIES cfipc;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "ipc;unit"
    LOG_MODULE     ipc_tests
)

add_gtest_executable(
    TEST_NAME      ipc_registry_test
    SOURCE_FILE    ipc_registry_test.cpp
    LINK_LIBRARIES cfipc;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "ipc;unit"
    LOG_MODULE     ipc_tests
)

add_gtest_executable(
    TEST_NAME      ipc_service_locator_test
    SOURCE_FILE    ipc_service_locator_test.cpp
    LINK_LIBRARIES cfipc;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "ipc;unit"
    LOG_MODULE     ipc_tests
)

add_gtest_executable(
    TEST_NAME      ipc_notify_test
    SOURCE_FILE    ipc_notify_test.cpp
    LINK_LIBRARIES cfipc;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "ipc;unit;notify"
    LOG_MODULE     ipc_tests
)

# Transport test owns its main() (QCoreApplication for QLocalSocket),
# so it links gtest (not gtest_main) and adds Qt6::Network.
add_gtest_executable(
    TEST_NAME      ipc_transport_test
    SOURCE_FILE    ipc_transport_test.cpp
    LINK_LIBRARIES cfipc;Qt6::Core;Qt6::Network;GTest::gtest
    LABELS         "ipc;unit;transport"
    LOG_MODULE     ipc_tests
)
