/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigKebaDoc.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigKebaDoc.cpp $
// CHECKIN:		$Date: 26.01.98 15:00 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 26.01.98 11:38 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"
#include "SDIConfigDoc.h"
#include "SDIConfigInput.h"

#include "SDIConfigKebaView.h"
#include "SDIConfigKebaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SDIConfigKebaDoc
IMPLEMENT_DYNCREATE(CSDIConfigKebaDoc, CSDIConfigDoc)
/////////////////////////////////////////////////////////////////////////////
CSDIConfigKebaDoc::CSDIConfigKebaDoc()
{
	// Soll-ByteRate
//	m_dwByteRate = 9600/10;
	// Empirisch ermittelte ByteRate
//	m_dwByteRate = 9600/60;
	m_dwByteRate = 160;

	m_sExecutableDir = "C:\\KEBA\\";
	m_sExecutableFile = "KONFIG.EXE";
	m_sConfigDir = m_sExecutableDir + "DATEN\\";
	m_sConfigFile = "TRANS.KFG";

	m_bExternProgramFound = FALSE;
	CFileStatus cfs;
	if (CFile::GetStatus(m_sExecutableDir+m_sExecutableFile, cfs)) {
		m_bExternProgramFound = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigKebaView* CSDIConfigKebaDoc::GetView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) {
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSDIConfigKebaView))) {
			// Zur Zeit hat jedes Doc nur einen View - diesen
			return (CSDIConfigKebaView*)pView;
		}
	}
	return (NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigKebaDoc::OnNewDocument()
{
	if (!CSDIConfigDoc::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigKebaDoc::~CSDIConfigKebaDoc()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaDoc::OnIdle()
{
	// Hier darf man auch direkt den View veraendern!

	// Erst mal Basisklasse aufrufen!!!
	CSDIConfigDoc::OnIdle();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIConfigKebaDoc, CSDIConfigDoc)
	//{{AFX_MSG_MAP(CSDIConfigKebaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaDoc diagnostics
#ifdef _DEBUG
void CSDIConfigKebaDoc::AssertValid() const
{
	CSDIConfigDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaDoc::Dump(CDumpContext& dc) const
{
	CSDIConfigDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaDoc serialization
void CSDIConfigKebaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	}
	else {
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigKebaDoc::IsControlRecordOk(SDIControlType ControlType)
{
	BOOL bReturn = FALSE;
	// OOPS gf todo
	// Zur Zeit wird nur Keba unterstuetzt
	bReturn = (SDICT_KEBA_PC2000==ControlType);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaDoc::Configurate(CSecID id)
{
	// Sichern der id
	m_idConfigurate = id;
	// Erst muessen wir die Konfigurations-Datei uebertragen
	StartTransferConfigFile();
	// Warten auf Rueckmeldung
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaDoc::StartTransferConfigFile()
{
	// Uebertragen der Konfig-Datei
	m_bTransfering = TRUE;

	// Setzen der Pfade der Konfigurations-Datei
	CString sSourcePath = m_sConfigDir + m_sConfigFile;
	CString sDestPath = sSourcePath;

	if ( !TransferFile(sSourcePath, sDestPath) ) {
		ResetTransfering(FALSE);
	}
	else {
		// Warten auf Rueckmeldung
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigKebaDoc::StartConfiguration()
{
	// Nur erlaubt, wenn Verbindung besteht und die ID gueltig
	if ( m_bConnected && m_idConfigurate.IsInputID() ) {
		// Starten der Konfiguration
		m_bConfiguring = TRUE;
		// Der User soll was sehen. waehrend er wartet
		GetView()->PostMessage(WM_COMMAND,MSG_SDI_CONFIGURATION_TRY, 0);

		// Setzen des Pfades der Konfigurations-Datei
		CString sPath = m_sConfigDir + m_sConfigFile;

		// Ausgabe im Statusbericht
		CString sText, sFormat;
		sFormat.LoadString(IDS_CONFIGURE);
		sText.Format(sFormat, m_pInput->GetCommentFromRecordArrayByGroupID(m_idConfigurate));
		AddStatusLine(sText);

		m_pInput->DoRequestSetValue(m_idConfigurate, SDI_KEY_START_XMODEM_TRANSMITTER, sPath, (DWORD)this);
		// Warten auf Rueckmeldung
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaDoc commands
