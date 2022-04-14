// BackupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "BackupDialog.h"

#include "Server.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "ObjectView.h"
#include "CopyReadOnlyVersion.h"

#include "SelectBackupDriveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackupDialog dialog
CBackupDialog::CBackupDialog(UINT IDD, CServer* pServer, CRechercheView* pParent /*=NULL*/)
	: CWK_Dialog(IDD, pParent)
{
	m_bHandleDeviceChange = TRUE;

	m_pCopyReadOnlyVersion = NULL;
	m_pDrive = NULL;
	m_pServer = pServer;
	m_pParent = pParent;
	CTime t = CTime::GetCurrentTime();
	m_dwBackupID = t.GetTime();

	m_dwCountDownInSeconds  = 0;

	m_i64BytesToCopy = 0;
	m_i64BytesCopied = 0;

	m_dwBytesPerSecond = 150 * 1024;
	m_pDrive = NULL;
	m_bDriveInfoChanged = FALSE;

	//{{AFX_DATA_INIT(CBackupDialog)
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
CBackupDialog::~CBackupDialog()
{
	m_ArchivesToCopy.DeleteAll();
	WK_DELETE(m_pDrive);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::InitDirectCDLanguageStrings()
{
	m_sDCDFormatting = _T("For&matieren...");
	m_sDCDRemoveWriteProtection = _T("&Schreibschutz aufheben...");

	CString sLoc(_T("directcd.loc"));
	CString sExe(_T("directcd.exe"));
	CString sDirectCDPath = FindDirectCDPath();

	if(FileExists(sDirectCDPath, sLoc))
	{
		m_sDCDFormatting = GetStringFromTable(sDirectCDPath, sLoc, 200);
		m_sDCDRemoveWriteProtection = GetStringFromTable(sDirectCDPath, sLoc, 203);
	}
	else
	{
		m_sDCDFormatting = GetStringFromTable(sDirectCDPath, sExe, 200);
		m_sDCDRemoveWriteProtection = GetStringFromTable(sDirectCDPath, sExe, 203);
	}
	WK_TRACE(_T("DirectCD Format          '%s'\n"), m_sDCDFormatting);
	WK_TRACE(_T("DirectCD WriteProtection '%s'\n"), m_sDCDRemoveWriteProtection);
}
//////////////////////////////////////////////////////////////////////
CString CBackupDialog::FindDirectCDPath()
{
	CString sDirectCD(_T("directcd.exe"));
	CString sPathToDirectCD(_T("C:\\Programme\\Adaptec\\DirectCD"));
	CString sRet = sPathToDirectCD;

	if(!FileExists(sPathToDirectCD, sDirectCD))
	{
		CString sPath(_T("C:\\"));
		CString sLocation;

		//ist File mehrfach vorhanden, wird nur das erste gefunden, nicht mehrere
		SearchFileRecursiv(sPath, sDirectCD, sLocation);
		if (!sLocation.IsEmpty())
		{
			WK_TRACE(_T("Found DirectCD %s\n"), sLocation);
			sRet = sLocation.Left(sLocation.GetLength() - sDirectCD.GetLength() -1);
		}
	}

	return sRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::FileExists(CString sPath, CString sFileName)
{
	return DoesFileExist(sPath+_T("\\")+sFileName);
}
///////////////////////////////////////////////////////////////////////
//gets the string on the delivered position (iStringID) from the sFile
CString CBackupDialog::GetStringFromTable(CString sPath, CString sFile, int iStringID)
{
	CString s(_T(""));
	CString sText(_T(""));

	HMODULE hFile = LoadLibraryEx(sPath + _T("\\") + sFile,NULL,0);

	if(hFile != NULL)
	{
		LoadString(hFile,iStringID,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
	}

	FreeLibrary(hFile);
	return sText;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::AddSequence(CIPCSequenceRecord* pSequence)
{
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::AddSequence\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::DeleteSequences(WORD wArchivNr)
{
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::DeleteSequences\n"));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::DeleteSequence(CIPCSequenceRecord* pSequence)
{
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::DeleteSequence\n"));
	BOOL bRet = FALSE;
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CBackupDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CBackupDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CBackupDialog message handlers
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::OnDestroy() 
{
	CWK_Dialog::OnDestroy();
	
	m_pParent->m_pBackupDialog = NULL;
	
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == ID_BACKUP_CONFIRM)
	{
		WORD wSequenceNr = LOWORD(lParam);
		WORD wArchiveNr = HIWORD(lParam);
		BackupConfirmed(wArchiveNr, wSequenceNr);
		return TRUE;
	}
	else if (wParam == ID_BACKUP_CANCEL_CONFIRM)
	{
		CancelBackupConfirmed((DWORD)lParam);
		return TRUE;
	}
	else if (wParam == ID_BACKUP_ERROR)
	{
		BackupError(lParam);
		return TRUE;
	}
	else if (wParam == ID_DRIVE_INFO)
	{
		// GF Yet not implemented, change InitDrives first!
		OnDrivesInfo();
		return TRUE;
	}
	
	return CWK_Dialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	TRACE(_T("OnDeviceChange\n"));
	if (m_bHandleDeviceChange)
	{
		if (theApp.IsNT())
		{
			return OnDeviceChangeNT(nEventType,dwData);
		}
		else
		{
			return OnDeviceChange95(nEventType,dwData);
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::OnDeviceChangeNT( UINT nEventType, DWORD dwData )
{
	if (nEventType == DBT_DEVICEARRIVAL)
	{
		// formatierte CD/R eingelegt
		HandleDeviceChange(TRUE, TRUE, dwData);//FALSE);
	}
	else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
	{
		// unformatierte CD eingelegt
		// oder CD/ entfernt
		HandleDeviceChange(FALSE, TRUE, dwData);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::OnDeviceChange95( UINT nEventType, DWORD dwData )
{
	PDEV_BROADCAST_HDR pHdr = NULL;
	
	if (nEventType == DBT_DEVICEARRIVAL)
	{
		// Wurde CD/R eingelegt ?
		pHdr = (PDEV_BROADCAST_HDR)dwData;
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			// ja CD/R wurde eingelegt
			WK_TRACE(_T("OnDeviceChange95 DBT_DEVICEARRIVAL\n"));
			HandleDeviceChange(TRUE,TRUE,dwData);
		}
	}
	else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
	{
		// Wurde CD/R entfernt ?
		pHdr = (PDEV_BROADCAST_HDR)dwData;
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			// ja CD/R wurde entfernt
			WK_TRACE(_T("OnDeviceChange95 DBT_DEVICEREMOVECOMPLETE\n"));
			HandleDeviceChange(FALSE,FALSE,dwData);
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::CheckSpaceOnDestination(BOOL bFormat /*= TRUE*/)
{
	BOOL bEnable = FALSE;
	BOOL bEnableNewCDR = FALSE;

	if (m_pDrive)
	{
		if (IsCDorDVD(m_pDrive->GetType()))
		{
			if (bFormat && m_pDrive->GetFSProducer() == FSP_UDF_ADAPTEC_DIRECTCD)
			{
				WK_TRACE(_T("try to format DirectCD %s %d/%d free\n"),
					m_pDrive->GetRootString(),
					m_pDrive->GetFreeMB(),m_pDrive->GetMB());

				if (ExecuteExplorerMenu(m_hWnd,
										m_pDrive->GetRootString(),
										m_sDCDFormatting))
				{
					WK_TRACE(_T("Direct CD %s formatted\n"),
						m_pDrive->GetRootString());
				}
				else
				{
					WK_TRACE(_T("try to remove write protection DirectCD %s %d/%d free\n"),
						m_pDrive->GetRootString(),
						m_pDrive->GetFreeMB(),m_pDrive->GetMB());
					if (ExecuteExplorerMenu(m_hWnd,
											m_pDrive->GetRootString(),
											m_sDCDRemoveWriteProtection))
					{
						WK_TRACE(_T("Direct CD %s write protection removed\n"),
							m_pDrive->GetRootString());
					}
				}

/*
				if (m_pDrive->GetMB()==0)
				{
					WK_TRACE(_T("Direct CD %s formatieren\n"),
						m_pDrive->GetRootString());
					ExecuteExplorerMenu(m_hWnd,
										m_pDrive->GetRootString(),
										m_sDCDFormatting);
				}
				else
				{
					WK_TRACE(_T("Direct CD %s Schreibschutz aufheben\n"),
						m_pDrive->GetRootString());
					ExecuteExplorerMenu(m_hWnd,
										m_pDrive->GetRootString(),
										m_sDCDRemoveWriteProtection);
				}
*/
			}
			bEnableNewCDR = TRUE;
		}
		m_pDrive->CheckSpace();

		if (m_i64BytesToCopy.GetInMB() > m_pDrive->GetFreeMB())
		{
			bEnable = FALSE;	
		}
		else
		{
			bEnable = TRUE;	
		}
	}

	return bEnable;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::AskForCDRFinalize()
{
	if (IDYES == AfxMessageBox(IDS_ASK_CLOSE_CDR, MB_YESNO))
	{
		if (m_pDrive->GetFSProducer() == FSP_UDF_CEQUADRAT_PACKETCD)
		{
			CString s;
			s = m_pDrive->GetFSVersion();
			WK_TRACE(_T("finalizing disc with CeQuadrat PacketCD %s\n"),s);
			if (s>_T("2.5"))
			{
				if (ExecuteExplorerMenu(m_hWnd,
										m_pDrive->GetRootString(),
										_T("&Finalisiere...")))
				{
					return TRUE;
				}
			}
			else
			{
				if (ExecuteExplorerMenu(m_hWnd,
										m_pDrive->GetRootString(),
										_T("&Finalize disc...")))
				{
					return TRUE;
				}
			}
		}
		else if (m_pDrive->GetFSProducer() == FSP_UDF_ADAPTEC_DIRECTCD)
		{
			CString s;
			s = m_pDrive->GetFSVersion();
			WK_TRACE(_T("finalizing disc with Adaptec DirectCD %s\n"),s);
			AfxMessageBox(IDP_EJECT_DIRECTCD);
			return TRUE;
		}
		else
		{
			WK_TRACE(_T("finalizing disc with unknown FS Producer\n"));
			return FALSE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::OnConfirmBackupByTime(WORD wUserData,
											 DWORD dwID,
											 const CSystemTime& stStartLessMaxCDCapacity,
											 const CSystemTime& stEndMoreMaxCDCapacity,
											 WORD wFlags,
											 CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmBackupByTime\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnIndicateError(DWORD dwCmd,
									CSecID id, 
									CIPCError errorCode,
									int iUnitCode,
									const CString &sErrorMsg)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmBackupByTime\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnConfirmVolumeInfos(int iNumDrives,
												    const CIPCDrive drives[])
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmVolumeInfos\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnIndicateSessionProgress\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnConfirmSession(DWORD dwSessionID)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmSession\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnConfirmCancelSession(DWORD dwSessionID)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmCancelSession\n"));
}
//////////////////////////////////////////////////////////////////////
void CBackupDialog::OnConfirmDeleteVolume(const CString& sVolume)
{
	// CAVEAT: In CIPC thread!
	WK_TRACE_WARNING(_T("NOT OVERWRITTEN CBackupDialog::OnConfirmDeleteVolume\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialog::OnDrivesInfo()
{
	// Overwritten by derived class or info is ignored
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::IsCDorDVD(UINT uSystemType)
{
	BOOL bReturn = FALSE;
	switch (uSystemType)
	{
		case DRIVE_CDROM:
		case DRIVE_CD_R:
		case DRIVE_CD_RW:
		case DRIVE_DVD_ROM:
		case DRIVE_DVD_RAM:
		case DRIVE_DVD_R:
		case DRIVE_DVD_RW:
			bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::IsWritableCDorDVD(UINT uSystemType)
{
	BOOL bReturn = FALSE;
	switch (uSystemType)
	{
		case DRIVE_CD_R:
		case DRIVE_CD_RW:
		case DRIVE_DVD_RAM:
		case DRIVE_DVD_R:
		case DRIVE_DVD_RW:
			bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialog::IsRewritableCDorDVD(UINT uSystemType)
{
	BOOL bReturn = FALSE;
	switch (uSystemType)
	{
		case DRIVE_CD_RW:
		case DRIVE_DVD_RAM:
		case DRIVE_DVD_RW:
			bReturn = TRUE;
	}
	return bReturn;
}
