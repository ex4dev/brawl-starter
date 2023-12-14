#include "launcher.h"
#include <QProcess>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonObject>
#include <iostream>

launcher::launcher(QString installDir, QString runner) {
    m_install_dir = installDir;
    m_network_access_manager = new QNetworkAccessManager(this);
}

void launcher::launchGame(QString backend, QString username, QString secret) {
    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(m_install_dir + QDir::separator() + "KnockoutCity");
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process->start("/usr/bin/wine", { "KnockoutCity.exe", "-backend=" + backend, "-username=" + username, "-secret=" + secret });
    // TODO delete process?
}

void launcher::launchGame(QString backend, QString username) {
    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(m_install_dir + QDir::separator() + "KnockoutCity");
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    // TODO use runner variable instead of hardcoded wine
    process->start("/usr/bin/wine", { "KnockoutCity.exe", "-backend=" + backend, "-username=" + username});
    std::cout << "/usr/bin/wine" << process->arguments().join(" ").toStdString() << std::endl;
    // TODO delete process?
}

void launcher::openLoginUrl() {
    QDesktopServices::openUrl(QUrl("https://api.kocity.xyz/web/discord"));
}

// TODO add support for account registration
void launcher::login(QString loginCode) {
    QNetworkRequest request(QUrl("https://api.kocity.xyz/auth/login"));
    QString body = "{\"code\": \"" + loginCode + "\"}";
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_network_access_manager->post(request, body.toUtf8());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);
        reply->deleteLater();
        emit loginResponseReceived(document);
    });
}

void launcher::getKeyAndLaunch(QString username, QString authToken, QString server) {
    QNetworkRequest request(QUrl("https://api.kocity.xyz/auth/getkey"));
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
