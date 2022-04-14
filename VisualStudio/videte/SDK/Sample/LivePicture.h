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
|* PROGRAM: LivePicture.h
|*
|* PURPOSE: Class to show live pictures of cameras.
******************************************************************************/

#pragma once
#include "picturecontrol.h"
#include "IPCOutputSample.h"
#include "IPCInputSample.h"


class CIPCOutputSample;

#define MAX_MPEG_FRAME_RATE 25
#define MAX_JPEG_FRAME_RATE 12

class CLivePicture :
	public CPictureControl
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CLivePicture(void);
	~CLivePicture(void);

//////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////
public:
	void					IndicateRecords(WORD wGroupID, 
											int iNumGroups, 
											int numRecords, 
											const CIPCOutputRecord records[],
											CIPCOutputSample* pOutputSample);
	void					IndicateNewRelayStatus(CSecID relayID, 
												BOOL bClosed);
	void					PictureData(const CIPCPictureRecord &pict,
										CSecID idArchive, 
										DWORD dwX, 
										DWORD dwY,
										int iFrameRate);

	static void				ReleaseMMTimer();
	void					KillGOPTimer();
	
//////////////////////////////////////////////////////////////////////
private:

	void					SetCompressionTypeOfCamera(CSecID SecID);

	//implementations for use of mpeg4 images
	void					SetGOPTimer(UINT uElapse);
	void					GetNextPictureOfGOP();
	static void CALLBACK	GOPTimer(UINT wTimerID, 
									 UINT msg, 
									 DWORD dwUser, 
									 DWORD dw1, 
									 DWORD dw2);

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
private:
	CIPCPictureRecord*	m_pPictureRecord;	//pointer to the live picture
	CMPEG4Decoder*		m_pMPEG4Decoder;	//pointer to the MPEG4 decoder
	CJpeg*				m_pJpeg;			//pointer to the Jpeg decoder
	CIPCOutputRecordArray	m_paCameras;	//pointerarray of the output records
											//of type 'camera'
	CIPCOutputRecordArray	m_paRelays;		//pointerarray of the output records
											//of type 'relay'
	CIPCOutputSample*	m_pCIPCOutputSample;//pointer to OutputSample which
											//controls the output connection
											//to the connected server
	CIPCInputSample*	m_pCIPCInputSample;	//pointer to InputSample which
											//controls the input connection
											//to the connected server
	CSecID				m_secIDCurrentCamera;//system wide internal SecID 
	volatile UINT		m_uGOPTimer;		//identifier for the multi media
											//GOP timer event

	//request up to 5 pictures to ensure 25 fps. if request a new picture after the last was received
	//we will not have 25 fps because the time between 2 request will be to big.
	//so we request up to 5 pictures and queue the received pictures until we have to show them.
	int					m_iPictureRequests;	//counter for number of live 
											//picture requests. 
											
	CIPCPictureRecords	m_GOPs;				//container for MPEG4 picture
											//groups. Up to 5 picture in one GOP.
	int					m_nCurrentPicture;	//number of current decoded Mpeg4 picture
											//within the GOP
	BOOL				m_bFrameWasDropped;	//FALSE if decoding of a MPEG4 picture fails
	BOOL				m_bUseMMTimer;		//TRUE if multi media timer can be used
											//FALSE if multi media timer cannot be used
	static UINT			gm_wTimerRes;		//timer resolution in milli seconds of
											//the multi media timer
	BOOL				m_bGOPTimerThreadPriorIsSet;	//TRUE if the thread priority
														//of the multi media thread
														//was set to THREAD_PRIORITY_NORMAL
	CCriticalSection	m_csPictureRecord;	//critical section object to ensure that 
											//one thread at the time has access to 
											//the current live picture
	CompressionType		m_CompressionType;	//the compression type of the live cameras,
											//MPEG 4 or Jpeg.

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
public:

	DECLARE_MESSAGE_MAP()
	afx_msg void			OnPaint();
	afx_msg LRESULT			DoRequestPicture(WPARAM, LPARAM);
	afx_msg LRESULT			OnDecodeVideo(WPARAM, LPARAM);
	afx_msg void			OnTimer(UINT nIDEvent);
	afx_msg void			OnDestroy();
};
//////////////////////////////////////////////////////////////////////