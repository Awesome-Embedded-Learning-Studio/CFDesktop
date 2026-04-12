#include "wallpaper_src_chain.h"
#include "base/policy_chain/policy_chain.hpp"
#include "cfconfig.hpp"
#include "cfpath/desktop_main_path_resolvers.h"
#include <QDir>
#include <QFileInfo>

namespace cf::desktop::wallpaper {
PolicyChain<QString> WallpaperImages() {
    return policy_chain_builder<QString>()
        .then([]() -> std::optional<QString> {
            // Policy 1: Load from ConfigStore wallpaper domain
            auto wp = cf::config::ConfigStore::instance().domain("wallpaper");
            auto path = wp.query<std::string>(
                cf::config::KeyView{.group = "wallpaper", .key = "source_path"}, "");
            if (!path.empty()) {
                QString qpath = QString::fromStdString(path);
                if (QFileInfo::exists(qpath)) {
                    return qpath;
                }
            }
            return std::nullopt;
        })
        .then([]() -> std::optional<QString> {
            // Policy 2: Scan Pictures directory for any image
            auto pictures = path::DesktopMainPathProvider::instance().absolutePath(
                path::DesktopMainPathProvider::PathType::Pictures);
            QDir dir(pictures);
            QStringList files = dir.entryList({"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.webp"},
                                              QDir::Files, QDir::Name);
            if (!files.isEmpty()) {
                return dir.absoluteFilePath(files.first());
            }
            return std::nullopt;
        })
        .build();
}
} // namespace cf::desktop::wallpaper
