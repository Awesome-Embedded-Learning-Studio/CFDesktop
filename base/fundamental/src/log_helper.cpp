#include "cffundamental/log_helper.h"
#include <QDateTime>

namespace cf::desktop::logger_helper {

QString log_filename() {
    QString datetime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return QString("cfdesktop_%1.log").arg(datetime);
}

} // namespace cf::desktop::logger_helper
