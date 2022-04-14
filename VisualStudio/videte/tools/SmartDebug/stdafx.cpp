// stdafx.cpp : source file that includes just the standard includes
// AddIn.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


#ifdef _TEST_APP
#include "copycsvdlg.h"


extern CAddInModule _AtlModule;

int _tmain(int argc, _TCHAR* argv[])
{
   srand(4);
   _AtlModule.SetResourceInstance(GetModuleHandle(argv[0]));

   CCopyCSVDlg dlg;
   dlg.DoModal(NULL);
   //dlg.Create(NULL);

	return 0;
}
#endif // _TEST_APP