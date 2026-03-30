/**
 * @file desktop/main/early_session/early_handle/early_handle.h
 * @brief Early session handle for managing early desktop initialization settings.
 *
 * Provides singleton access to early configuration settings and manages the
 * lifecycle of early settings during desktop initialization.
 *
 * @author CFDesktop Team
 * @date 2026-03-16
 * @version 0.13.1
 * @since 0.13.0
 * @ingroup early_session
 */
#pragma once
#include "base/singleton/simple_singleton.hpp"
#include "settings/early_settings.h"
#include <memory>
class QWidget;

namespace cf::desktop::early_stage {
/**
 * @brief Singleton handle for managing early desktop settings.
 *
 * Provides centralized access to early configuration settings throughout the
 * desktop initialization process.
 *
 * @note Not thread-safe unless externally synchronized.
 * @ingroup early_session
 *
 * @code
 * auto& handle = EarlyHandle::instance();
 * handle.setEarlySettingsHandle(std::make_unique<EarlySettings>(path));
 * const auto& settings = handle.early_settings();
 * @endcode
 */
class EarlyHandle : public SimpleSingleton<EarlyHandle> {
  public:
    EarlyHandle() = default;
    /**
     * @brief Registers the early settings handle with ownership transfer.
     *
     * Takes ownership of the provided settings object and stores it for later
     * access through the singleton interface.
     *
     * @param[in] settings Unique pointer to early settings. Ownership is transferred.
     * @throws None
     * @note None
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    void setEarlySettingsHandle(std::unique_ptr<early_settings::EarlySettings> settings);

    /**
     * @brief Releases and returns ownership of early settings.
     *
     * Transfers ownership of the stored settings object back to the caller.
     * The internal settings pointer is set to nullptr after this call.
     *
     * @return Unique pointer to early settings. Caller takes ownership.
     * @throws None
     * @note After calling this method, early_settings() is invalid until a new
     *       settings object is registered.
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    std::unique_ptr<early_settings::EarlySettings> unlockEarlySettings();

    /**
     * @brief Gets a reference to the early settings.
     *
     * Provides read-only access to the stored early settings object.
     *
     * @return Reference to early settings. Valid only if settings has been set.
     * @throws None
     * @note Undefined behavior if settings has not been set or has been unlocked.
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    const early_settings::EarlySettings& early_settings() const;

  private:
    /// @brief Unique pointer to early settings. Ownership: owner; may be nullptr.
    std::unique_ptr<early_settings::EarlySettings> settings;
};
} // namespace cf::desktop::early_stage
