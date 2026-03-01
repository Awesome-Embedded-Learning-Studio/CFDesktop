/**
 * @file MotionSpecPage.h
 * @brief Motion Spec page for the Material Gallery
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include "ThemePageWidget.h"
#include "ui/base/easing.h"
#include "ui/core/material/cfmaterial_motion.h"
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::core {
struct ICFTheme;
}

namespace cf::ui::gallery {

/**
 * @brief Animation preview widget.
 */
class MotionPreviewWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

  public:
    explicit MotionPreviewWidget(const cf::ui::core::MotionSpec& spec, const QString& name,
                                 QWidget* parent = nullptr);

    float progress() const { return progress_; }
    void setProgress(float progress);

    void startAnimation();
    void resetAnimation();
    void setDarkTheme(bool dark) {
        isDarkTheme_ = dark;
        update();
    }
    void updateSpec(const cf::ui::core::MotionSpec& spec);

  signals:
    void progressChanged();
    void animationFinished();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    void updateAnimation();
    float calculateEasedProgress(float linearProgress) const;

    cf::ui::core::MotionSpec spec_;
    QString name_;
    float progress_ = 0.0f;
    float elapsed_ = 0.0f;
    QTimer* timer_;
    bool isAnimating_ = false;
    bool isDarkTheme_ = false;
    QPointF ballPosition_;
};

/**
 * @brief Motion spec card widget.
 */
class MotionCardWidget : public QWidget {
    Q_OBJECT

  public:
    explicit MotionCardWidget(const cf::ui::core::MotionSpec& spec, const QString& name,
                              const QString& description, QWidget* parent = nullptr);

    void setThemeColors(const QColor& background, const QColor& surface, const QColor& onSurface);
    void setDarkTheme(bool dark) {
        isDarkTheme_ = dark;
        update();
    }

  signals:
    void playRequested(const cf::ui::core::MotionSpec& spec);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    void updateCurvePath();

    cf::ui::core::MotionSpec spec_;
    QString name_;
    QString description_;
    bool isHovered_ = false;
    bool isDarkTheme_ = false;

    // Theme colors
    QColor backgroundColor_;
    QColor surfaceColor_;
    QColor onSurfaceColor_;

    QPainterPath curvePath_;
};

/**
 * @brief Motion Spec page - displays all 9 Material Design 3 motion presets.
 */
class MotionSpecPage : public ThemePageWidget {
    Q_OBJECT

  public:
    explicit MotionSpecPage(QWidget* parent = nullptr);
    ~MotionSpecPage() override = default;

    QString pageTitle() const override { return "动效规范"; }
    void applyTheme(const cf::ui::core::ICFTheme& theme) override;

  private:
    void setupUI();
    void createPreviewSection();
    void createMotionCards();
    void updateWindowTheme();

    QString easingTypeToString(cf::ui::base::Easing::Type type) const;

  private slots:
    void onPlayRequested(const cf::ui::core::MotionSpec& spec);
    void onAnimationFinished();
    void onSpeedChanged(int index);

  private:
    // UI components
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* cardsLayout_;
    QFrame* previewFrame_;
    QVBoxLayout* previewLayout_;

    // Preview section
    QLabel* previewLabel_;
    QHBoxLayout* previewContainer_;
    MotionPreviewWidget* previewWidget_;
    QLabel* previewInfoLabel_;
    QComboBox* speedCombo_;

    // Theme
    bool isDarkTheme_ = false;

    // All 9 motion presets info
    struct MotionPresetInfo {
        QString name;
        QString description;
        cf::ui::core::MotionSpec spec;
    };
    QList<MotionPresetInfo> motionPresets_;
    QList<MotionCardWidget*> motionCards_;

    // Current animation
    QPropertyAnimation* currentAnimation_ = nullptr;
};

} // namespace cf::ui::gallery
