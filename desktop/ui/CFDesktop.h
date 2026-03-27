/**
 * @file CFDesktop.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Welcomes Center!
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "../export.h"
#include "CFDesktopProxy.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"

#include <QWidget>
namespace cf::desktop {
class PanelManager;
class ShellLayer;
class CFDesktopEntity;

/**
 * @brief Main desktop class representing the CFDesktop application.
 *
 * @details Manages desktop components including panel manager and shell layer.
 *          Provides controlled access through CFDesktopProxy for safety.
 *
 * @throws        None
 * @note          External access must go through GetProxy()
 * @warning       Direct access is restricted; use CFDesktopProxy
 * @since         0.1
 * @ingroup       desktop_ui
 */
class CF_DESKTOP_EXPORT CFDesktop final : public QWidget {
    Q_OBJECT
  public:
    friend class CFDesktopProxy; // Proxy Friendly
    // init with initers
    // like, register panel manager
    // shell layers... and so on!
    // And also hold the Widget itself!
    friend class CFDesktopEntity;

    /**
     * @brief Desktop component identifiers for availability checking.
     *
     * @details Component types:
     *          - Common: Minimal core components
     *          - PanelManager: Panel management system
     *          - ShellLayer: Shell layer interface
     *
     * @since         0.1
     * @ingroup       desktop_ui
     */
    enum class DesktopComponent { Common, PanelManager, ShellLayer };

    /**
     * @brief Checks if a specific desktop component is available.
     *
     * @param[in]     d The desktop component to check (default: Common).
     *
     * @return        True if the component is available, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    bool component_available(const DesktopComponent d = DesktopComponent::Common) const noexcept;

    // Make Weak Reference
    WeakPtr<CFDesktop> GetWeak() const { return weak_ptr_factory_.GetWeakPtr(); }

  private:
    /* Managed by Resources */
    struct InitResources {
        PanelManager* panel_manager_{nullptr};
        ShellLayer* shell_layer_{nullptr};
    };

    PanelManager* panel_manager_{nullptr};
    ShellLayer* shell_layer_{nullptr};

  private:
    CFDesktop(CFDesktopEntity* entity_object); /* pass the entity objects */
    ~CFDesktop();

    // register the desktop sources
    void register_desktop_resources(InitResources& resources);

  private:
    cf::WeakPtrFactory<CFDesktop> weak_ptr_factory_;
};
} // namespace cf::desktop
