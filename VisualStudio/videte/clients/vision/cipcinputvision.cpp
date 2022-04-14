/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputVision.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCInputVision.cpp $
// CHECKIN:		$Date: 2.07.04 15:59 $
// VERSION:		$Revision: 94 $
// LAST CHANGE:	$Modtime: 2.07.04 15:58 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "Vision.h"

#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "Server.h"
#include "CIPCInputVision.h"
#include "CIPCInputVisionCommData.h"
#include "DisplayWindow.h"

static TCHAR BASED_CODE szLog[]				= _T("Log");
static TCHAR BASED_CODE szProtocolPath[]	= _T("ProtocolPath");
static TCHAR BASED_CODE szProtocolDir[]		= _T("c:\\protocol");

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputVision::CIPCInputVision(LPCTSTR shmName, CServer* pServer, CSecID id)
	: CIPCInputClient(shmName,FALSE)
{
	m_pServer = pServer;
	m_ID = id;
	m_bGotInfo2 = FALSE;
	m_bAlarmOffSpanFeature = FALSE;
	m_idCommPort = SECID_NO_ID;
	m_dwTime = GetTickCount();
//	m_AlarmIDs.Add(SECID_NO_ID); // we do not get the first id from dial-up, dummy entry
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCInputVision::~CIPCInputVision()
{
	m_AlarmOffSpans.SafeDeleteAll();
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::Lock()
{
	m_csInputs.Lock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::Unlock()
{
	m_csInputs.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnReadChannelFound()
{
	m_dwTime = GetTickCount();
	// RKE: CIPCInputVision ist kein Master
	DoRequestConnection();	// muß das aufgerufen werden ?
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmConnection()
{
	m_dwTime = GetTickCount();
	if (   !m_pServer->IsLocal() 
		&& (0!=m_pServer->GetKind().CompareNoCase(_T("Mini B3")))
		&& (0!=m_pServer->GetKind().CompareNoCase(_T("TeleObserver")))
		) 
	{
		DoRequestGetValue(SECID_NO_ID,CSD_IS_DV,0);
#ifndef _DTS
		if (!m_pServer->IsDTS())
		{
			RequestWWWDir();
		}
#endif
	}

#ifndef _DTS
	if (   (0!=m_pServer->GetKind().CompareNoCase(_T("Mini B3")))
		&& (0!=m_pServer->GetKind().CompareNoCase(_T("TeleObserver")))
		&& (!m_pServer->IsDTS())
		) 
	{
		DoRequestGetValue(SECID_NO_ID,CSD_ALARM_SPAN_OFF_FEATURE,0);
	}
#endif

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
BOOL CIPCInputVision::Run(DWORD dwTimeOut)
{
	if (!m_bGotInfo2)
	{
		DWORD curTime = GetTickCount();
		if ((curTime-m_dwTime)>15000)
		{
			// time out post app to stop connection
			WK_TRACE_ERROR(_T("timeout input %s\n"),m_pServer->GetName());
			m_dwTime = curTime;
			AfxGetMainWnd()->PostMessage(ID_DISCONNECT_LOW+m_pServer->GetID());
		}
	}
	return CIPC::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	m_csInputs.Lock();
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,numRecords,records);

// gf todo
// Gross rumposaunen, bevor die Info komplett übernommen wurde?
// Da unten werden Member-Variablen gesetzt...
// wenn auch dies ab jetzt nicht mehr so relevant ist...
	m_bGotInfo2 = TRUE;

	// browse InputRecords for CommPort Input
	int i,c;

	c = GetNumberOfInputs();
	for (i=0;i<c;i++)
	{
		const CIPCInputRecord& rec = GetInputRecordFromIndex(i);
		if (rec.IsCommPort())
		{
			m_idCommPort = rec.GetID();
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

	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_INPUT_CONNECTED,m_pServer->GetID())
						);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnAddRecord(const CIPCInputRecord & record)
{
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnUpdateRecord(const CIPCInputRecord & record)
{
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_ACTUALIZE,m_pServer->GetID()),
						 record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnDeleteRecord(const CIPCInputRecord & record)
{
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_DELETE,m_pServer->GetID()),
						 record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnIndicateCommData(CSecID id,const CIPCExtraMemory &data,DWORD dwBlockNr)
{
	if (!m_pServer->IsLocal())
	{
		CVisionDoc* pDoc = m_pServer->GetDocument();
		if (pDoc && pDoc->IsCommDataConnected())
		{
			CIPCInputVisionCommData* pInput;
			pInput = pDoc->GetCIPCInputVisionCommData();
			if (pInput && (pInput->m_idCommPort!=SECID_NO_ID))
			{
				pInput->DoWriteCommData(pInput->m_idCommPort,data,dwBlockNr);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnRequestDisconnect()
{
	DelayedDelete();
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_INPUT_DISCONNECTED,m_pServer->GetID())
						);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmGetValue(	CSecID id, // might be used as group ID only
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData)
{
	if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
	{
		CameraControlType cct;
		DWORD dwID = 0;
		if (1 == _stscanf(sKey,CSD_CAM_CONTROL_TYPE_FMT,&dwID))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = m_pServer->GetDocument()->GetVisionView()->GetDisplayWindow(
								 m_pServer->GetID(),dwID);
			cct = NameToCameraControlType(sValue);
			if (WK_IS_WINDOW(pDW))
			{
				pDW->PostMessage(WM_USER,WPARAM_CCT,(LPARAM)cct);
			}
		}
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
	{
		DWORD dwID = 0;
		DWORD dwF  = 0;
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKT_FMT,&dwID))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = m_pServer->GetDocument()->GetVisionView()->GetDisplayWindow(
								 m_pServer->GetID(),dwID);
			_stscanf(sValue,_T("%08x"),&dwF);
			if (WK_IS_WINDOW(pDW))
			{
				pDW->PostMessage(WM_USER,WPARAM_CCF,dwF);
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
		CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{
			DWORD wParam = WPARAM_ALARM_OFF_SPAN_OFF;
			if (sValue == CSD_ON)
			{
				wParam = WPARAM_ALARM_OFF_SPAN_ON;
			}
			TRACE(_T("Value %s %08lx\n"),sValue,id.GetID());
			pOV->PostMessage(WM_USER,
							 MAKELONG(wParam,m_pServer->GetID()),
							 id.GetID());
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
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::RequestWWWDir()
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
	sCommand = _T("GetWWWDirectory ");
	if (!sParameter.IsEmpty())
	{
		sCommand += sParameter;
	}
	else
	{
		sCommand += _T("nop");
	}
	DoRequestGetFile(RFU_STRING_INTERFACE,sCommand);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmGetFile(	int iDestination,
										const CString &sFileName,
										const void *pData,
										DWORD dwDataLen, DWORD dwCodePage)
{
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

		CFile file;

		if (file.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			TRY
			{
				file.Write(pData,dwDataLen);
				file.Close();
			}
			WK_CATCH(_T("cannot write received www file"));
		}
		
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
					CFile::SetStatus(sFile,cfs);
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
					CVisionDoc* pDoc = m_pServer->GetDocument();
					if (pDoc)
					{
						CObjectView* pOV = pDoc->GetObjectView();
						if (WK_IS_WINDOW(pOV))
						{
							pOV->PostMessage(WM_USER,
								MAKELONG(WPARAM_INFORMATION,m_pServer->GetID()),
								(LPARAM)dwID);
						}
					}
				}
			}
		}

		if (0==sName.CompareNoCase(_T("camera")))
		{
			CVisionDoc* pDoc = m_pServer->GetDocument();
			if (pDoc)
			{
				CObjectView* pOV = pDoc->GetObjectView();
				if (WK_IS_WINDOW(pOV))
				{
					pOV->PostMessage(WM_USER,
						MAKELONG(WPARAM_CAMERA_MAP,m_pServer->GetID()));
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
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmGetSystemTime(const CSystemTime& st)
{
/*
	WK_TRACE(_T("OnConfirmGetSystemTime %s, %s\n"),
		m_pServer->GetName(),st.GetDateTime());
*/
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmSetSystemTime()
{
	CString s;
	s.LoadString(IDS_TIME_SET);
	CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),s);
	theApp.m_ErrorMessages.SafeAddTail(pEM);
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,WPARAM_ERROR_MESSAGE);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnIndicateError(DWORD dwCmd, CSecID id, 
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
				CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
				if (WK_IS_WINDOW(pOV))
				{
					pOV->PostMessage(WM_USER,WPARAM_ERROR_MESSAGE);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//ML 29.9.99 Begin Insertation{
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnIndicateAlarmState(WORD wGroupID,
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
void CIPCInputVision::OnIndicateAlarmNr(CSecID id,
									   BOOL bAlarm,
									   DWORD dwData1,
									   DWORD dwData2)
{
	// TODO show correct MD coordinates
	CVisionDoc* pDoc = m_pServer->GetDocument();
	if (pDoc)
	{
		CVisionView* pVV = pDoc->GetVisionView();
		if (WK_IS_WINDOW(pVV))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),
													    id);
			if (pDW && WK_IS_WINDOW(pDW))
			{
				pDW->SetMDValues(LOWORD(dwData1),HIWORD(dwData1));
			}
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
								pVV->PostMessage(WM_COMMAND,ID_VIEW_1_PLUS);
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
}
//////////////////////////////////////////////////////////////////////////////////////
WORD CIPCInputVision::GetNrFromAlarmID(CSecID alarmID)
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
void CIPCInputVision::OnConfirmGetAlarmOffSpans(CSecID id, int iNumRecords,
												const CSystemTimeSpan data[]) /*RM*/
{
	m_AlarmOffSpans.FromArray(iNumRecords,data);

	CVisionDoc* pDoc = m_pServer->GetDocument();
	if (pDoc)
	{
		CObjectView* pOV = pDoc->GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{
			pOV->PostMessage(WM_USER,
							 MAKELONG(WPARAM_ALARM_OFF_SPAN,m_pServer->GetID()),
							 id.GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::RequestSetAlarmOffSpans(CSecID idInput,
											  const CSystemTimeSpans& stAlarmOffSpans)
{
	DoRequestSetAlarmOffSpans(idInput, stAlarmOffSpans);
	WriteAlarmOffSpanProtocol(idInput);
}
//////////////////////////////////////////////////////////////////
void CIPCInputVision::WriteAlarmOffSpanProtocol(CSecID idInput)
{
	CWK_Profile prof;
	CString sDir = prof.GetString(szLog, szProtocolPath, szProtocolDir);
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

	for (int i=0;i<m_AlarmOffSpans.GetSize();i++)
	{
		s.Format(IDS_ALARM_OFF3,
				 m_AlarmOffSpans.GetAt(i)->GetStartTime().GetDateTime(),
				 m_AlarmOffSpans.GetAt(i)->GetEndTime().GetDateTime());
		sText += _T("<li><font color=\"#0000FF\">") + s + _T("</font></li>\n");
	}
	sText += _T("</ol>\n");

	CStdioFile f;

	TRY
	{
		if (f.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			f.WriteString(_T("<html>\n"));
			f.WriteString(_T("<title>\n"));
			f.WriteString(sTitle);
			f.WriteString(_T("</title>\n"));
			f.WriteString(_T("<body>\n"));
			f.WriteString(_T("<h3>")+sTitle+_T("</h3>\n"));
			f.WriteString(sText);
			f.WriteString(_T("</body>\n"));
			f.WriteString(_T("</html>\n"));

			f.Flush();
			f.Close();
		}
	}
	WK_CATCH (_T("Cannot write AlarmOffSpans html protocol"));
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVision::OnConfirmSetAlarmOffSpans(CSecID id) /*RM*/
{
	CString sErrorMessage;
	sErrorMessage.LoadString(IDS_ALARM_SPAN_OFF_CONFIRM);
	CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),sErrorMessage);
	theApp.m_ErrorMessages.SafeAddTail(pEM);
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,WPARAM_ERROR_MESSAGE);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
CString CIPCInputVision::GetInputNameFromID(CSecID id)
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
