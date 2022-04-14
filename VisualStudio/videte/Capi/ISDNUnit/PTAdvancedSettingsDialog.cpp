// PTAdvancedSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "isdnunit.h"
#include "PTAdvancedSettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTAdvancedSettingsDialog dialog


CPTAdvancedSettingsDialog::CPTAdvancedSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPTAdvancedSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPTAdvancedSettingsDialog)
	m_bDoHexDumpReceive = FALSE;
	m_bDoHexDumpSend = FALSE;
	m_bDisableEEPWrite = FALSE;
	m_bDisableFrameGrabbing = FALSE;
	m_bLowAsMid = FALSE;
	m_bDoTraceFrameSize = FALSE;
	//}}AFX_DATA_INIT
}


void CPTAdvancedSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPTAdvancedSettingsDialog)
	DDX_Check(pDX, IDC_DO_HEX_DUMP_RECEIVE, m_bDoHexDumpReceive);
	DDX_Check(pDX, IDC_DO_HEX_DUMP_SEND, m_bDoHexDumpSend);
	DDX_Check(pDX, IDC_PT_DISABLE_EEP_WRITE, m_bDisableEEPWrite);
	DDX_Check(pDX, IDC_PT_DISABLE_FRAME_GRABBING, m_bDisableFrameGrabbing);
	DDX_Check(pDX, IDC_PT_LOW_AS_MID, m_bLowAsMid);
	DDX_Check(pDX, IDC_PT_DO_TRACE_PICTURES, m_bDoTraceFrameSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPTAdvancedSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CPTAdvancedSettingsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTAdvancedSettingsDialog message handlers

BOOL CPTAdvancedSettingsDialog::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

BOOL CPTAdvancedSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();	// CG: This was added by the ToolTips component.
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool(GetDlgItem(IDC_DO_HEX_DUMP_SEND),IDS_TT_PT_HEX_DUMP_SEND);
		m_tooltip.AddTool(GetDlgItem(IDC_DO_HEX_DUMP_RECEIVE),IDS_TT_PT_HEX_DUMP_RECEIVE);

		m_tooltip.AddTool(GetDlgItem(IDC_PT_DISABLE_FRAME_GRABBING),IDS_TT_PT_DISABLE_FRAME_GRABBING);
		m_tooltip.AddTool(GetDlgItem(IDC_PT_LOW_AS_MID),IDS_TT_PT_LOW_AS_MID);
		m_tooltip.AddTool(GetDlgItem(IDC_PT_DISABLE_EEP_WRITE),IDS_TT_PT_DISABLE_EEP_WRITE);
			
	}
	return TRUE;	// CG: This was added by the ToolTips component.
}
