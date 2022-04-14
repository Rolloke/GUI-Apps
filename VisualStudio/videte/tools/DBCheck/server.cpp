/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: server.cpp $
// ARCHIVE:		$Archive: /Project/Tools/DBCheck/server.cpp $
// CHECKIN:		$Date: 16.07.02 15:40 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 16.07.02 10:04 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"

#include "DBCheck.h"
#include "Mainfrm.h"
#include "DBCheckDoc.h"
#include "Server.h"
#include "DBCheckView.h"
#include "ConnectingDialog.h"
/*
#include "DataBaseInfoDlg.h"
#include "DisplayWindow.h"
*/
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// for DEMO Version
static char szSerials[5][10] = {"911171851","941191352","941131958","961171356","971111863"};
int CServer::m_iSerialCounter = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServer::CServer(CDBCheckDoc* pDoc, WORD wID)
{
	m_pDoc = pDoc;
	m_wID = wID;
	// m_sName
	m_pDatabase  = NULL;
//	m_pDataBaseInfoDlg = NULL;

	m_bLocal = FALSE;
	m_bDV = FALSE;
	m_iMajorVersion = 0;
	m_iMinorVersion = 0;
	m_iSubVersion = 0;
	m_iBuildNr = 0;

	m_pConnectDialog = NULL;
	m_pConnectionRecord = NULL;
	m_pConnectThread = NULL;
	m_pDatabaseResult = NULL;
	m_bConnectingCancelled = FALSE;
	
	CWK_Profile wkp;
	m_AutoLogout.Load(wkp);
	
	m_dwQueryID = 1;
	m_bSearching = FALSE;
	m_bFirstQueryResult = FALSE;
	m_bStopSearchCopy = FALSE;
   m_idHost.Set((DWORD)0);
}

CServer::~CServer()
{
   Reset();
   WK_DELETE(m_pDatabase);
   WK_DELETE(m_pDatabaseResult);
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetFullName() const
{
	CString r;
	if (IsLocal())
	{
		r = m_sName;
	}
	else
	{
		r = m_sName + " (" + m_sKind +")";
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
void CServer::Lock()
{
	m_csSection.Lock();
}
//////////////////////////////////////////////////////////////////////
void CServer::Unlock()
{
	m_csSection.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnIdle()
{
	if (IsConnected())
	{
		CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();

		// actualize GUI

		if (m_pDatabase->GotInfo())
		{
			m_pDoc->ActualizeDatabase(m_pDatabase);
			m_pDatabase->ClearGotInfo();
		}


      if (m_arNewArchivs.GetSize())
		{
			CIPCArchivRecord *pArchiv;

			m_arNewArchivs.Lock();
			while(m_arNewArchivs.GetSize())
			{
				pArchiv = m_arNewArchivs.GetAt(0);
				m_pDoc->UpdateMyViews(NULL,
					MAKELONG(VDH_ADD_ARCHIV,GetID()),(CObject*)pArchiv);
				m_arNewArchivs.RemoveAt(0);
				WK_DELETE(pArchiv);
			}
			m_arNewArchivs.Unlock();
		}
		if (m_arNewFiles.GetSize())
		{
			CIPCArchivFileRecord *pFile;

			m_arNewFiles.Lock();
			while(m_arNewFiles.GetSize())
			{
				pFile = m_arNewFiles.GetAt(0);
				m_pDoc->UpdateMyViews(NULL,
					MAKELONG(VDH_ADD_FILE,GetID()),(CObject*)pFile);
				m_arNewFiles.RemoveAt(0);
				WK_DELETE(pFile);
			}
			m_arNewFiles.Unlock();
		}

		if (m_srNewSequences.GetSize())
		{
			CIPCSequenceRecord* pSequence;
			m_srNewSequences.Lock();
			while(m_srNewSequences.GetSize())
			{
				pSequence = m_srNewSequences.GetAt(0);
				m_pDoc->UpdateMyViews(NULL,
					MAKELONG(VDH_ADD_SEQUENCE,GetID()),(CObject*)pSequence);
				m_srNewSequences.RemoveAt(0);
				WK_DELETE(pSequence);
			}
			m_srNewSequences.Unlock();
		}

		if (m_arRemoved.GetSize())
		{
			DWORD dw;
			WORD wArchivNr;
			WORD wSequenceNr;

			while(m_arRemoved.GetSize())
			{
				dw = m_arRemoved.GetAt(0);
				wArchivNr = LOWORD(dw);
				wSequenceNr = HIWORD(dw);

				m_pDoc->UpdateMyViews(NULL,
					MAKELONG(VDH_REMOVE_FILE,GetID()),(CObject*)dw);
				m_arRemoved.RemoveAt(0);
				int i,c;
				c = m_arFiles.GetSize();
				for (i=0;i<c;i++)
				{
					CIPCArchivFileRecord *pFile = m_arFiles.GetAt(i);
					if 	( (pFile->GetArchivNr() == wArchivNr)	&&
						  (pFile->GetDirNr() == wSequenceNr) )
					{
						m_arFiles.RemoveAt(i);
						WK_DELETE(pFile);
						break;
					}
				}
				c = m_srSequences.GetSize();
				for (i=0;i<c;i++)
				{
					CIPCSequenceRecord* pSequence = m_srSequences.GetAt(i);
					if 	( (pSequence->GetArchiveNr() == wArchivNr)	&&
						  (pSequence->GetSequenceNr() == wSequenceNr) )
					{
						m_srSequences.RemoveAt(i);
						WK_DELETE(pSequence);
						break;
					}
				}
			}
		}

		if (m_arToOpenFiles.GetSize())
		{
			CIPCArchivFileRecord *pFile;

			m_arToOpenFiles.Lock();
			while(m_arToOpenFiles.GetSize())
			{
				pFile = m_arToOpenFiles.GetAt(0);
				m_pDoc->UpdateMyViews(NULL,
					MAKELONG(VDH_OPEN_FILE,GetID()),(CObject*)pFile);
				m_arToOpenFiles.RemoveAt(0);
				WK_DELETE(pFile);
			}
			m_arToOpenFiles.Unlock();
		}
/*
		if (!IsLocal() && m_AutoLogout.IsConnectionOver())
		{
			if (theApp.IsDemo())
			{
				pMF->PostMessage(WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
			}
			else
			{
				COemGuiApi::AutoDisconnectDialog(pMF,&m_AutoLogout,
					WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
			}
		}

		GetDatabase()->ActualizeArchivInfo();

		if (m_saErrorMessages.GetSize())
		{
			Lock();
			CString s = m_saErrorMessages.GetAt(0);
			m_saErrorMessages.RemoveAt(0);
			Unlock();
			CString sF;
			sF.Format(IDS_ERROR_ON,m_sName,s);
			COemGuiApi::MessageBox(sF,20,MB_OK|MB_ICONSTOP);
		}
*/		
	}
	else
	{
		Lock();
		if (m_pDatabaseResult)
		{
			ConnectDatabase();
		}
		Unlock();

	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnected() const
{
	if (m_pDatabase)
	{
		if (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDisconnected() const
{
	if (   (m_pDatabase && m_pDatabase->GetIsMarkedForDelete())
		|| (m_bConnectingCancelled && (m_pConnectThread == NULL))
		)
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::Connect(const CString& sNameRef,
					const CString& sNumberRef,
					CSecID idHost)
{
	DWORD dwConnectTimeout = 30*1000;
	m_sName = sNameRef;
	m_idHost = idHost;

	// CAVEAT sNameRef and sNumberRef are references on members of CHost in CHostArray!!!
	// theApp.GetHosts() will delete and renew the CHostArray!
	// So the references are killed!!!
	CString sNumber = sNumberRef;
	
    CHostArray& ha = theApp.GetHosts();
	CHost*  pHost = ha.GetHost(m_idHost);
	if (pHost)
	{
		m_sKind = pHost->GetTyp();
	}

	CString sDial;
	if (sNumber==LOCAL_LOOP_BACK)
	{
		m_bLocal = TRUE;
		dwConnectTimeout = 10*1000;
		sDial = sNumber;
	}
	else
	{
		sDial = CHost::GetCleanedUpNumber(sNumber);
	}

	m_pConnectionRecord = new CConnectionRecord(sDial, 
											*theApp.m_pUser, 
											theApp.m_pPermission->GetName(),
											theApp.m_pPermission->GetPassword(),
											dwConnectTimeout);
	if (   theApp.m_pPermission->IsSpecialReceiver()
		&& (sNumber != LOCAL_LOOP_BACK)
		)
	{
		CString sPin = "";
		COemGuiApi::PinDialog(NULL, sPin);
		m_pConnectionRecord->SetFieldValue(CSD_PIN, sPin);
	}
#ifdef _DEBUG
	if (   theApp.m_pPermission->IsSpecialReceiver()
		&& (sNumber == LOCAL_LOOP_BACK)
		)
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,"DVRT\\DV","");
		CString s = wkp.GetString("Process","PIN","");
		if (!s.IsEmpty())
		{
			 CWK_Profile::Decode(s);
		}
		m_pConnectionRecord->SetFieldValue(CSD_PIN, s);
	}
#endif
/*
   if (theApp.IsReceiver())
	{
		CString sSerial = "0";
		if (theApp.IsDemo())
		{
			sSerial = szSerials[m_iSerialCounter];
			m_iSerialCounter++;
			if (m_iSerialCounter>4)
			{
				m_iSerialCounter = 0;
			}
		}
		else
		{
			CWK_Profile wkp;
			// by default a invalid serial!
			sSerial = wkp.GetString("Version","Serial","0");
			if (sSerial.IsEmpty())
			{
				sSerial = "0";
			}
		}
		m_pConnectionRecord->SetFieldValue(CRF_SERIAL,sSerial);
	}
*/

   WK_STAT_LOG("Connect",1,m_sName);
	if (sNumber != LOCAL_LOOP_BACK)
	{
		WK_DESTROY_WINDOW(m_pConnectDialog);
		m_pConnectDialog = new CConnectingDialog(this, m_sName, dwConnectTimeout, AfxGetMainWnd());
	}

	m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this);
	m_pConnectThread->m_bAutoDelete = FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::CancelConnecting()
{
	Lock();
	m_bConnectingCancelled = TRUE;
	m_Fetch.Cancel();
	Unlock();

	WaitForSingleObject(m_pConnectThread->m_hThread, 200);
	WK_DELETE(m_pConnectThread);
}
//////////////////////////////////////////////////////////////////////
UINT CServer::ConnectThreadProc(LPVOID pParam)
{
	WK_TRACE("ConnectThreadProc %08lx\n",GetCurrentThreadId());

	CServer* pServer = (CServer*)pParam;

	pServer->Lock();
	CConnectionRecord c(*pServer->m_pConnectionRecord);
	pServer->Unlock();

	pServer->Lock();
	if (pServer->m_bConnectingCancelled)
	{
		pServer->Unlock();
		return FALSE;
	}
	pServer->Unlock();

	CIPCFetchResult frDatabase = pServer->m_Fetch.FetchDatabase(c);

	pServer->Lock();
	if (pServer->m_bConnectingCancelled)
	{
		pServer->Unlock();
		return FALSE;
	}
	pServer->Unlock();

	pServer->Lock();
	WK_DELETE(pServer->m_pDatabaseResult);
	pServer->m_pDatabaseResult = new CIPCFetchResult(frDatabase);
	if (   frDatabase.IsOkay()
		&& !frDatabase.GetErrorMessage().IsEmpty())
	{
		pServer->SetVersion(frDatabase.GetErrorMessage());
	}
	pServer->Unlock();

	BOOL bResult = frDatabase.IsOkay();
	AfxGetMainWnd()->PostMessage(WM_USER);
	return bResult;
}
//////////////////////////////////////////////////////////////////////
CString	CServer::GetErrorMessage(const CIPCFetchResult& fr)
{
	CString sMessage;
	UINT nID = IDP_NO_CONNECTION;

	sMessage.LoadString(IDP_NO_CONNECTION);
	// error condiontions 
	// CIPC_ERROR_UNABLE_TO_CONNECT with subcodes
	// CIPC_ERROR_INVALID_PERMISSIONS no subcode
	if (fr.GetError()==CIPC_ERROR_UNABLE_TO_CONNECT)
	{
		// errorCode 1:link-modul not found,  2: server not found,
		//			 3: server performs reset 4: max clients reached
		switch (fr.GetErrorCode())
		{
		case 1:
			nID = IDP_NO_CONNECTION_LINK_MODUL;
			break;
		case 2:
			nID = IDP_NO_CONNECTION_SERVER;
			break;
		case 3:
			nID = IDP_NO_CONNECTION_RESET;
			break;
		case 4:
			nID = IDP_NO_CONNECTION_MAX_CLIENT;
			break;
		default:
			nID = IDP_NO_CONNECTION;
			break;
		}

		sMessage.LoadString(nID);

		// NEW HEDU 17.8.97, append Error Message in [ ] 
		// but only, if it's not the NOT_RUNNING error
		if ( fr.GetErrorCode()!=1	// not running
			&& fr.GetErrorMessage().GetLength()) 
		{
			sMessage += "\n[ ";
			sMessage += fr.GetErrorMessage();
			// drop trailing newline
			if (sMessage[sMessage.GetLength()-1]=='\n') {
				sMessage = sMessage.Left(sMessage.GetLength()-1);
			}
			sMessage += "]";
		}
	} 
	else if (fr.GetError()==CIPC_ERROR_INVALID_PERMISSIONS)
	{
		nID = IDP_NO_CONNECTION_INVALID_PERMISSION;
		sMessage.LoadString(nID);
		// there are nor details, so there is nothing to append
	} 
	else if (fr.GetError()==CIPC_ERROR_INVALID_SERIAL)
	{
		nID = IDP_NO_CONNECTION_INVALID_SERIAL;
		sMessage.LoadString(nID);
	}
	else
	{
		sMessage.LoadString(nID);
	}

	return sMessage;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectDatabase()
{
	CString shmDatabaseName;
	shmDatabaseName = m_pDatabaseResult->GetShmName();
	if ( (m_pDatabaseResult->GetError()!=CIPC_ERROR_OKAY) || (shmDatabaseName.GetLength()==0))
	{
		m_sConnectionError = GetErrorMessage(*m_pDatabaseResult);
		AfxGetMainWnd()->PostMessage(WM_USER, 0xffffffff, (LPARAM)GetDocument());
		WK_STAT_LOG("Connect",0,m_sName);
	}
	else
	{
		m_pDatabase = new CIPCDatabaseDBCheck(m_pDatabaseResult->GetShmName(),
											  this,m_pDatabaseResult->GetAssignedID());
		WK_STAT_LOG("Connect",2,m_sName);
	}
	WK_DELETE(m_pDatabaseResult);
	WK_DESTROY_WINDOW(m_pConnectDialog);
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::Disconnect()
{
/*
	if (WK_IS_WINDOW(m_pDataBaseInfoDlg))
	{
		m_pDataBaseInfoDlg->DestroyWindow();
	}
*/
	WK_DELETE(m_pDatabase);
	WK_STAT_LOG("Connect",0,m_sName);
	if (theApp.m_pUser)
	{
		WK_TRACE_USER("%s trennt Verbindung zu %s\n",
			theApp.m_pUser->GetName(),GetName());
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::ResetConnectionAutoLogout()
{
	m_AutoLogout.ResetConnection();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddNewArchiv(const CIPCArchivRecord& data)
{
	CIPCArchivRecord* pArchiv = new CIPCArchivRecord(data);
	m_arNewArchivs.Lock();
	m_arNewArchivs.Add(pArchiv);
	m_arNewArchivs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::UpdateArchiv(const CIPCArchivRecord& data)
{
	CIPCArchivRecord* pArchiv = new CIPCArchivRecord(data);
	m_arUpdatedArchivs.Lock();
	m_arUpdatedArchivs.Add(pArchiv);
	m_arUpdatedArchivs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddArchivFile(const CIPCArchivFileRecord& data)
{
	m_arNewFiles.Lock();
	m_arNewFiles.Add(new CIPCArchivFileRecord(data));
	m_arNewFiles.Unlock();

	m_arFiles.Lock();
	m_arFiles.Add(new CIPCArchivFileRecord(data));
	m_arFiles.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddNewSequence(const CIPCSequenceRecord& seq)
{
	m_srNewSequences.Lock();
	m_srNewSequences.Add(new CIPCSequenceRecord(seq));
	m_srNewSequences.Unlock();

	m_srSequences.Lock();
	m_srSequences.Add(new CIPCSequenceRecord(seq));
	m_srSequences.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddRemoved(WORD wArchiv, WORD wSequence)
{
	m_arRemoved.Add(MAKELONG(wArchiv,wSequence));
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetCIPCSequenceRecord(WORD wArchiv,WORD wSequenceNr)
{
	return m_srSequences.GetSequence(wArchiv,wSequenceNr);
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetNextCIPCSequenceRecord(const CIPCSequenceRecord& s)
{
	CIPCSequenceRecord* pNextSequence = NULL;

	// first find the sequence
	m_srSequences.Lock();

	BOOL f = FALSE;

	for (int i=0;i<m_srSequences.GetSize();i++)
	{
		if (!f)
		{
			if (   m_srSequences.GetAt(i)->GetArchiveNr() == s.GetArchiveNr()
				&& m_srSequences.GetAt(i)->GetSequenceNr() == s.GetSequenceNr())
			{
				// gefunden
				f = TRUE;
			}
		}
		else
		{
			if (m_srSequences.GetAt(i)->GetArchiveNr() == s.GetArchiveNr())
			{
				pNextSequence = m_srSequences.GetAt(i);
				break;
			}
		}
	}

	m_srSequences.Unlock();

	return pNextSequence;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetPrevCIPCSequenceRecord(const CIPCSequenceRecord& s)
{
	CIPCSequenceRecord* pPrevSequence = NULL;

	// first find the sequence
	m_srSequences.Lock();

	BOOL f = FALSE;

	for (int i=m_srSequences.GetSize()-1;i>=0;i--)
	{
		if (!f)
		{
			if (   m_srSequences.GetAt(i)->GetArchiveNr() == s.GetArchiveNr()
				&& m_srSequences.GetAt(i)->GetSequenceNr() == s.GetSequenceNr())
			{
				// gefunden
				f = TRUE;
			}
		}
		else
		{
			if (m_srSequences.GetAt(i)->GetArchiveNr() == s.GetArchiveNr())
			{
				pPrevSequence = m_srSequences.GetAt(i);
				break;
			}
		}
	}

	m_srSequences.Unlock();

	return pPrevSequence;
}
//////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CServer::GetCIPCArchivRecord(WORD wArchiv)
{
	if (IsConnected())
	{
		return m_pDatabase->GetArchiv(wArchiv);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CIPCArchivFileRecord* CServer::GetCIPCArchivFileRecord(WORD wArchiv,WORD wDir)
{
	int i,c;
	CIPCArchivFileRecord* pFile;

	m_arFiles.Lock();

	c = m_arFiles.GetSize();
	for (i=0;i<c;i++)
	{
		pFile = m_arFiles.GetAt(i);
		if ((pFile->GetArchivNr()==wArchiv) &&
			(pFile->GetDirNr()==wDir))
		{
			m_arFiles.Unlock();
			return pFile;
		}
	}

	m_arFiles.Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetArchivName(WORD wArchiv)
{
	if (IsConnected())
	{
		return m_pDatabase->GetArchivName(wArchiv);
	}
	return "";
}
//////////////////////////////////////////////////////////////////////
void CServer::SetVersion(const CString& sVersion)
{
	WK_TRACE("SERVER VERSION is <%s>\n",sVersion);
	int iMajorVersion,iMinorVersion,iSubVersion,iBuildNr;

	iMajorVersion = iMinorVersion = iSubVersion = iBuildNr = 0;
	if (4 == sscanf(sVersion,"%d.%d.%d.%d",&iMajorVersion,&iMinorVersion,&iSubVersion,&iBuildNr))
	{
		m_iMajorVersion = iMajorVersion;
		m_iMinorVersion = iMinorVersion;
		m_iSubVersion = iSubVersion;
		m_iBuildNr = iBuildNr;
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::InitFields3x()
{
	m_Fields.SafeDeleteAll();

	CString s;
	// 3.6
/*
	s.LoadString(IDS_DTP_TIME);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTime,s,6,'T'));
	s.LoadString(IDS_DTP_DATE);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
	s.LoadString(IDS_GA_NR);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfGaNr,s,6,'N'));
	s.LoadString(IDS_TA_NR);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTaNr,s,4,'N'));
	s.LoadString(IDS_KTO_NR);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfKtNr,s,16,'N'));
	s.LoadString(IDS_BANKLEITZAHL);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfBcNr,s,10,'N'));
	s.LoadString(IDS_VALUE);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfAmnt,s,8,'N'));
	s.LoadString(IDS_CURRENCY);
	m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfCurr,s,3,'N'));
*/
}
//////////////////////////////////////////////////////////////////////
void CServer::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	m_Fields.SafeDeleteAll();
	m_Fields.FromArray(iNumRecords,data);
#ifdef _DEBUG
	for (int i=0;i<m_Fields.GetSize();i++)
	{
		TRACE("%s, %s\n",m_Fields.GetAt(i)->GetName(),m_Fields.GetAt(i)->GetValue());
	}
#endif
/*
	if (IsLocal())
	{
		if (!theApp.m_bBackupModus)
		{
			if (theApp.m_bSearchMaskAtStart)
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_SEARCH);
			}
		}
	}
*/
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetFieldName(const CString& sField)
{
	CIPCField* pField = m_Fields.GetCIPCField(sField);
	if (pField)
	{
		return pField->GetValue();
	}
	return "";

}
//////////////////////////////////////////////////////////////////////
void CServer::OpenSequence(WORD wArchiv, WORD wSequenceNr, DWORD dwRecordNr /* = 1*/)
{
	CDBCheckView* pDW = m_pDoc->GetDBCheckView();
   if (pDW)
   {
      pDW->SetFirstRequestedNr(0xffffffff);
   }
   //m_pDatabase->DoRequestRecordNr(wArchiv, wSequenceNr, dwRecordNr-1, dwRecordNr, (DWORD)-2);
   m_pDatabase->DoRequestRecords(wArchiv, wSequenceNr, 0, 0);
/*
	if (m_pDatabase->GetVersion()<3)
	{
//		if (pDW==NULL)
		{
			CIPCArchivFileRecord* pFile = GetCIPCArchivFileRecord((BYTE)wArchiv,(WORD)wSequenceNr);
			if (pFile && IsConnected())
			{
				if (theApp.m_pUser)
				{
					WK_TRACE_USER("%s öffnet Archiv %d Sequence %d auf %s\n",
						theApp.m_pUser->GetName(),
						wArchiv,wSequenceNr,
						GetName());
				}
				m_pDatabase->DoRequestOpenFile(*pFile);
			}
			else
			{
				WK_TRACE_ERROR("try to open non existing sequence\n");
			}
		}
//		if ( (dwRecordNr>1) && WK_IS_WINDOW(pDW))
		{
			// goto the record!
//			pDW->Navigate(dwRecordNr);
		}
	}
	else
	{
//		if (pDW==NULL)
		{
			if (theApp.m_pUser)
			{
				WK_TRACE_USER("%s öffnet Archiv %d Sequence %d auf %s\n",
					theApp.m_pUser->GetName(),
					wArchiv,wSequenceNr,
					GetName());
			}
			CIPCSequenceRecord* pSequence = GetCIPCSequenceRecord(wArchiv,wSequenceNr); 

         m_pDoc->UpdateMyViews(NULL,
								  MAKELONG(VDH_OPEN_SEQUENCE,GetID()),
								  (CObject*)pSequence);

		}
		// goto the record!
//		if (pDW) pDW->Navigate(dwRecordNr);
	}
*/
}
//////////////////////////////////////////////////////////////////////
void CServer::DeleteArchivFiles()
{
	if (IsConnected())
	{
		DWORD dw;
		WORD  wArchivNr;
		WORD  wSequenceNr;

		while (m_arToDelete.GetSize())
		{
			dw = m_arToDelete.GetAt(0);
			wArchivNr = LOWORD(dw);
			wSequenceNr = HIWORD(dw);
			if (theApp.m_pUser)
			{
				WK_TRACE_USER("%s löscht Sequence %d aus Archiv %d\n",
				theApp.m_pUser->GetName(),wSequenceNr,wArchivNr);
			}
			if (m_pDatabase->GetVersion()<3)
			{
				CIPCArchivFileRecord* pFile = GetCIPCArchivFileRecord((BYTE)wArchivNr,(WORD)wSequenceNr);
				m_pDatabase->DoRequestRemoveFile(*pFile);
			}
			else
			{
				m_pDatabase->DoRequestDeleteSequence(wArchivNr,wSequenceNr);
			}
			m_arToDelete.RemoveAt(0);
			Sleep(5);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::AddDeleteArchivFile(WORD wArchiv, WORD wSequenceNr)
{
	if (IsConnected())
	{
		m_arToDelete.Add(MAKELONG(wArchiv,wSequenceNr)); // low / high
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::AddToOpenFile(const CIPCArchivFileRecord& data)
{
	m_arToOpenFiles.Lock();
	m_arToOpenFiles.Add(new CIPCArchivFileRecord(data));
	m_arToOpenFiles.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::ShowInfoDialog()
{
/*
	if (m_pDataBaseInfoDlg)
	{
		m_pDataBaseInfoDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDataBaseInfoDlg = new CDataBaseInfoDlg(AfxGetMainWnd(),this);
	}
*/
}
//////////////////////////////////////////////////////////////////////
void CServer::RequestRecord(WORD wArchiv, WORD wSequence, DWORD dwCurrentRecord,DWORD dwNewRecord,DWORD dwCamID)
{
	if (IsConnected())
	{
		if (m_pDatabase->GetVersion()<3)
		{
			CIPCArchivFileRecord* pFile = GetCIPCArchivFileRecord(wArchiv,wSequence);
			if (pFile)
			{
				CIPCArchivFileRecord data(*pFile);
				data.SetRecNo((WORD)dwNewRecord);
				m_pDatabase->DoRequestRecNo(data);
			}
		}
		else
		{
			TRACE("requesting A=%d, S=%d, CR=%d, NR=%d, CAM=%08lx\n",
				wArchiv,wSequence,dwCurrentRecord,dwNewRecord,dwCamID);
			m_pDatabase->DoRequestRecordNr(wArchiv,
										   wSequence,
										   dwCurrentRecord,
										   dwNewRecord,
										   dwCamID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::StopSearch()
{
	if (IsConnected() && IsSearching())
	{
		m_pDatabase->DoResponseQueryResult(0);
	}
	m_bSearching = FALSE;
	m_bFirstQueryResult = FALSE;
	TRACE("deleting query results by stop search srv%d\n",m_wID);
	m_QueryResultsToCopy.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CServer::StopSearchCopy()
{
	m_bStopSearchCopy = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::ClearSearch()
{
	m_bSearching = FALSE;
	m_bFirstQueryResult = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CServer::StartSearch()
{
	if (IsConnected())
	{
		m_bStopSearchCopy = FALSE;
		
		TRACE("deleting query results by start search srv%d\n",m_wID);
		m_QueryResultsToCopy.SafeDeleteAll();

		CIPCFields fields;

		for (int i=0;i<theApp.GetQueryParameter().GetFields().GetSize();i++)
		{
			fields.Add(new CIPCField(*theApp.GetQueryParameter().GetFields().GetAt(i)));
		}

		//serach between 2 dates and times
		if(theApp.GetQueryParameter().UseTimeFromTo())
		{
			if(    theApp.GetQueryParameter().UseExactDate()
				&& theApp.GetQueryParameter().UseExactTime())
			{

				CString sStartDatum = theApp.GetQueryParameter().GetDateExact().GetDBDate();
				CString sEndDatum = theApp.GetQueryParameter().GetPCDateEnd().GetDBDate();
				CString sStartTime = theApp.GetQueryParameter().GetTimeExact().GetDBTime();
				CString sEndTime = theApp.GetQueryParameter().GetTimeEnd().GetDBTime();
				
				TRACE("Startzeit: %s, Endzeit: %s, Startdatum: %s, Enddatum: %s\n",
															sStartTime,
															sEndTime,
															sStartDatum,
															sEndDatum);

				if(sStartDatum == sEndDatum && sStartTime == sEndTime)
				{
					fields.Add(new CIPCField("DBP_DATE",
							theApp.GetQueryParameter().GetDateExact().GetDBDate(),
							8,'D'));
					fields.Add(new CIPCField("DBP_TIME",
							theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
							8,'T'));
				}
				else if(sStartDatum == sEndDatum)
				{
					fields.Add(new CIPCField("DBP_TIME",
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,'}'));
					fields.Add(new CIPCField("DBP_TIME",
						theApp.GetQueryParameter().GetTimeEnd().GetDBTime(),
						8,'{'));
					fields.Add(new CIPCField("DBP_DATE",
							theApp.GetQueryParameter().GetDateExact().GetDBDate(),
							8,'D'));				
				}
				else
				{
					fields.Add(new CIPCField("DBP_TIME",
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,'}'));
					fields.Add(new CIPCField("DBP_TIME",
						theApp.GetQueryParameter().GetTimeEnd().GetDBTime(),
						8,'{'));
					fields.Add(new CIPCField("DBP_DATE", 
						theApp.GetQueryParameter().GetDateExact().GetDBDate(), 
						8,'}'));
					fields.Add(new CIPCField("DBP_DATE",
						theApp.GetQueryParameter().GetPCDateEnd().GetDBDate(),
						8,'{'));
				}

			}

			else if (theApp.GetQueryParameter().UseExactDate())
			{
				fields.Add(new CIPCField("DBP_DATE", 
					theApp.GetQueryParameter().GetDateExact().GetDBDate(), 
					8,'}'));
				fields.Add(new CIPCField("DBP_DATE",
					theApp.GetQueryParameter().GetPCDateEnd().GetDBDate(),
					8,'{'));
			}
		}
		else
		{
			if (theApp.GetQueryParameter().UseExactDate())
			{

				fields.Add(new CIPCField("DBP_DATE",
						theApp.GetQueryParameter().GetDateExact().GetDBDate(),
						8,'D'));
			}
			if (theApp.GetQueryParameter().UseExactTime())
			{
				fields.Add(new CIPCField("DBP_TIME",
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,'T'));
			}
		}

/*			Y-Achse
			
			^
	       sY1__|_ _ _ _ _______________
			|		|				|
			|		|				|
			|		|	       _|_..|.... Fadenkreuz auf MD-Bild
			|		|			|	|	  mit Koordinaten
			|		|				|	  DBD_MD_X und DBD_MD_Y
          sY2__|_ _ _ _|_______________|
			|		|				|
			|
			|_______|_______________|__ > X-Achse
					
					|				|
				   sX1			   sX2
*/	

		//search only within the rectangle
		if(theApp.GetQueryParameter().UseRectToQuery())
		{
			CString sX1, sY1, sX2, sY2;
			sX1.Format("%04x", theApp.GetQueryParameter().GetRectToQuery().left);
			fields.Add(new CIPCField("DBD_MD_X",
						sX1,
						4, '}'));
			sY1.Format("%04x", theApp.GetQueryParameter().GetRectToQuery().top);
			fields.Add(new CIPCField("DBD_MD_Y",
						sY1,
						4, '}'));
			sX2.Format("%04x", theApp.GetQueryParameter().GetRectToQuery().right);
			fields.Add(new CIPCField("DBD_MD_X",
						sX2,
						4, '{'));
			sY2.Format("%04x", theApp.GetQueryParameter().GetRectToQuery().bottom);
			fields.Add(new CIPCField("DBD_MD_Y",
						sY2,
						4, '{'));

		}

		int iNumArchives = m_QueryArchives.GetArchives().GetSize();

		if (m_pDatabase->GetVersion()<3)
		{
			CString sQuery;

			if (iNumArchives+1!=m_pDatabase->GetRecords().GetSize())
			{
				sQuery = m_QueryArchives.GetOldArchives();
			}

			sQuery += fields.BuildOldQueryString();

			if (!sQuery.IsEmpty())
			{
				if (theApp.m_pUser)
				{
					WK_TRACE_USER("%s startet suche %d auf <%s>\n",
						theApp.m_pUser->GetName(),m_dwQueryID,GetName());
					WK_TRACE_USER("nach <%s>\n",sQuery);
				}
				m_bSearching = TRUE;
				m_bFirstQueryResult = TRUE;
				m_pDatabase->DoRequestQuery(m_dwQueryID,sQuery);
				m_dwQueryID += 8;

			}
		}
		else
		{
			WORD* pArchives = NULL;
			if (iNumArchives>0)
			{
				pArchives = new WORD[iNumArchives];
				for (int i=0;i<iNumArchives;i++)
				{
					pArchives[i] = m_QueryArchives.GetArchives().GetAt(i);
				}
			}

			if ((iNumArchives>0) || (fields.GetSize()>0))
			{
				if (theApp.m_pUser)
				{
					CString s,a,t;
					int i;
					for (i=0;i<iNumArchives;i++)
					{
						t.Format("%d,",pArchives[i]);
						a += t;
					}
					for	(i=0;i<fields.GetSize();i++)
					{
						t.Format("%s,%s;",fields.GetAt(i)->GetName(),
							fields.GetAt(i)->GetValue());
						s += t;
					}
					WK_TRACE_USER("%s startet suche %d auf <%s>\n",
						theApp.m_pUser->GetName(),
						m_dwQueryID,GetName());
					WK_TRACE_USER("in %s\n",a);
					WK_TRACE_USER("nach %s\n",s);
				}
				m_bSearching = TRUE;
				m_bFirstQueryResult = TRUE;
				m_pDatabase->DoRequestNewQuery(m_dwQueryID,
											   iNumArchives,
											   pArchives,
											   fields);
				m_dwQueryID += 8;
			}
			WK_DELETE_ARRAY(pArchives);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnConfirmDrives(int iNumDrives, const CIPCDrive drives[])
{
	m_Drives.SafeDeleteAll();
	m_Drives.FromArray(iNumDrives,drives);
/*
	if (IsLocal() && (theApp.m_bBackupModus))
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ARCHIV_BACKUP);
	}
*/
}
//////////////////////////////////////////////////////////////////////
void CServer::AddErrorMessage(const CString& sMessage)
{
	Lock();
	m_saErrorMessages.Add(sMessage);
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddQueryResult(DWORD dwUserID, WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
						    int iNumFields, const CIPCField fields[])
{
	if (IsSearching())
	{
		m_QueryResultsToCopy.SafeAdd(new CQueryResult(dwUserID,m_wID,wArchivNr,wSequenceNr,
									 dwRecordNr,iNumFields,fields));
		if (m_bFirstQueryResult)
		{
			CopyNextQueryResult();
			m_bFirstQueryResult = FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CServer::IsQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr)
{
	BOOL bRet = FALSE;
	
	m_QueryResultsToCopy.Lock();

	for (int i=0;i<m_QueryResultsToCopy.GetSize();i++)
	{
		CQueryResult* pQueryResult = m_QueryResultsToCopy.GetAt(i);
		if (pQueryResult && 
			pQueryResult->IsEqual(m_wID,wArchivNr,wSequenceNr,dwRecordNr))
		{
			m_QueryResultsToCopy.RemoveAt(i);
			delete pQueryResult;
			bRet = TRUE;
			break;
		}
	}

	m_QueryResultsToCopy.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CServer::CopyNextQueryResult()
{
	CQueryResult* pQueryResult = NULL;
	m_QueryResultsToCopy.Lock();
	if(m_QueryResultsToCopy.GetSize()>0)
	{
		pQueryResult = m_QueryResultsToCopy.GetAt(0);
	}
	m_QueryResultsToCopy.Unlock();
	if (pQueryResult)
	{
		DoCopyQueryResult(pQueryResult);
	}
	else
	{
		m_bFirstQueryResult = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CServer::DoCopyQueryResult(CQueryResult* pQueryResult)
{
	// get the picture and copy it 
	if (IsConnected())
	{
		if (m_bStopSearchCopy)
		{
			m_bStopSearchCopy = FALSE;
		}
		else
		{
			CIPCDatabaseDBCheck* pDatabase = GetDatabase();
			if (pDatabase->GetVersion()>2)
			{
				pDatabase->DoRequestRecordNr(pQueryResult->GetArchiveNr(),
											 pQueryResult->GetSequenceNr(),
											 0,// current record
											 pQueryResult->GetRecordNr(),
											 (DWORD)-1);
			}
		}
	}
}

void CServer::InitServer(CDBCheckDoc *pDoc, WORD wID)
{
	m_pDoc = pDoc;
	m_wID  = wID;
}

void CServer::Reset()
{
   m_idHost.Set((DWORD)0);
	WK_DELETE(m_pConnectionRecord);
	WK_DELETE(m_pConnectThread);
	WK_DESTROY_WINDOW(m_pConnectDialog);
	m_arFiles.SafeDeleteAll();
	m_arNewFiles.SafeDeleteAll();
	m_arToOpenFiles.SafeDeleteAll();
	m_arNewArchivs.SafeDeleteAll();
	m_arUpdatedArchivs.SafeDeleteAll();
	m_srNewSequences.SafeDeleteAll();
	m_srSequences.SafeDeleteAll();
	m_QueryResultsToCopy.SafeDeleteAll();
}
