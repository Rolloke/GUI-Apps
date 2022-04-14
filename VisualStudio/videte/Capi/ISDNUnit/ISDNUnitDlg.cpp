/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNUnitDlg.cpp $
// CHECKIN:		$Date: 30.09.05 14:43 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 30.09.05 14:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"

#include "wk_msg.h"
#include "capi4.h"

#include "Net_Messages.h"
#include "IsdnUnit.h"

#include "ISDNUnitDlg.h"
#include "CapiQueue.h"	// fotr the Trace toggles

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CISDNUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CISDNUnitDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(ID_ADVANCED_SETTINGS, OnAdvancedSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CISDNUnitDlg dialog
CISDNUnitDlg::CISDNUnitDlg(CISDNConnection* pConnection, CWnd* pParent /*=NULL*/)
	: CDialog(CISDNUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CISDNUnitDlg)
	m_sRufnummer = _T("");
	m_byBChannels = 0;
	m_bTraceSend = FALSE;
	m_bTraceReceive = FALSE;
	m_processorUsage = -1;
	m_bEnableKnockBox = FALSE;
	//}}AFX_DATA_INIT
	m_sCodedRufnummer	= _T("");
}


/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	if ( !pDX->m_bSaveAndValidate )
	{
		m_sRufnummer = MyGetApp()->GetOwnNumber();
		// Hole Anzahl der B-Kanaele
		m_byBChannels = (BYTE)MyGetApp()->GetNumConfiguredBChannels();

		m_bTraceSend = CCapiQueue::m_bDoLogSend;
		m_bTraceReceive = CCapiQueue::m_bDoLogReceive;
		m_processorUsage = MyGetApp()->GetProcessorUsage()-1;

		m_bEnableKnockBox = MyGetApp()->IsKnockBoxEnabled();
	}

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISDNUnitDlg)
	DDX_Control(pDX, IDC_B_CHANNEL, m_ctBChannels);
	DDX_Control(pDX, ID_ADVANCED_SETTINGS, m_ctAdvancedSettings);
	DDX_Text(pDX, IDC_RUFNUMMER, m_sRufnummer);
	DDV_MaxChars(pDX, m_sRufnummer, MAX_LEN_RUFNUMMER);
	DDX_Text(pDX, IDC_B_CHANNEL, m_byBChannels);
	DDV_MinMaxByte(pDX, m_byBChannels, 1, 2);
	DDX_Check(pDX, IDC_TRACE_SEND, m_bTraceSend);
	DDX_Check(pDX, IDC_TRACE_RECEIVE, m_bTraceReceive);
	DDX_Radio(pDX, IDC_USAGE_HIGH, m_processorUsage);
	DDX_Check(pDX, IDC_ENABLE_KNOCK_BOX, m_bEnableKnockBox);
	//}}AFX_DATA_MAP
	
}

/////////////////////////////////////////////////////////////////////////////
// CISDNUnitDlg message handlers
BOOL CISDNUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	if (MyGetApp()->RunAsPTUnit()==FALSE) {
		// NOT YET ISDNUnit advanced settings
		m_ctAdvancedSettings.ShowWindow(SW_HIDE);
	} else {
		// PTUnit always one BChannel
		// OOPS should always be 1 NOT YET checked in init
		m_ctBChannels.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::OnHelp() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::OnOK() 
{
	if ( UpdateData(TRUE) ) {
		CCapiQueue::m_bDoLogSend = m_bTraceSend;
		CCapiQueue::m_bDoLogReceive = m_bTraceReceive;
		ISDNProcessorUsage usage = (ISDNProcessorUsage)(m_processorUsage+1);	// OOPS direct cast
		MyGetApp()->SetProcessorUsage(usage);
		MyGetApp()->EnableKnockBox(m_bEnableKnockBox);
		int iNumChannels = m_byBChannels;
		MyGetApp()->SetupChanged(m_sRufnummer,iNumChannels);
		ShowWindow(SW_HIDE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CISDNUnitDlg::OnCancel() 
{
	if ( UpdateData(FALSE) )
	{
		ShowWindow(SW_HIDE);
	}
}


/*@TOPIC{Einstellungen|ISDNUnit,Settings}
@RELATED @LINK{ISDNUnit Hauptseite|ISDNUnit,MainPage}

@IMAGE{\Project\Doc\SeInDo\ISDNSettings.bmp}

@LISTHEAD{Eigene Rufnummer:}
@ITEM Die eigene Nummer oder 'A', um alles anzunehmen.
@LISTHEAD{Anzahl B-Kanäle:}
@ITEM 1 oder 2
@LISTHEAD{Zwischenanrufer_Zeige Dialog:}
@ITEM Wenn aktiviert erscheint der 
@LINK{Zwischenanrufer-Dialog|ISDNUnit,KnockKnockBox}
@LISTHEAD{Systembelastung}
@ITEM Wie stark darf die ISDNUnit das System belasten?
Messungen haben ergeben, daß "Mittel" den besten Gesammtwert 
(Systembelastung und Bitrate) ergibt.
Dies gilt fuer aktive Verbindungen, nicht für den Leerlauf, da sollte
die Belastung eh sehr gering sein.
@LISTHEAD{Protokolliere}
@ITEM Nur zur Fehlersuche gedacht!
Protokolliert im @LINK{ISDNUnit.log|Log,ISDNUnit} jedes einzelne Paket.

@ALSO @LINK{ISDNUnit Hauptseite|ISDNUnit,MainPage}
*/

#include "PTAdvancedSettingsDialog.h"
#include "PTBox.h"

void CISDNUnitDlg::OnAdvancedSettings() 
{

	if (MyGetApp()->RunAsPTUnit()) {
		CPTAdvancedSettingsDialog dlg;
		dlg.m_bDoHexDumpReceive = CPTBox::m_bDoHexDumpReceive;
		dlg.m_bDoHexDumpSend = CPTBox::m_bDoHexDumpSend;

		dlg.m_bDisableEEPWrite = CPTBox::m_bDisableEEPWrite;
		dlg.m_bDisableFrameGrabbing= CPTBox::m_bDisableFrameGrabbing;
		dlg.m_bDoTraceFrameSize = CPTBox::m_bDoTraceFrameSize;

		dlg.m_bLowAsMid = CPTBox::m_bLowAsMid;
		if (dlg.DoModal()==IDOK) {
			CPTBox::m_bDoHexDumpReceive = dlg.m_bDoHexDumpReceive;
			CPTBox::m_bDoHexDumpSend = dlg.m_bDoHexDumpSend;

			CPTBox::m_bDisableEEPWrite = dlg.m_bDisableEEPWrite;

			CPTBox::m_bDisableFrameGrabbing = dlg.m_bDisableFrameGrabbing;
			CPTBox::m_bLowAsMid = dlg.m_bLowAsMid;
			CPTBox::m_bDoTraceFrameSize = dlg.m_bDoTraceFrameSize;

			MyGetApp()->WriteDebugOptions();
		}
	} else {
		// NOT YET button should be hidden/disabled
	}
	
}
