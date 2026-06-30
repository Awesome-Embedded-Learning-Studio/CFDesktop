/**
 * @file    apps/calculator/calculator_panel.h
 * @brief   Calculator main panel (standalone app).
 *
 * The root widget of the standalone Calculator executable. Renders the
 * keypad with cfui MD3 widgets (Button grid + Label display) and reuses the
 * ported calculator_core expression parser. CFDesktop launches this binary
 * via AppLaunchService (QProcess), so it runs in its own process.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include <QString>
#include <QWidget>

class QKeyEvent;
class QPaintEvent;

namespace qw::widget::material {
class Label;
}

namespace cf::desktop::desktop_component {

/**
 * @brief  Root widget of the standalone Calculator application.
 *
 * @ingroup calculator
 */
class CalculatorPanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the Calculator panel.
     * @param[in] parent  Parent widget (nullptr for a top-level window).
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    explicit CalculatorPanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the panel.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    ~CalculatorPanel() override;

  protected:
    /**
     * @brief   Paints the rounded Material card background.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief          Dispatches keyboard input to the calculator.
     * @param[in] event  The key event.
     * @throws         None.
     * @since          0.20
     * @ingroup        calculator
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /**
     * @brief          Handles a button token (append, evaluate, clear, etc.).
     * @param[in] text  The button text.
     * @throws         None internally; parser exceptions are caught and shown.
     * @since          0.20
     * @ingroup        calculator
     */
    void onButton(const QString& text);

    /// @brief Syncs the display label with the current expression.
    void refreshDisplay();

    qw::widget::material::Label* display_{nullptr}; ///< Read-out display.
    QString expression_;                            ///< Current expression string.
};

} // namespace cf::desktop::desktop_component
