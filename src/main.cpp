#include "ui/kocity-qt.h"
#include <QApplication>

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

    return app.exec();
}

