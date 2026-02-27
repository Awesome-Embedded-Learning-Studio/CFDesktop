/**
 * @file ColorSchemePage.h
 * @brief Color Scheme page for the Material Gallery
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include "ThemePageWidget.h"
#include "ToastWidget.h"
#include "ui/core/material/cfmaterial_scheme.h"
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>

namespace cf::ui::core {
struct ICFTheme;
}

namespace cf::ui::gallery {

/**
 * @brief Color card widget displaying a single color token.
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
 * @brief Color Scheme page - displays all 26 Material Design 3 color tokens.
 *
 * Organized in 6 groups:
 * - Primary Colors (4)
 * - Secondary Colors (4)
 * - Tertiary Colors (4)
 * - Error Colors (4)
 * - Surface Colors (8)
 * - Utility Colors (5)
 */
class ColorSchemePage : public ThemePageWidget {
    Q_OBJECT

  public:
    explicit ColorSchemePage(QWidget* parent = nullptr);
    ~ColorSchemePage() override = default;

    QString pageTitle() const override { return "色彩方案"; }
    void applyTheme(const cf::ui::core::ICFTheme& theme) override;

  private:
    void setupUI();
    void createColorGroups();
    void updateAllColors();
    void updateWindowTheme();

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
    void onColorCardClicked(const QString& hexValue);

  private:
    // Theme pointer for accessing color scheme
    const cf::ui::core::ICFTheme* theme_ = nullptr;

    // UI components
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* colorGridLayout_;

    // Toast
    ToastWidget* toast_;

    // Color cards storage for theme updates
    struct ColorCardInfo {
        ColorCardWidget* widget;
        QString token;
    };
    QList<ColorCardInfo> colorCards_;

    // Theme colors for card background
    QColor cardBgColor_;
    QColor cardBorderColor_;

    // All 26 color tokens
    static const QStringList PRIMARY_TOKENS;
    static const QStringList SECONDARY_TOKENS;
    static const QStringList TERTIARY_TOKENS;
    static const QStringList ERROR_TOKENS;
    static const QStringList SURFACE_TOKENS;
    static const QStringList UTILITY_TOKENS;
};

} // namespace cf::ui::gallery
