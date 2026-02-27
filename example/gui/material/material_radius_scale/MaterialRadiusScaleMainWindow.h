/**
 * @file MaterialRadiusScaleMainWindow.h
 * @brief Material Design 3 Radius Scale Gallery - Main Window
 *
 * A visual gallery to display all Material Design 3 corner radius tokens
 * with visual preview cards.
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>

#include "ui/core/material/cfmaterial_radius_scale.h"

namespace cf::ui::gallery {

/**
 * @brief 圆角预览卡片 - 显示一个带圆角的矩形
 *
 * 每个卡片展示：
 * - 圆角级别的名称（如 "Corner Small"）
 * - 带对应圆角的预览矩形
 * - dp 值标签（如 "8 dp"）
 */
class RadiusPreviewWidget : public QWidget {
    Q_OBJECT
  public:
    explicit RadiusPreviewWidget(const QString& name, float radiusDp,
                                 const QColor& accentColor, QWidget* parent = nullptr);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString name_;
    float radiusDp_;
    QColor accentColor_;
};

/**
 * @brief 主窗口 - 展示所有 7 个圆角级别
 *
 * 显示 Material Design 3 的所有圆角半径：
 * - Corner None (0dp)
 * - Corner Extra Small (4dp)
 * - Corner Small (8dp)
 * - Corner Medium (12dp)
 * - Corner Large (16dp)
 * - Corner Extra Large (28dp)
 * - Corner Extra Extra Large (32dp)
 */
class MaterialRadiusScaleMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialRadiusScaleMainWindow(QWidget* parent = nullptr);
    ~MaterialRadiusScaleMainWindow() override = default;

  private:
    void setupUI();
    void createHeader();
    void createRadiusCards();

  private:
    cf::ui::core::MaterialRadiusScale radiusScale_;

    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QGridLayout* cardsLayout_;
};

} // namespace cf::ui::gallery
