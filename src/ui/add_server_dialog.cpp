#include "add_server_dialog.h"
#include "ui_add_server_dialog.h"
#include <QComboBox>

add_server_dialog::add_server_dialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::add_server_dialog)
{
    m_ui->setupUi(this);
    connect(m_ui->authenticationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &add_server_dialog::authenticationTypeChanged);
    m_ui->passwordLabel->hide();
    m_ui->passwordLineEdit->hide();
}

QString add_server_dialog::serverName() {
    return m_ui->serverNameLineEdit->text();
}

QString add_server_dialog::serverAddress() {
    return m_ui->serverAddressLineEdit->text();
}

int add_server_dialog::serverAuthType() {
    return m_ui->authenticationComboBox->currentIndex();
}

QString add_server_dialog::serverPassword() {
    return m_ui->passwordLineEdit->text();
}

void add_server_dialog::authenticationTypeChanged(int index) {
    if (index == 2) {
        m_ui->passwordLabel->show();
        m_ui->passwordLineEdit->show();
    } else {
        m_ui->passwordLabel->hide();
        m_ui->passwordLineEdit->hide();
    }
}
