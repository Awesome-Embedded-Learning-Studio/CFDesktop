# ============================================================
# Windows Example Launcher Generator
# ============================================================
# 为每个 example 可执行文件生成 Windows 启动脚本 (.ps1)
#
# 功能:
#   - 自动生成 .ps1 启动脚本
#   - 设置 PATH 指向共享的 runtimes/ 目录
#   - 设置 QT_PLUGIN_PATH
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
    set(PS1_NAME "${TARGET_NAME}.ps1")

    # 启动脚本内容
    set(LAUNCHER_CONTENT "# Launcher for ${TARGET_NAME}
# This script sets up the PATH to find shared Qt DLLs in ../../runtimes/

\$ScriptDir = Split-Path -Parent \$MyInvocation.MyCommand.Path
# From examples/{category}/ go up two levels to reach build_develop/, then into runtimes/
\$RuntimesDir = Join-Path \$ScriptDir \"..\\..\\runtimes\" -Resolve

Write-Host \"[Launcher] ScriptDir: \$ScriptDir\" -ForegroundColor Cyan
Write-Host \"[Launcher] RuntimesDir: \$RuntimesDir\" -ForegroundColor Cyan
Write-Host \"[Launcher] Exe: ${EXE_NAME}\" -ForegroundColor Cyan

if (-not (Test-Path \$RuntimesDir)) {
    Write-Host \"[Launcher] ERROR: RuntimesDir not found!\" -ForegroundColor Red
    exit 1
}

# Clear Qt environment to avoid conflicts
\$env:QT_PLUGIN_PATH = \$null
\$env:QML2_IMPORT_PATH = \$null

# Set PATH with runtimes FIRST to avoid loading wrong DLLs
\$env:PATH=\"\$RuntimesDir;\$env:PATH\"

# List Qt DLLs being loaded
Write-Host \"[Launcher] Qt DLLs in runtimes:\" -ForegroundColor Cyan
Get-ChildItem \$RuntimesDir -Filter \"Qt6*.dll\" | ForEach-Object { Write-Host \"  \$($_.Name)\" }

# Set Qt plugin path
\$env:QT_PLUGIN_PATH=\"\$RuntimesDir\"

Write-Host \"[Launcher] Starting executable...\" -ForegroundColor Green

# Launch the executable
\$exePath = Join-Path \$ScriptDir \"${EXE_NAME}\"
Write-Host \"[Launcher] Exe path: \$exePath\" -ForegroundColor Cyan

Start-Process -FilePath \$exePath -ArgumentList \$args -Wait
Write-Host \"[Launcher] Exit code: \$LASTEXITCODE\" -ForegroundColor Yellow
")

    # 写入 .ps1 文件
    file(WRITE "${OUTPUT_DIR}/${PS1_NAME}" "${LAUNCHER_CONTENT}")

    log_info("Launcher" "Generated: ${OUTPUT_DIR}/${PS1_NAME}")
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
