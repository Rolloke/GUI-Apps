#include "mainwindow.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QApplication>
#else
    #include <QtGui/QApplication>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("ELAC Nautik");
    app.setOrganizationDomain("ELAC.com");
    app.setApplicationName("SerialCommunicator");

    MainWindow w;
    w.show();

    return app.exec();
}
