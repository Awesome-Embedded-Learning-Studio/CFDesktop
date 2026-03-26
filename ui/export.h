/**
 * @file export.h
 * @brief CF UI Library export/import macros
 *
 * Defines CF_UI_EXPORT macro for controlling symbol visibility in shared libraries.
 * - When building cfui.dll/so, symbols are exported
 * - When using cfui.dll/so, symbols are imported
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

/**
 * @def CF_UI_EXPORT
 * @brief Export/import macro for CF UI Library symbols
 *
 * Usage:
 *   class CF_UI_EXPORT MyClass { ... };
 *
 *   CF_UI_EXPORT void myFunction();
 *
 * Platform behavior:
 * - Windows: Uses __declspec(dllexport/dllimport)
 * - Linux/GCC: Uses __attribute__((visibility("default")))
 *
 * CMake automatically defines CFUI_EXPORTS when building the shared library.
 * This follows the standard CMake convention: <target>_EXPORTS is defined
 * when compiling sources that are part of a SHARED library target.
 */

#if defined(_WIN32) || defined(_MSC_VER)
#    ifdef CFUI_STATIC_BUILD
#        define CF_UI_EXPORT
#    elif defined(CFUI_EXPORTS)
#        define CF_UI_EXPORT __declspec(dllexport)
#    else
#        define CF_UI_EXPORT __declspec(dllimport)
#    endif
#else
// Linux/Unix: same attribute for both building and using
#    define CF_UI_EXPORT __attribute__((visibility("default")))
#endif
