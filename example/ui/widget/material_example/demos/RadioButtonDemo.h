/**
 * @file RadioButtonDemo.h
 * @brief Material Design 3 RadioButton Demo Widget
 */

#pragma once

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

class RadioButtonDemo : public QWidget {
    Q_OBJECT

  public:
    explicit RadioButtonDemo(QWidget* parent = nullptr);
    ~RadioButtonDemo() override = default;

    QString title() const { return "RadioButton"; }
    QString description() const { return "Material Design 3 RadioButton Component"; }

  private:
    void setupUI();
    void createBasicRadioSection();
    void createRadioStatesSection();
    void createButtonGroupSection();
    void createErrorStateSection();
    void createInteractionDemoSection();

    QGroupBox* createGroupBox(const QString& title);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* selectionLabel_;
};

} // namespace cf::ui::example
