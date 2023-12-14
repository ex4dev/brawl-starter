#ifndef KOCITY_QT_H
#define KOCITY_QT_H

#include <QTableWidget>
#include <QMainWindow>
#include <QScopedPointer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include <src/installer.h>
#include <src/server_query.h>
#include <src/launcher.h>
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
    installer *m_installer;
    server_query *m_server_query_manager;
    launcher *m_launcher;
    static void insertTableRow(QTableWidget *tableWidget, QStringList itemText);

private slots:
    void launchGame();
    void openSettings();
    void gameInstallationStarted();
    void gameDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal);
    void gameDownloadFinished();
    void gameInstallationFinished();
    void publicServersReceived(QJsonDocument document);
    void loginActionTriggered();
    void loginResponseReceived(QJsonDocument document);
};

#endif // KOCITY_QT_H
