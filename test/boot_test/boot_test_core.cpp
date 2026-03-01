#include "boot_detect.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("boot_test_core");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Boot environment checker (Console version)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption verboseOption(QStringList() << "v"
                                                   << "verbose",
                                     "Show detailed output");
    parser.addOption(verboseOption);

    QCommandLineOption quietOption(QStringList() << "q"
                                                 << "quiet",
                                   "Only output errors");
    parser.addOption(quietOption);

    parser.process(app);

    bool verbose = parser.isSet(verboseOption);
    bool quiet = parser.isSet(quietOption);

    QTextStream out(stdout);
    QTextStream err(stderr);

    // 执行检测
    BootCheckResult result = BootDetect::performChecks();

    // 输出结果
    if (verbose) {
        out << BootDetect::getSummary({BootDetect::checkQtVersion(), BootDetect::checkQtModules(),
                                       BootDetect::checkCompiler(), BootDetect::checkSystemInfo()});
    } else if (!quiet) {
        out << result.message << "\n";
    }

    if (!result.success && !quiet) {
        err << "ERROR: Boot environment check failed!\n";
    }

    // 返回退出码
    return result.exitCode;
}
