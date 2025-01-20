#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QSettings>
#include <QNetworkAccessManager>
#include <QJsonDocument>

class launcher final : public QObject
{
    Q_OBJECT
    public:
        launcher(QSettings *settings);
        void launchGame(const QString &backend, const QString &username, const QString &secret);
        void launchGame(const QString &backend, const QString &username);
        void launchGame(const QString &launchUrl);
        void getKeyAndLaunch(const QString &username, const QString &authToken, const QString &server);

    private:
        QSettings *m_settings;
        QScopedPointer<QNetworkAccessManager> m_network_access_manager;
};

#endif // LAUNCHER_H
