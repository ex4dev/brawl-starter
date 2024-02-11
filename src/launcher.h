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
        void launchGame(QString backend, QString username, QString secret = nullptr);
        void getKeyAndLaunch(QString username, QString authToken, QString server);
    private:
        QSettings *m_settings;
        QScopedPointer<QNetworkAccessManager> m_network_access_manager;
};

#endif // LAUNCHER_H
