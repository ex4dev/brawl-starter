#include "ui/kocity-qt.h"
#include <QApplication>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        QCoreApplication::setApplicationName("kocity-qt");
        QCoreApplication::setOrganizationDomain("ex4.dev");
        QCoreApplication::setOrganizationName("ex4");
        QApplication app(argc, argv);
        kocity_qt w;
        w.show();

        return app.exec();
    } else {
        cout << argv[1] << endl;
    }
}

