#include "kocity-qt.h"
#include "ui_kocity-qt.h"
#include <iostream>
#include <qdialog.h>
#include <QMessageBox>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QInputDialog>

using std::cout;
using std::endl;

kocity_qt::kocity_qt(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::kocity_qt),
    m_settings(new QSettings())
{

    m_ui->setupUi(this);

    connect(m_ui->actionPlay, &QAction::triggered, this, &kocity_qt::launchGame);
    connect(m_ui->actionSettings, &QAction::triggered, this, &kocity_qt::openSettings);

    m_ui->progressBar->hide();

    m_installer = new installer("/home/tyler/.kocityqt");

    connect(m_installer, &installer::installationStarted, this, &kocity_qt::gameInstallationStarted);
    connect(m_installer, &installer::progressUpdated, this, &kocity_qt::gameDownloadProgressUpdated);
    connect(m_installer, &installer::downloadFinished, this, &kocity_qt::gameDownloadFinished);
    connect(m_installer, &installer::finished, this, &kocity_qt::gameInstallationFinished);

    m_server_query_manager = new server_query();
    connect(m_server_query_manager, &server_query::publicServersReceived, this, &kocity_qt::publicServersReceived);

    cout << "Hello" << endl;

    m_launcher = new launcher("/home/tyler/.kocityqt");
    connect(m_launcher, &launcher::loginResponseReceived, this, &kocity_qt::loginResponseReceived);

    connect(m_ui->actionLogin, &QAction::triggered, this, &kocity_qt::loginActionTriggered);

    m_server_query_manager->getPublicServers();
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
        QMessageBox dialog;
        dialog.setText("Please select a server.");
        dialog.setWindowTitle("Error");
        dialog.exec();
        return;
    }
    if (!m_installer->checkInstalled()) {
        m_installer->startGameInstallation();
        return;
    }
    QString username = m_settings->value("auth/username").toString();
    QString token = m_settings->value("auth/token").toString();
    if (username.isEmpty() || token.isEmpty()) {
        QMessageBox dialog;
        dialog.setText("You must log in to join this server.");
        dialog.setWindowTitle("Error");
        dialog.exec();
        return;
    }
    QString selectedAddress = m_ui->serverListWidget->selectedItems()[1]->text();
    cout << "Launching " << selectedAddress.toStdString() << endl;
    m_launcher->getKeyAndLaunch(username, token, selectedAddress);

}

void kocity_qt::openSettings() {
    cout << "Settings" << endl;

}

void kocity_qt::gameInstallationStarted() {
    m_ui->statusLabel->setText("Downloading files...");
    m_ui->progressBar->setValue(0);
    m_ui->progressBar->show();
}

void kocity_qt::gameDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal) {
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/ex4/KoCityQt"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    //you don't always have to specify all parameters, just the ones you want to update
    double percentComplete = (double) bytesReceived / bytesTotal;
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), true);// enable the progress
    properties.insert(QStringLiteral("progress"), (percentComplete));// set the progress value (from 0.0 to 1.0)
    properties.insert(QStringLiteral("count-visible"), false);// display the "counter badge"
    properties.insert(QStringLiteral("count"), 0);// set the counter value

    message << QStringLiteral("application://kocityqt.desktop")
            << properties;
    QDBusConnection::sessionBus().send(message);

    m_ui->progressBar->setValue(100 * percentComplete);
}

void kocity_qt::gameDownloadFinished() {
    m_ui->progressBar->hide();
    m_ui->statusLabel->setText("Extracting downloaded files...");
    cout << "[UI] Download finished" << endl;
}

void kocity_qt::gameInstallationFinished()
{
    m_ui->statusLabel->setText("Installation complete.");
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
    m_ui->serverListWidget->clearContents();
    m_ui->serverListWidget->setRowCount(0);
    insertTableRow(m_ui->serverListWidget, {"Local Server", "localhost:23600"});
    for (QJsonValueRef element : document.array()) {
        QJsonObject obj = element.toObject();
        QString status = obj.find("status").value().toString("Unknown");
        QString name = obj.find("name").value().toString("Unnamed");
        QString ip = obj.find("ip").value().toString("Unknown");
        int players = obj.find("players").value().toInt(-1);
        int maxPlayers = obj.find("maxPlayers").value().toInt(-1);
        insertTableRow(m_ui->serverListWidget, { name, ip, QString::number(players) + " / " + QString::number(maxPlayers), status});
    }
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
    m_ui->statusLabel->setText("Logged in as " + username);
}



#include "moc_kocity-qt.cpp"
