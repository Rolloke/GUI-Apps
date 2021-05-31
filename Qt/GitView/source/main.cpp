#include "mainwindow.h"
#include <QApplication>
#include "logger.h"
#include <QCommandLineParser>
#include <QTranslator>

namespace cmdline
{
    const QString Config      = "config";
    const QString Log2file    = "log2file";
    const QString Language    = "language";
}

int main(int argc, char *argv[])
{
    QFileInfo fFile(argv[0]);
    QString fTitle = fFile.baseName();
    Logger fLogger(fTitle.toStdString().c_str());

    QApplication fApp(argc, argv);

    QCommandLineParser cmd_line;
    cmd_line.addOption({{"c", cmdline::Config}, QObject::tr("Alternative Config file name."), "filename"});
//    cmd_line.addOption({{"l", cmdline::Log2file}, QObject::tr("Alternative Config file name."), "log2file"});

    // For debugging purpose: --lang=../../source/GitView_de.qm
    cmd_line.addOption({{"lang", cmdline::Language}, QObject::tr("Alternative Config file name."), "language"});
    cmd_line.parse(fApp.arguments());

//    if (cmd_line.value(cmdline::Log2file).toInt())
//    {
//        fLogger.openLogFile(fTitle.toStdString());
//    }

    QTranslator translator;
    bool loaded = translator.load(QLocale(), QLatin1String(fTitle.toStdString().c_str()), QLatin1String("_"), QLatin1String(":/i18n"));
    if (cmd_line.value(cmdline::Language).size())
    {
        loaded = translator.load(cmd_line.value(cmdline::Language));
    }
    if (loaded)
    {
        fApp.installTranslator(&translator);
    }

    MainWindow fWindow(cmd_line.value(cmdline::Config));
    fWindow.show();
    return fApp.exec();
}
