#include "cflog/sinks/console_sink.h"
#include "cflog/formatter/default_formatter.h"
#include <iostream>
#include <memory>

namespace cf::log {
bool ConsoleSink::write(const LogRecord& record) {
    if (!formatter_) {
        formatter_ = std::make_shared<DefaultFormatter>();
    }
    const auto str = formatter_->format_me(record);
    std::cout << str;

    return true;
}

bool ConsoleSink::flush() {
    std::cout << std::flush;
    return true;
}
} // namespace cf::log
