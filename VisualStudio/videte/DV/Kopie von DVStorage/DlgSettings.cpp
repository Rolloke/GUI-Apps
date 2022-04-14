// DlgSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DVStorage.h"
#include "DlgSettings.h"
#include <wkclasses/OScopeCtrl.h>

#define NO_OF_SCOPE_CTRLS 2
//////////////////////////////////////////////////////////////////////
// CDlgSettings dialog
IMPLEMENT_DYNAMIC(CDlgSettings, CDialog)
//////////////////////////////////////////////////////////////////////
CDlgSettings::CDlgSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSettings::IDD, pParent)
{
	m_uTimer = 0;
	m_pScopeCtrlStore = NULL;
	m_pScopeCtrlFps = NULL;
}
//////////////////////////////////////////////////////////////////////
CDlgSettings::~CDlgSettings()
{
	WK_DELETE(m_pScopeCtrlStore);
	WK_DELETE(m_pScopeCtrlFps);
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgSettings, CDialog)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_FAST_RESET, OnBnClickedBtnFastReset)
	ON_BN_CLICKED(IDC_BTN_CLEAR_OUTPUT, OnBnClickedBtnClearOutput)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER, OnUser)
	ON_BN_CLICKED(IDC_BTN_COPY_OUTPUT, OnBnClickedBtnCopyOutput)
	ON_BN_CLICKED(IDC_RD_DEBUG_TRACE, OnBnClickedRdDebugTrace)
	ON_BN_CLICKED(IDC_RD_DIAGRAM, OnBnClickedRdDiagram)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CDlgSettings message handlers
void CDlgSettings::OnBnClickedBtnReset()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_RESET, (LPARAM)m_hWnd);
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnBnClickedBtnFastReset()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_FAST_RESET, (LPARAM)m_hWnd);
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnBnClickedBtnClearOutput()
{
	SetDlgItemText(IDC_EDT_TRACE,_T(""));
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
BOOL CDlgSettings::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDebuggerWindowHandle(GetDlgItem(IDC_EDT_TRACE)->m_hWnd);
	ShowWindow(SW_SHOW);
	SendDlgItemMessage(IDC_RD_DEBUG_TRACE, BM_SETCHECK, BST_CHECKED, 0);
	
	m_uTimer = SetTimer(100, 500, NULL);
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnDestroy()
{
	SetDebuggerWindowHandle(NULL);
	if (m_uTimer)
	{
		KillTimer(m_uTimer);
		m_uTimer = 0;
	}
	WK_DELETE(m_pScopeCtrlStore);
	WK_DELETE(m_pScopeCtrlFps);
	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void CDlgSettings::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_uTimer)
	{
		SetDlgItemInt(IDC_TXTINT_BUBBLES, CIPCExtraMemory::GetCreatedBubbles());
	}
	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
LRESULT CDlgSettings::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case 1:
			SetDlgItemText(IDC_TXT_LINE1, (LPCTSTR)lParam);
			break;
		case 2:
			SetDlgItemText(IDC_TXT_LINE2, (LPCTSTR)lParam);
			break;
		case 3:
			SetDlgItemText(IDC_TXT_LINE3, (LPCTSTR)lParam);
			break;
		case 4:
			SetDlgItemText(IDC_TXT_LINE4, (LPCTSTR)lParam);
			break;
		case IDC_TXTINT_SAVE_BUFFER:
		case IDC_TXTINT_SAVE_BUFFER_COUNT:
		case IDC_TXTINT_STORE:
			SetDlgItemInt(wParam, lParam);
			if (wParam == IDC_TXTINT_STORE && m_pScopeCtrlStore && m_pScopeCtrlStore->IsWindowVisible())
			{
				m_dwAvgStore.AddValue((DWORD)lParam);
				m_pScopeCtrlStore->AppendPoint((DWORD)lParam, 0, FALSE);
				m_pScopeCtrlStore->AppendPoint(m_dwAvgStore.GetAverage(), 1);
			}
			break;
		case IDC_TXTINT_BITRATEMB:
		case IDC_TXTINT_FPS:
		{
			CString sFmt;
			sFmt.Format(_T("%.1f"), *((float*)&lParam));
			SetDlgItemText(wParam, sFmt);
			if (wParam == IDC_TXTINT_FPS && m_pScopeCtrlFps && m_pScopeCtrlFps->IsWindowVisible())
			{
				m_pScopeCtrlFps->AppendPoint((double)*((float*)&lParam));
			}
		}	break;

	}
	return 0;
}
//////////////////////////////////////////////////////////////////////

void CDlgSettings::OnBnClickedBtnCopyOutput()
{
	SendDlgItemMessage(IDC_EDT_TRACE, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
	SendDlgItemMessage(IDC_EDT_TRACE, WM_COPY, (WPARAM)0, (LPARAM)0);
}

void CDlgSettings::OnBnClickedRdDebugTrace()
{
	CWnd *pEdit = GetDlgItem(IDC_EDT_TRACE);
	pEdit->ShowWindow(SW_SHOW);
	SetDebuggerWindowHandle(pEdit->m_hWnd);
	if (m_pScopeCtrlStore && m_pScopeCtrlFps)
	{
		m_pScopeCtrlStore->ShowWindow(SW_HIDE);
		m_pScopeCtrlFps->ShowWindow(SW_HIDE);
	}
}

void CDlgSettings::OnBnClickedRdDiagram()
{
	CWnd *pEdit = GetDlgItem(IDC_EDT_TRACE);
	pEdit->ShowWindow(SW_HIDE);
	SetDebuggerWindowHandle(NULL);
	if (m_pScopeCtrlStore == NULL)
	{
		CRect rect;
		CString sLoad;
		int i = 0;
		pEdit->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.bottom = rect.top + rect.Height() / 2;
		m_pScopeCtrlStore = new COScopeCtrl(2);
		m_pScopeCtrlStore->Create(WS_CHILD, rect, this, 100+i++);

		rect.OffsetRect(0, rect.Height());
		m_pScopeCtrlFps = new COScopeCtrl;
		m_pScopeCtrlFps->Create(WS_CHILD, rect, this, 100+i);

		sLoad.LoadString(IDS_STORING_INTERVAL);
		m_pScopeCtrlStore->SetXUnits(sLoad) ;
		m_pScopeCtrlFps->SetXUnits(sLoad) ;

//		m_pScopeCtrlStore->SetBackgroundColor(RGB(0, 0, 64)) ;
//		m_pScopeCtrlStore->SetGridColor(RGB(192, 192, 255)) ;
//		m_pScopeCtrlStore->SetPlotColor(RGB(255, 255, 255)) ;
		sLoad.LoadString(IDS_STORE_MUES);
		m_pScopeCtrlStore->SetYUnits(sLoad) ;

//		m_pScopeCtrlFps->SetBackgroundColor(RGB(0, 0, 64)) ;
//		m_pScopeCtrlFps->SetGridColor(RGB(192, 192, 255)) ;
//		m_pScopeCtrlFps->SetPlotColor(RGB(255, 255, 255)) ;
		sLoad.LoadString(IDS_FRAMES_PER_SECOND);
		m_pScopeCtrlFps->SetYUnits(sLoad);
	}
	if (m_pScopeCtrlStore && m_pScopeCtrlFps)
	{
		m_pScopeCtrlStore->ShowWindow(SW_SHOW);
		m_dwAvgStore.SetNoOfAvgValues(theApp.GetInValue(_T("StoreAvgValues"), 20));
		m_pScopeCtrlStore->SetRange(0.0, (double)theApp.GetInValue(_T("RangeMaxStore"), 5000), 1) ;
		m_pScopeCtrlFps->ShowWindow(SW_SHOW);
		m_pScopeCtrlFps->SetRange(0.0, (double)theApp.GetInValue(_T("RangeMaxFps"), 1000), 1) ;
	}
}
