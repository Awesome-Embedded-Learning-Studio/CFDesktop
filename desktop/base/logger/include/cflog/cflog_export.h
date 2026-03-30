/**
 * @file    cflog_export.h
 * @brief   Export/import macros for CFLog library symbols.
 *
 * When building cflogger (static lib merged into CFDesktop_shared DLL),
 * CFLOG_BUILDING is defined so symbols are marked dllexport.
 * When consuming from the EXE or other targets, symbols are marked dllimport.
 */
#pragma once

#if defined(_WIN32) || defined(_MSC_VER)
#    ifdef CFLOG_STATIC_BUILD
#        define CFLOG_API
#    elif defined(CFLOG_BUILDING)
#        define CFLOG_API __declspec(dllexport)
#    else
#        define CFLOG_API __declspec(dllimport)
#    endif
#else
#    define CFLOG_API __attribute__((visibility("default")))
#endif
