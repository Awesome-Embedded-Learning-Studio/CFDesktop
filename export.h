/**
 * @file export.h
 * @brief CF Desktop Library export/import macros
 *
 * Defines CF_DESKTOP_EXPORT macro for controlling symbol visibility in shared libraries.
 * - When building CFDesktop.dll/so, symbols are exported
 * - When using CFDesktop.dll/so, symbols are imported
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

/**
 * @def CF_DESKTOP_EXPORT
 * @brief Export/import macro for CF Desktop Library symbols
 *
 * Usage:
 *   class CF_DESKTOP_EXPORT MyClass { ... };
 *
 *   CF_DESKTOP_EXPORT void myFunction();
 *
 * Platform behavior:
 * - Windows: Uses __declspec(dllexport/dllimport)
 * - Linux/GCC: Uses __attribute__((visibility("default")))
 *
 * CMake automatically defines CFDESKTOP_EXPORTS when building the shared library.
 * This follows the standard CMake convention: <target>_EXPORTS is defined
 * when compiling sources that are part of a SHARED library target.
 */

#if defined(_WIN32) || defined(_MSC_VER)
#    ifdef CFDESKTOP_STATIC_BUILD
#        define CF_DESKTOP_EXPORT
#    elif defined(CFDESKTOP_EXPORTS)
#        define CF_DESKTOP_EXPORT __declspec(dllexport)
#    else
#        define CF_DESKTOP_EXPORT __declspec(dllimport)
#    endif
#else
// Linux/Unix: same attribute for both building and using
#    define CF_DESKTOP_EXPORT __attribute__((visibility("default")))
#endif
