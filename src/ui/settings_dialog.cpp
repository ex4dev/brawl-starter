#include "settings_dialog.h"

settings_dialog::settings_dialog(QWidget *parent) : m_ui(new Ui::settings_dialog()), m_settings(new QSettings()) {
    m_ui->setupUi(this);
    connect(m_ui->btnLogIn, &QAbstractButton::clicked, this, &settings_dialog::onClickLogIn);
}
