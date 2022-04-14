/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDatabase.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDatabase.cpp $
// CHECKIN:		$Date: 9.11.05 9:29 $
// VERSION:		$Revision: 140 $
// LAST CHANGE:	$Modtime: 9.11.05 9:12 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "CIPCDatabase.h"
#include "CIPCOutput.h"
#include "PictureRecord.h"
#include "CIPCExtraMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////
// util
CIPCExtraMemory* BubbleFromFieldsAndBubble(CIPC *pCipc,
 										   const CIPCFields& f,
										   const CIPCExtraMemory* pMem)
{
	CIPCExtraMemory* pBubble = NULL;
#ifdef _UNICODE
	DWORD dwLen1 = f.GetBubbleLength(pCipc->GetCodePage());
#else
	DWORD dwLen1 = f.GetBubbleLength();
#endif			
	DWORD dwLen2 = pMem->GetLength();
	DWORD dwSum = dwLen1 + dwLen2 + 2 * sizeof(DWORD);
	const void* pSrc2 = pMem->GetAddress();

	pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE* pByte = (BYTE*)pBubble->GetAddressForWrite();

		if (pByte)
		{
			*pByte++ = HIBYTE(HIWORD(dwLen1));
			*pByte++ = LOBYTE(HIWORD(dwLen1));
			*pByte++ = HIBYTE(LOWORD(dwLen1));
			*pByte++ = LOBYTE(LOWORD(dwLen1));
#ifdef _UNICODE
			DWORD dwDelta = f.ToMemory(pByte, pCipc->GetCodePage());
#else
			DWORD dwDelta = f.ToMemory(pByte);
#endif			
			WK_ASSERT(dwDelta == dwLen1);
			*pByte++ = HIBYTE(HIWORD(dwLen2));
			*pByte++ = LOBYTE(HIWORD(dwLen2));
			*pByte++ = HIBYTE(LOWORD(dwLen2));
			*pByte++ = LOBYTE(LOWORD(dwLen2));
			CopyMemory(pByte,pSrc2,dwLen2);
		}
		else
		{
			WK_DELETE(pBubble);
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromFieldsAndPicture\n"));
		}
	}

	return pBubble;
}
/////////////////////////////////////////////////////////////////
CIPCField* FieldsFromDoubleBubble(int& iNumRecords ,const CIPCExtraMemory& src)
{
	const BYTE *pByte = (const BYTE*)src.GetAddress();
	if (pByte)
	{
		DWORD dw = 0;
		dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
			| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
		pByte += 4;
		if (dw<=src.GetLength())
		{
			return CIPCFields::FromMemory(dw,pByte,iNumRecords 
#ifdef _UNICODE
											,src.GetCIPC()->GetCodePage()
#endif
											  );
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////
CIPCExtraMemory* GetSecondBubble(CIPC *pCipc, const CIPCExtraMemory& src)
{
	CIPCExtraMemory* pBubble = NULL;
	const BYTE *pByte = (const BYTE*)src.GetAddress();

	if (pByte)
	{
		DWORD dw = 0;
		dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
			| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
		if (dw<=src.GetLength())
		{
			pByte += 4;
			pByte += dw;
			dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
				| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
			if (dw<=src.GetLength())
			{
				pByte += 4;

				pBubble = new CIPCExtraMemory();
				if (!pBubble->Create(pCipc,dw,pByte))
				{
					WK_DELETE(pBubble);
					WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in GetSecondBubble\n"));
				}
			}
		}
	}
	return pBubble;
}

// 
/////////////////////////////////////////////////////////////////
/* 
Function: constructor 
param: const char * | shmName | The systemwide unique shared memory name
for the CIPC object. The other process must use the same name.
param: BOOL | asMaster | The process which creates it's object first is master, so
asMaster must be TRUE, the second process must use FALSE. The server process
uses most times the asMaster flag as TRUE.
*/
CIPCDatabase::CIPCDatabase(LPCTSTR shmName, BOOL bMaster) 
		: CIPC(shmName, bMaster) 
{
}

void CIPCDatabase::DoRequestInfo()
{
	WriteCmd(CIPC_DB_REQUEST_INFO);
}

/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::DoRequestBackupInfo()
{
	WriteCmd(CIPC_DB_REQUEST_BACKUP_INFO);
}
//////////////////////////////////////////////////////////////////
static BYTE * DwordsToByteArray(int &numArchives,
					   DWORD dw1, DWORD dw2,
					   DWORD dw3, DWORD dw4
					   )
{
	signed char by = HIBYTE(HIWORD(dw1));
	numArchives = by;

	WK_ASSERT((numArchives>0 && numArchives<16) || numArchives<0);
	BYTE *pData;
	if (numArchives<0) pData = new BYTE[2];
	else pData = new BYTE[numArchives];

	// CAVEAT -1 used for hostSubID
	if (numArchives>=1 || numArchives<0) pData[0] = LOBYTE(HIWORD(dw1));
	if (numArchives>=2 || numArchives<0) pData[1] = HIBYTE(LOWORD(dw1));
	if (numArchives>=3) pData[2] = LOBYTE(LOWORD(dw1));

	if (numArchives>=4) pData[3] = HIBYTE(HIWORD(dw2));
	if (numArchives>=5) pData[4] = LOBYTE(HIWORD(dw2));
	if (numArchives>=6) pData[5] = HIBYTE(LOWORD(dw2));
	if (numArchives>=7) pData[6] = LOBYTE(LOWORD(dw2));

	if (numArchives>=8) pData[7] = HIBYTE(HIWORD(dw3));
	if (numArchives>=9) pData[8] = LOBYTE(HIWORD(dw3));
	if (numArchives>=10) pData[9] = HIBYTE(LOWORD(dw3));
	if (numArchives>=11) pData[10] = LOBYTE(LOWORD(dw3));

	if (numArchives>=12) pData[11] = HIBYTE(HIWORD(dw4));
	if (numArchives>=13) pData[12] = LOBYTE(HIWORD(dw4));
	if (numArchives>=14) pData[13] = HIBYTE(LOWORD(dw4));
	if (numArchives>=15) pData[14] = LOBYTE(LOWORD(dw4));

	return pData;
}
void ByteArrayToDwords(int numArchives, const BYTE archiveIDs[],
					   DWORD &dw1, DWORD &dw2,
					   DWORD &dw3, DWORD &dw4
					   )
{
	WK_ASSERT((numArchives>0 && numArchives<16) || numArchives<0);
	dw1=dw2=dw3=dw4=0;
	dw1=((DWORD)(numArchives & 0xff))<<24;	// store number

	// CAVEAT -1 used for hostSubID
	if (numArchives>=1 || numArchives<0) dw1 |= ((DWORD)archiveIDs[0])<<16;
	if (numArchives>=2 || numArchives<0) dw1 |= ((DWORD)archiveIDs[1])<<8;
	if (numArchives>=3) dw1 |= ((DWORD)archiveIDs[2])<<0;
	
	if (numArchives>=4) dw2 |= ((DWORD)archiveIDs[3])<<24;
	if (numArchives>=5) dw2 |= ((DWORD)archiveIDs[4])<<16;
	if (numArchives>=6) dw2 |= ((DWORD)archiveIDs[5])<<8;
	if (numArchives>=7) dw2 |= ((DWORD)archiveIDs[6])<<0;

	if (numArchives>=8) dw3 |= ((DWORD)archiveIDs[7])<<24;
	if (numArchives>=9) dw3 |= ((DWORD)archiveIDs[8])<<16;
	if (numArchives>=10) dw3 |= ((DWORD)archiveIDs[9])<<8;
	if (numArchives>=11) dw3 |= ((DWORD)archiveIDs[10])<<0;

	if (numArchives>=12) dw4 |= ((DWORD)archiveIDs[11])<<24;
	if (numArchives>=13) dw4 |= ((DWORD)archiveIDs[12])<<16;
	if (numArchives>=14) dw4 |= ((DWORD)archiveIDs[13])<<8;
	if (numArchives>=15) dw4 |= ((DWORD)archiveIDs[14])<<0;

}

BOOL CIPCDatabase::HandleCmd(DWORD dwCmd,
							 DWORD dwParam1, 
							 DWORD dwParam2,
							 DWORD dwParam3, 
							 DWORD dwParam4,
							 const CIPCExtraMemory *pExtraMem)
{
	BOOL isOkay=TRUE;
	switch (dwCmd) 
	{
	case CIPC_DB_REQUEST_INFO:
		OnRequestInfo();
		break;
	case CIPC_DB_CONFIRM_INFO:
		if (pExtraMem)
		{
			int iNumRecords = (int) dwParam1;
			CIPCArchivRecord *pData = CIPCArchivRecord::ArchivRecordsFromBubble(iNumRecords,pExtraMem);

			OnConfirmInfo(iNumRecords, pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_BACKUP_INFO:
		OnRequestBackupInfo();
		break;
	case CIPC_DB_CONFIRM_BACKUP_INFO:
		if (pExtraMem)
		{
			int iNumRecords = (int) dwParam1;
			CIPCArchivRecord *pData = CIPCArchivRecord::ArchivRecordsFromBubble(iNumRecords,pExtraMem);

			OnConfirmBackupInfo(iNumRecords, pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_SAVE_PICTURE:
		if (pExtraMem) 
		{
			CIPCPictureRecord pict(*pExtraMem);
			int numArchives;
			BYTE *pData;
			pData = DwordsToByteArray(numArchives,
				dwParam1, dwParam2,
				dwParam3, dwParam4
				);
			OnRequestSavePicture(numArchives, pData, pict);
			WK_DELETE(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_SAVE_BY_HOST:
		if (pExtraMem) 
		{
			CIPCPictureRecord pict(*pExtraMem);
			CSecID hostID(dwParam1);
			BOOL bAlarm = (dwParam2)!=0;
			BOOL bSearchResult = (dwParam3)!=0;
			OnRequestSavePictureForHost(hostID,pict,bAlarm,bSearchResult);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;

	case CIPC_DB_REQUEST_QUERY:
		if (pExtraMem) 
		{
			CString tmpString = pExtraMem->GetString();
			OnRequestQuery(dwParam1, tmpString);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_INDICATE_QUERY_RESULT:
		if (pExtraMem) 
		{
			CIPCPictureRecord pictData(*pExtraMem);
			OnIndicateQueryResult(dwParam1, pictData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_RESPONSE_QUERY_RESULT:
		OnResponseQueryResult(dwParam1);
		break;
	case CIPC_DB_CONFIRM_QUERY:
		OnConfirmQuery(dwParam1);
		break;
	case CIPC_DB_INDICATE_REMOVE_ARCHIV:
		OnIndicateRemoveArchiv((WORD)dwParam1);
		break;

	case CIPC_DB_INDICATE_NEW_ARCHIV:
		{
			CIPCArchivRecord *pData = CIPCArchivRecord::ArchivRecordsFromBubble(1,pExtraMem);
			OnIndicateNewArchiv(*pData);
			WK_DELETE_ARRAY(pData);
		}
		break;
	// new 4.0 stuff
	case CIPC_DB_REQUEST_SEQUENCE_LIST:
		OnRequestSequenceList((WORD)dwParam1);
		break;
	case CIPC_DB_CONFIRM_SEQUENCE_LIST:
		if (pExtraMem)
		{
			int iNumRecords = (int)dwParam2;
			CIPCSequenceRecord *pData = NULL;
			if (iNumRecords>0)
			{
				pData = CIPCSequenceRecord::SequenceRecordsFromBubble(iNumRecords,pExtraMem);
			}
			OnConfirmSequenceList((WORD)dwParam1,iNumRecords, pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnConfirmSequenceList((WORD)dwParam1,0,NULL);
		}
		break;
	case CIPC_DB_REQUEST_RECORD_NR:
		OnRequestRecordNr(LOWORD(dwParam1), // wArchivNr
						  HIWORD(dwParam1), // wSequenceNr
						  dwParam2,			// dwCurrentRecordNr
						  dwParam3,			// dwNewRecordNr
						  dwParam4);		// dwCamID
		break;
	case CIPC_DB_CONFIRM_RECORD_NR:
		if (pExtraMem)
		{
			WORD wArchivNr = LOWORD(dwParam1);
			WORD wSequenceNr = HIWORD(dwParam1);
			DWORD dwRecordNr = (DWORD)dwParam2;
			DWORD dwNrOfRecords = (DWORD)dwParam3;
			int iNumRecords = (int)dwParam4;

			CIPCField* pData = FieldsFromDoubleBubble(iNumRecords,*pExtraMem);
			CIPCExtraMemory* pPictBubble = GetSecondBubble(this,*pExtraMem);

			if (pPictBubble)
			{
				CIPCPictureRecord pict(*pPictBubble);

				OnConfirmRecordNr(wArchivNr,wSequenceNr,
								  dwRecordNr,dwNrOfRecords,
								  pict,iNumRecords,pData);
			}
			else
			{
				OnCommandReceiveError(dwCmd);
			}
			WK_DELETE_ARRAY(pData);
			WK_DELETE(pPictBubble);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_CONFIRM_RECORD_NR_MEDIA:
		if (pExtraMem)
		{
			WORD wArchivNr = LOWORD(dwParam1);
			WORD wSequenceNr = HIWORD(dwParam1);
			DWORD dwRecordNr = (DWORD)dwParam2;
			DWORD dwNrOfRecords = (DWORD)dwParam3;
			CSecID idMedia(dwParam4);

			int iNumRecords = 0;
			CIPCField* pData = FieldsFromDoubleBubble(iNumRecords,*pExtraMem);
			CIPCExtraMemory* pMediaBubble = GetSecondBubble(this,*pExtraMem);

			if (pMediaBubble)
			{
				CIPCMediaSampleRecord media(idMedia, *pMediaBubble, false);

				OnConfirmRecordNr(wArchivNr,
								  wSequenceNr,
								  dwRecordNr,
								  dwNrOfRecords,
								  media,
								  iNumRecords,pData);
			}
			else
			{
				OnCommandReceiveError(dwCmd);
			}
			WK_DELETE_ARRAY(pData);
			WK_DELETE(pMediaBubble);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
		break;
	case CIPC_DB_REQUEST_DELETE_SEQUENCE:
		OnRequestDeleteSequence((WORD)dwParam1,(WORD)dwParam2);
		break;
	case CIPC_DB_INDICATE_DELETE_SEQUENCE:
		OnIndicateDeleteSequence((WORD)dwParam1,(WORD)dwParam2);
		break;
	case CIPC_DB_REQUEST_FIELD_INFO:
		OnRequestFieldInfo();
		break;
	case CIPC_DB_CONFIRM_FIELD_INFO:
		if (pExtraMem)
		{
			int iNumRecords = (int) dwParam1;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iNumRecords,pExtraMem);
			OnConfirmFieldInfo(iNumRecords, pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_INDICATE_NEW_SEQUENCE:
		if (pExtraMem)
		{
			CIPCSequenceRecord *pData = CIPCSequenceRecord::SequenceRecordsFromBubble(1,pExtraMem);
			OnIndicateNewSequence(pData[0],(WORD)dwParam1,dwParam2);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_NEW_REQUEST_SAVE:
		if (pExtraMem)
		{
			WORD wArchivNr1 = (WORD)dwParam1;
			WORD wArchivNr2 = (WORD)dwParam3;
			int iNumRecords = (int)dwParam2;

			CIPCField *pData = FieldsFromDoubleBubble(iNumRecords,*pExtraMem);
			CIPCExtraMemory* pPictBubble = GetSecondBubble(this,*pExtraMem);
			if (pPictBubble)
			{
				WK_ASSERT(pPictBubble->GetLength()<=pExtraMem->GetLength());
				CIPCPictureRecord pict(*pPictBubble);
				OnRequestNewSavePicture(wArchivNr1,wArchivNr2,pict,iNumRecords,pData);
			}
			else
			{
				OnCommandReceiveError(dwCmd);
				WK_TRACE_ERROR(_T("no bubble in CIPC_DB_NEW_REQUEST_SAVE\n"));
			}
			WK_DELETE_ARRAY(pData);
			WK_DELETE(pPictBubble);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST:
		if (pExtraMem)
		{
			CSecID hostID(dwParam1);
		    BOOL bIsAlarmConnection = dwParam2;
		    BOOL bIsSearchResult = dwParam3;
			int iNumRecords = (int)dwParam4;

			CIPCField *pData = FieldsFromDoubleBubble(iNumRecords,*pExtraMem);
			CIPCExtraMemory* pPictBubble = GetSecondBubble(this,*pExtraMem);
			if (pData && pPictBubble)
			{
				CIPCPictureRecord pict(*pPictBubble);

				OnRequestNewSavePictureForHost(hostID,
											   bIsAlarmConnection,
											   bIsSearchResult,
			 								   pict,
											   iNumRecords,
											   pData);
			}
			else
			{
				OnCommandReceiveError(dwCmd);
				WK_TRACE(_T("no bubble in CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST\n"));
			}
			WK_DELETE_ARRAY(pData);
			WK_DELETE(pPictBubble);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_NEW_QUERY:
		if (pExtraMem) 
		{
			const BYTE *pByte = (const BYTE*)pExtraMem->GetAddress();
			int iNumArchives = (int)dwParam2;
			int iNumFields = (int)dwParam3;
			DWORD dwMaxQueryResults = dwParam4;
			WORD* pArchives = new WORD[iNumArchives];

			for (int i=0;i<iNumArchives;i++)
			{
				pArchives[i] = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
				pByte += 2;
			}
			DWORD dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
				| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
			pByte += 4;
			CIPCExtraMemory* pFieldBubble = new CIPCExtraMemory();
			if (pFieldBubble->Create(this,dw,pByte))
			{
				CIPCField *pFields = CIPCFields::FieldsFromBubble(iNumFields,pFieldBubble);
				
				OnRequestNewQuery(dwParam1,
								  iNumArchives,
								  pArchives,
								  iNumFields,
								  pFields,
								  dwMaxQueryResults);
				WK_DELETE_ARRAY(pFields);
			}
			else
			{
				WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in CIPC_DB_REQUEST_NEW_QUERY\n"));
			}
			WK_DELETE_ARRAY(pArchives);
			WK_DELETE(pFieldBubble);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_INDICATE_NEW_QUERY_RESULT:
		if (pExtraMem)
		{
			int iNumFields = (int) dwParam4;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
			OnIndicateNewQueryResult(dwParam1,
								     HIWORD(dwParam2),
							         LOWORD(dwParam2),
							         dwParam3,
									 iNumFields,
									 pData);		
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_DRIVES:
		OnRequestDrives();
		break;
	case CIPC_DB_CONFIRM_DRIVES:
		if (pExtraMem)
		{
			int iNumDrives = dwParam1;
			CIPCDrive *pData = CIPCDrives::DrivesFromBubble(iNumDrives,pExtraMem);
			OnConfirmDrives(iNumDrives,pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_BACKUP:
		if (pExtraMem)
		{
			CWK_String sPath,sName;
			DWORD dwUserData = dwParam1;
			int iNumIDs = (int)dwParam2;
			DWORD* pDWORD = NULL;
			const BYTE* pByte = (const BYTE*)pExtraMem->GetAddress();
#ifdef _UNICODE
			sPath = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
			sName = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
#else
			sPath = CIPCExtraMemory::Memory2CString(pByte);
			sName = CIPCExtraMemory::Memory2CString(pByte);
#endif

			pDWORD = new DWORD[iNumIDs];

			for (int i=0;i<iNumIDs;i++)
			{
				pDWORD[i] = CIPCExtraMemory::Memory2DWORD(pByte);
			}

			OnRequestBackup(dwUserData,sPath,sName,iNumIDs,pDWORD);

			WK_DELETE_ARRAY(pDWORD);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_CONFIRM_BACKUP:
		if (pExtraMem)
		{
			CString s;
			const BYTE* pByte = (const BYTE*)pExtraMem->GetAddress();
#ifdef _UNICODE
			s = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
#else
			s = CIPCExtraMemory::Memory2CString(pByte);
#endif
			OnConfirmBackup(dwParam1,s,dwParam2);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_CANCEL_BACKUP:
		OnRequestCancelBackup(dwParam1);
		break;
	case CIPC_DB_CONFIRM_CANCEL_BACKUP:
		OnConfirmCancelBackup(dwParam1);
		break;
	case CIPC_DB_REQUEST_DELETE_ARCHIV:
		OnRequestDeleteArchiv((WORD)dwParam1);
		break;
	case CIPC_DB_REQUEST_CAMERA_NAMES:
		OnRequestCameraNames((WORD)dwParam1,dwParam2);
		break;
	case CIPC_DB_CONFIRM_CAMERA_NAMES:
		if (pExtraMem)
		{
			WORD wArchivNr = (WORD)dwParam1;
			DWORD dwUserData = dwParam2;
			int iNumFields = (int) dwParam3;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
			OnConfirmCameraNames(wArchivNr,dwUserData,iNumFields,pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_NEW_CONFIRM_SAVE:
		OnConfirmNewSavePicture((WORD)dwParam1,CSecID(dwParam2));
		break;
	case CIPC_DB_REQUEST_BACKUP_BY_TIME:
		if (pExtraMem)
		{
			CString sPath,sName;
			WORD wUserData = (WORD)dwParam1;
			WORD wFlags = (WORD)dwParam2;
			int iNumIDs = (int)dwParam3;
			DWORD* pDWORD = NULL;
			const BYTE* pByte = (const BYTE*)pExtraMem->GetAddress();
			CSystemTime start,end;
			
#ifdef _UNICODE
			sPath = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
			sName = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
#else
			sPath = CIPCExtraMemory::Memory2CString(pByte);
			sName = CIPCExtraMemory::Memory2CString(pByte);
#endif
		
			pDWORD = new DWORD[iNumIDs];

			for (int i=0;i<iNumIDs;i++)
			{
				pDWORD[i] = CIPCExtraMemory::Memory2DWORD(pByte);
			}

			start.ReadFromBubble(pByte);
			end.ReadFromBubble(pByte);

			OnRequestBackupByTime(wUserData,
								  sPath,
								  sName,
								  iNumIDs,
								  pDWORD,
								  start,
								  end,
								  wFlags,
								  dwParam4);

			WK_DELETE_ARRAY(pDWORD);

		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_CONFIRM_BACKUP_BY_TIME:
		if (pExtraMem)
		{
			WORD wUser = LOWORD(dwParam1);
			WORD wFlags = HIWORD(dwParam1);
			DWORD dwID = dwParam2;
			ULARGE_INTEGER i;
			i.LowPart = dwParam3;
			i.HighPart = dwParam4;
			const BYTE* pByte = (const BYTE*)pExtraMem->GetAddress();
			CSystemTime start,end;
			start.ReadFromBubble(pByte);
			end.ReadFromBubble(pByte);
			OnConfirmBackupByTime(wUser,dwID,start,end,wFlags,i);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_ALARM_LIST_ARCHIVES:
		OnRequestAlarmListArchives();
		break;
	case CIPC_DB_CONFIRM_ALARM_LIST_ARCHIVES:
		if (pExtraMem)
		{
			int iNumRecords = (int) dwParam1;
			CIPCArchivRecord *pData = CIPCArchivRecord::ArchivRecordsFromBubble(iNumRecords,pExtraMem);

			OnConfirmAlarmListArchives(iNumRecords, pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_ALARM_LIST_RECORD:
		OnRequestAlarmListRecord((WORD)dwParam1,(WORD)dwParam2,dwParam3);
		break;
	case CIPC_DB_CONFIRM_ALARM_LIST_RECORD:
		if (pExtraMem)
		{
			WORD wArchivNr = LOWORD(dwParam1);
			WORD wSequenceNr = HIWORD(dwParam1);
			DWORD dwRecord = dwParam2;
			DWORD dwNumRecords = dwParam3;
			int iNumFields = (int) dwParam4;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
			OnConfirmAlarmListRecord(wArchivNr,
									 wSequenceNr,
									 dwRecord,
									 dwNumRecords,
									 iNumFields,
									 pData);
			WK_DELETE_ARRAY(pData);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_SAVE_DATA:
		if (pExtraMem)
		{
			WORD wArchivNr = (WORD)dwParam1;
			int iNumFields = (int)dwParam2;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
			OnRequestSaveData(wArchivNr,
							  iNumFields,
 							  pData);
			WK_DELETE_ARRAY(pData);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_RECORDS:
		{
			WORD wArchivNr = LOWORD(dwParam1);
			WORD wSequenceNr = HIWORD(dwParam1);
			DWORD dwFirstRecord = dwParam2;
			DWORD dwLastRecord = dwParam3;
			OnRequestRecords(wArchivNr,wSequenceNr,dwFirstRecord,dwLastRecord);
		}
		break;
	case CIPC_DB_CONFIRM_RECORDS:
		if (pExtraMem)
		{
			WORD wArchivNr = LOWORD(dwParam1);
			WORD wSequenceNr = HIWORD(dwParam1);
			int iNumFields = (int)dwParam2;
			int iNumRecords = (int)dwParam3;
			int iDummy=0;
			CIPCField *pData = CIPCFields::FieldsFromBubble(iDummy,
														    pExtraMem);
			CIPCFields fields;
			CIPCFields records;
			fields.FromArray(iNumFields,pData);
			for (int i=iNumFields;i<iNumFields+iNumRecords;i++)
			{
				records.Add(new CIPCField(pData[i]));
			}
			OnConfirmRecords(wArchivNr,wSequenceNr,fields,records);
			WK_DELETE_ARRAY(pData);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DB_REQUEST_OPEN_SEQUENCE:
		OnRequestOpenSequence((WORD)dwParam1,(WORD)dwParam2);
		break;
	case CIPC_DB_REQUEST_CLOSE_SEQUENCE:
		OnRequestCloseSequence((WORD)dwParam1,(WORD)dwParam2);
		break;
	case CIPC_DB_REQUEST_SAVE_MEDIA:
		if (pExtraMem)
		{
			WORD wArchiveNr = LOWORD(dwParam1);
			BOOL bIsAlarmConnection	= LOBYTE(HIWORD(dwParam1));
			BOOL bIsSearchResult = HIBYTE(HIWORD(dwParam1));
			int  iNumRecords = (int)dwParam2;
			CSecID idHost(dwParam3);
			CSecID idMedia(dwParam4);

			CIPCField* pData = FieldsFromDoubleBubble(iNumRecords,*pExtraMem);
			CIPCExtraMemory* pMediaBubble = GetSecondBubble(this,*pExtraMem);

			if (pMediaBubble)
			{
				CIPCMediaSampleRecord media(idMedia,*pMediaBubble);
				OnRequestSaveMedia(wArchiveNr,idHost,bIsAlarmConnection,bIsSearchResult,
					media,iNumRecords,pData);
			}
			else
			{
				OnCommandReceiveError(dwCmd);
			}
			WK_DELETE_ARRAY(pData);
			WK_DELETE(pMediaBubble);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;

	default:
		// call superclass
		isOkay = CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,pExtraMem);
		break;
	} // end of switch
	return isOkay;
}
////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::OnRequestInfo()
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::OnRequestBackupInfo()
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::DoConfirmBackupInfo(int iNumRecords, 
								 const CIPCArchivRecord data[])
{
	CIPCExtraMemory *pBubble = CIPCArchivRecord::BubbleFromArchivRecords(this,iNumRecords,data);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_BACKUP_INFO,(DWORD)iNumRecords);
}
////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::DoConfirmInfo(int iNumRecords, 
								 const CIPCArchivRecord data[])
{
	CIPCExtraMemory *pBubble = CIPCArchivRecord::BubbleFromArchivRecords(this,iNumRecords,data);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_INFO,(DWORD)iNumRecords);
}
////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
void CIPCDatabase::OnConfirmInfo(int /*iNumRecords*/, const CIPCArchivRecord data[])
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmBackupInfo(int /*iNumRecords*/, const CIPCArchivRecord data[])
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestSavePicture(
						int numArchives,
						const BYTE archiveIDs[],
						const CIPCPictureRecord &pictData
						) 
{
	if (pictData.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pictData.GetBubble());

		DWORD dw1,dw2,dw3,dw4;
		// can deal with -1 for numArchives
		ByteArrayToDwords(numArchives, archiveIDs,
						dw1,dw2,dw3,dw4
						);			
		
		WriteCmdWithExtraMemory(pBubble,
			CIPC_DB_REQUEST_SAVE_PICTURE,
			dw1,dw2,dw3,dw4
			);
	}
	else
	{
		OnCommandSendError(CIPC_DB_REQUEST_SAVE_PICTURE);
	}
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestSavePicture(int numArchives,
										const BYTE archiveIDs[],
										const CIPCPictureRecord &data
										)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void	CIPCDatabase::DoRequestSavePictureForHost(
							CSecID	hostID,
							const	CIPCPictureRecord &pictData,
							BOOL	bIsAlarmConnection,
							BOOL	bIsSearchResult
							)
{
	if (pictData.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pictData.GetBubble());

		WriteCmdWithExtraMemory(pBubble,
								CIPC_DB_REQUEST_SAVE_BY_HOST,
								(DWORD) hostID.GetID(),
								(DWORD) bIsAlarmConnection,
								(DWORD) bIsSearchResult);
	}
	else
	{
		OnCommandSendError(CIPC_DB_REQUEST_SAVE_BY_HOST);
	}
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void	CIPCDatabase::OnRequestSavePictureForHost(
							CSecID	hostID,
							const	CIPCPictureRecord &data,
							BOOL	bIsAlarmConnection,
							BOOL	bIsSearchResult
							)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestQuery(DWORD dwUserID, const CString& sQuery)
{
#ifdef _UNICODE
	WriteCmdWithString(sQuery,
					 CIPC_DB_REQUEST_QUERY, 
					 dwUserID);
#else
	CString sTmp(sQuery);
	WriteCmdWithData(sTmp.GetLength(),
					 LPCTSTR(sTmp),
					 CIPC_DB_REQUEST_QUERY, 
					 dwUserID);
#endif
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestQuery(DWORD dwID, const CString& sQuery)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateQueryResult(DWORD dwUserID, 
										 const CIPCPictureRecord &pictData)
{
	if (pictData.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pictData.GetBubble());
		WriteCmdWithExtraMemory(pBubble, CIPC_DB_INDICATE_QUERY_RESULT, 
								dwUserID);
	}
	else
	{
		OnCommandSendError(CIPC_DB_INDICATE_QUERY_RESULT);
	}
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateQueryResult(DWORD dwUserID, 
										 const CIPCPictureRecord &pictData)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoResponseQueryResult(DWORD dwUserID)
{
	WriteCmd(CIPC_DB_RESPONSE_QUERY_RESULT,dwUserID);
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnResponseQueryResult(DWORD dwID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmQuery(DWORD dwUserID)
{
	WriteCmd(CIPC_DB_CONFIRM_QUERY, dwUserID);
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmQuery(DWORD dwID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateNewArchiv(const CIPCArchivRecord& data)
{
	CIPCExtraMemory *pBubble = CIPCArchivRecord::BubbleFromArchivRecords(this,1,&data);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_INDICATE_NEW_ARCHIV);
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateRemoveArchiv(WORD wArchivNr)
{
	WriteCmd(CIPC_DB_INDICATE_REMOVE_ARCHIV,wArchivNr);
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestSequenceList(WORD wArchivNr)
{
	WriteCmd(CIPC_DB_REQUEST_SEQUENCE_LIST,wArchivNr);
}
////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestSequenceList(WORD wArchivNr)
{
	OverrideError();
}

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: DoConfirmSequenceList
 Zweck   : Sendet eine Anfrage an die Datenbank, um eine Liste der vorhandenen Sequenzen
           im einem Archiv zu erhalten.
           Ist iNumRecords > 0, so muß das Feld "data" belegt sein und so groß sein wie
           die angefragte Anzahl der Sequenzen.

 Parameter:  
  wArchivNr  : (E): Nummer des Archivs  (WORD)
  iNumRecords: (E): Anzahl der Sequenzen  (int)
  data[]     : (E): [angefragte Sequenzen]  (const CIPCSequenceRecord)

 Rückgabewert:  (void)
 <TITLE DoConfirmSequenceList>
*********************************************************************************************/
void CIPCDatabase::DoConfirmSequenceList(WORD wArchivNr, int iNumRecords,const CIPCSequenceRecord data[])
{
	if (iNumRecords>0)
	{
		CIPCExtraMemory *pBubble = CIPCSequenceRecord::BubbleFromSequenceRecords(this,iNumRecords,data);
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_SEQUENCE_LIST,
								(DWORD)wArchivNr,(DWORD)iNumRecords);
	}
	else
	{
		WriteCmd(CIPC_DB_CONFIRM_SEQUENCE_LIST,
				(DWORD)wArchivNr,(DWORD)iNumRecords);
	}
} 

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: OnConfirmSequenceList
 Zweck   : Liefert ein Array mit den vorhandenen Sequenzen in einem Archiv.

 Parameter:  
  wArchivNr  : (E): Nummer des Archivs  (WORD)
  iNumRecords: (E): Anzahl der Sequenzen  (int)
  data[]     : (E): Array mit den Sequenzen  (const CIPCSequenceRecord)

 Rückgabewert:  (void)
 <TITLE OnConfirmSequenceList>
*********************************************************************************************/
void CIPCDatabase::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords,const CIPCSequenceRecord data[])
{
	OverrideError();
}

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: DoRequestRecordNr
 Zweck   : Sendet eine Anfrage an die Datenbank, um das Bild und zusätzliche Daten zu
           erhalten.

 Parameter:  
  wArchivNr        : (E): Nummer des Archivs  (WORD)
  wSequenceNr      : (E): Nummer der Sequenz  (WORD)
  dwCurrentRecordNr: (E): Aktuelles Bild (Anfangs 0)  (DWORD)
  dwNewRecordNr    : (E): Nummer angefragten des Bildes  (DWORD)
  dwCamID          : (E): Kamera ID  (DWORD)

 Rückgabewert:  (void)
 <TITLE DoRequestRecordNr>
*********************************************************************************************/
void CIPCDatabase::DoRequestRecordNr(WORD wArchivNr,WORD wSequenceNr, 
									 DWORD dwCurrentRecordNr,DWORD dwNewRecordNr,
									 DWORD dwCamID)
{
	WriteCmd(CIPC_DB_REQUEST_RECORD_NR,
			 MAKELONG(wArchivNr,wSequenceNr),
			 dwCurrentRecordNr,
			 dwNewRecordNr,
			 dwCamID);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestRecordNr(WORD wArchivNr, 
									 WORD wSequenceNr, 
									 DWORD dwCurrentRecordNr,
									 DWORD dwNewRecordNr,
									 DWORD dwCamID)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
									 DWORD dwRecordNr,
									 DWORD dwNrOfRecords,
									 const CIPCPictureRecord &pict,
									 const CIPCFields& fields)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,fields,pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_RECORD_NR,
								MAKELONG(wArchivNr,wSequenceNr),//low,high
								dwRecordNr,
								dwNrOfRecords,
								fields.GetSize());
	}
	else
	{
		OnCommandSendError(CIPC_DB_CONFIRM_RECORD_NR);
	}
}

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: OnConfirmRecordNr
 Zweck   : Liefert ein Bild aus einer Sequenz in einem Archiv und zusätzliche Daten zum Bild

 Parameter:  
  wArchivNr    : (E): Nummer des Archivs  (WORD)
  wSequenceNr  : (E): Nummer der Sequenz  (WORD)
  dwRecordNr   : (E): Nummer des Bildes  (DWORD)
  dwNrOfRecords: (E): Anzahl der Bilder in der Sequenz  (DWORD)
  pict         : (E): JPEG-Daten  (const <LINK CIPCPictureRecord, CIPCPictureRecord&>)
  iNumRecords  : (E): Anzahl der Felder  (int)
  fields[]     : (E): Felder mit Datenbankinformationen (const <LINK CIPCField, CIPCField>)
                      
 Rückgabewert:  Keiner
 <TITLE OnConfirmRecordNr>
*********************************************************************************************/
void CIPCDatabase::OnConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
									 DWORD dwRecordNr,
									 DWORD dwNrOfRecords,
									 const CIPCPictureRecord &pict,
									 int iNumRecords, 
									 const CIPCField fields[])
{
	OverrideError();
}
/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: DoConfirmRecordNr
 Zweck   : Liefert ein Media Sample aus einer Sequenz in einem Archiv und zusätzliche Daten
           zum Ton

 Parameter:  
  wArchivNr    : (E): Nummer des Archivs    (WORD)
  wSequenceNr  : (E): Nummer der Sequenz    (WORD)
  dwRecordNr   : (E): Nummer des Media samples  (DWORD)
  dwNrOfRecords: (E): Anzahl der Elemente in der Sequenz    (DWORD)
  media        : (E): Referenz auf das Media Sample  (const CIPCMediaSampleRecord&)
  fields       : (E): dazugehörige Daten, wie Meldername ...  (const CIPCFields&)

 Rückgabewert:  (void)
*********************************************************************************************/
void CIPCDatabase::DoConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  const CIPCFields& fields)
{
	if (media.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,
															 fields,
															 media.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_RECORD_NR_MEDIA,
								MAKELONG(wArchivNr,wSequenceNr),//low,high
								dwRecordNr,
								dwNrOfRecords,
								media.GetID().GetID());
	}
	else
	{
		OnCommandSendError(CIPC_DB_CONFIRM_RECORD_NR_MEDIA);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	WriteCmd(CIPC_DB_REQUEST_DELETE_SEQUENCE,wArchivNr,wSequenceNr);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	WriteCmd(CIPC_DB_INDICATE_DELETE_SEQUENCE,wArchivNr,wSequenceNr);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestFieldInfo()
{
	WriteCmd(CIPC_DB_REQUEST_FIELD_INFO);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestFieldInfo()
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmFieldInfo(const CIPCFields& fields)
{
	CIPCExtraMemory *pBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_FIELD_INFO,(DWORD)fields.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateNewSequence(const CIPCSequenceRecord& data,
										 WORD  wPrevSequenceNr,
										 DWORD dwNrOfRecords)
{
	CIPCExtraMemory *pBubble = CIPCSequenceRecord::BubbleFromSequenceRecords(this,1,&data);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_INDICATE_NEW_SEQUENCE,wPrevSequenceNr,dwNrOfRecords);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateNewSequence(const CIPCSequenceRecord& data,
										 WORD  wPrevSequenceNr,
										 DWORD dwNrOfRecords)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestNewSavePicture(WORD wArchivNr1,
										   WORD wArchivNr2,
			 							   const CIPCPictureRecord &pict,
										   const CIPCFields& fields)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,
															  fields,
															  pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_NEW_REQUEST_SAVE,
								wArchivNr1,fields.GetSize(),wArchivNr2);
	}
	else
	{
		OnCommandSendError(CIPC_DB_NEW_REQUEST_SAVE);
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestNewSavePicture(WORD wArchivNr1,
										   WORD wArchivNr2,
			 							   const CIPCPictureRecord &pict,
										   int iNumRecords,
										   const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmNewSavePicture(WORD wArchivNr,
											CSecID camID)
{
	WriteCmd(CIPC_DB_NEW_CONFIRM_SAVE,wArchivNr,camID.GetID());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmNewSavePicture(WORD wArchivNr,
											CSecID camID)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   const CIPCFields& fields)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,
															  fields,
															  pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST,
			hostID.GetID(),bIsAlarmConnection,bIsSearchResult,fields.GetSize());
	}
	else
	{
		OnCommandSendError(CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST);
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   int iNumRecords,
												   const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestNewQuery(DWORD dwUserID,
									 int iNumArchives,
									 const WORD archives[],
									 const CIPCFields& fields,
									 DWORD dwMaxQueryResults/*=0*/)
{
	CIPCExtraMemory *pFieldBubble = fields.BubbleFromFields(this);
	if (pFieldBubble)
	{
		DWORD dwLen1 = pFieldBubble->GetLength();
		WORD w = 0;
		DWORD dwSum = dwLen1 + 2*sizeof(DWORD) + iNumArchives * sizeof(WORD);

		CIPCExtraMemory *pBubble = new CIPCExtraMemory();

		if (pBubble->Create(this,dwSum))
		{
			BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();

			for (int i=0;i<iNumArchives;i++)
			{
				w = archives[i];
				*pByte++ = HIBYTE(w);
				*pByte++ = LOBYTE(w);
			}

			*pByte++ = HIBYTE(HIWORD(dwLen1));
			*pByte++ = LOBYTE(HIWORD(dwLen1));
			*pByte++ = HIBYTE(LOWORD(dwLen1));
			*pByte++ = LOBYTE(LOWORD(dwLen1));
			CopyMemory(pByte,pFieldBubble->GetAddress(),dwLen1);

			WK_DELETE(pFieldBubble);

			WriteCmdWithExtraMemory(pBubble,CIPC_DB_REQUEST_NEW_QUERY,
									dwUserID,
									(DWORD)iNumArchives,
									(DWORD)fields.GetSize(),
									dwMaxQueryResults);
		}
		else
		{
			WK_DELETE(pBubble);
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoRequestNewQuery\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestNewQuery(DWORD dwUserID,
									  int numArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoIndicateNewQueryResult(DWORD dwUserID,
										    WORD wArchivNr,
										    WORD wSequenceNr,
										    DWORD dwRecordNr,
										    const CIPCFields& fields)
{
	CIPCExtraMemory *pFieldBubble = fields.BubbleFromFields(this);
	DWORD dw;
	dw = (wArchivNr << 16) | wSequenceNr;
	WriteCmdWithExtraMemory(pFieldBubble,CIPC_DB_INDICATE_NEW_QUERY_RESULT,
							dwUserID,
							dw,
							dwRecordNr,
							(DWORD)fields.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnIndicateNewQueryResult(DWORD dwUserID,
											  WORD wArchivNr,
											  WORD wSequenceNr,
											  DWORD wRecordNr,
											  int iNumFields,
											  const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestDrives()
{
	WriteCmd(CIPC_DB_REQUEST_DRIVES);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestDrives()
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmDrives(const CIPCDrives& drives)
{
	CIPCExtraMemory *pBubble = drives.BubbleFromDrives(this);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_DRIVES,(DWORD)drives.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[])
{
	OverrideError();
}
// 4.0 Backup			
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 const CString& sName,
								 int iNumIDs,
								 const DWORD dwIDs[])
{
	CIPCExtraMemory *pBubble = NULL;
	DWORD dwSum;
	BYTE* pByte = NULL;

#ifdef _UNICODE
	dwSum = sizeof(WORD) + CIPCExtraMemory::CString2Memory(pByte,sDestinationPath, m_wCodePage)
			+ sizeof(WORD) + CIPCExtraMemory::CString2Memory(pByte,sName, m_wCodePage)
			+ iNumIDs * sizeof(DWORD);
#else
	dwSum = sizeof(WORD) + sDestinationPath.GetLength()
			+ sizeof(WORD) + sName.GetLength()
			+ iNumIDs * sizeof(DWORD);
#endif

	pBubble = new CIPCExtraMemory();

	if (pBubble->Create(this,dwSum))
	{
		pByte = (BYTE*)pBubble->GetAddressForWrite();

#ifdef _UNICODE
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath, m_wCodePage);
		CIPCExtraMemory::CString2Memory(pByte,sName, m_wCodePage);
#else
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath);
		CIPCExtraMemory::CString2Memory(pByte,sName);
#endif

		for (int i=0; i<iNumIDs; i++)
		{
			CIPCExtraMemory::DWORD2Memory(pByte,dwIDs[i]);
		}

		DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);

		WriteCmdWithExtraMemory(pBubble,
								CIPC_DB_REQUEST_BACKUP,
								dwUserData,
								iNumIDs);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoRequestBackup\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 const CString& sName,
								 int iNumIDs,
								 const DWORD dwIDs[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID)
{
	CIPCExtraMemory *pBubble = NULL;
	DWORD dwSum;
	BYTE* pByte = NULL;

#ifdef _UNICODE
	dwSum = sizeof(WORD) + CIPCExtraMemory::CString2Memory(pByte,sDestinationPath, m_wCodePage);
#else
	dwSum = sizeof(WORD) + sDestinationPath.GetLength();
#endif

	pBubble = new CIPCExtraMemory();
	
	if (pBubble->Create(this,dwSum))
	{
		pByte = (BYTE*)pBubble->GetAddressForWrite();

#ifdef _UNICODE
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath, m_wCodePage);
#else
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath);
#endif

		DWORD dwDelta = pByte - (BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);

		WriteCmdWithExtraMemory(pBubble,
								CIPC_DB_CONFIRM_BACKUP,
								dwUserData,
								dwID);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmBackup\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestCancelBackup(DWORD dwUserData)
{
	WriteCmd(CIPC_DB_REQUEST_CANCEL_BACKUP,dwUserData);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestCancelBackup(DWORD dwUserData)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmCancelBackup(DWORD dwUserData)
{
	WriteCmd(CIPC_DB_CONFIRM_CANCEL_BACKUP,dwUserData);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmCancelBackup(DWORD dwUserData)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestDeleteArchiv(WORD wArchivNr)
{
	WriteCmd(CIPC_DB_REQUEST_DELETE_ARCHIV,wArchivNr);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestDeleteArchiv(WORD wArchivNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestCameraNames(WORD wArchivNr, DWORD dwUserData)
{
	WriteCmd(CIPC_DB_REQUEST_CAMERA_NAMES,wArchivNr,dwUserData);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestCameraNames(WORD wArchivNr,DWORD dwUserData)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmCameraNames(WORD wArchivNr,DWORD dwUserData,
								  const CIPCFields& fields)
{
	CIPCExtraMemory *pBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pBubble,
							CIPC_DB_CONFIRM_CAMERA_NAMES,
							wArchivNr,
							dwUserData,
							(DWORD)fields.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmCameraNames(WORD wArchivNr,DWORD dwUserData,
								  int iNumFields,
								  const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestBackupByTime(WORD wUserData,
									     const CString& sDestinationPath,
									     const CString& sName,
									     int iNumIDs,
									     const DWORD dwIDs[],
									     const CSystemTime& start,
									     const CSystemTime& end,
									     WORD  wFlags,
									     DWORD dwMaximumInMB/*=0*/)
{
	CIPCExtraMemory *pBubble = NULL;
	DWORD dwSum;
	BYTE* pByte = NULL;

#ifdef _UNICODE
	dwSum = sizeof(WORD) + CIPCExtraMemory::CString2Memory(pByte, sDestinationPath, m_wCodePage)
			+ sizeof(WORD) + CIPCExtraMemory::CString2Memory(pByte, sName, m_wCodePage)
			+ iNumIDs * sizeof(DWORD)
			+ CSystemTime::GetBubbleLength()
			+ CSystemTime::GetBubbleLength();
#else
	dwSum = sizeof(WORD) + sDestinationPath.GetLength()
			+ sizeof(WORD) + sName.GetLength()
			+ iNumIDs * sizeof(DWORD)
			+ CSystemTime::GetBubbleLength()
			+ CSystemTime::GetBubbleLength();
#endif

	pBubble = new CIPCExtraMemory();

	if (pBubble->Create(this,dwSum))
	{
		pByte = (BYTE*)pBubble->GetAddressForWrite();

#ifdef _UNICODE
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath, m_wCodePage);
		CIPCExtraMemory::CString2Memory(pByte,sName, m_wCodePage);
#else
		CIPCExtraMemory::CString2Memory(pByte,sDestinationPath);
		CIPCExtraMemory::CString2Memory(pByte,sName);
#endif

		for (int i=0; i<iNumIDs; i++)
		{
			CIPCExtraMemory::DWORD2Memory(pByte,dwIDs[i]);
		}

		start.WriteIntoBubble(pByte);
		end.WriteIntoBubble(pByte);

		DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);

		WriteCmdWithExtraMemory(pBubble,
								CIPC_DB_REQUEST_BACKUP_BY_TIME,
								wUserData,
								wFlags,
								iNumIDs,
								dwMaximumInMB);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoRequestBackup\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestBackupByTime(WORD  wUserData,
									   const CString& sDestinationPath,
									   const CString& sName,
									   int iNumIDs,
									   const DWORD dwIDs[],
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD  wFlags,
									   DWORD dwMaximumInMB)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmBackupByTime(WORD wUserData,
									     DWORD dwID,
									     const CSystemTime& start,
									     const CSystemTime& end,
									     WORD wFlags,
									     CIPCInt64 i64Size)
{
	DWORD dwParam1 = MAKELONG(wUserData,wFlags);
	DWORD dwParam2 = dwID;
	ULARGE_INTEGER i;
	i.QuadPart = i64Size.GetInt64();
	DWORD dwParam3 = i.LowPart;
	DWORD dwParam4 = i.HighPart;
	DWORD dwSum = 2 * CSystemTime::GetBubbleLength();
	CIPCExtraMemory *pBubble = NULL;
	BYTE* pByte;

	pBubble = new CIPCExtraMemory();

	if (pBubble->Create(this,dwSum))
	{
		pByte = (BYTE*)pBubble->GetAddressForWrite();
		start.WriteIntoBubble(pByte);
		end.WriteIntoBubble(pByte);

		DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);

		WriteCmdWithExtraMemory(pBubble,
								CIPC_DB_CONFIRM_BACKUP_BY_TIME,
								dwParam1,
								dwParam2,
								dwParam3,
								dwParam4);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmBackupByTime\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmBackupByTime(WORD wUserData,
									     DWORD dwID,
									     const CSystemTime& start,
									     const CSystemTime& end,
									     WORD wFlags,
									     CIPCInt64 i64Size)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestAlarmListArchives()
{
	WriteCmd(CIPC_DB_REQUEST_ALARM_LIST_ARCHIVES);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestAlarmListArchives()
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[])
{
	CIPCExtraMemory *pBubble = CIPCArchivRecord::BubbleFromArchivRecords(this,iNumRecords,data);
	WriteCmdWithExtraMemory(pBubble,CIPC_DB_CONFIRM_ALARM_LIST_ARCHIVES,(DWORD)iNumRecords);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr)
{
	WriteCmd(CIPC_DB_REQUEST_ALARM_LIST_RECORD,wArchivNr,wSequenceNr,dwRecordNr);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmAlarmListRecord(WORD wArchivNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr,
											 DWORD dwNrOfRecords,
											 const CIPCFields& fields)
{
	CIPCExtraMemory *pBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pBubble,
							CIPC_DB_CONFIRM_ALARM_LIST_RECORD,
							MAKELONG(wArchivNr,wSequenceNr),
							dwRecordNr,
							dwNrOfRecords,
							(DWORD)fields.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnConfirmAlarmListRecord(WORD wArchivNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr,
											 DWORD dwNrOfRecords,
											 int iNumFields,
											 const CIPCField fields[])
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoRequestSaveData(WORD wArchivNr,
								      const CIPCFields& fields)
{
	CIPCExtraMemory *pBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pBubble,
							CIPC_DB_REQUEST_SAVE_DATA,
							wArchivNr,
							(DWORD)fields.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::OnRequestSaveData(WORD wArchivNr,int iNumRecords,const CIPCField fields[])
{
	OverrideError();
}

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: DoRequestRecords
 Zweck   : Sendet eine Anfrage an die Datenbank, um die Bildspezifischen Datensätze in einer
           Sequenz eines Archivs zu erhalten.
           Die Anfrage wird in der virtuellen Funktion "OnConfirmRecords" beantwortet.

 Parameter:  
  wArchivNr      : (E): Nummer des Archivs  (WORD )
  wSequenceNr    : (E): Nummer der Sequenz  (WORD )
  dwFirstRecordNr: (E): erster Datensatz (1=Anfang, 0=alle)  (DWORD)
  dwLastRecordNr : (E): letzter Datensatz (0=alle)  (DWORD)
  
 Rückgabewert:  (void)
 <TITLE DoRequestRecords>
*********************************************************************************************/
void CIPCDatabase::DoRequestRecords(WORD  wArchivNr, WORD  wSequenceNr, DWORD dwFirstRecordNr, DWORD dwLastRecordNr)
{
	WriteCmd(CIPC_DB_REQUEST_RECORDS,
			 MAKELONG(wArchivNr,wSequenceNr),
			 dwFirstRecordNr,
			 dwLastRecordNr);
}

/*********************************************************************************************
 Klasse  : CIPCDatabase
 Funktion: OnRequestRecords
 Zweck   : Liefert die Bildspezifischen Datensätze in einer Sequenz eines Archivs.

 Parameter:  
  wArchivNr      : (E): Nummer des Archivs  (WORD )
  wSequenceNr    : (E): Nummer der Sequenz  (WORD )
  dwFirstRecordNr: (E): erster Datensatz (1=Anfang, 0=alle)  (DWORD)
  dwLastRecordNr : (E): letzter Datensatz (0=alle)  (DWORD)

 Rückgabewert:  (void)
 <TITLE OnRequestRecords>
*********************************************************************************************/
void CIPCDatabase::OnRequestRecords(WORD  wArchivNr, WORD  wSequenceNr, DWORD dwFirstRecordNr, DWORD dwLastRecordNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
NOT YET DOCUMENTED
*/
void CIPCDatabase::DoConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 const CIPCFields& fields,
									 const CIPCFields& records)
{
	CIPCFields flds;

	for (int i=0;i<fields.GetSize();i++)
	{
		flds.Add(new CIPCField(*fields[i]));
	}
	for (i=0;i<records.GetSize();i++)
	{
		flds.Add(new CIPCField(*records[i]));
	}
	CIPCExtraMemory *pBubble = flds.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pBubble,
							CIPC_DB_CONFIRM_RECORDS,
							MAKELONG(wArchivNr,wSequenceNr),
							(DWORD)fields.GetSize(),
							(DWORD)records.GetSize()
							);
}
/////////////////////////////////////////////////////////////////////////////
/* 
Function: 
Override this method to get confirms for <mf CIPCDatabase.DoRequestRecords>.
Every element of fields holds one database definition field,the name ist the
name of the field, the value is empty, the length ist the length of the
database field, the type is the type.
Every element of records holds one record of the sequence,
the name is the number, the value contains all values of the fields
concatted, the type is 'R'.
param: WORD | wArchivNr | the archive nr
param: WORD | wSequenceNr | the sequence nr
param: CIPCFields | fields | the sequence database definition
param: CIPCFields | records | the records
 <c CIPCFields>
*/
void CIPCDatabase::OnConfirmRecords(WORD  wArchivNr, 
									WORD  wSequenceNr, 
									CIPCFields fields,
									CIPCFields records)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::DoRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	WriteCmd(CIPC_DB_REQUEST_OPEN_SEQUENCE,wArchiveNr,wSequenceNr);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::OnRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::DoRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	WriteCmd(CIPC_DB_REQUEST_CLOSE_SEQUENCE,wArchiveNr,wSequenceNr);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::OnRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::DoRequestSaveMedia(WORD wArchiveNr,
									   CSecID hostID,
									   BOOL	bIsAlarmConnection,
									   BOOL	bIsSearchResult,
			 						   const CIPCMediaSampleRecord &media,
									   const CIPCFields& fields)
{
	if (media.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,
															  fields,
															  media.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_REQUEST_SAVE_MEDIA,
								MAKELONG(wArchiveNr,MAKEWORD(bIsAlarmConnection,bIsSearchResult)),
								fields.GetSize(),
								hostID.GetID(),
								media.GetID().GetID());
	}
	else
	{
		OnCommandSendError(CIPC_DB_REQUEST_SAVE_MEDIA);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCDatabase::OnRequestSaveMedia(	WORD wArchiveNr,
										CSecID hostID,
										BOOL	bIsAlarmConnection,
										BOOL	bIsSearchResult,
			 							const CIPCMediaSampleRecord &media,
										int iNumRecords,
										const CIPCField fields[])
{
	OverrideError();
}
