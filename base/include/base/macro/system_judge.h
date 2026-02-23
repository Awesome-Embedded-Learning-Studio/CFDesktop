/**
 * @file    base/include/base/macro/system_judge.h
 * @brief   Defines platform and architecture detection macros.
 *
 * Provides compile-time macros for detecting the operating system
 * and CPU architecture.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_macros
 */
#pragma once

/* ==================== Operating System Detection ==================== */

// Windows platform detection
#if defined(_WIN32) || defined(_WIN64)
#    define CFDESKTOP_OS_WINDOWS
#endif

// Linux platform detection
#if defined(__linux__)
#    define CFDESKTOP_OS_LINUX
#endif

/* ==================== Architecture Detection ==================== */

// x86_64 (AMD64) detection
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
#    define CFDESKTOP_ARCH_X86_64
#endif

// ARM64 detection
#if defined(__aarch64__) || defined(_M_ARM64)
#    define CFDESKTOP_ARCH_ARM64
#endif

// ARM32 detection
#if defined(__arm__) || defined(_M_ARM)
#    define CFDESKTOP_ARCH_ARM32
#endif
