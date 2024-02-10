#include "ui/kocity-qt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("kocity-qt");
    QCoreApplication::setOrganizationDomain("ex4.dev");
    QCoreApplication::setOrganizationName("ex4");
    QApplication app(argc, argv);
    kocity_qt w;
    w.show();

    return app.exec();
}

