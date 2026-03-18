/**
 * @file ComboBoxDemo.h
 * @brief Material Design 3 ComboBox Demo - Widget Component
 */

#pragma once

#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

class ComboBoxDemo : public QWidget {
    Q_OBJECT

  public:
    explicit ComboBoxDemo(QWidget* parent = nullptr);
    ~ComboBoxDemo() override = default;

    QString title() const { return "ComboBox"; }
    QString description() const { return "Material Design 3 ComboBox Component"; }

  private:
    void setupUI();
    void createVariantsSection();
    void createItemsSection();
    void createStatesSection();
    void createInteractiveDemoSection();
    QGroupBox* createGroupBox(const QString& title);

    void onComboBoxChanged(int index);
    void onDemoComboBoxChanged(int index);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* selectionLabel_;
};

} // namespace cf::ui::example
