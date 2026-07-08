/**
 * @file    gauge_widget.h
 * @brief   Circular gauge widget (ported verbatim from CCIMX GaugeWidget).
 *
 * A self-painted semicircular gauge: gradient arc, ticks + labels, animated
 * needle, center hub, title + value text. Used by the disk-usage card to
 * render a percentage as a dial instead of a linear bar.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QColor>
#include <QFont>
#include <QWidget>

namespace cf::desktop::desktop_component {

/**
 * @brief  Circular gauge that renders a numeric value as a dial.
 *
 * Drawn entirely in paintEvent (background, gradient arc, ticks, labels,
 * needle, center, texts). The needle animates to a new value over
 * ANIMATION_DURATION ms via a QPropertyAnimation on the @c value property.
 *
 * @note   Visual constants mirror CCIMX GaugeWidget exactly; theme-less.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class GaugeWidget : public QWidget {
    Q_OBJECT
    /// @brief Animated needle value (drives the needle angle).
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

  public:
    /// @brief Default minimum value of the gauge.
    static constexpr int DEF_MIN_VALUE{0};

    /// @brief Default maximum value of the gauge.
    static constexpr int DEF_MAX_VALUE{100};

    /// @brief Font size for the title text.
    static constexpr short TITLE_FONT_SZ{12};

    /// @brief Font size for the current value text.
    static constexpr short CURVAL_FOUT_SZ{10};

    /// @brief Duration of the needle animation in milliseconds.
    static constexpr int ANIMATION_DURATION{500};

    /// @brief Reference widget width (the dial is scaled to fit the widget).
    static constexpr float WIDGET_MIN_WIDTH{200};

    /// @brief Reference widget height (the dial is scaled to fit the widget).
    static constexpr float WIDGET_MIN_HEIGHT{200};

    /// @brief Base color for the gauge board outline.
    static constexpr QColor BOARD_COLOR = QColor(160, 160, 160);

    /// @brief Gradient start color for the board background.
    static constexpr QColor FROM_BOARD_COLOR = QColor(190, 190, 190);

    /// @brief Gradient end color for the board background.
    static constexpr QColor TO_BOARD_COLOR = QColor(110, 110, 110);

    /// @brief Width of the board outline.
    static constexpr short BOARD_LEN = 1;

    /// @brief Start angle for the arc (degrees, 0 at 3 o'clock, CCW positive).
    static constexpr short START_ANGLE{225};

    /// @brief Total sweep angle of the gauge arc (degrees).
    static constexpr short TOTAL_ANGLE{270};

    /// @brief Width of the color gradient arc pen.
    static constexpr short COLOR_GRAD_WIDTH{8};

    /// @brief Radius of the color gradient arc.
    static constexpr short COLOR_RADIUS{90};

    /// @brief Gradient start color of the arc (low end).
    static constexpr QColor START_COLOR = QColor(255, 0, 0);

    /// @brief Gradient end color of the arc (high end).
    static constexpr QColor END_COLOR = QColor(0, 255, 0);

    /// @brief Total number of ticks around the arc.
    static constexpr int TICK_CNT{55};

    /// @brief Pen width of main ticks.
    static constexpr short MAIN_TICK_WIDTH{2};

    /// @brief Pen width of sub ticks.
    static constexpr short SUB_TICK_WIDTH{1};

    /// @brief Length of main ticks.
    static constexpr short MAIN_TICK_LENGTH{8};

    /// @brief Length of sub ticks.
    static constexpr short SUB_TICK_LENGTH{5};

    /// @brief Number of sub ticks per main tick.
    static constexpr short SUB_MAIN_RATE{5};

    /// @brief Color of the ticks.
    static constexpr QColor TICK_COLOR = QColor(255, 255, 255);

    /// @brief Font size for the tick labels.
    static constexpr short LABEL_FONT_SZ{6};

    /// @brief Color for the tick labels.
    static constexpr QColor LABEL_COLOR = QColor(255, 255, 255);

    /**
     * @brief  Constructs the gauge.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit GaugeWidget(QWidget* parent = nullptr);

    /**
     * @brief  Sets the gauge value range.
     * @param[in]  min  Minimum value (needle at the arc start).
     * @param[in]  max  Maximum value (needle at the arc end).
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setRange(int min, int max) {
        min_value = min;
        max_value = max;
        update();
    }

    /**
     * @brief  Sets the title text drawn at the top of the dial.
     * @param[in]  title  Title text.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setTitle(const QString& title) {
        this->title = title;
        update();
    }

    /**
     * @brief  Sets the unit text drawn beside the value.
     * @param[in]  unit  Unit text (e.g. "%").
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setUnit(const QString& unit) {
        this->unit = unit;
        update();
    }

    /**
     * @brief   Returns the current gauge value.
     * @return  Current displayed value.
     * @throws  None
     * @note    None
     * @warning None
     * @since   0.19.0
     */
    double value() const { return current_value; }

    /**
     * @brief  Animates the needle to a new value.
     * @param[in]  value  New value to display.
     * @return     None
     * @throws     None
     * @note       Uses a QPropertyAnimation over ANIMATION_DURATION ms.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void update_value(double value);

  signals:
    /**
     * @brief  Emitted when the gauge value changes.
     * @param[in]  value  The new value.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void valueChanged(double value);

  protected:
    /**
     * @brief  Renders the gauge.
     * @param[in]  event  Paint event (unused).
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /**
     * @brief  Sets the value internally and emits valueChanged.
     * @param[in]  val  New value.
     * @return     None
     * @throws     None
     * @note       Driven by the QPropertyAnimation; not for manual calls.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setValue(double val) {
        if (qFuzzyCompare(current_value, val)) {
            return;
        }
        current_value = val;
        emit valueChanged(val);
        update();
    }

    /// @brief Draws the board background + radial glow.
    void drawBackground(QPainter& p);
    /// @brief Draws the red-to-green conical gradient arc.
    void drawArc(QPainter& p);
    /// @brief Draws the tick marks around the arc.
    void drawTicks(QPainter& p);
    /// @brief Draws numeric labels at the main ticks.
    void drawLabels(QPainter& p);
    /// @brief Draws the needle (with drop shadow) at the current value.
    void drawNeedle(QPainter& p);
    /// @brief Draws the center hub.
    void drawCenter(QPainter& p);
    /// @brief Draws the title and value+unit texts.
    void drawTexts(QPainter& p);

    double min_value{DEF_MIN_VALUE}; ///< Minimum value of the gauge.
    double max_value{DEF_MAX_VALUE}; ///< Maximum value of the gauge.
    double current_value{0.0};       ///< Current displayed value.

    QFont title_font{"Arial", TITLE_FONT_SZ, QFont::Bold};  ///< Title font.
    QFont value_font{"Arial", CURVAL_FOUT_SZ, QFont::Bold}; ///< Value font.
    QFont label_font{"Arial", LABEL_FONT_SZ};               ///< Label font.

    QString title; ///< Title text.
    QString unit;  ///< Unit text.
};

} // namespace cf::desktop::desktop_component
