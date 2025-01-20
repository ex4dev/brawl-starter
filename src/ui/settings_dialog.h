#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H


#include <qnetworkaccessmanager.h>
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
        explicit settings_dialog(QWidget *parent, QSettings *settings);

    private slots:
        void saveSettings();
        void startLoginProcess();
        void createAccount();
        void openSteamStorePage();
        void openSteamProperties();

    private:
        void login(const QString &loginCode);
        void finishLogin(const QByteArray &responseData);
        QString getDiscordCode();
        void logout();
        void updateLoginSection();
        QScopedPointer<Ui::settings_dialog> m_ui;
        QSettings *m_settings;
        QScopedPointer<QNetworkAccessManager> m_network_access_manager;
};

#endif // SETTINGS_DIALOG_H
