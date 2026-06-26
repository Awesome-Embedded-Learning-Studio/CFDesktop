# ============================================================
#  custom_target_helper.cmake
#  Windows Qt auto-deploy support for executable targets.
#
#  Features:
#    - Generates deploy script using QT_DEPLOY_SUPPORT
#    - Creates deploy_<target> custom target (manual trigger)
#    - Optional AUTO_DEPLOY: runs windeployqt after every build
#    - Optional INSTALL_DEPLOY: runs deploy during cmake --install
#
#  Usage:
#      qt_add_windows_deploy(target_name
#          [AUTO_DEPLOY]      # auto run after build
#          [INSTALL_DEPLOY]   # run deploy during install
#      )
#
# ============================================================

if(NOT WIN32)
    function(qt_add_windows_deploy target_name)
        message(STATUS "qt_add_windows_deploy skipped (non-Windows platform)")
    endfunction()
    return()
endif()

if(NOT DEFINED QT_DEPLOY_SUPPORT)
    message(FATAL_ERROR
        "QT_DEPLOY_SUPPORT is not defined.\n"
        "Make sure find_package(Qt6 REQUIRED COMPONENTS Core) is called before including this file."
    )
endif()

function(qt_add_windows_deploy target_name)

    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Target '${target_name}' does not exist.")
    endif()

    set(options AUTO_DEPLOY INSTALL_DEPLOY)
    cmake_parse_arguments(QT_DEPLOY "${options}" "" "" ${ARGN})

    # --------------------------------------------
    # Generate deploy script
    # qt_generate_deploy_app_script 输出的是一个
    # .cmake 脚本的路径，需要用 cmake -P 执行
    # --------------------------------------------
    qt_generate_deploy_app_script(
        TARGET ${target_name}
        OUTPUT_SCRIPT _deploy_script          # 脚本路径存入此变量
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    message(STATUS "Deploy script for '${target_name}': ${_deploy_script}")

    set(_locked_deploy_script
        "${CMAKE_BINARY_DIR}/.qt/locked_deploy_${target_name}_$<CONFIG>.cmake")
    file(GENERATE
        OUTPUT "${_locked_deploy_script}"
        CONTENT
"file(LOCK \"${CMAKE_BINARY_DIR}/.qt/windeployqt.lock\" GUARD PROCESS TIMEOUT 300)
set(QT_DEPLOY_PREFIX \"$<TARGET_FILE_DIR:${target_name}>\")
set(QT_DEPLOY_BIN_DIR \".\")
set(QT_DEPLOY_LIB_DIR \".\")
set(QT_DEPLOY_PLUGINS_DIR \"plugins\")
set(QT_DEPLOY_QML_DIR \"qml\")
include(\"${_deploy_script}\")
"
    )

    add_custom_target(deploy_${target_name}
        COMMAND ${CMAKE_COMMAND}
            --install "${CMAKE_BINARY_DIR}"
            --config $<CONFIG>
        DEPENDS ${target_name}
        COMMENT "Deploying Qt dependencies for '${target_name}'..."
        VERBATIM
    )

    if(QT_DEPLOY_AUTO_DEPLOY)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -P "${_locked_deploy_script}"
            COMMENT "Auto-deploying Qt dependencies for '${target_name}'..."
            VERBATIM
        )
        message(STATUS "[${target_name}] AUTO_DEPLOY enabled: will run after every build")
    endif()

    if(QT_DEPLOY_INSTALL_DEPLOY)
        install(SCRIPT "${_deploy_script}")
        message(STATUS "[${target_name}] INSTALL_DEPLOY enabled: will run during cmake --install")
    endif()

endfunction()
