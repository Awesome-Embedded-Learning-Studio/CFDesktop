#ifndef BOOT_DETECT_H
#define BOOT_DETECT_H

#include <QPair>
#include <QString>
#include <QVersionNumber>

struct BootCheckResult {
    bool success;
    QString message;
    QString details;
    int exitCode;

    BootCheckResult() : success(false), exitCode(1) {}
    BootCheckResult(bool s, const QString& msg, const QString& det = "", int code = 0)
        : success(s), message(msg), details(det), exitCode(code) {}
};

class BootDetect {
  public:
    // 执行所有检测
    static BootCheckResult performChecks();

    // 单独的检测项
    static BootCheckResult checkQtVersion();
    static BootCheckResult checkQtModules();
    static BootCheckResult checkCompiler();
    static BootCheckResult checkSystemInfo();

    // 获取检测摘要
    static QString getSummary(const QList<BootCheckResult>& results);

    // 格式化输出
    static QString formatResult(const BootCheckResult& result);
    static QString formatDetailedResult(const BootCheckResult& result);
};

#endif // BOOT_DETECT_H
