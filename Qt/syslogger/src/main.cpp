

#include "sysloggerwidget.h"


#include <QApplication>



int main(int argc, char* argv[])
{
    QApplication fApp(argc, argv);
    
    SysLoggerWidget fSysLoggerWidget;
    fSysLoggerWidget.show();

    return fApp.exec();
}