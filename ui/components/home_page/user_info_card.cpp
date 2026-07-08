/**
 * @file    user_info_card.cpp
 * @brief   Implementation of UserInfoCard (CCIMX UserInfoCard style).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "user_info_card.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QSysInfo>
#include <QVBoxLayout>

#if defined(_WIN32)
#    include <lmcons.h>
#    include <windows.h>
#else
#    include <pwd.h>
#    include <unistd.h>
#endif

namespace cf::desktop::desktop_component {

namespace {
/// @brief Green-to-teal gradient QSS (ported from CCIMX UserInfoCard).
inline constexpr const char* kCardQss = R"(
    #UserInfoCard { border-radius: 20px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #43e97b, stop:1 #38f9d7);
    }
    #UserInfoCard QLabel { color: white; }
    #avatar_label { border-radius: 36px; background: rgba(255,255,255,0.95);
        color: #43e97b; font-size: 44px; font-weight: bold; }
    #name_label { font-size: 40px; font-weight: bold; }
    #host_label, #os_label { font-size: 20px; }
)";

/// @brief Reads the login user's display name (POSIX GECOS or Windows session user).
QString readUserName() {
#if defined(_WIN32)
    wchar_t buf[UNLEN + 1] = {};
    DWORD len = UNLEN + 1;
    if (GetUserNameW(buf, &len)) {
        return QString::fromWCharArray(buf);
    }
    return QStringLiteral("user");
#else
    const struct passwd* pw = getpwuid(getuid());
    if (pw != nullptr) {
        if (pw->pw_gecos != nullptr && pw->pw_gecos[0] != '\0') {
            const QString gecos = QString::fromLocal8Bit(pw->pw_gecos);
            const int comma = gecos.indexOf(',');
            const QString first = (comma >= 0) ? gecos.left(comma) : gecos;
            if (!first.trimmed().isEmpty()) {
                return first.trimmed();
            }
        }
        if (pw->pw_name != nullptr && pw->pw_name[0] != '\0') {
            return QString::fromLocal8Bit(pw->pw_name);
        }
    }
    return QStringLiteral("user");
#endif
}

/// @brief Reads the machine hostname (cross-platform via QSysInfo).
QString readHostName() {
    return QSysInfo::machineHostName();
}

/// @brief Reads the OS name and architecture (cross-platform via QSysInfo).
QString readOsLine() {
    return QSysInfo::prettyProductName();
}
} // namespace

UserInfoCard::UserInfoCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("UserInfoCard");
    setStyleSheet(kCardQss);

    auto* main = new QHBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(20);

    avatar_label_ = new QLabel(this);
    avatar_label_->setObjectName("avatar_label");
    avatar_label_->setFixedSize(72, 72);
    avatar_label_->setAlignment(Qt::AlignCenter);
    main->addWidget(avatar_label_, 0, Qt::AlignVCenter);

    auto* info = new QVBoxLayout;
    info->setSpacing(6);
    name_label_ = new QLabel(this);
    name_label_->setObjectName("name_label");
    host_label_ = new QLabel(this);
    host_label_->setObjectName("host_label");
    os_label_ = new QLabel(this);
    os_label_->setObjectName("os_label");
    info->addWidget(name_label_);
    info->addWidget(host_label_);
    info->addWidget(os_label_);
    info->addStretch();
    main->addLayout(info, 1);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);

    refresh();
}

void UserInfoCard::refresh() {
    const QString name = readUserName();
    name_label_->setText(name);
    host_label_->setText(readHostName());
    os_label_->setText(readOsLine());
    // Use the first character (uppercased) as the avatar initial.
    const QChar initial = name.isEmpty() ? QChar('?') : name.at(0).toUpper();
    avatar_label_->setText(initial);
}

} // namespace cf::desktop::desktop_component
