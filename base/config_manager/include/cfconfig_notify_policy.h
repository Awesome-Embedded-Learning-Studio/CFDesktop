#pragma once

/**
 * @file    cfconfig_notify_policy.h
 * @brief   Defines the notification policy for configuration changes.
 *
 * This header specifies when and how configuration change notifications are
 * dispatched to registered watchers.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   <project version or "N/A">
 * @ingroup config
 */

namespace cf::config {

/**
 * @brief  Policy for when configuration change notifications are dispatched.
 *
 * Controls whether notification callbacks are invoked automatically when a
 * configuration value changes or must be triggered manually.
 *
 * @ingroup config
 */
enum class NotifyPolicy {
    /// Notification callbacks must be invoked manually by the caller.
    Manual,

    /// Notification callbacks are invoked immediately upon each configuration
    /// change.
    Immediate
};

} // namespace cf::config