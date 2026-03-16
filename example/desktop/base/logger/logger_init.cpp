#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/formatter/file_formatter.h"
#include "cflog/sinks/console_sink.h"
#include "cflog/sinks/file_sink.h"
#include <memory>

static cf::log::FormatterFactory factory;

void run_logger_init() {
    using namespace cf::log;
    // OK, init the logger with file and sinks
    factory.register_formatter("console", []() { return std::make_shared<AsciiColorFormatter>(); });
    factory.register_formatter("file", []() { return std::make_shared<FileFormatter>(); });

    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink); //< init here

    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink); //< init here
}
