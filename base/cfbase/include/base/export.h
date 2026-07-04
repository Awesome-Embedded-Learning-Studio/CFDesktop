/**
 * @file export.h
 * @brief CF Base Library export/import macros
 *
 * Defines CF_BASE_EXPORT macro for controlling symbol visibility in shared libraries.
 * - When building cfbase.dll/so, symbols are exported
 * - When using cfbase.dll/so, symbols are imported
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

/**
 * @def CF_BASE_EXPORT
 * @brief Export/import macro for CF Base Library symbols
 *
 * Usage:
 *   class CF_BASE_EXPORT MyClass { ... };
 *
 *   CF_BASE_EXPORT void myFunction();
 *
 * Platform behavior:
 * - Windows: Uses __declspec(dllexport/dllimport)
 * - Linux/GCC: Uses __attribute__((visibility("default")))
 *
 * CMake automatically defines CFBASE_EXPORTS when building the shared library.
 * This follows the standard CMake convention: <target>_EXPORTS is defined
 * when compiling sources that are part of a SHARED library target.
 */

#if defined(_WIN32) || defined(_MSC_VER)
#    if defined(cfbase_EXPORTS) || defined(CFBASE_EXPORTS)
#        define CF_BASE_EXPORT __declspec(dllexport)
#    else
#        define CF_BASE_EXPORT __declspec(dllimport)
#    endif
#else
// Linux/Unix: same attribute for both building and using
#    define CF_BASE_EXPORT __attribute__((visibility("default")))
#endif
