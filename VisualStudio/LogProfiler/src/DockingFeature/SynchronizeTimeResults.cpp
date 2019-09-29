//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "SynchronizeTimeResults.h"
#include "..\PluginDefinition.h"

extern NppData nppData;



SynchronizeTimeResults::SynchronizeTimeResults() 
    : DockingDlgInterface(IDD_TIME_STAMP_FIND_RESULTS)
{

}

SynchronizeTimeResults::~SynchronizeTimeResults()
{
}

INT_PTR CALLBACK SynchronizeTimeResults::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    case WM_INITDIALOG: return OnInitDialog();
	case WM_COMMAND : 
	{
		switch (LOWORD(wParam))
		{
        case IDC_BTN_CLEAR: return OnBtnClear();
        case IDC_BTN_DELETE_ENTRY: return OnBtnDeleteEntry();
        case IDC_TREE_FINDRESULTS:
            default: break;
        }
		return FALSE;
    }break;
    case WM_SIZE:
    {
        SIZE fClientSize = { LOWORD(lParam), HIWORD(lParam) };
        RECT fRect;
        GetClientRect(getDlgItem(IDC_BTN_CLEAR), &fRect);
        int fSizeCX = fRect.right - fRect.left;
        MoveWindow(getDlgItem(IDC_TREE_FINDRESULTS), fSizeCX+15, 5, fClientSize.cx - fSizeCX - 20, fClientSize.cy - 10, TRUE);
        return TRUE;
    }
    case WM_NOTIFY: return OnWmNotify((UINT)wParam, (NMHDR*)lParam);
	default: return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
	}
}

INT_PTR SynchronizeTimeResults::OnInitDialog()
{
    enableToolTip();
    for (UINT nID = IDC_TREE_FINDRESULTS; nID <= IDC_LAST_IDD_TIME_STAMP_FIND_RESULTS; ++nID)
    {
        addToolTip(nID);
    }
    return TRUE;
}


INT_PTR SynchronizeTimeResults::OnWmNotify(UINT , NMHDR* pNmHdr)
{
    INT_PTR fReturn = 0;
    switch (pNmHdr->code)
    {
    case NM_DBLCLK:
        if (pNmHdr->idFrom == IDC_TREE_FINDRESULTS)
        {
            CTreeView fTree(getDlgItem(IDC_TREE_FINDRESULTS));
            HTREEITEM hSelected = fTree.getItem();
            tstring fText;
			std::wstring fFileName;
            int fImage;
            fTree.getItemText(hSelected, fText, fImage);
#ifdef UNICODE
			fFileName = fText;
#else
			convertToUnicode(fText, fFileName);
#endif // UNICODE
            switch (fImage)
            {
            case TimeEntry: break;
            case FilenameEntry:
                openPathFileName(fFileName);
                break;
            case LineNoEntry:
            {
                int fLine;
                swscanf(fFileName.c_str(), L"Line %d", &fLine);
                fTree.getItemText(fTree.getItem(TVGN_PARENT, hSelected), fText, fImage);
#ifdef UNICODE
				fFileName = fText;
#else
				convertToUnicode(fText, fFileName);
#endif // UNICODE
				openPathFileName(fFileName);
                ScintillaWnd fWnd(getScintillaWindowHandle());
                fWnd.gotoLine(fLine-1);
            }   break;
            case TimeDifference: break;
            default:
                break;
            }

        }
        break;
    }
    return fReturn;
}

INT_PTR SynchronizeTimeResults::OnBtnClear()
{
    CTreeView fTree(getDlgItem(IDC_TREE_FINDRESULTS));
    fTree.deleteItem();
    return TRUE;
}

INT_PTR SynchronizeTimeResults::OnBtnDeleteEntry()
{
    CTreeView fTree(getDlgItem(IDC_TREE_FINDRESULTS));
    HTREEITEM hSelected = fTree.getItem();
    if (hSelected)
    {
        fTree.deleteItem(hSelected);
    }
    return TRUE;
}
