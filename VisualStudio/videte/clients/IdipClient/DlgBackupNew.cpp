// DlgBackupNew.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgBackupNew.h"

#include "Server.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewArchive.h"
#include "CopyReadOnlyVersion.h"
#include "MainFrm.h"

#include "DlgSelectBackupDrive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BACKUP_TEMP_DRIVE_RESERVED_MB				300	// MB
#define BACKUP_READER_SOFTWARE_MB					10	// MB
#define BACKUP_DESTINATION_RESERVED_MB				40	// MB
#define ABORT_CALCULATION_AT_SIZE_DIFFERENCE_KB		50	// kB


/////////////////////////////////////////////////////////////////////////////
// CDlgBackupNew dialog
/////////////////////////////////////////////////////////////////////////////
CDlgBackupNew::CDlgBackupNew(CServer* pServer, CViewIdipClient* pParent /*=NULL*/)
	: CDlgBackup(CDlgBackupNew::IDD, pServer, pParent)
{
	//{{AFX_DATA_INIT(CDlgBackupNew)
	m_sSize = _T("");
	m_sVelocity = _T("");
	m_sBackupName = _T("");
	m_sMBtoCopy = _T("");
	m_sStatus = _T("");
	m_sTime = _T("");
	m_sFree = _T("");
	m_sType = _T("");
	//}}AFX_DATA_INIT

	m_dwSessionID = GetTickCount();
	m_enumStatus = BS_NORMAL;
	
	m_uTimerRO = 0;
	m_uTimerDataCarrier = 0;
	m_uTimerDriveInfo = 0;

	// initialise all SystemTimes with current time to avoid misbehaviour
	m_stFirstDBImage.GetLocalTime();			// will be calculated
	m_stStartDateTime.GetLocalTime();			// will be calculated
	m_stEndDateTime.GetLocalTime();				// will be calculated
	m_stEndLessMaxCapacity.GetLocalTime();		// will be calculated
	m_stEndMoreMaxCapacity.GetLocalTime();		// will be calculated
	m_stEndDateTimeConfirmed.GetLocalTime();	// will be calculated

	m_i64DestCapacity = 0;
	m_i64LessMaxCapacity = 0;
	m_i64MoreMaxCapacity = 0;

	m_bStartDateTimeEntered = FALSE;
	m_bEndCalculationPossible = FALSE;
	m_bNeedCalculationSize = FALSE;
	m_bContinuationPossible = FALSE;

	CWK_Profile prof;
	CString sSection, sKey;
	sSection = SECTION_IDIP_CLIENT;
	sKey = _T("BackupReaderSoftwareMB");
	m_dwReaderSoftwareMB = (DWORD)prof.GetInt(sSection, sKey, BACKUP_READER_SOFTWARE_MB);
	prof.WriteInt(sSection, sKey, m_dwReaderSoftwareMB);
	sKey = _T("BackupDestinationReservedMB");
	m_dwReservedSpaceMB = (DWORD)prof.GetInt(sSection, sKey, BACKUP_DESTINATION_RESERVED_MB);
	prof.WriteInt(sSection, sKey, m_dwReservedSpaceMB);
	sKey = _T("BackupAbortCalculationAtkB");
	m_dwAbortAtDifference_kB = (DWORD)prof.GetInt(sSection, sKey, ABORT_CALCULATION_AT_SIZE_DIFFERENCE_KB);
	prof.WriteInt(sSection, sKey, m_dwAbortAtDifference_kB);
	sKey = _T("BackupTempDriveReservedMB");
	m_dwTempDriveReservedSpaceMB = (DWORD)prof.GetInt(sSection, sKey, BACKUP_TEMP_DRIVE_RESERVED_MB);
	prof.WriteInt(sSection, sKey, m_dwReservedSpaceMB);

	GetTempPath(_MAX_PATH, m_sTempLocation.GetBuffer(_MAX_PATH));
	m_sTempLocation.ReleaseBuffer();
	sKey = _T("BackupTempLocation");
	CString sTemp;
	sTemp = prof.GetString(sSection, sKey, m_sTempLocation);
	if(sTemp != m_sTempLocation)
	{
		m_sTempLocation = sTemp;
		prof.WriteString(sSection, sKey, m_sTempLocation);
	}


	m_sBackupName.LoadString(IDS_TITLE_BACKUP);
	CSystemTime st;
	st.GetLocalTime();
	m_sBackupName += _T(" ") + st.GetDateTime();

	m_bIsDataCarrierBackup	= FALSE;
	m_iProgressPercent		= 0;
	m_bCleanRW				= FALSE;

	m_bIsCancelled = FALSE;
	m_bTypeIsHighlighted = FALSE;
	
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CDlgBackupNew::~CDlgBackupNew()
{

}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::DoDataExchange(CDataExchange* pDX)
{
	CDlgBackup::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBackupNew)
	DDX_Control(pDX, IDC_CALCULATE_SIZE, m_ctrlCalculateSize);
	DDX_Control(pDX, IDC_CALCULATE_END, m_ctrlCalculateEnd);
	DDX_Control(pDX, IDC_RESET_START_END, m_ctrlResetStartEnd);
	DDX_Control(pDX, IDC_END_TIME, m_ctrlEndTime);
	DDX_Control(pDX, IDC_END_DATE, m_ctrlEndDate);
	DDX_Control(pDX, IDC_START_TIME, m_ctrlStartTime);
	DDX_Control(pDX, IDC_START_DATE, m_ctrlStartDate);
	DDX_Control(pDX, IDC_LIST_ARCHIVES, m_ctrlArchives);
	DDX_Control(pDX, IDC_CDR, m_ctrlCDR);
	DDX_Control(pDX, IDC_TXT_STATE, m_staticState);
	DDX_Control(pDX, IDC_TXT_SELECTED_MB, m_staticToCopy);
	DDX_Control(pDX, IDC_TXT_TYPE, m_staticType);
	DDX_Control(pDX, IDC_TXT_MB_FREE, m_staticFree);
	DDX_Control(pDX, IDC_CLOSE, m_ctrlClose);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_COMBO_DESTINATION, m_comboDestination);
	DDX_Text(pDX, IDC_SIZE_MB, m_sSize);
	DDX_Text(pDX, IDC_VELOCITY_KBS, m_sVelocity);
	DDX_Text(pDX, IDC_EDIT_BACKUP_NAME, m_sBackupName);
	DDX_Text(pDX, IDC_TXT_SELECTED_MB, m_sMBtoCopy);
	DDX_Text(pDX, IDC_TXT_STATE, m_sStatus);
	DDX_Text(pDX, IDC_TIME_S, m_sTime);
	DDX_Text(pDX, IDC_TXT_MB_FREE, m_sFree);
	DDX_Text(pDX, IDC_TXT_TYPE, m_sType);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgBackupNew, CDlgBackup)
	//{{AFX_MSG_MAP(CDlgBackupNew)
	ON_BN_CLICKED(IDC_CALCULATE_SIZE, OnCalculateSize)
	ON_BN_CLICKED(IDC_CALCULATE_END, OnCalculateEnd)
	ON_BN_CLICKED(IDC_RESET_START_END, OnResetStartEnd)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTINATION, OnSelchangeComboDestination)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CDR, OnCdr)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_END_DATE, OnDatetimechangeEndDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_END_TIME, OnDatetimechangeEndTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_START_DATE, OnDatetimechangeStartDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_START_TIME, OnDatetimechangeStartTime)
	ON_EN_CHANGE(IDC_EDIT_BACKUP_NAME, OnChangeEditBackupName)
	ON_MESSAGE(WM_USER_UPDATE_AND_ENABLE_CONTROLS, OnUpdateAndEnableControls)
	ON_MESSAGE(WM_USER_HANDLE_CD_AND_DVD_INFO, OnHandleCDandDVDInfo)
	ON_MESSAGE(WM_USER_BACKUP_DATA_CARRIER_FINISHED, OnBackupDataCarrierFinished)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::AddArchive(CIPCArchivRecord* pArchiv)
{
	if (pArchiv)
	{
		Lock(_T(__FUNCTION__));
		m_ArchivesToCopy.Add(new CIPCArchivRecord(*pArchiv));
		CString s;
		s.Format(_T("%d, %s"),pArchiv->GetID(),pArchiv->GetName());
		int i = m_ctrlArchives.AddString(s);
		m_ctrlArchives.SetItemData(i,pArchiv->GetID());
		Unlock();

		ArchivesChanged();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences)
{
	BOOL bRet = FALSE;

	Lock(_T(__FUNCTION__));
	for (int i=0;i<m_ArchivesToCopy.GetSize();i++)
	{
		CIPCArchivRecord* pPivot = m_ArchivesToCopy.GetAtFast(i);
		if (pPivot->GetID()==wArchivNr)
		{
			// delete it from list
			RemoveArchiveFromList(wArchivNr);
			m_ArchivesToCopy.RemoveAt(i);
			WK_DELETE(pPivot);
			bRet = TRUE; 
			break;
		}
	}
	Unlock();

	ArchivesChanged();
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr)
{
	WK_TRACE(_T("Database backup confirmed a=%d, s=%d\n"), wArchiveNr, wSequenceNr);

	if (wArchiveNr!=0)
	{
		// sequence from an archive
		if (wSequenceNr ==0)
		{
			CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
			// the last one of the archive
			CIPCArchivRecord* pArchiv = m_ArchivesToCopy.GetArchiv(wArchiveNr);
			if (pArchiv)
			{
				RemoveArchiveFromList(wArchiveNr);
				m_ArchivesToCopy.RemoveArchiv(wArchiveNr);
			}
		}
	}

	if (	(wSequenceNr==0)
		&& 	(wArchiveNr==0) )
	{
		BackupDatabaseFinished();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CancelBackupConfirmed(DWORD dwSessionID)
{
	// gf todo Krampf wegen eines WORDs!
	if (dwSessionID == LOWORD(m_dwSessionID))
	{
		WK_TRACE(_T("Session cancel confirmed %08lx\n"), m_dwSessionID);
	}
	else
	{
		WK_TRACE_WARNING(_T("Session cancel confirmed WRONG SessionID %08lx %08x\n"),
													dwSessionID, m_dwSessionID);
	}
	BackupCancelled(dwSessionID);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::BackupError(LPARAM lParam)
{
	m_sStatus.LoadString(IDS_BACKUP_FAILED);
	m_staticState.SetWindowText(m_sStatus);
	m_enumStatus = BS_NORMAL;
	EnableControls();

	if (lParam == 2)
	{
		AfxMessageBox(IDP_BACKUP_INITIALIZE_FAILED,MB_OK|MB_ICONSTOP);
	}
	else if (lParam == 3)
	{
		AfxMessageBox(IDP_BACKUP_COPY_FAILED,MB_OK|MB_ICONSTOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData)
{
	// Implies that only CD's or DVD' are inserted/removed
	// gf todo What about USB volumes and others?
	if (bInsert)
	{
		WK_TRACE(_T("CD/DVD inserted\n"));
	}
	else
	{
		WK_TRACE(_T("CD/DVD removed or Non-formatted CD/DVD inserted\n"));
	}
	WK_TRACE(_T("Checking backup drive\n"));

	CString sDriveRoot;
	BOOL bHandleIt = FALSE;

	Lock(_T(__FUNCTION__));
	// Do not handle it if DataCarrier is working,
	// maybe open/close while deleting RW
	// or end of burning
	if (   m_pDrive
		&& dwData
		&& (m_enumStatus != BS_RUNNING_DATACARRIER)
		)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			PDEV_BROADCAST_VOLUME pDBV = (PDEV_BROADCAST_VOLUME)dwData;
			DWORD dwDriveMask = pDBV->dbcv_unitmask;
			WORD wFlags = pDBV->dbcv_flags; // DBTF_MEDIA or DBTF_NET
			if (wFlags == DBTF_MEDIA)
			{
				for (int i = 2; i < 32; i++)
				{
					if (dwDriveMask & 1<<i)
					{
						sDriveRoot.Format(_T("%c:\\"), (char)(_T('a')+i));
						if (m_pDrive->GetRootString() == sDriveRoot)
						{
							bHandleIt = TRUE;	
							break;
						}
					}
				}
			}
		}
	}
	Unlock();

	if (bHandleIt)
	{	
		Sleep(500);
		CheckDriveSpace(bFormat);
		EnableControls();
	}
	else
	{
		WK_TRACE(_T("HandleDeviceChange not handled\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnConfirmBackupByTime(WORD wUserData,
											 DWORD dwID,
											 const CSystemTime& stStart,
											 const CSystemTime& stEnd,
											 WORD wFlags,
											 CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	if (wUserData == LOWORD(m_dwSessionID))
	{
		if (m_enumStatus == BS_CALCULATING_END_STARTED)
		{
			FirstCalculationEnd(dwID, stStart, stEnd, i64Size);
		}
		else if (m_enumStatus == BS_CALCULATING_END)
		{
			NextCalculationEnd(dwID, stStart, stEnd, i64Size);
		}
		else if (m_enumStatus == BS_CALCULATING_SIZE)
		{
			SizeCalculation(dwID, stStart, stEnd, i64Size);
		}
		else if (   m_enumStatus == BS_RUNNING_DATABASE
				 && (   (wFlags == BBT_PROGRESS)
					 || (wFlags == BBT_FINISHED)
					 )
				)
		{
			BackupProgress(dwID, i64Size);
		}
		else
		{
			// OOPS, whats then?
			WK_TRACE_ERROR(_T("OnConfirmBackupByTime WRONG Status\n"));
			WK_TRACE_ERROR(_T("   dwID:%08lx Start:%s End:%s wFlag:0x%02x iSize(MB): %lu\n"),
				dwID, stStart.GetDateTime(), stEnd.GetDateTime(), wFlags, i64Size.GetInMB());
		}
	}
	else
	{
		// gf is possible, if the user checks more archives
		WK_TRACE_WARNING(_T("OnConfirmBackupByTime WRONG BackupID xxxx%04x <=> %08lx\n"),
															wUserData, m_dwSessionID);
		WK_TRACE_WARNING(_T("   dwID:%lu Start:%s End:%s wFlag:0x%02x iSize(MB): %lu\n"),
			dwID, stStart.GetDateTime(), stEnd.GetDateTime(), wFlags, i64Size.GetInMB());
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnIndicateError(DWORD dwCmd,
									   CSecID id, 
									   CIPCError errorCode,
									   int iUnitCode,
									   const CString &sErrorMsg)
{
	// CAVEAT: In CIPC thread!
	// Called from DataCarrier only, Database has own routines
	WK_TRACE(_T("CDlgBackupNew OnIndicateError: cmd %s, id %08lx/%08lx, error %d/%d %s\n"),
		CIPC::NameOfCmd(dwCmd), id.GetID(), m_dwSessionID, errorCode, iUnitCode, sErrorMsg);

	switch (dwCmd)
	{
		case CIPC_DC_REQUEST_VOLUME_INFOS:
		case CIPC_DC_CONFIRM_VOLUME_INFOS:
			// no info about volume
			// normally it means, no medium found
			// gf todo what to do, if other cause?
			m_enumStatus = BS_NORMAL;
			m_sType.LoadString(IDS_NO_MEDIUM_FOUND);
			CalcDestinationCapacity();
			PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
			break;
		case CIPC_DC_REQUEST_SESSION:
		case CIPC_DC_CONFIRM_SESSION:
			// gf todo what to do?
			// First solution:
			// clear status and set error message
			m_enumStatus = BS_NORMAL;
			m_sStatus.LoadString(IDS_BACKUP_FAILED);
			CalcDestinationCapacity();
			PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
			break;
		case CIPC_DC_REQUEST_CANCEL_SESSION:
		case CIPC_DC_CONFIRM_CANCEL_SESSION:
			// gf todo what to do?
			// At least clear cancel flag and enable button again
			// Do nothing else, process will go on - I think...
			m_bIsCancelled = FALSE;
			PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
			break;
		case CIPC_DC_REQUEST_DELETE_VOLUME:
		case CIPC_DC_CONFIRM_DELETE_VOLUME:
			// gf todo what to do
			// As it will not be called yet,
			// it should not be happen
			WK_TRACE_ERROR(_T("CDlgBackupNew OnIndicateError Delete Volume, WHY?\n"));
			break;
		default:
			WK_TRACE_ERROR(_T("CDlgBackupNew OnIndicateError not handled\n"));
			break;
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[])
{
	// CAVEAT: In CIPC thread!
	CAutoCritSec acs(&m_ArchivesToCopy.m_cs);

	TRACE(_T("OnConfirmVolumeInfos %i drives\n"), iNumDrives);
	m_enumStatus = BS_NORMAL;
	if (m_pDrive && iNumDrives)
	{
		BOOL bFound =FALSE;
		const CIPCDrive* pDrive = NULL;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<iNumDrives ; i++)
		{
			pDrive = &drives[i];
			TRACE(_T("ACDC Volumeinfo %s\n"),pDrive->GetRootString());
			if (m_pDrive->GetRootString() == pDrive->GetRootString())
			{
				bFound =TRUE;
				m_bIsDataCarrierBackup = TRUE;
				break;
			}
		}
		if (bFound)
		{
			// gf TODO switch to total get functions
			m_pDrive->SetType(pDrive->GetType());

			switch (m_pDrive->GetType())
			{
			case DRIVE_REMOVABLE:
				// The disk can be removed from the drive; should not happen here
				m_sType.LoadString(IDS_DRIVE_FD);
				break;
			case DRIVE_FIXED:
				// The disk cannot be removed from the drive; should not happen here
				m_sType.LoadString(IDS_DRIVE_HD);
				break;
			case DRIVE_REMOTE:
				// The drive is a remote (network) drive; should not happen here
				m_sType.LoadString(IDS_DRIVE_NET);
				break;
			case DRIVE_CDROM:
				// The medium is a CD-ROM or already written CD-R
				m_sType.LoadString(IDS_MEDIUM_CD_ROM);
				break;
			case DRIVE_CD_R:
				// The medium is a fresh, writable CD-R
				m_sType.LoadString(IDS_MEDIUM_CD_R);
				break;
			case DRIVE_CD_RW:
				m_sType.LoadString(IDS_MEDIUM_CD_RW);
				break;
			case DRIVE_DVD_ROM:
				m_sType.LoadString(IDS_MEDIUM_DVD_ROM);
				break;
			case DRIVE_DVD_RAM:
				m_sType.LoadString(IDS_MEDIUM_DVD_RAM);
				break;
			case DRIVE_DVD_R:
				m_sType.LoadString(IDS_MEDIUM_DVD_R);
				break;
			case DRIVE_DVD_RW:
				m_sType.LoadString(IDS_MEDIUM_DVD_RW);
				break;
			}

			ULARGE_INTEGER uliFree, uliTotal, uliMBDummy;
			uliMBDummy.QuadPart = pDrive->GetFreeMB();
			uliFree.QuadPart = uliMBDummy.QuadPart * 1024 * 1024;
			m_pDrive->SetFreeBytesAvailableToCaller(uliFree);

			uliMBDummy.QuadPart = pDrive->GetMB();
			uliTotal.QuadPart = uliMBDummy.QuadPart * 1024 * 1024;
			m_pDrive->SetTotalNumberOfBytes(uliTotal);

			TRACE(_T("OnConfirmVolumeInfos Free %lu Total %lu MB\n"), m_pDrive->GetFreeMB() , m_pDrive->GetMB());
			if (   (uliFree.QuadPart != uliTotal.QuadPart)
				&& IsRewritableCDorDVD(m_pDrive->GetType())
				)
			{
				PostMessage(WM_USER_HANDLE_CD_AND_DVD_INFO, NULL, (LPARAM)m_pDrive->GetType());
			}
			else
			{
				CalcDestinationCapacity();
				PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
				if (m_bNeedCalculationSize)
				{
					// Force calculation
					PostMessage(WM_COMMAND,
								MAKEWPARAM(m_ctrlCalculateEnd.GetDlgCtrlID(), BN_CLICKED),
								(LPARAM)m_ctrlCalculateEnd.m_hWnd);
				}
			}
		}
		else
		{
			WK_TRACE(_T("backup medium not found with acdc %s\n"),m_pDrive->GetRootString());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	// CAVEAT: In CIPC thread!

	if (dwSessionID == m_dwSessionID)
	{
		TRACE(_T("OnIndicateSessionProgress %08lx %i%%\n"), dwSessionID, iProgress);

		if (m_bCleanRW)
		{
			m_iProgressPercent = iProgress;
			if (m_iProgressPercent == 100)
			{
				m_bCleanRW = FALSE;
				m_sStatus.LoadString(IDS_BACKUP_RUNNING);
				m_i64BytesCopied = 0;
				m_iProgressPercent = 0;
				// Writing lead in will takes approx. 40 seconds without update
				// so start timer to indicate progress to user 
				// Estimate time to burn for quad speed 600 kB/s
				m_dwCountDownInSeconds = m_i64BytesToCopy.GetInMB() * 1024 / 600;
				m_uTimerDataCarrier = SetTimer(TIMER_FOR_DATA_CARRIER, 1000, NULL);
			}
		}
		else
		{
			// At first call we can not kill the timer, because the next update is still far away
			// do not even calc info, will be unserious too
			if (m_iProgressPercent != 0)
			{
				m_iProgressPercent = iProgress;
				// from next call on it will update continuosly
				// so kill timer then
				if (m_uTimerDataCarrier)
				{
					if (KillTimer(m_uTimerDataCarrier))
					{
						m_uTimerDataCarrier = 0;
					}
				}

				m_i64BytesCopied = (m_i64BytesToCopy.GetInt64() / 100 * m_iProgressPercent);
				CalculateInfo();

				// at the end there is some rounding difference, so ... satisfy the user
				if (m_iProgressPercent == 100)
				{
					m_i64BytesCopied = m_i64BytesToCopy;
				}
				CalculateInfo();

				if (m_iProgressPercent == 100)
				{
					// Writing lead out will take approx. 20 seconds without update
					// so start timer to indicate progress to user 
					m_dwCountDownInSeconds = 30;
					m_uTimerDataCarrier = SetTimer(TIMER_FOR_DATA_CARRIER, 1000, NULL);
				}
			}
			else
			{
				m_iProgressPercent = iProgress;
			}
		}

		PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
	}
	else
	{
		WK_TRACE_ERROR(_T("OnIndicateSessionProgress WRONG BackupID %08lx %08lx\n"), dwSessionID, m_dwSessionID);
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnConfirmSession(DWORD dwSessionID)
{
	// CAVEAT: In CIPC thread!
	if (dwSessionID == m_dwSessionID)
	{
		WK_TRACE(_T("Session confirmed %08lx\n"), dwSessionID);
		PostMessage(WM_USER_BACKUP_DATA_CARRIER_FINISHED);
	}
	else
	{
		WK_TRACE_ERROR(_T("Session confirmed WRONG BackupID %08lx %08lx\n"), dwSessionID, m_dwSessionID);
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnConfirmCancelSession(DWORD dwSessionID)
{
	// CAVEAT: In CIPC thread!
	if (dwSessionID == m_dwSessionID)
	{
		WK_TRACE(_T("Session cancel confirmed %08lx\n"), m_dwSessionID);
	}
	else
	{
		WK_TRACE_WARNING(_T("Session cancel confirmed WRONG SessionID %08lx %08x\n"),
													dwSessionID, m_dwSessionID);
	}
	BackupCancelled(dwSessionID);
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnConfirmDeleteVolume(const CString& sVolume)
{
	// CAVEAT: In CIPC thread!
	TRACE(_T("Delete Volume confirmed %s\n"), sVolume);
	m_enumStatus = BS_NORMAL;

	Lock(_T(__FUNCTION__));
	if (m_pDrive && (m_pDrive->GetRootString() == sVolume))
	{
		CalcDestinationCapacity();
	}
	Unlock();

	PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CalculateInfo()
{
	DWORD dwKBToCopy = (DWORD)(m_i64BytesToCopy.GetInt64() / 1024);
	DWORD dwKBCopied = (DWORD)(m_i64BytesCopied.GetInt64() / 1024);
	m_sSize.Format(_T("%d / %d kBytes"), dwKBCopied, dwKBToCopy);

	DWORD dwNow		= GetTickCount();
	DWORD dwElapsed = dwNow - m_dwStartTime;

	if (dwElapsed!=0)
	{
		m_dwBytesPerSecond = (DWORD)((m_i64BytesCopied.GetInt64() * 1000) / dwElapsed);
	}

	m_sVelocity.Format(_T("%d kByte/s"),m_dwBytesPerSecond / 1024);

	CIPCInt64 iBytesStillToCopy = m_i64BytesToCopy;
	iBytesStillToCopy -= m_i64BytesCopied;

	if (m_dwBytesPerSecond!=0)
	{
		m_dwCountDownInSeconds = (DWORD)(iBytesStillToCopy.GetInt64() / m_dwBytesPerSecond);
	}

	m_sTime.Format(_T("%02d:%02d:%02d"), m_dwCountDownInSeconds / 3600,
									 (m_dwCountDownInSeconds / 60) % 60,
									 m_dwCountDownInSeconds % 60);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::EnableControls()
{
	BOOL bEnableGeneral = FALSE;
	BOOL bEnableItem = FALSE;

	// first check for updated drive info
	if (   m_bDriveInfoChanged
		&& m_comboDestination.GetCount()>0
		&& (m_enumStatus == BS_NORMAL)
		)
	{
		SelectDrive();
	}

	bEnableItem = (m_enumStatus == BS_NORMAL);
	m_comboDestination.EnableWindow(bEnableItem && m_comboDestination.GetCount()>0);

	Lock(_T(__FUNCTION__));
	bEnableItem =    (m_enumStatus == BS_NORMAL)
				  && (m_pDrive)
				  && IsCDorDVD(m_pDrive->GetType());
	Unlock();

	m_ctrlCDR.EnableWindow(bEnableItem);

	bEnableGeneral =    (m_enumStatus == BS_NORMAL)
					 && (m_i64DestCapacity != 0);
	EnableTreeView(bEnableGeneral);

	Lock(_T(__FUNCTION__));
	bEnableItem =    bEnableGeneral
				  && (m_enumStatus == BS_NORMAL)
				  && m_ArchivesToCopy.GetSize();
	Unlock();

	m_ctrlStartDate.EnableWindow(bEnableItem);
	m_ctrlStartTime.EnableWindow(bEnableItem);
	m_ctrlEndDate.EnableWindow(bEnableItem);
	m_ctrlEndTime.EnableWindow(bEnableItem);

	bEnableItem =    bEnableGeneral
				  && m_bNeedCalculationSize;
	m_ctrlCalculateSize.EnableWindow(bEnableItem);

	bEnableItem =    bEnableGeneral
				  && m_bEndCalculationPossible;
	m_ctrlCalculateEnd.EnableWindow(bEnableItem);

	bEnableItem =    bEnableGeneral
				  && m_bStartDateTimeEntered
				  && (m_enumStatus == BS_NORMAL);
	m_ctrlResetStartEnd.EnableWindow(m_bStartDateTimeEntered);

	bEnableItem =    bEnableGeneral
				  && (m_i64BytesToCopy > 0)
				  && (m_i64BytesToCopy <= m_i64DestCapacity)
				  && !m_bNeedCalculationSize;

	m_ctrlOK.EnableWindow(bEnableItem);

	bEnableItem = (   (m_enumStatus != BS_NORMAL)
				   && (m_bIsCancelled == FALSE));
	m_ctrlCancel.EnableWindow(bEnableItem);

	bEnableItem = (m_enumStatus == BS_NORMAL);
	m_ctrlClose.EnableWindow(bEnableItem);

	// set default button
	if(m_ctrlOK.IsWindowEnabled())
	{
		SetDefID(IDOK);
	}
	else if (m_ctrlClose.IsWindowEnabled())
	{
		SetDefID(IDC_CLOSE);
	}
	else
	{
		SetDefID(IDCANCEL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::EnableTreeView(BOOL bEnable)
{
	theApp.GetMainFrame()->GetViewArchive()->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::Calculate(BOOL bContinue)
{
	m_bStartDateTimeEntered = FALSE;
	RequestFirstImageInDB();
	if (bContinue)
	{
		m_stStartDateTime = m_stEndDateTime;
	}
	else
	{
		m_stStartDateTime = m_stFirstDBImage;
	}
	m_ctrlStartDate.SetTime(&m_stStartDateTime);
	m_ctrlStartTime.SetTime(&m_stStartDateTime);

//	RequestLastImageInDB();
	m_stEndDateTime.GetLocalTime();
	m_ctrlEndDate.SetTime(&m_stEndDateTime);
	m_ctrlEndTime.SetTime(&m_stEndDateTime);
	StartCalculationForEndTime();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::RequestFirstImageInDB()
{
	if (GetServer())
	{
		WORD wArchiveNr;
		CIPCSequenceRecord* pSequence = NULL;
		CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
		m_stFirstDBImage.GetLocalTime();
		for (int i = 0 ; i < m_ArchivesToCopy.GetSize() ; i++)
		{
			wArchiveNr = m_ArchivesToCopy.GetAtFast(i)->GetID();
			CServer *pServer = GetServer();
			if (pServer)
			{
				pSequence = pServer->GetFirstSequenceRecord(wArchiveNr);
				if (   pSequence
					&& (pSequence->GetTime() < m_stFirstDBImage)
					)
				{
					m_stFirstDBImage = pSequence->GetTime();
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*
void CDlgBackupNew::RequestLastImageInDB()
{
	CServer *pServer = GetServer();
	if (pServer)
	{
		m_stLastDBImage = CSystemTime();
		WORD wArchiveNr;
		CIPCSequenceRecord* pSequence = NULL;
		CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
		for (int i = 0 ; i < m_ArchivesToCopy.GetSize() ; i++)
		{
			wArchiveNr = m_ArchivesToCopy.GetAtFast(i)->GetID();
			pSequence = pServer->GetLastSequenceRecord(wArchiveNr);
			if (   pSequence
				&& (pSequence->GetTime() < m_stLastDBImage)
				)
			{
				m_stLastDBImage = pSequence->GetTime();
			}
		}
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::StartCalculationForEndTime()
{
	m_bEndCalculationPossible = FALSE;
	m_bNeedCalculationSize = FALSE;
	m_bContinuationPossible = FALSE;
	WK_TRACE(_T("StartCalculationForEndTime, NO Continuation\n"));

	m_stEndDateTimeConfirmed = 0;

	WORD wFlags = BBT_CALCULATE_END;
	m_enumStatus = BS_CALCULATING_END_STARTED;
	StartCalculation(wFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::StartCalculationForSize()
{
	m_bNeedCalculationSize = FALSE;

	WORD wFlags = BBT_CALCULATE;
	m_enumStatus = BS_CALCULATING_SIZE;
	StartCalculation(wFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::StartCalculation(WORD wFlags)
{
	CServer *pServer = GetServer();
	if (pServer)
	{
		m_i64LessMaxCapacity = 0;
		m_i64MoreMaxCapacity = 0;

		Lock(_T(__FUNCTION__));
		int iNrOfArchives = m_ArchivesToCopy.GetSize();
		DWORD* pArchives = NULL;
		pArchives = new DWORD[iNrOfArchives];
		//changes for VisualStudio 2005
		int i = 0;
		for(i=0; i<iNrOfArchives; i++)
		{
			pArchives[i] = MAKELONG(0, m_ArchivesToCopy.GetAtFast(i)->GetID());
		}
		Unlock();

		// New search, new ID
		m_dwSessionID = GetTickCount();
		CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
		// gf todo Krampf wegen eines WORDs!

		pDatabase->DoRequestBackupByTime(	LOWORD(m_dwSessionID),
											_T(""),
											_T(""),
											iNrOfArchives,
											pArchives,
											m_stStartDateTime,
											m_stEndDateTime,
											wFlags,
											m_i64DestCapacity.GetInMB());
		TRACE(_T("*** StartCalculation Archives %08lx : %i Start:%s End:%s iSize(MB): %lu ***\n"),
																m_dwSessionID,
																iNrOfArchives,
																m_stStartDateTime.GetDateTime(),
																m_stEndDateTime.GetDateTime(),
																m_i64DestCapacity.GetInMB());
		m_sStatus.LoadString(IDS_BACKUP_CALCULATING);
		m_staticState.SetWindowText(m_sStatus);
		WK_DELETE_ARRAY(pArchives);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::StartDatabaseBackup()
{
	m_enumStatus = BS_RUNNING_DATABASE;
	m_sStatus.LoadString(IDS_BACKUP_RUNNING);

	Lock(_T(__FUNCTION__));
	CString sDestination = m_pDrive->GetRootString();
	Unlock();

	if (m_bIsDataCarrierBackup)
	{
		// Create temporary dir
		if (CreateTempDir())
		{
			sDestination = m_sTempDir + _T('\\');
			m_sStatus.LoadString(IDS_TEMP_BACKUP_RUNNING);
		}
		else
		{
			AfxMessageBox(IDP_BACKUP_INITIALIZE_FAILED, MB_OK|MB_ICONSTOP);
			return;
		}
	}
	m_staticState.SetWindowText(m_sStatus);

	EnableControls();

	DWORD dwMBytes = m_i64BytesToCopy.GetInMB();
	m_ctrlProgress.SetRange32(0,dwMBytes);
	m_ctrlProgress.SetPos(0);
	m_dwStartTime = GetTickCount();

	EnableTreeView(FALSE);

	CServer *pServer = GetServer();
	if (pServer)
	{
		CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
		if (pDatabase)
		{
			Lock(_T(__FUNCTION__));
			int iArchives = m_ArchivesToCopy.GetSize();
			DWORD* pdwArchives = new DWORD[iArchives];
			WORD wArchiveNr,wSequenceNr;
			//changes for VisualStudio 2005
			int i = 0;
			for (i=0 ; i<iArchives ; i++)
			{
				wArchiveNr = m_ArchivesToCopy.GetAtFast(i)->GetID();
				wSequenceNr = 0;
				pdwArchives[i] = MAKELONG(wSequenceNr,wArchiveNr); // low,high
				WK_TRACE(_T("requesting backup a=%d,\n"),wArchiveNr);
			}
			Unlock();

			m_dwSessionID = GetTickCount();
			// gf todo Krampf wegen eines WORDs!
			WK_TRACE(_T("Requesting Database backup %08lx\n"), m_dwSessionID);
			pDatabase->DoRequestBackupByTime(LOWORD(m_dwSessionID),
											sDestination,
											m_sBackupName,
											iArchives,
											pdwArchives,
											m_stStartDateTime,
											m_stEndDateTime,
											BBT_EXECUTE|BBT_PROGRESS
											);
			WK_DELETE_ARRAY(pdwArchives);
		}
		else
		{
			// no database
			WK_TRACE(_T("Backup FAILED, no database\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::StartDataCarrierBackup()
{
	m_enumStatus = BS_RUNNING_DATACARRIER;
	if (m_bCleanRW)
	{
		m_sStatus.LoadString(IDS_CLEAN_RW);
		m_i64BytesCopied = 0;
		m_sSize = _T("");
		m_sVelocity = _T("");
		m_sTime = _T("");
	}
	else
	{
		m_sStatus.LoadString(IDS_BACKUP_RUNNING);
		// Writing lead in will takes approx. 40 seconds without update
		// so start timer to indicate progress to user 
		// Estimate time to burn for quad speed 600 kB/s
		m_dwCountDownInSeconds = m_i64BytesToCopy.GetInMB() * 1024 / 600;
		m_uTimerDataCarrier = SetTimer(TIMER_FOR_DATA_CARRIER, 1000, NULL);
	}
	m_staticState.SetWindowText(m_sStatus);

	m_ctrlProgress.SetPos(0);
	m_ctrlProgress.SetRange32(0, 100);
	DWORD dwFlags = CD_WRITE_ISO;

	Lock(_T(__FUNCTION__));
	switch (m_pDrive->GetType())
	{
	case DRIVE_DVD_RAM:
	case DRIVE_DVD_R:
	case DRIVE_DVD_RW:
		dwFlags = DVD_WRITE_ISO;
		break;
	}
	Unlock();

	CServer *pServer = GetServer();
	if (pServer && m_pParent->GetDocument()->IsDataCarrierConnected())
	{
		CIPCDataCarrierIdipClient* pDataCarrier = pServer->GetDocument()->GetDataCarrier();
		if (pDataCarrier && pDataCarrier->IsReady())
		{
			// search all files and dirs in temp dir
			CStringArray saFilesAndDirs;
			WK_SearchFiles(saFilesAndDirs, m_sTempDir, _T("*.*"), TRUE);
			CString sFileOrDir;
			CString sCompleteFilePath;
			CStringArray saToCopy;
			int iLen = 0;
			//changes for VisualStudio 2005
			int i = 0;
			for (i=0 ; i<saFilesAndDirs.GetSize() ; i++)
			{
				sFileOrDir = saFilesAndDirs.GetAt(i);
				iLen = m_sTempDir.GetLength() + sFileOrDir.GetLength() + 1;
				if (iLen > _MAX_PATH)
				{
					// error, path will be too long!
					WK_TRACE_ERROR(_T("Backup FAILED, file path to long (%i) for %s and %s\n"),
														iLen, m_sTempDir, sFileOrDir);
					return;
				}
				sCompleteFilePath = m_sTempDir + _T('\\') + sFileOrDir;
				saToCopy.Add(sCompleteFilePath);
			}
			m_dwStartTime = GetTickCount();
			WK_TRACE(_T("Requesting DataCarrier backup %08lx\n"), m_dwSessionID);
			pDataCarrier->DoRequestSession(m_dwSessionID,
										   m_pDrive->GetRootString(),
										   saToCopy,
										   dwFlags
										   );
		}
		else
		{
			// no database
			WK_TRACE_ERROR(_T("Backup FAILED, no data carrier\n"));
		}
	}
	else
	{
		// no database
		WK_TRACE_ERROR(_T("Backup FAILED, data carrier not connected\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgBackupNew::BackupCancelled(DWORD dwSessionID)
{
	// CAVEAT: Called from CIPC thread or main thread!
	m_bIsCancelled = FALSE;
	m_sStatus.LoadString(IDS_BACKUP_CANCELLED);
	m_enumStatus = BS_NORMAL;
	if (m_bIsDataCarrierBackup)
	{
		// delete temp dir
		Lock(_T(__FUNCTION__));
		if (   m_pDrive
			&& IsWritableCDorDVD(m_pDrive->GetType())
			)
		{
			Unlock();
			DeleteTempDir();
		}
		else
		{
			Unlock();
			// no temp dir?
		}
	}

	PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnOK() 
{
	// user started a new backup, ignore any previous unconfirmed cancel
	m_bIsCancelled = FALSE;
	if (theApp.m_pUser)
	{
		WK_TRACE_USER(_T("%s startet backup\n"),theApp.m_pUser->GetName());
	}
	Lock(_T(__FUNCTION__));
	if (m_ArchivesToCopy.GetSize() > 0)
	{
		Unlock();
		UpdateData();
		StartDatabaseBackup();
	}
	else
	{
		Unlock();
		// no sequences
		CString s;
		s.LoadString(IDP_NOTHING_TO_BACKUP);
		COemGuiApi::MessageBox(s,20,MB_OK|MB_ICONSTOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnCancel() 
{
	if (IDYES==AfxMessageBox(IDS_CANCEL_BACKUP,MB_YESNO|MB_ICONQUESTION))
	{
		CServer *pServer = GetServer();
		if (pServer)
		{
			if (m_enumStatus == BS_RUNNING_DATACARRIER)
			{
				CIPCDataCarrierIdipClient* pDataCarrier = pServer->GetDocument()->GetDataCarrier();
				if (pDataCarrier)
				{
					WK_TRACE(_T("Cancel DataCarrier backup %08lx\n"), m_dwSessionID);
					pDataCarrier->DoRequestCancelSession(m_dwSessionID);
					m_bIsCancelled = TRUE;
				}
			}
			else
			{
				CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
				if (pDatabase)
				{
					WK_TRACE(_T("Cancel Database backup %08lx\n"), m_dwSessionID);
					// gf todo Krampf wegen eines WORDs!
					pDatabase->DoRequestCancelBackup(LOWORD(m_dwSessionID));
					m_bIsCancelled = TRUE;
				}
			}
		}
		else
		{
			m_bIsCancelled = TRUE;
		}
		
		if (m_bIsCancelled)
		{
			WK_TRACE_USER(_T("%s cancelled backup\n"),theApp.m_pUser->GetName());
			EnableControls();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnSelchangeComboDestination() 
{

	int iCurSel = m_comboDestination.GetCurSel();

	if (iCurSel!=CB_ERR)
	{
		CString sRoot;
		m_comboDestination.GetLBText(iCurSel,sRoot);

		CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
		CServer *pServer = GetServer();
		if (   pServer
			&& (   m_pDrive == NULL
			    || m_pDrive->GetRootString() != sRoot)
			)
		{
			// drive changed
			m_enumStatus = BS_CHECKING_DRIVE;
			EnableControls();
			m_dwSessionID = GetTickCount();
			WK_DELETE(m_pDrive);
			sRoot = sRoot.Left(3);
			CIPCDrive*pDrive = pServer->GetDrives().GetDrive(sRoot);
			if (pDrive)
			{
				m_pDrive = new CIPCDrive(*pDrive);
				CheckDriveSpace(TRUE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBackupNew::OnUpdateAndEnableControls(WPARAM wParam, LPARAM lParam)
{
	if(m_enumStatus == BS_RUNNING_DATABASE)
	{
		m_ctrlProgress.SetPos(m_i64BytesCopied.GetInMB());
	}
	else if(m_enumStatus == BS_RUNNING_DATACARRIER)
	{
		m_ctrlProgress.SetPos(m_iProgressPercent);
	}

	CString sNoMedium;
	sNoMedium.LoadString(IDS_NO_MEDIUM_FOUND);
	if (   (m_sType == sNoMedium)
		&& !m_bTypeIsHighlighted
		)
	{
		SetHighlight(m_comboDestination.GetDlgCtrlID(), TRUE, SKIN_COLOR_RED);
//		SetHighlight(m_staticType.GetDlgCtrlID(), TRUE, SKIN_COLOR_RED);
		m_bTypeIsHighlighted = TRUE;
	}
	else if (m_bTypeIsHighlighted)
	{
		SetHighlight(m_comboDestination.GetDlgCtrlID(), FALSE, SKIN_COLOR_RED);
//		SetHighlight(m_staticType.GetDlgCtrlID(), FALSE, SKIN_COLOR_RED);
		m_bTypeIsHighlighted = FALSE;
		Invalidate();
	}

	UpdateData(FALSE);
	EnableControls();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBackupNew::OnHandleCDandDVDInfo(WPARAM wParam, LPARAM lParam)
{

	if (  IsRewritableCDorDVD(lParam))
	{
		if (AfxMessageBox(IDP_DELETE_CDRW, MB_YESNO) == IDYES)
		{
			m_bCleanRW = TRUE;
			CalcDestinationCapacity();
			if (m_bNeedCalculationSize)
			{
				StartCalculationForEndTime();
			}
		}
		else
		{	// set destination capacity to 0 to disable backup
			m_i64DestCapacity = 0;
		}
	}
	else
	{
		AfxMessageBox(IDP_MULTISESSION_NOT_POSSIBLE);
		// set destination capacity to 0 to disable backup
		m_i64DestCapacity = 0;
	}
	PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CheckDriveSpace(BOOL bFormat /*= TRUE*/)
{
	m_bIsDataCarrierBackup	= FALSE;
	m_bCleanRW				= FALSE;

	BOOL bKeepStatus = FALSE;
	CheckSpaceOnDestination(bFormat);

	CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
	if (m_pDrive)
	{
		switch (m_pDrive->GetType())
		{
		case DRIVE_REMOVABLE:
			// The disk can be removed from the drive. 
			m_sType.LoadString(IDS_DRIVE_FD);
			break;
		case DRIVE_FIXED:
			// The disk cannot be removed from the drive. 
			m_sType.LoadString(IDS_DRIVE_HD);
			break;
		case DRIVE_REMOTE:
			// The drive is a remote (network) drive. 
			m_sType.LoadString(IDS_DRIVE_NET);
			break;
		case DRIVE_CDROM:
		case DRIVE_CD_R:
		case DRIVE_CD_RW:
		case DRIVE_DVD_ROM:
		case DRIVE_DVD_RAM:
		case DRIVE_DVD_R:
		case DRIVE_DVD_RW:
			// The drive is a CD-ROM drive. 
			m_sType.LoadString(IDS_DRIVE_CD);
			m_pDrive->CheckSpace();
			if (  (m_pParent->GetDocument()->IsDataCarrierConnected()
				&& m_pParent->GetDocument()->GetDataCarrier()->IsReady())
				&& (m_pDrive->GetFreeMB() == 0)
				)
			{
				CIPCDataCarrierIdipClient* pDataCarrier = m_pParent->GetDocument()->GetDataCarrier();
				if (pDataCarrier)
				{
					bKeepStatus = TRUE;
					WK_TRACE(_T("Request Volume infos %08lx\n"), m_dwSessionID);
					pDataCarrier->DoRequestVolumeInfos();
					m_sType.LoadString(IDS_CDR_CHECK);
				}
			}
			else
			{
				if (m_pDrive->GetMB() > 2*1024)
				{
					// assuming DVD-RAM
					m_sType.LoadString(IDS_DRIVE_DVD_RAM);
					m_pDrive->SetType(DRIVE_DVD_RAM);
				}
			}
			break;
		}

		CalcDestinationCapacity();
		if (bKeepStatus == FALSE)
		{
			m_enumStatus = BS_NORMAL;
		}
		PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CalcDestinationCapacity()
{
	// Special case CDROM will return 0
	CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
	DWORD dwFreeMB = m_pDrive->GetFreeMB();
	if (m_bCleanRW)
	{
		dwFreeMB = m_pDrive->GetMB();
	}

	// gf todo
	// any space to reserve at destination for
	// - reader software
	// - disc organisation
	UINT uType = m_pDrive->GetType();
	if (IsCDorDVD(uType))
	{
		BOOL bEnoughSpace = FALSE;
		if (dwFreeMB > m_dwReaderSoftwareMB)
		{
			dwFreeMB -= m_dwReaderSoftwareMB;
			switch (uType)
			{
				case DRIVE_CDROM:
				case DRIVE_CD_R:
					if (dwFreeMB > m_dwReservedSpaceMB)
					{
						dwFreeMB -= m_dwReservedSpaceMB;
						bEnoughSpace = TRUE;
					}
					break;
				case DRIVE_CD_RW:
					bEnoughSpace = TRUE;
					break;
				case DRIVE_DVD_RAM:
					bEnoughSpace = TRUE;
					break;
				case DRIVE_DVD_R:
					bEnoughSpace = TRUE;
					break;
				case DRIVE_DVD_RW:
					bEnoughSpace = TRUE;
					break;
				default:
					break;
			}
		}
		if (bEnoughSpace == FALSE)
		{
			dwFreeMB = 0;
		}
	}
	else if (uType == DRIVE_REMOVABLE)
	{ // how about HDD?
		CCopyReadOnlyVersion crov(_T(".\\"), FALSE);
		DWORD dwReaderSoftwareSize = crov.GetReaderSoftwareSizeMB();
		if (dwFreeMB > dwReaderSoftwareSize)
		{
			dwFreeMB -= dwReaderSoftwareSize;
		}
		else
		{
			dwFreeMB = 0;
		}
	}
	m_sFree.Format(_T("%lu"), dwFreeMB);

	// check for change
	ULONGLONG ulDummy = dwFreeMB;
	CIPCInt64 i64SizeCurrent = ulDummy * 1024 * 1024;
	if (   (m_i64BytesToCopy != 0)
		&& (i64SizeCurrent != 0)
		&& (i64SizeCurrent != m_i64DestCapacity)
		&& (m_enumStatus != BS_RUNNING_DATABASE)
		&& (m_enumStatus != BS_RUNNING_DATACARRIER)
		)
	{
		// drive has changed, the user has to calculate again
		m_bEndCalculationPossible = TRUE;
		m_bNeedCalculationSize = TRUE;
	}
	m_i64DestCapacity = i64SizeCurrent;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::OnInitDialog() 
{
	CDlgBackup::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();
	AutoCreateSkinComboBox();
	AutoCreateSkinProgress();
	AutoCreateSkinListBox();
	AutoCreateSkinButtons();
	AutoCreateSkinListCtrl();
	// TODO GF Workaround Resource Editor Bug
	m_ctrlStartTime.ModifyStyle(NULL, DTS_TIMEFORMAT);
	m_ctrlEndTime.ModifyStyle(NULL, DTS_TIMEFORMAT);

	CServer *pServer = GetServer();
	if (pServer == NULL)
	{
		return FALSE;
	}

	CString sText;
	CWnd *pWnd = GetDlgItem(IDC_GRP_BACKUP);
	ASSERT(pWnd != NULL);
	if (pServer->IsLocal())
	{
		pWnd->GetWindowText(sText);
	}
	else
	{
		sText.LoadString(IDS_BACKUP_REMOTE);
	}
	sText += _T(" ");
	sText += pServer->GetFullName();
	pWnd->SetWindowText(sText);

	pWnd = GetDlgItem(IDC_GRP_DRIVE);
	ASSERT(pWnd != NULL);
	if (pServer->IsLocal() == FALSE)
	{
		sText.LoadString(IDS_BACKUP_DRIVE_REMOTE);
		pWnd->SetWindowText(sText);

		SetHighlight(m_comboDestination.GetDlgCtrlID(), TRUE, RGB(255,0,0));
//		SetHighlight(m_staticType.GetDlgCtrlID(), TRUE, RGB(255,0,0));
		m_bTypeIsHighlighted = TRUE;
	}

	EnableControls();
	CenterWindow(GetParent());


	//ist in eine Funktion gewandert, die von außen (also wenn der Dialog komplett fertig
	//erzeugt wurde) aufgerufen wird.
	//Wird das hier schon gemacht, ist die Antwort der Datenbank auf RefreshInfo schneller da,
	//als der Dialog hier komplett aufgebaut wurde.
	//m_uTimerDriveInfo = SetTimer(TIMER_FOR_DRIVE_INFO,1000,NULL);
	//pServer->GetDatabase()->RefreshInfo();

	/*
	{
		CAutoCritSec acs(&m_cs);
		if (InitDrives() == FALSE)
		{
			PostMessage(WM_COMMAND,
						MAKEWPARAM(m_ctrlClose.GetDlgCtrlID(), BN_CLICKED),
						(LPARAM)m_ctrlClose.m_hWnd);
		}

		m_sStatus.LoadString(IDS_BACKUP_NOT_STARTED);
		m_staticState.SetWindowText(m_sStatus);

		CString s;
		s.Format(_T("%d"),m_i64BytesToCopy.GetInMB());
		m_staticToCopy.SetWindowText(s);

		// select a drive, if any
		if (m_comboDestination.GetCount() > 0)
		{
			m_comboDestination.SetCurSel(0);
			PostMessage(WM_COMMAND,
						MAKEWPARAM(m_comboDestination.GetDlgCtrlID(), LBN_SELCHANGE),
						(LPARAM)m_comboDestination.m_hWnd);
		}
		else
		{
			// no drive! Disable controls
			EnableControls();
		}

	}
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDrivesInfo()
{
	if (m_uTimerDriveInfo)
	{
		KillTimer(m_uTimerDriveInfo);
		m_uTimerDriveInfo = 0;
	}

	m_bDriveInfoChanged = TRUE;
	InitDrives();
	if (m_enumStatus == BS_NORMAL)
	{
		SelectDrive();
	}
	if (!IsWindowVisible())
	{
		ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::InitDrives()
{
	BOOL bReturn = FALSE;
	CServer *pServer = GetServer();
	if (pServer)
	{
		BOOL bDirectCDInit = FALSE;
		bReturn = TRUE;
		CIPCDrive* pDrive = NULL;
		CIPCDrives& drives = pServer->GetDrives();
		CString  strFormat, strDriveType, strDriveName, strRoot;
		drives.Lock(_T(__FUNCTION__));
		m_comboDestination.ResetContent();
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<drives.GetSize() ; i++)
		{
			pDrive = drives.GetAtFast(i);
			if (   pDrive->IsEnabled()
				&& !pDrive->IsSystem()
				&& (   pDrive->IsWriteBackup()					// as backup drive declared
				    || pDrive->GetType() == DRIVE_REMOVABLE))	// removeable drives like memory stick
			{
				// all non-CD drives and local CD drives
				if (   pServer->IsLocal() 
					|| !pDrive->IsCDROM())
				{
					switch(pDrive->GetType())
					{
						case DRIVE_REMOVABLE: strDriveType.LoadString(IDS_DRIVE_FD); break;
						case DRIVE_FIXED:     strDriveType.LoadString(IDS_DRIVE_HD); break;
						case DRIVE_REMOTE:    strDriveType.LoadString(IDS_DRIVE_NET);break;
						case DRIVE_CDROM:     strDriveType.LoadString(IDS_DRIVE_CD); break;
						default:              strDriveType = _T("");                 break;
					}
					strRoot = pDrive->GetRootString();
					if (pServer->IsLocal())
					{
						BOOL bReturn = GetVolumeInformation(strRoot,strDriveName.GetBufferSetLength(MAX_PATH), MAX_PATH-1, NULL, NULL, NULL, NULL, 0);
						if (!bReturn) strDriveName = _T("");
						else          strDriveName.ReleaseBuffer();
						strFormat.Format(_T("%s (%s)"), strRoot, strDriveType);
						if (!strDriveName.IsEmpty()) strFormat = strFormat + ": " + strDriveName;
					}
					else
					{
						strFormat.Format(_T("%s (%s)"), strRoot, strDriveType);
						strFormat = strFormat + ": " + pServer->GetName();
					}
					m_comboDestination.AddString(strFormat);
				}
				if (   (pDrive->GetFSProducer() == FSP_UDF_ADAPTEC_DIRECTCD)
					&& (bDirectCDInit == FALSE)
					)
				{
					InitDirectCDLanguageStrings();
					// init only once
					bDirectCDInit = TRUE;
				}
			}
		}
		drives.Unlock();

		int iCount = m_comboDestination.GetCount();
		if (iCount == 0)
		{
			{
				CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
				WK_DELETE(m_pDrive);
			}
			if (m_pSelectDrive == NULL)
			{
				m_pSelectDrive = new CDlgSelectBackupDrive(this);
				INT_PTR nResult = m_pSelectDrive->DoModal();
				if (IDOK==nResult)
				{
					m_comboDestination.AddString(m_pSelectDrive->GetRootString());
				}
				else if (-1 == nResult)	// database disconnect
				{
					PostMessage(WM_USER, IDC_CLOSE, NULL);
					bReturn = FALSE;
				}
				else
				{
					// no drive no backup
					bReturn = FALSE;
				}

				WK_DELETE(m_pSelectDrive);
			}
			else
			{
				m_pSelectDrive->InsertDrives();
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::SelectDrive() 
{
	CServer *pServer = GetServer();
	if (pServer && m_comboDestination.GetCount() > 0)
	{
		int iIndex = CB_ERR;
		CString sRoot;
		CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
		if (m_pDrive)
		{
			sRoot = m_pDrive->GetRootString();
			sRoot = sRoot.Left(3);
			iIndex = m_comboDestination.FindStringExact(0, sRoot);
		}
		if (iIndex != CB_ERR)
		{	// found the previous drive, only select entry and update m_pDrive
			m_comboDestination.SetCurSel(iIndex);
			WK_DELETE(m_pDrive);
			m_pDrive = new CIPCDrive(*(pServer->GetDrives().GetDrive(sRoot)));
		}
		else
		{	// drive changed, select a new one
			m_comboDestination.SetCurSel(0);
			OnSelchangeComboDestination();
		}
	}
	else
	{
		Lock(_T(__FUNCTION__));
		WK_DELETE(m_pDrive);
		Unlock();
		EnableControls();
	}
	m_bDriveInfoChanged = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnClose() 
{
//	RKE: BackupMode obsolete
//	if (theApp.m_bBackupModus)
//	{
//		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT);
//		Do not post ID_APP_EXIT, this will also close the launcher
//	}
//	else
	{
		theApp.GetMainFrame()->GetViewArchive()->EnableWindow(TRUE);
		m_pParent->OnBackupClosed();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDestroy() 
{
	if (m_uTimerRO && KillTimer(m_uTimerRO))
	{
		m_uTimerRO = 0;
	}
	if (m_uTimerDataCarrier && KillTimer(m_uTimerDataCarrier))
	{
		m_uTimerDataCarrier = NULL;
	}
	CDlgBackup::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::ArchivesChanged()
{
	m_i64BytesToCopy = m_ArchivesToCopy.GetSizeBytes();
	m_sMBtoCopy.Format(_T("%d"),m_i64BytesToCopy.GetInMB());
	UpdateData(FALSE);

	BOOL bToCopy = ((m_i64DestCapacity > 0)	&& (m_i64BytesToCopy > 0));
	if (bToCopy)
	{
		Calculate(FALSE);
	}
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::BackupDatabaseFinished()
{
	WK_TRACE(_T("Backup database finished %08lx\n"),m_dwSessionID);

	m_ctrlProgress.SetPos(0);
	m_sSize = _T("");
	m_sVelocity = _T("");
	m_sTime = _T("");

	UpdateData(FALSE);
	
	if (m_bIsCancelled)
	{
		m_sStatus.LoadString(IDS_BACKUP_CANCELLED);
		m_staticState.SetWindowText(m_sStatus);
		if (m_bIsDataCarrierBackup)
		{
			// delete temp dir
			Lock(_T(__FUNCTION__));
			if (m_pDrive && IsWritableCDorDVD(m_pDrive->GetType()))
			{
				Unlock();
				DeleteTempDir();
			}
			else
			{
				Unlock();
				// no temp dir?
			}
		}
	}
	else
	{
		if (m_bIsDataCarrierBackup)
		{
			// copy reader for cd/r
			Lock(_T(__FUNCTION__));
			if (m_pDrive && IsWritableCDorDVD(m_pDrive->GetType()))
			{
				CopyReadOnlyVersion();
				Unlock();
			}
			else
			{
				Unlock();
				StartDataCarrierBackup();
			}
		}
		else
		{
			//ab Idip 5.0 Lesesoftware auf alle Backup Laufwerke kopieren
			//USB-Stick, USB-Festplatte.
			CServer* pServer = GetServer();
			if(pServer && pServer->CanSplittingBackup())
			{
				Lock(_T(__FUNCTION__));
				if (m_pDrive)
				{
					CopyReadOnlyVersion();
					Unlock();
				}
				else
				{
					Unlock();
					CheckForContinuation();
				}
			}
			else
			{
				// old style backup with Direct-CD
				m_sStatus.LoadString(IDS_BACKUP_FINISHED);
				m_staticState.SetWindowText(m_sStatus);

				m_i64BytesToCopy = 0;
				
				UpdateData(FALSE);

				// old style backup with Direct-CD knows only CDROM as type!
				// copy reader for cd/r
				Lock(_T(__FUNCTION__));
				if (m_pDrive && IsCDorDVD(m_pDrive->GetType()))
				{
					CopyReadOnlyVersion();
					Unlock();
				}
				else
				{
					Unlock();
					CheckForContinuation();
				}
			}

		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBackupNew::OnBackupDataCarrierFinished(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("Backup datacarrier finished %08lx\n"),m_dwSessionID);

	if (m_uTimerDataCarrier && KillTimer(m_uTimerDataCarrier))
	{
		m_uTimerDataCarrier = NULL;
	}

	DeleteTempDir();

	m_sStatus.LoadString(IDS_BACKUP_FINISHED);
	m_staticState.SetWindowText(m_sStatus);

	m_bCleanRW = FALSE;
	m_iProgressPercent = 0;
	m_ctrlProgress.SetPos(0);
	m_sSize = _T("");
	m_sVelocity = _T("");
	m_sTime = _T("");

	UpdateData(FALSE);

	CheckForContinuation();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::RemoveArchiveFromList(WORD wID)
{
	int i,c;

	c = m_ctrlArchives.GetCount();
	for (i=0;i<c;i++)
	{
		DWORD data = m_ctrlArchives.GetItemData(i);
		if (data==wID)
		{
			m_ctrlArchives.DeleteString(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_uTimerDriveInfo)
	{
		WK_TRACE(_T("time out waiting for archive and drive info for backup\n"));
		if (m_uTimerDriveInfo)
		{
			KillTimer(m_uTimerDriveInfo);
			m_uTimerDriveInfo = 0;
		}
		ShowWindow(SW_SHOW);
	}
	else if (nIDEvent == m_uTimerRO)
	{
		// the copy read only version process
		if (m_pCopyReadOnlyVersion->IsRunning())
		{
			// running
		}
		else
		{
			// finished
			CopyReadOnlyVersionFinished();
		}
	}
	else if (nIDEvent == m_uTimerDataCarrier)
	{
		// count down for Lead in / Lead out
		// do not let it _T("underflow")!
		if (m_dwCountDownInSeconds > 0)
		{
			m_dwCountDownInSeconds--;
		}
		else
		{
			// hop up and down again
			m_dwCountDownInSeconds++;
		}
		TRACE(_T("m_dwCountDownInSeconds %lu\n"), m_dwCountDownInSeconds);
		m_sTime.Format(_T("%02d:%02d:%02d"), m_dwCountDownInSeconds / 3600,
										 (m_dwCountDownInSeconds / 60) % 60,
										 m_dwCountDownInSeconds % 60);
		UpdateData(FALSE);
	}

	CDlgBackup::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CopyReadOnlyVersion()
{
	if (m_bIsDataCarrierBackup)
	{
		m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(m_sTempDir, m_bIsDataCarrierBackup);
	}
	else
	{
		m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(m_pDrive->GetRootString(), m_bIsDataCarrierBackup);
	}

/*
	//es soll imme die aktuelle Software mit raufkopiert werden
	if (m_pCopyReadOnlyVersion->IsAlreadyDone())
	{
		WK_DELETE(m_pCopyReadOnlyVersion);
		if (m_pDrive->IsCDROM())
		{
			AskForCDRFinalize();
		}
		return;
	}
*/
	EnableControls();
	m_sStatus.LoadString(IDS_COPY_ROV);
	m_staticState.SetWindowText(m_sStatus);
	m_uTimerRO = SetTimer(TIMER_FOR_NEW_BACKUP,1000,NULL);

	m_pCopyReadOnlyVersion->StartThread();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CopyReadOnlyVersionFinished()
{
	if (m_uTimerRO && KillTimer(m_uTimerRO))
	{
		m_uTimerRO = NULL;
	}

	WK_DELETE(m_pCopyReadOnlyVersion);
	m_sStatus.LoadString(IDS_COPY_ROV_DONE);
	m_staticState.SetWindowText(m_sStatus);

	if (m_bIsCancelled)
	{
		m_sStatus.LoadString(IDS_BACKUP_CANCELLED);
		m_staticState.SetWindowText(m_sStatus);
	}
	else
	{
		if (m_bIsDataCarrierBackup)
		{
			// do the real copy
			StartDataCarrierBackup();
		}
		else
		{	
			CServer* pServer = GetServer();
			if(pServer && pServer->CanSplittingBackup())
			{
				CheckForContinuation();
				m_sStatus.LoadString(IDS_BACKUP_FINISHED);
				m_staticState.SetWindowText(m_sStatus);

				m_bCleanRW = FALSE;
				m_iProgressPercent = 0;
				m_ctrlProgress.SetPos(0);
				m_sSize = _T("");
				m_sVelocity = _T("");
				m_sTime = _T("");

				UpdateData(FALSE);
			}
			else
			{
				// old style backup with Direct-CD knows only CDROM as type
				if (m_pDrive && IsCDorDVD(m_pDrive->GetType()))
				{
					AskForCDRFinalize();
				}
				CheckForContinuation();
			}

		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnChangeEditBackupName() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnCdr() 
{
	CheckDriveSpace(TRUE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CSystemTime stDate, stTime;
	m_ctrlStartDate.GetTime(&stDate);
	m_ctrlStartTime.GetTime(&stTime);
	CSystemTime stUser(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					   stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	CSystemTime stCurrent;
	stCurrent.GetLocalTime();

	// no time earlier than first image allowed, makes no sense
	if (stUser < m_stFirstDBImage)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(m_stFirstDBImage);
		m_ctrlStartTime.SetTime(m_stFirstDBImage);
	}
/*	else if (stUser > m_stEndDateTime)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(m_stEndDateTime);
		m_ctrlStartTime.SetTime(m_stEndDateTime);
	}
*/	else if (stUser > stCurrent)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(stCurrent);
		m_ctrlStartTime.SetTime(stCurrent);
	}

	m_ctrlStartDate.GetTime(&stDate);
	m_ctrlStartTime.GetTime(&stTime);
	CSystemTime stSet(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					  stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	m_stStartDateTime = stSet;

	m_bStartDateTimeEntered = TRUE;
	m_bNeedCalculationSize = TRUE;
	m_bEndCalculationPossible = TRUE;
	EnableControls();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CSystemTime stDate, stTime;
	m_ctrlStartDate.GetTime(&stDate);
	m_ctrlStartTime.GetTime(&stTime);
	CSystemTime stUser(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					   stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	CSystemTime stCurrent;
	stCurrent.GetLocalTime();

	// no time earlier than first image allowed, makes no sense
	if (stUser < m_stFirstDBImage)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(m_stFirstDBImage);
		m_ctrlStartTime.SetTime(m_stFirstDBImage);
	}
/*	else if (stUser > m_stEndDateTime)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(m_stEndDateTime);
		m_ctrlStartTime.SetTime(m_stEndDateTime);
	}
*/	else if (stUser > stCurrent)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlStartDate.SetTime(stCurrent);
		m_ctrlStartTime.SetTime(stCurrent);
	}

	m_ctrlStartDate.GetTime(&stDate);
	m_ctrlStartTime.GetTime(&stTime);
	CSystemTime stSet(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					  stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	m_stStartDateTime = stSet;

	m_bStartDateTimeEntered = TRUE;
	m_bNeedCalculationSize = TRUE;
	m_bEndCalculationPossible = TRUE;
	EnableControls();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDatetimechangeEndDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CSystemTime stDate, stTime;
	m_ctrlEndDate.GetTime(&stDate);
	m_ctrlEndTime.GetTime(&stTime);
	CSystemTime stUser(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					   stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	CSystemTime stCurrent;
	stCurrent.GetLocalTime();

	if (stUser < m_stStartDateTime)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlEndDate.SetTime(m_stStartDateTime);
		m_ctrlEndTime.SetTime(m_stStartDateTime);
	}
	else if (stUser > stCurrent)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlEndDate.SetTime(stCurrent);
		m_ctrlEndTime.SetTime(stCurrent);
	}

	m_ctrlEndDate.GetTime(&stDate);
	m_ctrlEndTime.GetTime(&stTime);
	CSystemTime stSet(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					  stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	m_stEndDateTime = stSet;

	m_bNeedCalculationSize = TRUE;
	m_bEndCalculationPossible = TRUE;
	EnableControls();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnDatetimechangeEndTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CSystemTime stDate, stTime;
	m_ctrlEndDate.GetTime(&stDate);
	m_ctrlEndTime.GetTime(&stTime);
	CSystemTime stUser(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					   stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	CSystemTime stCurrent;
	stCurrent.GetLocalTime();

	if (stUser < m_stStartDateTime)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlEndDate.SetTime(m_stStartDateTime);
		m_ctrlEndTime.SetTime(m_stStartDateTime);
	}
	else if (stUser > stCurrent)
	{
		// gf TODO Do we really have to inform the user?
		m_ctrlEndDate.SetTime(stCurrent);
		m_ctrlEndTime.SetTime(stCurrent);
	}

	m_ctrlEndDate.GetTime(&stDate);
	m_ctrlEndTime.GetTime(&stTime);
	CSystemTime stSet(stDate.GetDay(),  stDate.GetMonth(),  stDate.GetYear(),
					  stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
	m_stEndDateTime = stSet;

	m_bNeedCalculationSize = TRUE;
	m_bEndCalculationPossible = TRUE;
	EnableControls();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnResetStartEnd() 
{
	Calculate(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnCalculateEnd() 
{
	m_stEndDateTime.GetLocalTime();
	m_ctrlEndDate.SetTime(&m_stEndDateTime);
	m_ctrlEndTime.SetTime(&m_stEndDateTime);
	StartCalculationForEndTime();
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::OnCalculateSize() 
{
	StartCalculationForSize();
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::FirstCalculationEnd(DWORD dwID,
										  const CSystemTime& stStart,
										  const CSystemTime& stEnd,
										  CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	TRACE(_T("FirstCalculationEnd dwID:%08lx Start:%s End:%s iSize(MB): % 4lu\n"), dwID,
																		   stStart.GetDateTime(),
																		   stEnd.GetDateTime(),
																		   i64Size.GetInMB()
																		   );
	if (dwID == 0)
	{
		// If total size is smaller than destination space: OK, all can be backuped
		if (i64Size <= m_i64DestCapacity)
		{
			m_i64LessMaxCapacity = i64Size;
			m_stEndDateTime = m_stEndDateTimeConfirmed;
			CalculationFinished();
		}
		else
		{
			// We have to find the right end time
			m_enumStatus = BS_CALCULATING_END;

			// save the actual interval boundaries
			m_i64MoreMaxCapacity = i64Size;
			m_stEndLessMaxCapacity = stStart;
			m_stEndMoreMaxCapacity = stEnd;

			if (   (m_comboDestination.GetCount() > 1)
				|| (   m_pDrive
					&& m_pDrive->IsWritableRemovableDisk())
				)
			{
				m_bContinuationPossible = TRUE;
				WK_TRACE(_T("FirstCalculationEnd, Continuation possible\n"));
			}
			if (RequestNextCalculationEnd() == FALSE)
			{
				CalculationFinished();
			}
		}
	}
	else
	{
		// single confirms for every sequence, check for real end time
		if (m_stEndDateTimeConfirmed < stEnd)
		{
			m_stEndDateTimeConfirmed = stEnd;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::NextCalculationEnd(DWORD dwID,
										 const CSystemTime& stStart,
										 const CSystemTime& stEnd,
										 CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	TRACE(_T("NextCalculationEnd dwID:%lu Start:%s End:%s iSize(MB): %lu\n"), dwID,
																		  stStart.GetDateTime(),
																		  stEnd.GetDateTime(),
																		  i64Size.GetInMB()
																		  );
	BOOL bNextTry = TRUE;
	if (dwID == 0)
	{
		// If total size is matching destination: HURRAY, all can be backuped
		if (i64Size == m_i64DestCapacity)
		{
			m_i64LessMaxCapacity = i64Size;
			m_stEndDateTime = stEnd;
			bNextTry = FALSE;
		}
		else
		{
			// We have to find the right end time again
			m_enumStatus = BS_CALCULATING_END;

			// save the actual interval boundaries
			if (i64Size < m_i64DestCapacity)
			{
				m_i64LessMaxCapacity = i64Size;
				m_stEndLessMaxCapacity = stEnd;
			}
			else
			{
				m_i64MoreMaxCapacity = i64Size;
				m_stEndMoreMaxCapacity = stEnd;
			}

			bNextTry = RequestNextCalculationEnd();
		}
		if (!bNextTry)
		{
			CalculationFinished();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::RequestNextCalculationEnd()
{
	BOOL bDoNextTry = TRUE;
	// CAVEAT: In CIPC thread!

	// starting the adjustment with the lower interval boundary
	m_stEndDateTime = m_stEndLessMaxCapacity;

	// Find the mid between the interval boundaries
	CSystemTime		stDiff = m_stEndMoreMaxCapacity - m_stEndLessMaxCapacity;
	FILETIME		ftDiff;
	SystemTimeToFileTime(&stDiff, &ftDiff);
	
	LARGE_INTEGER	liDiff;
	liDiff.HighPart	= ftDiff.dwHighDateTime;
	liDiff.LowPart	= ftDiff.dwLowDateTime;

	// LARGE_INTEGER from FILETIME has 100 ns units
	// recalculate to seconds and take the half as adjust
	m_dwSeconds = (DWORD)(liDiff.QuadPart/(10*1000*1000));
	m_dwSeconds = m_dwSeconds / 2;

	// do we have a chance to do a real new test?
	if (m_dwSeconds == 0)
	{
		bDoNextTry = FALSE;
		m_stEndDateTimeConfirmed = m_stEndDateTime;
		TRACE(_T("Calculation aborted due to time diff\n"));
	}
	else
	{
		// do we really need a new test?
		CIPCInt64 i64Diff = m_i64DestCapacity;
		i64Diff -= m_i64LessMaxCapacity;
		DWORD dwDiffMin = m_dwAbortAtDifference_kB * 1024;
		if (i64Diff <= dwDiffMin)
		{
			bDoNextTry = FALSE;
			m_stEndDateTimeConfirmed = m_stEndDateTime;
			TRACE(_T("Calculation aborted due to size diff %lu\n"), dwDiffMin);
		}
	}

	if (bDoNextTry)
	{
		// increment end time by half interval
		m_stEndDateTime.IncrementTime(0,0,0,m_dwSeconds,0);

		// Request new times and sizes
		CServer *pServer = GetServer();
		if (pServer)
		{
			Lock(_T(__FUNCTION__));
			int iNrOfArchives = m_ArchivesToCopy.GetSize();
			DWORD* pArchives = NULL;
			pArchives = new DWORD[iNrOfArchives];
			//changes for VisualStudio 2005
			int i = 0;
			for(i=0; i<iNrOfArchives; i++)
			{
				pArchives[i] = MAKELONG(0, m_ArchivesToCopy.GetAtFast(i)->GetID());
			}
			Unlock();

			CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
			// gf todo Krampf wegen eines WORDS!!!
			pDatabase->DoRequestBackupByTime(	LOWORD(m_dwSessionID),
												_T(""),
												_T(""),
												iNrOfArchives,
												pArchives,
												m_stStartDateTime,
												m_stEndDateTime,
												BBT_CALCULATE,
												m_i64DestCapacity.GetInMB());
			WK_DELETE_ARRAY(pArchives);
		}
	}
	return bDoNextTry;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CalculationFinished()
{
	// CAVEAT: In CIPC thread!

	m_ctrlEndDate.SetTime(m_stEndDateTime);
	m_ctrlEndTime.SetTime(m_stEndDateTime);

	m_i64BytesToCopy = m_i64LessMaxCapacity;
	m_sMBtoCopy.Format(_T("%d"),m_i64BytesToCopy.GetInMB());
	
	m_sStatus.LoadString(IDS_BACKUP_NOT_STARTED);
	m_ctrlProgress.SetPos(0);
	m_sSize = _T("");
	m_sVelocity = _T("");
	m_sTime = _T("");

	m_enumStatus = BS_NORMAL;
	PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::SizeCalculation(DWORD dwID,
										 const CSystemTime& stStart,
										 const CSystemTime& stEnd,
										 CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	TRACE(_T("SizeCalculation dwID:%lu Start:%s End:%s iSize(MB): %lu\n"), dwID,
																	   stStart.GetDateTime(),
																	   stEnd.GetDateTime(),
																	   i64Size.GetInMB()
																	   );
	if (dwID == 0)
	{
		// If total size is smaller than destination space: OK, all can be backuped
		if (i64Size <= m_i64DestCapacity)
		{
			m_stEndDateTime = stEnd;
			m_i64LessMaxCapacity = i64Size;
			CalculationFinished();
		}
		else
		{
			// We have to find the right end time again
			StartCalculationForEndTime();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::BackupProgress(DWORD dwID, CIPCInt64 i64Size)
{
	// CAVEAT: In CIPC thread!
	TRACE(_T("BackupProgress dwID:%lu iSize(MB): %lu\n"), dwID, i64Size.GetInMB());
	if (dwID == 0)
	{
	}
	else
	{
	}

	m_i64BytesCopied = i64Size;

	CalculateInfo();

	PostMessage(WM_USER_UPDATE_AND_ENABLE_CONTROLS);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::CheckForContinuation()
{
	//
	BOOL bContinue = FALSE;
	WK_TRACE(_T("CheckForContinuation %i\n"), m_bContinuationPossible);
	if (   (m_bContinuationPossible)
//		&& (m_stEndDateTime == m_stEndDateTimeConfirmed)
		)
	{
		int iRet = AfxMessageBox(IDP_BACKUP_ASK_FOR_CONTINUATION, MB_ICONQUESTION|MB_YESNO);
		if (iRet == IDYES)
		{
			bContinue = TRUE;
		}
	}

	// check drive space before reset status or continue calculation
	CheckDriveSpace(TRUE);

	BOOL bToCopy = ((m_i64DestCapacity > 0) && (m_i64BytesToCopy > 0));

	if (bContinue)
	{
		if (bToCopy)
		{
			Calculate(TRUE);
		}
		else
		{
			//kein freier Speicherplatz mehr auf Backup Laufwerk
			//trotzdem den vorherigen Endzeitpunkt auf neuen Startzeitpunkt setzen
			m_stStartDateTime = m_stEndDateTime;
			m_stEndDateTime.GetLocalTime();

			m_ctrlStartDate.SetTime(&m_stStartDateTime);
			m_ctrlStartTime.SetTime(&m_stStartDateTime);
			m_ctrlEndDate.SetTime(&m_stEndDateTime);
			m_ctrlEndTime.SetTime(&m_stEndDateTime);
			m_enumStatus = BS_NORMAL;
		}
	}
	else
	{
		m_enumStatus = BS_NORMAL;
	}
	EnableControls();
	EnableTreeView(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::DestroyWindow() 
{
	CServer *pServer = GetServer();
	if (pServer)
	{
		CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
		WK_TRACE(_T("Cancel Database backup %08lx\n"), m_dwSessionID);
		// gf todo Krampf wegen eines WORDs!
		pDatabase->DoRequestCancelBackup(LOWORD(m_dwSessionID));
	}	
	return CDlgBackup::DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::CreateTempDir() 
{
	BOOL bReturn = FALSE;
	CString sTempDir;
	DWORD dwPathLen = m_sTempLocation.GetLength();
	DWORD dwMaxTempPathLen = _MAX_PATH - 50; // lets keep a reserve for filenames
	if ((dwPathLen != 0) && (dwPathLen < dwMaxTempPathLen))
	{
		CString sDir = _T("idip_00000001");
		DWORD dwLen = 0;

// GF todo Nur ein Temp Dir erlauben? Was, wenn nicht löschbar?
#if 1
		sTempDir = m_sTempLocation + sDir;
		dwLen = sTempDir.GetLength();
		if (dwLen > dwMaxTempPathLen)
		{
			// error, path will be too long!
			WK_TRACE(_T("CreateTempDir failed, temp dir path exceeds %i (%s)\n"), dwMaxTempPathLen, sTempDir);
		}
		else
		{
			//TK falls tempdir noch vorhanden, löschen
			if (DoesFileExist(sTempDir))
			{
				m_sTempDir = sTempDir;
				DeleteTempDir();
			}
			// Check for sufficient space on temp drive
			CString sDrive, sPath, sFilename, sExt;
			WK_SplitPathAll(sTempDir, sDrive, sPath,  sFilename, sExt);
			CIPCDrive driveTemp(sDrive, DVR_NORMAL_DRIVE);
			driveTemp.CheckSpace();
			DWORD dwFree = driveTemp.GetFreeMB();
			if (dwFree > (m_i64BytesToCopy.GetInMB() + m_dwReaderSoftwareMB + m_dwTempDriveReservedSpaceMB))
			{
				if (WK_CreateDirectory(sTempDir))
				{
					m_sTempDir = sTempDir;
					TRACE(_T("TempDir created%s\n"), m_sTempDir);
					bReturn = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("CreateTempDir failed (%s)\n"), sTempDir);
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("CreateTempDir failed (%s), not enough free space\n"), sTempDir);
			}
		}
// Oder Schleife über mehrere, dann vorher alle löschen?
/* #else
		for (DWORD dw=1 ; !bReturn && dw!=0 ; dw++)
		{
			sDir.Format(_T("idip_%08lx"), dw);
			sTempDir = m_sTempLocation + sDir;
			dwLen = sTempDir.GetLength();
			if (dwLen > dwMaxTempPathLen)
			{
				// error, path will be too long!
				WK_TRACE(_T("CreateTempDir failed, temp dir path exceeds %i (%s)\n"), dwMaxTempPathLen, sTempDir);
				break;
			}
			else
			{
				//TK falls tempdir noch vorhanden, löschen
				if (DoesFileExist(sTempDir))
				{
					m_sTempDir = sTempDir;
					DeleteTempDir();
				}
				if (WK_CreateDirectory(sTempDir))
				{
					m_sTempDir = sTempDir;
					TRACE(_T("TempDir created%s\n"), m_sTempDir);
					bReturn = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("CreateTempDir failed (%s)\n"), sTempDir);
				}
			}
		}
*/
#endif
	}
	else
	{
		WK_TRACE_ERROR(_T("CreateTempDir failed, temp path exceeds max len %i (%s)\n"), dwMaxTempPathLen, m_sTempLocation);
	}
	
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupNew::DeleteTempDir() 
{
	BOOL bReturn = FALSE;
	if (DeleteDirRecursiv(m_sTempDir))
	{
		TRACE(_T("TempDir deleted %s\n"), m_sTempDir);
		m_sTempDir = _T("");
		bReturn = TRUE;
	}
	else
	{
		WK_TRACE_ERROR(_T("TempDir delete FAILED %s\n"), m_sTempDir);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupNew::RefreshInfo()
{	
	m_uTimerDriveInfo = SetTimer(TIMER_FOR_DRIVE_INFO,1000,NULL);
	CServer *pServer = GetServer();
	if (pServer)
	{
		pServer->GetDatabase()->RefreshInfo();
	}
}

