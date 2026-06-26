# FrostedBackdrop unit tests.
# Custom main() (no gtest_main) to spin up an offscreen QGuiApplication so
# QPixmap/QPainter work in headless / CI environments.
add_executable(frosted_backdrop_test
    frosted_backdrop_test.cpp
)

target_include_directories(frosted_backdrop_test PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/ui/components
)

target_link_libraries(frosted_backdrop_test PRIVATE
    cfdesktop_frosted_backdrop
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

set_target_properties(frosted_backdrop_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

add_test(NAME frosted_backdrop_test COMMAND frosted_backdrop_test)
set_tests_properties(frosted_backdrop_test PROPERTIES
    LABELS "desktop;unit;components"
    TIMEOUT 60
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("frosted_backdrop_tests" "  - frosted_backdrop_test")

if(WIN32)
    qt_add_windows_deploy(frosted_backdrop_test AUTO_DEPLOY INSTALL_DEPLOY)

    set(_CF_QT_OFFSCREEN_PLUGIN_DIR
        "$<TARGET_FILE_DIR:frosted_backdrop_test>/plugins/platforms")

    if(TARGET Qt6::QOffscreenIntegrationPlugin)
        add_custom_command(TARGET frosted_backdrop_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${_CF_QT_OFFSCREEN_PLUGIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:Qt6::QOffscreenIntegrationPlugin>"
                "${_CF_QT_OFFSCREEN_PLUGIN_DIR}"
            COMMENT "Copying Qt offscreen platform plugin for frosted_backdrop_test"
            VERBATIM
        )
    else()
        get_filename_component(_CF_QT_PLATFORMS_DIR
            "${Qt6Gui_DIR}/../../../plugins/platforms" ABSOLUTE)
        add_custom_command(TARGET frosted_backdrop_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${_CF_QT_OFFSCREEN_PLUGIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_CF_QT_PLATFORMS_DIR}/qoffscreen.dll"
                "${_CF_QT_OFFSCREEN_PLUGIN_DIR}"
            COMMENT "Copying Qt offscreen platform plugin for frosted_backdrop_test"
            VERBATIM
        )
    endif()

    set_tests_properties(frosted_backdrop_test PROPERTIES
        ENVIRONMENT
            "QT_QPA_PLATFORM=offscreen;QT_QPA_PLATFORM_PLUGIN_PATH=${CMAKE_BINARY_DIR}/test/bin/plugins/platforms"
    )
endif()
