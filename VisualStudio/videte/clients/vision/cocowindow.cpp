/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CocoWindow.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CocoWindow.cpp $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 70 $
// LAST CHANGE:	$Modtime: 25.02.04 11:40 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"

#include "Vision.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "Server.h"
#include "CIPCOutputVisionDecoder.h"

#include "CocoWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CCocoWindow::m_bShowFPS = FALSE;
// Coco Resolutions
// RESOLUTION_LOW 176 x 144, 11:9	
// RESOLUTION_HIGH 352 x 288, 11:9
/////////////////////////////////////////////////////////////////////////////
// CCocoWindow
CCocoWindow::CCocoWindow(CServer* pServer, const CIPCOutputRecord &or,BOOL bJPEG2h263) 
	: CDisplayWindow(pServer, or)
{
	m_iX = 11;
	m_iY = 9;
	m_bIPicture = FALSE;
	m_iQuality = 0;
	m_bSoftwareDecoding = TRUE;
	m_dwTick = GetTickCount();
	m_iFrames = 0;
	m_pH263 = NULL;
	m_bJPEG2h263 = bJPEG2h263;

	if (theApp.m_pDongle->RunCoCoUnit())
	{
		if (theApp.IsWin95() || theApp.IsWin98())
		{
			m_bSoftwareDecoding = theApp.m_bCoCoSoft;
		}
	}
	if (   m_pServer->IsDTS()
		&& m_pServer->Can_DV_H263_QCIF()
		&& !theApp.m_bFirstCam)
	{
		m_Resolution = RESOLUTION_LOW;
	}
	if (   !m_pServer->IsDTS()
		&& m_pServer->IsMultiCamera()
		&& !theApp.m_bFirstCam)
	{
		m_Resolution = RESOLUTION_LOW;
	}


	if (bJPEG2h263)
	{
		CIPCOutputVision* pOutput = m_pServer->GetOutput();
		DWORD dwBitrate = pOutput->GetBitrate();

		if (    dwBitrate<=64*1024
			|| !theApp.m_bFirstCam)
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
CCocoWindow::~CCocoWindow()
{
	m_csH263.Lock();
	CH263* pH263 = m_pH263;
	m_pH263 = NULL;
	m_csH263.Unlock();
	WK_DELETE(pH263);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCocoWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CCocoWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_DECODE_VIDEO, OnDecodeVideo)
	ON_COMMAND(ID_FPS_1_1, OnFps11)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_1, OnUpdateFps11)
	ON_COMMAND(ID_FPS_3_1, OnFps31)
	ON_UPDATE_COMMAND_UI(ID_FPS_3_1, OnUpdateFps31)
	ON_COMMAND(ID_FPS_6_1, OnFps61)
	ON_UPDATE_COMMAND_UI(ID_FPS_6_1, OnUpdateFps61)
	ON_COMMAND(ID_FPS_12_1, OnFps121)
	ON_UPDATE_COMMAND_UI(ID_FPS_12_1, OnUpdateFps121)
	ON_COMMAND(ID_FPS_9_1, OnFps91)
	ON_UPDATE_COMMAND_UI(ID_FPS_9_1, OnUpdateFps91)
//	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_COMMAND(ID_VIDEO_SAVE_REFERENCE, OnVideoSaveReference)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
int CCocoWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDisplayWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_bSoftwareDecoding)
	{
		m_csH263.Lock();
		m_pH263 = new CH263;
		m_pH263->SetCaps(H263_FIT,!m_b1to1); // With new H26x-Decoder from DResearch obsolete
		m_pH263->Init(this,WM_COMMAND,ID_DECODE_VIDEO,0);
		m_csH263.Unlock();
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCocoWindow::IsCoco()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSize CCocoWindow::GetPictureSize()
{
	CSize s;
	s.cx = s.cy = 0;

	if (IsSoftwareDecoding())
	{
		m_csH263.Lock();
		s = m_pH263->GetImageDims();
		m_csH263.Unlock();
	}
	else
	{
		m_csPictureRecord.Lock();
		if (m_pPictureRecord)
		{
			if (m_pPictureRecord->GetResolution() == RESOLUTION_HIGH)
			{
				s.cx = 352;
				s.cy = 288;
			}
			else if (m_pPictureRecord->GetResolution() == RESOLUTION_LOW)
			{
				s.cx = 176;
				s.cy = 144;
			}
		}
		m_csPictureRecord.Unlock();
	}

	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CCocoWindow::GetTitleText(CDC* pDC)
{
	if (m_bShowFPS)
	{
		CString s;
		double fps = (double)m_iFrames*1000.0/(double)(GetTimeSpan(m_dwTick));
		
		s.Format(_T("%02.03f %s"),fps,CDisplayWindow::GetTitleText(pDC));
		
		return s;
	}
	else
	{
		return CDisplayWindow::GetTitleText(pDC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::GetVideoClientRect(LPRECT lpRect)
{
	CDisplayWindow::GetVideoClientRect(lpRect);
	if (IsHardwareDecoding())
	{
		CRect rect(lpRect);
		if ( rect.Width() > theApp.m_MaxMegraRect.Width() )
		{
			int x;
			x = (rect.Width() - theApp.m_MaxMegraRect.Width()) / 2;
			lpRect->left = rect.left + x;
			lpRect->right = rect.right - x;
		}
		if ( rect.Height() > theApp.m_MaxMegraRect.Height())
		{
			int y;
			y = (rect.Height() - theApp.m_MaxMegraRect.Height()) / 2;
			lpRect->top = rect.top + y;
			lpRect->bottom = rect.bottom - y;
		}
	}
	else
	{
		//m_H263.GetPictureRect(lpRect);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::GetVideoRect(LPRECT lpRect)
{
	GetVideoClientRect(lpRect);
	ClientToScreen(lpRect);
	lpRect->left = max(lpRect->left,0);
	lpRect->top  = max(lpRect->top,0);
	lpRect->right = min(lpRect->right,GetSystemMetrics(SM_CXSCREEN));
	lpRect->bottom = min(lpRect->bottom,GetSystemMetrics(SM_CYSCREEN));
	m_pVisionView->ScreenToClient(lpRect);
} 
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::SetCIPCOutputWindow(LPRECT lpRect /*= NULL*/)
{
	if (IsHardwareDecoding())
	{
		CRect rect;

		if (lpRect)
		{
			rect = lpRect;
		}
		else
		{
			GetVideoRect(rect);
		}

		if (IsActive())
		{
			BOOL bLocal = m_pServer->IsLocal();
			CVisionDoc* pDoc = m_pVisionView->GetDocument();
			CIPCOutputVisionDecoder* pDecoder = pDoc->GetCIPCOutputVisionDecoder();
			if (pDecoder)
			{
				pDecoder->DoRequestSetOutputWindow(SECID_NO_GROUPID,rect,
					bLocal ? ODM_LOCAL_VIDEO : ODM_DECODE);
			}
			else
			{
				WK_TRACE_ERROR(_T("SetCIPCOutputWindow without decoder, camera %s\n"),(LPCTSTR)GetName());
			}
		}
		if (!IsActive())
		{
			m_bWasSomeTimesActive = FALSE;
			InvalidateRect(NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		if (IsHardwareDecoding())
		{
			DrawKeyColor(pDC);
			GetVideoClientRect(rect);
			rect.DeflateRect(CW_KEYCOLOR_OFFSET,CW_KEYCOLOR_OFFSET);
		}		    
		else
		{
			GetVideoClientRect(rect);
			m_csH263.Lock();
			m_pH263->OnDraw(pDC,rect, m_rcZoom);
			m_csH263.Unlock();
		}
	}
	CDisplayWindow::OnDraw(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	m_csH263.Lock();
	m_pH263->OnDraw(pDC,rcDest);
	m_csH263.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::DrawKeyColor(CDC* pDC)
{
	CRect rect;
	GetVideoClientRect(rect);
	rect.DeflateRect(CW_KEYCOLOR_OFFSET,CW_KEYCOLOR_OFFSET);
	pDC->FillRect(rect,&m_pVisionView->m_KeyBrush);
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::ClearWasSomeTimesActive()
{
	if (!IsActive() && IsHardwareDecoding())
	{
		m_bWasSomeTimesActive = FALSE;
		InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCocoWindow::PictureData(const CIPCPictureRecord &pict)
{
	if (pict.GetCompressionType() == COMPRESSION_H263)
	{
		m_csPictureRecord.Lock();
		m_stTimeStamp = pict.GetTimeStamp();
		m_bTimeValid = TRUE;
		m_dwLastPictureConfirmedTime = GetTickCount();
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = new CIPCPictureRecord(pict);
		m_csPictureRecord.Unlock();


		if (IsHardwareDecoding())
		{
			Sleep(0);
			PostMessage(WM_COMMAND,ID_DECODE_VIDEO);
		}
		else if(IsSoftwareDecoding())
		{
			m_csH263.Lock();
			m_pH263->Decode(pict);
			m_csH263.Unlock();
			Sleep(0);
			if (m_bShowFPS)
			{
				m_iFrames++;
			}
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
void CCocoWindow::OnDecodeVideo() 
{
	if (IsSoftwareDecoding())
	{
		if (m_pServer->IsDTS())
		{
			if (   m_pServer->IsDun() 
				|| m_pServer->IsTCPIP()
				|| (   m_pServer->IsISDN()
					&& m_Resolution == RESOLUTION_LOW)
				|| (   m_pServer->IsISDN()
					&& (m_pVisionView->GetDocument()->GetSequence()	== FALSE)
					)
				)
			{
				if (IsRequesting())
				{
					DoRequestPictures();
				}
			}
			InvalidateRect(NULL);
			Sleep(0);
		}
		else	// not DV
		{
			if (   m_bJPEG2h263 
				&& !IsStreaming())
			{
				if (IsRequesting())
				{
					DoRequestPictures();
				}
			}
			InvalidateRect(NULL);
			Sleep(0);
		}
		m_bWasSomeTimesActive = TRUE;
	}
	else
	{
		// Hardware decoding!
#ifndef _DTS
		CRect rect;
		GetVideoRect(rect);

		CVisionDoc* pDoc = m_pVisionView->GetDocument();
		CIPCOutputVisionDecoder* pDecoder = pDoc->GetCIPCOutputVisionDecoder();
		if (pDecoder)
		{
			m_csPictureRecord.Lock();
			pDecoder->DoRequestH263Decoding(SECID_NO_GROUPID,rect, *m_pPictureRecord,GetCamID().GetID());
			m_csPictureRecord.Unlock();
		}
		if (   !m_bWasSomeTimesActive 
			&& m_pVisionView->SetRequestedDisplayWindow(this))
		{
			Sleep(100); // ML Nach Umschaltung warten bis Daten decodiert sind
			m_pVisionView->SetDisplayOutputWindow(SDOW_DISPLAY);
			m_bWasSomeTimesActive = TRUE; 
		}
		
		// ML Redraw des ClientRect, damit die Uhrzeit auch erneuert wird.
		GetClientRect(rect);
		InvalidateRect(rect,FALSE);
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnFps11() 
{
	m_iQuality = 4;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCocoWindow::CanAdjustFPS(int fps)
{
	if (   m_pServer
		&& m_pServer->IsConnected())
	{
		if (m_bJPEG2h263)
		{
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
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
			return fps <= ((m_Resolution == RESOLUTION_HIGH) ? iMaxHigh : iMaxLow);
		}
	}
	return theApp.m_bCoCoQuality;
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnUpdateFps11(CCmdUI* pCmdUI) 
{
	BOOL bEnable = CanAdjustFPS(1);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && (m_iQuality == 4));
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnFps31() 
{
	m_iQuality = 3;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnUpdateFps31(CCmdUI* pCmdUI) 
{
	BOOL bEnable = CanAdjustFPS(3);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && (m_iQuality == 3));
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnFps61() 
{
	m_iQuality = 2;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnUpdateFps61(CCmdUI* pCmdUI) 
{
	BOOL bEnable = CanAdjustFPS(6);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && (m_iQuality == 2));
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnFps121() 
{
	m_iQuality = 0;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnUpdateFps121(CCmdUI* pCmdUI) 
{
	BOOL bEnable = CanAdjustFPS(12);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && (m_iQuality == 0));
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnFps91() 
{
	m_iQuality = 1;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnUpdateFps91(CCmdUI* pCmdUI) 
{
	BOOL bEnable = CanAdjustFPS(9);
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && (m_iQuality == 1));
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1;
	m_csH263.Lock();
	if (IsSoftwareDecoding())
	{
		m_pH263->SetCaps(H263_FIT,!m_b1to1);
	}
	m_csH263.Unlock();
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCocoWindow::OnVideoSaveReference() 
{
	if (IsSoftwareDecoding())
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
			CIPCInputVision* pInput = m_pServer->GetInput();
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
						dwLen = cfs.m_size;
						pBuffer = new BYTE[dwLen];

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
					file.Close();
					Sleep(10);
				}
				WK_DELETE_ARRAY(pBuffer);
			}
		}
	}
	CReferenceWindow* pRW = m_pVisionView->GetReferenceWindow(GetServerID(),
															  m_orOutput.GetName());
	if (WK_IS_WINDOW(pRW))
	{
		pRW->Reload();
	}
}
