// LogDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "LogDialogBar.h"
#include "MainFrm.h"


// CLogDialogBar

IMPLEMENT_DYNAMIC(CLogDialogBar, CDialogBar)
CLogDialogBar::CLogDialogBar()
{
}

CLogDialogBar::~CLogDialogBar()
{
}


BEGIN_MESSAGE_MAP(CLogDialogBar, CDialogBar)
	//{{AFX_MSG_MAP(CLogDialogBar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_COMMAND(ID_EDIT_SEARCH_TIME_VALUE, OnEditSearchTimeValue)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CLogDialogBar message handlers

LRESULT CLogDialogBar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CDialogBar::HandleInitDialog(wParam, lParam);
	EnableToolTips();
	return lResult;
}
void CLogDialogBar::OnEditSearchTimeValue()
{
	CWnd *pWnd = theApp.GetMainWnd();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
	{
		((CMainFrame*)pWnd)->GetActiveFrame()->SendMessage(WM_COMMAND, ID_EDIT_SEARCH_TIME_VALUE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
BOOL CLogDialogBar::OnToolTipNotify(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		HWND hwnd = NULL;
		memcpy (&hwnd, &nID, sizeof(UINT));
		nID = ::GetDlgCtrlID(hwnd);
	}
	int nTW = ::SendMessage(pNMHDR->hwndFrom, TTM_GETMAXTIPWIDTH, 0, 0);  
	if (nTW == -1)
	{
		::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 250);  
	}

	if (nID != 0)
	{
		if (strTipText.LoadString(nID))
		{
			int nFind = strTipText.Find(_T("\n"));
			pTTT->lpszText = NULL;
			if (nFind != -1)
			{
				m_strTipText = strTipText.Mid(nFind+1);
				((CMainFrame*)theApp.GetMainWnd())->GetStatusBar()->SetPaneText(0, strTipText.Left(nFind));
			}
			else
			{
				m_strTipText = strTipText;
			}
			pTTT->lpszText = (LPTSTR)LPCTSTR(m_strTipText);
		}
		else
		{
			pTTT->lpszText = NULL;
			m_strTipText.Empty();
			return FALSE;
		}
	}
	*pResult = 0;

	return TRUE;    // message was handled
}
