# ============================================================
# Windows Example Launcher Generator
# ============================================================
# 为每个 example 可执行文件生成 Windows 启动脚本 (.bat)
#
# 功能:
#   - 自动生成 .bat 启动脚本
#   - 设置 PATH 指向共享的 runtimes/ 目录
#   - 提供环境隔离保护
#   - Linux 下跳过
# ============================================================

# 全局列表：收集需要生成启动脚本的可执行文件
set(LAUNCHER_EXECUTABLES "" CACHE INTERNAL "List of executables needing launcher scripts")

# ============================================================
# 函数: cf_register_example_launcher
# ============================================================
# 注册一个可执行文件，为其生成启动脚本
#
# 参数:
#   TARGET_NAME - 目标名称
#   CATEGORY    - 分类 (base, ui, gui)
#
# 用法:
#   cf_register_example_launcher(button_example "ui")
#
function(cf_register_example_launcher TARGET_NAME CATEGORY)
    if(WIN32)
        list(APPEND LAUNCHER_EXECUTABLES "${TARGET_NAME}|${CATEGORY}")
        set(LAUNCHER_EXECUTABLES "${LAUNCHER_EXECUTABLES}" CACHE INTERNAL "List of executables needing launcher scripts")
        log_info("Launcher" "Registered launcher for '${TARGET_NAME}' (${CATEGORY})")
    endif()
endfunction()

# ============================================================
# 函数: cf_generate_launcher_script
# ============================================================
# 为单个可执行文件生成启动脚本
#
# 参数:
#   TARGET_NAME  - 目标名称
#   CATEGORY     - 分类
#   OUTPUT_DIR   - 输出目录
#
function(cf_generate_launcher_script TARGET_NAME CATEGORY OUTPUT_DIR)
    if(NOT WIN32)
        return()
    endif()

    # 获取可执行文件名 (带扩展名)
    set(EXE_NAME "${TARGET_NAME}.exe")
    set(BAT_NAME "${TARGET_NAME}.bat")

    # 启动脚本内容
    set(LAUNCHER_CONTENT "@echo off
REM Launcher for ${TARGET_NAME}
REM This script sets up the PATH to find shared Qt DLLs in ../runtimes/

setlocal

REM Get the directory where this script is located
set \"SCRIPT_DIR=%~dp0\"

REM Add runtimes directory to PATH (relative to script location)
REM The runtimes/ directory is at the same level as examples/
set \"PATH=%SCRIPT_DIR%..\\runtimes;%PATH%\"

REM Launch the executable
REM %* passes all command line arguments to the executable
start \"\" \"%SCRIPT_DIR%%EXE_NAME%\" %*\

endlocal")

    # 写入 .bat 文件
    file(WRITE "${OUTPUT_DIR}/${BAT_NAME}" "${LAUNCHER_CONTENT}")

    log_info("Launcher" "Generated: ${OUTPUT_DIR}/${BAT_NAME}")
endfunction()

# ============================================================
# 函数: cf_generate_all_launchers
# ============================================================
# 为所有注册的可执行文件生成启动脚本
#
# 必须在所有 add_subdirectory 之后调用
#
function(cf_generate_all_launchers)
    if(NOT WIN32 OR NOT LAUNCHER_EXECUTABLES)
        return()
    endif()

    message(STATUS "Generating Windows launcher scripts...")

    foreach(LAUNCHER_ENTRY ${LAUNCHER_EXECUTABLES})
        # 分割 "TARGET_NAME|CATEGORY" 格式
        string(REPLACE "|" ";" LAUNCHER_LIST "${LAUNCHER_ENTRY}")
        list(GET LAUNCHER_LIST 0 TARGET_NAME)
        list(GET LAUNCHER_LIST 1 CATEGORY)

        # 获取输出目录
        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples/${CATEGORY}")

        # 生成启动脚本
        cf_generate_launcher_script("${TARGET_NAME}" "${CATEGORY}" "${OUTPUT_DIR}")
    endforeach()

    message(STATUS "Generated ${CMAKE_MATCH_COUNT} launcher scripts")
endfunction()

# ============================================================
# 宏: cf_add_example_with_launcher
# ============================================================
# 便捷宏：创建 example 并自动注册启动脚本
#
# 参数:
#   TARGET_NAME - 目标名称
#   CATEGORY    - 分类 (base, ui, gui)
#   SOURCES     - 源文件列表
#
# 用法:
#   cf_add_example_with_launcher(my_example "ui" main.cpp widget.cpp)
#   target_link_libraries(my_example PRIVATE cfui Qt6::Widgets)
#
macro(cf_add_example_with_launcher TARGET_NAME CATEGORY)
    # 创建可执行文件
    add_executable(${TARGET_NAME} ${ARGN})

    # 设置输出目录
    cf_set_example_output_dir(${TARGET_NAME} ${CATEGORY})

    # 注册启动脚本
    cf_register_example_launcher(${TARGET_NAME} ${CATEGORY})

    # Windows 特定设置
    if(WIN32)
        set_target_properties(${TARGET_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endmacro()
