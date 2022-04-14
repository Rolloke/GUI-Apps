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
|* PROGRAM: livepicture.cpp
|*
|* PURPOSE: Class to show live pictures of cameras; implementation file.
******************************************************************************/

#include "StdAfx.h"
#include "livepicture.h"
#include "SampleDlg.h"

static CLivePicture* m_pThis;
UINT CLivePicture::gm_wTimerRes = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLivePicture::CLivePicture() 
{
	m_pThis					= this;
	m_pPictureRecord		= NULL;
	m_pMPEG4Decoder			= NULL;
	m_pJpeg					= NULL;
	m_pCIPCOutputSample		= NULL;
	m_secIDCurrentCamera	= DWORD(0);
	m_uGOPTimer				= 0;
	m_iPictureRequests		= 0;
	m_nCurrentPicture		= -1;
	m_bFrameWasDropped		= FALSE;
	m_bUseMMTimer			= TRUE;
	m_bGOPTimerThreadPriorIsSet = FALSE;
	m_CompressionType		= COMPRESSION_UNKNOWN;
}

CLivePicture::~CLivePicture(void)
{
	m_GOPs.SafeDeleteAll();
	DELETE_PTR(m_pMPEG4Decoder);
	DELETE_PTR(m_pJpeg);
	DELETE_PTR(m_pPictureRecord);
	m_paCameras.DeleteAll();
	m_paRelays.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLivePicture, CPictureControl)
	ON_WM_PAINT()
	ON_MESSAGE(WM_REQUEST_PICTURES, DoRequestPicture)
	ON_MESSAGE(WM_DECODE_VIDEO, OnDecodeVideo)	
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/*********************************************************************************************
Description   : This function is called once when the connection to the servers output client
				was successful. The outputs will be splited into output types cameras and relays
				and the corresponding list controls on the GUI will be filled with informations.

Parameters:   
wGroupID		: (I): Indicates the group (for internal use).			(WORD)
iNumGroups		: (I): Number of groups (for internal use).				(int) 
numRecords		: (I): Number of output records (cameras and relays)
					   in the records[] array.							(int)
records[]		: (I): Array of all outputs from the connected server.	(const CIPCOutputRecord)
pOutputSample	: (I): Pointer to the output sample.					(CIPCOutputSample*)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::IndicateRecords(WORD wGroupID, 
								   int iNumGroups, 
								   int numRecords, 
								   const CIPCOutputRecord records[], 
								   CIPCOutputSample* pOutputSample)
{
	if(pOutputSample)
	{
		//initialize
		m_pCIPCOutputSample = pOutputSample;
	}

	if(m_pSampleDlg)
	{
		m_paCameras.DeleteAll();
		m_paRelays.DeleteAll();
		for (int i=0; i<numRecords; i++)
		{
			if(records[i].IsCamera())
			{
				//fill the camera pointer array
				m_paCameras.Add(new CIPCOutputRecord(records[i]));
			}
			else if(records[i].IsRelay())
			{
				//fill the relay pointer array
				m_paRelays.Add(new CIPCOutputRecord(records[i]));
			}
		}

		if(m_paCameras.GetSize()>0)
		{
			//fill the camera list control on the main dialog
			m_pSampleDlg->FillListCtrl_Cameras(m_paCameras);
		}

		if(m_paRelays.GetSize()>0)
		{
			//fill the relay list control on the main dialog
			m_pSampleDlg->FillListCtrl_Relays(m_paRelays);
		}
	}
}

/*********************************************************************************************
Description   : This function is called once when the status of a relay of the connected server
				has changed. The relay list control will be updated.

Parameters:   
relayID		: (I): The ID of the changed relay.					(CSecID)
bClosed		: (I): The new status of the relay, open or closed.	(BOOL) 

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::IndicateNewRelayStatus(CSecID relayID, 
									      BOOL bClosed)
{
	for(int i = 0; i < m_paRelays.GetSize(); i++)
	{	
		DWORD dwID = 0;
		CString sCameraID;
		CString sType(_T("normally open"));
		CString sStatus(_T("open"));
		CString sName;
		CSecID ID;
		WORD wFlags = m_paRelays.GetAtFast(i)->GetFlags();

		sName = m_paRelays.GetAtFast(i)->GetName();
		ID = m_paRelays.GetAtFast(i)->GetID();
		dwID = m_paRelays.GetAtFast(i)->GetID().GetID();
		sCameraID.Format(_T("%d"), dwID);

		if(ID == relayID)
		{
			//change status open/close
			bClosed ? wFlags |= OREC_RELAY_CLOSED : wFlags &= ~OREC_RELAY_CLOSED;
			
			//set the new status
			m_paRelays.GetAtFast(i)->SetFlags(wFlags);

			//break when specific relay was changed
			break;
		}
	}

	if(m_pSampleDlg)
	{
		//fill the relay list control on the main dialog
		m_pSampleDlg->FillListCtrl_Relays(m_paRelays);
	}
}

/*********************************************************************************************
Description   : This function is called once when the connection to the servers output client
				was successful. The outputs are splited into output types cameras and relays
				and the corresponding list controls on the GUI will be filled with informations.

Parameters:   
&pict		: (I): Reference to the live picture.).						(const CIPCPictureRecord)
idArchive	: (I): Indicates the archive were stored the last picture
				   of this camera. If no storing process is active
				   on the connected server the value is SECID_NO_ID.	(CSecID)
dwMDX		: (I): X-Coordinate of the highest movement detection 
				   within the picture.									(DWORD) 
dwMDY		: (I): Y-Coordinate of the highest movement detection 
				   within the picture.									(DWORD) 
iFrameRate	: (I): Framerate (frames per second) of the picture
				   picture stream. In case of Mpeg4 it is 25 fps.		(int)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::PictureData(const CIPCPictureRecord &pict, 
							   CSecID idArchive, 
							   DWORD dwX, 
							   DWORD dwY, 
							   int iFrameRate)
{
	if (!IsWindowVisible())
	{
		//make sure the live picture control is visible on main dialog
		ShowWindow(SW_SHOW);
	}

	DWORD dwCamID = 0;

	//use a critical section to make sure that the picture record is used only in this thread
	CAutoCritSec acs(&m_csPictureRecord);

	//check the compression type of the picture
	if(pict.GetCompressionType() == COMPRESSION_MPEG4)
	{
		BOOL bSetGOP = FALSE;

		//in this Sample we only receive MPEG4 pictures with a frame rate of 25.
		//the Sample will receive 5 GOP's (group of pictures). each GOP's contains one
		//e.g. a frame rate of 5 picture is also possible
		if (iFrameRate == 25 && pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			//how many pictures are in the GOP
			int nNumPics = pict.GetNumberOfPictures();
			
			if (nNumPics > 1)
			{
				//we have more than one picture in the GOP
				if (m_iPictureRequests>0)
				{
					//decrease the number of requests
					m_iPictureRequests--;
				}

				if (m_pPictureRecord)
				{
					//a GOP already exists, add GOP to queue
					m_GOPs.Add(new CIPCPictureRecord(pict));
				}
				else									
				{
					//GOP not exists, put GOP to picture record
					m_pPictureRecord = new CIPCPictureRecord(pict);
					m_nCurrentPicture = 0;
				}
				if (m_pMPEG4Decoder == NULL)
				{
					//create a MPEG4 decoder
					m_pMPEG4Decoder = new CMPEG4Decoder();
				}

				DWORD dwTimeStep = pict.GetFrameTimeStep();
				double dTimeStepAvg = 40000.0;	// presume 25 fps

				if (m_uGOPTimer == 0)			// start timer
				{
					// round to the nearest integer
					dwTimeStep = (DWORD)((dTimeStepAvg / 1000.0) + 0.5);
					SetGOPTimer(dwTimeStep);
					m_nCurrentPicture = 0;
				}
				bSetGOP = TRUE;
			}
		}

		if (!bSetGOP)
		{
			//we have no GOP's, we have only one picture e.g. 5fps with one picture in one GOP
			if (m_uGOPTimer)
			{
				if(m_pPictureRecord)
				{
					//get camera ID of the current picture
					dwCamID = m_pPictureRecord->GetCamID().GetID();
					m_nCurrentPicture = -1;
					DELETE_PTR(m_pPictureRecord);

					KillGOPTimer();

					//request the next picture
					PostMessage(WM_REQUEST_PICTURES, dwCamID); 
				}
			}

			CAutoCritSec acs(&m_csPictureRecord);
			if (m_iPictureRequests>0)
			{
				m_iPictureRequests--;
			}

			DELETE_PTR(m_pPictureRecord);

			//save the current picture
			m_pPictureRecord = new CIPCPictureRecord(pict);

			BOOL bLowBandWidth = FALSE;
			if(m_pCIPCOutputSample)
			{
				bLowBandWidth = m_pCIPCOutputSample->IsLowBandwidth();
			}
			// doppelt hält besser
			// wenn der Decoder zu voll ist Bilder weglassen,
			// besser als aller RAM wech und Crash
			//TRACE("### m_pMPEG4Decoder: 0x%x\n", m_pMPEG4Decoder);
			if (m_pMPEG4Decoder == NULL)
			{
				//create a MPEG4 decoder
				m_pMPEG4Decoder = new CMPEG4Decoder();
			}

			//decode and show the current picture
			//if we have too much picture for decoding in the decoder drop a frame to ensure 
			//a minimized use of RAM space 
			if (   bLowBandWidth
				|| (   (m_pMPEG4Decoder->GetDecoderQueueLength()<=(DWORD)iFrameRate)
				&& (!m_bFrameWasDropped || (pict.GetPictureType() == SPT_FULL_PICTURE || pict.GetPictureType() == SPT_GOP_PICTURE))
				)
				)
			{
				if(m_pPictureRecord)
				{
					dwCamID = m_pPictureRecord->GetCamID().GetID();
				}

				//decode the current picture
				BOOL bDecoded = m_pMPEG4Decoder->DecodeMpeg4FromMemory((BYTE*)pict.GetData(), pict.GetDataLength());
				m_bFrameWasDropped = !bDecoded;
				acs.Unlock();

				//show the decoded picture
				PostMessage(WM_DECODE_VIDEO, dwCamID);
			}
			else
			{
				m_bFrameWasDropped = TRUE;
			}
		}
	}
	else if(pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		//create a Jpeg decoder
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
		}

		if (m_pJpeg)
		{
			DELETE_PTR(m_pPictureRecord);

			//save the current picture
			m_pPictureRecord = new CIPCPictureRecord(pict);

			if(m_pPictureRecord)
			{
				//decode the current picture
				dwCamID = m_pPictureRecord->GetCamID().GetID();
				if (!m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength()))
				{
					TRACE(_T("DecodeJpegFromMemory failed \n"));
				}

				//show the decoded picture
				PostMessage(WM_DECODE_VIDEO, dwCamID);
	
				//request the next Jpeg picture
				PostMessage(WM_REQUEST_PICTURES, dwCamID); 
			}
		}
	}
}
/*********************************************************************************************
Description   : This function draws the picture in the live picture window. Depending on 
				the current picture record (Mpeg4 or Jpeg) it draws the picture.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::OnPaint()
{
	//device context for painting
    CPaintDC dc(this); 
	
	CAutoCritSec acs(&m_csPictureRecord);
	if(m_pPictureRecord)
	{
		CWindowDC dc(this);
		CRect rcCtrl;

		CompressionType ct = COMPRESSION_UNKNOWN;
		ct = m_pPictureRecord->GetCompressionType();

		if (ct == COMPRESSION_MPEG4)
		{
			if(m_pMPEG4Decoder)
			{
				GetClientRect(&rcCtrl);
				m_pMPEG4Decoder->OnDraw(&dc, rcCtrl);
			}
		}
		else if(ct == COMPRESSION_JPEG)
		{
			if(m_pJpeg)
			{
				GetClientRect(&rcCtrl);
				m_pJpeg->OnDraw(&dc, rcCtrl);
			}
		}
	}
}

/*********************************************************************************************
Description   : This function is called each time when the last live picture was shown on
				GUI (Jpeg) or when the list of requests is less than 5 (Mpeg4).
				In case of Jpeg live pictures the next Jpeg picture will be requested
				after the last picture was delivered to the function PictureData(...).
				In case of Mpeg4 live pictures the next Mpeg4 picture will be requested
				in function OnTimer() when the count of requests is less than 5.

Parameters:   
wParam	: (I): The ID of the new selected camera. 			(WPARAM)
pParam	: (I): The ID of the (old) former selected camera.	(LPARAM)

Return Value:  (LRESULT)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
LRESULT CLivePicture::DoRequestPicture(WPARAM wParam, LPARAM pParam)
{
	//wParam contains the new selected camera ID, 
	//pParam contains the last active camera ID which has to be switched off
	//on first call of this function wParam and pParam contains the same camera ID
	//if requesting pictures of the same camera pParam will be 0

	CSecID secIDNewCamera((DWORD)wParam);

	if(m_pCIPCOutputSample)
	{
		if(pParam != 0)
		{
			//request pictures of a different camera
			m_secIDCurrentCamera = (DWORD)pParam;

			//stop requesting of picture from the last camera
			m_pCIPCOutputSample->DoRequestStopVideo(m_secIDCurrentCamera,0);

			//start requesting picture of the new current camera
			SetCompressionTypeOfCamera(secIDNewCamera);
			m_pCIPCOutputSample->RequestLivePictures(secIDNewCamera, m_CompressionType);
			m_secIDCurrentCamera = secIDNewCamera;
			
			//reset all variables to start with the new current camera
			m_iPictureRequests = 1;
			m_nCurrentPicture = -1;
			m_GOPs.SafeDeleteAll();
			DELETE_PTR(m_pMPEG4Decoder);
			DELETE_PTR(m_pJpeg);
			DELETE_PTR(m_pPictureRecord);
		}
		else
		{
			if(m_secIDCurrentCamera == secIDNewCamera)
			{					
				//request picture of the same camera as before
				m_pCIPCOutputSample->RequestLivePictures(secIDNewCamera, m_CompressionType);
				m_iPictureRequests++;
			}
		}
	}
	return 0;
}

/*********************************************************************************************
Description   : This function updates the GUI.

Parameters:   
wParam	: (I): Not in use.	(WPARAM)
pParam	: (I): Not in use.	(LPARAM)

Return Value:  (LRESULT)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
LRESULT CLivePicture::OnDecodeVideo(WPARAM wParam, LPARAM pParam)
{
	//draw the new live picture
	InvalidateRect(NULL);
	UpdateWindow();
	return 0;
}

/*********************************************************************************************
Description   : This function controls the requests of Mpeg4 live pictures and decides, when
				the next Mpeg4 live picture request has to be started.

Parameters:   
nIDEvent	: (I): The TimerID.	(UINT)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::OnTimer(UINT nIDEvent)
{
	CAutoCritSec acs(&m_csPictureRecord);
	if (nIDEvent == m_uGOPTimer)
	{
		if (m_nCurrentPicture == -1)
		{
			//no picture in GOP to decode, request next picture
			DWORD dwCamID = 0;
			if(m_pPictureRecord)
			{
				dwCamID = m_pPictureRecord->GetCamID().GetID();

				//request the next picture of the same camera as before
				PostMessage(WM_REQUEST_PICTURES, dwCamID);
			}
			return;
		}

		int nNumPictures = 0;
		BOOL bNumPicturesReduced = FALSE;
		if (m_pPictureRecord)
		{
			int nNumGOPS = (int)m_GOPs.GetSize();
			nNumPictures = m_pPictureRecord->GetNumberOfPictures();
			if (nNumGOPS > 1)
			{
				//more than one GOP, reduce
				nNumPictures = nNumPictures - nNumGOPS + 1;
				bNumPicturesReduced = TRUE;
			}

			if (m_iPictureRequests<5)
			{
				//do not more than 5 request
				//only start next request when we received the first picture of 5
				PostMessage(WM_REQUEST_PICTURES, m_pPictureRecord->GetCamID().GetID());
				m_iPictureRequests++;
			}

			if (m_nCurrentPicture >= nNumPictures)
			{
				//get the next picture from within the GOP
				GetNextPictureOfGOP();
			}
		}

		if (m_pPictureRecord)
		{
			BOOL dwCamID = m_pPictureRecord->GetCamID().GetID();
			if(!bNumPicturesReduced)
			{
				nNumPictures = m_pPictureRecord->GetNumberOfPictures();
			}
			if (m_pMPEG4Decoder == NULL)
			{
				//create MPEG4 decoder
				m_pMPEG4Decoder = new CMPEG4Decoder();
			}

			//decode the current live picture
			m_pMPEG4Decoder->DecodeMpeg4FromMemory((BYTE*)m_pPictureRecord->GetData(m_nCurrentPicture),
			m_pPictureRecord->GetDataLength(m_nCurrentPicture));

			//show the decoded picture in the live picture control
			PostMessage(WM_DECODE_VIDEO, dwCamID); 

			m_nCurrentPicture++;

			if (m_nCurrentPicture == nNumPictures)
			{
				//get the next picture from within the GOP
				GetNextPictureOfGOP();
			}
		}
	}
	acs.Unlock();
	CPictureControl::OnTimer(nIDEvent);
}

/*********************************************************************************************
Description   : This callback function is used with a multimedia timer. To handle 25 fps
				with Mpeg4 live pictures we need this multimedia timer to make sure, that every
				40ms the next Mpeg4 live picture has to be shown on the GUI.
				A "normal" timer is not exact enough.

Parameters:   
wTimerID: (I): The TimerID.															(UINT)
msg		: (I): Not in use but has to be defined (see MFC help for timeSetEvent).	(UINT)
dwUser	: (I): Not in use but has to be defined (see MFC help for timeSetEvent).	(DWORD)
dw1		: (I): Not in use but has to be defined (see MFC help for timeSetEvent).	(DWORD)
dw2		: (I): Not in use but has to be defined (see MFC help for timeSetEvent).	(DWORD)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CALLBACK CLivePicture::GOPTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{ 
	if(m_pThis)
	{
		if(!m_pThis->m_bGOPTimerThreadPriorIsSet)
		{
			//the thread priority of the GOPtimer has to be set to "normal"
			//if not, the GOPtimer will slow down the computer
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			m_pThis->m_bGOPTimerThreadPriorIsSet = TRUE;
		}

		if (m_pThis->m_uGOPTimer)
		{
			m_pThis->OnTimer(wTimerID);
		}
	}
}

/*********************************************************************************************
Description   : This function starts a specified multimedia timer event. If no multimedia
				timer is available a "normal" timer will be started instead.

Parameters:   
uElapse: (I): The time period for the multimedia timer. (UINT)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::SetGOPTimer(UINT uElapse)
{
	if (m_bUseMMTimer)
	{
		TIMECAPS tc;
		MMRESULT mmr = 0;
		if (gm_wTimerRes == 0)
		{
			mmr = ::timeGetDevCaps(&tc, sizeof(TIMECAPS));
			if (mmr == TIMERR_NOERROR) 
			{
				gm_wTimerRes = min(max(tc.wPeriodMin, 5), tc.wPeriodMax);
				mmr = ::timeBeginPeriod(gm_wTimerRes);
			}

			if (mmr != TIMERR_NOERROR)
			{
				m_bUseMMTimer = FALSE;
			}

		}
		if (m_bUseMMTimer)
		{
			m_uGOPTimer = ::timeSetEvent(uElapse, gm_wTimerRes, GOPTimer, 0, TIME_PERIODIC);
		}
	}

	if (!m_bUseMMTimer)
	{
		m_uGOPTimer = CWnd::SetTimer(110, uElapse, NULL);
	}
}

/*********************************************************************************************
Description   : This function kills the specified multimedia timer. If no multimedia
				timer is available the "normal" timer will be killed.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::KillGOPTimer()
{
	m_GOPs.SafeDeleteAll();
	if (m_uGOPTimer)
	{
		UINT uIDtemp = m_uGOPTimer;
		
		// set to zero, to make event id invalid
		m_uGOPTimer = 0;				

		if (m_bUseMMTimer)
		{
			::timeKillEvent(uIDtemp);
		}
		else
		{
			CWnd::KillTimer(uIDtemp);
		}
	}
}

/*********************************************************************************************
Description   : This function is called when the live window on the GUI will be destroyed and
				kills the multimedia timer.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::OnDestroy()
{
	KillGOPTimer();
	CPictureControl::OnDestroy();
}

/*********************************************************************************************
Description   : This function starts the next request of the current picture and gets 
				the next i-frame or p-frame. 
				In case of Mpeg4 live pictures and a high framerate (e.g. 25) this sample 
				receives 25 pictures per second. To speed up the transfer of 25 Mpeg4 pictures 
				in one second this sample receives 5 groups of pictures (GOP). The first 
				picture of the group is an i-frame (intraframe), the other pictures 
				are p-frames (predicted frame).

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::GetNextPictureOfGOP()
{
	int nNumGops = (int)m_GOPs.GetSize();
	if (m_pPictureRecord)
	{
		//we already have a picture, request the next
		DWORD dwCamID = m_pPictureRecord->GetCamID().GetID();
		PostMessage(WM_REQUEST_PICTURES, dwCamID);
	}

	DELETE_PTR(m_pPictureRecord);

	if (nNumGops)
	{
		//get the next picture from within the GOP
		m_pPictureRecord = m_GOPs.GetAt(0);
		m_GOPs.RemoveAt(0);
		m_nCurrentPicture = 0;
	}
	else
	{
		m_nCurrentPicture = -1;
	}
}

/*********************************************************************************************
Description   : This function clears a previously set minimum timer resolution. 

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::ReleaseMMTimer()
{
	if (gm_wTimerRes)
	{
		timeEndPeriod(gm_wTimerRes);
		gm_wTimerRes = 0;
	}
}

/*********************************************************************************************
Description   : This function sets the compression type of the specific camera.

Parameters:   
SecID: (I): The ID of the camera to set. (CSecID)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CLivePicture::SetCompressionTypeOfCamera(CSecID SecID)
{
	for(int i = 0; i < m_paCameras.GetSize(); i++)
	{
		if(m_paCameras.GetAtFast(i)->GetID() == SecID)
		{
			if(m_paCameras.GetAtFast(i)->CameraDoesJpeg())
			{
				m_CompressionType = COMPRESSION_JPEG;
			}
			else if(m_paCameras.GetAtFast(i)->CameraDoesMpeg())
			{
				m_CompressionType = COMPRESSION_MPEG4;
			}
			break;
		}
	}
}