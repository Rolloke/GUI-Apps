// CIPCDatabaseStorage.cpp: implementation of the CIPCDatabaseStorage class.
//
//////////////////////////////////////////////////////////////////////

// Error Codes during Reset
//  0 request info during server reset
//  1 no permission
//  2 no permission
//  3 request file list during server reset
//  4 request sequence list during server reset
//  5 request save picture during server reset
//  6 request new save picture during server reset
//  7 request new save picture for host during server reset
//  8 request save picture for host during server reset
//  9 request open file during server reset
// 10 request rec no during server reset
// 11 request record nr during server reset
// 12 request camera names
// 13 request file close
// 14 request query
// 15 responce query result
// 16 request new query
// 17 request remove file 
// 18 request delete sequence
// 19 request delete archive
// 20 request drives
// 21 request backup
// 22 request cancel backup
// 23 request field info during server reset


#include "stdafx.h"
#include "DVStorage.h"
#include "CIPCDatabaseStorage.h"
#include "Collection.h"
#include "Search.h"
#include "SearchThread.h"
#include "Mainfrm.h"

//#define USE_TRACE_TIMER 1

#ifdef USE_TRACE_TIMER
 #include "wk_timer.h"

 CWK_Timer theTimer;
 #define ADD_MICRO_TIME_SPAN(TIME, TCK, OLDTCK) TCK    = theTimer.GetMicroTicks(); \
												TIME  += (DWORD) (TCK.QuadPart - OLDTCK.QuadPart); \
					                            OLDTCK = TCK;
#else
 #undef ADD_MICRO_TIME_SPAN
 #define ADD_MICRO_TIME_SPAN(TIME, TCK, OLDTCK) // TIME, TCK, OLDTCK
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseStorage::CIPCDatabaseStorage(CClient* pClient, LPCTSTR shmName)
	: CIPCDatabase(shmName, TRUE)
{
	m_bArchiveRequests = FALSE;
	m_bAlarmListRequests = FALSE;
	m_bDriveRequests = FALSE;

	m_pClient = pClient;
	m_dwClientVersion = 0;
	if (m_pClient)
	{
		WK_TRACE(_T("new Client %08lx %s\n"),m_pClient->GetID(),shmName);
	}

	m_wLastRequestArchiveNr = 0;
	m_wLastRequestSequenceNr = 0;
	m_dwLastRequestRecordNr = 0;

	m_currentArchive = 0;
	m_currentSequence = 0;
	m_dwCurrentRecord = 0;
	m_pH263Encoder = NULL;
	m_pMpeg4Encoder = NULL;
	m_pJpeg = NULL;
	m_bThreadIDTraced = FALSE;

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseStorage::~CIPCDatabaseStorage()
{
	WK_DELETE(m_pMpeg4Encoder);
	WK_DELETE(m_pH263Encoder);
	WK_DELETE(m_pJpeg);

	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	WK_TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
	WK_TRACE(_T("requesting client version\n"));
	DoRequestVersionInfo(0);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwClientVersion = dwVersion;
	WK_TRACE(_T("client version is %d\n"),m_dwClientVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestGetValue(CSecID id,
										    const CString &sKey,
										    DWORD dwServerData)
{
	UTRACE(_T("OnRequestGetValue:%s\n"), sKey);

	if (0==sKey.CompareNoCase(_T("isdv")))
	{
		DoConfirmGetValue(id,sKey,CSD_ON,dwServerData);
	}
	else if (0==sKey.CompareNoCase(_T("canskip")))
	{
		DoConfirmGetValue(id,sKey,CSD_ON,dwServerData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestSetValue(CSecID id,
										   const CString &sKey,
										   const CString &sValue,
										   DWORD dwServerData)
{
	UTRACE(_T("OnRequestSetValue:%s:%s\n"), sKey, sValue);
	if (sKey == CSD_NAME)
	{
		DWORD dwClientID = (DWORD)-1;
        if (m_pClient)
		{
			dwClientID = m_pClient->GetID();
		}
		theApp.SetCollectionName(dwClientID,id,sValue);
		DoConfirmSetValue(id,sKey,sValue,dwServerData);
	}
}
//////////////////////////////////////////////////////////////////////
// send list of all fields
void CIPCDatabaseStorage::OnRequestFieldInfo()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_FIELD_INFO_DURING_RESET, 
						NULL);
		return;
	}
	CIPCFields fields;
	DoConfirmFieldInfo(fields);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestCameraNames(WORD wCollectionNr,DWORD dwUserData)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wCollectionNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_CAMERA_NAMES_DURING_RESET,	NULL);
		return;
	}

	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	CCollection* pCollection = theApp.m_Archives.GetCollection(wCollectionNr);
	if (pCollection)
	{
		CIPCFields fields;
		CString rKey;
		rKey.Format(_T("%04x00%02x"),SECID_GROUP_OUTPUT,wCollectionNr & 0xFF);
		UTRACE(_T("OnRequestCameraNames:%s:%s\n"), rKey, pCollection->GetName());
		fields.Add(new CIPCField(rKey,pCollection->GetName(),64,_T('C')));
		DoConfirmCameraNames(wCollectionNr,dwUserData,fields);
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::CollectionToArchiveRecord(CCollection& arc, 
													CIPCArchivRecord& rec)
{
	rec.SetName(arc.GetName());
	rec.SetID(arc.GetNr());

	BYTE bFlags = 0;
	switch (arc.GetType())
	{
	case ALARM_ARCHIV:
		bFlags |= CAR_IS_ALARM;
		break;
	case RING_ARCHIV:
		bFlags |= CAR_IS_RING;
		break;
	case SICHERUNGS_RING_ARCHIV:
		bFlags |= CAR_IS_SAFE_RING;
		break;
	case BACKUP_ARCHIV:
		bFlags |= CAR_IS_BACKUP;
		break;
	case ALARM_LIST_ARCHIV:
		bFlags |= CAR_IS_ALARM_LIST;
		rec.SetSafeRingID(arc.GetSafeRingFor());
		break;
	}
	rec.SetFlags(bFlags);
	rec.SetSizeInMB(arc.IsRing() ? arc.GetSizeBytes().GetInMB() : arc.GetSizeMB());
	rec.SetBenutzterPlatz(arc.GetSizeBytes().GetInt64());
}
//////////////////////////////////////////////////////////////////////
// send list of all archivs
void CIPCDatabaseStorage::OnRequestInfo()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_ARCHIVE_DURING_RESET,	
						NULL);
		return;
	}

	if (!m_bThreadIDTraced)
	{
		m_bThreadIDTraced = TRUE;
		WK_TRACE(_T("OnRequestInfo, %s is Thread %08lx\n"),GetShmName(),GetCurrentThreadId());
	}

	CAutoCritSec acs(&theApp.m_Archives.m_cs);

	CCollection* pCollection;
	int i,c;
	CIPCArchivRecord *pData = NULL;
	
	c = 0;

	for (i=0;i<theApp.m_Archives.GetSize();i++)
	{
		pCollection = theApp.m_Archives.GetAtFast(i);
		if (   !pCollection->IsPreAlarm() 
			&& !pCollection->IsAlarmList())
		{
			c++;
		}
	}

	if (c>0)
	{
		int j=0;
		pData = new CIPCArchivRecord[c];
									   
		for (i=0; i<theApp.m_Archives.GetSize(); i++)
		{
			pCollection = theApp.m_Archives.GetAtFast(i);
			if (   !pCollection->IsPreAlarm()
				&& !pCollection->IsAlarmList())
			{
				CollectionToArchiveRecord(*pCollection, pData[j++]);
			}
		}
	}

	acs.Unlock();

	m_bArchiveRequests = TRUE;
	if (pData!=NULL)
	{
		DoConfirmInfo(c, pData);
	}

	WK_DELETE_ARRAY(pData);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestAlarmListArchives()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
			SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, 
			DBEC_ARCHIVE_DURING_RESET,
			NULL);
		return;
	}
	CAutoCritSec acs(&theApp.m_Archives.m_cs);

	CCollection* pCollection;
	int i,c;
	CIPCArchivRecord *pData = NULL;
	
	c = 0;

	for (i=0;i<theApp.m_Archives.GetSize();i++)
	{
		pCollection = theApp.m_Archives.GetAtFast(i);
		if (pCollection->IsAlarmList())
		{
			c++;
		}
	}

	if (c>0)
	{
		int j=0;
		pData = new CIPCArchivRecord[c];
									   
		for (i=0; i<theApp.m_Archives.GetSize(); i++)
		{
			pCollection = theApp.m_Archives.GetAtFast(i);
			if (pCollection->IsAlarmList())
			{
				CollectionToArchiveRecord(*pCollection, pData[j++]);
			}
		}
	}

	acs.Unlock();

	m_bAlarmListRequests = TRUE;
	if (pData!=NULL)
	{
		DoConfirmAlarmListArchives(c, pData);
	}

	WK_DELETE_ARRAY(pData);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::TapeToSequenceRecord(const CTape& seq, 
											   CIPCSequenceRecord& rec)
{
	rec.SetName(seq.GetName());
	rec.SetArchiveNr(seq.GetArchiveNr());
	rec.SetSequenceNr(seq.GetNr());
	rec.SetFlags(seq.GetFlags());
	rec.SetNrOfPictures(seq.GetNrOfRecords());
	rec.SetSize(seq.GetSizeBytes());
	rec.SetTime(seq.GetFirstTime());
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestSequenceList(WORD wArchivNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), 
						id, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SEQUENCE_DURING_RESET,	
						NULL);
		return;
	}
	CCollection* pCollection;
	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	pCollection = theApp.m_Archives.GetCollection(wArchivNr);
	if (pCollection)
	{
		int i,n;
		CIPCSequenceRecord* pData = NULL;

		CAutoCritSec acsC(pCollection->GetCS(), _T(__FUNCTION__), DVS_TIMEOUT);
		n = pCollection->GetNrTapes();
		if (n>0)
		{
			pData = new CIPCSequenceRecord[n];
			for (i=0; i<n; i++)
			{
				CTape* pTape = pCollection->GetTapeIndex(i);
				if (pTape)
				{
					TapeToSequenceRecord(*pTape,pData[i]);
				}
				else
				{
					WK_TRACE_ERROR(_T("Tape doesn't exist for client request %s\n"),
						pCollection->GetName());
				}
			}
		}
		acsC.Unlock();
		DoConfirmSequenceList(wArchivNr, n, pData);
		WK_DELETE_ARRAY(pData);
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestNewSavePicture(WORD wArchivNr1,
												  WORD wArchivNr2,
			 									  const CIPCPictureRecord &pict,
												  int iNumRecords,
												  const CIPCField fields[])
{
	CSecID id (SECID_GROUP_ARCHIVE,wArchivNr1);
	if (theApp.IsResetting() && !theApp.IsFastResetting())
	{
		DoIndicateError(GetLastCmd(), id, CIPC_ERROR_ACCESS_DENIED, DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}
	if (!m_bThreadIDTraced)
	{
		m_bThreadIDTraced = TRUE;
		WK_TRACE(_T("%s Thread %08lx\n"),GetShmName(),GetCurrentThreadId());
	}

	CImageData *pNI = new CImageData(id, new CIPCPictureRecord(pict), iNumRecords, fields);
	theApp.m_SaveThread.AddData(pNI);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestSaveMedia(	WORD wArchiveNr,
												CSecID hostID,
												BOOL	bIsAlarmConnection,
												BOOL	bIsSearchResult,
			 									const CIPCMediaSampleRecord &media,
												int iNumRecords,
												const CIPCField fields[])
{
	CSecID id (SECID_GROUP_ARCHIVE, wArchiveNr);
	if (theApp.IsResetting() && !theApp.IsFastResetting())
	{
		DoIndicateError(GetLastCmd(), id, CIPC_ERROR_ACCESS_DENIED, DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}
	if (!m_bThreadIDTraced)
	{
		m_bThreadIDTraced = TRUE;
		WK_TRACE(_T("%s Thread %08lx\n"),GetShmName(),GetCurrentThreadId());
	}

	theApp.m_SaveThread.AddData(new CImageData(id, new CIPCMediaSampleRecord(media), iNumRecords, fields));
}


//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestSaveData(WORD wArchivNr,
											  int iNumRecords,
											  const CIPCField fields[])
{
	CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
	if (theApp.IsResetting() && !theApp.IsFastResetting())
	{
		DoIndicateError(GetLastCmd(), id, CIPC_ERROR_ACCESS_DENIED, DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}


	CImageData*pNI = new CImageData(id, iNumRecords, fields);
	theApp.m_SaveThread.AddData(pNI);

	if (   theApp.m_bTraceSaveThread)
	{
		if (GetReceiveQueueLength() > 0 || GetSendQueueLength() > 0)
		{
			WK_TRACE(_T("ReceiveQueueLength %d, SendQueueLength %d\n"), GetReceiveQueueLength(), GetSendQueueLength());
			TraceOutstandingCmds();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestDisconnect()
{
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::SetLastConfirm(DWORD dwRecordNr)
{
	m_dwLastRequestRecordNr = dwRecordNr;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestRecordNr(WORD  wArchivNr, 
										    WORD  wTapeNr, 
										    DWORD dwCurrentRecordNr,
										    DWORD dwNewRecordNr,
										    DWORD dwCamID)
{
#ifdef _DEBUG
	CSystemTime st;
	st.GetLocalTime();
	//TRACE(_T("\n"));
	//TRACE(_T("##### request record: A=%04x, S=%d R=%d %s,%03d\n"),
	//	wArchivNr, wTapeNr,dwNewRecordNr,st.GetTime(),st.wMilliseconds);
#endif
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_RECORD_NR_DURING_RESET, NULL);
		TRACE(_T("DoIndicateError CMD=%08lx, id=%08lx\n"),GetLastCmd(),id.GetID());
		return;
	}
	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	CCollection* pCollection = theApp.m_Archives.GetCollection(wArchivNr);
	CTape* pTape = NULL;
	if (pCollection)
	{
		pTape = pCollection->GetTapeID(wTapeNr);
		if (pTape)
		{
			pTape->ConfirmRecord(dwCurrentRecordNr,dwNewRecordNr,dwCamID,this);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestRecNo no Sequence A=%04hx S=%d\n"),wArchivNr,wTapeNr);
			DWORD dwID = MAKELONG(wTapeNr,wArchivNr);
			DoIndicateError(dwID,dwNewRecordNr,CIPC_ERROR_INVALID_VALUE,3);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestRecNo no archive A=%04hx\n"),wArchivNr);
		DWORD dwID = MAKELONG(wTapeNr,wArchivNr);
		DoIndicateError(dwID,dwNewRecordNr,
			CIPC_ERROR_INVALID_VALUE,DBEC_RECORD_NR_NO_ARCHIVE);
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestAlarmListRecord(WORD  wArchivNr, 
												   WORD  wSequenceNr, 
												   DWORD dwRecordNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_RECORD_NR_DURING_RESET);
		TRACE(_T("DoIndicateError CMD=%08lx, id=%08lx\n"),GetLastCmd(),id.GetID());
		return;
	}

	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	CCollection* pCollection = theApp.m_Archives.GetCollection(wArchivNr);
	CTape* pTape = NULL;
	if (pCollection)
	{
		pTape = pCollection->GetTapeID(wSequenceNr);
		if (pTape)
		{
			CIPCFields fields;
			pTape->GetFields(dwRecordNr,fields);
			DoConfirmAlarmListRecord(wArchivNr,wSequenceNr,dwRecordNr,
									 pTape->GetNrOfRecords(),fields);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestAlarmListRecord no Sequence A=%04hx S=%d\n"),wArchivNr,wSequenceNr);
			DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
			DoIndicateError(dwID,dwRecordNr,CIPC_ERROR_INVALID_VALUE,3);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestAlarmListRecord no archive A=%04hx\n"),wArchivNr);
		DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
		DoIndicateError(dwID,dwRecordNr,CIPC_ERROR_INVALID_VALUE,4);
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestRecords(WORD  wArchivNr, 
										   WORD  wSequenceNr, 
										   DWORD dwFirstRecordNr, // 0 for all
										   DWORD dwLastRecordNr) // 0 for all
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, 
			DBEC_RECORD_NR_DURING_RESET, NULL);
		TRACE(_T("DoIndicateError CMD=%08lx, id=%08lx\n"),GetLastCmd(),id.GetID());
		return;
	}

	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	CCollection* pCollection = theApp.m_Archives.GetCollection(wArchivNr);
	CTape* pTape = NULL;
	if (pCollection)
	{
		pTape = pCollection->GetTapeID(wSequenceNr);
		if (pTape)
		{
			CIPCFields fields;
			CIPCFields records;
			CSystemTime t;

			if (pCollection->IsAlarmList())
			{
				CCollection* pRing = theApp.m_Archives.GetCollection(pCollection->GetSafeRingFor());
				if (   pRing
					&& pRing->GetNrTapes()>0)
				{
					CTape* pFirstTape = pRing->GetAtFast(0);
					if (pFirstTape)
					{
						t = pFirstTape->GetFirstTime();
					}
				}
			}

			pTape->GetFieldsAndRecords(dwFirstRecordNr,dwLastRecordNr,fields,records,t);
			DoConfirmRecords(wArchivNr,wSequenceNr,fields,records);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestRecords no Sequence A=%04hx S=%d\n"),wArchivNr,wSequenceNr);
			DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
			DoIndicateError(dwID,SECID_NO_ID,CIPC_ERROR_INVALID_VALUE,
				DBEC_RECORD_NR_NO_SEQUENCE);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestRecords no archive A=%04hx\n"),wArchivNr);
		DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
		DoIndicateError(dwID,SECID_NO_ID,
			CIPC_ERROR_INVALID_VALUE,DBEC_RECORD_NR_NO_ARCHIVE);
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::ConfirmRecordNr(WORD wArchivNr, 
										 WORD wSequenceNr, 
										 DWORD dwRecordNr,
										 DWORD dwNrOfRecords,
										 const CIPCPictureRecord &pict,
										 const CIPCFields& fields)
{
	if (   m_pClient->IsLowBandwidth()
		&& pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
//			m_pJpeg->SetProperties(768,288, IJL_YCBCR);
			m_pJpeg->SetProperties(768,576, IJL_YCBCR);
			if (m_pClient->UseH263())
			{
				m_pJpeg->EnableNoiseReduction();
			}
		}

		m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),
									  pict.GetDataLength(),
									  TRUE,
									  CJ_NOTHING,TRUE);

		int iWidth = m_pJpeg->GetImageDims().cx;
		int iHeight = m_pJpeg->GetImageDims().cy;
		if (   pict.GetResolution() == RESOLUTION_2CIF
			&& iWidth>384)
		{
			iWidth = 352;
			iHeight = 288;
		}
		else if (   pict.GetResolution() == RESOLUTION_QCIF
			&& iWidth>192)
		{
			iWidth = 176;
			iHeight = 144;
		}

		if (   m_pMpeg4Encoder
			&& (   m_pMpeg4Encoder->GetWidth()!=iWidth
			|| m_pMpeg4Encoder->GetHeight()!=iHeight)
			)
		{
			WK_DELETE(m_pMpeg4Encoder);
		}
		if (   m_pH263Encoder
			&& (   m_pH263Encoder->GetWidth()!=iWidth
			|| m_pH263Encoder->GetHeight()!=iHeight)
			)
		{
			WK_DELETE(m_pH263Encoder);
		}


		DWORD dwRecordDifference = 0;

		if (dwRecordNr > m_dwCurrentRecord)
		{
			dwRecordDifference = dwRecordNr-m_dwCurrentRecord;
		}
		else
		{
			dwRecordDifference = m_dwCurrentRecord-dwRecordNr;
		}

		if (   (m_currentArchive!= wArchivNr)
			|| (m_currentSequence!=wSequenceNr)
			|| (dwRecordDifference > 1)
			)

		{
			if (   m_pClient->UseH263()
				&& m_pH263Encoder)
			{
				m_pH263Encoder->ForceIntraFrame();
			}
			if (  m_pClient->UseMpeg4()
				&& m_pMpeg4Encoder)
			{
				WK_DELETE(m_pMpeg4Encoder);
			}
		}
		if (   m_pClient->UseH263()
			&& m_pH263Encoder == NULL)
		{
			m_pH263Encoder = new CH26xEncoder();
			m_pH263Encoder->Init(iWidth,iHeight,0,0,FALSE);
		}
		if (   m_pClient->UseMpeg4()
			&& m_pMpeg4Encoder == NULL)
		{
			m_pMpeg4Encoder = new CMPEG4Encoder();
			m_pMpeg4Encoder->Init(iWidth,iHeight,32000,5,25);
		}

		CIPCPictureRecord* pPict = NULL;
		if (   m_pClient->UseH263()
			&& m_pH263Encoder)
		{
			m_pH263Encoder->SetYUV422Data(pict.GetCamID(),
										m_pJpeg->GetImageDims().cx,
										m_pJpeg->GetImageDims().cy,
										m_pJpeg->GetDibBuffer());
			m_pH263Encoder->Run();
			pPict = m_pH263Encoder->GetEncodedPicture(this,pict.GetCamID(),pict.GetTimeStamp());
		}
		else if (   m_pClient->UseMpeg4()
			     && m_pMpeg4Encoder)
		{
			m_pMpeg4Encoder->SetYUV422Data(pict.GetCamID(),
											m_pJpeg->GetImageDims().cx,
											m_pJpeg->GetImageDims().cy,
											m_pJpeg->GetDibBuffer());
			m_pMpeg4Encoder->Encode();
			pPict = m_pMpeg4Encoder->GetEncodedPicture(this,pict.GetCamID(),pict.GetTimeStamp());
		}
		if (pPict)
		{
/*
			TRACE(_T("sending H.263 %s size %d in %d ms\n"),
				(pPict->GetPictureType()==SPT_FULL_PICTURE) ? _T("I") : _T("P"),
				pPict->GetDataLength(),
				GetTickCount()-dwTick);
*/

			DoConfirmRecordNr(wArchivNr,
							  wSequenceNr,
							  dwRecordNr,
							  dwNrOfRecords,
							  *pPict,
							  fields);
			m_currentArchive = wArchivNr;
			m_currentSequence = wSequenceNr;
			m_dwCurrentRecord = dwRecordNr;
		}
		else
		{
			TRACE(_T("no H.263 or Mpeg4 picture\n"));
		}
		WK_DELETE(pPict);
	}
	else
	{
#ifdef _DEBUG
		CSystemTime st;
		st.GetLocalTime();
		//TRACE(_T("\n"));
		//TRACE(_T("##### confirm record: A=%04x, S=%d R=%d %s,%03d\n"),
		//	wArchivNr, wSequenceNr,dwRecordNr,st.GetTime(),st.wMilliseconds);
#endif
		// confirm the real picture
		DoConfirmRecordNr(wArchivNr,
						  wSequenceNr,
						  dwRecordNr,
						  dwNrOfRecords,
						  pict,
						  fields);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnResponseQueryResult(DWORD dwUserID)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_QUERY_DURING_RESET, NULL);
		return;
	}
	CSearchThread* pSearchThread = NULL;
	for (int i=0;i<theApp.m_SearchThreads.GetSize();i++)
	{
		pSearchThread = theApp.m_SearchThreads.GetAtFast(i);
		if (pSearchThread->GetSearch()->GetClientID() == m_pClient->GetID())
		{
			if (dwUserID==0)
			{
				// search is to be cancelled
				pSearchThread->Cancel();
			}
			else
			{
				//
				pSearchThread->GetSearch()->IncreaseResponses();
			}
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults/*=0*/)
{
	if (theApp.IsResetting())
	{
		CSecID id;
		if (iNumArchives>0)
		{
			id = CSecID (SECID_GROUP_ARCHIVE,archives[0]);
		}
		DoIndicateError(GetLastCmd(), id, CIPC_ERROR_ACCESS_DENIED, 
			DBEC_QUERY_DURING_RESET, NULL);
		return;
	}
	theApp.m_Searchs.SafeAddTail(new CSearch(dwUserID,
									  m_pClient->GetID(),
									  iNumArchives,
									  archives,
									  iNumFields,
									  fields,
									  dwMaxQueryResults));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_QUERY);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestDeleteSequence(WORD wArchivNr, WORD wTapeNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_DELETE_SEQUENCE_DURING_RESET, NULL);
		return;
	}
	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	CCollection* pCollection = theApp.m_Archives.GetCollection(wArchivNr);
	pCollection->DeleteTape(wTapeNr);
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestDeleteArchiv(WORD wArchivNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_DELETE_ARCHIVE_DURING_RESET,	NULL);
		return;
	}
	if (theApp.m_Archives.DeleteCollection(wArchivNr))
	{
		WK_TRACE(_T("archive %d deleted\n"),wArchivNr);
		return;
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot delete archive %d\n"),wArchivNr);
		CString s;
		s.Format(IDP_CANNOT_DELETE_ARCHIV,wArchivNr);
		DoIndicateError(GetLastCmd(),
						CSecID(SECID_GROUP_ARCHIVE,wArchivNr),
						CIPC_ERROR_ARCHIVE,1,s);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestDrives()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_DRIVES_DURING_RESET, NULL);
		return;
	}
	theApp.m_Drives.CheckSpace();
	CAutoCritSec acs(&theApp.m_Archives.m_cs);
	DoConfirmDrives(theApp.m_Drives);
	acs.Unlock();
	m_bDriveRequests = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestBackup(DWORD dwUserData,
										 const CString& sDestinationPath,
										 const CString& sName,
										 int iNumIDs,
										 const DWORD dwIDs[])
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 
			DBEC_BACKUP_DURING_RESET, NULL);
		return;
	}
	WK_TRACE(_T("OnRequestBackup %d\n"),dwUserData);
	theApp.m_Backups.SafeAddTail(new CBackup(dwUserData,
											 m_pClient->GetID(),
											 sDestinationPath,
											 sName,
											 iNumIDs,
											 dwIDs
											 ));
	UTRACE(_T("OnRequestBackup:%s:%s\n"), sDestinationPath, sName);

	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_BACKUP);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestBackupByTime(WORD  wUserData,
											    const CString& sDestinationPath,
											    const CString& sName,
											    int iNumIDs,
											    const DWORD dwIDs[],
											    const CSystemTime& start,
											    const CSystemTime& end,
											    WORD  wFlags,
												DWORD dwMaximumInMB)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_BACKUP_DURING_RESET, NULL);
		return;
	}
	CString sIDs,sID;
	for (int i=0;i<iNumIDs;i++)
	{
		sID.Format(_T("%08x "),dwIDs[i]);
		sIDs += sID;
	}
	WK_TRACE(_T("OnRequestBackupByTime ID=%d <%s>-<%s> IDs=%d, %s FLAGS=%x max MB=%d\n"),
		wUserData,start.GetDateTime(),end.GetDateTime(),iNumIDs, sIDs,wFlags,dwMaximumInMB);
	theApp.m_Backups.SafeAddTail(new CBackup(wUserData,
											 m_pClient->GetID(),
											 sDestinationPath,
											 sName,
											 iNumIDs,
											 dwIDs,
											 start,
											 end,
											 wFlags,
											 dwMaximumInMB
											 ));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_BACKUP);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestCancelBackup(DWORD dwUserData)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_BACKUP_DURING_RESET, NULL);
		return;
	}
	WK_TRACE(_T("OnRequestCancelBackup %d\n"),dwUserData);
	theApp.CancelBackup(dwUserData);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseStorage::OnRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	//musste überschrieben werden, da ansonsten bei der Aufwahl auf den DTS vom IdipClient 
	//die "not overwritten" Fehlermeldung beim IdipClient hochpoppte
}