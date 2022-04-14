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
|* PROGRAM: IPCDatabaseSample.cpp
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's databases; implementation file.
******************************************************************************/

#include "StdAfx.h"
#include "Sample.h"
#include "ipcDatabaseSample.h"
#include "PlayPicture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDatabaseSample::CIPCDatabaseSample(CPlayPicture& PlayPicture, 
									   LPCTSTR shmName, 
									   WORD wCodePage)
: CIPCDatabaseClient(shmName)
{
	m_pPlayPicture	= &PlayPicture;

	//start the CIPCDatabaseSample thread. this thread communicates with the database
	//of the connected server 
	StartThread();
}
CIPCDatabaseSample::~CIPCDatabaseSample(void)
{
	StopThread();
	m_Sequences.DeleteAll();
}

/*********************************************************************************************
Description   : Override this function to get the confirmation of the database connection.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCDatabaseSample::OnConfirmConnection()
{
	//baseclass has to be informed that connection was established
	CIPCDatabaseClient::OnConfirmConnection(); 
}

/*********************************************************************************************
Description   : Override this function to get all archives from the database.

Parameters:   
iNumRecords	: (I): Numbers of archives in the data[] array.			(int)
data[]		: (I): Array of all archive records from the database.	(const CIPCArchivRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCDatabaseSample::OnConfirmInfo(int iNumRecords, 
									   const CIPCArchivRecord data[])
{
	//baseclass has to be informed that the info was confirmed
	CIPCDatabaseClient::OnConfirmInfo(iNumRecords,data);

	//send the last command, in this case "CIPC_DB_CONFIRM_INFO" to the PlayPicture control
	//to fill the archive list control on main dialog
	m_pPlayPicture->PostMessage(WM_USER,GetLastCmd());
}

/*********************************************************************************************
Description   : Override this function to get all sequences from one archive.

Parameters:   
wArchiveNr	: (I): The number of the archive which contains the sequences.	(WORD)
iNumRecords	: (I): Numbers of sequences in the data[] array.				(int)
data[]		: (I): Array of all sequences from the specific archive.		(const CIPCSequenceRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCDatabaseSample::OnConfirmSequenceList(WORD wArchiveNr, 
											   int iNumRecords, 
											   const CIPCSequenceRecord data[])
{
	if(m_pPlayPicture)
	{
		//fill the sequence list
		m_pPlayPicture->IndicateSequenceList(wArchiveNr, iNumRecords,data, this);

		//send the last command, in this case "CIPC_DB_CONFIRM_SEQUENCE_LIST" to the 
		//PlayPicture control to fill the sequence list control on main dialog
		m_pPlayPicture->PostMessage(WM_USER,GetLastCmd(),wArchiveNr);
	}
}

/*********************************************************************************************
Description   : Override this function to receive the requested picture. With the picture
				also an array of fields[] will be received. The fields[] contains extra
				informations stored with the picture like format (Jpeg, Mpeg), timestamp, etc.

Parameters:   
wArchiveNr		: (I): The number of the archive which contains 
				   the sequences and the requested picture.				(WORD)		
wSequenceNr		: (I): The number of the sequence which contains 
				   the requested picture.								(WORD)
dwRecordNr		: (I): The number of the requested record.				(DWORD)
dwNrOfRecords	: (I): The number of pictures within the sequence.		(DWORD)
&pict			: (I): Reference of the picture itself.					(const CIPCPictureRecord)
iNumFields		: (I): Numbers of fields safed with the picture.		(int)
fields[]		: (I): Array of all fields safed with the picture.		(const CIPCField)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCDatabaseSample::OnConfirmRecordNr(WORD wArchiveNr, 
										   WORD wSequenceNr, 
										   DWORD dwRecordNr,
										   DWORD dwNrOfRecords,
										   const CIPCPictureRecord &pict,
										   int iNumFields,
										   const CIPCField fields[])
{
	if(m_pPlayPicture)
	{
		//indicate the received database picture and fields to the PlayPicture control
		//to show the picture and picture data on main dialog
		m_pPlayPicture->IndicatePicture(wArchiveNr, wSequenceNr, dwRecordNr, dwNrOfRecords, pict, iNumFields, fields);
	}
}