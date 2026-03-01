/**
 * @file RadiusScalePage.h
 * @brief Radius Scale page for the Material Gallery
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include "ThemePageWidget.h"
#include "ui/core/material/cfmaterial_radius_scale.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>

namespace cf::ui::core {
struct ICFTheme;
}

namespace cf::ui::gallery {

/**
 * @brief Corner radius preview card widget.
 */
class RadiusPreviewWidget : public QWidget {
    Q_OBJECT
  public:
    explicit RadiusPreviewWidget(const QString& name, float radiusDp, const QColor& accentColor,
                                 QWidget* parent = nullptr);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString name_;
    float radiusDp_;
    QColor accentColor_;
};

/**
 * @brief Radius Scale page - displays all 7 Material Design 3 corner radius levels.
 */
class RadiusScalePage : public ThemePageWidget {
    Q_OBJECT

  public:
    explicit RadiusScalePage(QWidget* parent = nullptr);
    ~RadiusScalePage() override = default;

    QString pageTitle() const override { return "圆角规范"; }
    void applyTheme(const cf::ui::core::ICFTheme& theme) override;

  private:
    void setupUI();
    void createRadiusCards();

  private:
    const cf::ui::core::ICFTheme* theme_ = nullptr;

    // UI components
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QGridLayout* cardsLayout_;

    // Theme colors
    QColor cardBgColor_;
    QColor cardBorderColor_;
    QColor textColor_;
};

} // namespace cf::ui::gallery
