/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: processlist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/processlist.cpp $
// CHECKIN:		$Date: 21.04.06 9:13 $
// VERSION:		$Revision: 41 $
// LAST CHANGE:	$Modtime: 21.04.06 8:52 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"

#include "ProcessList.h"

/////////////////////////////////////////////////////////////////////////////
// CProcess
CProcess::CProcess()	
{
	Init();
}
/////////////////////////////////////////////////////////////////////////////
CProcess::CProcess(CSecID id)
{
	Init();
	m_id = id;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::Init()
{
	// m_id
	// m_sName;
	m_Type			= P_NOTYPE;

	m_eResolution	= RESOLUTION_2CIF;
	m_eCompression	= COMPRESSION_3;

	m_idPermission	= SECID_NO_ID;
	m_ArchivID		= SECID_NO_ID;
	m_bSetTime		= FALSE;

	m_dwMainTime	= 0;
	m_dwPauseTime   = 0;
	m_dwRecordOrCallTime	= 1000; // 1 s sind 1000 ms!
	m_dwConnectionTime = 0;

	m_wNrPict		= 1;
	m_bHold			= FALSE;
	m_bInterval		= FALSE;

	m_waCall.RemoveAll();
	m_waArchiv.RemoveAll();

	m_bDeleteAtSuccess = FALSE;
	m_dwBackupTimeSpan = BU_TS_ALL;
	// m_sDestination

	m_dwSwitchTime	= 0;
	m_bFullScreen	= FALSE;
	m_bSave			= FALSE;
	m_bExclusive	= FALSE;
	m_bDisconnect = TRUE;
	m_saCameras.RemoveAll();

	m_eSubType		= VOut_NoSubType;
	m_idGroupCard	= SECID_NO_ID;
	m_dwVOutOutput	= (DWORD)(-1); // Invalid Output
	//m_sCameras
}
/////////////////////////////////////////////////////////////////////////////
CProcess::~CProcess()	
{
}
/////////////////////////////////////////////////////////////////////////////
CProcess::ProcessType CProcess::String2Type(const CString& s)
{
	if (s == _T("isdn"))   return P_CALL;
	if (s == _T("camera")) return P_SAVE;
	if (s == _T("backup")) return P_BACKUP;
	if (s == _T("actualimage")) return P_ACTUAL_IMAGE;
	if (s == _T("imagecall")) return P_IMAGE_CALL;
	if (s == _T("checkcall")) return P_CHECK_CALL;
	if (s == _T("guardtour")) return P_GUARD_TOUR;
	if (s == _T("vout")) return P_VIDEO_OUT;
	if (s == _T("upload")) return P_UPLOAD;
	return P_NOTYPE;
}
/////////////////////////////////////////////////////////////////////////////
CString	CProcess::Type2String(ProcessType type)
{
	if (type == P_CALL) return _T("isdn");
	if (type == P_SAVE) return _T("camera");
	if (type == P_BACKUP) return _T("backup");
	if (type == P_ACTUAL_IMAGE) return _T("actualimage");
	if (type == P_IMAGE_CALL) return _T("imagecall");
	if (type == P_CHECK_CALL) return _T("checkcall");
	if (type == P_GUARD_TOUR) return _T("guardtour");
	if (type == P_VIDEO_OUT) return _T("vout");
	if (type == P_UPLOAD) return _T("upload");
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
CString	CProcess::VOutSubType2String(VOutSubType vOutSubType)
{
	CString sReturn;
	if (vOutSubType == VOut_Client) sReturn.LoadString(IDS_VOUT_CLIENT);
	if (vOutSubType == VOut_Sequencer) sReturn.LoadString(IDS_VOUT_SEQUENCER);
	if (vOutSubType == VOut_Activation) sReturn.LoadString(IDS_VOUT_ACTIVATION);
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::Load(int i, CWK_Profile& wkp)
{
	CString s = wkp.GetString(SEC_NAME_PROZESS,i,_T(""));
	BOOL bSaveChanges = FALSE;

	if (s.IsEmpty())
	{
		return FALSE;
	}
	m_Type = String2Type(wkp.GetStringFromString(s, INI_OUTPUT_TYPESTR, _T("")));
	m_id = (DWORD)wkp.GetHexFromString(s, INI_ID, 0);
	m_sName = wkp.GetStringFromString(s, INI_COMMENT, _T(""));

	CString sName;
	switch (m_Type)
	{
		case P_CHECK_CALL: sName.LoadString(IDS_CHECK_CALL); break;
		case P_IMAGE_CALL: sName.LoadString(IDS_IMAGE_CALL); break;
	}
	if (!sName.IsEmpty() && sName != m_sName)
	{
		m_sName      = sName;
		bSaveChanges = TRUE;
	}

	if (   IsCall() 
		|| IsSave()
		|| IsUpload())
	{
		m_dwMainTime = (DWORD)wkp.GetNrFromString(s, INI_MAIN_TIME, 0);
		m_dwPauseTime = (DWORD)wkp.GetNrFromString(s, INI_PAUSE_TIME, 0);
		m_dwRecordOrCallTime = (DWORD)wkp.GetNrFromString(s, INI_REC_TIME, 0);
		m_wNrPict = (WORD)wkp.GetNrFromString(s, INI_NR_OF_PICTURE, 0);

		m_bHold = (BOOL)wkp.GetNrFromString(s, INI_HOLD, 0);
		m_bInterval = (BOOL)wkp.GetNrFromString(s, INI_PROCESSINTERVAL, 0);

		if (IsCall())
		{
			SetPermission(CSecID( (DWORD)wkp.GetHexFromString(s, INI_PERMISSION, SECID_NO_ID)));
			m_dwConnectionTime = (DWORD)wkp.GetNrFromString(s,INI_OUTPUT_TYPE,0);
		}
		
	}

	// HEDU 011098 CAVEAT Do not drop the load to read old format
	// Save will not write these values
	if (IsSave())
	{
		m_eResolution  = (Resolution) wkp.GetNrFromString(s, INI_RESOLUTION, m_eResolution);
		m_eCompression = (Compression)wkp.GetNrFromString(s, INI_COMPRESSION, m_eCompression);

		WORD w,wID;
		CString sSub;
		w = 1;
		wID = SECID_NO_SUBID;
		BOOL bLoop = TRUE;
		while (bLoop)
		{
			sSub.Format(_T("\\L%u="),w++);
			wID = (WORD)wkp.GetNrFromString(s, sSub, SECID_NO_SUBID);
			if (wID!=SECID_NO_SUBID)
			{
				m_waArchiv.Add(wID);
			}
			else
			{
				break;
			}
		}
	}
	
	if (   IsCall() 
		|| IsCheckCall() 
		|| IsImageCall()
		|| IsGuardTour())
	{
		WORD w,wID;
		CString sSub;
		w = 1;
		wID = SECID_NO_SUBID;
		BOOL bLoop = TRUE;
		while (bLoop)
		{
			sSub.Format(_T("\\L%u="),w++);
			wID = (WORD)wkp.GetNrFromString(s, sSub, SECID_NO_SUBID);
			if (wID!=SECID_NO_SUBID)
			{
				m_waCall.Add(wID);
			}
			else
			{
				break;
			}
		}
	}

	if (IsBackup())
	{
		m_sDestination = wkp.GetStringFromString(s, INI_DRIVE,_T(""));
		m_bDeleteAtSuccess = wkp.GetNrFromString(s, INI_DELETE_AT_SUCC,FALSE);
		m_ArchivID = (DWORD)wkp.GetHexFromString(s, INI_B_ARCHIVE_ID,SECID_NO_ID);
		m_dwBackupTimeSpan = wkp.GetNrFromString(s, INI_BACKUP_TIMESPAN, BU_TS_ALL);
	}

	if (IsActualImage())
	{
		m_eResolution  = (Resolution) wkp.GetNrFromString(s, INI_RESOLUTION, m_eResolution);
		m_eCompression = (Compression)wkp.GetNrFromString(s, INI_COMPRESSION, m_eCompression);
	}

	if (IsImageCall())
	{
		SetPermission(CSecID( (DWORD)wkp.GetHexFromString(s, INI_PERMISSION, SECID_NO_ID)));
	}

	if (IsCheckCall())
	{
		SetPermission(CSecID( (DWORD)wkp.GetHexFromString(s, INI_PERMISSION, SECID_NO_ID)));
		m_bSetTime = wkp.GetNrFromString(s,_T("\\ST"),m_bSetTime);
	}
	if (IsGuardTour())
	{
		SetPermission(CSecID( (DWORD)wkp.GetHexFromString(s, INI_PERMISSION, SECID_NO_ID)));
		m_dwRecordOrCallTime = (DWORD)wkp.GetNrFromString(s, _T("\\TRY"), m_dwRecordOrCallTime);
		m_dwSwitchTime = wkp.GetNrFromString(s,_T("\\SWITCH"), m_dwSwitchTime);
		CString sControl = wkp.GetStringFromString(s, _T("\\CONTROL"), _T("LEVEL"));
		m_bHold = (sControl.CompareNoCase(_T("EDGE"))==0) ? TRUE : FALSE;
		m_bSave = wkp.GetNrFromString(s,_T("\\SAVE"), m_bSave);
		m_bFullScreen = wkp.GetNrFromString(s,_T("\\FS"), m_bFullScreen);
		m_bExclusive = wkp.GetNrFromString(s,_T("\\EXCLUSIVE"), m_bExclusive);
		m_bDisconnect = wkp.GetNrFromString(s,_T("\\DISCONNECT"), m_bExclusive);
		CString sSub, sCameras;
		for (WORD w=1 ; w!=0 ; w++)
		{
			sSub.Format(_T("\\C%u="), w);
			sCameras = wkp.GetStringFromString(s, sSub, _T(""));
			if (sCameras.IsEmpty())
			{	// error or end
				break;
			}
			else
			{
				m_saCameras.Add(sCameras);
			}
		}
		if (m_waCall.GetSize() != m_saCameras.GetSize())
		{	// error, something went wrong
			WK_TRACE_ERROR(_T("Load guard tour process %s call/cameras mismatch\n"), m_sName);
			return FALSE;
		}
	}
	if (IsVideoOut())
	{
		m_eSubType = (VOutSubType)wkp.GetNrFromString(s,_T("\\Subtype"), m_eSubType);
		WORD wGroupID = (WORD)wkp.GetHexFromString(s,_T("\\GROUP"), m_idGroupCard.GetGroupID());
		m_idGroupCard.Set(wGroupID, 0);
		m_dwVOutOutput = wkp.GetNrFromString(s,_T("\\VOUT"), m_dwVOutOutput);
		m_sCameras = wkp.GetStringFromString(s, _T("\\Cam"), _T(""));
		m_dwSwitchTime = wkp.GetNrFromString(s,_T("\\SWITCH"), m_dwSwitchTime);
	}
	
	if (IsUpload())
	{
		m_sServerName = wkp.GetStringFromString(s,_T("\\SERVER"), _T(""));
		m_sLogin = wkp.GetStringFromString(s,_T("\\LOGIN"), _T(""));
		CString sPassword = wkp.GetStringFromString(s,_T("\\PWD"), _T(""));
		wkp.Decode(sPassword);
		m_sPassword = sPassword;
		m_sFilename = wkp.GetStringFromString(s,_T("\\FILE"), _T(""));
	}
	if (bSaveChanges)
	{
		Save(i-1, wkp);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::Save(int i, CWK_Profile& wkp)
{
	CString sType;
	CString s,s1;
	WORD wID;

	sType = Type2String(m_Type);

	if (IsCall() || IsSave() || IsUpload())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\MT%lu\\RT%lu\\PT%lu\\NP%u\\IT%u\\H%u\\RS%u\\CP%u"), 
					m_id.GetID(),
					m_sName, 
					sType, 
					m_dwMainTime, 
					m_dwRecordOrCallTime, 
					m_dwPauseTime, 
					m_wNrPict, 
					m_bInterval, 
					m_bHold, 
					(int)m_eResolution, 
					(int)m_eCompression
					);
		if (IsCall())
		{
			CString s1;
			s1.Format(_T("\\CT%d"),m_dwConnectionTime);
			s += s1;
		}
	}
	else if (IsBackup())
	{
		
		s.Format(INI_ID _T("%x") INI_COMMENT _T("%s") \
				 INI_OUTPUT_TYPESTR _T("%s") INI_DRIVE _T("%s") \
				 INI_B_ARCHIVE_ID _T("%08lx") \
				 INI_DELETE_AT_SUCC _T("%u") \
				 INI_BACKUP_TIMESPAN _T("%u"),
//		_T("\\ID%x\\CO%s\\TY%s\\DRIVE%s\\BARC%08lx\\BDAS%u"),
					m_id.GetID(), m_sName, 
					sType, m_sDestination,
					m_ArchivID.GetID(),
					m_bDeleteAtSuccess,
					m_dwBackupTimeSpan);
	}
	else if (IsActualImage())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\RS%u\\CP%u"),
					m_id.GetID(),
					m_sName, 
					sType, 
					(int)m_eResolution, 
					(int)m_eCompression);
	}
	else if (IsImageCall())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\PM%08lx"),
					m_id.GetID(),
					m_sName, 
					sType, 
					m_idPermission.GetID());
	}
	else if (IsCheckCall())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\PM%08lx\\ST%d"),
					m_id.GetID(),
					m_sName, 
					sType, 
					m_idPermission.GetID(),
					m_bSetTime);
	}
	else if (IsGuardTour())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\PM%08lx\\TRY%d\\SWITCH%d\\CONTROL%s\\SAVE%d\\FS%d\\EXCLUSIVE%d\\DISCONNECT%d"),
					m_id.GetID(),
					m_sName, 
					sType, 
					m_idPermission.GetID(),
					m_dwRecordOrCallTime,
					m_dwSwitchTime,
					m_bHold ? _T("Edge") : _T("Level"),
					m_bSave,
					m_bFullScreen,
					m_bExclusive,
					m_bDisconnect
					);
		CString sCamera;
		for (int i=0 ; i<m_saCameras.GetSize() ; i++)
		{
			sCamera = m_saCameras.GetAt(i);
			if (!sCamera.IsEmpty())
			{
				s1.Format(_T("\\C%u=%s"), i+1, sCamera);	
				s += s1;
			}
		}
	}
	else if (IsVideoOut())
	{
		s.Format(_T("\\ID%x\\CO%s\\TY%s\\Subtype%u\\GROUP%04x\\VOUT%u\\Cam%s\\SWITCH%u"),
					m_id.GetID(),
					m_sName, 
					sType, 
					m_eSubType,
					m_idGroupCard.GetGroupID(),
					m_dwVOutOutput,
					m_sCameras,
					m_dwSwitchTime
					);
	}

	if (IsCall())
	{
		s1.Format(_T("\\PM%08lx"),m_idPermission.GetID());
		s += s1;
	}

	if (IsUpload())
	{
		CString sPassword = m_sPassword;
		wkp.Encode(sPassword);
		s1.Format(_T("\\SERVER%s\\LOGIN%s\\PWD%s\\FILE%s"),
			m_sServerName,m_sLogin,sPassword,m_sFilename);
		s += s1;
	}

	if (   IsCall() 
		|| IsCheckCall() 
		|| IsImageCall()
		|| IsGuardTour())
	{
		for (int j=0;j<m_waCall.GetSize();j++)
		{
			wID = m_waCall.GetAt(j);
			if (wID != SECID_NO_SUBID)
			{
				s1.Format(_T("\\L%u=%u"), j+1, wID);	
				s += s1;
			}
		}
	}
	wkp.WriteStringIndex(SEC_NAME_PROZESS, i+1, s); 
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetName(const CString& sNewName)
{
	m_sName = sNewName;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetCall(const CString& sName,
					   DWORD dwDuration, 
					   DWORD dwPause,
					   DWORD dwCallTime,
					   BOOL  bHold,
					   BOOL  bInterval,
					   CSecID	idPermission,
					   DWORD dwConnectionTime)
{
	m_Type  = P_CALL;
	m_sName = sName;
	m_dwMainTime  = dwDuration;
	m_dwPauseTime = dwPause;
	m_dwRecordOrCallTime  = dwCallTime;
	m_bHold = bHold;
	m_bInterval = bInterval;
	m_idPermission = idPermission;
	m_dwConnectionTime = dwConnectionTime;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetSave(	const CString& sName,
						DWORD dwDuration, 
						DWORD dwPause,
						WORD  wNrPict,
						DWORD dwRecTime,
						Resolution res,
						Compression comp,
						BOOL  bHold,
						BOOL  bInterval)
{
	m_Type = P_SAVE;
	m_sName = sName;
	m_dwMainTime  = dwDuration;
	m_dwPauseTime = dwPause;
	m_wNrPict = wNrPict;
	m_dwRecordOrCallTime = dwRecTime;
	m_eResolution = res;
	m_eCompression = comp;
	m_bHold = bHold;
	m_bInterval = bInterval;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetBackup(const CString& sName,
						 const CString& sDestination,
						 CSecID idArchive, 
						 BOOL bDeleteAtSuccess,
						 DWORD dwBackupTimeSpan)
{
	m_Type  = P_BACKUP;
	m_sName = sName;
	m_sDestination = sDestination;
	m_ArchivID = idArchive;
	m_bDeleteAtSuccess = bDeleteAtSuccess;
	m_dwBackupTimeSpan = dwBackupTimeSpan;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetActualImage(const CString& sName,Resolution res, Compression comp)
{
	m_Type = P_ACTUAL_IMAGE;
	m_sName = sName;
	m_eResolution = res;
	m_eCompression = comp;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetImageCall(const CString& sName,CSecID idPermission)
{
	m_Type = P_IMAGE_CALL;
	m_sName = sName;
	m_idPermission = idPermission;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetCheckCall(const CString& sName,CSecID idPermission,BOOL bSetTime)
{
	m_Type = P_CHECK_CALL;
	m_sName = sName;
	m_idPermission = idPermission;
	m_bSetTime = bSetTime;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetGuardTour(const CString&		sName,
							const CSecID&		idPermission,
							const DWORD&		dwTries,
							const DWORD&		dwSwitchTime,
							const BOOL&			bHold,
							const BOOL&			bSave,
							const BOOL&			bFullScreen,
							const BOOL&			bExclusive,
							const BOOL&			bDisconnect,
							const CWordArray&	waHosts,
							const CStringArray&	saCameras)
{
	m_Type = P_GUARD_TOUR;
	m_sName = sName;
	m_idPermission = idPermission;
	m_dwRecordOrCallTime = dwTries;
	m_dwSwitchTime = dwSwitchTime;
	m_bHold = bHold;
	m_bSave = bSave;
	m_bFullScreen = bFullScreen;
	m_bExclusive = bExclusive;
	m_bDisconnect = bDisconnect;
	m_waCall.RemoveAll();
	for (int i=0 ; i< waHosts.GetSize() ; i++)
	{
		m_waCall.Add(waHosts.GetAt(i));
	}
	m_saCameras.RemoveAll();
	for (i=0 ; i< waHosts.GetSize() ; i++)
	{
		m_saCameras.Add(saCameras.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::GetGuardTourData(CString&		sName,
								CSecID&			idPermission,
								DWORD&			dwTries,
								DWORD&			dwSwitchTime,
								BOOL&			bHold,
								BOOL&			bSave,
								BOOL&			bFullScreen,
								BOOL&			bExclusive,
								BOOL&			bDisconnect,
								CWordArray&		waHosts,
								CStringArray&	saCameras)
{
	BOOL bOK = FALSE;
	if (m_Type == P_GUARD_TOUR)
	{
		bOK = TRUE;
		sName = m_sName;
		idPermission = m_idPermission;
		dwTries = m_dwRecordOrCallTime;
		dwSwitchTime = m_dwSwitchTime;
		bHold = m_bHold;
		bSave = m_bSave;
		bFullScreen = m_bFullScreen;
		bExclusive = m_bExclusive;
		bDisconnect = m_bDisconnect;
		waHosts.RemoveAll();
		for (int i=0 ; i< m_waCall.GetSize() ; i++)
		{
			waHosts.Add(m_waCall.GetAt(i));
		}
		saCameras.RemoveAll();
		for (i=0 ; i< m_saCameras.GetSize() ; i++)
		{
			saCameras.Add(m_saCameras.GetAt(i));
		}
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::SetVideoOut(const CString&		sName,
						   const VOutSubType&	subType,
						   const CSecID&		idGroupCard,
						   const DWORD&			dwOutput,
						   const CString&		sCameras,
						   const DWORD&			dwDwellTime)
{
	m_Type = P_VIDEO_OUT;
	m_sName = sName;
	m_eSubType = subType;
	m_idGroupCard = idGroupCard;
	m_dwVOutOutput = dwOutput;
	m_sCameras = sCameras;
	m_dwSwitchTime = dwDwellTime;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::GetVideoOut(CString&		sName,
						   VOutSubType&	subType,
						   CSecID&		idGroupCard,
						   DWORD&		dwOutput,
						   CString&		sCameras,
						   DWORD&		dwDwellTime)
{
	BOOL bOK = FALSE;
	if (m_Type == P_VIDEO_OUT)
	{
		bOK = TRUE;
		sName = m_sName;
		subType = m_eSubType;
		idGroupCard = m_idGroupCard;
		dwOutput = m_dwVOutOutput;
		sCameras = m_sCameras;
		dwDwellTime = m_dwSwitchTime;
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::SetUpload(const CString& sName,
						DWORD dwDuration,
						DWORD dwRecTime,
						Resolution res,
						Compression comp,
						BOOL  bHold,
						const CString& sServerName,
						const CString& sLogin,
						const CString& sPassword,
						const CString& sFilename)
{
	m_Type = P_UPLOAD;
	
	m_sName = sName;
	m_sName.Replace(_T('\\'),_T('/'));
	
	m_sLogin = sLogin;
	m_sLogin.Replace(_T('\\'),_T('/'));
	
	m_sPassword = sPassword;
	m_sPassword.Replace(_T('\\'),_T('/'));
	
	m_sServerName = sServerName;
	m_sServerName.Replace(_T('\\'),_T('/'));

	m_sFilename = sFilename;
	m_sFilename.Replace(_T('\\'),_T('/'));

	m_eResolution = res;
	m_eCompression = comp;

	m_bHold = bHold;
	m_bInterval = TRUE;
	m_dwMainTime = dwDuration;
	
	m_wNrPict = 1;
	m_dwRecordOrCallTime = 1000;
	m_dwPauseTime = (dwRecTime-1)*1000;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::ClearCalls()
{
	m_waCall.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::AddCall(WORD wCall)
{
	m_waCall.Add(wCall);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::HasCall(WORD wCall) const
{
	for (int i=0;i<m_waCall.GetSize();i++)
	{
		if (m_waCall.GetAt(i)==wCall)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::ClearCall(WORD wCall)
{
	for (int i=0;i<m_waCall.GetSize();i++)
	{
		if (m_waCall.GetAt(i)==wCall)
		{
			m_waCall.RemoveAt(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::ClearArchivs()
{
	m_waArchiv.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::AddArchiv(WORD wArchiv)
{
	m_waArchiv.Add(wArchiv);
}
/////////////////////////////////////////////////////////////////////////////
// CProcessList
/////////////////////////////////////////////////////////////////////////////
CProcessList::CProcessList()
{
}
/////////////////////////////////////////////////////////////////////////////
CProcessList::~CProcessList()
{
	for (int i=0;i<GetSize();i++)
	{
		CProcess* p = GetAt(i);	
		WK_DELETE(p);
	}
	RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
CProcess* CProcessList::GetProcess(CSecID id) const
{
	CProcess *pProcess;

	for (int i=0;i<GetSize();i++) 
	{
		pProcess = GetAt(i);
		if (pProcess && id==pProcess->GetID()) 
		{
			return pProcess;		// EXIT
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CProcess* CProcessList::AddProcess()
{
	CProcess* pProcess;
	CSecID id(SECID_GROUP_PROCESS,1);
	
	BOOL bLoop = TRUE;
	while (bLoop)
	{
		pProcess = GetProcess(id);
		if (!pProcess)
		{
			break;
		}
		id = id.GetID() + 1; 
	}
	
	pProcess = new CProcess(id);
	Add(pProcess);

	return pProcess;
}
/////////////////////////////////////////////////////////////////////////////
void CProcessList::DeleteProcess(CProcess* pProcess)
{
	int i,c;
	CProcess* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = (CProcess*)GetAt(i);
		if (pProcess==pSearch)
		{
			// found
			RemoveAt(i);
			WK_DELETE(pProcess);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// Alle Prozesse werden gelöscht
void CProcessList::Reset()
{
	int i,c;
	CProcess* pProcess;

	c = GetSize();
	for (i=0;i<c;i++) 
	{
		pProcess = (CProcess*)GetAt(i);
		WK_DELETE(pProcess);

	} 
	RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CProcessList::Load(CWK_Profile& wkp)
{
	CProcess* pProcess = NULL;
	CString sMax = wkp.GetString(SEC_NAME_PROZESS,ENT_NAME_MAX_PROZESS,_T("0"));
	int c = _ttoi(sMax);

	for (int i=1;i<=c;i++) 
	{	
		pProcess = new CProcess();
		if (pProcess->Load(i,wkp))
		{
			Add(pProcess);
		}
		else
		{
			WK_DELETE(pProcess);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcessList::Save(CWK_Profile& wkp)
{
	// Save the processes from the list
	CProcess* pProcess;
	int c = GetSize();
	int i;
	CString s1;
	CString sType;

	wkp.DeleteSection(SEC_NAME_PROZESS);

	// Save new count of processes
	CString sNewMax;
	sNewMax.Format(_T("%d"),c);
	wkp.WriteString(SEC_NAME_PROZESS, ENT_NAME_MAX_PROZESS, sNewMax);

	for (i=0 ; i<c ; i++)
	{
		pProcess = GetAt(i);
		if (pProcess)
		{
			pProcess->Save(i,wkp);
		}
	}	// end of loop
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::GetCallArray(CWordArray& waHosts)  const
{
	waHosts.RemoveAll();
	for (int i=0 ; i< m_waCall.GetSize() ; i++)
	{
		waHosts.Add(m_waCall.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::GetCameraArray(CStringArray&	saCameras) const
{
	saCameras.RemoveAll();
	for (int i=0 ; i< m_saCameras.GetSize() ; i++)
	{
		saCameras.Add(m_saCameras.GetAt(i));
	}
}

void CProcess::SetCallTime(DWORD dw)
{
	m_dwRecordOrCallTime = dw;
}

void CProcess::SetSwitchTime(DWORD dw)
{
   m_dwSwitchTime = dw;
}

void CProcess::SetFullscreen(BOOL b)
{
   m_bFullScreen = b;
}

void CProcess::SetSave(BOOL b)
{
	m_bSave = b;
}

void CProcess::SetExclusive(BOOL b)
{
   m_bExclusive = b;
}
void CProcess::SetDisconnect(BOOL b)
{
	m_bDisconnect = b;
}

void CProcess::SetHold(BOOL b)
{
   m_bHold = b;
}
