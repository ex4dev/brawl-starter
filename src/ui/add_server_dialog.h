#ifndef ADD_SERVER_DIALOG_H
#define ADD_SERVER_DIALOG_H

#include <QLabel>
#include <QScopedPointer>
#include <QDialog>
#include "ui_add_server_dialog.h"

namespace Ui
{
class add_server_dialog;
}

class add_server_dialog : public QDialog
{
    Q_OBJECT
    public:
        explicit add_server_dialog(QWidget *parent = 0);
        QString serverName();
        QString serverAddress();
        int serverAuthType();
        QString serverPassword();

    private slots:
        void authenticationTypeChanged(int index);
    private:
        QScopedPointer<Ui::add_server_dialog> m_ui;
};

#endif // ADD_SERVER_DIALOG_H
