/**
 * @file GroupBoxDemo.h
 * @brief Material Design 3 GroupBox Demo Widget
 */

#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::example {

/**
 * @brief Demo widget for Material Design 3 GroupBox component.
 *
 * Displays various GroupBox configurations:
 * - Different elevation levels (0-5)
 * - With and without border
 * - Different corner radius values
 * - Content examples with various widgets
 * - Nested group boxes
 */
class GroupBoxDemo : public QWidget {
    Q_OBJECT

  public:
    explicit GroupBoxDemo(QWidget* parent = nullptr);
    ~GroupBoxDemo() override = default;

    QString title() const { return "GroupBox"; }
    QString description() const { return "Material Design 3 GroupBox Component"; }

  private:
    void setupUI();
    void createBasicGroupBoxSection();
    void createElevationSection();
    void createBorderSection();
    void createCornerRadiusSection();
    void createContentExamplesSection();
    void createNestedGroupBoxSection();

  private:
    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
