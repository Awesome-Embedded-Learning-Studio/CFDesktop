#include "cflog.h"
#include "async_queue/async_queue.h"
#include "cflog/cflog.hpp"
#include "impl/cflog_impl.h"

namespace cf::log {

// ============================================================================
// Logger Implementation
// ============================================================================

Logger::Logger() : logger_impl(std::make_shared<LoggerImpl>()) {
    // No default sink - user must configure sinks explicitly
}

Logger::~Logger() = default;

bool Logger::log(level log_level, std::string_view msg, std::string_view tag,
                 std::source_location loc) {
    if (log_level < minimal_level.load(std::memory_order_relaxed)) {
        return false;
    }
    return logger_impl->log(log_level, msg, tag, loc);
}

void Logger::flush() {
    logger_impl->flush();
}

void Logger::flush_sync() {
    logger_impl->flush_sync();
}

void Logger::add_sink(std::shared_ptr<ISink> sink) {
    logger_impl->add_sink(std::move(sink));
}

void Logger::remove_sink(ISink* sink) {
    logger_impl->remove_sink(sink);
}

void Logger::clear_sinks() {
    logger_impl->clear_sinks();
}

void trace(std::string_view msg, std::string_view tag, std::source_location loc) {
    Logger::instance().log(level::TRACE, msg, tag, loc);
}

void debug(std::string_view msg, std::string_view tag, std::source_location loc) {
    Logger::instance().log(level::DEBUG, msg, tag, loc);
}

void info(std::string_view msg, std::string_view tag, std::source_location loc) {
    Logger::instance().log(level::INFO, msg, tag, loc);
}

void warning(std::string_view msg, std::string_view tag, std::source_location loc) {
    Logger::instance().log(level::WARNING, msg, tag, loc);
}

void error(std::string_view msg, std::string_view tag, std::source_location loc) {
    Logger::instance().log(level::ERROR, msg, tag, loc);
}

void set_level(level lvl) {
    Logger::instance().setMininumLevel(lvl);
}

void flush() {
    Logger::instance().flush();
}

void add_sink(std::shared_ptr<ISink> sink) {
    Logger::instance().add_sink(std::move(sink));
}

void remove_sink(ISink* sink) {
    Logger::instance().remove_sink(sink);
}

void clear_sinks() {
    Logger::instance().clear_sinks();
}

} // namespace cf::log
