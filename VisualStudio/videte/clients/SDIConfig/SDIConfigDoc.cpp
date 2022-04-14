/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigDoc.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigDoc.cpp $
// CHECKIN:		$Date: 25.01.99 11:20 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 25.01.99 10:40 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"

#include "ConnectionDlg.h"
#include "SDIConfigInput.h"
#include "SDIConfigRecord.h"

#include "SDIConfigView.h"
#include "SDIConfigDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigDoc
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSDIConfigDoc, CDocument)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIConfigDoc, CDocument)
	//{{AFX_MSG_MAP(CSDIConfigDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigDoc construction/destruction
CSDIConfigDoc::CSDIConfigDoc()
{
	// TODO: add one-time construction code here
	m_dwFileSize = 0;
	m_dwByteRate = 0;

	m_pInput = NULL;
	m_pInputToDelete = NULL;
	m_hExternProcess = NULL;
	ClearDoc();
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigDoc::~CSDIConfigDoc()
{
	ClearDoc();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::OnIdle()
{
	CDocument::OnIdle();
	WK_DELETE(m_pInputToDelete);
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigDoc diagnostics
#ifdef _DEBUG
void CSDIConfigDoc::AssertValid() const
{
	CDocument::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigDoc serialization
void CSDIConfigDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	}
	else {
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	ClearStatusText();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::IsControlRecordOk(SDIControlType ControlType)
{
	BOOL bReturn = FALSE;
	WK_TRACE_WARNING("CSDIConfigDoc::IsControlRecordOk() NOT OERWRITTEN\n");
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigView* CSDIConfigDoc::GetView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) {
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSDIConfigView))) {
			// Zur Zeit hat jedes Doc nur einen View - diesen
			return (CSDIConfigView*)pView;
		}
	}
	return (NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::GetConfigState(CString& sReturn)
{
	m_csConfigState.Lock();
	sReturn = m_sConfigState;
	m_csConfigState.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::IsExternBusy()
{
	BOOL bReturn = FALSE;
	if (m_hExternProcess) {
		DWORD dwExitCode;
		if ( GetExitCodeProcess(m_hExternProcess,&dwExitCode) ) {
			if (STILL_ACTIVE==dwExitCode) {
				bReturn = TRUE;
			}
			else {
				m_hExternProcess = NULL;
				// Ausgabe im Statusbericht
				AddStatusLine(IDS_EXTERN_PROGRAM_CLOSED);
				GetView()->PostMessage(WM_COMMAND, MSG_SDI_EXTERN_PROGRAM_STOPPED, 0);
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::ResetConnecting(BOOL bSelf)
{
	BOOL bReturn = FALSE;
	if (m_bConnecting) {
		bReturn = TRUE;
		m_bConnecting = FALSE;
		CString sFormat, sText;
		if (bSelf) {
			sFormat.LoadString(IDS_CONNECT_END);
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_STOP_CONNECTION, 0);
		}
		else {
			sFormat.LoadString(IDS_DISCONNECTED);
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONNECTION_FAILED, 0);
		}
		sText.Format(sFormat, m_sHost);
		AddStatusLine(sText);
//		WK_TRACE("%s Nr %s\n", sText, m_sHost);
		m_sHost = "";
		m_sNumber = "";
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::ResetConnected(BOOL bSelf)
{
	BOOL bReturn = FALSE;
	if (m_bConnected) {
		bReturn = TRUE;
		m_bConnected = FALSE;
		CString sFormat, sText;
		if (bSelf) {
			sFormat.LoadString(IDS_CONNECT_END);
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_STOP_CONNECTION, 0);
		}
		else {
			sFormat.LoadString(IDS_DISCONNECTED);
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONNECTION_FAILED, 0);
		}
		sText.Format(sFormat, m_sHost);
		AddStatusLine(sText);
//		WK_TRACE("%s Nr %s\n", sText, m_sHost);
		m_sHost = "";
		m_sNumber = "";
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::ResetTransfering(BOOL bOK)
{
	BOOL bReturn = FALSE;
	if (m_bTransfering) {
		bReturn = TRUE;
		m_bTransfering = FALSE;
		// View benachrichtigen, damit er aktualisiert wird
		if (bOK) {
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_FILE_TRANSFER_READY, 0);
		}
		else {
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_FILE_TRANSFER_FAILED, 0);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::ResetConfiguring(BOOL bOK)
{
	BOOL bReturn = FALSE;
	if (m_bConfiguring) {
		bReturn = TRUE;
		m_bConfiguring = FALSE;
		m_idConfigurate = SECID_NO_ID;
		if (bOK) {
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONFIGURATION_READY, 0);
		}
		else {
			GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONFIGURATION_FAILED, 0);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::StartExternProgram()
{
	// Konfigurationsdatei erzeugen
	// Aufruf des externen Programms
	STARTUPINFO StartUpInfo;
	StartUpInfo.cb = sizeof(STARTUPINFO);
    StartUpInfo.lpReserved      = NULL;
    StartUpInfo.lpDesktop       = NULL;
    StartUpInfo.lpTitle	      = NULL;
    StartUpInfo.dwX	          = 0;
    StartUpInfo.dwY			  = 0;
    StartUpInfo.dwXSize	      = 0;
    StartUpInfo.dwYSize	      = 0;
    StartUpInfo.dwXCountChars   = 0;
    StartUpInfo.dwYCountChars   = 0;
    StartUpInfo.dwFillAttribute = 0;
    StartUpInfo.dwFlags	      = /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
    StartUpInfo.wShowWindow     = SW_SHOWNORMAL;
    StartUpInfo.cbReserved2     = 0;
    StartUpInfo.lpReserved2     = NULL;

	PROCESS_INFORMATION ProzessInformation;

	BOOL bProcess = FALSE;

	CString sCmdLine = m_sExecutableDir+m_sExecutableFile+" "+m_sConfigFile;

	bProcess = CreateProcess(
				(LPCSTR)NULL,	// pointer to name of executable module 
				(LPSTR)sCmdLine.GetBuffer(sCmdLine.GetLength()),	// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
				FALSE,	// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				(LPVOID)NULL,	// pointer to new environment block 
				(LPCTSTR)m_sExecutableDir,	// pointer to current directory name 
				(LPSTARTUPINFO)&StartUpInfo,	// pointer to STARTUPINFO 
				(LPPROCESS_INFORMATION)&ProzessInformation 	// pointer to PROCESS_INFORMATION  
				);
	if (!bProcess) {
//		WK_TRACE("Executable NOT startet %s\n", sCmdLine);
		// Ausgabe im Statusbericht
		AddStatusLine(IDS_EXTERN_PROGRAM_NOT_STARTED);

	}
	else {
		m_hExternProcess = ProzessInformation.hProcess;
		// Ausgabe im Statusbericht
		AddStatusLine(IDS_EXTERN_PROGRAM_STARTED);
		GetView()->PostMessage(WM_COMMAND, MSG_SDI_EXTERN_PROGRAM_START, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::SelectHosts()
{
	CConnectionDlg dlg;
	if (IDOK==dlg.DoModal()) {
		m_sHost = dlg.m_sHost;
		m_sNumber = dlg.m_sTelefon;
		if (m_sNumber=="0") {
			m_sNumber = LOCAL_LOOP_BACK;
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::StartConnection(BOOL bLocal)
{
	if (bLocal) {
		m_sNumber = LOCAL_LOOP_BACK;
		m_sHost.LoadString(IDS_LOCAL_SERVER);
	}
	else if ( !SelectHosts() ) {
		return;
	}

	DWORD dwConnectTimeout=10000;	// default timeout

	CSDIConfigApp* pApp = (CSDIConfigApp*)AfxGetApp();
	CUser* pUser = pApp->GetUser();
	CPermission* pPermission = pApp->GetPermission();
	ASSERT(pUser && pPermission);
	CConnectionRecord c(m_sNumber, 
						*pUser, 
						pPermission->GetName(),
						pPermission->GetPassword(),
						dwConnectTimeout
						);

	PostFetchInputConnection(c, MSG_SDI_CONNECTION_READY, (DWORD)this);

	m_bConnecting = TRUE;
	GetView()->PostMessage(WM_COMMAND, MSG_SDI_START_CONNECTION, 0);
	CString sFormat, sText;
	sFormat.LoadString(IDS_CONNECTING);
	sText.Format(sFormat, m_sHost);
	AddStatusLine(sText);
//	WK_TRACE("%s Nr %s\n", sText, m_sNumber);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::OnConnection(CIPCFetchResult& FetchResult)
{
	CString shmInputName = FetchResult.GetShmName();
	CString sFormat, sText;
	BOOL bConnectOK = (FetchResult.GetError()==CIPC_ERROR_OKAY) && !shmInputName.IsEmpty();
	if (bConnectOK) {
		m_pInput = new CSDIConfigInput(shmInputName, FetchResult.GetAssignedID(), this);
		m_bConnecting = FALSE;
		m_bConnected = TRUE;
		sFormat.LoadString(IDS_CONNECTED);
//		GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONNECTION_READY, 0);
	}
	else {
		m_bConnecting = FALSE;
		sFormat.LoadString(IDS_CONNECT_FAILED);
		GetView()->PostMessage(WM_COMMAND, MSG_SDI_CONNECTION_FAILED, 0);
	}
	sText.Format(sFormat, m_sHost);
	AddStatusLine(sText);
	if (!bConnectOK) {
		sFormat.LoadString(IDS_FETCH_ERROR);
		sText.Format(sFormat, FetchResult.GetErrorCode(), FetchResult.GetErrorMessage());
		AddStatusLine(sText);
//		WK_TRACE("%s Grund:%s SMN:<%s>\n",
//				 sText, NameOfEnum(FetchResult.GetError()), shmInputName);
	}
//	else {
//		WK_TRACE("%s\n", sText);
//	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::EndConnection(BOOL bSelf)
{
	ResetConfiguring(FALSE);
	ResetTransfering(FALSE);
	ResetConnecting(bSelf);
	ResetConnected(bSelf);

	if (m_pInput) {
		m_pInput->DelayedDelete();
		m_pInputToDelete = m_pInput;
		m_pInput = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::ControlWasDeleted(const CSecID id)
{
	if (id == m_idConfigurate) {
		// Die Konfiguration wurde unterbrochen
		m_idConfigurate = SECID_NO_ID;
		ResetConfiguring(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::ClearDoc()
{
	m_bConnecting = FALSE;
	m_bConnected = FALSE;
	m_bConfiguring = FALSE;
	m_bTransfering = FALSE;
	m_idConfigurate = SECID_NO_ID;
	m_sHost = "";
	m_sNumber = "";
	if (m_hExternProcess) {
		DWORD dwExitCode;
		if ( GetExitCodeProcess(m_hExternProcess,&dwExitCode) ) {
			if (STILL_ACTIVE==dwExitCode) {
				AfxMessageBox(IDP_EXTERN_PROGRAM_TERMINATED, MB_OK);
				TerminateProcess(m_hExternProcess,dwExitCode);
			}
		}
	}
	WK_DELETE(m_pInputToDelete);
	WK_DELETE(m_pInput);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::AddStatusLine(const CString& sLine)
{
	AddStatusLineIntern(sLine);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::AddStatusLine(UINT uID)
{
	CString sLine;
	sLine.LoadString(uID);
	AddStatusLineIntern(sLine);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::AddStatusLineIntern(const CString& sLine)
{
	m_csConfigState.Lock();
	m_sConfigState += sLine;
	m_sConfigState += "\r\n";
	m_csConfigState.Unlock();
	GetView()->PostMessage(WM_COMMAND,MSG_SDI_UPDATE_STATE, 0);
	WK_TRACE("%s\n", sLine);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigDoc::ClearStatusText()
{
	m_csConfigState.Lock();
	m_sConfigState = "";
	m_csConfigState.Unlock();
	GetView()->PostMessage(WM_COMMAND,MSG_SDI_UPDATE_STATE, 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::TransferFile(CString& sSourcePath, CString& sDestPath) 
{
	BOOL bReturn = FALSE;
	CFile file;
	CFileException cfe;
	CFileStatus cfs;
	LPVOID pBuffer;
	CString sLine, sFormatLine;

	// Ausgabe im Statusbericht Dateitransfer
	sFormatLine.LoadString(IDS_CONFIG_FILE_TRANSFERING);
	sLine.Format(sFormatLine, sSourcePath);
	AddStatusLine(sLine);

	if (CFile::GetStatus(sSourcePath, cfs)) {
		if (file.Open(sSourcePath, CFile::modeRead|CFile::shareDenyNone, &cfe)) {
			// Daten der Datei holen
			m_dwFileSize = (DWORD)cfs.m_size;
			CTime timeModified = cfs.m_mtime;
			// Ausgabe im Statusbericht Dateidaten
			sFormatLine.LoadString(IDS_STATE_OF_FILE);
			CString sTimeModified, sFormatTimeModified;
			sFormatTimeModified.LoadString(IDS_FORMAT_TIME_MODIFIED);
			sTimeModified = timeModified.Format(sFormatTimeModified);
			sLine.Format(sFormatLine, m_dwFileSize, sTimeModified);
			AddStatusLine(sLine);
			// Transfer dauert mindestens etwas (hier 1s)
			DWORD dwTime = 1;
			DWORD dwByterate = m_pInput->GetBitrate()/8;
			if (dwByterate) {
				dwTime += (cfs.m_size)/(dwByterate);
			}
			CTimeSpan timeSpan((time_t)dwTime);
			CString sTimeTransfer, sFormatTimeTransfer;
			sFormatTimeTransfer.LoadString(IDS_FORMAT_TIME_TRANSFER);
			sTimeTransfer = timeSpan.Format("%M min %S s");
			// Ausgabe im Statusbericht Dauer
			sFormatLine.LoadString(IDS_TIME_TRANSFER);
			sLine.Format(sFormatLine, sTimeTransfer);
			AddStatusLine(sLine);

			// Buffer anlegen, um Datei einzulesen
			pBuffer = malloc(cfs.m_size);
			// wenn Einlesen vollstaendig geklappt hat
			if (cfs.m_size==(LONG)file.Read(pBuffer,cfs.m_size)) {
				// Datei uebertragen
				m_pInput->DoRequestFileUpdate(RFU_FULLPATH,sDestPath,
												pBuffer,cfs.m_size,FALSE);
				// Der User soll was sehen. waehrend er wartet
				GetView()->PostMessage(WM_COMMAND,MSG_SDI_FILE_TRANSFER_START, 0);
				bReturn = TRUE;
			}
			else {
				// Datei konnte nicht gelesen werden
				sFormatLine.LoadString(IDS_FILE_NOT_READ);
				sLine.Format(sFormatLine, sSourcePath);
				AddStatusLine(sLine);
			}
			free(pBuffer);
		}
		else {
			// Datei konnte nicht geoeffnet werden
			sFormatLine.LoadString(IDS_FILE_NOT_OPENED);
			sLine.Format(sFormatLine, sSourcePath);
			AddStatusLine(sLine);
		}
	}
	else {
		// Datei nicht gefunden
		sFormatLine.LoadString(IDS_FILE_NOT_FOUND);
		sLine.Format(sFormatLine, sSourcePath);
		AddStatusLine(sLine);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigDoc::GetFile(CString& sSourcePath, CString& sDestPath)
{
	BOOL bReturn = FALSE;
	WK_TRACE_WARNING("GetFile not implemented Source:%s Dest:%s\n", sSourcePath, sDestPath);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigDoc commands
