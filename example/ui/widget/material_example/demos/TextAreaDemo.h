/**
 * @file TextAreaDemo.h
 * @brief Material Design 3 TextArea Demo Widget
 */

#pragma once
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class QGroupBox;

namespace cf::ui::example {

class TextAreaDemo : public QWidget {
    Q_OBJECT
  public:
    explicit TextAreaDemo(QWidget* parent = nullptr);
    ~TextAreaDemo() override = default;

    QString title() const { return "TextArea"; }
    QString description() const { return "Material Design 3 TextArea Component"; }

  private:
    void setupUI();
    void createTextAreaVariantsSection();
    void createTextAreaStatesSection();
    void createCharacterCounterSection();
    void createMinMaxLinesSection();
    void createInteractionDemoSection();

    QGroupBox* createGroupBox(const QString& title);
    void onDemoTextChanged();

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* charCountLabel_;
};

} // namespace cf::ui::example
