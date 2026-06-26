#include "wallpaper_src_chain.h"
#include "aex/policy_chain/policy_chain.hpp"
#include "cfconfig.hpp"
#include "cflog.h"
#include "cfpath/desktop_main_path_resolvers.h"
#include <QDir>
#include <QFileInfo>

namespace cf::desktop::wallpaper {
aex::PolicyChain<QString> WallpaperImages() {
    return aex::policy_chain_builder<QString>()
        .then([]() -> std::optional<QString> {
            // Policy 1: Load from ConfigStore wallpaper domain
            log::trace("Scanning from the config file to load wallpaper");
            auto wp = cf::config::ConfigStore::instance().domain("wallpaper");
            auto path = wp.query<std::string>(
                cf::config::KeyView{.group = "wallpaper", .key = "source_path"}, "");
            if (!path.empty()) {
                QString qpath = QString::fromStdString(path);
                log::tracef("scan out the dirent: {}", qpath.toStdString());
                if (QFileInfo::exists(qpath)) {
                    return qpath;
                }
            }
            return std::nullopt;
        })
        .then([]() -> std::optional<QString> {
            /* If not, we use the pictures in Picture Dirent */
            return path::DesktopMainPathProvider::instance().absolutePath(
                path::DesktopMainPathProvider::PathType::Pictures);
        })
        .build();
}
} // namespace cf::desktop::wallpaper
