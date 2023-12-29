#ifndef KOCITY_QT_H
#define KOCITY_QT_H

#include <QTableWidget>
#include <QMainWindow>
#include <QScopedPointer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include <QProgressBar>
#include <src/installer.h>
#include <src/server_query.h>
#include <src/launcher.h>
#include "settings_dialog.h"

namespace Ui {
class kocity_qt;
}

class kocity_qt : public QMainWindow
{
    Q_OBJECT

public:
    explicit kocity_qt(QWidget *parent = nullptr);
    ~kocity_qt() override;

private:
    QScopedPointer<Ui::kocity_qt> m_ui;
    QScopedPointer<QSettings> m_settings;
    QString m_game_directory;
    QProgressBar *m_download_progress_bar;
    QScopedPointer<installer> m_installer;
    QScopedPointer<server_query> m_server_query_manager;
    QScopedPointer<launcher> m_launcher;
    static void insertTableRow(QTableWidget *tableWidget, QStringList itemText);

private slots:
    void launchGame();
    void openSettings();
    void gameInstallationStarted();
    void gameDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal);
    void gameDownloadFinished();
    void gameInstallationFinished();
    void publicServersReceived(QJsonDocument document);
    void refreshServerList();
    void loginActionTriggered();
    void loginResponseReceived(QJsonDocument document);
    void addServer();
    void deleteServer();
};

#endif // KOCITY_QT_H
