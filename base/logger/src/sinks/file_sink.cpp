#include "cflog/sinks/file_sink.h"
#include "cflog/cflog_level.hpp"
#include "cflog/formatter/default_formatter.h"
#include <filesystem>
#include <iostream>
#include <memory>

namespace cf::log {

FileSink::FileSink(const std::string& filepath, OpenMode mode) : filepath_(filepath), mode_(mode) {
    try {
        std::filesystem::path file_path(filepath);
        if (file_path.has_parent_path()) {
            std::filesystem::create_directories(file_path.parent_path());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[FileSink] Failed to create directory: " << e.what() << std::endl;
        return;
    }

    std::ios::openmode openmode = std::ios::out;
    if (mode_ == OpenMode::Append) {
        openmode |= std::ios::app;
    } else {
        openmode |= std::ios::trunc;
    }

    file_.open(filepath_, openmode);

    if (!file_.is_open()) {
        std::cerr << "[FileSink] Failed to open file: " << filepath_ << std::endl;
    }
}

FileSink::~FileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool FileSink::write(const LogRecord& record) {
    if (!file_.is_open()) {
        return false;
    }

    if (!formatter_) {
        formatter_ = std::make_shared<DefaultFormatter>();
    }

    const auto str = formatter_->format_me(record);
    file_ << str;

    if (record.lvl == level::ERROR) {
        file_.flush(); // if we meet sth important, flush it!
    }

    return !file_.fail();
}

bool FileSink::flush() {
    if (!file_.is_open()) {
        return false;
    }
    file_.flush();
    return !file_.fail();
}

} // namespace cf::log
