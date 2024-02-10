#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H


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
        explicit settings_dialog(QSettings *settings);

    signals:
        void onClickLogIn();
        void onClickLogOut();
    private slots:
        void browseInstallLocation();
        void saveSettings();

private:
        QScopedPointer<Ui::settings_dialog> m_ui;
        QSettings *m_settings;

};

#endif // SETTINGS_DIALOG_H
