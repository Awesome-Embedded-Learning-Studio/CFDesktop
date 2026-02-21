#pragma once
#include <QtGlobal>

#ifdef Q_OS_WIN
#    define CFDESKTOP_OS_WINDOWS
#endif

// Linux 平台判定
#ifdef Q_OS_LINUX
#    define CFDESKTOP_OS_LINUX
#endif

// Unix-like 平台判定（包含 Linux、macOS、BSD）
#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)
#    error \
        "Current OS May failed to compiled the CFDesktop, If you have successed, fork and patch the repo! PR are welcomed!"
#endif

#ifdef Q_PROCESSOR_X86_64
#    define CFDESKTOP_ARCH_X86_64
#endif
#ifdef Q_PROCESSOR_ARM_64
#    define CFDESKTOP_ARCH_ARM64
#endif

#ifdef Q_PROCESSOR_ARM_32
#    define CFDESKTOP_ARCH_ARM32
#endif
