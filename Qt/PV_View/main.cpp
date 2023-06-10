#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    for ( int i = 0; i < argc; i++ )
    {
        QString s = argv[i] ;
        if ( s.startsWith( "-non-gui" ) )
        {
            w.set_no_gui();
        }
    }

    if (w.display_gui())
    {
        w.show();
    }
    return a.exec();
}
