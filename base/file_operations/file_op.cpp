#include "file_op.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

namespace cf::desktop::settings::path {
static constexpr const char* DESKTOP_ROOT = "cfdesktop";
static constexpr const char* DESKTOP = "desktop";
static constexpr const char* RUNTIME = "runtime";
} // namespace cf::desktop::settings::path

namespace cf::desktop::base::filesystem {
// create file or dirent anyway
bool create_anyway(const QString& path) {
    if (path.isEmpty()) {
        return false;
    }

    QFileInfo info(path);
    if (info.exists()) {
        return true;
    }

    // Create parent directories if they don't exist
    QDir dir;
    if (!dir.mkpath(info.absolutePath())) {
        return false;
    }

    // Determine if it's a file or directory based on extension
    // If path ends with '/', it's a directory
    if (path.endsWith('/') || path.endsWith(QDir::separator())) {
        return dir.mkdir(path);
    }

    // If the path has an extension (contains '.' after the last separator), treat as file
    // Otherwise, treat as directory
    QString fileName = info.fileName();
    if (fileName.contains('.')) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            return true;
        }
        return false;
    } else {
        return dir.mkdir(path);
    }
}

QString app_runtime_dir() {
    return QCoreApplication::applicationDirPath();
}

QString concat_filepath(const QString& dirent, const QString& rest_dir) {
    return QDir(dirent).filePath(rest_dir);
}

bool exsited(const QString& path) {
    if (path.isEmpty()) {
        return false;
    }
    return QFileInfo::exists(path);
}

} // namespace cf::desktop::base::filesystem