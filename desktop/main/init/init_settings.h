/**
 * @file desktop/main/init/init_settings.h
 * @brief Init session settings and boot widget manager.
 *
 * Manages the initialization session settings and handles the transfer of
 * early boot information to the init session, including ownership management
 * of the boot widget.
 *
 * @author N/A
 * @date N/A
 * @version N/A
 * @since N/A
 * @ingroup init_session
 */
#pragma once
#include "base/singleton/simple_singleton.hpp"
#include <QString>
#include <memory>

class QJsonObject;

class QWidget;
namespace cf::desktop::early_stage {
class EarlyHandle;
}

namespace cf::desktop::init_session {

/**
 * @brief Manages init session settings and boot widget ownership.
 *
 * Handles the transfer of early boot information from the early stage
 * to the init session, and manages the ownership lifecycle of the boot widget.
 *
 * @note Not thread-safe unless externally synchronized.
 * @ingroup init_session
 *
 * @code
 * auto& initInfo = InitInfoHandle::Instance();
 * initInfo.setBootWidget(bootWidget);
 * QWidget* widget = initInfo.unlockBootWidget();
 * @endcode
 */
class InitInfoHandle : public SimpleSingleton<InitInfoHandle> {
  public:
    ~InitInfoHandle();
    /**
     * @brief Transfers early boot information from the early stage.
     *
     * @param[in] early_handle Reference to the early stage handle containing boot info.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup init_session
     */
    void passedEarlyBootInfo(early_stage::EarlyHandle& early_handle);

    /**
     * @brief Sets the boot widget for ownership management.
     *
     * Stores the boot widget pointer for later retrieval via unlockBootWidget.
     * Takes ownership of the widget pointer.
     *
     * @param[in] b Pointer to the boot widget. Ownership: transferred.
     * @throws None
     * @note The stored pointer can be retrieved and cleared using unlockBootWidget.
     * @warning Only one widget can be stored at a time; calling again replaces the previous value.
     * @since N/A
     * @ingroup init_session
     */
    void setBootWidget(QWidget* b);

    /**
     * @brief Releases and returns the stored boot widget.
     *
     * @return Pointer to the boot widget, or nullptr if none was stored.
     *         Ownership: transferred to caller.
     * @throws None
     * @note Clears the internal widget pointer after returning.
     * @warning None
     * @since N/A
     * @ingroup init_session
     */
    QWidget* unlockBootWidget();

    /**
     * @brief Get the Boot Widget object
     *
     * @return QWidget*
     */
    QWidget* getBootWidget() { return boot_widget_; }

    /**
     * @brief Get the root position indicated
     *
     * @return QString
     */
    QString root_position() const;

    /**
     * @brief Get the desktop config folder path (relative to app runtime dir).
     *
     * Reads desktop.config_folder from early settings, defaults to "settings/desktop/".
     *
     * @return QString
     */
    QString config_folder() const;

  private:
    /// @brief Pointer to the boot widget. Ownership: owner; may be nullptr.
    QWidget* boot_widget_{nullptr};
    /// @brief Path to load early settings from. Ownership: owner.
    QString load_early_settings_from;
    /// @brief JSON object containing early configuration. Ownership: owner.
    QJsonObject* early_settings_ = nullptr;
};

} // namespace cf::desktop::init_session
