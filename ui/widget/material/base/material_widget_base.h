/**
 * @file    material_widget_base.h
 * @brief   Common Material Design widget behavior composition helper.
 *
 * Encapsulates the shared initialization, signal connections, and event
 * forwarding used by all interactive Material Design 3 widgets.
 *
 * @ingroup ui_widget_material_base
 */

#pragma once

#include "base/include/base/weak_ptr/weak_ptr.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "export.h"
#include "focus_ring.h"
#include "ripple_helper.h"
#include "state_machine.h"
#include "widget/material/base/elevation_controller.h"

#include <QEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QObject>
#include <QRectF>
#include <QWidget>

namespace cf::ui::widget::material::base {

/**
 * @brief  Composition helper for Material Design widget behavior.
 *
 * @details Encapsulates the common initialization, signal connections,
 *          and event forwarding shared by all interactive Material Design 3
 *          widgets. Uses a Config struct to control which helper components
 *          are created.
 *
 *          This is a "has-a" composition, not inheritance, because widgets
 *          inherit from different Qt base classes (QPushButton, QCheckBox,
 *          QSlider, etc.).
 *
 * @note    The owner QWidget must remain valid for the lifetime of this object.
 * @warning Do not use with non-interactive widgets (Label, Separator).
 * @since   0.1
 * @ingroup ui_widget_material_base
 *
 * @code
 * // In widget constructor:
 * m_material(this, MaterialWidgetBase::Config{
 *     .useElevation = true,
 *     .initialElevation = 2
 * });
 *
 * // In event handlers:
 * void MyWidget::enterEvent(QEnterEvent* event) {
 *     QPushButton::enterEvent(event);
 *     m_material.onEnterEvent();
 * }
 * @endcode
 */

/**
 * @brief  Configuration for MaterialWidgetBase helper creation.
 *
 * @since  0.1
 * @ingroup ui_widget_material_base
 */
struct MaterialWidgetBaseConfig {
    bool useRipple = true;                                       ///< Create RippleHelper.
    bool useElevation = false;                                   ///< Create MdElevationController.
    bool useFocusIndicator = true;                               ///< Create MdFocusIndicator.
    RippleHelper::Mode rippleMode = RippleHelper::Mode::Bounded; ///< Ripple mode.
    int initialElevation = 0;                                    ///< Initial elevation level.
};

class CF_UI_EXPORT MaterialWidgetBase : public QObject {
    Q_OBJECT
  public:
    using Config = MaterialWidgetBaseConfig;

    /**
     * @brief  Construct and initialize helper components.
     *
     * @param[in] owner  The parent widget (must not be null).
     * @param[in] config Configuration controlling which helpers to create.
     *
     * @throws        None
     * @note          Connects repaint signals to owner's update() slot.
     * @warning        owner must remain valid for the lifetime of this object.
     * @since          0.1
     * @ingroup        ui_widget_material_base
     */
    MaterialWidgetBase(QWidget* owner, const Config& config = Config{});

    ~MaterialWidgetBase() override = default;

    // Non-copyable, movable
    MaterialWidgetBase(const MaterialWidgetBase&) = delete;
    MaterialWidgetBase& operator=(const MaterialWidgetBase&) = delete;

    // --- Event forwarding ---

    /**
     * @brief  Forward hover enter to helpers and trigger repaint.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onEnterEvent();

    /**
     * @brief  Forward hover leave to helpers and trigger repaint.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onLeaveEvent();

    /**
     * @brief  Forward mouse press to helpers and trigger repaint.
     *
     * @param[in] pos    Mouse position relative to the widget.
     * @param[in] bounds Widget bounds for ripple calculation.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onMousePress(const QPoint& pos, const QRectF& bounds);

    /**
     * @brief  Forward mouse release to helpers and trigger repaint.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onMouseRelease();

    /**
     * @brief  Forward focus-in to helpers and trigger repaint.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onFocusIn();

    /**
     * @brief  Forward focus-out to helpers and trigger repaint.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onFocusOut();

    /**
     * @brief  Forward enabled-state change to helpers and trigger repaint.
     *
     * @param[in] enabled Whether the widget is now enabled.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    void onEnabledChange(bool enabled);

    // --- Accessors ---

    /**
     * @brief  Get the state machine helper.
     *
     * @return Pointer to StateMachine, or nullptr if not created.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    StateMachine* stateMachine() const { return m_stateMachine; }

    /**
     * @brief  Get the ripple helper.
     *
     * @return Pointer to RippleHelper, or nullptr if not created.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    RippleHelper* ripple() const { return m_ripple; }

    /**
     * @brief  Get the elevation controller.
     *
     * @return Pointer to MdElevationController, or nullptr if not created.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    MdElevationController* elevation() const { return m_elevation; }

    /**
     * @brief  Get the focus indicator.
     *
     * @return Pointer to MdFocusIndicator, or nullptr if not created.
     *
     * @since  0.1
     * @ingroup ui_widget_material_base
     */
    MdFocusIndicator* focusIndicator() const { return m_focusIndicator; }

  private:
    QWidget* m_owner;
    StateMachine* m_stateMachine = nullptr;
    RippleHelper* m_ripple = nullptr;
    MdElevationController* m_elevation = nullptr;
    MdFocusIndicator* m_focusIndicator = nullptr;
};

} // namespace cf::ui::widget::material::base
