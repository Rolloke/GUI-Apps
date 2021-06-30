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
/*
app = QApplication(sys.argv)
file = QFile(":/dark.qss")
file.open(QFile.ReadOnly | QFile.Text)
stream = QTextStream(file)
app.setStyleSheet(stream.readAll())
 */
int main(int argc, char *argv[])
{
    QFileInfo fFile(argv[0]);
    QString fTitle = fFile.baseName();
    Logger fLogger(fTitle.toStdString().c_str());

    QApplication fApp(argc, argv);

    QCommandLineParser cmd_line;
    cmd_line.addOption({{"c", cmdline::Config}, QObject::tr("Alternative Config file name."), "filename"});
    cmd_line.addOption({{"log", cmdline::Log2file}, QObject::tr("Alternative Config file name."), "log2file"});

    // For debugging purpose: --lang=../../source/GitView_de.qm
    cmd_line.addOption({{"lang", cmdline::Language}, QObject::tr("Language."), "language"});
    cmd_line.parse(fApp.arguments());
    FILE* logfile = 0;


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

    if (cmd_line.value(cmdline::Log2file).toInt())
    {
        std::string filename = fTitle.toStdString();
        filename += ".log";
        logfile = fopen(filename.c_str(), "a+t");
        auto log_function = [logfile](const std::string&text){ fprintf(logfile, "%s\n", text.c_str()); };

        fLogger.setLogFunction(log_function);
    }

    fWindow.show();
    auto return_value = fApp.exec();
    if (logfile)
    {
        fclose(logfile);
    }
    return return_value;
}
