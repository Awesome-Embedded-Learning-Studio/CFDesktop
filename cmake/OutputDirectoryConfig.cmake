# ============================================================
# Output Directory Configuration
# ============================================================
# 配置构建产物的输出目录，实现分类管理
#
# 目录结构:
#   bin/         - 主应用和共享库
#   examples/    - 示例程序 (按类型分类)
#   plugins/     - Qt 插件
#   resources/   - 资源文件
#   runtimes/    - Windows Qt 运行时 DLLs (共享)
# ============================================================

# 设置全局输出目录 (默认，可被覆盖)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ============================================================
# 兼容性宏: cf_register_example_launcher
# ============================================================
# 在非 Windows 平台上定义为空操作
# (在 Windows 上由 ExampleLauncher.cmake 提供实际实现)
if(NOT WIN32)
    macro(cf_register_example_launcher TARGET_NAME CATEGORY)
        # Linux 下不需要启动脚本，空操作
    endmacro()
endif()

# ============================================================
# 函数: cf_set_example_output_dir
# ============================================================
# 为 example 可执行文件设置分类输出目录
#
# 参数:
#   TARGET_NAME - 目标名称
#   CATEGORY    - 分类名称 (base, ui, gui)
#
# 用法:
#   cf_set_example_output_dir(my_example "ui")
#
function(cf_set_example_output_dir TARGET_NAME CATEGORY)
    if(NOT TARGET ${TARGET_NAME})
        message(WARNING "cf_set_example_output_dir: Target '${TARGET_NAME}' does not exist")
        return()
    endif()

    # 设置输出目录为 examples/{category}/
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/${CATEGORY}"
    )

    # 确保输出目录存在
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/examples/${CATEGORY}")

    log_info("OutputDir" "Target '${TARGET_NAME}' -> examples/${CATEGORY}")
endfunction()

# ============================================================
# 函数: cf_set_main_app_output_dir
# ============================================================
# 为主应用设置输出目录 (bin/)
#
# 参数:
#   TARGET_NAME - 主应用目标名称
#
function(cf_set_main_app_output_dir TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(WARNING "cf_set_main_app_output_dir: Target '${TARGET_NAME}' does not exist")
        return()
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    log_info("OutputDir" "Main app '${TARGET_NAME}' -> bin/")
endfunction()

# ============================================================
# 函数: cf_get_runtime_output_dir
# ============================================================
# 获取运行时输出目录 (用于 Qt 部署)
#
# 输出变量:
#   CF_RUNTIME_OUTPUT_DIR - 运行时输出目录
#
function(cf_get_runtime_output_dir VAR_NAME)
    set(${VAR_NAME} "${CMAKE_BINARY_DIR}/bin" PARENT_SCOPE)
endfunction()

# ============================================================
# 函数: cf_get_examples_output_dir
# ============================================================
# 获取 examples 输出目录
#
# 参数:
#   CATEGORY - 分类名称 (可选)
# 输出变量:
#   CF_EXAMPLES_OUTPUT_DIR - examples 输出目录
#
function(cf_get_examples_output_dir VAR_NAME CATEGORY)
    if(CATEGORY)
        set(${VAR_NAME} "${CMAKE_BINARY_DIR}/examples/${CATEGORY}" PARENT_SCOPE)
    else()
        set(${VAR_NAME} "${CMAKE_BINARY_DIR}/examples" PARENT_SCOPE)
    endif()
endfunction()

# ============================================================
# 函数: cf_get_runtimes_dir
# ============================================================
# 获取 runtimes 目录 (Windows Qt DLLs)
#
# 输出变量:
#   CF_RUNTIMES_DIR - runtimes 目录
#
function(cf_get_runtimes_dir VAR_NAME)
    set(${VAR_NAME} "${CMAKE_BINARY_DIR}/runtimes" PARENT_SCOPE)
endfunction()

# ============================================================
# 函数: cf_get_plugins_dir
# ============================================================
# 获取 plugins 目录 (Qt 插件)
#
# 输出变量:
#   CF_PLUGINS_DIR - plugins 目录
#
function(cf_get_plugins_dir VAR_NAME)
    set(${VAR_NAME} "${CMAKE_BINARY_DIR}/plugins" PARENT_SCOPE)
endfunction()

# ============================================================
# 函数: cf_configure_qt_plugin_paths
# ============================================================
# 配置 Qt 插件路径 (用于 windeployqt)
#
# 设置 QT_PLUGIN_PATH 环境变量，使 Qt 能找到插件
#
function(cf_configure_qt_plugin_paths)
    if(WIN32)
        # 设置 Qt 插件输出到 plugins/ 目录
        set(CMAKE_INSTALL_PLUGINSDIR "${CMAKE_BINARY_DIR}/plugins" CACHE PATH "")
    endif()
endfunction()

# ============================================================
# 函数: cf_setup_output_dirs
# ============================================================
# 创建所有输出目录
#
function(cf_setup_output_dirs)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/examples/base")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/examples/ui")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/examples/gui")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/resources")

    if(WIN32)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/runtimes")
    endif()

    log_info("OutputDir" "Output directories created")
endfunction()

# 自动创建输出目录
cf_setup_output_dirs()
