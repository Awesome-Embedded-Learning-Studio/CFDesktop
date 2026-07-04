/**
 * @file    cflog_export.h
 * @brief   Export/import macros for CFLog library symbols.
 *
 * When building cflogger (shared lib), CMake auto-defines cflogger_EXPORTS
 * so symbols are marked dllexport. When consuming, symbols are dllimport.
 */
#pragma once

#if defined(_WIN32) || defined(_MSC_VER)
#    ifdef cflogger_EXPORTS
#        define CFLOG_API __declspec(dllexport)
#    else
#        define CFLOG_API __declspec(dllimport)
#    endif
#else
#    define CFLOG_API __attribute__((visibility("default")))
#endif
