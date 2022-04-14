/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: ipcinputsample.cpp
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's inputs (alarm detectors); implementation file.
******************************************************************************/

#include "StdAfx.h"
#include "ipcinputsample.h"
#include "SampleDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCInputSample::CIPCInputSample(CSampleDlg* pSampleDlg, 
								 LPCTSTR szShmName, 
								 WORD wCodePage)
: CIPCInputClient(szShmName, FALSE)
{
	m_pSampleDlg	= pSampleDlg;

	//start the CIPCInputSample thread. this thread communicates with the input
	//of the connected server 
	StartThread();
}
CIPCInputSample::~CIPCInputSample(void)
{
	m_paInputRecords.DeleteAll();
	StopThread();
}

/*********************************************************************************************
Description   : This function has to be overwritten.
				Override this function to get each input record (alarm detectors) separately.
Parameters:   
&record	: (I): Reference to the input record (alarm detector).	(const CIPCInputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCInputSample::OnAddRecord(const CIPCInputRecord & record)
{
	//insert some code here if you want to handle each alarm detector separately
}

/*********************************************************************************************
Description   : This function has to be overwritten.
				Override this function to get each status change 
				of one input record (alarm detectors) separately.
Parameters:   
&record	: (I): Reference to the input record (alarm detector) whose 
			   status has changed.									(const CIPCInputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCInputSample::OnUpdateRecord(const CIPCInputRecord & record)
{
	//insert some code here if you want to handle each status change 
	//of an alarm detector separately
}

/*********************************************************************************************
Description   : Override this function to get the confirmation of the input connection.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCInputSample::OnConfirmConnection()
{
	//request alarm detectors of the connected server
	//answer will be received through function "OnConfirmInfoInputs(..)"
	DoRequestInfoInputs(SECID_NO_GROUPID);	// SECID_NO_GROUPID = all groups
}

/*********************************************************************************************
Description   : Override this function to get all input records (alarm detectors).

Parameters:   
wGroupID	: (I): Indicates the group (for internal use).			(WORD)
iNumGroups	: (I): Number of input groups (for internal use).		(int) 
numRecords  : (I): Numbers of all input records (alarm detectors) 
				   of the connected server. All input records means
				   all enabled and all disabled detectors.			(int)
records[]	: (I): Array of all input records (alarm detectors).	(const CIPCInputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCInputSample::OnConfirmInfoInputs(WORD wGroupID, 
										  int iNumGroups, 
										  int numRecords, 
										  const CIPCInputRecord records[])
{
	//empty the inputrecord array
	m_paInputRecords.DeleteAll();

	for (int i=0;i<numRecords;i++)
	{
		CIPCInputRecord rec = records[i];

		if (rec.GetIsEnabled()) 
		{
			//if connected server is an idip, detector is enabled in system management
			//if server is a DTS, all detectors are enabled
			m_paInputRecords.Add(new CIPCInputRecord(records[i]));
		}
		if (rec.GetIsActive()) //detector is enabled, something was detected
		{		
			TRACE(_T(" IsActive: %s\n"), rec.GetName());
		}
	}

	if(m_pSampleDlg)
	{
		//fill alarm list control with all enabled alarms of the server
		m_pSampleDlg->FillListCtrl_Detectors(m_paInputRecords);
	}

	//only when the base class function is called, the virtual function OnAddRecord(...)
	//will be called from the base class.
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,numRecords,records);
}

/*********************************************************************************************
Description   : Override this function to get the new alarm state of
				an input record (alarm detector).

Parameters:   
wGroupID	: (I): Indicates the group 
				   (e.g. SECID_NO_GROUPID, see declaration).			(WORD)
inputMask	: (I): Indicates the new alarm state.	
				   1 high, 0 low										(DWORD)
changeMask  : (I): Indicates the input whose alarm state has changed.
				   1 changed, 0 unchanged								(DWORD)
sInfoString	: (I): InfoString (for internal use).						(LPCTSTR)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCInputSample::OnIndicateAlarmState(WORD wGroupID,
										   DWORD inputMask,
										   DWORD changeMask,
										   LPCTSTR sInfoString)
{
	BOOL bFlagSignaled = FALSE;

	for (int nI = 0; nI < MAX_NUMBER_OF_INPUTS; nI++)
	{
		//find the bit in the change mask that has changed
		if (TSTBIT(changeMask, (WORD)nI))
		{
			//get the ID of the alarm
			CSecID alarmID(wGroupID, (WORD)nI);

			//check the new alarm state
			if (TSTBIT(inputMask, nI))
			{
				TRACE(_T("Alarm On Input %hx,%d\n"), alarmID,nI);
				bFlagSignaled = TRUE;
			}
			else
			{
				TRACE(_T("Alarm Off Input %d\n"), nI);
				bFlagSignaled = FALSE;
			}

			//set the new alarm state of all input records (alarm detectors)
			for(int i = 0; i < m_paInputRecords.GetSize(); i++)
			{
				DWORD dwID = m_paInputRecords.GetAtFast(i)->GetID().GetID();
				if(dwID == alarmID.GetID())
				{
					WORD wFlags = m_paInputRecords.GetAtFast(i)->GetFlags();
					bFlagSignaled ? (wFlags = wFlags | IREC_STATE_ALARM) : (wFlags = wFlags &~ IREC_STATE_ALARM);
					m_paInputRecords.GetAtFast(i)->SetFlags((BYTE)wFlags);
				}
			}
		}
	}

	if(m_pSampleDlg)
	{	
		//fill the alarm detector list control on the main dialog
		m_pSampleDlg->FillListCtrl_Detectors(m_paInputRecords);
	}

	//only when the base class function is called, the virtual function OnUpdateRecord(...)
	//will be called from the base class.
	CIPCInputClient::OnIndicateAlarmState(wGroupID, inputMask, changeMask, sInfoString);
}