#include "installer.h"
#include <iostream>
#include <qdir.h>
#include <qprocess.h>

installer::installer(QString *installDir) {
    m_install_dir = installDir;
    manager = new QNetworkAccessManager(this);
}
void installer::startGameInstallation() {
    QUrl requestUrl("https://chonky-delivery-network.akamaized.net/KnockoutCity-HighRes-10.0-269701.zip");

    // Step 1: open the file
    // TODO make it automatically create the folder

    m_temp_zip_file = new QFile(*m_install_dir + QDir::separator() + requestUrl.fileName());
    std::cout << "Saving to " << m_temp_zip_file->fileName().toStdString() << std::endl;
    if (!m_temp_zip_file->open(QIODevice::WriteOnly)) {
        std::cout << "Warning: failed to open file" << std::endl;
        delete m_temp_zip_file;
        return;
    }

    // Step 2: create the request
    QNetworkRequest request(requestUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true); // TODO is this needed?
    QNetworkReply* reply = manager->get(request);

    // Step 3: read data when it's ready
    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        // When new data can be written
        writeNewData(reply);
    });

    connect(reply, &QNetworkReply::finished, this, [=]() {
        writeNewData(reply);
        std::cout << "Download finished." << std::endl;
        QProcess *process = new QProcess(this);
        emit downloadFinished();
        process->connect(process, &QProcess::stateChanged, this, [=](QProcess::ProcessState newState) {
            if (newState == QProcess::ProcessState::NotRunning) {
                std::cout << "Installation complete." << std::endl;
                m_temp_zip_file->remove();
                m_temp_zip_file->deleteLater();
                emit finished();
                process->deleteLater();
            }
        });
        process->connect(process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
            std::cout << "Error occured while extracting the files: " << error << std::endl;
        });
        process->setWorkingDirectory(*m_install_dir);
        process->start("unzip", QStringList() << reply->request().url().fileName());
        reply->deleteLater();
    });
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        if (bytesTotal < 0) return;
        emit progressUpdated(bytesReceived, bytesTotal);
    });
    emit installationStarted();
}

void installer::writeNewData(QNetworkReply *reply) {
    QByteArray data = reply->readAll();
    if (m_temp_zip_file != nullptr && m_temp_zip_file->isOpen()) {
        m_temp_zip_file->write(data);
    } else {
        std::cout << "Failed to write to file. Aborting download." << std::endl;
        delete m_temp_zip_file;
        reply->abort();
        reply->deleteLater();
        return;
    }
}

bool installer::checkInstalled() {
    QFileInfo info(*m_install_dir + QDir::separator() + "KnockoutCity");
    return info.exists() && info.isDir();
}
