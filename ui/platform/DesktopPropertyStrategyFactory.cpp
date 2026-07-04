#include "DesktopPropertyStrategyFactory.h"
#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

class PlatformFactory::PlatformImpl {
  public:
    PlatformImpl() : api_(cf::desktop::platform_strategy::native()) {}

    IDesktopPropertyStrategy* create(IDesktopPropertyStrategy::StrategyType t) {
        return api_.creator_func(t);
    }

    void release(IDesktopPropertyStrategy* ptr) {
        if (api_.release_func) {
            api_.release_func(ptr);
        }
    }

    const PlatformFactoryAPI& api() const { return api_; }

  private:
    PlatformFactoryAPI api_;
};

PlatformFactory::PlatformFactory() : impl_(std::make_shared<PlatformImpl>()) {}

PlatformFactory::~PlatformFactory() = default;

std::unique_ptr<IDesktopPropertyStrategy, PlatformFactory::StrategyDeleter>
PlatformFactory::factorize_unique(const IDesktopPropertyStrategy::StrategyType t) {
    auto* raw = impl_->create(t);
    if (!raw) {
        return {nullptr, nullptr};
    }
    return {raw, [impl = impl_](IDesktopPropertyStrategy* ptr) { impl->release(ptr); }};
}

std::shared_ptr<IDesktopPropertyStrategy>
PlatformFactory::factorize_shared(const IDesktopPropertyStrategy::StrategyType t) {
    auto* raw = impl_->create(t);
    if (!raw) {
        return nullptr;
    }

    // memory check safety
    return std::shared_ptr<IDesktopPropertyStrategy>(
        raw, [impl = impl_](IDesktopPropertyStrategy* ptr) { impl->release(ptr); });
}

} // namespace cf::desktop::platform_strategy