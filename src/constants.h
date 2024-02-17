#ifndef KOCITYQT_CONSTANTS_H
#define KOCITYQT_CONSTANTS_H
#include <QString>
#include <QDir>
#include <QVariant>
#include <QUrl>

namespace constants {
    static inline const QString SETTING_DEFAULT_INSTALL_DIR = QDir::homePath() + QDir::separator() + ".kocityqt";
    static inline const QVariant SETTING_DEFAULT_OFFLINE_USERNAME = QVariant(QStringLiteral("Brawler"));
    static inline const QString SETTING_PATH_USERNAME = QStringLiteral("auth/username");
    static inline const QString SETTING_PATH_OFFLINE_USERNAME = QStringLiteral("offline-username");
    static inline const QString SETTING_PATH_INSTALL_DIR = QStringLiteral("directory");
    static inline const QString SETTING_PATH_SERVERS = QStringLiteral("servers/servers");
    static inline const QString SETTING_PATH_SERVER_ADDRESS = QStringLiteral("servers/address_");
    static inline const QString SETTING_PATH_SERVER_AUTHENTICATION = QStringLiteral("servers/authentication_");
    static inline const QString SETTING_PATH_SERVER_PASSWORD = QStringLiteral("servers/password_");

    static inline const QString RUNNER_PATH = QStringLiteral("/usr/bin/wine");

    static inline const QUrl XYZ_LOGIN_PAGE_URL = QUrl(QStringLiteral("https://api.kocity.xyz/web/discord"));
    static inline const QUrl XYZ_LOGIN_REQUEST_URL = QUrl(QStringLiteral("https://api.kocity.xyz/auth/login"));
    static inline const QUrl XYZ_LOGIN_KEY_URL = QUrl(QStringLiteral("https://api.kocity.xyz/auth/getkey"));
    static inline const QUrl XYZ_REGISTER_REQUEST_URL = QUrl(QStringLiteral("https://api.kocity.xyz/auth/register"));

    static inline const QString STR_ERROR = QStringLiteral("Error");
}

#endif //KOCITYQT_CONSTANTS_H
