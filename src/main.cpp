#include "ui/kocity-qt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(QStringLiteral("Brawl Starter"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("brawl-starter.tswanson.dev"));
    QCoreApplication::setOrganizationName(QStringLiteral("ex4"));
    QApplication app(argc, argv);
    kocity_qt w;
    w.show();

    return app.exec();
}

