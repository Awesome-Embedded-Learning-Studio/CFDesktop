/**
 * @file ButtonExampleWindow.h
 * @brief Material Design 3 Button Example - Main Window
 *
 * A visual gallery to display all Material Design 3 button variants
 * with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#pragma once

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

/**
 * @brief Main window for the Material Button Example.
 *
 * Displays all 5 Material Design 3 button variants:
 * - Filled Button (填充按钮)
 * - Tonal Button (音调按钮)
 * - Outlined Button (轮廓按钮)
 * - Text Button (文本按钮)
 * - Elevated Button (升级按钮)
 *
 * Also demonstrates:
 * - Buttons with leading icons
 * - Disabled button states
 * - Different elevation levels
 * - Button interaction (ripple effects, hover states)
 */
class ButtonExampleWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit ButtonExampleWindow(QWidget* parent = nullptr);
    ~ButtonExampleWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createButtonVariantsSection();
    void createButtonStatesSection();
    void createButtonWithIconSection();
    void createElevationSection();
    void createInteractionDemoSection();

    QGroupBox* createGroupBox(const QString& title);
    QWidget* createScrollContent();

  private slots:
    void onDemoButtonClicked();

  private:
    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;

    // Header
    QLabel* titleLabel_;
    QLabel* subtitleLabel_;

    // Interaction demo
    int clickCount_ = 0;
};

} // namespace cf::ui::example
