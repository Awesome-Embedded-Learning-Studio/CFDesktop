/**
 * @file    service_locator.h
 * @brief   In-process service registry for typed lookup by name.
 *
 * A small type-erased map from name to shared_ptr<T>, so subsystems can
 * publish and discover services without hard references to each other.
 * Process-wide singleton; not for cross-process use (that is what the
 * IPC message layer is for).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

#include <QHash>
#include <QString>
#include <memory>

namespace cf::ipc {

/**
 * @brief  Process-wide typed service registry.
 *
 * @note   Thread-unsafe by default; callers synchronize if needed.
 * @warning None
 * @since  0.19.0
 * @ingroup ipc
 *
 * @code
 * cf::ipc::ServiceLocator::instance().registerService<ILogger>(
 *     "logger", std::make_shared<ConsoleLogger>());
 * auto log = cf::ipc::ServiceLocator::instance().resolve<ILogger>("logger");
 * @endcode
 */
class ServiceLocator {
  public:
    /**
     * @brief  Returns the process-wide singleton instance.
     *
     * @return     Reference to the singleton service locator.
     * @throws     None
     * @note       Created on first access.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static ServiceLocator& instance() {
        static ServiceLocator locator;
        return locator;
    }

    /**
     * @brief  Publishes a service instance under a name.
     *
     * @param[in]  name     Lookup key.
     * @param[in]  service  The service instance to store.
     * @throws     None
     * @note       Re-registering a name replaces the prior instance.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    template <typename T> void registerService(const QString& name, std::shared_ptr<T> service) {
        services_[name] = std::static_pointer_cast<void>(service);
    }

    /**
     * @brief  Resolves a previously published service.
     *
     * @param[in]  name  Lookup key.
     * @return     The stored service, or nullptr if none is registered.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    template <typename T> std::shared_ptr<T> resolve(const QString& name) const {
        const auto it = services_.constFind(name);
        if (it == services_.cend()) {
            return nullptr;
        }
        return std::static_pointer_cast<T>(it.value());
    }

    /**
     * @brief  Drops all registered services.
     *
     * @throws     None
     * @note       Intended for tests; clears the shared_ptr entries.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    void clear() { services_.clear(); }

  private:
    ServiceLocator() = default;

    /// @brief Type-erased service storage.
    QHash<QString, std::shared_ptr<void>> services_;
};

} // namespace cf::ipc
