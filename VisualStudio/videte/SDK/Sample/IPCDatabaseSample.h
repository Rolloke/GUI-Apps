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
|* PROGRAM: IPCDatabaseSample.h
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's databases.
******************************************************************************/

#pragma once
#include "cipcdatabaseclient.h"

class CPlayPicture;

class CIPCDatabaseSample :
	public CIPCDatabaseClient
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CIPCDatabaseSample(CPlayPicture& PlayPicture, LPCTSTR shmName, WORD wCodePage);
	virtual ~CIPCDatabaseSample(void);

//////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////
protected:

	virtual void			OnConfirmConnection();
	virtual void			OnConfirmInfo(int iNumRecords, 
										  const CIPCArchivRecord data[]);

	virtual	void			OnConfirmSequenceList(WORD wArchivNr, 
												  int iNumRecords, 
												  const CIPCSequenceRecord data[]);

	virtual	void			OnConfirmRecordNr(WORD wArchivNr, 
											  WORD wSequenceNr, 
											  DWORD dwRecordNr,
											  DWORD dwNrOfRecords,
											  const CIPCPictureRecord &pict,
											  int iNumFields,
											  const CIPCField fields[]);

//////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////
public:

	inline CIPCSequenceRecords& GetSequences();

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
private:

	CPlayPicture*			m_pPlayPicture; //pointer to the class which 
											//controls the replay of database pictures
	CIPCSequenceRecords		m_Sequences;	//pointer array which holds 
											//all sequences of one archive
};
//////////////////////////////////////////////////////////////////////

/*********************************************************************************************
Description   : Returns the pointer array of all sequences of one archive.

Parameters:   none

Return Value:  pointer array of sequences. (CIPCSequenceRecords&)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
inline CIPCSequenceRecords& CIPCDatabaseSample::GetSequences()
{
	return m_Sequences;
}
