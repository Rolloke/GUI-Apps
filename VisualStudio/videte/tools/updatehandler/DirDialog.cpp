// DirDialog.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "DirDialog.h"

#include <Dlgs.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Dialog items
#define IDC_TXT_SEARCHIN	0x0443 // 1091 : (Static) Search in
#define IDC_COMBO_SEARCHIN	0x0471 // 1137 : (ComboBox) Search in
#define IDC_TXT_TOOLBAR		0x0440 // 1188 : (Static) Toolbar
#define IDC_WND_LIST_PARENT 0x0461 // 1121 : (SHELLDLL_DefView) Parent of (SysListView32) ListView
#define IDC_TXT_FILENAME	0x0442 // 1090 : (Static) FileName
#define IDC_EDT_FILENAME	0x0480 // 1152 : (Edit) Filename
#define IDC_COMBO_FILENAME	0x047c // 1148 : (ComboBox) Filename
#define IDC_TXT_FILETYPE	0x0441 // 1089 : (Static) FileType
#define IDC_COMBO_FILETYPE	0x0470 // 1136 : (ComboBox) FileType
// IDOK
// IDCANCEL
#define IDC_BTN_HELP		0x040e // 1038 : (Button) Help

/////////////////////////////////////////////////////////////////////////////
// CDirDialog

IMPLEMENT_DYNAMIC(CDirDialog, CFileDialog)

CWnd CDirDialog::gm_wndDummy;

CDirDialog::CDirDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CDirDialog, CFileDialog)
	//{{AFX_MSG_MAP(CDirDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDirDialog::OnInitDone()
{
	CWnd &comboFileType = GetFDDlgItem(IDC_COMBO_FILETYPE);
	CWnd &wndFileName   = GetFDDlgItem(IDC_TXT_FILENAME);
	CWnd &edtFileName	= GetFDDlgItem(IDC_EDT_FILENAME);
	CWnd &btnOK			= GetFDDlgItem(IDOK);
	CWnd &btnCancel		= GetFDDlgItem(IDCANCEL);
	GetParent()->SetWindowText(_T("Quelle wählen"));

	GetFDDlgItem(IDC_TXT_FILETYPE).ShowWindow(SW_HIDE);
	comboFileType.ShowWindow(SW_HIDE);

	if (edtFileName.m_hWnd)						// alter dialog
	{
		edtFileName.ShowWindow(SW_HIDE);
	}
	else										// neuer Dialog
	{
		GetFDDlgItem(IDC_COMBO_FILENAME).ShowWindow(SW_HIDE);
	}
	btnOK.SetWindowText(_T("&Wählen"));

	CRect rcFN, rcOK, rcCancel;
	wndFileName.GetWindowRect(rcFN);
	btnOK.GetWindowRect(rcOK);
	btnCancel.GetWindowRect(rcCancel);
	CSize szFileNameName = rcOK.BottomRight() - rcFN.TopLeft();
	wndFileName.ModifyStyle(0, SS_PATHELLIPSIS);
	wndFileName.SetWindowPos(NULL,0,0,szFileNameName.cx, szFileNameName.cy, SWP_NOMOVE|SWP_NOZORDER);
	szFileNameName.cx = -MulDiv(rcOK.Width(), 11, 10);
	szFileNameName.cy  = rcCancel.top - rcOK.top;
	rcOK.OffsetRect(szFileNameName);
	GetParent()->ScreenToClient(rcOK);
	btnOK.MoveWindow(rcOK);

	CString sTemp = GetPathName();
	int p = sTemp.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sTemp = sTemp.Left(p);
	}
	else
	{
		sTemp = m_sDir;
	}
	wndFileName.SetWindowText(sTemp);
}
void CDirDialog::OnFolderChange()
{
	GetParent()->GetDlgItem(IDOK)->SetWindowText(_T("&Wählen"));
	CString sTemp = GetPathName();
	int p = sTemp.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sTemp = sTemp.Left(p);
	}
	else
	{
		sTemp = m_sDir;
	}
	GetFDDlgItem(IDC_TXT_FILENAME).SetWindowText(sTemp);
}
CWnd& CDirDialog::GetFDDlgItem(UINT nID)
{
	CWnd *pParent = GetParent();
	if (pParent)
	{
		CWnd *pWnd = pParent->GetDlgItem(nID);
		if (pWnd)
		{
			return *pWnd;
		}
	}
	return gm_wndDummy;
}
