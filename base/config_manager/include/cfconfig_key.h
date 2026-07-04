/**
 * @file    desktop/base/config_manager/include/cfconfig_key.h
 * @brief   Defines configuration key types and conversion utilities.
 *
 * Provides lightweight view and owner types for configuration keys, along with
 * helper classes for converting between different key representations.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   <project version or "N/A">
 * @ingroup config_manager
 */

#pragma once
#include <string>
#include <string_view>

namespace cf::config {

/**
 * @brief  Non-owning view of a configuration key.
 *
 * Provides lightweight read-only access to key components without copying.
 * Suitable for temporary key references and parameter passing.
 *
 * @ingroup config_manager
 */
struct KeyView {
    /** @brief Group identifier this key belongs to. */
    std::string_view group;

    /** @brief Specific key identifier within the group. */
    std::string_view key;
};

/**
 * @brief  Owning container for a configuration key.
 *
 * Stores the full key and its description as owning strings. Intended for
 * long-term storage and when ownership transfer is required.
 *
 * @ingroup config_manager
 */
struct Key {
    /** @brief Complete key string representation. */
    std::string full_key;

    /** @brief Human-readable description used in helper generation. */
    std::string full_description;
};

/**
 * @brief  Base helper class for key representation conversions.
 *
 * Defines interface for converting between KeyView and Key types. Derived
 * classes may implement policies to modify or normalize keys during
 * conversion.
 *
 * @note   None.
 * @warning None.
 * @since  N/A.
 * @ingroup config_manager
 */
class KeyHelper {
  public:
    /**
     * @brief  Destroys the KeyHelper instance.
     *
     * @throws None.
     * @note   Default implementation performs no cleanup.
     * @warning None.
     * @since  N/A.
     * @ingroup config_manager
     */
    virtual ~KeyHelper() = default;

    /**
     * @brief  Converts a KeyView to a Key with ownership.
     *
     * Creates an owned Key from a view, potentially applying transformation
     * policies depending on the derived implementation.
     *
     * @param[in]  kv  Source KeyView to convert.
     * @param[out] k   Output Key receiving the converted result.
     * @return         True if conversion succeeded, false otherwise.
     * @throws         None.
     * @note           Derived classes may override to implement custom
     *                 conversion logic.
     * @warning        The caller must ensure `k` remains valid while in use.
     * @since          N/A.
     * @ingroup        config_manager
     */
    virtual bool fromKeyViewToKey(const KeyView& kv, Key& k);

    /**
     * @brief  Creates a KeyView from an owned Key.
     *
     * Returns a non-owning view into the Key's internal storage.
     *
     * @param[in] k  Source Key to create a view from.
     * @return       KeyView pointing to the Key's internal data.
     * @throws       None.
     * @note         The returned KeyView remains valid only while the source
     *               Key remains unchanged and in scope.
     * @warning      The caller must ensure the source Key outlives the
     *               returned view.
     * @since        N/A.
     * @ingroup      config_manager
     */
    virtual KeyView fromKeyToKeyView(const Key& k);
};

} // namespace cf::config
