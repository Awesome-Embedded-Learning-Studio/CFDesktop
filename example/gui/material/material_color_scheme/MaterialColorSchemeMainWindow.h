/**
 * @file MaterialColorSchemeMainWindow.h
 * @brief Material Design 3 Color Scheme Gallery - Main Window
 *
 * A visual gallery to display all Material Design 3 color tokens
 * with light/dark theme switching support.
 *
 * @author CFDesktop Team
 * @date 2026-02-25
 * @version 0.1
 */

#pragma once

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "ui/core/material/cfmaterial_scheme.h"

namespace cf::ui::gallery {

/**
 * @brief Toast notification widget for displaying temporary messages.
 *
 * Shows a semi-transparent notification at the bottom of the window
 * that automatically fades out after 2 seconds.
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
 * @brief Color card widget displaying a single color token.
 *
 * Shows:
 * - Color preview swatch
 * - Token name (e.g., "md.primary")
 * - HEX color value
 * - Contrast ratio with paired color
 *
 * Click to copy HEX value to clipboard.
 */
class ColorCardWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ColorCardWidget(const QString& tokenName, const QColor& color,
                             const QString& contrastInfo, QWidget* parent = nullptr);

    void updateColor(const QColor& color, const QString& contrastInfo);

  signals:
    void clicked(const QString& hexValue);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    QString tokenName_;
    QColor color_;
    QString hexValue_;
    QString contrastInfo_;
    bool isHovered_ = false;
};

/**
 * @brief Theme toggle switch widget.
 *
 * A mobile-style toggle switch for switching between light and dark themes.
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
 * @brief Main window for the Material Color Scheme Gallery.
 *
 * Displays all 26 Material Design 3 color tokens organized in groups:
 * - Primary Colors (4)
 * - Secondary Colors (4)
 * - Tertiary Colors (4)
 * - Error Colors (4)
 * - Surface Colors (8)
 * - Utility Colors (5)
 */
class MaterialColorSchemeMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialColorSchemeMainWindow(QWidget* parent = nullptr);
    ~MaterialColorSchemeMainWindow() override;

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void setupUI();
    void createHeader();
    void createColorGroups();
    void updateAllColors();
    void updateWindowTheme();
    int calculateColumnCount() const;

    // Color group creation helpers
    void createColorGroup(const QString& title, QGridLayout* layout, int& row,
                          const QStringList& tokens);

    // Calculate WCAG contrast ratio
    float calculateContrastRatio(const QColor& fg, const QColor& bg) const;

    // Get contrast pair for a token
    QString getContrastPair(const QString& token) const;

    // Show toast notification
    void showToast(const QString& message);

  private slots:
    void onThemeChanged(bool isDark);
    void onColorCardClicked(const QString& hexValue);

  private:
    // Color schemes
    cf::ui::core::MaterialColorScheme lightScheme_;
    cf::ui::core::MaterialColorScheme darkScheme_;
    bool isDarkTheme_ = false;

    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QHBoxLayout* headerLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* colorGridLayout_;

    // Header components
    QLabel* titleLabel_;
    QLabel* themeLabel_;
    ThemeSwitch* themeSwitch_;

    // Toast
    ToastWidget* toast_;

    // Color cards storage for theme updates
    struct ColorCardInfo {
        ColorCardWidget* widget;
        QString token;
    };
    QList<ColorCardInfo> colorCards_;

    // All 26 color tokens
    static const QStringList PRIMARY_TOKENS;
    static const QStringList SECONDARY_TOKENS;
    static const QStringList TERTIARY_TOKENS;
    static const QStringList ERROR_TOKENS;
    static const QStringList SURFACE_TOKENS;
    static const QStringList UTILITY_TOKENS;
};

} // namespace cf::ui::gallery
