/**
 * @file    ui/widget/material/widget/scrollview/scrollview.h
 * @brief   Material Design 3 ScrollView widget.
 *
 * Implements Material Design 3 scroll area with custom scrollbars,
 * fade effects, and theme integration. Supports both horizontal and
 * vertical scrolling with proper Material Design styling.
 *
 * @author  Material Design Framework Team
 * @date    2026-03-18
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "aex/weak_ptr/weak_ptr.h"
#include "base/color.h"
#include "cfmaterial_animation_factory.h"
#include "export.h"
#include <QElapsedTimer>
#include <QScrollArea>
#include <QTimer>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

// Forward declaration of internal overlay class
class ScrollbarOverlay;

/**
 * @brief Scrollbar interaction states for Material Design 3.
 *
 * Defines the various visual states a scrollbar can be in
 * following Material Design 3 state layer specifications.
 *
 * @since 0.1
 * @ingroup ui_widget_material_widget
 */
enum class ScrollbarState {
    /** Idle state - 40% opacity, 12dp width */
    Idle,
    /** Hovered state - 100% opacity, 16dp width */
    Hovered,
    /** Dragged/Pressed state - 100% opacity, 16dp width, state layer overlay */
    Dragged
};

/**
 * @brief  Material Design 3 ScrollView widget.
 *
 * @details Implements Material Design 3 scroll area with custom scrollbars
 *          following Material Design 3 specifications. Features include:
 *          - Custom scrollbar drawing (12dp width)
 *          - Scrollbar fade in/out effects
 *          - Proper Material Design colors
 *          - Support for both horizontal and vertical scrollbars
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT ScrollView : public QScrollArea {
    Q_OBJECT
    Q_PROPERTY(bool scrollbarFadeEnabled READ scrollbarFadeEnabled WRITE setScrollbarFadeEnabled)
    Q_PROPERTY(int scrollbarFadeDelay READ scrollbarFadeDelay WRITE setScrollbarFadeDelay)
    Q_PROPERTY(
        bool scrollbarHoverExpansion READ scrollbarHoverExpansion WRITE setScrollbarHoverExpansion)

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Initializes with default Material Design styling.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit ScrollView(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Components are parented to this; Qt deletes them.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~ScrollView() override;

    /**
     * @brief  Gets whether scrollbar fade effect is enabled.
     *
     * @return        true if fade effect is enabled, false otherwise.
     *
     * @throws        None
     * @note          Fade effect hides scrollbars when not scrolling.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool scrollbarFadeEnabled() const;

    /**
     * @brief  Sets whether scrollbar fade effect is enabled.
     *
     * @param[in]     enabled true to enable fade effect, false to disable.
     *
     * @throws        None
     * @note          When enabled, scrollbars fade out after inactivity.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setScrollbarFadeEnabled(bool enabled);

    /**
     * @brief  Gets the scrollbar fade delay in milliseconds.
     *
     * @return        Fade delay in milliseconds.
     *
     * @throws        None
     * @note          Default is 500ms.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int scrollbarFadeDelay() const;

    /**
     * @brief  Sets the scrollbar fade delay in milliseconds.
     *
     * @param[in]     delay Fade delay in milliseconds.
     *
     * @throws        None
     * @note          Scrollbars fade after this period of inactivity.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setScrollbarFadeDelay(int delay);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the scroll view.
     *
     * @throws        None
     * @note          Based on content size and scrollbar space.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets whether scrollbar hover expansion is enabled.
     *
     * @return        true if hover expansion is enabled, false otherwise.
     *
     * @throws        None
     * @note          When enabled, scrollbar expands from 12dp to 16dp on hover.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool scrollbarHoverExpansion() const;

    /**
     * @brief  Sets whether scrollbar hover expansion is enabled.
     *
     * @param[in]     enabled true to enable hover expansion, false to disable.
     *
     * @throws        None
     * @note          Follows Material Design 3 hover interaction pattern.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setScrollbarHoverExpansion(bool enabled);

  protected:
    /**
     * @brief  Paints the scroll view and custom scrollbars.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Completely overrides default scrollbar drawing.
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
     * @note          Updates scrollbar geometry.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief  Handles scroll event.
     *
     * @param[in]     event Scroll event.
     *
     * @throws        None
     * @note          Shows scrollbars and resets fade timer.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void scrollContentsBy(int dx, int dy) override;

    /**
     * @brief  Handles mouse enter event.
     *
     * @param[in]     event Enter event.
     *
     * @throws        None
     * @note          Shows scrollbars on hover.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief  Handles mouse leave event.
     *
     * @param[in]     event Leave event.
     *
     * @throws        None
     * @note          Starts fade timer when leaving widget.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief  Handles mouse press event.
     *
     * @param[in]     event Mouse press event.
     *
     * @throws        None
     * @note          Detects scrollbar thumb drag start.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse move event.
     *
     * @param[in]     event Mouse move event.
     *
     * @throws        None
     * @note          Tracks hover over scrollbars and thumb drag.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse release event.
     *
     * @param[in]     event Mouse release event.
     *
     * @throws        None
     * @note          Ends scrollbar thumb drag.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

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
     * @brief  Timer event for scrollbar fade effect.
     *
     * @param[in]     event Timer event.
     *
     * @throws        None
     * @note          Handles fade animation timer.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void timerEvent(QTimerEvent* event) override;

    /**
     * @brief  Event filter for viewport resize/move tracking.
     *
     * @param[in]     obj Watched object.
     * @param[in]     ev Event.
     * @return        true if event was handled, false otherwise.
     *
     * @throws        None
     * @note          Keeps overlay synchronized with viewport geometry.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool eventFilter(QObject* obj, QEvent* ev) override;

  private:
    // Internal overlay mouse event handlers (called by ScrollbarOverlay)
    void overlayMousePressEvent(QMouseEvent* event);
    void overlayMouseMoveEvent(QMouseEvent* event);
    void overlayMouseReleaseEvent(QMouseEvent* event);
    // Handle scrollbar mouse events from eventFilter
    void handleScrollbarMouseEvent(QMouseEvent* event, const QPoint& pos);
    // Drawing helpers
    void drawVerticalScrollbar(QPainter& p);
    void drawHorizontalScrollbar(QPainter& p);
    void drawScrollbarTrack(QPainter& p, const QRectF& trackRect, Qt::Orientation orientation);
    void drawScrollbarThumb(QPainter& p, const QRectF& thumbRect, Qt::Orientation orientation);
    void drawStateLayer(QPainter& p, const QRectF& rect, const QColor& color, float opacity);

    // Scrollbar geometry calculations (using absolute coordinates)
    QRectF verticalScrollbarTrackRect() const;
    QRectF verticalScrollbarThumbRect() const;
    QRectF horizontalScrollbarTrackRect() const;
    QRectF horizontalScrollbarThumbRect() const;

    // Color access methods
    CFColor backgroundColor() const;
    CFColor scrollbarTrackColor() const;
    CFColor scrollbarThumbColor() const;
    CFColor scrollbarStateLayerColor() const;

    // Unit conversion helpers
    float scrollBarWidth() const;
    float scrollBarMargin() const;
    float scrollBarCornerRadius() const;
    float scrollBarOpacity() const;
    float scrollBarExpandedWidth() const;
    float currentScrollBarWidth() const;

    // State helpers
    ScrollbarState scrollbarState() const;
    void setScrollbarState(ScrollbarState state);
    bool isPointOverVerticalThumb(const QPoint& pos) const;
    bool isPointOverHorizontalThumb(const QPoint& pos) const;
    bool isPointOverVerticalTrack(const QPoint& pos) const;
    bool isPointOverHorizontalTrack(const QPoint& pos) const;

    // Animation helpers
    void updateWidthAnimation();
    void updateOpacityAnimation();
    int deltaPosToScroll(int delta, float maxThumbTravel, int range);
    friend class ScrollbarOverlay;
    // Internal overlay widget (opaque pointer, defined in .cpp)
    ScrollbarOverlay* m_overlay = nullptr;

    // Fade effect helpers
    void showScrollbars();
    void hideScrollbars();
    void startFadeTimer();
    void stopFadeTimer();

    // Behavior components
    aex::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;

    // Scrollbar state
    bool m_scrollbarFadeEnabled = true;
    bool m_scrollbarHoverExpansion = true;
    int m_scrollbarFadeDelay = 500; // milliseconds
    int m_fadeTimerId = 0;
    float m_scrollbarOpacity = 0.4f; // Initial opacity (idle state)
    float m_targetOpacity = 0.4f;
    float m_currentWidth = 12.0f; // Current animated width in dp
    float m_targetWidth = 12.0f;  // Target width in dp
    bool m_isHovering = false;
    bool m_isScrolling = false;
    bool m_isDraggingThumb = false;
    Qt::Orientation m_dragOrientation = Qt::Vertical;
    int m_dragStartPosition = 0;
    int m_dragScrollValue = 0;

    // Hover state tracking
    bool m_isHoveringVerticalThumb = false;
    bool m_isHoveringHorizontalThumb = false;
    bool m_isHoveringVerticalTrack = false;
    bool m_isHoveringHorizontalTrack = false;

    // Animation state
    QElapsedTimer m_animationTimer;
    static constexpr int ANIMATION_FRAME_MS = 16;          // ~60fps
    static constexpr float ANIMATION_SPEED_WIDTH = 0.3f;   // Width interpolation factor
    static constexpr float ANIMATION_SPEED_OPACITY = 0.2f; // Opacity interpolation factor
};

} // namespace cf::ui::widget::material
