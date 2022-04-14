/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ProcessMacro.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/ProcessMacro.cpp $
// CHECKIN:		$Date: 26.04.06 9:09 $
// VERSION:		$Revision: 63 $
// LAST CHANGE:	$Modtime: 25.04.06 20:37 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "sec3.h"
#include "ProcessMacro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPCTSTR NameOfEnum(ProcessMacroType type)
{
	switch (type) 
	{
		NAME_OF_ENUM(PMT_INVALID);
		NAME_OF_ENUM(PMT_RECORD);
		NAME_OF_ENUM(PMT_ALARM_CALL);
		NAME_OF_ENUM(PMT_RELAY);
		NAME_OF_ENUM(PMT_BACKUP);
		NAME_OF_ENUM(PMT_ACTUAL_IMAGE);
		NAME_OF_ENUM(PMT_IMAGE_CALL);
		NAME_OF_ENUM(PMT_CHECK_CALL);
		NAME_OF_ENUM(PMT_GUARD_TOUR);
		NAME_OF_ENUM(PMT_VIDEO_OUT);
		NAME_OF_ENUM(PMT_AUDIO);
		NAME_OF_ENUM(PMT_PTZ_PRESET);
		NAME_OF_ENUM(PMT_ALARM_LIST);
		NAME_OF_ENUM(PMT_UPLOAD);
	}
	return _T("");
}

ProcessMacroType CProcessMacro::TypeFromString(const CString &s)
{
	if (s==_T("relay")) return PMT_RELAY;
	else if (s==_T("isdn")) return PMT_ALARM_CALL;
	else if (s==_T("camera")) return PMT_RECORD;
	else if (s==_T("backup")) return PMT_BACKUP;
	else if (s==_T("actualimage")) return PMT_ACTUAL_IMAGE;
	else if (s==_T("imagecall")) return PMT_IMAGE_CALL;
	else if (s==_T("checkcall")) return PMT_CHECK_CALL;
	else if (s==_T("guardtour")) return PMT_GUARD_TOUR;
	else if (s==_T("vout")) return PMT_VIDEO_OUT;
	else if (s==_T("upload")) return PMT_UPLOAD;
	else {
		TRACE(_T("invalid process macro type %s\n"),s);
		return PMT_INVALID;
	}
}

BOOL CProcessMacro::IsValidMacro() const
{
	// NOT YET error messages should print the Macro itself
	BOOL bOkay = TRUE;
	// CAVEAT all if's only set FALSE, so order is not important
	if (GetType()==PMT_RECORD) 
	{
		// NOT YET min record time (in ms)
		if (m_dwRecordTime==0) {
			bOkay=FALSE;
			WK_TRACE_ERROR(_T("ProcessMacro: %s invalid recordTime\n"),(LPCTSTR)m_sComment,m_dwRecordTime);
		}
		if (m_dwNumPictures==0) {
			bOkay=FALSE;
			WK_TRACE_ERROR(_T("ProcessMacro: %s invalid numPictures\n"),(LPCTSTR)m_sComment,m_dwNumPictures);
		}

		// Dropped for 4.0
		/*
		if (m_IDs.GetSize()==0) {
			bOkay=FALSE;
			WK_TRACE(_T("ProcessMacro: %s has no archives\n"),(LPCTSTR)m_sComment);
		}
		*/
	} 
	else if (GetType()==PMT_ALARM_CALL) 
	{
		// NOT YET all combinations
		if (m_pPermission==NULL) 
		{
			bOkay=FALSE;
			WK_TRACE_ERROR(_T("Could not find Permission for AlamCall\n"));
		}
	}
	else if (GetType()==PMT_RELAY) 
	{
		// there are exactly TWO standard processes
		DWORD dwID = GetID().GetID();
		if (dwID!=SECID_PROCESS_RELAY_EDGE_CONTROL
			&& dwID!=SECID_PROCESS_RELAY_STATE_CONTROL) {
			bOkay=FALSE;
			WK_TRACE_ERROR(_T("Invalid Relay Standard ProcessMacro[%x]: %s\n"),
				dwID,
				(LPCTSTR)m_sComment
				);
		}
	} 
	else if (GetType()==PMT_BACKUP) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_ACTUAL_IMAGE) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_IMAGE_CALL) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_CHECK_CALL) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_GUARD_TOUR) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_VIDEO_OUT) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_ALARM_LIST) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_UPLOAD) 
	{
		// TODO VALIDATE Makro
		bOkay = TRUE;
	}
	else if (GetType()==PMT_INVALID) 
	{
		bOkay=FALSE;
		WK_TRACE_ERROR(_T("ProcessMacro: %s invalid type\n"),(LPCTSTR)m_sComment);
	}
	// NOT YET other types
	return bOkay;
}

CProcessMacro::CProcessMacro(ProcessMacroType t, 
							 const CString& sComment,
							 CSecID id)
{
	m_Type = t;
	m_sComment = sComment;
	m_ID = id;

	m_bKeepAlive = FALSE;

	m_bIsIntervall=FALSE;
	m_dwPauseTime=0;
	m_dwOverallTime=0;	// can be infinity
	// picture parameter
	m_Compression = COMPRESSION_NONE;
	m_Resolution = RESOLUTION_NONE;
	//
	m_dwRecordTime=0;
	m_dwNumPictures=0;
	m_dwRecordSlice=0;
	m_dwPicsPerSlice=0;
	m_dwNumExpectedPictures = 0;
	m_dwConnectionTime = 0;

	// m_IDs
	m_pArchiveIDsAsBytes = NULL;
	//
	m_pPermission = NULL;

	// 
	m_bDeleteAtSuccess = FALSE;
	m_dwBackupTimeSpan = BU_TS_ALL;
	// m_ArchivID;			// backup archiv ID
	// m_sDestination;

	m_bSetTime = FALSE;
	m_bGTFullscreen = FALSE;
	m_bGTExclusive = FALSE;
	m_bGTSave = FALSE;
	m_bGTDisconnect = TRUE;

	m_iVideoOutPort = 0;
	m_iVideoOutSubtype = 0;
	m_wGroupID = SECID_NO_GROUPID;
}
CProcessMacro::~CProcessMacro()
{
	WK_DELETE_ARRAY(m_pArchiveIDsAsBytes);
}

CString CProcessMacro::CreateDumpString() const
{
	CString s;
	CString sIntervall;
	CString sOverallTime;
	if (m_bIsIntervall) {
		sIntervall = _T("Intervall");
	} else {
		sIntervall = _T("Non-Intervall");
	}

	// format overallTime text, avoid the -1 for infinity
	if (m_dwOverallTime==CPM_TIME_INFINITY) {
		sOverallTime = _T("infinity");
	} else {
		sOverallTime.Format(_T("%d"),m_dwOverallTime);
	}

	if (GetType() == PMT_RECORD) 
	{
		s.Format(_T("ProcessMacro %s[%x]: %s %s keepAlive %d\n\
			\tRecordTime %d NumPictures %d slice %d[%d], expectedPictures %d\n\
			\t%s PauseTime %d OverallTime %s"),
			m_sComment, m_ID.GetID(),NameOfEnum(m_Compression), NameOfEnum(m_Resolution), m_bKeepAlive,
			m_dwRecordTime, m_dwNumPictures, m_dwRecordSlice, m_dwPicsPerSlice, 
			m_dwNumExpectedPictures,
			sIntervall, m_dwPauseTime, (LPCTSTR)sOverallTime
			);
	}
	else if (GetType() == PMT_ALARM_CALL) 
	{
		s.Format(_T("ProcessMacro %s[%x] (call): %s %s keepAlive %d RecordTime %d \n\
			\t%s PauseTime %d OverallTime %s"),
			m_sComment, m_ID.GetID(),NameOfEnum(m_Compression), NameOfEnum(m_Resolution), m_bKeepAlive,
			m_dwRecordTime, 
			sIntervall, m_dwPauseTime, (LPCTSTR)sOverallTime
			);
	}
	else if (GetType() == PMT_BACKUP) 
	{
		s.Format(_T("ProcessMacro %s[%08lx] archiv %08lx drive %s delete %s timespan %d"),
			m_sComment,
			m_ID.GetID(),
			m_ArchivID.GetID(),
			m_sDestination,
			m_bDeleteAtSuccess ? _T("yes") : _T("no"),
			m_dwBackupTimeSpan);
	}
	else if (GetType() == PMT_ACTUAL_IMAGE)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x]"),m_sComment,m_ID.GetID());
	}
	else if (GetType() == PMT_IMAGE_CALL)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x]"),m_sComment,m_ID.GetID());
	}
	else if (GetType() == PMT_CHECK_CALL)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x]"),m_sComment,m_ID.GetID());
	}
	else if (GetType() == PMT_GUARD_TOUR)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x] Fullscreen=%d, Exclusive=%d,Save=%d,Disconnect=%d"),m_sComment,m_ID.GetID(),
			m_bGTFullscreen,m_bGTExclusive,m_bGTSave,m_bGTDisconnect);
	}
	else if (GetType() == PMT_VIDEO_OUT)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x] Type %d, Port %d, Card %x"),m_sComment,m_ID.GetID(),
			m_iVideoOutSubtype,m_iVideoOutPort,m_wGroupID);
	}
	else if (GetType() == PMT_ALARM_LIST)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x]"),m_sComment,m_ID.GetID());
	}
	else if (GetType() == PMT_UPLOAD)
	{
		// TODO
		s.Format(_T("ProcessMacro %s[%x]: %s %s keepAlive %d RecordTime %d NumPictures %d slice %d[%d], expectedPictures %d %s PauseTime %d OverallTime %s"),
					m_sComment, m_ID.GetID(),NameOfEnum(m_Compression), NameOfEnum(m_Resolution), m_bKeepAlive,
					m_dwRecordTime, m_dwNumPictures, m_dwRecordSlice, m_dwPicsPerSlice, 
					m_dwNumExpectedPictures,
					sIntervall, m_dwPauseTime, (LPCTSTR)sOverallTime
					);
		s += _T(" Server:")+m_sServerName;
		s += _T(" Login:")+m_sLogin;
		s += _T(" Filename:")+m_sFilename;
	}
	else if (GetType() == PMT_RELAY)
	{
		// NOT YET RELAY
		s.Format(_T("ProcessMacro %s[%x]: ...\n"),m_sComment, m_ID.GetID() );
	}
	else 
	{
		s = _T("invalid process macro");
	}
	return s;
}

static void CalcSlice(DWORD dwRecordTime, DWORD dwNumPictures,
					  DWORD &dwRecordSlice, DWORD &dwPicsPerSlice)
{
	if (dwRecordTime<=1000) {
		// the simple case just one second
		dwRecordSlice = dwRecordTime;
		dwPicsPerSlice = dwNumPictures;
	} else {
		// OOPS really all cases caught ?
		DWORD x=1;
		do {
			// actually rec / (num/x)
			dwRecordSlice = dwRecordTime * x / dwNumPictures;	
			dwPicsPerSlice = x;
			x++;
		} while (dwRecordSlice<1000 && x<=dwNumPictures);
	}
}

#if 1
void TestCalcSlice()
{
#define SLICE(rec,num)	CalcSlice(rec,num,slice,pics); \
						WK_TRACE(_T("CalcSlice recordTime %d pics %d --> slice %d picsPerSlice %d\n"), \
						rec,num,slice,pics);
	DWORD slice,pics;	// output
	// SLICE(recordTime, pics);
	SLICE(	 1000,	 1);
	SLICE(	 1000,	12);
	SLICE(	 5*60*1000,	5);
	SLICE(	 5*1000,	5);
	SLICE(	 5*1000,	500);
}
#endif

void CProcessMacro::SetRecordValues(DWORD dwRecordTime, DWORD dwNumPictures)
{
	m_dwRecordTime = dwRecordTime;
	m_dwNumPictures = dwNumPictures;

	if (m_dwRecordTime && m_dwNumPictures) {
		// OOPS what about INFINITY?
		if (m_dwRecordTime<1000) {
			WK_TRACE(_T("ProcessMacro %s:minimal record time is 1 second, adjusted\n"),
						(LPCTSTR)GetName());
			m_dwRecordTime=1000;
		}
		
		CalcSlice(m_dwRecordTime,m_dwNumPictures,
					m_dwRecordSlice, m_dwPicsPerSlice);

		if (GetType()==PMT_RECORD ) {
			if (IsIntervall()) {
				if (m_dwOverallTime!=CPM_TIME_INFINITY) {
					// take the actice slice time PLUS the PauseTime
					m_dwNumExpectedPictures = (m_dwOverallTime/(m_dwRecordSlice+GetPauseTime()))*m_dwPicsPerSlice;
				} else {
					// unlimited (solange EingangsStatus erhalten)
					// keine Aussage moeglich
					m_dwNumExpectedPictures = 0;
				}
			} else {
				// non intervall
				m_dwNumExpectedPictures =  (m_dwRecordTime/m_dwRecordSlice)*m_dwPicsPerSlice;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CProcessMacro::SetGuardTourValues(DWORD dwDwellTime,
										DWORD dwNrOfTries,
										BOOL bFullscreen,
										BOOL bExclusive,
										BOOL bSave,
										BOOL bDisconnect,
										const CWordArray& idHosts,
										const CStringArray& sCameras)
{
	m_dwRecordTime = dwDwellTime;
	m_dwPicsPerSlice = dwNrOfTries;
	m_bGTFullscreen = bFullscreen;
	m_bGTExclusive = bExclusive;
	m_bGTSave = bSave;
	m_bGTDisconnect = bDisconnect;
	SetIDs(idHosts);
	m_sGTCameras.RemoveAll();
	m_sGTCameras.Append(sCameras);
}
/////////////////////////////////////////////////////////////////////
void CProcessMacro::SetVideoOutValues(int iVideoPort,
									   int iSubtype,
									   DWORD dwDwellTime,
									   WORD wGroupID,
									   const CString& sCameras)
{
	m_iVideoOutPort = iVideoPort;
	m_iVideoOutSubtype = iSubtype;
	m_dwRecordTime = dwDwellTime;
	m_wGroupID = wGroupID;

	CStringArray saCameras;
	SplitString(sCameras,saCameras,_T(';'));
	SeparatSAToWA(saCameras,m_IDs,_T('-'));
	for (int i=0;i<m_IDs.GetSize();i++)
	{
		m_IDs[i]--;
		// TRACE(_T("%d-%d\n"),i,m_IDs[i]);
	}
}
void CProcessMacro::SetUploadValues(const CString& sServerName,
									const CString& sLogin,
									const CString& sPassword,
									const CString& sFilename)
{
	m_sServerName = sServerName;
	m_sLogin = sLogin;
	m_sPassword = sPassword;
	m_sFilename = sFilename;

}


void CProcessMacro::SetIDs(const CWordArray &ids)
{
	// convert to SavePicture format
	m_IDs.SetSize(0);
	WK_DELETE_ARRAY(m_pArchiveIDsAsBytes);
	if (ids.GetSize()) 
	{
		m_pArchiveIDsAsBytes= new BYTE[ids.GetSize()];
	}
	for (int a=0;a<ids.GetSize();a++) 
	{								
		m_IDs.Add(ids[a]);
		m_pArchiveIDsAsBytes[a]=LOBYTE(ids[a]);
	}
}

void CProcessMacro::SetOverallTime(DWORD dwTime)
{
	m_dwOverallTime=dwTime;
	if (m_dwOverallTime==0) {
		m_dwOverallTime=CPM_TIME_INFINITY;
	}
}
void CProcessMacro::SetConnectionTime(DWORD dwTime)
{
	m_dwConnectionTime = dwTime;
}

void CProcessMacro::SetPauseTime(DWORD dwTime)
{
	m_dwPauseTime =dwTime;
}

void CProcessMacro::SetIsIntervall(BOOL bIntervall)
{
	m_bIsIntervall = bIntervall;
}
void CProcessMacro::SetKeepAlive(BOOL bKeepAlive)
{
	m_bKeepAlive= bKeepAlive;
}
void CProcessMacro::SetPictureParameter(Compression comp, Resolution res)
{
	m_Compression = comp;
	m_Resolution = res;
}

void CProcessMacro::SetResolution(Resolution res)
{
	m_Resolution = res;
}


void CProcessMacro::SetPermission(const CPermission *pPermission)
{
	m_pPermission = pPermission;
}

void CProcessMacro::SetBackup(CSecID idBackupArchive,
							  const CString& sDestination,
							  BOOL bDeleteAtSuccess,
							  DWORD dwBackupTimeSpan)
{
	m_bKeepAlive = TRUE;
	m_dwOverallTime = CPM_TIME_INFINITY;
	m_ArchivID = idBackupArchive;
	m_sDestination = sDestination;
	m_bDeleteAtSuccess = bDeleteAtSuccess;
	m_dwBackupTimeSpan = dwBackupTimeSpan;
}
void CProcessMacro::SetSetTime(BOOL bSetTime)
{
	m_bSetTime = bSetTime;
}
////////////////////////////////////////////////////////////////////////////
const CProcessMacro *CProcessMacros::GetMacroByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (GetAt(i)->GetID()==id) 
		{
			return GetAt(i);	// EXIT
		}
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////
void CProcessMacros::Load(CPermissionArray& permissions)
{
	DeleteAll();

	CWK_Profile wkp;
	CString s = wkp.GetString(_T("ProcessMacros"), _T("MaxProzess"), _T("0"));
	int iMaxProz = _ttoi(s);

	for (int i=1;i<=iMaxProz;i++) 
	{	
		CString sEntry = wkp.GetString(_T("ProcessMacros"), i, _T(""));
		if (sEntry.GetLength()) 
		{
			CString sType = CWK_Profile::GetStringFromString(sEntry, INI_OUTPUT_TYPESTR,_T(""));
			CProcessMacro *pMacro = new CProcessMacro(CProcessMacro::TypeFromString(sType),
													  CWK_Profile::GetStringFromString(sEntry, INI_COMMENT, _T("")),
													  CSecID( (DWORD)CWK_Profile::GetHexFromString(sEntry, _T("\\ID"), 0)));
			if (pMacro->GetID()==SECID_NO_ID) 
			{
				WK_TRACE_ERROR(_T("OOPS process macro has invalid id %x\n"),
					pMacro->GetID().GetID());
			}
			
			pMacro->SetOverallTime ((DWORD)CWK_Profile::GetNrFromString(sEntry, INI_MAIN_TIME, CPM_TIME_INFINITY));
			pMacro->SetPauseTime( (DWORD)CWK_Profile::GetNrFromString(sEntry, INI_PAUSE_TIME, 0) );
			DWORD dwRecordTime = (DWORD)CWK_Profile::GetNrFromString(sEntry, INI_REC_TIME, 0);
			DWORD dwNumPictures = (WORD)CWK_Profile::GetNrFromString(sEntry, INI_NR_OF_PICTURE, 0);
			
			if (pMacro->GetType()==PMT_ALARM_CALL) 
			{
				dwNumPictures=1;	// to get one large slice
				if (dwRecordTime<10000) 
				{	// fitting to FetchAlarmConnection
					dwRecordTime=10000;
					WK_TRACE_WARNING(_T("ProcessMacro: %s, min call time is 10 sec. Adjusted!\n"),(LPCTSTR)pMacro->GetName());
				}
				DWORD dwPermission = (DWORD)CWK_Profile::GetHexFromString(sEntry, INI_PERMISSION, SECID_NO_ID);
				const CPermission *pPermission;
				pPermission = permissions.GetPermission(CSecID(dwPermission));

				if (pPermission==NULL) 
				{	// OOPS for now take SuperVisor if unknown
					pPermission = permissions.GetPermission(_T("SuperVisor"));
				}
				if (pPermission==NULL) 
				{
					WK_TRACE_ERROR(_T("failed to get AlarmPermission for 0x%x\n"),dwPermission);
				}
				pMacro->SetPermission(pPermission);
				pMacro->SetConnectionTime(wkp.GetNrFromString(sEntry,_T("\\CT"),0));
			}
			
			pMacro->SetKeepAlive( (CWK_Profile::GetNrFromString(sEntry, _T("\\H"), 0)!=0) );
			pMacro->SetIsIntervall ( (CWK_Profile::GetNrFromString(sEntry, _T("\\IT"), 0)!=0) );
			// CAVEAT order is important SetRecordValues makes use of m_bIsInterval etc.			
			pMacro->SetRecordValues(dwRecordTime,dwNumPictures);

			pMacro->SetPictureParameter( (Compression)CWK_Profile::GetNrFromString(sEntry, _T("\\CP"), COMPRESSION_NONE),
				(Resolution)CWK_Profile::GetNrFromString(sEntry, _T("\\RS"), RESOLUTION_NONE) );

			// compression not used for B&W
			switch (pMacro->GetCompression()) 
			{
				case COMPRESSION_1: 
					pMacro->SetPictureParameter(COMPRESSION_5,pMacro->GetResolution()); 
					break;
				case COMPRESSION_2: 
					pMacro->SetPictureParameter(COMPRESSION_7,pMacro->GetResolution()); 
					break;
				case COMPRESSION_3: 
					pMacro->SetPictureParameter(COMPRESSION_12,pMacro->GetResolution()); 
					break;
				case COMPRESSION_4: 
					pMacro->SetPictureParameter(COMPRESSION_17,pMacro->GetResolution()); 
					break;
				case COMPRESSION_5: 
					pMacro->SetPictureParameter(COMPRESSION_22,pMacro->GetResolution()); 
					break;
				default:
					pMacro->SetPictureParameter(COMPRESSION_12,pMacro->GetResolution());
			}
		

			TRACE(_T("loading macro %s\n"),sEntry);
			if (pMacro->GetType()!=PMT_GUARD_TOUR)
			{
				CWordArray aIDs;
				WORD w,wID;
				CString sSub;
				w = 1;
				wID = SECID_NO_SUBID;
				BOOL bLoop = TRUE;
				while (bLoop)
				{
					sSub.Format(_T("\\L%u="),w++);
					wID = (WORD)wkp.GetNrFromString(sEntry, sSub, SECID_NO_SUBID);
					if (wID!=SECID_NO_SUBID)
					{
						TRACE(_T("adding id %04hx\n"),wID);
						aIDs.Add(wID);
					}
					else
					{
						break;
					}
				}
				pMacro->SetIDs(aIDs);
			}

			if (pMacro->GetType()==PMT_BACKUP)
			{
				CString sDestination = wkp.GetStringFromString(sEntry, INI_DRIVE, _T(""));
				BOOL bDeleteAtSuccess = wkp.GetNrFromString(sEntry, INI_DELETE_AT_SUCC, FALSE);
				CSecID archiveID = (DWORD)wkp.GetHexFromString(sEntry, INI_B_ARCHIVE_ID, SECID_NO_ID);
				DWORD dwBackupTimeSpan = (DWORD)wkp.GetNrFromString(sEntry, INI_BACKUP_TIMESPAN, BU_TS_ALL);
				pMacro->SetBackup(archiveID,sDestination,bDeleteAtSuccess, dwBackupTimeSpan);
			}
			else if (pMacro->GetType()==PMT_ACTUAL_IMAGE)
			{
				// TODO
				// Resolution 
				// Compression
			}
			else if (   (pMacro->GetType()==PMT_IMAGE_CALL)
				     || (pMacro->GetType()==PMT_CHECK_CALL)
				     || (pMacro->GetType()==PMT_GUARD_TOUR)
					 )
			{
				// Permission
				DWORD dwPermission = (DWORD)CWK_Profile::GetHexFromString(sEntry, _T("\\PM"), SECID_NO_ID);
				const CPermission *pPermission = permissions.GetPermission(CSecID(dwPermission));
				if (pPermission==NULL) 
				{
					WK_TRACE_ERROR(_T("failed to get AlarmPermission for 0x%x\n"),dwPermission);
				}
				pMacro->SetPermission(pPermission);
				BOOL bSetTime = CWK_Profile::GetNrFromString(sEntry,_T("\\ST"),FALSE);
				pMacro->SetSetTime(bSetTime);
				pMacro->SetKeepAlive(TRUE);
			}

			if (pMacro->GetType()==PMT_GUARD_TOUR)
			{
				BOOL bFullScreen = wkp.GetNrFromString(sEntry,_T("\\FS"),FALSE);
				BOOL bSave = wkp.GetNrFromString(sEntry,_T("\\SAVE"),FALSE);
				DWORD dwSwitch = wkp.GetNrFromString(sEntry,_T("\\SWITCH"),1);
				CString sControl = wkp.GetStringFromString(sEntry,_T("\\CONTROL"),_T(""));
				DWORD dwTry = wkp.GetNrFromString(sEntry,_T("\\TRY"),1);
				BOOL bExclusive = wkp.GetNrFromString(sEntry,_T("\\EXCLUSIVE"),TRUE);
				BOOL bDisconnect = wkp.GetNrFromString(sEntry,_T("\\DISCONNECT"),TRUE);


				pMacro->SetKeepAlive(0==sControl.CompareNoCase(_T("EDGE")));
				
				CWordArray aIDs;
				CStringArray sCameras;
				WORD w,wID;
				CString sSub1,sSub2,sOne;
				w = 1;
				wID = SECID_NO_SUBID;
				BOOL bLoop = TRUE;
				while (bLoop)
				{
					sSub1.Format(_T("\\L%u="),w);
					sSub2.Format(_T("\\C%u="),w);
					w++;
					wID = (WORD)wkp.GetNrFromString(sEntry, sSub1, SECID_NO_SUBID);
					sOne = wkp.GetStringFromString(sEntry,sSub2,_T(""));
					TRACE(_T("HostID=%d Cameras=%s\n"),wID,sOne);
					if (   wID!=SECID_NO_SUBID
						&& !sOne.IsEmpty())
					{
						aIDs.Add(wID);
						sCameras.Add(sOne);
					}
					else
					{
						break;
					}
				}

				pMacro->SetGuardTourValues(dwSwitch,dwTry,bFullScreen,bExclusive,bSave,bDisconnect,aIDs,sCameras);
			}
			if (pMacro->GetType() == PMT_VIDEO_OUT)
			{
				int iSubtype = wkp.GetNrFromString(sEntry,_T("\\Subtype"),0);
				int iVideoPort = wkp.GetNrFromString(sEntry,_T("\\VOUT"),0);
				WORD wGroupID = (WORD)wkp.GetHexFromString(sEntry,_T("\\GROUP"),0);
				DWORD dwDwell = wkp.GetNrFromString(sEntry,_T("\\SWITCH"),0);
				CString sCameras = wkp.GetStringFromString(sEntry,_T("\\Cam"),_T(""));
				pMacro->SetVideoOutValues(iVideoPort,iSubtype,dwDwell,wGroupID,sCameras);
				if (   dwDwell>0
					&& pMacro->GetIDs().GetSize()>1)
				{
					pMacro->SetOverallTime(CPM_TIME_INFINITY);
				}
			}
			if (pMacro->GetType() == PMT_UPLOAD)
			{
				CString sServerName,sLogin,sPassword,sFilename;
				sServerName = wkp.GetStringFromString(sEntry,_T("\\SERVER"), _T(""));
				sLogin = wkp.GetStringFromString(sEntry,_T("\\LOGIN"), _T(""));
				sPassword = wkp.GetStringFromString(sEntry,_T("\\PWD"), _T(""));
				sFilename = wkp.GetStringFromString(sEntry,_T("\\FILE"), _T(""));
				wkp.Decode(sPassword);
				pMacro->SetUploadValues(sServerName,sLogin,sPassword,sFilename);
			}

			CString sDump = pMacro->CreateDumpString();

			if (pMacro->IsValidMacro()) 
			{
				Add(pMacro);
				WK_TRACE(_T("MACRO: %s\n"),(LPCTSTR)sDump);
			} 
			else 
			{
				WK_TRACE(_T("Invalid macro (NOT USED): %s\n"),(LPCTSTR)sDump);
				WK_DELETE(pMacro);
			}
		} 
		else 
		{
			WK_TRACE_ERROR(_T("Empty process macro at position %d\n"),i);
		}
	}	// end of loop i<=iMaxProz
}
///////////////////////////////////////////////////////////////////
CProcessMacros::CProcessMacros()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
///////////////////////////////////////////////////////////////////
void CProcessMacros::AddDefaultMacros()
{
	CString s;
	
	// Add standard Relay macros
	// EDGE
	s.LoadString(IDS_EDGE);
	CProcessMacro *pMacro = new CProcessMacro(PMT_RELAY,s,CSecID(SECID_PROCESS_RELAY_EDGE_CONTROL));
	if (pMacro->IsValidMacro()) 
	{
		Add(pMacro);
		WK_TRACE(_T("MACRO: %s\n"),pMacro->GetName());
	} 
	else 
	{
		WK_TRACE(_T("Invalid macro (NOT USED): %s"),(LPCTSTR)pMacro->GetName());
		WK_DELETE(pMacro);
	}
	// STATE
	s.LoadString(IDS_LEVEL);
	pMacro = new CProcessMacro(PMT_RELAY,s,CSecID(SECID_PROCESS_RELAY_STATE_CONTROL));
	if (pMacro->IsValidMacro()) 
	{
		Add(pMacro);
		WK_TRACE(_T("MACRO: %s\n"),(LPCTSTR)pMacro->GetName());
	} 
	else 
	{
		WK_TRACE(_T("Invalid macro (NOT USED): %s"),(LPCTSTR)pMacro->GetName());
		WK_DELETE(pMacro);
	}

	// add default audio macro
	s.LoadString(IDS_AUDIO);
	pMacro = new CProcessMacro(PMT_AUDIO,s,CSecID(SECID_PROCESS_AUDIO));
	pMacro->SetOverallTime(0L);
	pMacro->SetIsIntervall(TRUE);

	if (pMacro->IsValidMacro()) 
	{
		Add(pMacro);
		WK_TRACE(_T("MACRO: %s\n"),(LPCTSTR)pMacro->GetName());
	} 
	else 
	{
		WK_TRACE(_T("Invalid macro (NOT USED): %s"),(LPCTSTR)pMacro->GetName());
		WK_DELETE(pMacro);
	}

	s.LoadString(IDS_PTZ_PRESET);
	pMacro = new CProcessMacro(PMT_PTZ_PRESET,s,CSecID(SECID_PROCESS_PRESET));
	if (pMacro->IsValidMacro()) 
	{
		Add(pMacro);
		WK_TRACE(_T("MACRO: %s\n"),(LPCTSTR)pMacro->GetName());
	} 
	else 
	{
		WK_TRACE(_T("Invalid macro (NOT USED): %s"),(LPCTSTR)pMacro->GetName());
		WK_DELETE(pMacro);
	}
	
	s.LoadString(IDS_ALARM_LIST);
	pMacro = new CProcessMacro(PMT_ALARM_LIST,s,CSecID(SECID_PROCESS_ALARM_LIST));
	if (pMacro->IsValidMacro()) 
	{
		Add(pMacro);
		WK_TRACE(_T("MACRO: %s\n"),(LPCTSTR)pMacro->GetName());
	} 
	else 
	{
		WK_TRACE(_T("Invalid macro (NOT USED): %s"),(LPCTSTR)pMacro->GetName());
		WK_DELETE(pMacro);
	}


}
