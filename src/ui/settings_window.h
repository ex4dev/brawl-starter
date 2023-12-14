#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui
{
class settings_window;
}

class settings_window : public QWidget
{
    Q_OBJECT

private:
    QScopedPointer<Ui::settings_window> m_ui;
};

#endif // SETTINGS_WINDOW_H
