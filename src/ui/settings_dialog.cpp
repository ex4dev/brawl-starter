#include "settings_dialog.h"
#include "src/constants.h"
#include <QFileDialog>

settings_dialog::settings_dialog(QSettings *settings) : m_ui(new Ui::settings_dialog()), m_settings(settings) {
    m_ui->setupUi(this);
    // TODO move the login management code to this file?
    connect(m_ui->btnLogIn, &QAbstractButton::clicked, this, &settings_dialog::onClickLogIn);
    connect(m_ui->btnSignOut, &QAbstractButton::clicked, this, &settings_dialog::onClickLogOut);
    connect(m_ui->btnBrowseInstallation, &QAbstractButton::clicked, this, &settings_dialog::browseInstallLocation);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &settings_dialog::saveSettings);

    m_ui->fldInstallLocation->setText(m_settings->value(constants::SETTING_PATH_INSTALL_DIR, constants::SETTING_DEFAULT_INSTALL_DIR).toString());
    QString username = m_settings->value(constants::SETTING_PATH_USERNAME).toString();
    m_ui->lblLoggedIn->setText(username.isEmpty() ? QStringLiteral("Not logged in") : QStringLiteral("Logged in as ") + username);
    if (username.isEmpty()) m_ui->btnSignOut->hide();
    else {
        m_ui->btnLogIn->hide();
        m_ui->btnRegister->hide();
    }
}

void settings_dialog::browseInstallLocation() {
    QFileDialog dialog;
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (dialog.exec()) {
        QString newDir = dialog.selectedFiles()[0];
        m_ui->fldInstallLocation->setText(newDir);
    }
}

void settings_dialog::saveSettings() {
    m_settings->setValue(constants::SETTING_PATH_INSTALL_DIR, m_ui->fldInstallLocation->text());
}