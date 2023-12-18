#ifndef INSTALLER_H
#define INSTALLER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <qfile.h>

class installer : public QObject {
    Q_OBJECT

    public:
        installer(QString installDir);
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
        QString m_install_dir;
        QFile *m_temp_zip_file;
        void writeNewData(QNetworkReply *data);
};

#endif
