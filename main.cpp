#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;

    QString program_root = argv[0];
    int pos = program_root.lastIndexOf('/');
    program_root = program_root.left(pos);
#if 1
    QString translation_file = program_root + "/RaspiMediaList";
    bool loaded = translator.load(QLocale(), translation_file, QLatin1String("_"), QLatin1String(":/i18n"));
#else
    QString file = root + "/RaspiMediaList_de.qm";
    bool loaded = translator.load(file);
#endif
    if (loaded)
    {
        a.installTranslator(&translator);
    }

    QSharedPointer<MainWindow> window(new MainWindow());

    window->show();

    return a.exec();
}
