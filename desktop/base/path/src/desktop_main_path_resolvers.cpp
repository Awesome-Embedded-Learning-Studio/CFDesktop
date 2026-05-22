#include "cfpath/desktop_main_path_resolvers.h"
#include "cflog.h"
#include <QDir>

namespace {
QString make(const QString& root, const char* components) {
    QDir rootDir(root);
    return rootDir.absoluteFilePath(QString::fromUtf8(components));
}
} // namespace

namespace cf::desktop::path {

DesktopMainPathProvider::DesktopMainPathProvider(const QString& desktop_active_root)
    : root{desktop_active_root} {}

bool DesktopMainPathProvider::setup() {
    for (int i = 0; i < PathTypeCnt; ++i) {
        const auto p = static_cast<PathType>(i);
        if (component_exsited(p)) {
            log::tracef("Component [{}] existed at: {}", DesktopMainPathProvider::kPathNames[i],
                        absolutePath(p).toStdString());
            continue;
        }
        if (!QDir(absolutePath(p)).mkpath(QStringLiteral("."))) {
            return false;
        }
    }
    return true;
}

bool DesktopMainPathProvider::component_exsited(const PathType p) {
    return QDir(absolutePath(p)).exists();
}

bool DesktopMainPathProvider::request_created(const PathType p) {
    if (component_exsited(p)) {
        return true;
    }
    return QDir(absolutePath(p)).mkpath(QStringLiteral("."));
}

QString DesktopMainPathProvider::absolutePath(const PathType p) {
    return make(root, DesktopMainPathProvider::kPathNames[static_cast<size_t>(p)]);
}

} // namespace cf::desktop::path
