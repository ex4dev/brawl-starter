#include "server_query.h"

server_query::server_query() {
    m_manager = new QNetworkAccessManager(this);
    m_manager->connect(m_manager, &QNetworkAccessManager::finished, this, &server_query::onReceivePublicServers);
}
void server_query::getPublicServers()
{
    QUrl requestUrl("https://api.kocity.xyz/stats/servers");
    QNetworkRequest request(requestUrl);
    m_manager->get(request);
}

void server_query::onReceivePublicServers(QNetworkReply *reply) {
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    emit publicServersReceived(document);

}

