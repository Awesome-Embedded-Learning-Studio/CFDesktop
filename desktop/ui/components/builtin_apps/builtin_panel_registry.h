/**
 * @file    builtin_panel_registry.h
 * @brief   Registry of in-process builtin application panels by app_id.
 *
 * BuiltinPanelRegistry maps app_id -> IBuiltinPanel instance so the launcher
 * can resolve a BuiltinPanel launch_kind entry to its panel without a
 * hardcoded if-chain. The registry holds non-owning pointers; panel lifetime
 * is managed by the desktop (panels are Qt children of the desktop surface).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "ibuiltin_panel.h"

#include <QHash>
#include <QString>

#include <vector>

namespace cf::desktop::desktop_component {

/**
 * @brief  Maps app_id to its in-process IBuiltinPanel instance.
 *
 * Singleton accessed via instance(). Registration happens at desktop startup
 * (each builtin panel calls registerPanel this); lookup happens at launch
 * time. Misses are logged as warnings (no silent fallback).
 *
 * @ingroup components
 */
class BuiltinPanelRegistry {
  public:
    /**
     * @brief   Accesses the process-wide registry singleton.
     *
     * @return  Reference to the singleton instance.
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    static BuiltinPanelRegistry& instance();

    /**
     * @brief          Registers a builtin panel under its appId().
     *
     * @param[in] panel  Non-owning pointer; caller retains ownership.
     *
     * @throws         None
     * @note           Re-registering an existing id logs a warning and replaces.
     * @since          0.20
     * @ingroup        components
     */
    void registerPanel(IBuiltinPanel* panel);

    /**
     * @brief          Looks up a panel by app_id.
     *
     * @param[in] id    The app_id to find.
     *
     * @return         The panel, or nullptr if unregistered (logged as warning).
     * @throws         None
     * @since          0.20
     * @ingroup        components
     */
    IBuiltinPanel* find(const QString& id) const;

    /**
     * @brief          Tests whether a panel is registered (silent).
     *
     * @param[in] id    The app_id to test.
     *
     * @return         True if registered.
     * @throws         None
     * @note           Unlike find(), a miss is not logged.
     * @since          0.20
     * @ingroup        components
     */
    bool contains(const QString& id) const;

    /**
     * @brief   Returns all registered panels (for launcher grid population).
     *
     * @return  Vector of registered panel pointers (insertion order).
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    std::vector<IBuiltinPanel*> all() const;

  private:
    BuiltinPanelRegistry() = default;
    QHash<QString, IBuiltinPanel*> panels_;
};

} // namespace cf::desktop::desktop_component
