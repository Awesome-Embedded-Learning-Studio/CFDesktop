/**
 * @file    simple_boot_widget.h
 * @brief   Simple Boot Widget for CFDesktop startup screen.
 *
 * A clean, modern startup screen featuring:
 * - Logo and branding at top center
 * - Step-based progress indicator with visual dots
 * - Detailed progress bar with percentage
 * - Current step description
 * - Version and copyright information
 * - Light minimalist color scheme
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */
#pragma once
#include "boot_progress_widget.h"
#include "boot_stage_info.h"

#include <QWidget>

namespace Ui {
class SimpleBootWidget;
}

namespace cf::desktop::init_session {

/**
 * @brief Step information for the segmented progress indicator.
 *
 * @since 0.1
 * @ingroup desktop_ui_widget_init_session
 */
struct StepInfo {
    QString name;   ///< Display name of the step
    bool completed; ///< Whether the step is completed
    bool active;    ///< Whether the step is currently active

    StepInfo() : completed(false), active(false) {}
    explicit StepInfo(const QString& n) : name(n), completed(false), active(false) {}
};

/**
 * @class SimpleBootWidget
 * @brief A modern, minimalist startup screen widget.
 *
 * @details This widget provides a clean startup experience with:
 *          - Logo and application branding
 *          - Segmented progress indicator with animated transitions
 *          - Material Design-style progress bar
 *          - Dynamic step descriptions
 *          - Version and copyright information
 *
 * @since  0.1
 * @ingroup desktop_ui_widget_init_session
 */
class SimpleBootWidget : public BootProgressWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QWidget parent.
     *
     * @throws        None
     * @note          Initializes UI and loads the application logo.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    explicit SimpleBootWidget(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Cleans up UI resources.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    ~SimpleBootWidget() override;

    /**
     * @brief  Sets the slogan/tagline.
     *
     * @param[in]     slogan The slogan text to display.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void set_slogan(const QString& slogan);

    /**
     * @brief  Enables or disables the background animation.
     *
     * @param[in]     enabled true to enable animation, false to disable.
     *
     * @throws        None
     * @note          Background animation uses a subtle gradient shift.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void set_background_animation_enabled(bool enabled);

  protected:
    /**
     * @brief  Handles the start of a boot stage.
     *
     *        Called when a new boot stage begins to update the progress
     *        indicator and step visuals.
     *
     * @param[in]     bs_info The boot stage information containing stage
     *                        name and progress data.
     *
     * @throws        None
     * @note          Updates the current step index and refreshes visuals.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void stage_start(const BootStageInfo& bs_info) override;

    /**
     * @brief  Paint event handler for custom drawing.
     *
     * @param[in]     event The paint event.
     *
     * @throws        None
     * @note          Draws the animated background, connector lines, and logo watermark.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /**
     * @brief  Initializes the UI from the .ui file.
     *
     * @throws        None
     * @note          Sets up the UI components and loads default resources.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void setup_ui();

    /**
     * @brief  Creates the step indicator dots and labels.
     *
     * @throws        None
     * @note          Clears existing widgets and rebuilds the step indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void create_step_indicator();

    /**
     * @brief  Updates the visual state of all step indicators.
     *
     * @throws        None
     * @note          Refreshes the appearance of dots and labels.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void update_step_visuals();

    /**
     * @brief  Loads the application logo from resources.
     *
     * @throws        None
     * @note          Attempts to load from :/early_gui_welcome/CFDesktop.logo.png
     * @warning       Falls back to a text-based placeholder if image not found.
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void load_logo();

    Ui::SimpleBootWidget* ui;          ///< Qt Designer UI interface
    QList<StepInfo> m_steps;           ///< List of step information
    int m_currentStepIndex;            ///< Current active step index
    QPixmap m_logoPixmap;              ///< Cached logo pixmap
    QTimer* m_animationTimer;          ///< Timer for background animation
    float m_animationProgress;         ///< Current animation progress (0-1)
    bool m_backgroundAnimationEnabled; ///< Whether background animation is enabled
};

} // namespace cf::desktop::init_session
