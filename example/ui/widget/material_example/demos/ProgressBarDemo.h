/**
 * @file ProgressBarDemo.h
 * @brief Material Design 3 ProgressBar Demo Widget
 */

#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class QGroupBox;

namespace cf::ui::widget::material {
class ProgressBar;
}

namespace cf::ui::example {

/**
 * @brief Demo widget for Material Design 3 ProgressBar component.
 *
 * Displays:
 * - Determinate progress bars with different values
 * - Indeterminate progress bar (animated)
 * - Disabled state
 * - Interactive demo with increment/decrement buttons
 */
class ProgressBarDemo : public QWidget {
    Q_OBJECT

  public:
    explicit ProgressBarDemo(QWidget* parent = nullptr);
    ~ProgressBarDemo() override;

    QString title() const { return "ProgressBar"; }
    QString description() const { return "Material Design 3 Progress Indicator Component"; }

  private:
    void setupUI();
    void createDeterminateSection();
    void createIndeterminateSection();
    void createStatesSection();
    void createInteractiveDemoSection();

    QGroupBox* createGroupBox(const QString& title);

  private slots:
    void onIncrementClicked();
    void onDecrementClicked();
    void onResetClicked();
    void updateProgress();

  private:
    QWidget* scrollContent_;
    QVBoxLayout* layout_;

    // Interactive demo
    widget::material::ProgressBar* demoProgressBar_;
    QTimer* progressTimer_{nullptr};
    int demoValue_ = 0;
};

} // namespace cf::ui::example
