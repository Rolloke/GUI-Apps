/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndCoco.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndCoco.cpp $
// CHECKIN:		$Date: 5.05.06 13:28 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 5.05.06 11:34 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"
#include "WndCoco.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "ViewCamera.h"
#include "Server.h"
#include "CIPCOutputIdipClientDecoder.h"
#include "WndReference.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Coco Resolutions
// RESOLUTION_QCIF 176 x 144, 11:9	
// RESOLUTION_2CIF 352 x 288, 11:9

IMPLEMENT_DYNAMIC(CWndCoco, CWndLive)
/////////////////////////////////////////////////////////////////////////////
// CWndCoco
CWndCoco::CWndCoco(CServer* pServer, const CIPCOutputRecord &or,BOOL bJPEG2h263) 
	: CWndLive(pServer, or)
{
//	m_iX = 11;
//	m_iY = 9;
	m_bIPicture = FALSE;
	m_iQuality = 0;
	m_dwTick = GetTickCount();
	m_iFrames = 0;
	m_pH263 = NULL;
	m_bJPEG2h263 = bJPEG2h263;
	m_nType = WST_COCO;

	if (   m_pServer->IsDTS()
		&& m_pServer->Can_DV_H263_QCIF()
		&& (theApp.m_bFirstCam==ALL_CAMS))
	{
		m_Resolution = RESOLUTION_QCIF;
	}
	if (   !m_pServer->IsDTS()
		&& m_pServer->IsMultiCamera()
		&& (theApp.m_bFirstCam==ALL_CAMS))
	{
		m_Resolution = RESOLUTION_QCIF;
	}


	if (bJPEG2h263)
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		DWORD dwBitrate = pOutput->GetBitrate();

		if (    dwBitrate<=64*1024
			|| (theApp.m_bFirstCam==ALL_CAMS))
		{
			m_iQuality = 4;
		}
		else
		{
			m_iQuality = 3;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CWndCoco::~CWndCoco()
{
	m_csH263.Lock();
	CH263* pH263 = m_pH263;
	m_pH263 = NULL;
	m_csH263.Unlock();
	WK_DELETE(pH263);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndCoco, CWndLive)
	//{{AFX_MSG_MAP(CWndCoco)
	ON_WM_CREATE()
	ON_MESSAGE(WM_DECODE_VIDEO, OnDecodeVideo)
	ON_COMMAND_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnFps_X_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnUpdateFps_X_X)
//	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_COMMAND(ID_VIDEO_SAVE_REFERENCE, OnVideoSaveReference)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
int CWndCoco::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndLive::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_csH263.Lock();
	m_pH263 = new CH263;
	m_pH263->SetCaps(H263_FIT,!m_b1to1); // With new H26x-Decoder from DResearch obsolete
	m_pH263->Init(this,WM_DECODE_VIDEO,0, 0);
	m_csH263.Unlock();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CSize CWndCoco::GetPictureSize()
{
	CSize s;
	s.cx = s.cy = 0;

	m_csH263.Lock();
	s = m_pH263->GetImageDims();
	m_csH263.Unlock();

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndCoco::GetTitleText(CDC* pDC)
{
	if (theApp.m_bShowFPS)
	{
		CString s;
		double fps = (double)m_iFrames*1000.0/(double)(GetTimeSpan(m_dwTick));
		
		s.Format(_T("%02.03f %s"),fps,CWndLive::GetTitleText(pDC));
		
		return s;
	}
	else
	{
		return CWndLive::GetTitleText(pDC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::GetVideoRect(LPRECT lpRect)
{
	GetVideoClientRect(lpRect);
	ClientToScreen(lpRect);
	lpRect->left = max(lpRect->left,0);
	lpRect->top  = max(lpRect->top,0);
	lpRect->right = min(lpRect->right,GetSystemMetrics(SM_CXSCREEN));
	lpRect->bottom = min(lpRect->bottom,GetSystemMetrics(SM_CYSCREEN));
	m_pViewIdipClient->ScreenToClient(lpRect);
} 
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		GetVideoClientRect(rect);
		m_csH263.Lock();
		m_pH263->OnDraw(pDC,rect, m_rcZoom);
		m_csH263.Unlock();
	}
	CWndLive::OnDraw(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	m_csH263.Lock();
	m_pH263->OnDraw(pDC,rcDest);
	m_csH263.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::DrawKeyColor(CDC* pDC)
{
	CRect rect;
	GetVideoClientRect(rect);
	rect.DeflateRect(CW_KEYCOLOR_OFFSET,CW_KEYCOLOR_OFFSET);
	pDC->FillRect(rect,&m_pViewIdipClient->m_KeyBrush);
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::ClearWasSomeTimesActive()
{
	if (!IsCmdActive())
	{
		m_bWasSomeTimesActive = FALSE;
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndCoco::PictureData(const CIPCPictureRecord &pict, CSecID idArchive, DWORD dwX, DWORD dwY)
{
	if (m_iPictureRequests>0)
	{
		m_iPictureRequests--;
	}
	if (pict.GetCompressionType() == COMPRESSION_H263)
	{
		m_csPictureRecord.Lock();
		m_stTimeStamp = pict.GetTimeStamp();
		m_bTimeValid = TRUE;
		m_idArchive = idArchive;
		m_dwLastPictureConfirmedTime = GetTickCount();
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = new CIPCPictureRecord(pict);
		m_csPictureRecord.Unlock();


		m_csH263.Lock();
		m_pH263->Decode(pict);
		m_csH263.Unlock();
		Sleep(0);
		if (theApp.m_bShowFPS)
		{
			m_iFrames++;
		}
		StorePicture(pict);
		m_iWaitingForPicture = 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndCoco::OnDecodeVideo(WPARAM, LPARAM)
{
	if (m_pServer->IsDTS())
	{
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		Invalidate();
		UpdateWindow();
		if (   m_pServer->IsDun() 
			|| m_pServer->IsTCPIP()
			|| (   m_pServer->IsISDN()
				&& m_Resolution == RESOLUTION_QCIF)
			|| (   m_pServer->IsISDN()
				&& (m_pViewIdipClient->GetDocument()->GetSequence()	== FALSE)
				)
			)
		{
			if (IsRequesting())
			{
				DoRequestPictures();
			}
		}
		Sleep(0);
	}
	else	// not DV
	{
		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		Invalidate();
		UpdateWindow();
		if (   m_bJPEG2h263 
			&& !IsStreaming())
		{
			if (IsRequesting())
			{
				DoRequestPictures();
			}
		}
		Sleep(0);
	}
	m_bWasSomeTimesActive = TRUE;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::OnFps_X_X(UINT nID) 
{
	switch (nID)
	{
		case ID_FPS_1_1:  m_iQuality = 4; break;
		case ID_FPS_3_1:  m_iQuality = 3; break;
		case ID_FPS_6_1:  m_iQuality = 2; break;
		case ID_FPS_9_1:  m_iQuality = 1; break;
		case ID_FPS_12_1: m_iQuality = 0; break;
		default: ASSERT(FALSE); return;
	}
	
	DoRequestPictures(RQ_FORCE_ONE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::OnUpdateFps_X_X(CCmdUI* pCmdUI) 
{
	int nQuality = -1;
	int nFps	 = -1;
	switch (pCmdUI->m_nID)
	{
		case ID_FPS_1_1:  nQuality = 4; nFps = 1;  break;
		case ID_FPS_3_1:  nQuality = 3; nFps = 3;  break;
		case ID_FPS_6_1:  nQuality = 2; nFps = 6;  break;
		case ID_FPS_9_1:  nQuality = 1; nFps = 9;  break;
		case ID_FPS_12_1: nQuality = 0; nFps = 12; break;
		default: ASSERT(FALSE); return;
	}
	BOOL bEnable = CanAdjustFPS(nFps);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && m_iQuality == nQuality);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndCoco::CanAdjustFPS(int fps)
{
	if (   m_pServer
		&& m_pServer->IsConnectedLive())
	{
		if (m_bJPEG2h263)
		{
			CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
			DWORD dwBitrate = pOutput->GetBitrate();

			int iMaxLow = 6;
			int iMaxHigh = 3;
			if (CanMultipleRequests())
			{
				if (dwBitrate<=64*1024)
				{
					iMaxHigh = 6;
					iMaxLow = 9;
				}
				else if (dwBitrate<=128*1024)
				{
					iMaxHigh = 9;
					iMaxLow = 12;
				}
			}
			else
			{
				if (dwBitrate<=64*1024)
				{
					iMaxHigh = 3;
					iMaxLow = 6;
				}
				else if (dwBitrate<=128*1024)
				{
					iMaxHigh = 6;
					iMaxLow = 9;
				}
			}
			return fps <= ((m_Resolution == RESOLUTION_2CIF) ? iMaxHigh : iMaxLow);
		}
	}
	return theApp.m_bCoCoQuality;
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1 ? true : false;
	m_csH263.Lock();
	m_pH263->SetCaps(H263_FIT,!m_b1to1);
	m_csH263.Unlock();
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndCoco::OnVideoSaveReference() 
{
	CJpeg jpeg;
	CString sLocal,sRemote;

	sRemote = GetRemoteReferenceFileName();
	sLocal = GetLocalReferenceFileName();

	m_csH263.Lock();
	HGLOBAL hDIB = m_pH263->GetDib()->GetHDIB();
	m_csH263.Unlock();
	if (hDIB)
	{
		jpeg.EncodeJpegToFile(hDIB, sRemote);
		GlobalFree(hDIB);
	}
	
	if (!m_pServer->IsLocal())
	{
		CIPCInputIdipClient* pInput = m_pServer->GetInput();
		if (pInput)
		{
			CFile file;
			LPBYTE pBuffer=NULL;
			DWORD dwLen=0;
			CFileException cfe;
			CFileStatus cfs;

			if (file.Open(sRemote,CFile::modeRead|CFile::shareDenyNone,&cfe))
			{
				if (file.GetStatus(cfs))
				{
					if (IsBetween(cfs.m_size, 0, MAX_DWORD))
					{
						dwLen = (DWORD)cfs.m_size;
						pBuffer = new BYTE[(UINT)dwLen];

						TRY
						{
							if (dwLen==file.Read(pBuffer,dwLen))
							{
								pInput->DoRequestFileUpdate(RFU_FULLPATH,
									sLocal,pBuffer,dwLen,FALSE);
							}
						}
						WK_CATCH(_T("cannot read temp reference image"));
					}
					else
					{
						WK_TRACE_ERROR(_T("Filesize out of range\n"));
					}
				}
				file.Close();
				Sleep(10);
			}
			WK_DELETE_ARRAY(pBuffer);
		}
	}
	CWndReference* pRW = m_pViewIdipClient->GetWndReference(GetServerID(),
															  m_orOutput.GetName());
	if (WK_IS_WINDOW(pRW))
	{
		pRW->Reload();
	}
}
/////////////////////////////////////////////////////////////////////////////
CH263* CWndCoco::GetH263()
{
	return m_pH263;
}