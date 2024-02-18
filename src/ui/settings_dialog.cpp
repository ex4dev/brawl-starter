#include "settings_dialog.h"

#include <QDesktopServices>

#include "src/constants.h"
#include "src/secrets.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>

settings_dialog::settings_dialog(QSettings *settings) :
    m_ui(new Ui::settings_dialog()),
    m_settings(settings),
    m_network_access_manager(new QNetworkAccessManager(this)) {

    m_ui->setupUi(this);
    connect(m_ui->btnLogIn, &QAbstractButton::clicked, this, &settings_dialog::startLoginProcess);
    connect(m_ui->btnRegister, &QAbstractButton::clicked, this, &settings_dialog::createAccount);
    connect(m_ui->btnSignOut, &QAbstractButton::clicked, this, &settings_dialog::logout);
    connect(m_ui->btnBrowseInstallation, &QAbstractButton::clicked, this, &settings_dialog::browseInstallLocation);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &settings_dialog::saveSettings);

    m_ui->fldInstallLocation->setText(m_settings->value(constants::SETTING_PATH_INSTALL_DIR, constants::SETTING_DEFAULT_INSTALL_DIR).toString());
    m_ui->fldUsername->setText(m_settings->value(constants::SETTING_PATH_OFFLINE_USERNAME, constants::SETTING_DEFAULT_OFFLINE_USERNAME).toString());

    updateLoginSection();

}

void settings_dialog::browseInstallLocation() {
    QFileDialog dialog;
    dialog.setDirectory(m_ui->fldInstallLocation->text());
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (dialog.exec()) {
        QString newDir = dialog.selectedFiles()[0];
        m_ui->fldInstallLocation->setText(newDir);
    }
}

void settings_dialog::saveSettings() {
    m_settings->setValue(constants::SETTING_PATH_INSTALL_DIR, m_ui->fldInstallLocation->text());
    m_settings->setValue(constants::SETTING_PATH_OFFLINE_USERNAME, m_ui->fldUsername->text());
}

void settings_dialog::startLoginProcess() {
    QString code = getDiscordCode();
    if (code != nullptr) login(code);
}

QString settings_dialog::getDiscordCode() {
    QDesktopServices::openUrl(constants::XYZ_LOGIN_PAGE_URL);
    bool ok;
    QString loginCode = QInputDialog::getText(this, QStringLiteral("Login"), QStringLiteral("Enter the six-digit code from the website that just opened."), QLineEdit::Normal, "", &ok, Qt::WindowStaysOnTopHint);
    if (ok && !loginCode.isEmpty()) {
        return loginCode;
    }
    return nullptr;
}

void settings_dialog::login(const QString &loginCode) {
    QNetworkRequest request(constants::XYZ_LOGIN_REQUEST_URL);
    QString body = "{\"code\": \"" + loginCode + "\"}";
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_network_access_manager->post(request, body.toUtf8());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        finishLogin(reply->readAll());
        reply->deleteLater();
    });
}

void settings_dialog::finishLogin(const QByteArray &responseData) {
    QJsonDocument document = QJsonDocument::fromJson(responseData);
    QJsonObject obj = document.object();

    QJsonValue message = obj.value("message");
    if (!message.isUndefined()) { // there was a server error during the login process
        QString type = obj.value("type").toString();
        if (type == QStringLiteral("account_exists")) {
            QMessageBox::critical(this, constants::STR_ERROR, QStringLiteral("A kocity.xyz account is already associated with this Discord account. Please log in instead."));
            return;
        }
        QMessageBox::critical(this, obj.value("type").toString(), message.toString());
        return;
    }
    QString username = obj.value("username").toString();
    m_settings->setValue(constants::SETTING_PATH_USERNAME, username);
    storeToken(username.toLocal8Bit().data(), obj.value("authToken").toString().toLocal8Bit().data());

    updateLoginSection();
}

void settings_dialog::logout() {
    QString username = m_settings->value(constants::SETTING_PATH_USERNAME).toString();
    if (username.isEmpty()) return;
    deleteToken(username.toLocal8Bit().data());
    m_settings->remove(constants::SETTING_PATH_USERNAME);
    updateLoginSection();
}

void settings_dialog::updateLoginSection() {
    QString username = m_settings->value(constants::SETTING_PATH_USERNAME).toString();
    m_ui->lblLoggedIn->setText(username.isEmpty() ? QStringLiteral("Not logged in") : QStringLiteral("Logged in as ") + username);
    if (username.isEmpty()) {
        m_ui->btnSignOut->hide();
        m_ui->btnLogIn->show();
        m_ui->btnRegister->show();
    } else {
        m_ui->btnSignOut->show();
        m_ui->btnLogIn->hide();
        m_ui->btnRegister->hide();
    }
}

void settings_dialog::createAccount() {
    QString loginCode = getDiscordCode();
    if (loginCode == nullptr) return;
    bool ok;
    QString username = QInputDialog::getText(
        this,
        QStringLiteral("Set Username"),
        QStringLiteral("Enter the username for your new account. Once set, this cannot be changed. This name will be visible to other players on authenticated servers."),
        QLineEdit::Normal,
        m_settings->value(constants::SETTING_PATH_OFFLINE_USERNAME, constants::SETTING_DEFAULT_OFFLINE_USERNAME).toString(),
        &ok,
        Qt::WindowStaysOnTopHint
    );
    if (ok && !username.isEmpty()) {
        // TODO remove quotes and other disallowed characters from username
        QNetworkRequest request(constants::XYZ_REGISTER_REQUEST_URL);
        QString body = "{\"username\": \"" + username + "\", \"code\": \"" + loginCode + "\"}";
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply *reply = m_network_access_manager->post(request, body.toUtf8());
        connect(reply, &QNetworkReply::finished, this, [=]() {
            finishLogin(reply->readAll());
            reply->deleteLater();
        });
    }
}