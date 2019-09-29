#pragma once
#include ".\DockingFeature\StaticDialog.h"
class AboutDlg :
    public StaticDialog
{
public:
    AboutDlg();
    ~AboutDlg();
private:
    INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

