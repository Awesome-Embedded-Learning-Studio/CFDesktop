/**
 * @file    calculator_builtin_panel.h
 * @brief   In-process builtin adapter for the Calculator app.
 *
 * Wraps the standalone CalculatorPanel (apps/calculator/) as an in-process
 * IBuiltinPanel so the Calculator can run either detached (its own process
 * via the apps/calculator executable) or in-process (this adapter, when the
 * hardware tier prefers in-process apps to save RAM). The CalculatorPanel
 * source is shared between both builds; only this adapter adds the builtin
 * lifecycle (popup/hidePanel over the desktop).
 *
 * @note    Architecture exception: desktop links the apps/calculator panel
 *          source. Tracked for migration to a neutral shared lib in
 *          milestone_05.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "ibuiltin_panel.h"

#include <QWidget>

class QRect;

namespace cf::desktop::desktop_component {

class CalculatorPanel; // forward — full definition needed only in the .cpp

/**
 * @brief  In-process builtin adapter wrapping the standalone CalculatorPanel.
 *
 * Holds a CalculatorPanel child and exposes it through IBuiltinPanel so the
 * launcher grid can surface a "calculator" entry that renders in-process.
 *
 * @ingroup components
 */
class CalculatorBuiltinPanel final : public QWidget, public IBuiltinPanel {
  public:
    /**
     * @brief   Constructs the builtin Calculator adapter.
     *
     * @param[in] parent  Owning widget (the desktop surface).
     *
     * @throws  None
     * @note    Starts hidden; call popup() to show.
     * @since   0.20
     * @ingroup components
     */
    explicit CalculatorBuiltinPanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the adapter.
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    ~CalculatorBuiltinPanel() override;

    /**
     * @brief   Returns the stable launcher app_id.
     *
     * @return  The id "calculator".
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    QString appId() const override;

    /**
     * @brief   Returns the icon label.
     *
     * @return  The display name "Calculator".
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    QString displayName() const override;

    /**
     * @brief          Sizes/centers the panel over the area and shows it.
     *
     * @param[in] available  Free screen geometry (excludes docked panels).
     *
     * @throws         None
     * @note           A null/empty rect centers on the primary screen.
     * @since          0.20
     * @ingroup        components
     */
    void popup(const QRect& available) override;

    /**
     * @brief   Hides the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    void hidePanel() override;

  private:
    CalculatorPanel* panel_; ///< The wrapped panel (Qt-child owned).
};

} // namespace cf::desktop::desktop_component
