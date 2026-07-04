/**
 * @file    boot_progress_widget.h
 * @brief   Abstract base class for boot progress widgets.
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */
#pragma once
#include "boot_stage_info.h"
#include "init_stage.h"
#include <QRect>
#include <QWidget>

namespace cf::desktop::init_session {

/**
 * @class BootProgressWidget
 * @brief Abstract base class for boot progress widgets.
 *
 * @details Provides the interface for widgets that display boot progress
 *          during application initialization. Subclasses implement the
 *          visual representation of boot stages.
 *
 * @since  0.1
 * @ingroup desktop_ui_widget_init_session
 */
class BootProgressWidget : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QWidget parent. Default is nullptr.
     *
     * @throws        None
     * @note          Initializes the base widget.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    BootProgressWidget(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Virtual destructor for proper cleanup in derived classes.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    virtual ~BootProgressWidget();

    /**
     * @brief  Sets the total number of boot stages.
     *
     * @param[in]     stage_tol Total number of stages. Unit: count.
     *
     * @throws        None
     * @note          Stores the total stage count for progress calculations.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void setTotalStage(size_t stage_tol) { stage_to_ = stage_tol; }

    /**
     * @brief  Sets the window size to half of the screen size and centers it.
     *
     * @throws        None
     * @note          Default: Uses the primary screen geometry to calculate half size.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    virtual void setInitialDisplayStatus();

  public slots:
    /**
     * @brief  Slot that handles the start of a boot stage.
     *
     *        Called externally when a new boot stage begins. Forwards
     *        the stage information to the pure virtual stage_start method.
     *
     * @param[in]     bs_info The boot stage information containing stage
     *                        name and progress data.
     *
     * @throws        None
     * @note          Delegates to the virtual stage_start method.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void do_stage_start(const BootStageInfo& bs_info) { stage_start(bs_info); }

  protected:
    /**
     * @brief  Pure virtual method for handling boot stage start.
     *
     *        Called by do_stage_start when a new boot stage begins.
     *        Subclasses must implement this to update their visual
     *        representation of the boot progress.
     *
     * @param[in]     bs_info The boot stage information containing stage
     *                        name and progress data.
     *
     * @throws        None
     * @note          Subclasses implement this to provide custom visuals.
     * @warning       Must be implemented by all derived classes.
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    virtual void stage_start(const BootStageInfo& bs_info) = 0;

  protected:
    size_t stage_to_{0};
};

/**
 * @brief  Boot progress widget style selection.
 *
 * @since 0.1
 * @ingroup desktop_ui_widget_init_session
 */
enum class BootstrapStyle {
    Simple,     ///< Simple minimalist boot progress widget
    Perfect,    ///< Enhanced boot progress widget with additional features
    SelfDefined ///< User-defined custom boot progress widget
};

/**
 * @brief  Factory function for creating boot progress widgets.
 *
 * @param[in]     s The style of boot progress widget to create.
 *
 * @return        Pointer to the created BootProgressWidget instance.
 * @throws        None
 * @note          Caller takes ownership of the returned widget.
 * @warning       Returns nullptr if style is not supported.
 * @since         0.1
 * @ingroup       desktop_ui_widget_init_session
 */
BootProgressWidget* GetBootWidget(BootstrapStyle s);

} // namespace cf::desktop::init_session
