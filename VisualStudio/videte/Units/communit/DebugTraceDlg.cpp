// DebugTraceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommUnit.h"
#include "DebugTraceDlg.h"
#include ".\debugtracedlg.h"


/////////////////////////////////////////////////////////////////////////////
// CDebugTraceDlg dialog
IMPLEMENT_DYNAMIC(CDebugTraceDlg, CWK_Dialog)
/////////////////////////////////////////////////////////////////////////////
CDebugTraceDlg::CDebugTraceDlg(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CDebugTraceDlg::IDD, pParent)
{
}
/////////////////////////////////////////////////////////////////////////////
CDebugTraceDlg::~CDebugTraceDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDebugTraceDlg, CWK_Dialog)
	ON_BN_CLICKED(IDC_BTN_CLEAR_OUTPUT, OnBnClickedBtnClearOutput)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CK_TOPMOST, OnBnClickedCkTopmost)
	ON_BN_CLICKED(IDC_CK_SETTINGS, OnBnClickedCkSettings)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBnClickedBtnReset)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDebugTraceDlg message handlers
void CDebugTraceDlg::OnBnClickedBtnClearOutput()
{
	SendDlgItemMessage(IDC_EDT_DEBUG_OUT, WM_SETTEXT, 0, (LPARAM)_T(""));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDebugTraceDlg::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

	SetDebuggerWindowHandle(::GetDlgItem(m_hWnd, IDC_EDT_DEBUG_OUT));
	ShowWindow(SW_SHOW);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::OnDestroy()
{
	SetDebuggerWindowHandle(NULL);
	CWK_Dialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::OnBnClickedCkTopmost()
{
	BOOL bTopMost;
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_TOPMOST, bTopMost);
	SetWindowPos(bTopMost ? &CWnd::wndTopMost : &CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::OnBnClickedCkSettings()
{
	BOOL bSettings, bTemp;
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_SETTINGS, bSettings);
	int nCmdShowSettings;
	if (bSettings)
	{
		GetDlgItem(IDC_EDT_DEBUG_OUT)->ShowWindow(SW_HIDE);
		nCmdShowSettings = SW_SHOW;

		DDX_Check(&dx, IDC_CK_TRACE_COMMAND, theApp.m_bTraceCommand);
		DDX_Check(&dx, IDC_CK_TRACE_RS232_EVENTS, theApp.m_bTraceRS232Events);

		bTemp = (theApp.m_bTraceData) ? TRUE : FALSE;
		DDX_Check(&dx, IDC_CK_TRACE_DATA, bTemp);
		if (theApp.m_bTraceData)
		{
			bTemp = (theApp.m_bTraceData & 0x0008) ? FALSE : TRUE;
			DDX_Check(&dx, IDC_CK_HEX_DATA, bTemp);
			bTemp = (theApp.m_bTraceData & 0x0004) ? TRUE : FALSE;
			DDX_Check(&dx, IDC_CK_ASCCII_DATA, bTemp);
		}
	}
	else
	{
		GetDlgItem(IDC_EDT_DEBUG_OUT)->ShowWindow(SW_SHOW);
		nCmdShowSettings = SW_HIDE;
		DDX_Check(&dx, IDC_CK_TRACE_COMMAND, theApp.m_bTraceCommand);
		DDX_Check(&dx, IDC_CK_TRACE_RS232_EVENTS, theApp.m_bTraceRS232Events);

		DDX_Check(&dx, IDC_CK_TRACE_DATA, theApp.m_bTraceData);
		if (theApp.m_bTraceData)
		{
			DDX_Check(&dx, IDC_CK_HEX_DATA, bTemp);
			if (!bTemp) theApp.m_bTraceData |= 0x0008;
			else		theApp.m_bTraceData &= ~0x0008;
			DDX_Check(&dx, IDC_CK_ASCCII_DATA, bTemp);
			if (bTemp)	theApp.m_bTraceData |= 0x0004;
			else		theApp.m_bTraceData &= ~0x0004;
		}
	}
	GetDlgItem(IDC_CK_TRACE_COMMAND)->ShowWindow(nCmdShowSettings);
	GetDlgItem(IDC_CK_TRACE_RS232_EVENTS)->ShowWindow(nCmdShowSettings);
	GetDlgItem(IDC_CK_TRACE_DATA)->ShowWindow(nCmdShowSettings);
	GetDlgItem(IDC_CK_HEX_DATA)->ShowWindow(nCmdShowSettings);
	GetDlgItem(IDC_CK_ASCCII_DATA)->ShowWindow(nCmdShowSettings);
	GetDlgItem(IDC_GRP_SETTINGS)->ShowWindow(nCmdShowSettings);
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::OnBnClickedBtnReset()
{
	theApp.OnReset();
}
/////////////////////////////////////////////////////////////////////////////
void CDebugTraceDlg::OnCancel()
{
	CWK_Dialog::OnCancel();
	DestroyWindow();
}
