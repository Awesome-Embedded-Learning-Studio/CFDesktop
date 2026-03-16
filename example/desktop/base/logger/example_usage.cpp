#include "cflog.h"
#include <iostream>

void run_logger_init();

int main() {
    std::cout << "Welcome! This is CFLog Module Example!" << std::endl;
    run_logger_init();
    using namespace cf::log;
    set_level(level::TRACE);

    trace("Hello Trace!");
    debug("Hello debug");
    info("Hello Info");
    warning("Hello warning");
    error("Hello error");

    flush();
    std::cout << "End! This is CFLog Module Example!" << std::endl;
}