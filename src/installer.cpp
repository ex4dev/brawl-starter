#include "installer.h"
#include <qdir.h>
#include <QMessageBox>
#include <qprocess.h>
#include <QSettings>

#include "constants.h"

installer::installer(QSettings *settings) {
    m_settings = settings;
    manager = new QNetworkAccessManager(this);
}
void installer::startGameInstallation() {
    const QUrl requestUrl(QStringLiteral("https://chonky-delivery-network.akamaized.net/KnockoutCity-HighRes-10.0-269701.zip"));

    // Step 1: open the file

    const QDir installDir(m_settings->value(constants::SETTING_PATH_INSTALL_DIR, constants::SETTING_DEFAULT_INSTALL_DIR).toString());
    if (!installDir.exists() && !installDir.mkpath(".")) {
        QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Failed to create directory at ") + installDir.path());
        return;
    }

    m_temp_zip_file = new QFile(installDir.filePath(requestUrl.fileName()));
    if (!m_temp_zip_file->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Failed to open download file. Double check that write permissions are enabled for the installation directory."));
        delete m_temp_zip_file;
        return;
    }

    // Step 2: create the request
    QNetworkRequest request(requestUrl);
    QNetworkReply* reply = manager->get(request);

    // Step 3: read data when it's ready
    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        // When new data can be written
        writeNewData(reply);
    });

    connect(reply, &QNetworkReply::finished, this, [=]() {
        writeNewData(reply);
        QProcess *process = new QProcess(this);
        emit downloadFinished();
        connect(process, &QProcess::stateChanged, this, [=](QProcess::ProcessState newState) {
            if (newState == QProcess::ProcessState::NotRunning) {
                m_temp_zip_file->remove();
                m_temp_zip_file->deleteLater();
                emit finished();
                process->deleteLater();
            }
        });
        connect(process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
            QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("An error occured while extracting the downloaded files: ") + QString::number(error));
        });
        process->setWorkingDirectory(installDir.path());
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
        QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Failed to write new data to file. Aborting download."));
        delete m_temp_zip_file;
        reply->abort();
        reply->deleteLater();
    }
}

bool installer::checkInstalled() {
    QSettings settings;
    QString installDir = m_settings->value(constants::SETTING_PATH_INSTALL_DIR, constants::SETTING_DEFAULT_INSTALL_DIR).toString();
    QFileInfo info(QDir(installDir).filePath("KnockoutCity"));
    return info.exists() && info.isDir();
}
