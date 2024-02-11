#include "launcher.h"
#include <QProcess>
#include <QNetworkReply>
#include <QJsonObject>
#include <iostream>
#include <QMessageBox>

#include "constants.h"

launcher::launcher(QSettings *settings) :
    m_settings(settings),
    m_network_access_manager(new QNetworkAccessManager(this)) {}

void launcher::launchGame(QString backend, QString username, QString secret) {
    QProcess *process = new QProcess(this);
    QDir installDir = QDir(m_settings->value(constants::SETTING_PATH_INSTALL_DIR, constants::SETTING_DEFAULT_INSTALL_DIR).toString());
    process->setWorkingDirectory(installDir.filePath(QStringLiteral("KnockoutCity")));
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    if (secret == nullptr) {
        process->start(constants::RUNNER_PATH, { "KnockoutCity.exe", "-backend=" + backend, "-username=" + username});
    } else {
        process->start(constants::RUNNER_PATH, { "KnockoutCity.exe", "-backend=" + backend, "-username=" + username, "-secret=" + secret });
    }

    connect(process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("An error occured with the game process: ") + QString::number(error));
    });
    connect(process, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus) {
        std::cout << "Game process exited with code " + exitCode << std::endl;
        process->deleteLater();
    });
}

void launcher::getKeyAndLaunch(QString username, QString authToken, QString server) {
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
