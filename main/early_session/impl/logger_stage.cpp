#include "logger_stage.h"
#include "cflog.h"
#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/formatter/file_formatter.h"
#include "cflog/sinks/console_sink.h"
#include "cflog/sinks/file_sink.h"
#include "early_handle/early_handle.h"
#include <memory>

namespace cf::desktop::early_stage {
LoggerStage::BootResult LoggerStage::run_session() {
    using namespace cf::log;
    static FormatterFactory factory;

    // OK, init the logger with file and sinks
    factory.register_formatter("console", []() { return std::make_shared<AsciiColorFormatter>(); });
    factory.register_formatter("file", []() { return std::make_shared<FileFormatter>(); });

    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink); //< init here

    const auto logger_path =
        EarlyHandle::instance().early_settings().get_boot_logger_path().toStdString();
    auto file_sink = std::make_shared<FileSink>(logger_path);
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink); //< init here

    /* Logger OK */
    using namespace cf::log;

    // Log initialization details with debugftag
    debugftag("LoggerStage", "Console sink initialized with AsciiColorFormatter");
    debugftag("LoggerStage", "File sink initialized at: {}", logger_path);
    debugftag("LoggerStage", "File formatter configured");

    // Pretty print CFLog Ready messages at all levels
    info("═══════════════════════════════════════════════════════════════", "LoggerStage");
    info("               ✦ CFLog System Initialized ✦                    ", "LoggerStage");
    info("═══════════════════════════════════════════════════════════════", "LoggerStage");

    return BootResult::OK;
}

} // namespace cf::desktop::early_stage