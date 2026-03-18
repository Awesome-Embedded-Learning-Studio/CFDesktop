/**
 * @file TextFieldDemo.h
 * @brief Material Design 3 TextField Demo Widget
 */

#pragma once
#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

class TextFieldDemo : public QWidget {
    Q_OBJECT
  public:
    explicit TextFieldDemo(QWidget* parent = nullptr);
    ~TextFieldDemo() override = default;

    QString title() const { return "TextField"; }
    QString description() const { return "Material Design 3 TextField Component"; }

  private:
    void setupUI();
    void createTextFieldVariantsSection();
    void createTextFieldStatesSection();
    void createTextFieldWithIconsSection();
    void createTextFieldWithErrorSection();
    void createTextFieldAdvancedSection();

    QGroupBox* createGroupBox(const QString& title);
    void onTextChanged(const QString& text);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
