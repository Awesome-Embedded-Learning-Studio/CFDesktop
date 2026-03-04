# ============================================================
# Qt Unified Deployment Utilities (Shared DLLs)
# ============================================================
# 提供动态注册机制，让子项目自动注册到统一部署列表
#
# 新架构: 共享 Qt DLLs 部署到 runtimes/ 目录
# - 节省磁盘空间 (13 个 examples 从 ~1.3GB 优化到 ~100MB)
# - 启动脚本配置 PATH 指向 runtimes/
# - 插件部署到 plugins/ 目录
# ============================================================

# 定义一个全局列表变量，用于收集所有需要部署的可执行文件
set(QT_DEPLOY_EXECUTABLES "" CACHE INTERNAL "List of executables for Qt deployment")

# ============================================================
# 宏: cf_register_qt_deployment (已弃用，保留兼容性)
# ============================================================
# 注意: 不再需要单独注册，使用共享部署
# 此宏仅为向后兼容保留，实际不再使用
#
macro(cf_register_qt_deployment TARGET_NAME)
    # 不再需要单独注册，使用共享部署
    # 保留此宏以兼容现有代码
endmacro()

# ============================================================
# 函数: cf_setup_shared_qt_deployment
# ============================================================
# 设置共享的 Qt 运行时部署
# 将 Qt DLLs 部署到共享的 runtimes/ 目录
#
# 必须在所有子项目 add_subdirectory 之后调用
#
function(cf_setup_shared_qt_deployment)
    if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
        # 找到 windeployqt
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt
            HINTS "${CMAKE_PREFIX_DIR}/bin" "${_qt_install_prefix}/bin"
        )

        if(WINDEPLOYQT_EXECUTABLE)
            # 共享运行时目录
            set(RUNTIMES_DIR "${CMAKE_BINARY_DIR}/runtimes")
            set(PLUGINS_DIR "${CMAKE_BINARY_DIR}/plugins")
            set(DEPLOY_MARKER "${RUNTIMES_DIR}/.deploy_done")

            # 创建目录
            file(MAKE_DIRECTORY "${RUNTIMES_DIR}")
            file(MAKE_DIRECTORY "${PLUGINS_DIR}")

            message(STATUS "Setting up shared Qt deployment:")
            message(STATUS "  Runtimes -> ${RUNTIMES_DIR}")
            message(STATUS "  Plugins  -> ${PLUGINS_DIR}")

            # 找到第一个可执行文件作为 windeployqt 入口
            # 优先使用 examples/gui 下的主 gallery
            set(DEPLOY_EXE "")

            # 按优先级查找可执行文件
            if(TARGET material_gallery)
                set(DEPLOY_EXE "$<TARGET_FILE:material_gallery>")
            elseif(TARGET button_example)
                set(DEPLOY_EXE "$<TARGET_FILE:button_example>")
            elseif(EXISTS "${CMAKE_BINARY_DIR}/examples/gui")
                # 查找 examples/gui 下的第一个可执行文件
                file(GLOB EXE_FILES "${CMAKE_BINARY_DIR}/examples/gui/*.exe")
                if(EXE_FILES)
                    list(GET EXE_FILES 0 DEPLOY_EXE)
                endif()
            endif()

            # 如果没找到，查找 examples 下的任意可执行文件
            if(NOT DEPLOY_EXE)
                file(GLOB_RECURSE EXE_FILES "${CMAKE_BINARY_DIR}/examples/*.exe")
                if(EXE_FILES)
                    list(GET EXE_FILES 0 DEPLOY_EXE)
                endif()
            endif()

            if(DEPLOY_EXE)
                # 创建自定义部署 target
                add_custom_target(deploy_qt_shared)

                # 执行 windeployqt，部署到 runtimes/ 目录
                add_custom_command(TARGET deploy_qt_shared POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E env QT_HASH_SEED=0
                    "${WINDEPLOYQT_EXECUTABLE}"
                        --dir "${RUNTIMES_DIR}"
                        --no-compiler-runtime
                        --no-translations
                        --no-system-d3d-compiler
                        --no-opengl-sw
                        --verbose 1
                        --plugdir "${PLUGINS_DIR}"
                    "${DEPLOY_EXE}"
                    COMMAND ${CMAKE_COMMAND} -E touch "${DEPLOY_MARKER}"
                    COMMENT "Deploying shared Qt DLLs to ${RUNTIMES_DIR}"
                    VERBATIM
                )

                message(STATUS "  Deploy target: deploy_qt_shared")
            else()
                message(WARNING "No executable found for windeployqt, skipping Qt deployment")
            endif()

        else()
            message(WARNING "windeployqt not found, skipping automatic Qt deployment")
        endif()
    endif()
endfunction()

# ============================================================
# 函数: cf_setup_unified_qt_deployment (兼容旧接口)
# ============================================================
# 保持向后兼容，实际调用新的共享部署函数
#
function(cf_setup_unified_qt_deployment)
    cf_setup_shared_qt_deployment()
endfunction()
