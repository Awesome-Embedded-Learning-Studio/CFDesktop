/**
 * @file once_init.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Call once inits
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <mutex>
#include <utility>

namespace cf {

template <typename Resources> struct CallOnceInit {
  public:
    /**
     * @brief   Construct a new Call Once Init object
     *          One can call with build-time, or
     *          manually get sources
     *
     * @tparam Args
     * @param args
     */
    template <typename... Args> explicit CallOnceInit(Args&&... args)
        : resource(std::forward<Args>(args)...), initialized(true) {}

    CallOnceInit() = default;
    Resources& get_resources() {
        if (!initialized) {
            std::call_once(init_flag, [this]() {
                init_resources();
                initialized = true;
            });
        }
        return resource;
    }

    void force_reinit() {
        std::lock_guard<std::mutex> lock(force_mtx);
        force_do_reinit();
        initialized = true;
    }

    template <typename... Args> void force_reinit(Args&&... args) {
        std::lock_guard<std::mutex> lock(force_mtx);
        resource = Resources(std::forward<Args>(args)...);
        initialized = true;
    }

  protected:
    Resources resource;
    virtual bool init_resources() = 0;
    virtual bool force_do_reinit() = 0;

  private:
    std::once_flag init_flag;
    std::mutex force_mtx;
    bool initialized{false};
};

} // namespace cf