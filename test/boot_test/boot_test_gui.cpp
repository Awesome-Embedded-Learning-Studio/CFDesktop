#include "boot_detect.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

class BootTestWindow : public QMainWindow {
    Q_OBJECT

  public:
    BootTestWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();

        // 延迟执行检测，确保窗口已显示
        QTimer::singleShot(100, this, &BootTestWindow::runChecks);
    }

  private slots:
    void runChecks() {
        m_textEdit->clear();
        m_textEdit->append("========== Boot Test Results ==========");
        m_textEdit->append("=====================================\n");

        // 执行各项检测
        BootCheckResult qtVerResult = BootDetect::checkQtVersion();
        displayResult(qtVerResult);

        BootCheckResult qtModResult = BootDetect::checkQtModules();
        displayResult(qtModResult);

        BootCheckResult compilerResult = BootDetect::checkCompiler();
        displayResult(compilerResult);

        BootCheckResult sysInfoResult = BootDetect::checkSystemInfo();
        displayResult(sysInfoResult);

        // 总结
        m_textEdit->append("\n=====================================");

        bool allPassed = qtVerResult.success && qtModResult.success;
        int passCount = 0;
        int failCount = 0;

        if (qtVerResult.success)
            passCount++;
        else
            failCount++;
        if (qtModResult.success)
            passCount++;
        else
            failCount++;
        if (compilerResult.success)
            passCount++;
        else
            failCount++;
        if (sysInfoResult.success)
            passCount++;
        else
            failCount++;

        QString summary = QString("Total: %1 passed, %2 failed").arg(passCount).arg(failCount);

        m_textEdit->append(summary);

        // 更新状态标签
        if (allPassed) {
            m_statusLabel->setText("Status: ALL CHECKS PASSED");
            m_statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        } else {
            m_statusLabel->setText("Status: SOME CHECKS FAILED");
            m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        }

        // 设置退出码
        m_exitCode = allPassed ? 0 : 1;
    }

    void onExitClicked() { qApp->exit(m_exitCode); }

    void onCloseClicked() { qApp->exit(m_exitCode); }

  private:
    void setupUI() {
        setWindowTitle("Boot Environment Test");
        resize(600, 500);

        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        // 标题
        QLabel* titleLabel = new QLabel("Qt Boot Environment Checker");
        titleLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // 状态标签
        m_statusLabel = new QLabel("Status: Checking...");
        m_statusLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(m_statusLabel);

        // 文本显示区域
        m_textEdit = new QTextEdit();
        m_textEdit->setReadOnly(true);
        m_textEdit->setFont(QFont("Consolas", 9));
        mainLayout->addWidget(m_textEdit);

        // 按钮区域
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();

        QPushButton* rerunButton = new QPushButton("Rerun Tests");
        connect(rerunButton, &QPushButton::clicked, this, &BootTestWindow::runChecks);
        buttonLayout->addWidget(rerunButton);

        QPushButton* exitButton = new QPushButton("Exit");
        connect(exitButton, &QPushButton::clicked, this, &BootTestWindow::onExitClicked);
        buttonLayout->addWidget(exitButton);

        buttonLayout->addStretch();
        mainLayout->addLayout(buttonLayout);
    }

    void displayResult(const BootCheckResult& result) {
        QString status = result.success ? "[PASS]" : "[FAIL]";
        QString color = result.success ? "green" : "red";

        m_textEdit->append(QString("<span style='color:%1;'><b>%2 %3</b></span>")
                               .arg(color)
                               .arg(status)
                               .arg(result.message));

        if (!result.details.isEmpty()) {
            m_textEdit->append("  Details: " + QString(result.details).replace("\n", "<br>") +
                               "  ");
        }

        m_textEdit->append("");
    }

    QTextEdit* m_textEdit;
    QLabel* m_statusLabel;
    int m_exitCode = 1;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("boot_test_gui");
    app.setApplicationVersion("1.0.0");

    BootTestWindow window;
    window.show();

    return app.exec();
}

#include "boot_test_gui.moc"
