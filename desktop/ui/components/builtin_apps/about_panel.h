/**
 * @file    about_panel.h
 * @brief   In-process "About CFDesktop" panel (builtin app).
 *
 * AboutPanel is a frameless child widget shown over the desktop when the
 * "builtin:about" app entry is launched. It renders a rounded Material card
 * with the CFDesktop version and the running target, and dismisses on a tap.
 * As a child of the desktop it renders inside the single desktop window, so it
 * does not fight an external app for the framebuffer on linuxfb.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QString>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QRect;

namespace cf::desktop::desktop_component {

/**
 * @brief  Builtin "About" panel shown over the desktop.
 *
 * Demonstrates an in-process (non-QProcess) app: clicking the matching launcher
 * tile shows this panel; a tap anywhere hides it. No external binary is needed,
 * which suits the single-framebuffer linuxfb target.
 *
 * @ingroup components
 */
class AboutPanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the About panel.
     *
     * @param[in] parent  Owning widget (the desktop surface).
     *
     * @throws None
     * @note   The panel starts hidden; call popup() to show it.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    explicit AboutPanel(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the panel.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    ~AboutPanel() override;

    /**
     * @brief  Sizes, centers over the available area, and shows the panel.
     *
     * @param[in] available  The free screen geometry (excludes docked panels).
     *
     * @throws None
     * @note   A null/empty rect centers on the primary screen.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void popup(const QRect& available);

    /**
     * @brief  Hides the panel.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void hidePanel();

  protected:
    /**
     * @brief  Paints the rounded card with the version and target text.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Dismisses the panel on a left-button release.
     *
     * @param[in] event  The mouse event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    QString version_text_; ///< First line (version).
    QString target_text_;  ///< Second line (running target).
};

} // namespace cf::desktop::desktop_component
