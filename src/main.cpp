#include "ui/kocity-qt.h"
#include <QApplication>
#include <QDBusMessage>
#include <QDBusConnection>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(QStringLiteral("brawl-starter"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("tswanson.dev"));
    QCoreApplication::setOrganizationName(QStringLiteral("ex4"));
    QGuiApplication::setDesktopFileName(QStringLiteral("dev.tswanson.brawl-starter"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Brawl Starter"));
    QApplication app(argc, argv);
    kocity_qt w;
    w.show();

    // Clear taskbar progress on startup
    auto message = QDBusMessage::createSignal(QStringLiteral("/dev/tswanson/BrawlStarter"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));
    QVariantMap properties;
    properties.insert(QStringLiteral("progress-visible"), false);
    properties.insert(QStringLiteral("progress"), 0);
    message << QStringLiteral("application://dev.tswanson.brawl-starter.desktop") << properties;
    QDBusConnection::sessionBus().send(message);

    return app.exec();
}

