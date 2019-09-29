#include "AboutDlg.h"


AboutDlg::AboutDlg()
{
}


AboutDlg::~AboutDlg()
{
}

INT_PTR CALLBACK AboutDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM  )
{
    if (message == WM_INITDIALOG)
    {
        //char szFileName[MAX_PATH];
        //::GetModuleFileNameA((HMODULE)_hInst, szFileName, MAX_PATH);
        //DWORD fSize = GetFileVersionInfoSizeA(szFileName, 0);
        //if (fSize)
        //{
        //    std::vector<BYTE> fData;
        //    fData.resize(fSize);
        //    GetFileVersionInfoA(szFileName, 0, fSize, &fData[0]);
        //    char fText[MAX_PATH];
        //    UINT fbSize = MAX_PATH;
        //    VerQueryValueA(&fData[0], "\\StringFileInfo\\040904b0\\FileDescription", (LPVOID*)&fText[0], &fbSize);
        //    fText[fbSize] = 0;
        //    fbSize = MAX_PATH;
        //    VerQueryValueA(&fData[0], "\\StringFileInfo\\040904b0\\ProductVersion", (LPVOID*)&fText[0], &fbSize);
        //    fText[fbSize] = 0;
        //}
        return 1;
    }
    else if (message == WM_COMMAND && (wParam == IDOK || wParam == IDCANCEL))
    {
        EndDialog(_hSelf, 0);
        return 0;
    }
    return 0;
}
