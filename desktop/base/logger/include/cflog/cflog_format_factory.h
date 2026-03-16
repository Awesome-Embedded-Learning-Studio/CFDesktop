/**
 * @file    desktop/base/logger/include/cflog/cflog_format_factory.h
 * @brief   Factory for creating and managing formatter instances.
 *
 * Provides a thread-safe factory for creating formatter instances
 * with support for registration, caching, and singleton patterns.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include "cflog_format.h"
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <unordered_map>

namespace cf::log {

/**
 * @brief  Factory for creating and managing formatter instances.
 *
 * Provides thread-safe registration, creation, and caching of
 * formatter instances.
 *
 * @ingroup cflog
 *
 * @note   Thread-safe for all operations.
 *
 * @code
 * FormatterFactory factory;
 * factory.register_formatter("console", []() { return std::make_shared<ConsoleFormatter>(); });
 * auto formatter = factory.get_or_create("console");
 * @endcode
 */
class FormatterFactory {
  public:
    using IFormatterPtr = std::shared_ptr<IFormatter>;
    using IFormatterTag = std::string;
    using IFormatterTagView = std::string_view;
    using MakeFormatter = std::function<IFormatterPtr()>;

    /**
     * @brief  Default constructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    FormatterFactory() = default;

    /**
     * @brief  Destructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    ~FormatterFactory() = default;

    // Disable copy and move (std::mutex is not movable)
    FormatterFactory(const FormatterFactory&) = delete;
    FormatterFactory& operator=(const FormatterFactory&) = delete;
    FormatterFactory(FormatterFactory&&) = delete;
    FormatterFactory& operator=(FormatterFactory&&) = delete;

    /**
     * @brief  Registers a formatter creator function.
     *
     * @param[in] tag     The tag identifying the formatter type.
     * @param[in] creator Function that creates a new formatter instance.
     * @throws            None
     * @note              None
     * @warning           None
     * @since             0.1
     * @ingroup           cflog
     */
    void register_formatter(IFormatterTagView tag, MakeFormatter creator) {
        std::lock_guard<std::mutex> lock(factory_mutex_);
        format_factory_[std::string(tag)] = std::move(creator);
    }

    /**
     * @brief  Registers a formatter instance as a singleton creator.
     *
     * @param[in] tag      The tag identifying the formatter type.
     * @param[in] instance Shared pointer to the formatter instance.
     * @throws             None
     * @note               The same instance is returned each time.
     * @warning            None
     * @since              0.1
     * @ingroup            cflog
     */
    void register_formatter(IFormatterTagView tag, IFormatterPtr instance) {
        register_formatter(tag, [instance = std::move(instance)]() { return instance; });
    }

    /**
     * @brief  Unregisters a formatter creator.
     *
     * @param[in] tag The tag identifying the formatter type.
     * @return        true if unregistered, false if not found.
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       cflog
     */
    bool unregister_formatter(IFormatterTagView tag) {
        std::lock_guard<std::mutex> lock(factory_mutex_);
        return format_factory_.erase(std::string(tag)) > 0;
    }

    /**
     * @brief  Creates a new formatter instance (non-cached).
     *
     * @param[in] formatter_tag The tag identifying the formatter type.
     * @return                  Shared pointer to a new formatter instance,
     *                          or empty if not found.
     * @throws                  None
     * @note                    Each call creates a new instance.
     * @warning                 None
     * @since                   0.1
     * @ingroup                 cflog
     */
    IFormatterPtr create(IFormatterTagView formatter_tag) const {
        std::lock_guard<std::mutex> lock(factory_mutex_);
        auto it = format_factory_.find(std::string(formatter_tag));
        if (it == format_factory_.end()) {
            return {};
        }
        return it->second ? it->second() : nullptr;
    }

    /**
     * @brief  Gets or creates a formatter instance with caching.
     *
     * Returns a cached instance if available, otherwise creates
     * and caches a new instance.
     *
     * @param[in] formatter_tag The tag identifying the formatter type.
     * @return                  Shared pointer to the formatter instance.
     * @throws                  None
     * @note                    The instance is cached for future calls.
     * @warning                 None
     * @since                   0.1
     * @ingroup                 cflog
     */
    IFormatterPtr get_or_create(IFormatterTagView formatter_tag) {
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            auto cache_it = formatter_cache_.find(std::string(formatter_tag));
            if (cache_it != formatter_cache_.end()) {
                return cache_it->second;
            }
        }

        // Create new instance
        auto formatter = create(formatter_tag);
        if (!formatter) {
            return {};
        }

        // Cache and return
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            formatter_cache_[std::string(formatter_tag)] = formatter;
        }
        return formatter;
    }

    /**
     * @brief  Clears the formatter cache.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    void clear_cache() {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        formatter_cache_.clear();
    }

  private:
    // Map of formatter creators (tag -> creator function)
    std::unordered_map<std::string, MakeFormatter> format_factory_;

    // Cache for created formatters (tag -> cached formatter instance)
    std::unordered_map<std::string, IFormatterPtr> formatter_cache_;

    // Mutex for thread-safe factory access
    mutable std::mutex factory_mutex_;

    // Mutex for thread-safe cache access
    mutable std::mutex cache_mutex_;
};

} // namespace cf::log
