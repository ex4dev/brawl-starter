#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QLabel>
#include <QScopedPointer>
#include <QSettings>
#include "ui_settings_dialog.h"

namespace Ui
{
class settings_dialog;
}

class settings_dialog : public QDialog
{
    Q_OBJECT
    public:
        explicit settings_dialog(QWidget *parent = 0);

    signals:
        void onClickLogIn();
        void onClickLogOut();
    private slots:
        void browseInstallLocation();

    private:
        QScopedPointer<Ui::settings_dialog> m_ui;
        QScopedPointer<QSettings> m_settings;

};

#endif // SETTINGS_DIALOG_H
