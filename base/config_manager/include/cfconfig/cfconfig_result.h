/**
 * @file cfconfig_result.h
 * @brief Result type enumerations for ConfigStore operations.
 *
 * Defines enums for the result of various ConfigStore operations
 * such as key registration, unregistration, and notification.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

namespace cf::config {

/**
 * @brief Key registration result.
 */
enum class RegisterResult {
    KeyAlreadyIn = 0,        ///< The key is already registered
    KeyRegisteredSuccess = 1 ///< The key was registered successfully
};

/**
 * @brief Key unregistration result.
 */
enum class UnRegisterResult {
    KeyUnexisted = 0,          ///< The key does not exist
    KeyUnRegisteredSuccess = 1 ///< The key was unregistered successfully
};

/**
 * @brief Notification result.
 */
enum class NotifyResult {
    NotifyFailed = 0,       ///< Notification failed (internal error)
    NothingWorthNotify = 1, ///< No pending changes to notify
    NotifySuccess = 2       ///< Notification completed successfully
};

} // namespace cf::config
