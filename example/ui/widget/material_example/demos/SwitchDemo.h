/**
 * @file SwitchDemo.h
 * @brief Material Design 3 Switch Demo - Widget Component
 */

#pragma once

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

class SwitchDemo : public QWidget {
    Q_OBJECT

  public:
    explicit SwitchDemo(QWidget* parent = nullptr);
    ~SwitchDemo() override = default;

    QString title() const { return "Switch"; }
    QString description() const { return "Material Design 3 Switch Component"; }

  private:
    void setupUI();
    void createBasicStatesSection();
    void createWithLabelsSection();
    void createDisabledSection();
    void createInteractiveDemoSection();

    QGroupBox* createGroupBox(const QString& title);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;

  private slots:
    void onSwitchToggled(bool checked);
};

} // namespace cf::ui::example
