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
|* PROGRAM: ipcOutputSample.cpp
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's outputs (cameras, relays); implementation file.
******************************************************************************/

#include "StdAfx.h"
#include "LivePicture.h"
#include "ipcOutputSample.h"
#include "SampleDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCOutputSample::CIPCOutputSample(CLivePicture& LivePicture, 
								   LPCTSTR shmName,
								   WORD wCodePage)
: CIPCOutputClient(shmName,FALSE)
{
	m_pLivePicture		= &LivePicture;
	m_iFrameRate		= MAX_MPEG_FRAME_RATE;
	m_dwBitRate			= 8 * 1024 * 1024;
	StartThread();
}
CIPCOutputSample::~CIPCOutputSample(void)
{
	StopThread();
}

/*********************************************************************************************
Description   : Override this function to get the confirmation of the output connection.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnConfirmConnection()
{
	//request the bitrate of the connection (between 64kBit ... 8mBit)+
	//answer will be received through function "OnConfirmBitrate(..)"
	DoRequestBitrate();
	
	//request all cameras and relays of the connected server
	//answer will be received through function "OnConfirmInfoOutputs(..)"
	DoRequestInfoOutputs(SECID_NO_GROUPID);	// SECID_NO_GROUPID = all groups
}

/*********************************************************************************************
Description   : This function has to be overwritten.
				Override this function to get each output record (cameras, relays) separately.
Parameters:   
&record	: (I): Reference to the output record (cameras, relays).	(const CIPCOutputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnAddRecord(const CIPCOutputRecord& record)
{
	//insert some code here if you want to handle each live camera/relay separately
}
/*********************************************************************************************
Description   : This function has to be overwritten.
				Override this function to be informed about status change of a relay
				or cameras separately. 
				E.g.: Status change of relay: open or closed
					  Status change of a camera: camera is switched off in the system manegment
					  on an idip system

Parameters:   
&record	: (I): Reference to the output record (cameras, relays).	(const CIPCOutputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnUpdateRecord(const CIPCOutputRecord & record)
{
	//insert some code here if you want to handle each status change of 
	//a live camera/relay separately
}
/*********************************************************************************************
Description   : Override this function to get all input records (cameras, relays).

Parameters:   
wGroupID	: (I): Indicates the group (for internal use).			(WORD)
iNumGroups	: (I): Number of output groups (for internal use).		(int) 
numRecords  : (I): Numbers of output records (cameras, relays) 
				   in the records[] array.							(int)
records[]	: (I): Array of all output records (cameras, relays).	(const CIPCOutputRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnConfirmInfoOutputs(WORD wGroupID, 
											int iNumGroups, 
											int numRecords, 
											const CIPCOutputRecord records[])
{

	if(m_pLivePicture)
	{
		//indicate the received cameras and relays to the LivePicture control
		//to show the picture of life cameras on main dialog
		m_pLivePicture->IndicateRecords(wGroupID, iNumGroups, numRecords, records, this);
	}

	//only when the base class function is called, the virtual function OnAddRecord(...)
	//will be called from the base class.
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,numRecords,records);
}

/*********************************************************************************************
Description   : Override this function to be informed when the status of a relay has changed.

Parameters:   
relayID	: (I): Indicates the relay ID.					(CSecID)
bClosed	: (I): new status of the relay (open/clode).	(BOOL) 

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{ 
	if(m_pLivePicture)
	{
		//indicate the new status of the relay to the LivePicture control
		//to update the relay list control on main dialog
		m_pLivePicture->IndicateNewRelayStatus(relayID, bClosed);
	}

	//only when the base class function is called, the virtual function OnUpdateRecord(...)
	//will be called from the base class.
	CIPCOutputClient::OnConfirmSetRelay(relayID, bClosed);
}
/*********************************************************************************************
Description   : Depending on the compression type (Mpeg4, Jpeg, others) corresponding 
				live pictures will be requested.				
Parameters:   
sID			: (I): Indicates the system wide SecID (see declaration of CSecID)	(CSecID)
ct			: (I): Compression type of the requested live picture.				(CompressionType) 

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::RequestLivePictures(CSecID sID, 
										   CompressionType ct)
{
	//set default values
	Resolution res = RESOLUTION_2CIF;
	Compression comp = COMPRESSION_12;

	if(ct == COMPRESSION_MPEG4)
	{
		Resolution resMpeg = RESOLUTION_CIF;
		if (IsLowBandwidth())
		{
			//on low bandwidth set down the resolution to safe bandwidth
			if (res == RESOLUTION_2CIF)
			{
				resMpeg = RESOLUTION_CIF;
			}
			else if (res == RESOLUTION_CIF)
			{
				resMpeg = RESOLUTION_CIF;
			}
			else if (res == RESOLUTION_CIF)
			{
				resMpeg = RESOLUTION_CIF;
			}
		}
		else
		{
			if (res == RESOLUTION_2CIF)
			{
				resMpeg = RESOLUTION_2CIF;
			}
			else if (res == RESOLUTION_CIF)
			{
				resMpeg = RESOLUTION_CIF;
			}
		}

		//in this sample we request 25 fps on MPEG4 pictures
		m_iFrameRate = MAX_MPEG_FRAME_RATE;

		//start the request of MPEG4 pictures
		DoRequestStartVideo(sID,res,comp,COMPRESSION_MPEG4,m_iFrameRate,5,0);
	}
	else if(ct == COMPRESSION_JPEG)
	{
		//in this sample we request 12 fps on Jpeg pictures 
		m_iFrameRate = MAX_JPEG_FRAME_RATE;

		//start the request of Jpeg pictures
		DoRequestNewJpegEncoding(sID,res,comp,m_iFrameRate); 
	}
	else
	{
		//other compression types e.g. H263
	}
}

/*********************************************************************************************
Description   : Override this function to get the requested Mpeg4 live picture.
				This function is called after starting Mpeg4 live picture request with
				"DoRequestStartVideo(...)"
				
Parameters:   
&pict		: (I): Reference to the indicated picture.					(const CIPCPictureRecord)
dwMDX		: (I): X-Coordinate of the highest movement detection 
			       within the picture.									(DWORD) 
dwMDY		: (I): Y-Coordinate of the highest movement detection 
				   within the picture.									(DWORD) 
dwUserData	: (I): Indicates some user data (for internal use)			(DWORD) 
idArchive	: (I): Indicates the archive were stored the last picture
				   of this camera. If no storing process is active
				   on the connected server the value is SECID_NO_ID.	(CSecID)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnIndicateVideo(const CIPCPictureRecord& pict,
									   DWORD dwMDX,
									   DWORD dwMDY,
									   DWORD dwUserData, 
									   CSecID idArchive)
{

	if (m_pLivePicture)
	{
		//Indicate the picture to the live picture window on the dialog to show the picture
		m_pLivePicture->PictureData(pict, idArchive, dwMDX, dwMDY, m_iFrameRate);
	}
}

/*********************************************************************************************
Description   : Override this function to get the requested Jpeg live picture.
				This function is called after starting Jpeg live picture request with
				"DoRequestNewJpegEncoding(...)"

Parameters:   
&pict		: (I): Reference to the indicated picture.					(const CIPCPictureRecord)
dwUserData	: (I): Indicates some user data (for internal use)			(DWORD) 
idArchive	: (I): Indicates the archive were stored the last picture
				   of this camera. If no storing process is active
				   on the connected server the value is SECID_NO_ID.	(CSecID)	

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnConfirmJpegEncoding(const CIPCPictureRecord &pict, 
											 DWORD dwUserData, 
											 CSecID idArchive)
{
	if (m_pLivePicture)
	{
		//Indicate the picture to the live picture window on the dialog to show the picture
		m_pLivePicture->PictureData(pict,idArchive,0,0,m_iFrameRate);
	}
}

/*********************************************************************************************
Description   : Override this function to get the bitrate of the connected server.

Parameters:   
dwBitRate	: (I): Birate of the connected server.				(DWORD)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CIPCOutputSample::OnConfirmBitrate(DWORD dwBitRate)
{
	if (dwBitRate==0)
	{
		dwBitRate = 512 * 1024;
	}
	m_dwBitRate = dwBitRate;
}

/*********************************************************************************************
Description   : Checks if the bandwidth of the connection to the server is high or low.

Parameters:   none

Return Value:  TRUE if the bandwidth is low,
			   FALSE if the bandwidth is high. (BOOL)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CIPCOutputSample::IsLowBandwidth()
{
	return m_dwBitRate <= 128*1024;
}