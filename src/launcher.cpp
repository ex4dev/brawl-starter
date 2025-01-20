#include "launcher.h"
#include <QProcess>
#include <QNetworkReply>
#include <QJsonObject>
#include <iostream>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStringBuilder>

#include "constants.h"

launcher::launcher(QSettings *settings) :
    m_settings(settings),
    m_network_access_manager(new QNetworkAccessManager(this)) {}

void launcher::launchGame(const QString &backend, const QString &username, const QString &secret) {
    QString launchUrl = constants::STEAM_RUN_URL % "//-backend=" % backend % " -username=" % username % " -secret=" % secret;
    launchGame(launchUrl);
}

void launcher::launchGame(const QString &backend, const QString &username) {
    const QString launchUrl = constants::STEAM_RUN_URL % "//-backend=" % backend % " -username=" % username;
    launchGame(launchUrl);
}

void launcher::launchGame(const QString &launchUrl) {
    QDesktopServices::openUrl(QUrl(launchUrl));
}

void launcher::getKeyAndLaunch(const QString &username, const QString &authToken, const QString &server) {
    QNetworkRequest request(constants::XYZ_LOGIN_KEY_URL);
    QString body = "{\"username\": \"" + username + "\", \"authToken\": \"" + authToken + "\", \"server\": \"" + server + "\"}";
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_network_access_manager->post(request, body.toUtf8());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);
        QString authKey = document.object().value("authkey").toString(username);
        reply->deleteLater();
        launchGame(server, authKey);
    });
}
