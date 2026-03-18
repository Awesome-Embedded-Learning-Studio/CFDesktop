/**
 * @file ButtonDemo.h
 * @brief Material Design 3 Button Demo - Widget Component
 */

#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class QGroupBox;

namespace cf::ui::example {

class ButtonDemo : public QWidget {
    Q_OBJECT

  public:
    explicit ButtonDemo(QWidget* parent = nullptr);
    ~ButtonDemo() override = default;

    QString title() const { return "Button"; }
    QString description() const { return "Material Design 3 Button Component"; }

  private:
    void setupUI();
    void createButtonVariantsSection();
    void createButtonStatesSection();
    void createButtonWithIconSection();
    void createElevationSection();
    void createInteractionDemoSection();
    QGroupBox* createGroupBox(const QString& title);

    void onDemoButtonClicked();

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    int clickCount_ = 0;
};

} // namespace cf::ui::example
