/**
 * @file CheckBoxDemo.h
 * @brief Material Design 3 CheckBox Demo - Widget Component
 */

#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <qgroupbox.h>
class QGroupBox;

namespace cf::ui::example {

class CheckBoxDemo : public QWidget {
    Q_OBJECT

  public:
    explicit CheckBoxDemo(QWidget* parent = nullptr);
    ~CheckBoxDemo() override = default;

    QString title() const { return "CheckBox"; }
    QString description() const { return "Material Design 3 CheckBox Component"; }

  private:
    void setupUI();
    void createCheckboxStatesSection();
    void createTristateSection();
    void createErrorStateSection();
    void createDisabledSection();
    void createInteractiveDemoSection();
    void createCustomLabelsSection();
    QGroupBox* createGroupBox(const QString& title);

    void onAcceptCheckboxClicked(bool checked);
    void onTermsCheckboxClicked(bool checked);
    void onNewsletterCheckboxClicked(bool checked);
    void onErrorToggleClicked(bool checked);

    QWidget* scrollContent_;
    QVBoxLayout* layout_;
    QLabel* statusLabel_;
};

} // namespace cf::ui::example
