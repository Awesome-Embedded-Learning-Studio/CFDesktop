/**
 * @file MaterialTypographyMainWindow.h
 * @brief Material Design 3 Typography Gallery - Main Window
 *
 * A visual gallery to display all Material Design 3 Type Scale tokens
 * with 15 font styles organized in 5 categories.
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
#include <QTimer>
#include <QPropertyAnimation>

#include "ui/core/material/cfmaterial_fonttype.h"

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
 * @brief Font card widget displaying a single typography token.
 *
 * Shows:
 * - Font preview text
 * - Token name (e.g., "md.typography.displayLarge")
 * - Font properties (size, weight, line-height)
 *
 * Click to copy CSS style to clipboard.
 */
class FontCardWidget : public QWidget {
    Q_OBJECT
  public:
    explicit FontCardWidget(const QString& tokenName, const QFont& font,
                           float lineHeight, const QString& previewText,
                           QWidget* parent = nullptr);

    void updateFont(const QFont& font, float lineHeight);

    QSize sizeHint() const override;

  signals:
    void clicked(const QString& cssStyle);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    QString generateCssStyle() const;

    QString tokenName_;
    QFont font_;
    float lineHeight_;
    QString previewText_;
    QString cssStyle_;
    bool isHovered_ = false;
};

/**
 * @brief Main window for the Material Typography Gallery.
 *
 * Displays all 15 Material Design 3 Type Scale tokens organized in groups:
 * - Display Styles (3)
 * - Headline Styles (3)
 * - Title Styles (3)
 * - Body Styles (3)
 * - Label Styles (3)
 */
class MaterialTypographyMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialTypographyMainWindow(QWidget* parent = nullptr);
    ~MaterialTypographyMainWindow() override;

  private:
    void setupUI();
    void createHeader();
    void createFontGroups();
    int calculateColumnCount() const;

    void createFontGroup(const QString& title, QGridLayout* layout, int& row,
                        const QStringList& tokens);

    QString getPreviewText(const QString& token) const;
    void showToast(const QString& message);

  private slots:
    void onFontCardClicked(const QString& cssStyle);

  private:
    cf::ui::core::MaterialTypography typography_;

    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QHBoxLayout* headerLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* fontGridLayout_;

    // Header components
    QLabel* titleLabel_;

    // Toast
    ToastWidget* toast_;

    // Font cards storage
    struct FontCardInfo {
        FontCardWidget* widget;
        QString token;
    };
    QList<FontCardInfo> fontCards_;

    // 15 typography tokens grouped by category
    static const QStringList DISPLAY_TOKENS;
    static const QStringList HEADLINE_TOKENS;
    static const QStringList TITLE_TOKENS;
    static const QStringList BODY_TOKENS;
    static const QStringList LABEL_TOKENS;
};

} // namespace cf::ui::gallery
