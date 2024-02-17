#include "kocity-qt.h"
#include "ui_kocity-qt.h"
#include <qdialog.h>
#include <QMessageBox>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QInputDialog>
#include <src/secrets.h>
#include "add_server_dialog.h"
#include "settings_dialog.h"
#include "src/constants.h"

kocity_qt::kocity_qt(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::kocity_qt),
    m_settings(new QSettings()),
    m_download_progress_bar(nullptr),
    m_installer(new installer(m_settings.get())),
    m_launcher(new launcher(m_settings.get())),
    m_server_query_manager(new server_query())
{

    m_ui->setupUi(this);
    m_ui->serverListWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_ui->actionPlay, &QAction::triggered, this, &kocity_qt::launchGame);
    connect(m_ui->actionSettings, &QAction::triggered, this, &kocity_qt::openSettings);
    connect(m_ui->actionAddServer, &QAction::triggered, this, &kocity_qt::addServer);
    connect(m_ui->actionRefresh, &QAction::triggered, this, &kocity_qt::refreshServerList);
    connect(m_ui->actionDeleteServer, &QAction::triggered, this, &kocity_qt::deleteServer);

    if (!m_installer->checkInstalled()) m_ui->actionPlay->setText(QStringLiteral("Install"));

    connect(m_installer.data(), &installer::installationStarted, this, &kocity_qt::gameInstallationStarted);
    connect(m_installer.data(), &installer::progressUpdated, this, &kocity_qt::gameDownloadProgressUpdated);
    connect(m_installer.data(), &installer::downloadFinished, this, &kocity_qt::gameDownloadFinished);
    connect(m_installer.data(), &installer::finished, this, &kocity_qt::gameInstallationFinished);

    connect(m_server_query_manager.data(), &server_query::publicServersReceived, this, &kocity_qt::publicServersReceived);

    refreshServerList();
}

kocity_qt::~kocity_qt() = default;

void kocity_qt::insertTableRow(QTableWidget* tableWidget, QStringList itemText)
{
    int rowCount = tableWidget->rowCount();
    tableWidget->insertRow(rowCount);
    for (qsizetype i = 0; i < itemText.size(); ++i) {
        const QString &str = itemText.at(i);
        QTableWidgetItem *newItem = new QTableWidgetItem(str);
        tableWidget->setItem(rowCount, i, newItem);
    }
}

void kocity_qt::launchGame() {
    if (!m_installer->checkInstalled()) {
        m_installer->startGameInstallation();
        return;
    }
    QList<QTableWidgetItem*> selectedItems = m_ui->serverListWidget->selectedItems();
    if (selectedItems.size() == 0) {
        QMessageBox::critical(this, constants::STR_ERROR, QStringLiteral("Please select a server."));
        return;
    }
    QString selectedAddress = m_ui->serverListWidget->selectedItems()[1]->text();
    QString selectedType = m_ui->serverListWidget->selectedItems()[4]->text();
    QString selectedServerName = m_ui->serverListWidget->selectedItems()[0]->text();
    int authType = selectedType == QStringLiteral("Default") ? 1 : m_settings->value(constants::SETTING_PATH_SERVER_AUTHENTICATION + selectedServerName).toInt();
    if (authType == 2) {
        QString username = m_settings->value(constants::SETTING_PATH_OFFLINE_USERNAME, constants::SETTING_DEFAULT_OFFLINE_USERNAME).toString();
        QString password = m_settings->value(constants::SETTING_PATH_SERVER_PASSWORD + selectedServerName).toString();
        m_launcher->launchGame(selectedAddress, username, password);
        m_ui->statusBar->showMessage("Launching " + selectedServerName + " as " + username);
    } else if (authType == 1){
        QString username = m_settings->value(constants::SETTING_PATH_USERNAME).toString();
        if (username.isEmpty()) {
            QMessageBox::critical(this, constants::STR_ERROR, QStringLiteral("Before you can join this server, you must log in from the settings window."));
            return;
        }
        m_ui->statusBar->showMessage(QStringLiteral("Logging in and launching ") + selectedServerName);
        // TODO retrieve token asynchronously?
        GError *error = nullptr;
        gchar *token_cstr = getTokenSync(username.toLocal8Bit().data(), &error);
        const QString token(token_cstr);
        secret_password_free(token_cstr);
        if (error != nullptr) {
            QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Could not retrieve login token: ") + error->message);
            g_error_free(error);
            return;
        }
        if (token.isEmpty()) {
            QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("No login token stored. Please log in again."));
            return;
        }
        m_launcher->getKeyAndLaunch(username, token, selectedAddress);
    } else {
        QString username = m_settings->value(constants::SETTING_PATH_OFFLINE_USERNAME, constants::SETTING_DEFAULT_OFFLINE_USERNAME).toString();
        m_ui->statusBar->showMessage("Launching " + selectedServerName + " as " + username);
        m_launcher->launchGame(selectedAddress, username);
    }

}

void kocity_qt::openSettings() {
    settings_dialog settingsDialog(m_settings.get());
    settingsDialog.exec();
}

void kocity_qt::gameInstallationStarted() {
    m_ui->statusBar->showMessage(QStringLiteral("Downloading files..."));
    if (m_download_progress_bar == nullptr) {
        m_download_progress_bar = new QProgressBar(this);
        m_ui->statusBar->addPermanentWidget(m_download_progress_bar);
    }
    m_download_progress_bar->setValue(0);
}

void kocity_qt::gameDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal) {
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/tswanson/kocity-qt"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    double percentComplete = (double) bytesReceived / bytesTotal;
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), true);
    properties.insert(QStringLiteral("progress"), (percentComplete));

    message << QStringLiteral("application://kocity-qt.desktop")
            << properties;
    QDBusConnection::sessionBus().send(message);

    m_download_progress_bar->setValue(100 * percentComplete);
}

void kocity_qt::gameDownloadFinished() {
    m_ui->statusBar->showMessage(QStringLiteral("Extracting downloaded files..."));
}

void kocity_qt::gameInstallationFinished()
{
    m_ui->actionPlay->setText(QStringLiteral("Play"));
    m_ui->statusBar->showMessage(QStringLiteral("Installation complete."));
    m_ui->statusBar->removeWidget(m_download_progress_bar);
    m_download_progress_bar->deleteLater();
    // TODO linux check
    // Hide taskbar download progress
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/tswanson/kocity-qt"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), false);
    properties.insert(QStringLiteral("progress"), 0);
    message << QStringLiteral("application://kocity-qt.desktop")
            << properties;
    QDBusConnection::sessionBus().send(message);
    system(R"(notify-send -a kocity-qt "Installation complete" "Knockout City is installed and ready to be played.")");
}

void kocity_qt::publicServersReceived(QJsonDocument document)
{
    for (QJsonValueRef element : document.array()) {
        QJsonObject obj = element.toObject();
        QString status = obj.find(QLatin1String("status")).value().toString(QLatin1String("Unknown"));
        QString name = "[" + obj.find(QLatin1String("region")).value().toString(QLatin1String("?")) + "] " + obj.find(QLatin1String("name")).value().toString(QLatin1String("Unnamed"));
        QString ip = obj.find(QLatin1String("ip")).value().toString(QLatin1String("Unknown"));
        int players = obj.find(QLatin1String("players")).value().toInt(-1);
        int maxPlayers = obj.find(QLatin1String("maxPlayers")).value().toInt(-1);
        insertTableRow(m_ui->serverListWidget, { name, ip, QString::number(players) + " / " + QString::number(maxPlayers), status, QStringLiteral("Default")});
    }
}

void kocity_qt::refreshServerList() {
    m_ui->serverListWidget->clearContents();
    m_ui->serverListWidget->setRowCount(0);
    QStringList privateServers = m_settings->value(constants::SETTING_PATH_SERVERS).toStringList();
    for (const QString &serverName : privateServers) {
        QString serverAddress = m_settings->value(constants::SETTING_PATH_SERVER_ADDRESS + serverName).toString();
        insertTableRow(m_ui->serverListWidget, { serverName, serverAddress, "", "", "Custom" });
    }
    m_server_query_manager->getPublicServers();
}

void kocity_qt::addServer() {
    add_server_dialog dialog;
    if (dialog.exec()) {
        QString serverName = dialog.serverName();
        QStringList servers = m_settings->value(constants::SETTING_PATH_SERVERS).toStringList() << serverName;
        m_settings->setValue(constants::SETTING_PATH_SERVERS, servers);
        m_settings->setValue(constants::SETTING_PATH_SERVER_ADDRESS + serverName, dialog.serverAddress());
        m_settings->setValue(constants::SETTING_PATH_SERVER_AUTHENTICATION + serverName, dialog.serverAuthType());
        m_settings->setValue(constants::SETTING_PATH_SERVER_PASSWORD + serverName, dialog.serverPassword());

        insertTableRow(m_ui->serverListWidget, { serverName, dialog.serverAddress(), "", "", "Custom"});
    }
}

void kocity_qt::deleteServer() {
    QList<QTableWidgetItem*> selectedItems = m_ui->serverListWidget->selectedItems();
    if (selectedItems.size() == 0) {
        QMessageBox::critical(this, constants::STR_ERROR, QStringLiteral("Please select a server to delete."));
        return;
    }
    QString selectedServerType = selectedItems[4]->text();
    if (selectedServerType == "Default") {
        QMessageBox::critical(this, constants::STR_ERROR, QStringLiteral("Default servers cannot be removed from the server list."));
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this, QStringLiteral("Delete Server"), QStringLiteral("Are you sure you want to delete this server?"));
    if (reply == QMessageBox::Yes) {
        QString selectedServerName = selectedItems[0]->text();
        QStringList servers = m_settings->value(constants::SETTING_PATH_SERVERS).toStringList();
        servers.removeAll(selectedServerName);
        m_settings->setValue(constants::SETTING_PATH_SERVERS, servers);
        m_settings->remove(constants::SETTING_PATH_SERVER_ADDRESS + selectedServerName);
        m_settings->remove(constants::SETTING_PATH_SERVER_AUTHENTICATION + selectedServerName);
        m_settings->remove(constants::SETTING_PATH_SERVER_PASSWORD + selectedServerName);
        m_ui->serverListWidget->model()->removeRow(selectedItems[0]->row());
    }
}



#include "moc_kocity-qt.cpp"
