/**
 * @file MaterialMotionSpecMainWindow.h
 * @brief Material Design 3 Motion Spec Gallery - Main Window
 *
 * A visual gallery to display all Material Design 3 motion presets
 * with interactive animation demonstrations and easing curve visualization.
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#pragma once

#include "ui/core/material/cfmaterial_motion.h"
#include <QColor>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPointF>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace cf::ui::gallery {

/**
 * @brief Animation preview widget.
 *
 * Shows a visual representation of the motion spec with an animated ball
 * that demonstrates the easing curve and duration.
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
    void updateSpec(const cf::ui::core::MotionSpec& spec) {
        spec_ = spec;
        resetAnimation();
    }

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

    // Ball position cache
    QPointF ballPosition_;
};

/**
 * @brief Motion spec card widget.
 *
 * Displays information about a single motion preset:
 * - Name and description
 * - Duration and easing type
 * - Visual easing curve preview
 * - Play button for animation demo
 */
class MotionCardWidget : public QWidget {
    Q_OBJECT

  public:
    explicit MotionCardWidget(const cf::ui::core::MotionSpec& spec, const QString& name,
                              const QString& description, QWidget* parent = nullptr);

    void setThemeColors(const QColor& background, const QColor& surface, const QColor& onSurface);

  signals:
    void playRequested(const cf::ui::core::MotionSpec& spec);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    cf::ui::core::MotionSpec spec_;
    QString name_;
    QString description_;
    bool isHovered_ = false;
    bool isDarkTheme_ = false;

    // Theme colors
    QColor backgroundColor_;
    QColor surfaceColor_;
    QColor onSurfaceColor_;

    // Easing curve points cache
    void updateCurvePath();
    QPainterPath curvePath_;
};

/**
 * @brief Toast notification widget for displaying temporary messages.
 */
class ToastWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ToastWidget(const QString& message, QWidget* parent = nullptr);
    void show(int durationMs = 2000);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString message_;
};

/**
 * @brief Main window for the Material Motion Spec Gallery.
 *
 * Displays all 9 Material Design 3 motion presets:
 * - Short Enter/Exit (200/150ms)
 * - Medium Enter/Exit (300/250ms)
 * - Long Enter/Exit (450/400ms)
 * - State Change (200ms)
 * - Ripple Expand/Fade (400/150ms)
 */
class MaterialMotionSpecMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialMotionSpecMainWindow(QWidget* parent = nullptr);
    ~MaterialMotionSpecMainWindow() override;

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void setupUI();
    void createHeader();
    void createMotionCards();
    void createPreviewSection();
    void updateWindowTheme();

    QString easingTypeToString(cf::ui::base::Easing::Type type) const;

  private slots:
    void onPlayRequested(const cf::ui::core::MotionSpec& spec);
    void onAnimationFinished();
    void onThemeToggle();
    void onSpeedChanged(int index);

  private:
    // Motion scheme
    cf::ui::core::MaterialMotionScheme motionScheme_;

    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QHBoxLayout* headerLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* cardsLayout_;
    QFrame* previewFrame_;
    QVBoxLayout* previewLayout_;

    // Header components
    QLabel* titleLabel_;
    QLabel* themeLabel_;
    QPushButton* themeButton_;

    // Preview section
    QLabel* previewLabel_;
    QHBoxLayout* previewContainer_;
    MotionPreviewWidget* previewWidget_;
    QLabel* previewInfoLabel_;
    QComboBox* speedCombo_;

    // Theme
    bool isDarkTheme_ = false;

    // Toast
    ToastWidget* toast_;

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
