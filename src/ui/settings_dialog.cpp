#include "settings_dialog.h"
#include <QFileDialog>
#include <QDir>

settings_dialog::settings_dialog(QWidget *parent) : m_ui(new Ui::settings_dialog()), m_settings(new QSettings()) {
    m_ui->setupUi(this);
    // TODO move the login management code to this file?
    connect(m_ui->btnLogIn, &QAbstractButton::clicked, this, &settings_dialog::onClickLogIn);
    connect(m_ui->btnSignOut, &QAbstractButton::clicked, this, &settings_dialog::onClickLogOut);
    connect(m_ui->btnBrowseInstallation, &QAbstractButton::clicked, this, &settings_dialog::browseInstallLocation);

    m_ui->fldInstallLocation->setText(m_settings->value("directory", QDir::homePath() + QDir::separator() + ".kocityqt").toString());
    QString username = m_settings->value("auth/username").toString();
    m_ui->lblLoggedIn->setText(username.isEmpty() ? "Not logged in" : "Logged in as " + username);
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
