

#ifdef _MSC_VER
#pragma warning( push)
#pragma warning( disable : 4127 4244 4251 4311 4312 4512 4800)
#include <qt/QApplication.h>
#pragma warning( pop )
#else
#include <qapplication.h>
#endif

#include "XMLEdit.h"


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   CXMLEditDlg dlg;
   int nRet = dlg.exec();
   return nRet;
}
