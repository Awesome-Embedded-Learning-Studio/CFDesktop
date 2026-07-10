/**
 * @file    desktop/ui/components/wallpaper/WallPaperEngine.h
 * @brief   Timed wallpaper rotation engine.
 *
 * Owns the auto-rotation policy: reads the @c wallpaper config domain,
 * drives a QTimer, and emits a switch request (with the active
 * SwitchingMode) when it is time to advance. The engine deliberately
 * does @e not switch the layer itself — switching synchronously fires
 * the layer's image-changed callback, so the shell layer strategy must
 * arm its transition state first. The strategy therefore consumes the
 * request, performs the switch, and runs the per-frame animation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.19
 * @ingroup wallpaper
 */

#pragma once
#include "WallPaperLayer.h"
#include "cfconfig/cfconfig_watcher.h"
#include "wallpaper/TransitionComposer.h"
#include "wallpaper/WallPaperToken.h"

#include <QEasingCurve>
#include <QObject>
#include <QTimer>
#include <functional>
#include <random>
#include <vector>

namespace cf::desktop::wallpaper {

/**
 * @brief  Auto-rotation selector strategy.
 *
 * @ingroup wallpaper
 */
enum class Selector {
    Sequential, ///< Advance in storage order, wrapping at the end.
    Random      ///< Pick a uniformly random different wallpaper each tick.
};

/**
 * @brief  Picks the next wallpaper ID per the selector strategy.
 *
 * Pure function (no layer or Qt state) for unit testability.
 *
 * - @ref Selector::Sequential locates @p current_id in @p ids and returns
 *   the following ID, wrapping to the first at the end. If @p current_id
 *   is not found, returns the first ID.
 * - @ref Selector::Random returns an ID chosen uniformly from the
 *   candidates excluding @p current_id (falls back to @c ids.front() if
 *   only one distinct candidate exists).
 *
 * @param[in] ids         All wallpaper IDs in storage order; may be empty.
 * @param[in] current_id  The currently shown wallpaper ID.
 * @param[in] selector    Sequential or Random.
 * @param[in] rng         Random engine; consulted only for Random.
 *
 * @return                The next ID, or an empty string if @p ids is empty.
 *
 * @throws                None.
 *
 * @note                  None.
 * @warning               None.
 * @since                 0.19
 * @ingroup               wallpaper
 */
wallpaper_token_id_t selectNextWallpaper(const std::vector<wallpaper_token_id_t>& ids,
                                         const wallpaper_token_id_t& current_id, Selector selector,
                                         std::mt19937& rng);

/**
 * @brief  Drives timed wallpaper rotation per the @c wallpaper config.
 *
 * Reads @c switch_mode / @c switch_selector / @c switch_interval_ms /
 * @c animation_duration_ms / @c switch_easing / @c disable_animation from
 * ConfigStore. Exposes the parsed values so the shell layer strategy can
 * size and ease its per-frame animation without re-reading config.
 *
 * The engine holds a non-owning pointer to the WallPaperLayer only to
 * consult storage size for the @c size()>1 guard; it never mutates the
 * layer.
 *
 * @ingroup wallpaper
 */
class WallPaperEngine : public QObject {
  public:
    /**
     * @brief  Callback fired when a timed switch is due.
     *
     * The strategy arms its transition state and performs the actual
     * layer switch in this callback.
     *
     * @param[in] mode  The SwitchingMode to use for this transition.
     */
    using RequestTransition = std::function<void(SwitchingMode mode)>;

    /**
     * @brief  Constructs the engine bound to a layer and switch callback.
     *
     * @param[in] layer    Non-owning pointer to the wallpaper layer; used
     *                     only for the size guard. May be nullptr.
     * @param[in] request  Invoked on each timer tick with the active mode.
     * @param[in] parent   QObject parent.
     *
     * @throws             None.
     *
     * @note               None.
     * @warning            None.
     * @since              0.19
     * @ingroup            wallpaper
     */
    WallPaperEngine(WallPaperLayer* layer, RequestTransition request, QObject* parent = nullptr);

    ~WallPaperEngine() override;

    WallPaperEngine(const WallPaperEngine&) = delete;
    WallPaperEngine& operator=(const WallPaperEngine&) = delete;

    /**
     * @brief  Starts timed rotation if enabled by config and storage.
     *
     * No-op when @c disable_animation is set, @c switch_mode is Fixed, or
     * the layer holds one or fewer wallpapers.
     *
     * @throws  None.
     *
     * @note    None.
     * @warning None.
     * @since   0.19
     * @ingroup wallpaper
     */
    void start();

    /**
     * @brief  Stops timed rotation.
     *
     * @throws  None.
     *
     * @note    None.
     * @warning None.
     * @since   0.19
     * @ingroup wallpaper
     */
    void stop();

    /**
     * @brief The active switching mode (Fixed/Gradient/Movement).
     *
     * @return The configured switching mode.
     */
    SwitchingMode mode() const noexcept { return mode_; }

    /**
     * @brief The active selector (Sequential/Random).
     *
     * @return The configured selector.
     */
    Selector selector() const noexcept { return selector_; }

    /**
     * @brief Transition animation duration in milliseconds.
     *
     * @return The configured duration.
     */
    int animationDurationMs() const noexcept { return duration_ms_; }

    /**
     * @brief Transition animation easing curve.
     *
     * @return The configured easing curve.
     */
    QEasingCurve easing() const noexcept { return easing_; }

  private:
    /**
     * @brief Invoked on each timer timeout; requests a transition when able.
     */
    void onTimerTick();

    /**
     * @brief Reads the wallpaper config domain and refreshes cached settings.
     */
    void loadConfig();

    /// @brief Reloads config and re-evaluates rotation when the config changes.
    void onConfigChanged();

    WallPaperLayer* layer_;
    RequestTransition request_;
    QTimer timer_;
    SwitchingMode mode_{SwitchingMode::Movement};
    Selector selector_{Selector::Sequential};
    int interval_ms_{20000};
    int duration_ms_{2000};
    QEasingCurve easing_{QEasingCurve::InOutCubic};
    bool disable_animation_{false};

    /// @brief ConfigStore watcher handle for live wallpaper reload.
    cf::config::WatcherHandle config_watch_{0};
};

} // namespace cf::desktop::wallpaper
