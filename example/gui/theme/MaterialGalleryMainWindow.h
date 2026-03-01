/**
 * @file MaterialGalleryMainWindow.h
 * @brief Main window for the Material Design 3 Gallery
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace cf::ui::core {
struct ICFTheme;
class ThemeManager;
} // namespace cf::ui::core

namespace cf::ui::gallery {

// Forward declarations
class ThemeSidebar;
class ThemePageWidget;
class ColorSchemePage;
class MotionSpecPage;
class RadiusScalePage;
class TypographyPage;

/**
 * @brief Theme toggle switch widget (header).
 */
class ThemeSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        float knobPosition READ knobPosition WRITE setKnobPosition NOTIFY knobPositionChanged)

  public:
    explicit ThemeSwitch(QWidget* parent = nullptr);

    bool isDark() const { return isDark_; }
    void setDark(bool dark);

    float knobPosition() const { return knobPosition_; }
    void setKnobPosition(float pos);

  signals:
    void themeChanged(bool isDark);
    void knobPositionChanged();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    bool isDark_ = false;
    float knobPosition_ = 0.0f;
    QPropertyAnimation* animation_;
};

/**
 * @brief Main window for the Material Design 3 Gallery.
 *
 * Features:
 * - Left sidebar with 4 tab items (Color Scheme, Motion Spec, Radius Scale, Typography)
 * - Content area with QStackedWidget for switching between pages
 * - Theme toggle switch in header for light/dark theme switching
 * - ThemeManager integration for global theme management
 */
class MaterialGalleryMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialGalleryMainWindow(QWidget* parent = nullptr);
    ~MaterialGalleryMainWindow() override;

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void setupUI();
    void createHeader();
    void setupThemeManager();
    void applyThemeToAllPages(const cf::ui::core::ICFTheme& theme);

  private slots:
    void onTabClicked(int index);
    void onThemeChanged(bool isDark);
    void onThemeManagerChanged(const cf::ui::core::ICFTheme& theme);

  private:
    // Theme Manager
    cf::ui::core::ThemeManager* themeManager_ = nullptr;
    bool isDarkTheme_ = false;

    // UI components
    QWidget* centralWidget_;
    QHBoxLayout* mainLayout_;
    QVBoxLayout* contentLayout_;
    QHBoxLayout* headerLayout_;

    // Sidebar
    ThemeSidebar* sidebar_;

    // Header components
    QLabel* titleLabel_;
    QLabel* themeLabel_;
    ThemeSwitch* themeSwitch_;

    // Content area
    QStackedWidget* contentStack_;

    // Pages
    ColorSchemePage* colorSchemePage_;
    MotionSpecPage* motionSpecPage_;
    RadiusScalePage* radiusScalePage_;
    TypographyPage* typographyPage_;
};

} // namespace cf::ui::gallery
