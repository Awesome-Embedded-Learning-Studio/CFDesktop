/**
 * @file    CFDesktopProxy.h
 * @brief   CFDesktop Proxy header for controlled desktop access.
 *
 * @details Provides the common interfaces for querying desktops.
 *          Proxy provides a common interface for visiting the components.
 *
 * @author  CFDesktop Team
 * @date    2026-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui
 */
#pragma once
#include "../export.h"
#include "CFDesktop.h"
#include "base/weak_ptr/weak_ptr.h"
#include <memory>

namespace cf::desktop {
class CFDesktop;

/**
 * @brief  Provides the common interfaces for querying desktops.
 *
 * @details Proxy provides a common interface for visiting the components.
 *          Acts as a proxy to the underlying desktop instance using a weak
 *          reference to avoid circular dependencies.
 *
 * @throws  None
 * @note    None
 * @warning None
 * @since   0.1
 * @ingroup desktop_ui
 *
 * @code
 * auto proxy = desktop->createProxy();
 * proxy->someMethod();
 * @endcode
 */
class CF_DESKTOP_EXPORT CFDesktopProxy {
  public:
    /**
     * @brief  Destroys the proxy instance.
     *
     * @details Default destructor. Does not affect the referenced desktop.
     *
     * @throws  None
     * @note    None
     * @warning None
     * @since   0.1
     * @ingroup desktop_ui
     */
    virtual ~CFDesktopProxy() = default;

  protected:
    /**
     * @brief      Constructs a proxy with a desktop weak reference.
     *
     * @details    Initializes the proxy with a weak reference to the desktop.
     *            The proxy does not own the desktop and relies on the
     *            weak reference for safety.
     *
     * @param[in]  desktop Weak reference to the desktop instance. Must be
     *                     valid at construction time.
     *
     * @throws     None
     * @note       The weak reference is stored for later access; caller must
     *             ensure the desktop outlives proxy operations.
     * @warning    None
     * @since      0.1
     * @ingroup    desktop_ui
     */
    CFDesktopProxy(WeakPtr<CFDesktop> desktop);

    /// @brief Weak reference to the desktop instance. Ownership: observer.
    WeakPtr<CFDesktop> desktop_;
};

} // namespace cf::desktop
