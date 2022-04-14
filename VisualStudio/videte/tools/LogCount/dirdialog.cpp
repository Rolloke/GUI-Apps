// DirDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirDialog
IMPLEMENT_DYNAMIC(CDirDialog, CFileDialog)
/////////////////////////////////////////////////////////////////////////////
CDirDialog::CDirDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDirDialog, CFileDialog)
	//{{AFX_MSG_MAP(CDirDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CDirDialog::OnInitDone()
{
	CString sText;
	sText.LoadString(IDS_CHOSE_DIR);
	GetParent()->SetWindowText(sText);
//	GetParent()->GetDlgItem(1090)->ShowWindow(SW_HIDE);
	GetParent()->GetDlgItem(1089)->ShowWindow(SW_HIDE);
	GetParent()->GetDlgItem(1136)->ShowWindow(SW_HIDE);
	GetParent()->GetDlgItem(1152)->ShowWindow(SW_HIDE);
	sText.LoadString(IDS_CHOSE);
	GetParent()->GetDlgItem(IDOK)->SetWindowText(sText);

	CRect rect;
	GetParent()->GetDlgItem(1090)->GetClientRect(rect);
	GetParent()->GetDlgItem(1090)->SetWindowPos(NULL,0,0,300,rect.Height(),SWP_NOMOVE|SWP_NOZORDER);
	

	CString sTemp = GetPathName();
	int p = sTemp.ReverseFind('\\');
	if (p!=-1)
	{
		sTemp = sTemp.Left(p);
	}
	else
	{
		sTemp = m_sDir;
	}
	GetParent()->GetDlgItem(1090)->SetWindowText(sTemp);
}
/////////////////////////////////////////////////////////////////////////////
void CDirDialog::OnFolderChange()
{
	CString sTemp;
	sTemp.LoadString(IDS_CHOSE);
	GetParent()->GetDlgItem(IDOK)->SetWindowText(sTemp);
	sTemp = GetPathName();
	int p = sTemp.ReverseFind('\\');
	if (p!=-1)
	{
		sTemp = sTemp.Left(p);
	}
	else
	{
		sTemp = m_sDir;
	}
	GetParent()->GetDlgItem(1090)->SetWindowText(sTemp);
}
