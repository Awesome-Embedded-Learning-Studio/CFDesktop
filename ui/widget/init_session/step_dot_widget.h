/**
 * @file    step_dot_widget.h
 * @brief   Custom widget for drawing step indicator dots.
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */

#pragma once

#include <QPainter>
#include <QPainterPath>
#include <QWidget>

namespace cf::desktop::init_session {

/**
 * @brief Custom widget for drawing step indicator dots.
 *
 * @since 0.1
 * @ingroup desktop_ui_widget_init_session
 */
class StepDotWidget : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Visual state of a step indicator dot.
     *
     * @since 0.1
     * @ingroup desktop_ui_widget_init_session
     */
    enum State {
        Inactive, ///< Step not yet reached
        Active,   ///< Currently on this step
        Completed ///< Step finished
    };

    /**
     * @brief  Constructor.
     *
     * @param[in]     index  Zero-based index of the step in the sequence.
     * @param[in]     name   Display name of the step.
     * @param[in]     parent QWidget parent. Default is nullptr.
     *
     * @throws        None
     * @note          Initializes the widget with Inactive state.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    explicit StepDotWidget(int index, const QString& name, QWidget* parent = nullptr);

    /**
     * @brief  Gets the step index.
     *
     * @return        Zero-based index of the step in the sequence.
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    int stepIndex() const;

    /**
     * @brief  Gets the step name.
     *
     * @return        Display name of the step.
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    QString stepName() const;

    /**
     * @brief  Gets the current state of the step indicator.
     *
     * @return        Current State (Inactive, Active, or Completed).
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    State state() const;

    /**
     * @brief  Sets the state of the step indicator.
     *
     * @param[in]     s New state to set (Inactive, Active, or Completed).
     *
     * @throws        None
     * @note          Triggers a repaint to update the visual appearance.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void setState(State s);

  protected:
    /**
     * @brief  Paint event handler for custom drawing.
     *
     * @param[in]     event The paint event.
     *
     * @throws        None
     * @note          Draws the step dot with appropriate styling based on
     *               state (gray for Inactive, primary color for Active,
     *               accent color for Completed).
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui_widget_init_session
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    int m_index;    ///< Zero-based index of the step
    QString m_name; ///< Display name of the step
    State m_state;  ///< Current visual state of the step
};

} // namespace cf::desktop::init_session
