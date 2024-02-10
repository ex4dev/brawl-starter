#ifndef INSTALLER_H
#define INSTALLER_H

#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QSettings>

class installer final : public QObject {
    Q_OBJECT

    public:
        installer(QSettings *settings);
        /**
         * Returns true if the KnockoutCity folder exists inside the game install directory.
        **/
        bool checkInstalled();
    public slots:
        void startGameInstallation();
    signals:
        void installationStarted();
        void progressUpdated(qint64 bytesReceived, qint64 bytesTotal);
        void downloadFinished();
        void finished();
    private:
        QNetworkAccessManager *manager;
        QSettings *m_settings;
        QFile *m_temp_zip_file;
        void writeNewData(QNetworkReply *data);
};

#endif
