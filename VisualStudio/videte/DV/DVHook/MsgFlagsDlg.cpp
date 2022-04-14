// MsgFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvhook.h"
#include "MsgFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgFlagsDlg dialog


CMsgFlagsDlg::CMsgFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgFlagsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMsgFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgFlagsDlg)
	DDX_Control(pDX, IDC_LIST_MSG, m_ListMsgFlags);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CMsgFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgFlagsDlg message handlers

BOOL CMsgFlagsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   m_ListMsgFlags.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	CString sHeadline = _T("Flags");
	CRect rc;
	m_ListMsgFlags.GetClientRect(&rc);
	LV_COLUMN lvc = {LVCF_TEXT|LVCF_WIDTH, 0, rc.right, (_TCHAR*)LPCTSTR(sHeadline), sHeadline.GetLength(), 0, 0, 0};
	m_ListMsgFlags.InsertColumn(0, &lvc);
	int i;
	_TCHAR *pszItem[] =
	{
		_T("Output to Logfile"),				// 0x00000001
		_T("Output to Window"),					// 0x00000002
		_T("Debug Messages from Hook.Dll"),	// 0x00000004
		_T("Unused"),								// 0x00000008
		_T("WM_ACTIVATE"),						// 0x00000010
		_T("WM_NCACTIVATE"),						// 0x00000020
		_T("WM_INITDIALOG"),						// 0x00000040
		_T("WM_INITMENU"),						// 0x00000080
		_T("WM_MENUSELECT"),						// 0x00000100
		_T("WM_CREATE"),							// 0x00000200
		_T("WM_NCCREATE"),						// 0x00000400
		_T("WM_SETFOCUS"),						// 0x00000800
		_T("WM_SHOWWINDOW"),						// 0x00001000
		_T("WM_MOVE"),								// 0x00002000
		_T("WM_LBUTTONDOWN"),					// 0x00004000
		_T("WM_LBUTTONUP"),						// 0x00008000
		_T("WM_LBUTTONDBLCLK"),					// 0x00010000
		_T("WM_RBUTTONDOWN"),					// 0x00020000
		_T("WM_RBUTTONUP"),						// 0x00040000
		_T("WM_RBUTTONDBLCLK"),					// 0x00080000
		_T("WM_MBUTTONDOWN"),					// 0x00100000
		_T("WM_MBUTTONUP"),						// 0x00200000
		_T("WM_MBUTTONDBLCLK"),					// 0x00400000
		_T("WM_WINDOWPOSCHANGING"),			// 0x00800000
		_T("WM_KILLFOCUS"),						// 0x01000000
		_T("WM_DESTROY"),							// 0x02000000
		_T("WM_CLOSE"),							// 0x04000000
		_T("Others"),								// 0x08000000
		_T("HCBT_DESTROYWND"),					// 0x10000000
		_T("HCBT_ACTIVATE"),						// 0x20000000
		_T("HCBT_CLICKSKIPPED"),				// 0x40000000
		_T("HCBT_SETFOCUS")						// 0x80000000
	};
	for (i=0; i<32; i++)
	{
		m_ListMsgFlags.InsertItem(i, pszItem[i]);
		m_ListMsgFlags.SetCheck(i, ((1<<i)&m_dwFlags) ? TRUE:FALSE);
	}
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMsgFlagsDlg::OnOK() 
{
	for (int i=0; i<32; i++)
	{
		if (m_ListMsgFlags.GetCheck(i))
		{
			m_dwFlags |= (1<<i);
		}
		else
		{
			m_dwFlags &= ~(1<<i);
		}
	}
	
	CDialog::OnOK();
}
