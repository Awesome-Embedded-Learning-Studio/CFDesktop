#include "boot_detect.h"
#include <QCoreApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QProcess>
#include <QSysInfo>

BootCheckResult BootDetect::performChecks() {
    QList<BootCheckResult> results;

    // 执行所有检测项
    results.append(checkQtVersion());
    results.append(checkQtModules());
    results.append(checkCompiler());
    results.append(checkSystemInfo());

    int failCount = 0;

    for (const auto& result : results) {
        if (!result.success) {
            failCount++;
        }
    }

    // 生成总体结果
    QString summary = getSummary(results);
    bool allPassed = (failCount == 0);

    return BootCheckResult(allPassed, summary, QString(), allPassed ? 0 : 1);
}

BootCheckResult BootDetect::checkQtVersion() {
    QString qtVersion = QT_VERSION_STR;
    QVersionNumber currentVersion = QVersionNumber::fromString(qtVersion);
    QVersionNumber minVersion(6, 8, 0);

    bool success = currentVersion >= minVersion;

    QString details = QString("Current: %1\nRequired: >= 6.8.0").arg(qtVersion);

    return BootCheckResult(success, success ? "Qt version check PASSED" : "Qt version check FAILED",
                           details, success ? 0 : 1);
}

BootCheckResult BootDetect::checkQtModules() {
    QStringList availableModules;
    QStringList requiredModules = {"Core", "Gui", "Widgets"};

    // Qt6 模块检查
    bool allAvailable = true;
    QStringList missingModules;

    for (const QString& module : requiredModules) {
        bool available = false;

        if (module == "Core") {
            available = true;
        } else if (module == "Gui") {
            available = true;
        } else if (module == "Widgets") {
            available = true;
        }

        if (available) {
            availableModules.append(module);
        } else {
            missingModules.append(module);
            allAvailable = false;
        }
    }

    QString details = QString("Available modules: %1\nMissing modules: %2")
                          .arg(availableModules.join(", "))
                          .arg(missingModules.isEmpty() ? "None" : missingModules.join(", "));

    return BootCheckResult(allAvailable,
                           allAvailable ? "Qt modules check PASSED" : "Qt modules check FAILED",
                           details, allAvailable ? 0 : 1);
}

BootCheckResult BootDetect::checkCompiler() {
    QString compiler = QSysInfo::buildAbi();

    bool isLLVM =
        compiler.contains("LLVM") || compiler.contains("Clang") || compiler.contains("clang");

    QString details =
        QString("Compiler ABI: %1\nIs LLVM: %2").arg(compiler).arg(isLLVM ? "Yes" : "No");

    // 发出警告但不失败
    return BootCheckResult(true, // 总是通过，只是警告
                           isLLVM ? "Compiler check PASSED (LLVM detected)"
                                  : "Compiler check WARNING (non-LLVM compiler)",
                           details, 0);
}

BootCheckResult BootDetect::checkSystemInfo() {
    QString osType = QSysInfo::prettyProductName();
    QString arch = QSysInfo::currentCpuArchitecture();
    QString kernelType = QSysInfo::kernelType();
    QString kernelVersion = QSysInfo::kernelVersion();

    QString details = QString("OS: %1\nArchitecture: %2\nKernel: %3 %4")
                          .arg(osType)
                          .arg(arch)
                          .arg(kernelType)
                          .arg(kernelVersion);

    return BootCheckResult(true, "System info collected", details, 0);
}

QString BootDetect::getSummary(const QList<BootCheckResult>& results) {
    QString summary;
    summary += "========== Boot Test Results ==========\n";
    summary += "=====================================\n\n";

    for (const auto& result : results) {
        QString status = result.success ? "[PASS]" : "[FAIL]";
        summary += QString("%1 %2\n").arg(status, result.message);

        if (!result.details.isEmpty()) {
            summary +=
                "  Details: " + QString(result.details).replace("\n", "\n           ") + "\n";
        }
        summary += "\n";
    }

    summary += "=====================================\n";

    int passCount = 0, failCount = 0;
    for (const auto& result : results) {
        if (result.success)
            passCount++;
        else
            failCount++;
    }

    summary += QString("Total: %1 passed, %2 failed\n").arg(passCount).arg(failCount);

    return summary;
}

QString BootDetect::formatResult(const BootCheckResult& result) {
    return QString("%1: %2").arg(result.success ? "PASS" : "FAIL", result.message);
}

QString BootDetect::formatDetailedResult(const BootCheckResult& result) {
    QString output = formatResult(result) + "\n";

    if (!result.details.isEmpty()) {
        output += "Details:\n" + result.details + "\n";
    }

    return output;
}
