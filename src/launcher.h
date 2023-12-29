#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>

class launcher : public QObject
{
    Q_OBJECT
    public:
        launcher(QString *installDir, QString runner = "wine");
        void launchGame(QString backend, QString username, QString secret);
        void launchGame(QString backend, QString username);
        void openLoginUrl();
        void login(QString loginCode);
        void getKeyAndLaunch(QString username, QString authToken, QString server);
    signals:
        void loginResponseReceived(QJsonDocument document);
    private:
        QString *m_install_dir;
        QScopedPointer<QNetworkAccessManager> m_network_access_manager;
};

#endif // LAUNCHER_H
