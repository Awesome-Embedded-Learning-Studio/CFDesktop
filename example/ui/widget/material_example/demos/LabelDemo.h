/**
 * @file LabelDemo.h
 * @brief Material Design 3 Label Demo Widget
 */

#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <qgroupbox.h>
class QGroupBox;

namespace cf::ui::example {

/**
 * @brief Demo widget for Material Design 3 Label component.
 *
 * Displays all 15 Material Design 3 typography styles:
 * - Display Large/Medium/Small (hero content)
 * - Headline Large/Medium/Small (app bar text)
 * - Title Large/Medium/Small (section headings)
 * - Body Large/Medium/Small (main content)
 * - Label Large/Medium/Small (secondary information)
 *
 * Also demonstrates:
 * - Different color variants (OnSurface, Primary, Error, etc.)
 * - Disabled state
 * - Auto-hiding behavior
 * - Word wrap functionality
 */
class LabelDemo : public QWidget {
    Q_OBJECT

  public:
    explicit LabelDemo(QWidget* parent = nullptr);
    ~LabelDemo() override = default;

    QString title() const { return "Label"; }
    QString description() const { return "Material Design 3 Typography Component"; }

  private:
    void setupUI();
    void createDisplaySection();
    void createHeadlineSection();
    void createTitleSection();
    void createBodySection();
    void createLabelSection();
    void createColorVariantsSection();
    void createStatesSection();
    void createFeaturesSection();

    QGroupBox* createGroupBox(const QString& title);

  private:
    QWidget* scrollContent_;
    QVBoxLayout* layout_;
};

} // namespace cf::ui::example
