/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputClient.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutputClient.cpp $
// CHECKIN:		$Date: 31.03.05 9:18 $
// VERSION:		$Revision: 33 $
// LAST CHANGE:	$Modtime: 31.03.05 8:48 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcOutputClient.h"
#include "CipcOutputRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
{CIPCOutputClient Overview|Overview,CIPCOutputClient}
 {members|CIPCOutputClient},
{CIPCInputClient} 
*/
/*
  {CIPCInput}
*/

/*Function: NOT YET DOCUMENTED */
//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputClient::CIPCOutputClient(LPCTSTR shmName, BOOL asMaster)
	: CIPCOutput(shmName, asMaster)
{
	m_pLastRecord = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
CIPCOutputClient::~CIPCOutputClient()
{
	m_Array.DeleteAll();
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCOutputClient::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCOutputClient::OnConfirmConnection()
{
	DoRequestInfoOutputs(SECID_NO_GROUPID);	// all groups
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCOutputClient::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
	int i,j;
	CIPCOutputRecord* pRec;

	// first update already existing records
	for (i=0;i<numRecords;i++) 
	{
		pRec = GetOutputRecordPtrFromSecID(records[i].GetID());
		if (pRec==NULL) 
		{
			// add new record
			if (records[i].IsEnabled())
			{
				pRec = new CIPCOutputRecord(records[i]);
				m_Array.Add(pRec);
				OnAddRecord(*pRec);
			}
		} 
		else 
		{
			// update already existing record
			if (*pRec!=records[i])
			{
				*pRec=records[i];
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
			for (j=0;j<numRecords;j++)
			{
				if (pRec->GetID() == records[j].GetID())
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
				m_pLastRecord = NULL;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
CIPCOutputRecord* CIPCOutputClient::GetOutputRecordPtrFromSecID(CSecID id)
{
	CIPCOutputRecord* pRec=NULL;

	// mini cache
	if (m_pLastRecord && m_pLastRecord->GetID()==id) 
	{
		return m_pLastRecord;
	}
	
	for (int i=0;i<m_Array.GetSize();i++)
	{
		pRec = m_Array.GetAtFast(i);
		if (pRec->GetID()==id) 
		{
			m_pLastRecord = pRec;
			return pRec;
		}
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
const CIPCOutputRecord& CIPCOutputClient::GetOutputRecordFromSecID(CSecID id)
{
	const CIPCOutputRecord* pRec=GetOutputRecordPtrFromSecID(id);
	if (pRec==NULL)
	{
		TRACE(_T("OutputRecord %x not found\n"), id.GetID());
	}
	return *pRec;
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
const CIPCOutputRecord& CIPCOutputClient::GetOutputRecordFromIndex(int i)
{
	if (i<m_Array.GetSize()) 
	{
		return *m_Array.GetAtFast(i);
	} 
	else 
	{
		CIPCOutputRecord* pRec = NULL;
		WK_TRACE_ERROR(_T("CIPCOutputClient index error %d not in 0..%d\n"),
			i,m_Array.GetSize());
		return *pRec;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCOutputClient::OnConfirmHardware(WORD wGroupID, 
									int iErrorCode,
									BOOL /*bCanSWCompress*/,
									BOOL /*bCanSWDecompress*/,
									BOOL /*bCanColorCompress*/,
									BOOL /*bCanColorDecompress*/,
									BOOL /*bCanOverlay*/
									)
{
	if (iErrorCode!=0) 
	{
		// set error flags in all related records
		CIPCOutputRecord* pRec;
		for (int i=0;i<m_Array.GetSize();i++) 
		{
			pRec = m_Array.GetAtFast(i);
			if (pRec->GetID().GetGroupID()==wGroupID) 
			{
				pRec->SetFlags((WORD)(pRec->GetFlags() & ~OREC_IS_OKAY));	// error
				OnUpdateRecord(*pRec);
			}
		}
	} 
	else
	{
		DoRequestInfoOutputs(wGroupID);
		// OOPS inital connection does the same for all groups
	}
}
/*Function: NOT YET DOCUMENTED */
void CIPCOutputClient::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	CIPCOutputRecord *pRec = GetOutputRecordPtrFromSecID(relayID);
	if (pRec && pRec->IsRelay()) {
			if (bClosed)
				pRec->SetFlags((WORD)(pRec->GetFlags()|OREC_RELAY_CLOSED));
			else
				pRec->SetFlags((WORD)(pRec->GetFlags() & ~OREC_RELAY_CLOSED));
			OnUpdateRecord(*pRec);
	} 
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputClient::OnAddRecord(const CIPCOutputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputClient::OnUpdateRecord(const CIPCOutputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputClient::OnDeleteRecord(const CIPCOutputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
