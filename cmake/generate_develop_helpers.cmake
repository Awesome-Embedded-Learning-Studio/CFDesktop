# Generate Development Environment Configuration Helpers
# Provides functions to generate IDE-specific configuration files

#[[.synopsis
    generate_vscode_clangd

    Generates VSCode clangd configuration in the build directory.

    This function creates a .vscode folder under the build directory
    with a settings.json file configured for clangd based on the
    current toolchain (GCC/LLVM).

    The generated configuration includes:
    - clangd.path: Points to clangd.exe in the Tools directory
    - clangd.arguments: Includes compile-commands-dir, query-driver, etc.

    Note: query-driver uses a glob pattern (Tools/*/bin/g++.exe) to
    cover both MinGW-GCC and LLVM-MinGW toolchains simultaneously,
    so the config remains valid when switching toolchains.

    Usage:
        generate_vscode_clangd()
#]]
function(generate_vscode_clangd)
    get_filename_component(COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(TOOLCHAIN_ROOT   "${COMPILER_BIN_DIR}"   DIRECTORY)
    get_filename_component(TOOLS_DIR        "${TOOLCHAIN_ROOT}"     DIRECTORY)

    string(TOLOWER "${TOOLCHAIN_ROOT}" TOOLCHAIN_ROOT_LOWER)

    set(CLANGD_PATH "")
    set(QUERY_DRIVER_GLOB "")

    # ------------------------------------------------------------------ #
    #  Windows Qt toolchain                                                #
    # ------------------------------------------------------------------ #
    if("${TOOLCHAIN_ROOT_LOWER}" MATCHES "llvm-mingw")
        set(CLANGD_PATH       "${COMPILER_BIN_DIR}/clangd.exe")
        set(QUERY_DRIVER_GLOB "${TOOLS_DIR}/*/bin/g++.exe")

    elseif("${TOOLCHAIN_ROOT_LOWER}" MATCHES "mingw")
        file(GLOB CLANGD_CANDIDATES "${TOOLS_DIR}/llvm-mingw*/bin/clangd.exe")
        if(NOT CLANGD_CANDIDATES)
            message(WARNING
                "generate_vscode_clangd: clangd.exe not found under "
                "${TOOLS_DIR}/llvm-mingw*/bin/ — skipping VSCode config generation.")
            return()
        endif()
        list(SORT CLANGD_CANDIDATES)
        list(GET  CLANGD_CANDIDATES -1 CLANGD_PATH)
        set(QUERY_DRIVER_GLOB "${TOOLS_DIR}/*/bin/g++.exe")

    # ------------------------------------------------------------------ #
    #  Linux / macOS system toolchain                                      #
    #  Compiler typically at /usr/bin/g++ or /usr/local/bin/g++           #
    #  or a distro clang at /usr/bin/clang++                              #
    # ------------------------------------------------------------------ #
    elseif("${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/usr" OR
           "${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/opt" OR
           "${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/home")

        # Use `which` to locate clangd — avoids find_program cache pitfalls.
        # Try versioned binaries first (clangd-17, clangd-16 …) then bare clangd.
        set(CLANGD_PATH "")
        foreach(_candidate clangd-17 clangd-16 clangd-15 clangd-14 clangd)
            execute_process(
                COMMAND which ${_candidate}
                OUTPUT_VARIABLE _WHICH_OUT
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(_WHICH_OUT)
                set(CLANGD_PATH "${_WHICH_OUT}")
                break()
            endif()
        endforeach()

        if(NOT CLANGD_PATH)
            message(WARNING
                "generate_vscode_clangd: clangd not found in PATH — "
                "install it with: sudo apt install clangd  /  brew install llvm")
            return()
        endif()

        set(QUERY_DRIVER_GLOB
            "/usr/bin/g++,/usr/bin/g++-*,/usr/bin/clang++,/usr/local/bin/g++,/usr/local/bin/clang++")
    else()
        message(WARNING
            "generate_vscode_clangd: unrecognised toolchain root '${TOOLCHAIN_ROOT}' "
            "— skipping VSCode config generation.")
        return()
    endif()

    # --- Normalise paths -----------------------------------------------
    cmake_path(CONVERT "${CLANGD_PATH}"       TO_CMAKE_PATH_LIST CLANGD_PATH_JSON       NORMALIZE)
    cmake_path(CONVERT "${QUERY_DRIVER_GLOB}" TO_CMAKE_PATH_LIST QUERY_DRIVER_GLOB_JSON NORMALIZE)
    cmake_path(CONVERT "${CMAKE_BINARY_DIR}"  TO_CMAKE_PATH_LIST CMAKE_BINARY_DIR_JSON  NORMALIZE)

    # --- Write settings.json -------------------------------------------
    set(VSCODE_DIR    "${CMAKE_BINARY_DIR}/.vscode")
    set(SETTINGS_JSON "${VSCODE_DIR}/settings.json")
    set(EXTENSION_JSON "${VSCODE_DIR}/extensions.json")
    file(MAKE_DIRECTORY "${VSCODE_DIR}")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/generate_develop_helpers/templates/vscode_settings.json.in"
        "${SETTINGS_JSON}"
        @ONLY
    )

    message(STATUS "Generated VSCode clangd configuration: ${SETTINGS_JSON}")
    message(STATUS "  clangd.path:  ${CLANGD_PATH_JSON}")
    message(STATUS "  query-driver: ${QUERY_DRIVER_GLOB_JSON}")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/generate_develop_helpers/templates/extensions.json.in"
        "${EXTENSION_JSON}"
        @ONLY
    )

    message(STATUS "Also Extensions OK")
endfunction()


#[[.synopsis
    generate_vscode_debug_config

    Generates VSCode tasks.json and launch.json for debugging.

    This function creates .vscode/tasks.json and .vscode/launch.json
    in the build directory, configured based on the current toolchain.

    Toolchain Detection:
    - LLVM-MinGW: uses lldb-vscode.exe
    - MinGW-GCC:   uses gdb.exe

    Usage:
        generate_vscode_debug_config()
#]]
function(generate_vscode_debug_config)
    get_filename_component(COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(TOOLCHAIN_ROOT   "${COMPILER_BIN_DIR}"   DIRECTORY)
    get_filename_component(TOOLS_DIR        "${TOOLCHAIN_ROOT}"     DIRECTORY)

    string(TOLOWER "${TOOLCHAIN_ROOT}" TOOLCHAIN_ROOT_LOWER)

    set(DEBUGGER_TYPE "")
    set(DEBUGGER_PATH "")

    # ------------------------------------------------------------------ #
    #  Windows Qt toolchain                                                #
    # ------------------------------------------------------------------ #
    if("${TOOLCHAIN_ROOT_LOWER}" MATCHES "llvm-mingw")
        set(DEBUGGER_TYPE "lldb")
        set(DEBUGGER_PATH "${COMPILER_BIN_DIR}/lldb-vscode.exe")

    elseif("${TOOLCHAIN_ROOT_LOWER}" MATCHES "mingw")
        set(DEBUGGER_TYPE "gdb")
        set(DEBUGGER_PATH "${COMPILER_BIN_DIR}/gdb.exe")

    # ------------------------------------------------------------------ #
    #  Linux / macOS system toolchain                                      #
    # ------------------------------------------------------------------ #
    elseif("${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/usr" OR
           "${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/opt" OR
           "${TOOLCHAIN_ROOT_LOWER}" MATCHES "^/home")

        # Detect compiler type and choose appropriate debugger
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(DEBUGGER_TYPE "lldb")
            find_program(DEBUGGER_PATH lldb-vscode)
            if(NOT DEBUGGER_PATH)
                find_program(DEBUGGER_PATH lldb)
            endif()
            # Fallback to standard paths if not found in PATH
            if(NOT DEBUGGER_PATH)
                set(DEBUGGER_PATHS "/usr/bin/lldb-vscode" "/usr/bin/lldb" "/usr/local/bin/lldb-vscode" "/usr/local/bin/lldb")
                foreach(path IN LISTS DEBUGGER_PATHS)
                    if(EXISTS "${path}")
                        set(DEBUGGER_PATH "${path}")
                        break()
                    endif()
                endforeach()
            endif()
        else()
            set(DEBUGGER_TYPE "gdb")
            find_program(DEBUGGER_PATH gdb)
            # Fallback to standard paths if not found in PATH
            if(NOT DEBUGGER_PATH)
                set(DEBUGGER_PATHS "/usr/bin/gdb" "/usr/local/bin/gdb")
                foreach(path IN LISTS DEBUGGER_PATHS)
                    if(EXISTS "${path}")
                        set(DEBUGGER_PATH "${path}")
                        break()
                    endif()
                endforeach()
            endif()
        endif()

        if(NOT DEBUGGER_PATH)
            message(WARNING
                "generate_vscode_debug_config: debugger not found in PATH or standard locations — "
                "install gdb or lldb")
            return()
        endif()
    else()
        message(WARNING
            "generate_vscode_debug_config: unrecognised toolchain root '${TOOLCHAIN_ROOT}' "
            "— skipping VSCode debug config generation.")
        return()
    endif()

    # Verify debugger exists (for Windows toolchain)
    if(NOT EXISTS "${DEBUGGER_PATH}")
        message(WARNING
            "generate_vscode_debug_config: debugger not found at '${DEBUGGER_PATH}' "
            "— skipping VSCode debug config generation.")
        return()
    endif()

    # --- Normalise paths -----------------------------------------------
    cmake_path(CONVERT "${DEBUGGER_PATH}"       TO_CMAKE_PATH_LIST DEBUGGER_PATH_JSON       NORMALIZE)
    cmake_path(CONVERT "${CMAKE_BINARY_DIR}"    TO_CMAKE_PATH_LIST CMAKE_BINARY_DIR_JSON    NORMALIZE)
    cmake_path(CONVERT "${CMAKE_CURRENT_SOURCE_DIR}" TO_CMAKE_PATH_LIST PROJECT_ROOT_JSON NORMALIZE)

    # --- Write tasks.json and launch.json --------------------------------
    set(VSCODE_DIR "${CMAKE_BINARY_DIR}/.vscode")
    set(TASKS_JSON "${VSCODE_DIR}/tasks.json")
    set(LAUNCH_JSON "${VSCODE_DIR}/launch.json")
    file(MAKE_DIRECTORY "${VSCODE_DIR}")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/generate_develop_helpers/templates/vscode_tasks.json.in"
        "${TASKS_JSON}"
        @ONLY
    )

    message(STATUS "Generated VSCode tasks.json: ${TASKS_JSON}")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/generate_develop_helpers/templates/vscode_launch.json.in"
        "${LAUNCH_JSON}"
        @ONLY
    )

    message(STATUS "Generated VSCode launch.json: ${LAUNCH_JSON}")
    message(STATUS "  Debugger: ${DEBUGGER_TYPE} (${DEBUGGER_PATH_JSON})")
endfunction()


#[[.synopsis
    add_generate_develop_helpers_target

    Adds a CMake target that generates all development helper configurations.

    This creates a target named 'generate-develop-helpers' that can be
    built to regenerate IDE configuration files.

    Usage:
        add_generate_develop_helpers_target()

    Then run:
        cmake --build out/build --target generate-develop-helpers
#]]
function(add_generate_develop_helpers_target)
    add_custom_target(generate-develop-helpers ALL
        COMMAND ${CMAKE_COMMAND}
            -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -D CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
            -D CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_develop_helpers.cmake
        COMMENT "Generating development environment helpers..."
        VERBATIM
    )

    add_custom_command(TARGET generate-develop-helpers POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Development helpers generated successfully!"
        COMMAND ${CMAKE_COMMAND} -E echo "  VSCode config: ${CMAKE_BINARY_DIR}/.vscode/settings.json"
        COMMAND ${CMAKE_COMMAND} -E echo ""
    )
endfunction()


# Allow this script to be run standalone via: cmake -P generate_develop_helpers.cmake
if(NOT DEFINED CMAKE_PROJECT_NAME AND NOT DEFINED PROJECT_NAME)
    foreach(_required_var CMAKE_CXX_COMPILER CMAKE_BINARY_DIR CMAKE_CURRENT_SOURCE_DIR)
        if(NOT DEFINED ${_required_var})
            message(FATAL_ERROR "${_required_var} must be set when running in script mode")
        endif()
    endforeach()

    generate_vscode_clangd()
    generate_vscode_debug_config()
endif()