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
    QFileInfo file_info(argv[0]);
    std::string program_title = file_info.baseName().toStdString();
    Logger fLogger(program_title.c_str());

    QApplication app(argc, argv);
    QApplication::setApplicationName("GitView");
    QApplication::setApplicationVersion("1.2.0.2");

    QCommandLineParser cmd_line;
    cmd_line.setApplicationDescription("Git repository viewer");
    cmd_line.addOption({{"c", cmdline::Config}, QObject::tr("Alternative Config file name."), "filename"});
    cmd_line.addOption({{"log", cmdline::Log2file}, QObject::tr("Log file name for debugging."), "log2file"});
    cmd_line.addHelpOption();
    cmd_line.addVersionOption();

    // For debugging purpose: --lang=../../source/GitView_de.qm
    cmd_line.addOption({{"lang", cmdline::Language}, QObject::tr("Language."), "language"});
    cmd_line.parse(app.arguments());
    cmd_line.process(app);

    FILE* logfile = 0;

    QTranslator translator;
    bool loaded = translator.load(QLocale(), QLatin1String(program_title.c_str()), QLatin1String("_"), QLatin1String(":/i18n"));
    if (cmd_line.value(cmdline::Language).size())
    {
        loaded = translator.load(cmd_line.value(cmdline::Language));
    }
    if (loaded)
    {
        app.installTranslator(&translator);
    }

    MainWindow window(cmd_line.value(cmdline::Config));
    window.set_app_path(argv[0]);

    if (cmd_line.value(cmdline::Log2file).toInt())
    {
        std::string filename = program_title +  ".log";
        logfile = fopen(filename.c_str(), "a+t");
        auto log_function = [logfile](const std::string&text){ fprintf(logfile, "%s\n", text.c_str()); };

        fLogger.setLogFunction(log_function);
    }

    window.show();
    auto return_value = app.exec();
    if (logfile)
    {
        fclose(logfile);
    }
    return return_value;
}
