#include "ui/kocity-qt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(QStringLiteral("kocity-qt"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("tswanson.dev"));
    QCoreApplication::setOrganizationName(QStringLiteral("ex4"));
    QApplication app(argc, argv);
    kocity_qt w;
    w.show();

    return app.exec();
}

