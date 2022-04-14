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
|* PROGRAM: IPCOutputSample.h
|*
|* PURPOSE: Central class for connection and communication 
|*			to videte's outputs (cameras, relays).
******************************************************************************/

#pragma once
#include "cipcoutputclient.h"
#include "LivePicture.h"

class CLivePicture;

class CIPCOutputSample :	public CIPCOutputClient
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CIPCOutputSample(CLivePicture& LivePicture, LPCTSTR shmName, WORD wCodePage);
	~CIPCOutputSample(void);

//////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////
protected:

	virtual void			OnConfirmConnection();
	virtual void			OnConfirmBitrate(DWORD dwBitrate);
	virtual void			OnAddRecord(const CIPCOutputRecord& record);
	virtual void			OnUpdateRecord(const CIPCOutputRecord & record);
	virtual void			OnConfirmInfoOutputs(WORD wGroupID, 
												int iNumGroups, 
												int numRecords, 
												const CIPCOutputRecord records[]);
	virtual void			OnConfirmSetRelay(CSecID relayID, 
											  BOOL bClosed);

	virtual void			OnIndicateVideo(const CIPCPictureRecord& pict,
											DWORD dwMDX,
											DWORD dwMDY,
											DWORD dwUserData,
											CSecID idArchive);

	virtual void			OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
												DWORD dwUserID, 
												CSecID idArchive);

//////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////
public:

	void					RequestLivePictures(CSecID sID, 
												CompressionType m_CompressionType);
	BOOL					IsLowBandwidth();

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
private:
	CLivePicture*			m_pLivePicture;	//pointer to the class which 
											//controls the live pictures
	int						m_iFrameRate;	//the framerate for showing 
											//live pictures
	DWORD					m_dwBitRate;	//the bitrate of the connection
											//to the server
};
//////////////////////////////////////////////////////////////////////
