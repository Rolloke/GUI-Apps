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
|* PROGRAM: PlayPicture.h
|*
|* PURPOSE: Class to show database pictures.
******************************************************************************/

#pragma once
#include "picturecontrol.h"
#include "IPCDatabaseSample.h"

/////////////////////////////////////////////////////////////////////////////
// definition of playstatus to handle playback of database pictures
typedef enum PlayStatus
{
	PS_STEP_FORWARD		=	1,
	PS_PLAY_FORWARD		=	2,
	PS_FAST_FORWARD		=	3,
	PS_STEP_BACKWARD	=	4,
	PS_PLAY_BACKWARD	=	5,
	PS_FAST_BACKWARD	=	6,
	PS_STOP				=	7
};

class CPlayPicture :
	public CPictureControl
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CPlayPicture(void);
	~CPlayPicture(void);

//////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////
	void					IndicateSequenceList(WORD wArchiveNr, 
												int iNumRecords, 
												const CIPCSequenceRecord data[],
												CIPCDatabaseSample* pCIPCDatabaseSample);

	void					IndicatePicture(WORD wArchiveNr, 
											WORD wSequenceNr, 
											DWORD dwRecordNr, 
											DWORD dwNrOfRecords,
											const CIPCPictureRecord &pict, 
											int iNumFields,
											const CIPCField fields[]);

	void					Navigate(PlayStatus ps,
									WORD wArchiveNr, 
									WORD wSequenceNr, 
									DWORD dwRecordNr, 
									int nRecCount = 0, 
									DWORD dwCam = SECID_NO_ID);

	void					PictureData(const CIPCPictureRecord &pict);
	CIPCSequenceRecord*		GetNextSequence(WORD wSequenceNr);
	CIPCSequenceRecord*		GetPrevSequence(WORD wSequenceNr);

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
private:

	CIPCSequenceRecords		m_Sequences;		//array of sequences
	CIPCPictureRecord*		m_pPictureRecord;	//pointer to a databse picture
	CMPEG4Decoder*			m_pMPEG4Decoder;	//pointer to MPEG4 decoder
	CJpeg*					m_pJpeg;			//pointer to Jpeg decoder
	CIPCDatabaseSample*		m_pCIPCDatabaseSample;//pointer to DatabasSample which
												  //controls the database connection
												  //to the connected server
	PlayStatus				m_PlayStatus;		//current playstatus for playback	
												//of database pictures

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
private:
	DECLARE_MESSAGE_MAP()
	afx_msg void			OnPaint();
	afx_msg LRESULT			OnUser(WPARAM wParam, LPARAM lParam);
};
//////////////////////////////////////////////////////////////////////
