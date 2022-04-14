/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputIdipClient.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCInputIdipClient.cpp $
// CHECKIN:		$Date: 15.08.06 14:47 $
// VERSION:		$Revision: 47 $
// LAST CHANGE:	$Modtime: 15.08.06 14:46 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "IdipClient.h"
#include "Mainfrm.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "ViewAlarmList.h"
#include "Server.h"
#include "CIPCInputIdipClient.h"
#include "CIPCInputIdipClientCommData.h"
#include "WndLive.h"
#include "WndHTML.h"

#define DEFAULT_PROTOCOLDIR	_T("c:\\protocol")

IMPLEMENT_DYNAMIC(CIPCInputIdipClient, CObject)

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIdipClient::CIPCInputIdipClient(LPCTSTR szShmName, CServer* pServer, CSecID id, WORD wCodePage)
	: CIPCInputClient(szShmName, FALSE)
{
	m_pServer = pServer;
	m_ID = id;
	m_wCodePage = wCodePage;
	m_bGotInfo = FALSE;
	m_bAlarmOffSpanFeature = FALSE;
	m_idCommPort = SECID_NO_ID;
	m_dwTime = GetTickCount();
//	m_AlarmIDs.Add(SECID_NO_ID); // we do not get the first id from dial-up, dummy entry
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIdipClient::~CIPCInputIdipClient()
{
	m_AlarmOffSpans.SafeDeleteAll();
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::Lock()
{
	m_csInputs.Lock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::Unlock()
{
	m_csInputs.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnReadChannelFound()
{
	m_dwTime = GetTickCount();
	// RKE: CIPCInputIdipClient ist kein Master
	DoRequestConnection();	// muß das aufgerufen werden ?
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmConnection()
{
	if (m_wCodePage != CODEPAGE_UNICODE)			// request only, if unicode is not used
	{
		WORD wCodePage = m_wCodePage;
		m_wCodePage = CODEPAGE_UNICODE;				// request is made in unicode
		DoRequestGetValue(CSecID(), CRF_CODEPAGE);	// Do you speak Unicode?
		m_wCodePage = wCodePage;
	}
	m_dwTime = GetTickCount();
	BOOL bIsMiniB3       = m_pServer->GetKind().CompareNoCase(_T("Mini B3")) == 0;
	BOOL bIsTeleObserver = m_pServer->GetKind().CompareNoCase(_T("TeleObserver")) == 0;
	if (   !m_pServer->IsLocal() 
		&& !bIsMiniB3
		&& !bIsTeleObserver) 
	{
		DoRequestGetValue(SECID_NO_ID,CSD_IS_DV,0);
	}

	if (   !m_pServer->IsDTS()
		&& !bIsMiniB3
		&& !bIsTeleObserver) 
	{
		DoRequestGetValue(SECID_NO_ID,CSD_ALARM_SPAN_OFF_FEATURE,0);
	}

	if (!m_pServer->IsLocal())
	{
		UpdateServerTimeOffset();
	}
	if (m_pServer->GetVersion().IsEmpty())
	{
		if (m_pServer->IsDTS())
		{
			DoRequestGetFile(RFU_STRING_INTERFACE,_T("GetFileVersion c:\\dv\\dvprocess.exe"));
		}
		else
		{
			DoRequestGetFile(RFU_STRING_INTERFACE,_T("GetFileVersion c:\\security\\sec3.exe"));
		}
	}
	DoRequestInfoInputs(SECID_NO_GROUPID);	// all groups
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::UpdateServerTimeOffset()
{
	DoRequestGetFile(RFU_STRING_INTERFACE, SI_GETCURRENTTIME _T(" nop"));
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIdipClient::Run(DWORD dwTimeOut)
{
	if (!m_bGotInfo)
	{
		DWORD curTime = GetTickCount();
		if (GetTimeSpanSigned(m_dwTime, curTime) > 15000)
		{
			// time out post app to stop connection
			WK_TRACE_ERROR(_T("timeout input %s\n"),m_pServer->GetName());
			m_dwTime = curTime;
			AfxGetMainWnd()->PostMessage(ID_DISCONNECT_LOW+m_pServer->GetID());
		}
	}
	return CIPC::Run(dwTimeOut);
}
CString Flags2String(WORD wFlags)
{
	CString sFlags;
	if (wFlags & IREC_STATE_OKAY)
	{
		sFlags += _T("|IREC_STATE_OKAY");
	}
	if (wFlags & IREC_STATE_ENABLED)
	{
		sFlags += _T("|IREC_STATE_ENABLED");
	}
	if (wFlags & IREC_STATE_ALARM)
	{
		sFlags += _T("|IREC_STATE_ALARM");
	}
	if (wFlags & IREC_STATE_EDGE)
	{
		sFlags += _T("|IREC_STATE_EDGE");
	}
	if (wFlags & IREC_STATE_NEEDS_CONFIRMATION)
	{
		sFlags += _T("|IREC_STATE_NEEDS_CONFIRMATION");
	}
	if (wFlags & IREC_IS_COMM_PORT)
	{
		sFlags += _T("|IREC_IS_COMM_PORT");
	}
	if (wFlags & IREC_STATE_TEMP_DEACTIVATE)
	{
		sFlags += _T("|IREC_STATE_TEMP_DEACTIVATE");
	}
	return sFlags;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	BOOL bGotInfo = m_bGotInfo;
	theApp.EndResetting();

	m_csInputs.Lock();
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,numRecords,records);

// TODO GF
// Gross rumposaunen, bevor die Info komplett übernommen wurde?
// Da unten werden Member-Variablen gesetzt...
// wenn auch dies ab jetzt nicht mehr so relevant ist...
	m_bGotInfo = TRUE;

	// browse InputRecords for CommPort Input
	int i,c;

	for (i=0;i<numRecords;i++)
	{
		CString sFlags = Flags2String(records[i].GetFlags());
		TRACE(_T("%d, %08lx, %s, %04hx,%s\n"),i,records[i].GetID().GetID(),(LPCTSTR)records[i].GetName(),records[i].GetFlags(),(LPCTSTR)sFlags);
	}
	for (i=0;i<numRecords;i++)
	{
		if (records[i].IsCommPort())
		{
			m_idCommPort = records[i].GetID();
			break;
		}
	}
// ML 30.9.99 Begin Insertation
	// Den Status aller Melder an GEMOS senden
	if (m_pServer->IsControlConnected())
	{
		c = GetNumberOfInputs();
		for (i=0;i<c && m_pServer->IsControlConnected();i++)
		{
			const CIPCInputRecord& rec = GetInputRecordFromIndex(i);
			if (   rec.GetIsEnabled()
				&& m_pServer->IsControlConnected())
			{
				m_pServer->GetControl()->ConfirmAlarm(rec.GetID(), Enabled);

				if (rec.GetIsActive())
				{
					m_pServer->GetControl()->ConfirmAlarm(rec.GetID(), On);
				}
				else
				{
					m_pServer->GetControl()->ConfirmAlarm(rec.GetID(), Off);
				}
			}		
			else
			{
				m_pServer->GetControl()->ConfirmAlarm(rec.GetID(), Disabled);
			}
		}
	}
	m_csInputs.Unlock();
// ML 30.9.99 End Insertation

	if (!bGotInfo)
	{
		CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
		if (pOV)
		{
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_INPUT_CONNECTED, m_pServer->GetID()));
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmAlarmState(WORD wGroupID, DWORD inputMask)
{
	TRACE(_T("CIPCInputIdipClient::OnConfirmAlarmState(%d, %08x)\n"), wGroupID, inputMask);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnAddRecord(const CIPCInputRecord & record)
{
	if (m_bGotInfo)
	{
		CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
		if (pOV)
		{
			pOV->PostMessage(WM_USER, MAKELONG(WPARAM_ACTUALIZE, m_pServer->GetID()), record.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnUpdateRecord(const CIPCInputRecord & record)
{
	CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
	if (pOV)
	{
		WORD wParam = record.GetIsEnabled() ? WPARAM_ACTUALIZE : WPARAM_DELETE;
		pOV->PostMessage(WM_USER, MAKELONG(wParam,m_pServer->GetID()), record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnDeleteRecord(const CIPCInputRecord & record)
{
	CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
	if (pOV)
	{
		pOV->PostMessage(WM_USER, MAKELONG(WPARAM_DELETE,m_pServer->GetID()), record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnIndicateCommData(CSecID id,const CIPCExtraMemory &data,DWORD dwBlockNr)
{
	if (!m_pServer->IsLocal())
	{
		CIdipClientDoc* pDoc = m_pServer->GetDocument();
		if (pDoc && pDoc->IsCommDataConnected())
		{
			CIPCInputIdipClientCommData* pInput;
			pInput = pDoc->GetCIPCInputIdipClientCommData();
			if (pInput && (pInput->m_idCommPort!=SECID_NO_ID))
			{
				pInput->DoWriteCommData(pInput->m_idCommPort,data,dwBlockNr);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnRequestDisconnect()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_DISCONNECT_LOW+m_pServer->GetID());
//	DelayedDelete();
//	CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
//	if (pOV)
//	{
//		pOV->PostMessage(WM_USER, MAKELONG(WPARAM_INPUT_DISCONNECTED,m_pServer->GetID()));
//	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmGetValue(	CSecID id, // might be used as group ID only
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData)
{
	CViewIdipClient*pVIC = theApp.GetMainFrame()->GetViewIdipClient();
	if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
	{
		CameraControlType cct;
		DWORD dwID = 0;
		if (1 == _stscanf(sKey,CSD_CAM_CONTROL_TYPE_FMT,&dwID))
		{
			cct = NameToCameraControlType(sValue);
			pVIC->LockSmallWindows(_T(__FUNCTION__));
			CWndLive* pDW = pVIC->GetWndLive(m_pServer->GetID(),dwID);
			if (pDW)
			{
				pDW->PostMessage(WM_USER,WPARAM_CCT,(LPARAM)cct);
			}
			pVIC->UnlockSmallWindows();
		}
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
	{
		DWORD dwID = 0;
		DWORD dwF  = 0;
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKT_FMT,&dwID))
		{
			pVIC->LockSmallWindows(_T(__FUNCTION__));
			CWndLive* pDW = pVIC->GetWndLive(m_pServer->GetID(), dwID);
			_stscanf(sValue,_T("%08x"),&dwF);
			if (dwF & PTZF_EXTENDED)
			{
				CString sKey;
				sKey.Format(CSD_CAM_CONTROL_FKTEX_FMT, dwID);
				DoRequestGetValue(GetCommID(), sKey);
			}
			if (pDW)
			{
				pDW->PostMessage(WM_USER,WPARAM_CCF,dwF);
			}
			pVIC->UnlockSmallWindows();
		}
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKTEX_FMT,&dwID))
		{
			pVIC->LockSmallWindows(_T(__FUNCTION__));
			CWndLive* pDW = pVIC->GetWndLive(m_pServer->GetID(),dwID);
			_stscanf(sValue,_T("%08x"), &dwF);
			if (pDW)
			{
				pDW->PostMessage(WM_USER,WPARAM_CCF_EX,dwF);
			}
			pVIC->UnlockSmallWindows();
		}
	}
	else if (0==_tcsncmp(sKey, _T("PTZF_"), 5))
	{
		CString sID = sKey.Mid(sKey.GetLength()-10);
		DWORD dwID;
		if (1 == _stscanf(sID, _T("(%08x)"), &dwID))
		{
			CWndLive* pDW = pVIC->GetWndLive(m_pServer->GetID(),dwID);
			if (pDW)
			{
				pDW->PostMessage(WM_USER, WPARAM_CCFKT_NAME, (LPARAM)_tcsdup(sKey+sValue));
			}
		}
	}
	else if (sKey == CSD_ALARM_SPAN_OFF_FEATURE)
	{
		m_bAlarmOffSpanFeature = (sValue == CSD_ON);
		TRACE(_T("OnConfirmGetValue Input HasAlarmSpanOff %i\n"), m_bAlarmOffSpanFeature);
	}
	else if (sKey == CSD_ALARM_SPAN_OFF_STATUS)
	{
		CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
		if (pOV)
		{
			DWORD wParam = WPARAM_ALARM_OFF_SPAN_OFF;
			if (sValue == CSD_ON)
			{
				wParam = WPARAM_ALARM_OFF_SPAN_ON;
			}
			TRACE(_T("Value %s %08lx\n"),sValue,id.GetID());
			pOV->PostMessage(WM_USER, MAKELONG(wParam,m_pServer->GetID()), id.GetID());
		}
	}
	else if (sKey == CSD_IS_DV)
	{
		m_pServer->SetDTS(sValue == CSD_ON);
		TRACE(_T("OnConfirmGetValue Input IsDTS %i\n"), m_pServer->IsDTS());
	}
	else if (0==sKey.CompareNoCase(_T("GetFileVersion")))
	{
		if (!sValue.IsEmpty())
		{
			WK_TRACE(_T("SERVER VERSION is <%s>\n"),sValue);
			m_pServer->SetVersion(sValue);
		}
	}
	else if (sKey == CSD_RESET && sValue == CSD_ON)
	{
		theApp.SetResetting();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::DoRequestWWWDir()
{
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	CString sParameter,sHost,sCommand,sDir,sSearch;
	CString sRoot = CNotificationMessage::GetWWWRoot();
	sHost.Format(_T("%08lx"),m_pServer->GetHostID().GetID());
	sDir = sRoot + _T('\\') + sHost;
	WK_CreateDirectory(sDir);
	sSearch  = sDir + _T("\\*.*");

	hFound = FindFirstFile((LPCTSTR)sSearch, &found);
	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// still no recursion over dirs
			}
			else
			{
				CTime t(found.ftLastWriteTime);
				CString s;
				s.Format(_T("%s(%08lx)"),found.cFileName,(DWORD)t.GetTime());
				sParameter +=s;
			}
		}
	}
	FindClose(hFound);
	sCommand = SI_GETWWWDIRECTORY _T(" ");
	if (!sParameter.IsEmpty())
	{
		sCommand += sParameter;
	}
	else
	{
		sCommand += _T("nop");
	}
	// get the whole directory
	DoRequestGetFile(RFU_STRING_INTERFACE,sCommand);
//	WK_TRACE(_T("DoRequestGetFile %s\n"),sCommand);

	if (theApp.m_bShowCamereaMapAtConnection)
	{	// get extra map everytime
		CString sCameraMap;
		sCameraMap.Format(_T("%s\\%08lx\\camera.htm"),CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);
		DoRequestGetFile(RFU_FULLPATH,sCameraMap);
//		WK_TRACE(_T("DoRequestGetFile %s\n"),sCameraMap);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmGetFile(	int iDestination,
										const CString &sFileName,
										const void *pData,
										DWORD dwDataLen, DWORD dwCodePage)
{
	WK_TRACE(_T("OnConfirmGetFile %d, %s\n"),iDestination,sFileName);
	if (iDestination == CFU_WWW_FILE)
	{
		CString sTime,sName,sTemp,sFile,sDir,sRemoteDir,sRoot;
		int p;

		// creating cache directory for host
		sRoot = CNotificationMessage::GetWWWRoot();
		sDir.Format(_T("%s\\%08lx"),sRoot,m_pServer->GetHostID().GetID());
		WK_CreateDirectory(sDir);
		
		sTemp = sFileName;
		p = sTemp.ReverseFind('(');
		if (p!=-1)
		{
			sTime = sTemp.Mid(p);
			sTemp = sTemp.Left(p);
		}

		p = sTemp.ReverseFind('\\');
		if (p!=-1)
		{
			sFile = sTemp.Mid(p+1);

			CString pathremote,pathlocal;
			pathlocal.Format(_T("%s\\%08lx"),sRoot,SECID_LOCAL_HOST);
			pathremote = sTemp.Left(p);
			if (0!=pathremote.CompareNoCase(pathlocal))
			{
				pathremote = pathremote.Mid(pathlocal.GetLength()+1);
				if (!pathremote.IsEmpty())
				{
					sRemoteDir = pathremote+'\\';
					WK_CreateDirectory(sDir+'\\'+pathremote);
				}
			}
		}
		else
		{
			sFile = sTemp;
		}

		p = sFile.Find('.');
		if (p!=-1)
		{
			sName = sFile.Left(p);
		}
		sFile = sDir + '\\' + sRemoteDir + sFile;

		TRACE(_T("receiced file %s\n"),sFile);
		CFile file;
		BOOL bOK = TRUE;

		if (file.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			TRY
			{
				file.Write(pData,dwDataLen);
				file.Close();
			}
			CATCH (CFileException, pE)
			{
				WK_TRACE_ERROR(_T("cannot write www file %s, %s\n"),sFile,GetLastErrorString(pE->m_lOsError));
				bOK = FALSE;
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open www file %s %s\n"),sFile,GetLastErrorString());
			bOK = FALSE;
		}

		if (bOK)
		{
			if (!sTime.IsEmpty())
			{
				CFileStatus cfs;
				DWORD dwT=0;
				if (1 == _stscanf(sTime,_T("(%08lx)"),&dwT))
				{
					CTime t1((time_t)dwT);
					if (CFile::GetStatus(sFile,cfs))
					{
						cfs.m_mtime = CTime((time_t)dwT);
						cfs.m_ctime = CTime((time_t)dwT);
						cfs.m_atime = CTime((time_t)dwT);
						TRY
						{
							CFile::SetStatus(sFile,cfs);
						}
						CATCH (CFileException, pCFE)
						{
							WK_TRACE_ERROR(_T("cannot set file status %s, %s\n"),sFile,
								GetLastErrorString(pCFE->m_lOsError));
						}
						END_CATCH
					}
				}
			}
		}

		if (sName.GetLength()==8)
		{
			DWORD dwID = 0;
			if (1 == _stscanf(sName,_T("%08lx"),&dwID))
			{
				if (m_pServer->GetAlarmID().IsInputID() 
					&& (m_pServer->GetAlarmID()==dwID)
					)
				{
					CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
					if (pOV)
					{
						pOV->PostMessage(WM_USER, MAKELONG(WPARAM_INFORMATION,m_pServer->GetID()), (LPARAM)dwID);
					}
				}
			}
		}

	}
	else if (iDestination == CFU_GET_FILE_VERSION)
	{
		CString sVersion;
		UINT  dwValueLen;
		PBYTE pCopyData;
		void* pValueData;

		/*if(dwDataLen > 100*1024*1024)
		{
			WK_TRACE(_T("#### DateLength: %d\n"), dwDataLen);
		}
		*/
		pCopyData = new BYTE[dwDataLen];
		CopyMemory(pCopyData,pData,dwDataLen);

		struct LANGANDCODEPAGE 
		{
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		UINT cbTranslate = 0;

		// Read the list of languages and code pages.

		VerQueryValue(pCopyData, 
					  _T("\\VarFileInfo\\Translation"),
					  (LPVOID*)&lpTranslate,
					  &cbTranslate);

		for (UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
		{
			CString sLanguage;
			sLanguage.Format(_T("\\StringFileInfo\\%04x%04x\\FileVersion"),
							lpTranslate[i].wLanguage,
							lpTranslate[i].wCodePage);
			
			// Retrieve file description for language and code page _T("i"). 
			if (VerQueryValue(pCopyData, 
							  sLanguage.GetBuffer(0), 
							  &pValueData, 
							  &dwValueLen))
			{
				sVersion = (LPCTSTR)pValueData;
				sLanguage.ReleaseBuffer();
				break;
			}
			else
			{
				sLanguage.ReleaseBuffer();
			}
		}

		if (sVersion.IsEmpty())
		{
			sVersion = ExtractFileVersion((BYTE*)pData,dwDataLen);
		}
		if (!sVersion.IsEmpty())
		{
			WK_TRACE(_T("SERVER VERSION is <%s>\n"),sVersion);
			m_pServer->SetVersion(sVersion);
		}
		WK_DELETE_ARRAY(pCopyData);
	}
	else if (iDestination == CFU_FILE)
	{
		CString sName(sFileName);
		sName.MakeLower();
		
		if (-1!=sName.Find(_T("camera")))
		{
			CView* pOV = theApp.GetMainFrame()->GetViewIdipClient();
			if (pOV)
			{
				pOV->PostMessage(WM_USER, MAKELONG(WPARAM_CAMERA_MAP, m_pServer->GetID()));
			}
		}
	}
	else if (iDestination == CFU_GET_CURRENT_TIME)
	{
		CWK_String sTime;
		sTime.InitFromMemory(GetCodePage() == CODEPAGE_UNICODE, pData, (int)dwDataLen, GetCodePage());
		UINT n, nD=0, nMo=0, nY=0, nH=0, nM=0, nS=0;
		n = _stscanf(sTime, _T("%02d.%02d.%02d %02d:%02d:%02d"), &nD, &nMo, &nY, &nH, &nM, &nS);
		if (n == 6)
		{
			CSystemTime st((WORD)nD, (WORD)nMo, (WORD)(2000+nY), (WORD)nH, (WORD)nM, (WORD)nS);
			CSystemTime stLocal;
			stLocal.GetLocalTime();
			int nOffset = 0;
			if (stLocal < st)
			{
				CSystemTime sSpan = st - stLocal;
				nOffset = sSpan.GetSecond() + sSpan.GetMinute()*60 + sSpan.GetHour()*3600;
				if (sSpan.GetMilliseconds() > 500)
				{
					nOffset++;
				}
			}
			else
			{
				CSystemTime sSpan = stLocal - st;
				nOffset = sSpan.GetSecond() + sSpan.GetMinute()*60 + sSpan.GetHour()*3600;
				if (sSpan.GetMilliseconds() > 500)
				{
					nOffset++;
				}
				nOffset = -nOffset;
			}
			WK_TRACE(_T("Server:%s = %d Seconds Time offset\n"), m_pServer->GetName(), nOffset);
			m_pServer->m_nRemoteTimeOffset = (signed short)nOffset;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmGetSystemTime(const CSystemTime& st)
{
/*
	WK_TRACE(_T("OnConfirmGetSystemTime %s, %s\n"),
		m_pServer->GetName(),st.GetDateTime());
*/
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmSetSystemTime()
{
	CString s;
	s.LoadString(IDS_TIME_SET);
	CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),s);
	theApp.m_ErrorMessages.SafeAddTail(pEM);
	CView* pOV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pOV)
	{
		pOV->PostMessage(WM_USER, WPARAM_ERROR_MESSAGE);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage)
{
	if (error==CIPC_ERROR_UNABLE_TO_CONNECT)
	{
	}
	else if (error == INP_REQUEST_GET_FILE)
	{
	}
	else if (error == CIPC_ERROR_GET_FILE)
	{
	}
	else
	{
		if (!sErrorMessage.IsEmpty())
		{
			if (-1==sErrorMessage.Find(_T("GetWWW")))
			{
				WK_TRACE_ERROR(_T("%s\n"),sErrorMessage);
				CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),sErrorMessage);
				theApp.m_ErrorMessages.SafeAddTail(pEM);
				CView* pOV = theApp.GetMainFrame()->GetViewIdipClient();
				if (pOV)
				{
					pOV->PostMessage(WM_USER, WPARAM_ERROR_MESSAGE);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//ML 29.9.99 Begin Insertation{
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnIndicateAlarmState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  LPCTSTR sInfoString
									  )
{		
	if (m_pServer->IsControlConnected())
	{
		for (int nI = 0; nI < 32; nI++)
		{
			if (TSTBIT(changeMask, (WORD)nI))
			{
				CSecID alarmID(wGroupID, (WORD)nI);
				
				if (TSTBIT(inputMask, nI))
				{
					TRACE(_T("Alarm On Input %hx,%d\n"), wGroupID,nI);
					if (m_pServer->IsControlConnected())
					{
						m_pServer->GetControl()->ConfirmAlarm(alarmID.GetID(), On);
					}
				}
				else
				{
					TRACE(_T("Alarm Off Input %d\n"), nI);
					if (m_pServer->IsControlConnected())
					{
						m_pServer->GetControl()->ConfirmAlarm(alarmID.GetID(), Off);
					}
				}
			}
		}
	}
	
	CIPCInputClient::OnIndicateAlarmState(wGroupID, inputMask, changeMask, sInfoString);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnIndicateAlarmNr(CSecID id,
									   BOOL bAlarm,
									   DWORD dwData1,
									   DWORD dwData2)
{
	// call base class for non MD alarms with
	// more than 32 alarm inputs

	CIPCInputClient::OnIndicateAlarmNr(id,bAlarm,dwData1,dwData2);
	// TODO show correct MD coordinates
	CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pVV)
	{
		pVV->LockSmallWindows(_T(__FUNCTION__));
		CWndLive* pDW = pVV->GetWndLive(m_pServer->GetID(), id);
		if (pDW && pDW->GetType() != WST_MPEG4)
		{
			pDW->SetMDValues(LOWORD(dwData1),HIWORD(dwData1));
		}
		pVV->UnlockSmallWindows();
/*
		TRACE(_T("OnIndicateAlarmNr %08lx, %d, (%d,%d)\n"),
				id.GetID(),bAlarm,LOWORD(dwData1),HIWORD(dwData1));
*/

		// Check for DTS alarms to reset 1plus mode if necessary
		if (m_pServer->IsDTS())
		{
			// but only for extern alarms
			DWORD dwExternAlarm = 0x00000480;
			if ((id.GetID() & dwExternAlarm) != 0)
			{
				if (bAlarm)
				{
/*						if (m_AlarmIDs.Has(id) == FALSE)
					{
						m_AlarmIDs.Add(id.GetID());
					}
*/					}
				else
				{
/*						if (m_AlarmIDs.Remove(id))
					{
					}
					else
					{	// assume it was the unknown first id from dial-up
						m_AlarmIDs.Remove(SECID_NO_ID)
					}
					if (m_AlarmIDs.GetSize() == 0)
*/						{
						// if view mode is 1plus and 1plus-at-alarm is set
						// switch back to square mode
						if (   theApp.m_b1PlusAlarm
							&& pVV->IsView1plus())
						{
							pVV->SendMessage(WM_COMMAND,ID_VIEW_3_3);
						}
					}
/*						else
					{
						if (pVV->IsView1plus())
						{
							// if current 1plus is cleared, switch to other alarm cam
						}
					}
*/					}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
WORD CIPCInputIdipClient::GetNrFromAlarmID(CSecID alarmID)
{
	m_csInputs.Lock();
	WORD wAlarmCnt = 0;
	WORD wReturn = (WORD)-1;

	for (int i=0;i<GetNumberOfInputs();i++)
	{
		const CIPCInputRecord& rec = GetInputRecordFromIndex(i);

		if (rec.GetID() == alarmID)
		{
			wReturn = wAlarmCnt; // Liefere Nummer der Kamera
			break;
		}
		wAlarmCnt++;
	}
	m_csInputs.Unlock();
	return wReturn;
}

//ML 29.9.99 End Insertation{

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmGetAlarmOffSpans(CSecID id, int iNumRecords,
												const CSystemTimeSpan data[]) /*RM*/
{
	m_AlarmOffSpans.FromArray(iNumRecords,data);

	CViewAlarmList* pOV = theApp.GetMainFrame()->GetViewAlarmList();
	if (pOV)
	{
		pOV->PostMessage(WM_USER, MAKELONG(WPARAM_ALARM_OFF_SPAN, m_pServer->GetID()), id.GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::RequestSetAlarmOffSpans(CSecID idInput,
											  const CSystemTimeSpans& stAlarmOffSpans)
{
	DoRequestSetAlarmOffSpans(idInput, stAlarmOffSpans);
	WriteAlarmOffSpanProtocol(idInput);
}
//////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::WriteAlarmOffSpanProtocol(CSecID idInput)
{
	CWK_Profile prof;
	CString sDir = prof.GetString(SECTION_LOG, LOG_PROTOCOLPATH, DEFAULT_PROTOCOLDIR);
	WK_CreateDirectory(sDir);

	CString sInputName = GetInputNameFromID(idInput);

	CSystemTime st;
	st.GetLocalTime();
	CString sTime = st.GetDBDate() + st.GetDBTime();

	CString sHost = _T("UNKNOWN");
	if (m_pServer)
	{
		sHost = m_pServer->GetName();
	}

	CString sFile;
	sFile.Format(_T("%s\\%s %s %s.htm"), sDir, sHost, sInputName, sTime);
	
	CString sTitle,s;
	s.LoadString(IDS_ALARM_OFF);
	sTitle.LoadString(IDS_PROTOCOL);
	sTitle += _T(": ");
	sTitle += s;
	sTitle += _T(" ");
	sTitle += sInputName;
	sTitle += _T(" ");
	sTitle += st.GetDateTime();

	CString sUser = _T("UNKNOWN");
	if (theApp.m_pUser)
	{
		sUser = theApp.m_pUser->GetName();
	}

	CString sText;
	sText.Format(IDS_ALARM_OFF2, sUser, sHost, sInputName);
	sText += _T("\n<ol>\n");

	m_AlarmOffSpans.Lock();
	for (int i=0;i<m_AlarmOffSpans.GetSize();i++)
	{
		s.Format(IDS_ALARM_OFF3,
				 m_AlarmOffSpans.GetAtFast(i)->GetStartTime().GetDateTime(),
				 m_AlarmOffSpans.GetAtFast(i)->GetEndTime().GetDateTime());
		sText += _T("<li><font color=\"#0000FF\">") + s + _T("</font></li>\n");
	}
	m_AlarmOffSpans.Unlock();

	sText += _T("</ol>\n");

	CStdioFileU f;
	f.SetFileMCBS((WORD)CWK_String::GetCodePage());
	CString sMetaCodePage = CWndHTML::GetHTMLCodePageMetaTag() + _T("\n");;

	TRY
	{
		if (f.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			f.WriteString(_T("<html>\n"));
            f.WriteString(_T(" <head>\n"));
            f.WriteString(sMetaCodePage);
			f.WriteString(_T("  <title>\n"));
			f.WriteString(sTitle);
			f.WriteString(_T("  </title>\n"));
            f.WriteString(_T(" </head>\n"));
			f.WriteString(_T(" <body>\n"));
			f.WriteString(_T("  <h3>")+sTitle+_T("</h3>\n"));
			f.WriteString(sText);
			f.WriteString(_T(" </body>\n"));
			f.WriteString(_T("</html>\n"));

			f.Flush();
			f.Close();
		}
	}
	WK_CATCH (_T("Cannot write AlarmOffSpans html protocol"));
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClient::OnConfirmSetAlarmOffSpans(CSecID id) /*RM*/
{
	CString sErrorMessage;
	sErrorMessage.LoadString(IDS_ALARM_SPAN_OFF_CONFIRM);
	CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),sErrorMessage);
	theApp.m_ErrorMessages.SafeAddTail(pEM);
	CView* pOV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pOV)
	{
		pOV->PostMessage(WM_USER, WPARAM_ERROR_MESSAGE);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
CString CIPCInputIdipClient::GetInputNameFromID(CSecID id)
{
	CString sReturn;
	m_csInputs.Lock();
	CIPCInputRecord* pRecord = GetInputRecordPtrFromSecID(id);

	if (pRecord)
	{
		sReturn = pRecord->GetName();
	}

	m_csInputs.Unlock();
	return sReturn;
}
