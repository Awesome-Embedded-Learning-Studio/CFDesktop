/**
 * @file    ui/widget/material/widget/textarea/textarea.h
 * @brief   Material Design 3 TextArea widget.
 *
 * Implements Material Design 3 text area with support for filled and outlined
 * variants. Includes floating labels, character counter, helper/error text,
 * and multi-line text input.
 *
 * @author  CFDesktop Team
 * @date    2026-03-01
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTextEdit>
#include <QWidget>

#include "base/color.h"
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "cfmaterial_animation_factory.h"
#include "export.h"

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class StateMachine;
class RippleHelper;
class MdFocusIndicator;
} // namespace base

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 TextArea widget.
 *
 * @details Implements Material Design 3 text area with support for filled
 *          and outlined variants. Includes floating labels, character counter,
 *          helper/error text, and multi-line text input with auto-resize support.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT TextArea : public QTextEdit {
    Q_OBJECT
    Q_PROPERTY(TextAreaVariant variant READ variant WRITE setVariant)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString helperText READ helperText WRITE setHelperText)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(bool showCharacterCounter READ showCharacterCounter WRITE setShowCharacterCounter)
    Q_PROPERTY(bool isFloating READ isFloating NOTIFY floatingChanged)
    Q_PROPERTY(int minLines READ minLines WRITE setMinLines)
    Q_PROPERTY(int maxLines READ maxLines WRITE setMaxLines)

  public:
    /**
     * @brief  TextArea visual variant.
     *
     * @since  0.1
     * @ingroup ui_widget_material_widget
     */
    enum class TextAreaVariant { Filled, Outlined };
    Q_ENUM(TextAreaVariant);

    /**
     * @brief  Constructor with variant.
     *
     * @param[in]     variant TextArea visual variant.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit TextArea(TextAreaVariant variant = TextAreaVariant::Filled, QWidget* parent = nullptr);

    /**
     * @brief  Constructor with text and variant.
     *
     * @param[in]     text Initial text content.
     * @param[in]     variant TextArea visual variant.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit TextArea(const QString& text, TextAreaVariant variant = TextAreaVariant::Filled,
                      QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~TextArea() override;

    /**
     * @brief  Gets the text area variant.
     *
     * @return        Current text area variant.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TextAreaVariant variant() const;

    /**
     * @brief  Sets the text area variant.
     *
     * @param[in]     variant TextArea variant to use.
     *
     * @throws        None
     * @note          Changing variant updates the visual appearance.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setVariant(TextAreaVariant variant);

    /**
     * @brief  Gets the label text.
     *
     * @return        Current label text.
     *
     * @throws        None
     * @note          The label floats when the field has focus or content.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QString label() const;

    /**
     * @brief  Sets the label text.
     *
     * @param[in]     label Label text to display.
     *
     * @throws        None
     * @note          The label floats when the field has focus or content.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setLabel(const QString& label);

    /**
     * @brief  Gets the helper text.
     *
     * @return        Current helper text.
     *
     * @throws        None
     * @note          Helper text is displayed below the text area.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QString helperText() const;

    /**
     * @brief  Sets the helper text.
     *
     * @param[in]     text Helper text to display.
     *
     * @throws        None
     * @note          Helper text is displayed below the text area.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setHelperText(const QString& text);

    /**
     * @brief  Gets the error text.
     *
     * @return        Current error text.
     *
     * @throws        None
     * @note          Error text takes precedence over helper text.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QString errorText() const;

    /**
     * @brief  Sets the error text.
     *
     * @param[in]     text Error text to display. Empty string clears error state.
     *
     * @throws        None
     * @note          Error text takes precedence over helper text.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setErrorText(const QString& text);

    /**
     * @brief  Checks if the label is in floating state.
     *
     * @return        true if label is floating, false otherwise.
     *
     * @throws        None
     * @note          Label floats when field has focus or content.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool isFloating() const;

    /**
     * @brief  Gets whether character counter is shown.
     *
     * @return        true if character counter is visible, false otherwise.
     *
     * @throws        None
     * @note          Character counter is shown in the helper text area.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool showCharacterCounter() const;

    /**
     * @brief  Sets whether character counter is shown.
     *
     * @param[in]     show true to show character counter, false to hide.
     *
     * @throws        None
     * @note          Character counter is shown in the helper text area.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setShowCharacterCounter(bool show);

    /**
     * @brief  Gets the maximum length.
     *
     * @return        Maximum character length.
     *
     * @throws        None
     * @note          0 means no maximum.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int maxLength() const;

    /**
     * @brief  Sets the maximum length.
     *
     * @param[in]     length Maximum character length. 0 means no maximum.
     *
     * @throws        None
     * @note          0 means no maximum.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setMaxLength(int length);

    /**
     * @brief  Gets the minimum number of visible lines.
     *
     * @return        Minimum number of lines.
     *
     * @throws        None
     * @note          Default is 1.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int minLines() const;

    /**
     * @brief  Sets the minimum number of visible lines.
     *
     * @param[in]     lines Minimum number of visible lines.
     *
     * @throws        None
     * @note          Affects the minimum height of the text area.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setMinLines(int lines);

    /**
     * @brief  Gets the maximum number of visible lines.
     *
     * @return        Maximum number of lines, or 0 for unlimited.
     *
     * @throws        None
     * @note          0 means no maximum (scrollbar appears).
     * @warning       None
     * @since         0.1
     * @ingroup      ui_widget_material_widget
     */
    int maxLines() const;

    /**
     * @brief  Sets the maximum number of visible lines.
     *
     * @param[in]     lines Maximum number of visible lines, or 0 for unlimited.
     *
     * @throws        None
     * @note          When set, the text area grows up to this many lines.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setMaxLines(int lines);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the text area.
     *
     * @throws        None
     * @note          Based on label, padding, helper text, and line count.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets the minimum recommended size.
     *
     * @return        Minimum recommended size.
     *
     * @throws        None
     * @note          Ensures touch target size requirements.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  signals:
    /**
     * @brief  Signal emitted when floating state changes.
     *
     * @param[in]     floating true if label is floating, false otherwise.
     *
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void floatingChanged(bool floating);

  protected:
    /**
     * @brief  Paints the text area.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements Material Design paint pipeline.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles resize event.
     *
     * @param[in]     event Resize event.
     *
     * @throws        None
     * @note          Updates internal layout calculations.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief  Handles mouse press event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Triggers ripple effect.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse release event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Updates ripple state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles focus in event.
     *
     * @param[in]     event Focus event.
     *
     * @throws        None
     * @note          Updates floating label state and shows focus indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void focusInEvent(QFocusEvent* event) override;

    /**
     * @brief  Handles focus out event.
     *
     * @param[in]     event Focus event.
     *
     * @throws        None
     * @note          Updates floating label state and hides focus indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void focusOutEvent(QFocusEvent* event) override;

    /**
     * @brief  Handles widget state change event.
     *
     * @param[in]     event Change event.
     *
     * @throws        None
     * @note          Updates appearance based on state changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void changeEvent(QEvent* event) override;

    /**
     * @brief  Handles key press event.
     *
     * @param[in]     event Key event.
     *
     * @throws        None
     * @note          Blocks Enter key when maxLines limit is reached.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief  Handles text change event.
     *
     * @param[in]     text New text content.
     *
     * @throws        None
     * @note          Updates floating label state and character counter.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void textChanged();

  private:
    // Drawing helpers - Material Design paint pipeline
    void drawBackground(QPainter& p, const QRectF& fieldRect);
    void drawOutline(QPainter& p, const QRectF& fieldRect);
    void drawHelperText(QPainter& p, const QRectF& helperRect);
    void drawCharacterCounter(QPainter& p, const QRectF& helperRect);
    void drawFocusIndicator(QPainter& p, const QRectF& fieldRect);
    void drawRipple(QPainter& p, const QRectF& fieldRect);

    // Layout helpers
    QRectF fieldRect() const;
    QRectF textRect() const;
    QRectF helperTextRect() const;

    // Animation helpers
    void updateFloatingState(bool shouldFloat);
    void animateFloatingTo(bool floating);
    void updateGeometryForLines();

    // Color access methods
    CFColor containerColor() const;
    CFColor onContainerColor() const;
    CFColor labelColor() const;
    CFColor inputTextColor() const;
    CFColor outlineColor() const;
    CFColor focusOutlineColor() const;
    CFColor errorColor() const;
    CFColor helperTextColor() const;
    float cornerRadius() const;
    QFont inputFont() const;
    QFont labelFont() const;
    QFont helperFont() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdFocusIndicator* m_focusIndicator;

    // Properties
    TextAreaVariant m_variant;
    QString m_label;
    QString m_helperText;
    QString m_errorText;
    bool m_showCharacterCounter;
    int m_maxLength;
    int m_minLines;
    int m_maxLines;

    // Internal state
    bool m_isFloating;
    bool m_hasError;
    float m_floatingProgress; // 0.0 = resting, 1.0 = floating
    bool m_updatingGeometry;
};

} // namespace cf::ui::widget::material
