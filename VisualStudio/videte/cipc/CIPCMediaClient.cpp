// CIPCMediaClient.cpp: implementation of the CIPCMediaClient class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "CIPCMediaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCMediaClient::CIPCMediaClient(LPCTSTR shmName, BOOL bAsMaster):
                 CIPCMedia(shmName, bAsMaster)
{
	m_Array.SetAutoDelete(true);
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCMediaClient::~CIPCMediaClient()
{

}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnConfirmConnection()
{
	DoRequestInfoMedia(SECID_NO_GROUPID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records)
{
	int i,j;
	CIPCMediaRecord* pRec;

	for (i=0;i<iNumRecords;i++) 
	{
		pRec = GetMediaRecordPtrFromSecID(records.GetAtFast(i)->GetID());
		if (pRec==NULL) 
		{
			// add new record
			pRec = new CIPCMediaRecord(*records.GetAtFast(i));
			m_Array.Add(pRec);
			OnAddRecord(*pRec);

		} 
		else 
		{
			// update already existing record
			if (*pRec != *records.GetAtFast(i))
			{
				*pRec = *records.GetAtFast(i);
				OnUpdateRecord(*pRec);
			}
		}
	}

	BOOL bFound = FALSE;
	// second browse for deleted records, 
	// do it reverse to make sure i is valid index
	for (i=m_Array.GetSize()-1;i>=0;i--)
	{
		pRec = m_Array.GetAtFast(i);
		bFound = FALSE;
		if ( (wGroupID == pRec->GetID().GetGroupID()) || (wGroupID == SECID_NO_GROUPID))
		{
			// question Is pRec still in records[] ?
			for (j=0;j<iNumRecords;j++)
			{
				if (pRec->GetID() == records.GetAtFast(j)->GetID())
				{
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				// pRec is not in records so delete it from our array
				// give derived classes a chance
				OnDeleteRecord(*pRec);
				m_Array.RemoveAt(i);
				WK_DELETE(pRec);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnAddRecord(const CIPCMediaRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnUpdateRecord(const CIPCMediaRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCMediaClient::OnDeleteRecord(const CIPCMediaRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : *CIPCMediaClient
 Function   : GetMediaRecordPtrFromSecID
 Description: Returns the MediaRecord from this SecID

 Parameters:   
  id: (E): Identifier of this record  (CSecID)

 Result type: MediaRecord pointer (CIPCMediaRecord*)
 created: September, 29 2003
 <TITLE GetMediaRecordPtrFromSecID>
*********************************************************************************************/
CIPCMediaRecord *CIPCMediaClient::GetMediaRecordPtrFromSecID(CSecID id) const
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		CIPCMediaRecord* pRec;
		pRec = (CIPCMediaRecord*)m_Array.GetAtFast(i);
		if (pRec->GetID()==id)
			return pRec;
	}

	return NULL;
}
/*********************************************************************************************
 Class      : *CIPCMediaClient
 Function   : GetMediaRecordPtrFromSecID
 Description: Returns the MediaRecord from this SecID

 Parameters:   
  id: (E): Identifier of this record  (CSecID)

 Result type: MediaRecord (CIPCMediaRecord&)
 created: September, 29 2003
 <TITLE GetMediaRecordPtrFromSecID>
*********************************************************************************************/
const CIPCMediaRecord& CIPCMediaClient::GetMediaRecordFromSecID(CSecID id) const
{
	CIPCMediaRecord* pRec;
	
	pRec = (CIPCMediaRecord*)GetMediaRecordPtrFromSecID(id);
	if (pRec==NULL) 
	{
		WK_TRACE_ERROR(_T("MediaRecord not found, id was %08x\n"),id.GetID());
	}

	return *pRec;
}
/*********************************************************************************************
 Class      : CIPCMediaClient
 Function   : GetMediaRecordFromIndex
 Description: Return the MediaRecor with the index i

 Parameters:   
  i: (E): Index of the MediaRecord  (int)

 Result type: MediaRecord (const CIPCMediaRecord&)
 created: September, 29 2003
 <TITLE GetMediaRecordFromIndex>
*********************************************************************************************/
const CIPCMediaRecord& CIPCMediaClient::GetMediaRecordFromIndex(int i) const
{
	CIPCMediaRecord* pRec = NULL;
	if (i<m_Array.GetSize())
	{
		pRec = m_Array.GetAtFast(i);
	}
	else 
	{
		WK_TRACE_ERROR(_T("CIPCMediaClient: range error %d not in 0..%d\n"),i,m_Array.GetSize());
	}

	return *pRec;
}
