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
|* PROGRAM: playpicture.cpp
|*
|* PURPOSE: Class to show database pictures; implementation file.
******************************************************************************/

#include "StdAfx.h"
#include "playpicture.h"
#include "SampleDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPlayPicture::CPlayPicture(void)
{
	m_pPictureRecord		= NULL;
	m_pMPEG4Decoder			= NULL;
	m_pJpeg					= NULL;
	m_PlayStatus			= PS_STOP;
}
/////////////////////////////////////////////////////////////
CPlayPicture::~CPlayPicture(void)
{
	DELETE_PTR(m_pMPEG4Decoder);
	DELETE_PTR(m_pJpeg);
	DELETE_PTR(m_pPictureRecord);
	m_Sequences.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPlayPicture, CPictureControl)
	ON_WM_PAINT()
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()

/*********************************************************************************************
Description   : This function draws the picture and additional data of the current picture 
				in the play picture window. Depending on the current picture 
				record (Mpeg4 or Jpeg) it draws the picture.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CPlayPicture::OnPaint()
{
	//device context for painting
	CPaintDC dc(this); 

	if(m_pPictureRecord)
	{
		CWindowDC dc(this);
		CRect rcCtrl;

		//set default compression type
		CompressionType ct = COMPRESSION_UNKNOWN;
		GetClientRect(&rcCtrl);
		ct = m_pPictureRecord->GetCompressionType();

		if (ct == COMPRESSION_MPEG4)
		{
			if(m_pMPEG4Decoder)
			{
				//draw the MPEG4 live picture
				m_pMPEG4Decoder->OnDraw(&dc, rcCtrl);
				
				if(m_pSampleDlg)
				{
					//draw to show new text messages
					m_pSampleDlg->OnDraw(&dc);
				}
			}
		}
		else if (ct == COMPRESSION_JPEG)
		{
			if(m_pJpeg)
			{
				//draw the Jpeg live picture
				m_pJpeg->OnDraw(&dc, rcCtrl);

				if(m_pSampleDlg)
				{
					//draw to show new text messages
					m_pSampleDlg->OnDraw(&dc);
				}
			}
		}
	}
}

/*********************************************************************************************
Description   : This function handles user defined messages.

Parameters:   
wParam	: (I): The user defined command.	(WPARAM)
pParam	: (I): The ID of the archive.		(LPARAM)

Return Value:  (LRESULT)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
LRESULT CPlayPicture::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CIPC_DB_CONFIRM_INFO:
		//this command indicates that the function CIPCDatabaseSample::OnConfirmInfo(..) was called
		if(m_pSampleDlg)
		{
			//fill the archives list control with the archives of the connected server
			m_pSampleDlg->FillListCtrl_Archives();
		}
		break;

	case CIPC_DB_CONFIRM_SEQUENCE_LIST:
		{
			//this command indicates that the function CIPCDatabaseSample::OnConfirmSequenceList(..) was called
			if(m_pSampleDlg)
			{
				//fill the sequence list control with the sequences of the specific archive
				WORD wArchiveNr = (WORD)lParam;
				m_Sequences.Lock();
				m_pSampleDlg->FillListCtrl_Sequences(wArchiveNr, m_Sequences);
				m_Sequences.Unlock();
			}
			break;
		}
	}
	return 0;
}

/*********************************************************************************************
Description   : This function fills the sequence list m_Sequences.

Parameters:   
wArchiveNr			: (I): The number of the archive which contains 
						   the sequences.								(WORD)
iNumRecords			: (I): Numbers of sequences in the data[] array.	(int)
data[]				: (I): Array of all sequences from the 
						   specific archive.							(const CIPCSequenceRecord)
pCIPCDatabaseSample	: (I): Pointer to the database sample class.		(CIPCDatabaseSample*)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CPlayPicture::IndicateSequenceList(WORD wArchiveNr, 
										int iNumRecords, 
										const CIPCSequenceRecord data[],
										CIPCDatabaseSample* pCIPCDatabaseSample)
{
	if(pCIPCDatabaseSample)
	{
		m_pCIPCDatabaseSample = pCIPCDatabaseSample;
	}
	m_Sequences.Lock();
	m_Sequences.SafeDeleteAll();
	for (int i=0;i<iNumRecords;i++)
	{
		//add all sequence records to the sequence list
		m_Sequences.Add(new CIPCSequenceRecord(data[i]));
	}
	m_Sequences.Unlock();
}

/*********************************************************************************************
Description   : This function is called when a requested database picture was received.
				It handles the playback functions, calculates which picture has to be
				requested next and calls the function "PictureData(...)" to draw the picture.
				With the picture also an array of fields[] was received. 
				The fields[] contains extra	informations stored with the picture 
				like format (Jpeg, Mpeg), timestamp, etc.

Parameters:   
wArchiveNr		: (I): The number of the archive which contains 
					   the sequences and the requested picture.			(WORD)		
wSequenceNr		: (I): The number of the sequence which contains 
					   the requested picture.							(WORD)
dwRecordNr		: (I): The number of the requested record.				(DWORD)
dwNrOfRecords	: (I): The number of pictures within the sequence.		(DWORD)
&pict			: (I): Reference of the picture itself.					(const CIPCPictureRecord)
iNumFields		: (I): Numbers of fields safed with the picture.		(int)
fields[]		: (I): Array of all fields safed with the picture.
					   E.g. timestamp of the picture, motion detection
					   coordinates, other database fields (see idip 
					   SystemManagment->Database->DatabaseFields)		(const CIPCField)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CPlayPicture::IndicatePicture(WORD wArchiveNr, 
								   WORD wSequenceNr, 
								   DWORD dwRecordNr, 
								   DWORD dwNrOfRecords,
								   const CIPCPictureRecord &pict, 
								   int iNumFields,
								   const CIPCField fields[])
{
	CString sTextMessage, sPictureInfo;

	//create text message with informations about the current database picture
	sTextMessage.Format(_T("Archive: %d  Seque: %d  Pict: %d of %d"), wArchiveNr
																	, wSequenceNr
																	, dwRecordNr
																	, dwNrOfRecords);
	CSystemTime st = pict.GetTimeStamp();

	//create time stamp information of the current database picture
	sPictureInfo.Format(_T("Date: %s  Time: %s MilliSek: %d\n"), st.GetDate(), st.GetTime(), st.GetMilliseconds());
	if(m_pSampleDlg)
	{
		//set record informations and text messages
		m_pSampleDlg->SetCurrentRecord(wArchiveNr, wSequenceNr, dwRecordNr);
		m_pSampleDlg->SetDBTextMsg(sTextMessage);
		m_pSampleDlg->SetPictureInfo(sPictureInfo);

		//should we play the next picture?
		if(m_PlayStatus != PS_STOP)
		{
			int iDirection = 1;
		
			switch (m_PlayStatus)
			{
				case PS_FAST_FORWARD:
					iDirection = 10;
					break;
				case PS_PLAY_BACKWARD:
					iDirection = -1;
					break;
				case PS_FAST_BACKWARD:
					iDirection = -10;
					break;
				default:
					iDirection = 1;
					break;
			}
		
			//start request of the next database picture
			Navigate(m_PlayStatus, wArchiveNr, wSequenceNr, dwRecordNr, iDirection);
		}
	}
	
	//show the database picture in the database picture control
	PictureData(pict);
}

/*********************************************************************************************
Description   : This function decodes the requested picture and redraws the picture.

Parameters:   
&pict			: (I): Reference of the picture itself.		(const CIPCPictureRecord)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CPlayPicture::PictureData(const CIPCPictureRecord &pict)
{
	//delete old database picture and get the current one
	DELETE_PTR(m_pPictureRecord);
	m_pPictureRecord = new CIPCPictureRecord(pict);

	if(m_pPictureRecord)
	{
		if (m_pPictureRecord->GetCompressionType() == COMPRESSION_MPEG4)
		{
			if (m_pMPEG4Decoder == NULL)
			{
				//create MPEG4 decoder
				m_pMPEG4Decoder = new CMPEG4Decoder();
			}
			
			if(m_pMPEG4Decoder)
			{
				//decode the MPEG4 database picture
				if(m_pMPEG4Decoder->DecodeMpeg4FromMemory((BYTE*)pict.GetData(), pict.GetDataLength()))
				{
					//show the database picture control and draw the picture
					ShowWindow(SW_SHOW);
					InvalidateRect(NULL);
				}
			}
		}
		else if(m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG)
		{
			if(m_pJpeg == NULL)
			{
				//create Jpeg decoder
				m_pJpeg = new CJpeg();
			}

			if(m_pJpeg)
			{
				//decode the Jpeg database picture
				if (m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(), pict.GetDataLength()))
				{
					//show the database picture control and draw the picture
					ShowWindow(SW_SHOW);
					InvalidateRect(NULL);
				}
			}
		}
	}
}

/*********************************************************************************************
Description   : This function starts the next request of a picture.

Parameters:   
ps			: (I): The current playstatus for replay.				(PlayStatus)
wArchiveNr	: (I): The number of the archive which contains 
				   the sequences and the requested picture.			(WORD)		
wSequenceNr	: (I): The number of the sequence which contains 
				   the requested picture.							(WORD)
dwRecordNr	: (I): The number of the requested record.				(DWORD)
nRecCount	: (I): The step width to the next picture to request.	(int)
dwCam		: (I): Camera ID (for internal use).					(DWORD)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CPlayPicture::Navigate(PlayStatus ps, WORD wArchiveNr, WORD wSequenceNr, DWORD dwRecordNr, 
							int nRecCount /*=0*/, DWORD dwCam /*= SECID_NO_ID*/)
{
	m_PlayStatus = ps;
	CIPCSequenceRecord* pSR = NULL;
	CString sTextMessage;
	DWORD dwNewRecord = 0;

	if(m_Sequences.GetSize())
	{
		//get the current sequence from the sequence list
		pSR = m_Sequences.GetSequence(wArchiveNr,wSequenceNr);
	}

	if (pSR)
	{
		if (nRecCount>0) //direction forward...
		{
			//step to next database picture
			dwNewRecord = dwRecordNr + nRecCount;

			//do we reahced the and of the current sequence?
			if (dwNewRecord > pSR->GetNrOfPictures())
			{
				//get the first record of the next sequence
				pSR = GetNextSequence(wSequenceNr);
				if (pSR == NULL)
				{
					pSR = m_Sequences.GetSequence(wArchiveNr,wSequenceNr);
					dwNewRecord = pSR->GetNrOfPictures();
				}
				else
				{
					dwRecordNr	= 0;
					dwNewRecord = 1;
					wSequenceNr = pSR->GetSequenceNr();
				}
			}


			if (   dwNewRecord > 0
				&& dwNewRecord != dwRecordNr)
			{
				//request the next database picture
				m_pCIPCDatabaseSample->DoRequestRecordNr(wArchiveNr,
														 wSequenceNr,
														 dwRecordNr,
														 dwNewRecord,
														 dwCam);

			}
		}
		else if (nRecCount<0) //direction backwards ...
		{
			int iRecordNr = dwRecordNr;

			//do we reached the begin of the current sequence?
			if (iRecordNr<=-nRecCount)
			{
				// get the last record of the previous sequence
				pSR = GetPrevSequence(wSequenceNr);
				if (pSR == NULL)
				{
					pSR = m_Sequences.GetSequence(wArchiveNr,wSequenceNr);
					dwNewRecord = 1;
				}
				else
				{
					dwRecordNr	= 0;
					dwNewRecord	= pSR->GetNrOfPictures();
					wSequenceNr = pSR->GetSequenceNr();
				}
			}
			else
			{
				//set the new database picture for request
				if(dwRecordNr + nRecCount > 0)
				{
					dwNewRecord = dwRecordNr + nRecCount;
				}
				else
				{
					dwRecordNr	= 0;
					dwNewRecord = dwRecordNr + 1;
				}
			}

			if (   dwNewRecord > 0
				&& dwNewRecord != dwRecordNr)
			{
				//request the previous database picture
				m_pCIPCDatabaseSample->DoRequestRecordNr(wArchiveNr,
															wSequenceNr,
															dwRecordNr,
															dwNewRecord,
															dwCam);
			}
		}
		else //nRecCount == 0, no direction, it's a database search
		{
			//not implemented
		}
	}
}

/*********************************************************************************************
Description   : This function returns the next sequence from the sequence list.

Parameters:   
wSequenceNr	: (I): The number of the current sequence. (WORD)

Return Value:  
pSR			: (O): The number of the next sequence. (CIPCSequenceRecord*)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
CIPCSequenceRecord* CPlayPicture::GetNextSequence(WORD wSequenceNr)
{
	CIPCSequenceRecord* pSR = NULL;

	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		if (m_Sequences.GetAtFast(i)->GetSequenceNr() == wSequenceNr)
		{
			if (i+1<m_Sequences.GetSize())
			{
				pSR = m_Sequences.GetAtFast(i+1);
			}
			// always break if found
			break;
		}
	}
	if (   (pSR == NULL)
		&& (wSequenceNr == 0)
		&& (m_Sequences.GetSize()>0))
	{
		pSR = m_Sequences.GetAtFast(0);
	}
	m_Sequences.Unlock();

	return pSR;
}

/*********************************************************************************************
Description   : This function returns the previous sequence from the sequence list.

Parameters:   
wSequenceNr	: (I): The number of the current sequence. (WORD)

Return Value:  
pSR			: (O): The number of the previous sequence. (CIPCSequenceRecord*)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
CIPCSequenceRecord* CPlayPicture::GetPrevSequence(WORD wSequenceNr)
{
	CIPCSequenceRecord* pSR = NULL;

	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		if (m_Sequences.GetAtFast(i)->GetSequenceNr() == wSequenceNr)
		{
			if (i>0)
			{
				pSR = m_Sequences.GetAtFast(i-1);
			}
			// always break if found
			break;
		}
	}
	if (   (pSR == NULL)
		&& (wSequenceNr == 0)
		&& (m_Sequences.GetSize()>0))
	{
		pSR = m_Sequences.GetAtFast(m_Sequences.GetSize()-1);
	}
	m_Sequences.Unlock();

	return pSR;
}