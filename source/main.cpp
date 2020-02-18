#include "mainwindow.h"
#include <QApplication>
#include "logger.h"
#include <QCommandLineParser>

namespace cmdline
{
    const QString Config      = "config";
}

int main(int argc, char *argv[])
{
    QFileInfo fFile(argv[0]);
    QString fTitle = fFile.baseName();
    Logger fLogger(fTitle.toStdString().c_str());
    fLogger.openLogFile(fTitle.toStdString());

    QApplication fApp(argc, argv);
    QCommandLineParser fCmdLine;
    fCmdLine.addOption({{"c", cmdline::Config}, "Alternative Config file name.", "filename"});
    fCmdLine.parse(fApp.arguments());

    MainWindow fWindow(fCmdLine.value(cmdline::Config));
    fWindow.show();
    return fApp.exec();
}
