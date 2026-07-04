#include "impl/cflog_impl.h"
#include "async_queue/async_queue.h"
#include "cflog/cflog_record.h"
#include <chrono>

namespace cf::log {

LoggerImpl::LoggerImpl() : post_queue(std::make_unique<AsyncPostQueue>()) {
    post_queue->start();
}

LoggerImpl::~LoggerImpl() {
    post_queue->stop();
}

bool LoggerImpl::log(level log_level, std::string_view msg, std::string_view tag,
                     std::source_location loc) {
    LogRecord record;
    record.lvl = log_level;
    record.tag = tag;
    record.msg = msg;
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = loc;

    post_queue->submit(std::move(record));
    return true;
}

void LoggerImpl::flush() {
    post_queue->flush();
}

void LoggerImpl::flush_sync() {
    post_queue->flush_sync();
}

void LoggerImpl::add_sink(std::shared_ptr<ISink> sink) {
    post_queue->add_sink(std::move(sink));
}

void LoggerImpl::remove_sink(ISink* sink) {
    post_queue->remove_sink(sink);
}

void LoggerImpl::clear_sinks() {
    post_queue->clear_sinks();
}

} // namespace cf::log
