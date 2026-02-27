/**
 * @file ThemeSidebar.h
 * @brief Left sidebar with tab items for navigation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include <QWidget>
#include <QList>

namespace cf::ui::core {
struct ICFTheme;
}

namespace cf::ui::gallery {

/**
 * @brief Left sidebar navigation widget with clickable tab items.
 *
 * Displays 4 tab items with emoji icons + text labels:
 * - 🎨 Color Scheme (色彩方案)
 * - ⚡ Motion Spec (动效规范)
 * - 🔲 Radius Scale (圆角规范)
 * - 🔤 Typography (字体规范)
 *
 * Provides visual feedback for active/hover states.
 */
class ThemeSidebar : public QWidget {
    Q_OBJECT

  public:
    explicit ThemeSidebar(QWidget* parent = nullptr);
    ~ThemeSidebar() override = default;

    /**
     * @brief Set the active tab index
     * @param index Tab index (0-3)
     */
    void setActiveIndex(int index);

    /**
     * @brief Get the current active tab index
     * @return Active tab index
     */
    int activeIndex() const { return activeIndex_; }

    /**
     * @brief Apply theme colors to the sidebar
     * @param theme Theme to apply
     */
    void applyTheme(const cf::ui::core::ICFTheme& theme);

  signals:
    /**
     * @brief Emitted when a tab is clicked
     * @param index The clicked tab index
     */
    void tabClicked(int index);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

  private:
    struct TabItem {
        QString icon;        // Emoji icon
        QString label;       // Text label (Chinese)
        QString labelEn;     // English label (optional)
    };

    /**
     * @brief Get the tab item at a given position
     * @param pos Position in widget coordinates
     * @return Tab index, or -1 if not on a tab
     */
    int tabAtPosition(const QPoint& pos) const;

    /**
     * @brief Get the bounding rect for a tab item
     * @param index Tab index
     * @return Rectangle for the tab
     */
    QRect tabRect(int index) const;

    QList<TabItem> tabs_;
    int activeIndex_ = 0;
    int hoverIndex_ = -1;

    // Theme colors
    QColor backgroundColor_;
    QColor activeColor_;
    QColor hoverColor_;
    QColor textColor_;
    QColor iconColor_;
    QColor borderColor_;
};

} // namespace cf::ui::gallery
