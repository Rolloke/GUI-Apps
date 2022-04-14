// KnockKnockBox.cpp : implementation file
//

#include "stdafx.h"
#include "isdnunit.h"
#include "KnockKnockBox.h"
#include "ISDNConnection.h"
#include "host.h"
#include ".\knockknockbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKnockKnockBox dialog
CKnockKnockBox::CKnockKnockBox(CISDNConnection *pConnection, CWnd* pParent /*=NULL*/)
	: CDialog(CKnockKnockBox::IDD, pParent)
{
	m_pConnection = pConnection;
	//{{AFX_DATA_INIT(CKnockKnockBox)
	m_sName = _T("");
	m_sNumber = _T("");
	m_sTime = _T("");
	m_sCurrentConnection = _T("");
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CKnockKnockBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKnockKnockBox)
	DDX_Text(pDX, IDC_KNOCK_NAME, m_sName);
	DDX_Text(pDX, IDC_KNOCK_NUMBER, m_sNumber);
	DDX_Text(pDX, IDC_KNOCK_TIME, m_sTime);
	DDX_Text(pDX, IDC_CURRENT_CONNECTION, m_sCurrentConnection);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BOOL CKnockKnockBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CKnockKnockBox, CDialog)
	//{{AFX_MSG_MAP(CKnockKnockBox)
	ON_BN_CLICKED(IDC_CLOSE_ACTIVE_CONNECTION, OnCloseActiveConnection)
	ON_BN_CLICKED(IDC_CLOSE_ONE_CHANNEL, OnBnClickedCloseOneChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CKnockKnockBox::OnCloseActiveConnection() 
{
	ShowWindow(SW_HIDE);
	if (m_pConnection)
	{
		// no need for CrossThread, KnockKnock is created in the CapiThread
		m_pConnection->CloseConnection();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CKnockKnockBox::OnBnClickedCloseOneChannel()
{
	ShowWindow(SW_HIDE);
	if (m_pConnection)
	{
		m_pConnection->CloseOneChannel();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CKnockKnockBox::OnOK() 
{
	ShowWindow(SW_HIDE);
	// CAVEAT do not call CDialog::OnOK for NonModal
}
/////////////////////////////////////////////////////////////////////////////
/*@MD override virtual fns, even if there is no explicit button for it */
void CKnockKnockBox::OnCancel() 
{
	ShowWindow(SW_HIDE);
	// CAVEAT do not call CDialog::OnCancel for NonModal
}
/////////////////////////////////////////////////////////////////////////////
void CKnockKnockBox::SetCallInfo(const CAbstractCapiCallInfo &callInfo, BOOL bCanCloseConnection)
{
	CTime currentTime;
	currentTime = CTime::GetCurrentTime();
	// NOT YET use IDS_ for localized date format ?
	// might be builtin CTime::Format
	m_sTime = currentTime.Format(_T("%H:%M, %d.%m.%Y"));

	// callingPartyNumber:
	m_sNumber = callInfo.GetCallingParty();

	// guessed remote station name:
	m_sName.LoadString(IDS_UNKNOWN);
	BOOL bFound = FALSE;
	if (m_sNumber.GetLength()==0)
	{
		bFound = TRUE;	// can't guess without number
		m_sName.LoadString(IDS_WITHOUT_NUMBER);
	}
	// NOT YET keep on searching if not exact matched
	CWK_Profile wkp;
	CHostArray hosts;
	hosts.Load(wkp);
	CHost* pHost = NULL;
	CString sHostNumber;
	CString sRightPart;
	for (int i=0 ; bFound==FALSE && i<hosts.GetSize() ; i++)
	{
		pHost = hosts.GetAt(i);
		if (pHost->IsISDN())
		{
			sHostNumber = pHost->GetNumber();
			if (sHostNumber.GetLength() >= m_sNumber.GetLength())
			{
				sRightPart = sHostNumber.Right(m_sNumber.GetLength());
				if (sRightPart == m_sNumber)
				{
					m_sName = pHost->GetName();
					bFound=TRUE;
				}
			}
		}
	}	// end of loop over all hosts

	// active connection:
	if (bCanCloseConnection)
	{
		m_sCurrentConnection = m_pConnection->GetRemoteHostName();
		m_sCurrentConnection += _T("[");
		m_sCurrentConnection += m_pConnection->GetRemoteNumber();
		m_sCurrentConnection += _T("]");
	}
	else
	{
		m_sCurrentConnection = "";
	}

	BOOL bEnableCloseOneChannel = FALSE;
	if (   theApp.DoCloseOneChannel()
		&& m_pConnection->GetConnectState() == CONNECT_OPEN
		&& m_pConnection->GetNumBChannels() == 2
		&& (theApp.IsSecondInstance() || theApp.IsSecondInstanceAllowed()) )
	{
		bEnableCloseOneChannel = TRUE;
	}

	GetDlgItem(IDC_TXT_CURRENT_CONNECTION)->EnableWindow(bCanCloseConnection);
	GetDlgItem(IDC_CLOSE_ACTIVE_CONNECTION)->EnableWindow(bCanCloseConnection);

	GetDlgItem(IDC_CLOSE_ONE_CHANNEL)->EnableWindow(bEnableCloseOneChannel);

	UpdateData(FALSE);

	SetWindowPos( &CWnd::wndTopMost, NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);	
	MessageBeep(MB_OK);

	if (theApp.DoCloseOneChannel() == AUTOMATICALLY && bEnableCloseOneChannel)
	{
		PostMessage(WM_COMMAND, IDC_CLOSE_ONE_CHANNEL, (LPARAM)m_hWnd);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*@TOPIC{Zwischen-Anrufe|ISDNUnit,KnockKnock}
@RELATED 
@LINK{Dialog|ISDNUnit,KnockKnockBox}, @LINK{Einstellungen|ISDNUnit,Settings}

Ein Zwischen-Anruf, ist wenn in eine bestehende Verbindung ein Anruf eingeht.
Das Trennen der aktuellen Verbindung ist dem Benutzer überlassen, es gibt KEINEN Automatismus.
Damit der Benutzer merkt, was los ist erscheint ein @LINK{Dialog|ISDNUnit,KnockKnockBox}.

Das Erscheinen des Dialogs ist optional und über die 
@LINK{Einstellungen|ISDNUnit,Settings}, abschaltbar.
Bei default ist es aktiv, das heisst der Dialog kommt.

@ALSO @LINK{Dialog|ISDNUnit,KnockKnockBox}, @LINK{Einstellungen|ISDNUnit,Settings}
*/

/*@TOPIC{Der Zwischen-Anrufer Dialog|ISDNUnit,KnockKnockBox}

@RELATED @LINK{Zwischen-Anrufe|ISDNUnit,KnockKnock}
@LINE

@IMAGE{\Project\Doc\SeInDo\KnockKnock.bmp}

Lebenslauf:
@ITEM Erscheint,
@ITEM2 wenn ein Zwischenanruf eingeht, 
sofern es in den Einstellungen nicht unterbunden ist.

@ITEM Verschwindet
@ITEM2 durch den Benutzer, bei einem der großen Knöpfe
@ITEM2 wenn die akutelle Verbindung verschwindet
@ITEM2 wenn "Zeige Dialog" in den Einstellungen deaktiviert wird.

Es gibt genau einen Dialog. Wenn ein weiterer Anruf eingeht, werden
die angezeigten Daten aktualisiert.

@PAR
@BW{Anmerkungen:} Dadurch daß der Dialog bei jedem CLOSE automatisch
verschwindet, ist also nicht mehr ersichtlich, ob irgendein Zwischnanruf war.
Aber es hat den Vorteil, daß der Dialog sich wiklich immer auf
den aktuelln Zustand bezieht. Sonst kommt leicht sowas:
@ITEM	10:00 ALARMAnruf von StationA wird angenommen
@ITEM	10:01 ALARMAnruf von StationB blendet ZwischnAnruferBox ein.
@ITEM	10:05 StationA ist fertig, wird getrennt, der Dialog verschwindet.
(Okay in einer Wachzentrale wird das auch nur vom Benutzer gemacht)
@ITEM	10:06 ALARMAnruf StationB wird angenommen.

Würde jetzt noch der Dialog rumstehen, könnte ein unaufmerksamer Benutzer,
nur den "Aktive Vebindungen trennen" Knopf sehen und drücken. Das
würde dann aber StationB trennen.

Es ist schon angedacht, noch eine Art ProtokollDialog dazu zu bauen.
Da koennte man dann alle Vorgaenge einsehen. Zur Zeit steht zwar
alles im LogFile (ISDNUnit.log), aber wer will da schon ran,
selbst mit SecAnalzyer nicht so einfach.

@ALSO @LINK{Zwischen-Anrufe|ISDNUnit,KnockKnock}
*/
