#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    int font_size = 0;
    QString font_name;
    for (int n = 1; n < argc; ++n)
    {
        if (strcmp(argv[n], "--font_size") == 0 && n < (argc - 1))
        {
            font_size = atoi(argv[n+1]);
        }
        else if (strcmp(argv[n], "--font_name") == 0 && n < (argc - 1))
        {
            font_name = QString(argv[n+1]);
        }
    }
    if (font_size)
    {
        if (font_name.size() == 0)
        {
            font_name = "Ubuntu Regular";
        }
        QFont font(font_name);
        font.setPointSizeF(font_size);
        QApplication::setFont(font);
    }
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
