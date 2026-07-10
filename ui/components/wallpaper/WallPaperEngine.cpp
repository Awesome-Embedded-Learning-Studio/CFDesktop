/**
 * @file    desktop/ui/components/wallpaper/WallPaperEngine.cpp
 * @brief   Implementation of the timed wallpaper rotation engine.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.19
 * @ingroup wallpaper
 */

#include "WallPaperEngine.h"

#include "cfconfig.hpp"
#include "cflog.h"

#include <algorithm>

namespace cf::desktop::wallpaper {

namespace {
/// Tag for engine log lines.
constexpr const char* kLogTag = "WallPaperEngine";

/**
 * @brief  Parses a switch_mode string, warning on unknown values.
 *
 * @param[in] value  Raw config string.
 * @return           Parsed mode; Movement on unknown input.
 */
SwitchingMode parse_switch_mode(const std::string& value) {
    if (value == "fixed") {
        return SwitchingMode::Fixed;
    }
    if (value == "gradient") {
        return SwitchingMode::Gradient;
    }
    if (value == "movement") {
        return SwitchingMode::Movement;
    }
    log::warningftag(kLogTag, "Unknown switch_mode '{}', falling back to movement", value);
    return SwitchingMode::Movement;
}

/**
 * @brief  Parses a switch_selector string, warning on unknown values.
 *
 * @param[in] value  Raw config string.
 * @return           Parsed selector; Sequential on unknown input.
 */
Selector parse_selector(const std::string& value) {
    if (value == "random") {
        return Selector::Random;
    }
    if (value == "sequential") {
        return Selector::Sequential;
    }
    log::warningftag(kLogTag, "Unknown switch_selector '{}', falling back to sequential", value);
    return Selector::Sequential;
}

/**
 * @brief  Parses a switch_easing string, warning on unknown values.
 *
 * @param[in] value  Raw config string.
 * @return           Parsed curve; InOutCubic on unknown input.
 */
QEasingCurve parse_easing(const std::string& value) {
    if (value == "inoutcubic") {
        return QEasingCurve::InOutCubic;
    }
    if (value == "outcubic") {
        return QEasingCurve::OutCubic;
    }
    if (value == "linear") {
        return QEasingCurve::Linear;
    }
    log::warningftag(kLogTag, "Unknown switch_easing '{}', falling back to InOutCubic", value);
    return QEasingCurve::InOutCubic;
}
} // namespace

wallpaper_token_id_t selectNextWallpaper(const std::vector<wallpaper_token_id_t>& ids,
                                         const wallpaper_token_id_t& current_id, Selector selector,
                                         std::mt19937& rng) {
    if (ids.empty()) {
        return {};
    }
    if (selector == Selector::Random) {
        std::vector<size_t> candidates;
        candidates.reserve(ids.size());
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i] != current_id) {
                candidates.push_back(i);
            }
        }
        if (candidates.empty()) {
            return ids.front();
        }
        std::uniform_int_distribution<size_t> dis(0, candidates.size() - 1);
        return ids[candidates[dis(rng)]];
    }
    const auto it = std::find(ids.begin(), ids.end(), current_id);
    if (it == ids.end()) {
        return ids.front();
    }
    auto next = it + 1;
    if (next == ids.end()) {
        next = ids.begin(); // wrap at the end
    }
    return *next;
}

WallPaperEngine::WallPaperEngine(WallPaperLayer* layer, RequestTransition request, QObject* parent)
    : QObject(parent), layer_(layer), request_(std::move(request)), timer_(this) {
    loadConfig();
    connect(&timer_, &QTimer::timeout, this, &WallPaperEngine::onTimerTick);
    // Live reload: re-evaluate rotation whenever a wallpaper config key changes
    // (the Settings window writes this domain).
    config_watch_ = cf::config::ConfigStore::instance()
                        .domain("wallpaper")
                        .watch(
                            "wallpaper.*",
                            [this](const cf::config::Key&, const std::any*, const std::any*,
                                   cf::config::Layer) { onConfigChanged(); },
                            cf::config::NotifyPolicy::Immediate);
    log::traceftag(kLogTag, "Constructed: mode={} selector={} interval={}ms duration={}ms",
                   static_cast<int>(mode_), static_cast<int>(selector_), interval_ms_,
                   duration_ms_);
}

WallPaperEngine::~WallPaperEngine() {
    stop();
    if (config_watch_ != 0) {
        cf::config::ConfigStore::instance().domain("wallpaper").unwatch(config_watch_);
    }
}

void WallPaperEngine::loadConfig() {
    auto wp = cf::config::ConfigStore::instance().domain("wallpaper");
    mode_ = parse_switch_mode(wp.query<std::string>(
        cf::config::KeyView{.group = "wallpaper", .key = "switch_mode"}, "movement"));
    selector_ = parse_selector(wp.query<std::string>(
        cf::config::KeyView{.group = "wallpaper", .key = "switch_selector"}, "sequential"));
    interval_ms_ = wp.query<int>(
        cf::config::KeyView{.group = "wallpaper", .key = "switch_interval_ms"}, 20000);
    duration_ms_ = wp.query<int>(
        cf::config::KeyView{.group = "wallpaper", .key = "animation_duration_ms"}, 2000);
    easing_ = parse_easing(wp.query<std::string>(
        cf::config::KeyView{.group = "wallpaper", .key = "switch_easing"}, "inoutcubic"));
    disable_animation_ = wp.query<bool>(
        cf::config::KeyView{.group = "wallpaper", .key = "disable_animation"}, false);
}

void WallPaperEngine::onConfigChanged() {
    // start() reloads config and re-evaluates (disable/mode/size guards).
    stop();
    start();
}

void WallPaperEngine::start() {
    loadConfig(); // pick up the latest values
    if (disable_animation_) {
        log::infoftag(kLogTag, "Auto-switch disabled by config (disable_animation)");
        return;
    }
    if (mode_ == SwitchingMode::Fixed) {
        log::infoftag(kLogTag, "Auto-switch disabled (mode is Fixed)");
        return;
    }
    if (!layer_ || layer_->tokenStorage().size() <= 1) {
        log::traceftag(kLogTag, "Not starting timer: need more than one wallpaper");
        return;
    }
    timer_.setInterval(interval_ms_);
    timer_.start();
    log::infoftag(kLogTag, "Started auto-switch (interval={}ms)", interval_ms_);
}

void WallPaperEngine::stop() {
    timer_.stop();
}

void WallPaperEngine::onTimerTick() {
    // Defensive: start() guards these, but a hot re-entry should still no-op.
    if (mode_ == SwitchingMode::Fixed || disable_animation_) {
        return;
    }
    if (!layer_ || layer_->tokenStorage().size() <= 1) {
        return;
    }
    if (request_) {
        request_(mode_);
    }
}

} // namespace cf::desktop::wallpaper
