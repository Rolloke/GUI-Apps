/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ConnectionDlg.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/ConnectionDlg.cpp $
// CHECKIN:		$Date: 24.03.98 11:02 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 24.03.98 11:00 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#include "stdafx.h"
#include "sdiconfig.h"

#include "ConnectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog
CConnectionDlg::CConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionDlg)
	m_sName = _T("");
	m_sPassword = _T("");
	//}}AFX_DATA_INIT
	CWK_Profile wkp;
	m_pHostArray = new CHostArray();
	m_pHostArray->Load(wkp);
}
/////////////////////////////////////////////////////////////////////////////
CConnectionDlg::~CConnectionDlg()
{
	WK_DELETE(m_pHostArray);
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionDlg)
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_LB_HOST, m_lbHosts);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sPassword);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CConnectionDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectionDlg)
	ON_LBN_DBLCLK(IDC_LB_HOST, OnDblclkListboxHost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg message handlers
BOOL CConnectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i,c;
	CString sAdd;

	c = m_pHostArray->GetSize();

	for (i=0;i<c;i++)
	{
		sAdd = ((CHost*)m_pHostArray->GetAt(i))->GetName();
		sAdd += " " + ((CHost*)m_pHostArray->GetAt(i))->GetTyp();
		m_lbHosts.AddString(sAdd);
	}
	
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		if (m_ToolTip.Create(this) ) {
			m_ToolTip.Activate(TRUE);
			// Lasse den ToolTip die Zeilenumbrueche beachten
			m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
			// TODO: Use one of the following forms to add controls:
			// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
			// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
			//Lade die ToolTip-Strings
//			m_sTTEditName.LoadString(IDC_EDIT_NAME);
//			m_ToolTip.AddTool(GetDlgItem(IDC_LB_HOST), IDC_LB_HOST);
//			m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_NAME), m_sTTEditName);
//			m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD), IDC_EDIT_PASSWORD);

			// Alle Child-Windows mit WS_TABSTOP (Controls mit Tab) durchklappern,
			CWnd* pItem = GetNextDlgTabItem(this);
			// Sicherheitshalber testen, ob Child vorhanden
			if ( pItem && IsChild(pItem) ) {
				// Bei diesem Control beginnt die Suche
				UINT uStartID = pItem->GetDlgCtrlID();
				UINT uID = uStartID;
				CString sTip;
				int iIndex = 0;
				// Wir wollen keine Endlosschleife
				BOOL bComplete=FALSE;
				while ( !bComplete ) {
					// die ToolTip-Texte aus der Resource laden,
					sTip.LoadString(uID);
					// falls Text vorhanden ins Array schreiben und das Tool anhaengen
					if ( !sTip.IsEmpty() ) {
						iIndex = m_sTTArray.Add(sTip);
						m_ToolTip.AddTool( pItem, m_sTTArray.GetAt(iIndex) );
					}
					pItem = GetNextDlgTabItem(pItem);
					uID = pItem->GetDlgCtrlID();
					// Wir wollen nicht immer wieder von vorne anfangen
					if (uStartID == uID) {
						bComplete = TRUE;
					}
				}
			}
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionDlg::OnDblclkListboxHost() 
{
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionDlg::OnOK() 
{
	int i;

	i = m_lbHosts.GetCurSel();
	if (i!=CB_ERR && i<m_pHostArray->GetSize())
	{
		CHost* pHost;
		pHost = (CHost*)m_pHostArray->GetAt(i);
		m_sHost = pHost->GetName();
		m_sTelefon = pHost->GetNumber();
	}
	else
		return;
	
	CDialog::OnOK();
}

BOOL CConnectionDlg::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_ToolTip.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}
