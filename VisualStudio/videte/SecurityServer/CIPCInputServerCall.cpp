// CIPCInputServerCall.cpp: implementation of the CIPCInputServerCall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"

#include "CIPCInputServerCall.h"

#include "MultipleCallProcess.h"
#include "CIPCOutputServerCall.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCInputServerCall::CIPCInputServerCall(CMultipleCallProcess* pProcess,LPCTSTR shmName)
 : CIPCInput(shmName,FALSE)
{
	m_pProcess = pProcess;
	m_bIsReady = FALSE;
	m_bError = FALSE;
	m_sName = m_pProcess->GetCurrentHost()->GetName();
	m_iNrOfFiles = 0;
	m_bGotActualDir = FALSE;
	m_bGotReferenceDir = FALSE;

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCInputServerCall::~CIPCInputServerCall()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnReadChannelFound()
{
	TRACE("CIPCInputServerCall::OnReadChannelFound %s\n",GetShmName());
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OverrideError()
{
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmConnection()
{
	// may be a trace
	WK_TRACE(_T("input connection with %s\n"),m_sName);
	CIPCInput::OnConfirmConnection();
	DoRequestInfoInputs(SECID_NO_GROUPID);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmInfoInputs(WORD wGroupID, 
												int iNumGroups,
												int iNumRecords, 
												const CIPCInputRecord records[])
{
	WK_TRACE(_T("input info %s\n"),m_sName);
	if (m_pProcess->IsCheckCallProcess())
	{
		m_bIsReady = !m_pProcess->GetMacro().GetSetTime();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnRequestDisconnect()
{
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::RequestImages()
{
	int i,c;
	CString sActualImage,sReferenceImage;
	CString sActualDir,sReferenceDir;

	sActualDir.Format(_T("%s\\%08lx\\ist"),
				CNotificationMessage::GetWWWRoot(),
				SECID_LOCAL_HOST);
	sReferenceDir.Format(_T("%s\\%08lx"),
				CNotificationMessage::GetWWWRoot(),
				SECID_LOCAL_HOST);
	c = m_pProcess->GetCIPCOutputServerCall()->GetNumberOfOutputs();

	for (i=0;i<c;i++)
	{
		const CIPCOutputRecord& rec = m_pProcess->GetCIPCOutputServerCall()->GetOutputRecordFromIndex(i);

		if (   rec.IsCamera()
			&& rec.IsReference())
		{
			m_iNrOfFiles += 2;

			CString s;
			s.LoadString(IDS_GET_IMAGE);
			m_pProcess->WriteProtocol(s+_T(" ")+rec.GetName()+_T("<br>\n"));
			
			sActualImage.Format(_T("%s\\%08lx.jpg"),sActualDir,rec.GetID().GetID());
			sReferenceImage.Format(_T("%s\\%08lx.jpg"),sReferenceDir,rec.GetID().GetID());
			
			DoRequestGetFile(RFU_FULLPATH,sActualImage);
			DoRequestGetFile(RFU_FULLPATH,sReferenceImage);
		}
	}

	TRACE(_T("m_iNrOfFiles = %d\n"),m_iNrOfFiles);

	if (m_iNrOfFiles == 0)
	{
		// TODO protocol no reference cams
		CString s;
		s.LoadString(IDS_NO_REF_CAMS);
	    m_pProcess->WriteProtocol(s+_T("<br>\n"));
		WK_TRACE(_T("no reference cams for %s found\n"),m_sName);
		m_bGotActualDir = TRUE;
		m_bGotReferenceDir = TRUE;
	}
	else
	{
		WK_TRACE(_T("requesting dir %s\n"),sActualDir);
		DoRequestGetFile(RFU_FULLPATH,sActualDir);
		WK_TRACE(_T("requesting dir %s\n"),sReferenceDir);
		DoRequestGetFile(RFU_FULLPATH,sReferenceDir);
	}

	// may be there's no camera so check end to begin
	CheckEnd();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::RequestSetTime()
{
	CSystemTime st;
	st.GetLocalTime();
	WK_TRACE(_T("setting time on %s to %s\n"),
		m_pProcess->GetCurrentHost()->GetName(),
		st.GetDateTime());
	DoRequestSetSystemTime(st);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmGetFile(	int iDestination,
											const CString &sFileName,
											const void *pData,
											DWORD dwDataLen, DWORD dwCodePage)
{
	if (iDestination == CFU_FILE)
	{
		OnConfirmFile(sFileName,pData,dwDataLen);
	}
	else if (iDestination == CFU_DIRECTORY)
	{
		OnConfirmDirectory(sFileName,pData,dwDataLen);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm file %s from %s\n"),sFileName,m_sName);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnIndicateError(DWORD dwCmd, CSecID id, 
										  CIPCError error, int iErrorCode,
										  const CString &sErrorMessage)
{
	WK_TRACE_ERROR(_T("connection with %s , %s, %d , %s \n"),
				   m_sName,NameOfEnum(error),iErrorCode,sErrorMessage);

	if (error == CIPC_ERROR_GET_FILE)
	{
		switch (iErrorCode)
		{
		case 4:
			if (   -1 == sErrorMessage.Find(_T('.'))
				&& -1 != sErrorMessage.Find(_T("ist")))
			{
				m_bGotActualDir = TRUE;
			}
			else
			{
				m_iNrOfFiles--;
				TRACE(_T("m_iNrOfFiles = %d\n"),m_iNrOfFiles);
			}
			CheckEnd();
			break;
		default:
			break;
		}
	}
	else if (error == CIPC_ERROR_INVALID_PERMISSIONS)
	{
		switch (iErrorCode)
		{
		case 1:
			m_bError = TRUE;
			m_bIsReady = TRUE;
			break;
		default:
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::CheckEnd()
{
	if (   (m_iNrOfFiles <= 0)
		&& m_bGotActualDir
		&& m_bGotReferenceDir)
	{
		WK_TRACE(_T("all files confirmed %s\n"),m_sName);
		m_bIsReady = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmFile(const CString &sFileName, const void *pData, DWORD dwDataLen)
{
	WK_TRACE(_T("file confirmed from %s, %s\n"),m_sName,sFileName);

	CString sLocal,sRemote;
	CString sDir;
	CString sNewFileName;

	// calc file names
	sLocal.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sRemote.Format(_T("%08lx"),m_pProcess->GetCurrentHost()->GetID().GetID());
	sNewFileName = sFileName;
	sNewFileName.Replace(sLocal,sRemote);

	// create directory if not done
	sDir.Format(_T("%s\\%s"),CNotificationMessage::GetWWWRoot(),sRemote);
	WK_CreateDirectory(sDir);
	sDir += _T("\\ist");
	WK_CreateDirectory(sDir);

   _TCHAR drive[_MAX_DRIVE];   
   _TCHAR dir[_MAX_DIR];
   _TCHAR fname[_MAX_FNAME];   
   _TCHAR ext[_MAX_EXT];
   DWORD dwID=0;
   
   sNewFileName.MakeLower();
   _tsplitpath(sNewFileName,drive,dir,fname,ext);

   if (1 == _stscanf(fname,_T("%08lx"),&dwID))
   {
	   CIPCOutputRecord* pRec = m_pProcess->GetCIPCOutputServerCall()->GetOutputRecordPtrFromSecID(CSecID(dwID));
	   if (pRec && pRec->IsCamera())
	   {
		   CString s;
		   if (-1 == sNewFileName.Find(_T("ist")))
		   {
				s.LoadString(IDS_GOT_ACTUALIMAGE);
		   }
		   else
		   {
				s.LoadString(IDS_GOT_REFERENCEIMAGE2);
		   }
		   m_pProcess->WriteProtocol(s+_T(" ")+pRec->GetName()+_T("<br>\n"));
	   }
   }

	// save file
	CFile file;

	if (file.Open(sNewFileName,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			file.Write(pData,dwDataLen);
			file.Flush();
			file.Close();
			WK_TRACE(_T("image saved under %s\n"),sNewFileName);
		}
		CATCH(CFileException , e)
		{
			WK_TRACE_ERROR(_T("cannot save image, %s\n"),
				GetLastErrorString(e->m_lOsError));
		}
		END_CATCH
	}

	m_iNrOfFiles--;

	CheckEnd();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmDirectory(const CString &sFileName, 
											 const void *pData, 
											 DWORD dwDataLen)
{
	WK_TRACE(_T("dir confirmed from %s, %s\n"),m_sName,sFileName);

	CString sLocal,sRemote;
	CString sDir,sFile;
	CString sNewFileName;

	// calc file names
	sLocal.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sRemote.Format(_T("%08lx"),m_pProcess->GetCurrentHost()->GetID().GetID());
	sDir = sFileName;
	sDir.Replace(sLocal,sRemote);
	sDir.MakeLower();

	WK_TRACE(_T("mapping to dir %s\n"),sDir);

	if (-1!=sDir.Find(_T("ist")))
	{
		m_bGotActualDir = TRUE;
	}
	else
	{
		m_bGotReferenceDir = TRUE;
	}

	if (pData && (dwDataLen>0))
	{
		DWORD dwFD = sizeof(WIN32_FIND_DATA);
		int i;
		int iSize = dwDataLen/dwFD;
		LPWIN32_FIND_DATA lpWFD;
		BYTE* pD = (BYTE*)pData;
		CFileStatus cfs;

		for (i=0;i<iSize;i++)
		{
			lpWFD = (LPWIN32_FIND_DATA)pD;
			// setting file date
			if (lpWFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				sFile = sDir + _T('\\') + lpWFD->cFileName;
				if (CFile::GetStatus(sFile,cfs))
				{
					cfs.m_mtime = CTime(lpWFD->ftLastWriteTime);
					cfs.m_ctime = CTime(lpWFD->ftCreationTime);
					cfs.m_atime = CTime(lpWFD->ftLastAccessTime);
					if (cfs.m_ctime.GetTime() == 0)
						cfs.m_ctime = cfs.m_mtime;
					if (cfs.m_atime.GetTime() == 0)
						cfs.m_atime = cfs.m_mtime;
					TRY
					{
						CFile::SetStatus(sFile,cfs);
					}
					WK_CATCH(_T("cannot set time"));
				}
			}

			pD += dwFD;
		}
	}
	else
	{
		WK_TRACE(_T("empty dir info\n"));
	}
	CheckEnd();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputServerCall::OnConfirmSetSystemTime()
{
	WK_TRACE(_T("set time confirmed by %s\n"),m_pProcess->GetCurrentHost()->GetName());
	m_bIsReady = TRUE;
}
