#ifndef SERVER_QUERY_H
#define SERVER_QUERY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class server_query : public QObject
{
    Q_OBJECT
    public:
        server_query();
    public slots:
        void getPublicServers();
    signals:
        void publicServersReceived(QJsonDocument document);
    private slots:
        void onReceivePublicServers(QNetworkReply *reply);
    private:
        QNetworkAccessManager *m_manager;
};
#endif // SERVER_QUERY_H
