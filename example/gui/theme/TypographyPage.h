/**
 * @file TypographyPage.h
 * @brief Typography page for the Material Gallery
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include "ThemePageWidget.h"
#include "ToastWidget.h"
#include "ui/core/material/cfmaterial_fonttype.h"
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

namespace cf::ui::core {
struct ICFTheme;
}

namespace cf::ui::gallery {

/**
 * @brief Font card widget displaying a single typography token.
 */
class FontCardWidget : public QWidget {
    Q_OBJECT
  public:
    explicit FontCardWidget(const QString& tokenName, const QFont& font, float lineHeight,
                            const QString& previewText, QWidget* parent = nullptr);

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
 * @brief Typography page - displays all 15 Material Design 3 typography tokens.
 *
 * Organized in 5 categories:
 * - Display Styles (3)
 * - Headline Styles (3)
 * - Title Styles (3)
 * - Body Styles (3)
 * - Label Styles (3)
 */
class TypographyPage : public ThemePageWidget {
    Q_OBJECT

  public:
    explicit TypographyPage(QWidget* parent = nullptr);
    ~TypographyPage() override = default;

    QString pageTitle() const override { return "字体规范"; }
    void applyTheme(const cf::ui::core::ICFTheme& theme) override;

  private:
    void setupUI();
    void createFontGroups();
    int calculateColumnCount() const;

    void createFontGroup(const QString& title, QGridLayout* layout, int& row,
                         const QStringList& tokens);

    QString getPreviewText(const QString& token) const;
    void showToast(const QString& message);

  private slots:
    void onFontCardClicked(const QString& cssStyle);

  private:
    const cf::ui::core::ICFTheme* theme_ = nullptr;

    // UI components
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    QGridLayout* fontGridLayout_;

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

    // Theme colors
    QColor cardBgColor_;
    QColor cardBorderColor_;
    QColor textColor_;
};

} // namespace cf::ui::gallery
