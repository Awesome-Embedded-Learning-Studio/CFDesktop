/**
 * @file    centered_taskbar.h
 * @brief   Bottom-edge centered taskbar panel.
 *
 * CenteredTaskbar is the bottom-edge panel implementation behind IPanel. It
 * lays out TaskbarIcon tiles in a centered row, paints a translucent Material
 * surface with a top divider, follows the active theme, and forwards tile
 * clicks as appClicked(app_id).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.1
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include "aex/weak_ptr/weak_ptr.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include "app_entry.h"
#include "components/IPanel.h"

#include <QColor>
#include <QList>
#include <QWidget>

class QHBoxLayout;

namespace cf::desktop::desktop_component {

class StartButton;
class TaskbarIcon;

/**
 * @brief  QWidget-based centered taskbar.
 *
 * Implements the bottom-edge panel contract: a centered row of application
 * tiles on a translucent surface. Forwards clicks via appClicked() and is
 * registered with PanelManager like the status bar.
 *
 * @ingroup components
 */
class CenteredTaskbar final : public QWidget, public cf::desktop::IPanel {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the taskbar.
     *
     * @param[in] parent  Owning widget (typically the desktop surface).
     *
     * @throws None
     * @note   Builds the layout and applies the current theme.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    explicit CenteredTaskbar(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the taskbar.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    ~CenteredTaskbar() override;

    // -- IPanel ---------------------------------------------------------------
    /**
     * @brief  Returns the anchoring edge.
     *
     * @return Always PanelPosition::Bottom.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    cf::desktop::PanelPosition position() const override;

    /**
     * @brief  Returns the layout priority.
     *
     * @return Priority value (outermost on the edge).
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    int priority() const override;

    /**
     * @brief  Returns the bar thickness in pixels.
     *
     * @return Preferred height in pixels.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    int preferredSize() const override;

    /**
     * @brief  Returns the widget positioned by the layout engine.
     *
     * @return This taskbar widget.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    QWidget* widget() const override;

    // -- Taskbar API ----------------------------------------------------------
    /**
     * @brief  Rebuilds the tile row from an application list.
     *
     * @param[in] apps  The applications to show, centered in the bar.
     *
     * @throws None
     * @note   Replaces any previously shown tiles.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setApps(const QList<AppEntry>& apps);

    /**
     * @brief  Updates the running indicator for one application.
     *
     * @param[in] app_id   The application identifier to update.
     * @param[in] running  true to mark it running.
     *
     * @throws None
     * @note   No-op when app_id is not shown.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void updateRunningState(const QString& app_id, bool running);

    /**
     * @brief  Returns a weak reference to this taskbar.
     *
     * @return aex::WeakPtr valid for this instance's lifetime.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    aex::WeakPtr<CenteredTaskbar> GetWeak() const { return weak_factory_.GetWeakPtr(); }

  signals:
    /**
     * @brief  Emitted when a tile is clicked.
     *
     * @param[in] app_id  The clicked application identifier.
     *
     * @since 0.19
     * @ingroup components
     */
    void appClicked(const QString& app_id);

    /**
     * @brief  Emitted to request the application launcher (reserved for MS4).
     *
     * @since 0.19
     * @ingroup components
     */
    void launcherRequested();

  protected:
    /**
     * @brief  Paints the translucent surface and top divider.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme colors are resolved in applyTheme().
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// @brief Creates the centered row layout.
    void setupUi();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();

    QHBoxLayout* layout_{nullptr};       ///< Outer row: start button + centered icons.
    QHBoxLayout* icon_layout_{nullptr};  ///< Dynamic icon row. Ownership: this widget.
    StartButton* start_button_{nullptr}; ///< Launcher trigger. Ownership: this widget.
    QList<TaskbarIcon*> icons_;          ///< Current tiles. Ownership: Qt parented.

    QColor background_color_; ///< Surface fill for the bar.
    QColor divider_color_;    ///< Top hairline divider color.

    /// Weak pointer factory (must be the last member).
    mutable aex::WeakPtrFactory<CenteredTaskbar> weak_factory_{this};
};

} // namespace cf::desktop::desktop_component
