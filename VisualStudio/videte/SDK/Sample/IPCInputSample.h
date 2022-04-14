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
|* PROGRAM: IPCInputSample.cpp
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's inputs (detectors).
******************************************************************************/

#pragma once
#include "cipcinputclient.h"

#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define MAX_NUMBER_OF_INPUTS 32

class CIPCInputRecord;
class CSampleDlg;

class CIPCInputSample :
	public CIPCInputClient
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CIPCInputSample(CSampleDlg* pSampleDlg, LPCTSTR szShmName, WORD wCodePage);
	~CIPCInputSample(void);

//////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////
protected:

	virtual void			OnAddRecord(const CIPCInputRecord& pRec);
	virtual void			OnUpdateRecord(const CIPCInputRecord& pRec);
	virtual void			OnConfirmConnection();
	virtual void			OnConfirmInfoInputs(WORD wGroupID, 
												int iNumGroups, 
												int numRecords, 
												const CIPCInputRecord records[]);

	virtual void			OnIndicateAlarmState(WORD wGroupID,
												DWORD inputMask, 
												DWORD changeMask,	
												LPCTSTR sInfoString);

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
private:
	CSampleDlg*				m_pSampleDlg;				//pointer to the main dialog
	CIPCInputRecordArray	m_paInputRecords;			//pointerarray of all
														//input records (alarm detectors)
};
//////////////////////////////////////////////////////////////////////
