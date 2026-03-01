/**
 * @file    cfmaterial_animation_factory.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Animation Factory Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the CFMaterialAnimationFactory class for creating and managing
 * Material Design 3 animations with token-based lookup and strategy
 * pattern customization.
 */

#include "cfmaterial_animation_factory.h"
#include "animation_factory_manager.h"
#include "cfmaterial_fade_animation.h"
#include "cfmaterial_scale_animation.h"
#include "cfmaterial_slide_animation.h"
#include "token/animation_token_mapping.h"
#include <cstring>

namespace cf::ui::components::material {

using namespace cf::ui::core;
using namespace token_literals;

// =============================================================================
// Constructor / Destructor
// =============================================================================

CFMaterialAnimationFactory::CFMaterialAnimationFactory(const ICFTheme& theme,
                                                       std::unique_ptr<AnimationStrategy> strategy,
                                                       QObject* parent)
    : ICFAnimationManagerFactory(parent), theme_(theme), strategy_(std::move(strategy)),
      globalEnabled_(true) {
    // Use default strategy if none provided
    if (!strategy_) {
        strategy_ = std::make_unique<DefaultAnimationStrategy>();
    }
}

CFMaterialAnimationFactory::~CFMaterialAnimationFactory() {
    // All owned animations will be automatically destroyed
    animations_.clear();
}

// =============================================================================
// Public Methods
// =============================================================================

cf::WeakPtr<ICFAbstractAnimation>
CFMaterialAnimationFactory::getAnimation(const char* animationToken) {

    // Check global enabled state
    if (!globalEnabled_) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // Check if animation already exists
    auto it = animations_.find(animationToken);
    if (it != animations_.end()) {
        // Return existing WeakPtr from the animation
        return it->second->GetWeakPtr();
    }

    // Resolve token to descriptor
    AnimationDescriptor descriptor = resolveToken(animationToken);

    // Check if token was found
    if (descriptor.animationType == nullptr) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // Apply strategy
    descriptor = applyStrategy(descriptor, nullptr);

    // Check if animation should be enabled
    if (!shouldEnableAnimation(nullptr)) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // Create animation based on type
    std::unique_ptr<ICFAbstractAnimation> animation;
    const char* type = descriptor.animationType;

    if (std::strcmp(type, "fade") == 0) {
        animation = createFadeAnimation(descriptor, nullptr);
    } else if (std::strcmp(type, "slide") == 0) {
        animation = createSlideAnimation(descriptor, nullptr);
    } else if (std::strcmp(type, "scale") == 0) {
        animation = createScaleAnimation(descriptor, nullptr);
    }

    // Store and return WeakPtr
    if (animation) {
        const char* tokenKey = descriptor.motionToken;
        ICFAbstractAnimation* rawPtr = animation.get();
        animations_[tokenKey] = std::move(animation);
        emit animationCreated(QString::fromUtf8(tokenKey));
        return rawPtr->GetWeakPtr();
    }

    return cf::WeakPtr<ICFAbstractAnimation>();
}

cf::WeakPtr<ICFAbstractAnimation>
CFMaterialAnimationFactory::createAnimation(const AnimationDescriptor& descriptor,
                                            QWidget* targetWidget) {

    // Check global enabled state
    if (!globalEnabled_) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // Apply strategy
    AnimationDescriptor adjustedDescriptor = applyStrategy(descriptor, targetWidget);

    // Check if animation should be enabled
    if (!shouldEnableAnimation(targetWidget)) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // Create animation based on type
    std::unique_ptr<ICFAbstractAnimation> animation;
    const char* type = adjustedDescriptor.animationType;

    if (std::strcmp(type, "fade") == 0) {
        animation = createFadeAnimation(adjustedDescriptor, targetWidget);
    } else if (std::strcmp(type, "slide") == 0) {
        animation = createSlideAnimation(adjustedDescriptor, targetWidget);
    } else if (std::strcmp(type, "scale") == 0) {
        animation = createScaleAnimation(adjustedDescriptor, targetWidget);
    }

    // Generate a unique key for this animation
    std::string key = adjustedDescriptor.motionToken;
    key += "_";
    key += std::to_string(reinterpret_cast<uintptr_t>(targetWidget));

    // Store and return WeakPtr
    if (animation) {
        ICFAbstractAnimation* rawPtr = animation.get();
        animations_[key] = std::move(animation);
        emit animationCreated(QString::fromUtf8(key.c_str()));
        return rawPtr->GetWeakPtr();
    }

    return cf::WeakPtr<ICFAbstractAnimation>();
}

void CFMaterialAnimationFactory::setStrategy(std::unique_ptr<AnimationStrategy> strategy) {

    strategy_ = std::move(strategy);
    if (!strategy_) {
        strategy_ = std::make_unique<DefaultAnimationStrategy>();
    }
}

void CFMaterialAnimationFactory::setEnabledAll(bool enabled) {
    if (globalEnabled_ != enabled) {
        globalEnabled_ = enabled;
        emit animationEnabledChanged(enabled);
    }
}

ICFAnimationManagerFactory::RegisteredResult
CFMaterialAnimationFactory::registerOneAnimation(const QString& name, const QString& type) {
    // Material Design factory uses predefined token mappings
    // Custom registrations are stored in a separate map
    (void)name; // TODO: Implement custom animation registration
    (void)type; // TODO: Implement custom animation registration
    return ICFAnimationManagerFactory::RegisteredResult::UNSUPPORT_TYPE;
}

ICFAnimationManagerFactory::RegisteredResult
CFMaterialAnimationFactory::registerAnimationCreator(const QString& name,
                                                     AnimationCreator creator) {
    // Material Design factory uses predefined token mappings
    // Custom registrations are stored in a separate map
    (void)name;    // TODO: Implement custom animation registration
    (void)creator; // TODO: Implement custom animation registration
    return ICFAnimationManagerFactory::RegisteredResult::UNSUPPORT_TYPE;
}

// =============================================================================
// Private Methods
// =============================================================================

AnimationDescriptor CFMaterialAnimationFactory::resolveToken(const char* token) {
    const auto* mapping = findTokenMapping(token);
    if (mapping) {
        return AnimationDescriptor(mapping->animationType, mapping->motionToken, mapping->property,
                                   mapping->defaultFrom, mapping->defaultTo,
                                   0 // delayMs
        );
    }

    // Return empty descriptor if token not found
    return AnimationDescriptor();
}

std::unique_ptr<ICFAbstractAnimation>
CFMaterialAnimationFactory::createFadeAnimation(const AnimationDescriptor& desc, QWidget* widget) {

    // Get motion spec from theme
    auto& motionSpec = theme_.motion_spec();

    // Query duration and easing from motion token
    int duration = motionSpec.queryDuration(desc.motionToken);
    int easing = motionSpec.queryEasing(desc.motionToken);

    // Create fade animation with raw pointer (lifetime guaranteed by theme_)
    auto anim = std::make_unique<CFMaterialFadeAnimation>(&motionSpec, nullptr);
    anim->setRange(desc.fromValue, desc.toValue);
    if (widget) {
        anim->setTargetWidget(widget);
    }
    return anim;
}

std::unique_ptr<ICFAbstractAnimation>
CFMaterialAnimationFactory::createSlideAnimation(const AnimationDescriptor& desc, QWidget* widget) {

    // Get motion spec from theme
    auto& motionSpec = theme_.motion_spec();

    // Query duration and easing from motion token
    int duration = motionSpec.queryDuration(desc.motionToken);
    int easing = motionSpec.queryEasing(desc.motionToken);

    // Determine slide direction from property
    SlideDirection direction = SlideDirection::Up;
    if (std::strcmp(desc.property, "positionX") == 0) {
        direction = SlideDirection::Right;
    }

    // Create slide animation with raw pointer (lifetime guaranteed by theme_)
    auto anim = std::make_unique<CFMaterialSlideAnimation>(&motionSpec, direction, nullptr);
    anim->setRange(desc.fromValue, desc.toValue);
    if (widget) {
        anim->setTargetWidget(widget);
    }
    return anim;
}

std::unique_ptr<ICFAbstractAnimation>
CFMaterialAnimationFactory::createScaleAnimation(const AnimationDescriptor& desc, QWidget* widget) {

    // Get motion spec from theme
    auto& motionSpec = theme_.motion_spec();

    // Query duration and easing from motion token
    int duration = motionSpec.queryDuration(desc.motionToken);
    int easing = motionSpec.queryEasing(desc.motionToken);

    // Create scale animation with raw pointer (lifetime guaranteed by theme_)
    auto anim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec, nullptr);
    anim->setRange(desc.fromValue, desc.toValue);
    if (widget) {
        anim->setTargetWidget(widget);
    }
    return anim;
}

AnimationDescriptor CFMaterialAnimationFactory::applyStrategy(const AnimationDescriptor& descriptor,
                                                              QWidget* widget) {

    if (strategy_) {
        return strategy_->adjust(descriptor, widget);
    }
    return descriptor;
}

bool CFMaterialAnimationFactory::shouldEnableAnimation(QWidget* widget) const {
    if (strategy_) {
        return strategy_->shouldEnable(widget);
    }
    return globalEnabled_;
}

} // namespace cf::ui::components::material
