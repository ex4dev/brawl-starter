#include "kocity-qt.h"
#include "ui_kocity-qt.h"
#include <iostream>
#include <qdialog.h>
#include <QMessageBox>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QInputDialog>
#include <QDir>
#include "add_server_dialog.h"
#include "settings_dialog.h"

using std::cout;
using std::endl;

kocity_qt::kocity_qt(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::kocity_qt),
    m_settings(new QSettings()),
    m_game_directory(m_settings->value("directory", QDir::homePath() + QDir::separator() + ".kocityqt").toString()),
    m_installer(new installer(&m_game_directory)),
    m_server_query_manager(new server_query()),
    m_launcher(new launcher(&m_game_directory))
{

    m_ui->setupUi(this);
    m_ui->serverListWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_ui->actionPlay, &QAction::triggered, this, &kocity_qt::launchGame);
    connect(m_ui->actionSettings, &QAction::triggered, this, &kocity_qt::openSettings);
    connect(m_ui->actionAddServer, &QAction::triggered, this, &kocity_qt::addServer);
    connect(m_ui->actionRefresh, &QAction::triggered, this, &kocity_qt::refreshServerList);
    connect(m_ui->actionDeleteServer, &QAction::triggered, this, &kocity_qt::deleteServer);

    m_ui->progressBar->hide();

    connect(m_installer.data(), &installer::installationStarted, this, &kocity_qt::gameInstallationStarted);
    connect(m_installer.data(), &installer::progressUpdated, this, &kocity_qt::gameDownloadProgressUpdated);
    connect(m_installer.data(), &installer::downloadFinished, this, &kocity_qt::gameDownloadFinished);
    connect(m_installer.data(), &installer::finished, this, &kocity_qt::gameInstallationFinished);

    connect(m_server_query_manager.data(), &server_query::publicServersReceived, this, &kocity_qt::publicServersReceived);

    connect(m_launcher.data(), &launcher::loginResponseReceived, this, &kocity_qt::loginResponseReceived);

    refreshServerList();
}

kocity_qt::~kocity_qt() = default;

void kocity_qt::insertTableRow(QTableWidget* tableWidget, QStringList itemText)
{
    int rowCount = tableWidget->rowCount();
    int columnCount = tableWidget->columnCount();
    tableWidget->insertRow(rowCount);
    for (qsizetype i = 0; i < itemText.size(); ++i) {
        const QString &str = itemText.at(i);
        QTableWidgetItem *newItem = new QTableWidgetItem(str);
        tableWidget->setItem(rowCount, i, newItem);
    }
}

void kocity_qt::launchGame() {
    QList<QTableWidgetItem*> selectedItems = m_ui->serverListWidget->selectedItems();
    if (selectedItems.size() == 0) {
        QMessageBox::critical(this, "Error", "Please select a server.");
        return;
    }
    // TODO test this
    if (!m_installer->checkInstalled()) {
        if (m_download_progress_bar == nullptr) {
            m_download_progress_bar = new QProgressBar();
            m_ui->statusBar->addPermanentWidget(m_download_progress_bar);
        }
        m_installer->startGameInstallation();
        return;
    }
    QString selectedAddress = m_ui->serverListWidget->selectedItems()[1]->text();
    QString selectedType = m_ui->serverListWidget->selectedItems()[4]->text();
    QString selectedServerName = m_ui->serverListWidget->selectedItems()[0]->text();
    int authType = selectedType == "Default" ? 1 : m_settings->value("servers/authentication_" + selectedServerName).toInt();
    if (authType == 2) {
        QString username = m_settings->value("offline-username", QVariant("Unnamed")).toString();
        QString password = m_settings->value("servers/password_" + selectedServerName).toString();
        m_launcher->launchGame(selectedAddress, username, password);
        m_ui->statusBar->showMessage("Launching " + selectedServerName + " as " + username);
    } else if (authType == 1){
        QString username = m_settings->value("auth/username").toString();
        QString token = m_settings->value("auth/token").toString();
        if (username.isEmpty() || token.isEmpty()) {
            QMessageBox::critical(this, "Error", "You must log in to join this server.");
            return;
        }
        m_ui->statusBar->showMessage("Logging in and launching " + selectedServerName);
        m_launcher->getKeyAndLaunch(username, token, selectedAddress);
    } else {
        QString username = m_settings->value("offline-username", QVariant("Unnamed")).toString();
        m_ui->statusBar->showMessage("Launching " + selectedServerName + " as " + username);
        m_launcher->launchGame(selectedAddress, username);
    }

}

void kocity_qt::openSettings() {
    settings_dialog settingsDialog(this);
    connect(&settingsDialog, &settings_dialog::onClickLogIn, this, &kocity_qt::loginActionTriggered);
    cout << settingsDialog.exec() << endl;

}

void kocity_qt::gameInstallationStarted() {
    m_ui->statusBar->showMessage("Downloading files...");
    m_ui->progressBar->setValue(0);
    m_ui->progressBar->show();
}

void kocity_qt::gameDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal) {
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/ex4/KoCityQt"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    double percentComplete = (double) bytesReceived / bytesTotal;
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), true);
    properties.insert(QStringLiteral("progress"), (percentComplete));

    message << QStringLiteral("application://kocityqt.desktop")
            << properties;
    QDBusConnection::sessionBus().send(message);

    m_ui->progressBar->setValue(100 * percentComplete);
}

void kocity_qt::gameDownloadFinished() {
    m_ui->progressBar->hide();
    m_ui->statusBar->showMessage("Extracting downloaded files...");
    cout << "[UI] Download finished" << endl;
}

void kocity_qt::gameInstallationFinished()
{
    m_ui->statusBar->showMessage("Installation complete.");
    // TODO linux check
    // Hide taskbar download progress
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/ex4/KoCityQt"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), false);
    properties.insert(QStringLiteral("progress"), 0);
    message << QStringLiteral("application://kocityqt.desktop")
            << properties;
    QDBusConnection::sessionBus().send(message);
    system("notify-send -a kocityqt \"Installation complete\" \"Knockout City is installed and ready to be played.\"");
}

void kocity_qt::publicServersReceived(QJsonDocument document)
{
    for (QJsonValueRef element : document.array()) {
        QJsonObject obj = element.toObject();
        QString status = obj.find("status").value().toString("Unknown");
        QString name = "[" + obj.find("region").value().toString("?") + "] " + obj.find("name").value().toString("Unnamed");
        QString ip = obj.find("ip").value().toString("Unknown");
        int players = obj.find("players").value().toInt(-1);
        int maxPlayers = obj.find("maxPlayers").value().toInt(-1);
        insertTableRow(m_ui->serverListWidget, { name, ip, QString::number(players) + " / " + QString::number(maxPlayers), status, "Default"});
    }
}

void kocity_qt::refreshServerList() {
    m_ui->serverListWidget->clearContents();
    m_ui->serverListWidget->setRowCount(0);
    QStringList privateServers = m_settings->value("servers/servers").toStringList();
    for (QString serverName : privateServers) {
        QString serverAddress = m_settings->value("servers/address_" + serverName).toString();
        insertTableRow(m_ui->serverListWidget, { serverName, serverAddress, "", "", "Custom" });
    }
    m_server_query_manager->getPublicServers();
}

void kocity_qt::loginActionTriggered() {
    m_launcher->openLoginUrl();
    bool ok;
    QString loginCode = QInputDialog::getText(this, "Login", "Enter the six-digit code from the website that just opened.", QLineEdit::Normal, "", &ok);
    if (ok && !loginCode.isEmpty()) {
        m_launcher->login(loginCode);
    }
}

void kocity_qt::loginResponseReceived(QJsonDocument document) {
    QJsonObject obj = document.object();
    QString username = obj.value("username").toString();
    m_settings->setValue("auth/username", username);
    m_settings->setValue("auth/token", obj.value("authToken").toString());
    m_ui->statusBar->showMessage("Logged in as " + username);
}

void kocity_qt::addServer() {
    add_server_dialog dialog;
    if (dialog.exec()) {
        QString serverName = dialog.serverName();
        QStringList servers = m_settings->value("servers/servers").toStringList() << serverName;
        m_settings->setValue("servers/servers", servers);
        m_settings->setValue("servers/address_" + serverName, dialog.serverAddress());
        m_settings->setValue("servers/authentication_" + serverName, dialog.serverAuthType());
        m_settings->setValue("servers/password_" + serverName, dialog.serverPassword());

        insertTableRow(m_ui->serverListWidget, { serverName, dialog.serverAddress(), "", "", "Custom"});
    }
}

void kocity_qt::deleteServer() {
    QList<QTableWidgetItem*> selectedItems = m_ui->serverListWidget->selectedItems();
    if (selectedItems.size() == 0) {
        QMessageBox::critical(this, "Error", "Please select a server to delete.");
        return;
    }
    QString selectedServerType = selectedItems[4]->text();
    if (selectedServerType == "Default") {
        QMessageBox::critical(this, "Error", "Default servers cannot be removed from the server list.");
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Server", "Are you sure you want to delete this server?");
    if (reply == QMessageBox::Yes) {
        QString selectedServerName = selectedItems[0]->text();
        QStringList servers = m_settings->value("servers/servers").toStringList();
        servers.removeAll(selectedServerName);
        m_settings->setValue("servers/servers", servers);
        m_settings->remove("servers/address_" + selectedServerName);
        m_settings->remove("servers/authentication_" + selectedServerName);
        m_settings->remove("servers/password_" + selectedServerName);
        m_ui->serverListWidget->model()->removeRow(selectedItems[0]->row());
    }
}



#include "moc_kocity-qt.cpp"
