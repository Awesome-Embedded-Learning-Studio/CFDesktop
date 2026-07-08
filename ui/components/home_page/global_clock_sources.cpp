/**
 * @file    global_clock_sources.cpp
 * @brief   Implementation of the GlobalClockSources singleton.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "global_clock_sources.h"

#include <QTime>
#include <QTimer>

namespace cf::desktop::desktop_component {

GlobalClockSources& GlobalClockSources::instance() {
    static GlobalClockSources source;
    return source;
}

GlobalClockSources::GlobalClockSources() : QObject(nullptr) {
    auto* timer = new QTimer(this);
    timer->setTimerType(Qt::CoarseTimer);
    connect(timer, &QTimer::timeout, this, [this]() { emit timeUpdate(QTime::currentTime()); });
    timer->start(1000);
}

} // namespace cf::desktop::desktop_component
