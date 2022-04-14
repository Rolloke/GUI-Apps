/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputClient.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcInputClient.cpp $
// CHECKIN:		$Date: 31.03.05 9:18 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 31.03.05 8:48 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcInputClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*{CIPCInputClient Overview|Overview,CIPCInputClient}
 {members|CIPCInputClient},
{CIPCOutputClient} 
*/
/*
  {CIPCInput}
*/

/*Function: NOT YET DOCUMENTED */
//////////////////////////////////////////////////////////////////////////////////////
CIPCInputClient::CIPCInputClient(LPCTSTR shmName, BOOL asMaster)
	: CIPCInput(shmName, asMaster)
{
}

//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
CIPCInputClient::~CIPCInputClient()
{
	m_Array.DeleteAll();
}
/*{int CIPCInputClient::GetNumberOfInputs() const}
*/
//////////////////////////////////////////////////////////////////////////////////////

/*Function: NOT YET DOCUMENTED */
CIPCInputRecord *CIPCInputClient::GetInputRecordPtrFromSecID(CSecID id) const
{

	for (int i=0;i<m_Array.GetSize();i++)
	{
		CIPCInputRecord* pRec = m_Array.GetAtFast(i);
		if (pRec->GetID()==id)
			return pRec;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
const CIPCInputRecord& CIPCInputClient::GetInputRecordFromSecID(CSecID id) const
{
	CIPCInputRecord* pRec;
	
	pRec = (CIPCInputRecord*)GetInputRecordPtrFromSecID(id);
	if (pRec==NULL) 
	{
		WK_TRACE_ERROR(_T("InputRecord not found, id was %08x\n"),id.GetID());
	}

	return *pRec;
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
const CIPCInputRecord& CIPCInputClient::GetInputRecordFromIndex(int i) const
{
	CIPCInputRecord* pRec = NULL;
	if (i<m_Array.GetSize())
	{
		pRec = m_Array.GetAtFast(i);
	}
	else 
	{
		WK_TRACE_ERROR(_T("CIPCInputClient: range error %d not in 0..%d\n"),i,m_Array.GetSize());
	}

	return *pRec;
}

//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnReadChannelFound()
{
// GF Sollte eigentlich laut CIPC-Theori nur beim Master aufgerufen werden,
// dann funzt der Verbindungsaufbau aber nicht mehr
//	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnConfirmConnection()
{
	DoRequestInfoInputs(SECID_NO_GROUPID);
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	int i,j;
	CIPCInputRecord* pRec;

	for (i=0;i<numRecords;i++) 
	{
		pRec = GetInputRecordPtrFromSecID(records[i].GetID());
		if (pRec==NULL) 
		{
			// add new record
			pRec = new CIPCInputRecord(records[i]);
			m_Array.Add(pRec);
			OnAddRecord(*pRec);

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
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	if (iErrorCode!=0) 
	{
		CIPCInputRecord* pRec;
		for (int i=0;i<m_Array.GetSize();i++) 
		{
			pRec = m_Array.GetAtFast(i);
			if (pRec->GetID().GetGroupID()==wGroupID) 
			{
				pRec->SetFlags((BYTE)(pRec->GetFlags() & ~IREC_STATE_OKAY));	// error
				OnUpdateRecord(*pRec);
			}
		}
	} else 
	{
		DoRequestInfoInputs(wGroupID);
		// OOPS inital connection does the same for all groups
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputClient::OnIndicateAlarmNr(CSecID id,
									    BOOL bAlarm,
									    DWORD dwData1,
									    DWORD dwData2)
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		CIPCInputRecord* pRec = m_Array.GetAtFast(i);
		if (pRec->GetID() == id)
		{
			if (bAlarm)
				pRec->SetFlags((BYTE)(pRec->GetFlags()|IREC_STATE_ALARM));
			else
				pRec->SetFlags((BYTE)(pRec->GetFlags() & ~IREC_STATE_ALARM));
			OnUpdateRecord(*pRec);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
											LPCTSTR szInfoString)	// 1 high, 0 low; 1 changed, 0 unchanged
{
	DWORD dwOneBit=1;
	WORD  i,j,d;

	for (i=0;i<32;i++, dwOneBit <<= 1) 
	{
		if (changeMask & dwOneBit)
		{	
			CIPCInputRecord* pRec;
			CSecID tmpID(wGroupID,i);
			
			d = (WORD)m_Array.GetSize();

			for (j=0;j<d;j++)
			{
				pRec = m_Array.GetAtFast(j);
				if (pRec->GetID()==tmpID)
				{
					if (inputMask & dwOneBit)
						pRec->SetFlags((BYTE)(pRec->GetFlags()|IREC_STATE_ALARM));
					else
						pRec->SetFlags((BYTE)(pRec->GetFlags() & ~IREC_STATE_ALARM));
					OnUpdateRecord(*pRec);
					break;
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED */
void CIPCInputClient::OnConfirmAlarmState(WORD wGroupID,DWORD inputMask)
{
	DWORD dwOneBit=1;

	for (WORD i=0;i<32; i++, dwOneBit <<= 1) 
	{
		CIPCInputRecord* pRec;
		CSecID tmpID(wGroupID,i);
		
		for (WORD j=0;j<m_Array.GetSize();j++)
		{
			pRec = m_Array.GetAtFast(j);
			if (pRec->GetID()==tmpID)
			{
				if (inputMask & dwOneBit)
					pRec->SetFlags((BYTE)(pRec->GetFlags()|IREC_STATE_ALARM));
				else
					pRec->SetFlags((BYTE)(pRec->GetFlags() & ~IREC_STATE_ALARM));
				OnUpdateRecord(*pRec);
				break;	// this id is done
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputClient::OnAddRecord(const CIPCInputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputClient::OnUpdateRecord(const CIPCInputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputClient::OnDeleteRecord(const CIPCInputRecord& pRec)
{
	WK_TRACE_ERROR(_T("%s not overridden\n"),_T(__FUNCTION__));
	ASSERT(0);
}
