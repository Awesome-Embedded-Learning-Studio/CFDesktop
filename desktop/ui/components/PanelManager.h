/**
 * @file    PanelManager.h
 * @brief   Manages edge-occupying panels and their layout.
 *
 * PanelManager registers, unregisters, and lays out panels around
 * the desktop shell. It computes the available geometry after
 * accounting for all registered panels.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "base/weak_ptr/weak_ptr.h"
#include <QList>
#include <QObject>
#include <QPaintDevice>
namespace cf::desktop {
class IPanel;

/**
 * @brief  Manages panels and computes available shell geometry.
 *
 * @ingroup components
 */
class PanelManager : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief  Registration result for panel operations.
     *
     * @ingroup components
     */
    enum class RegisterFeedback {
        OK,            ///< Registration succeeded.
        InvalidPanel,  ///< The panel pointer is null or invalid.
        DuplicatePanel ///< The panel is already registered.
    };

    /**
     * @brief  Unregistration result for panel operations.
     *
     * @ingroup components
     */
    enum class UnRegisterFeedback {
        OK,           ///< Unregistration succeeded.
        InvalidPanel, ///< The panel pointer is null or invalid.
        UnknownPanel  ///< The panel was not found in the registry.
    };

    explicit PanelManager(QWidget* host, QObject* parent = nullptr);
    virtual ~PanelManager();

    /**
     * @brief  Registers a panel with the layout engine.
     *
     * @param[in]  panel  Weak reference to the panel to register.
     *
     * @return Registration feedback indicating success or failure.
     */
    virtual RegisterFeedback registerPanel(WeakPtr<IPanel> panel);

    /**
     * @brief  Unregisters a panel from the layout engine.
     *
     * @param[in]  panel  Weak reference to the panel to unregister.
     *
     * @return Unregistration feedback indicating success or failure.
     */
    virtual UnRegisterFeedback unregisterPanel(WeakPtr<IPanel> panel);

    /**
     * @brief  Returns the available geometry for the shell layer.
     *
     * The available geometry is the screen area remaining after
     * all registered panels have been laid out.
     *
     * @return The available rectangle in device pixels.
     */
    virtual const QRect availableGeometry() const;

  signals:

    /**
     * @brief  Emitted when the available geometry changes.
     *
     * @param[in]  rect  The new available geometry.
     */
    void availableGeometryChanged(const QRect& rect);

  protected:
    /**
     * @brief  Recomputes the layout of all registered panels.
     */
    virtual void relayout();

  protected:
    /// Host widget for panel positioning. Ownership: external.
    QWidget* host_{nullptr};
    /// Registered panels (weak references). Ownership: external.
    std::vector<WeakPtr<IPanel>> panels;

  private:
    /// Cached available geometry after layout computation.
    QRect available_geometry_;
};
} // namespace cf::desktop
