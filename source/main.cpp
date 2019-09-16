#include "mainwindow.h"
#include "main.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QApplication>
#else
    #include <QtGui/QApplication>
#endif

#include <stdio.h>
#include <stdarg.h>

int main(int argc, char *argv[])
{
    int fReturn = 0;
    bool fRestart = false;
    do
    {
        QApplication fApp(argc, argv);
        fApp.setOrganizationName("KESoft");
        MainWindow fWindow(fApp);
        fApp.setApplicationName(fWindow.windowTitle());
        fWindow.show();

        fReturn = fApp.exec();
        fRestart = fWindow.doRestart();
    }
    while (fRestart);

    return fReturn;
}


Logger gTrace("Oscilloscope");

void setWidgetStyleSheet(QWidget& aWidget, const QColor* aColor, const QColor* aBackgroundColor)
{
    QString fStyleSheet = aWidget.metaObject()->className();
    fStyleSheet += " {";
    if (aColor)
    {
        fStyleSheet += "color: ";
        fStyleSheet += aColor->name();
        fStyleSheet += ";";
    }
    if (aBackgroundColor)
    {
        fStyleSheet += " background-color: ";
        fStyleSheet += aBackgroundColor->name();
        fStyleSheet += ";";
    }
    fStyleSheet += "}";

    aWidget.setStyleSheet(fStyleSheet);
}

void setWidgetColor(QWidget& aWidget, QPalette::ColorRole aRole, const QColor& aColor)
{
    QBrush fBrush(aColor);
    QPalette fPalette = aWidget.palette();
    fPalette.setBrush(aRole, fBrush);
    aWidget.setPalette(fPalette);
}
