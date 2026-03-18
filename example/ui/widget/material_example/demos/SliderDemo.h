/**
 * @file SliderDemo.h
 * @brief Material Design 3 Slider Demo Widget
 */

#pragma once

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

class SliderDemo : public QWidget {
    Q_OBJECT

  public:
    explicit SliderDemo(QWidget* parent = nullptr);
    ~SliderDemo() override = default;

    QString title() const { return "Slider"; }
    QString description() const { return "Material Design 3 Slider Component"; }

  private:
    void setupUI();
    void createHorizontalSection();
    void createVerticalSection();
    void createStatesSection();
    void createTickMarksSection();
    void createInteractiveDemoSection();

    QGroupBox* createGroupBox(const QString& title);

    void onHorizontalSliderChanged(int value);
    void onVerticalSliderChanged(int value);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* horizontalValueLabel_;
    QLabel* verticalValueLabel_;
};

} // namespace cf::ui::example
