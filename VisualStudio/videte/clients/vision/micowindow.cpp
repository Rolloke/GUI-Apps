/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MicoWindow.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/MicoWindow.cpp $
// CHECKIN:		$Date: 4.11.03 10:50 $
// VERSION:		$Revision: 76 $
// LAST CHANGE:	$Modtime: 4.11.03 10:43 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "Vision.h"

#include "MicoWindow.h"

#include "VisionDoc.h"
#include "VisionView.h"

#include "Server.h"
#include "CIPCOutputVision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Mico Resolutions
// RESOLUTION_LOW 384 x 288, 12:9
// RESOLUTION_MID 768 x 288, 24:9
// RESOLUTION_HIGH 768 x 576, 12:9

/////////////////////////////////////////////////////////////////////////////
// CMicoWindow
CMicoWindow::CMicoWindow(CServer* pServer, const CIPCOutputRecord &or) 
	: CDisplayWindow(pServer, or)
{
	m_pJpeg = NULL;
	m_iX = 4;
	m_iY = 3;

	if (pServer->IsLocal())
	{
		m_iQuality = 0;
	}
	else
	{
		m_iQuality = 1;
	}

	m_dwSum = 0;
	m_nCount = 0;
	m_iSoftFPS = 1;

	m_iMaxFPS = 12;
}
/////////////////////////////////////////////////////////////////////////////
CMicoWindow::~CMicoWindow()
{
	WK_DELETE(m_pJpeg);
}
/////////////////////////////////////////////////////////////////////////////
int CMicoWindow::GetRequestedFPS()
{
	if (m_iQuality < 0)
	{
		return -m_iQuality+1;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMicoWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CMicoWindow)
	ON_WM_CREATE()
	ON_COMMAND(ID_CHANGE_VIDEO, OnChangeVideo)
	ON_COMMAND(ID_FPS_1_1, OnFps11)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_1, OnUpdateFps11)
	ON_COMMAND(ID_FPS_1_2, OnFps12)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_2, OnUpdateFps12)
	ON_COMMAND(ID_FPS_1_3, OnFps13)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_3, OnUpdateFps13)
	ON_COMMAND(ID_FPS_1_4, OnFps14)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_4, OnUpdateFps14)
	ON_COMMAND(ID_FPS_1_5, OnFps15)
	ON_UPDATE_COMMAND_UI(ID_FPS_1_5, OnUpdateFps15)
	ON_COMMAND(ID_FPS_2_1, OnFps21)
	ON_UPDATE_COMMAND_UI(ID_FPS_2_1, OnUpdateFps21)
	ON_COMMAND(ID_FPS_3_1, OnFps31)
	ON_UPDATE_COMMAND_UI(ID_FPS_3_1, OnUpdateFps31)
	ON_COMMAND(ID_FPS_4_1, OnFps41)
	ON_UPDATE_COMMAND_UI(ID_FPS_4_1, OnUpdateFps41)
	ON_COMMAND(ID_FPS_6_1, OnFps61)
	ON_UPDATE_COMMAND_UI(ID_FPS_6_1, OnUpdateFps61)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FPS_12_1, OnFps121)
	ON_UPDATE_COMMAND_UI(ID_FPS_12_1, OnUpdateFps121)
	ON_COMMAND(ID_VIDEO_SAVE_REFERENCE, OnVideoSaveReference)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CMicoWindow::IsMico()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSize CMicoWindow::GetPictureSize()
{
	CSize s(0,0);
	m_csPictureRecord.Lock();
	if (   m_pJpeg 
		&& m_pJpeg->IsValid())
	{
		s = m_pJpeg->GetImageDims();
	}
	m_csPictureRecord.Unlock();
	return s;
}
////////////////////////////////////////////////////////////////////////////
int CMicoWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDisplayWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetVideoClientRect(rect);

	CIPCOutputVision* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		m_iMaxFPS = pOutput->GetMaxMicoFPS();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnChangeVideo() 
{
	if (!theApp.m_pDongle->AllowSoftDecodeMiCo())
	{
		WK_TRACE_ERROR(_T("jpeg decoding not allowed in dongle\n"));
		return;
	}
	m_bIsDecoding = TRUE;
	m_csPictureRecord.Lock();
	if (m_pPictureRecord)
	{
		if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG
			|| m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422)
		{

			if (m_pJpeg)
			{
				DWORD dwT = WK_GetTickCount();
				if (m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG)
				{
					if (!m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength()))
					{
						TRACE(_T("DecodeJpegFromMemory failed %s\n"),GetName());
					}
				}
				else if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422
						 || m_pPictureRecord->GetCompressionType() == COMPRESSION_RGB_24)
				{
					m_pJpeg->SetDIBData((LPBITMAPINFO)m_pPictureRecord->GetData());
				}

				m_bWasSomeTimesActive = TRUE;
				InvalidateRect(NULL,FALSE);

				DWORD dwDiff = WK_GetTimeSpan(dwT);
				// auf volle 10er aufrunden
				dwDiff = dwDiff + 10 - (dwDiff%10);
				dwDiff *= 2;

				int iSoftFPS;
				if (dwDiff>0)
				{
					iSoftFPS = 1000 / dwDiff;
				}
				else
				{
					// GetTickCount has a resolution of 10 ms, so the real time may be 9,999... ms (who knows...)
					// Therefor take 10 ms as guaranteed value => 1000/10
					iSoftFPS = 100;
				}

				if (m_nCount == 100)
				{
					m_dwSum = 0;
					m_nCount = 0;
				}
				else
				{
					m_dwSum += iSoftFPS; 
					m_nCount++;
					m_iSoftFPS = (m_dwSum / m_nCount);
					if (m_iSoftFPS>m_iMaxFPS)
					{
						m_iSoftFPS = m_iMaxFPS;
					}
					if (m_iSoftFPS<1)
					{
						m_iSoftFPS = 1;
					}
					if (m_iQuality<-m_iSoftFPS+1)
					{
						m_iQuality = -m_iSoftFPS+1;
						if (IsRequesting())
						{
							DoRequestPictures();
						}
					}
				}
			}

			if (   !IsStreaming()
				&& IsRequesting())
			{
				DoRequestPictures();
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("unknown picture record type in OnChangeVideo %s %s\n"),
				GetName(),NameOfEnum(m_pPictureRecord->GetCompressionType()));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("no picture record in OnChangeVideo %s\n"),GetName());
	}
	m_csPictureRecord.Unlock();
	m_bIsDecoding = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnDraw(CDC* pDC)
{
	if (theApp.m_pDongle->AllowSoftDecodeMiCo())
	{
		if (m_bWasSomeTimesActive)
		{
			CRect rect;
			GetVideoClientRect(rect);
			m_csTracker.Lock();
			if (   m_pJpeg
				&& m_pJpeg->IsValid())
			{
				m_pJpeg->OnDraw(pDC, rect, m_rcZoom);
			}
			else
			{
				WK_TRACE_ERROR(_T("jpeg object invalid %s\n"),GetName());
			}
			m_csTracker.Unlock();
		}
	}
	else
	{
		DrawFace(pDC);
	}
	CDisplayWindow::OnDraw(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	if (   m_pJpeg
		&& m_pJpeg->IsValid())
	{
		m_pJpeg->OnDraw(pDC,rcDest);
	}
}
/////////////////////////////////////////////////////////////////////////////
// iQuality = 0 high speed, low quality, high compression
// iQuality = 4 low speed, high quality, low compression
void CMicoWindow::OnFps11() 
{
	m_iQuality = 0;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps11(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_iQuality == 0);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps12() 
{
	m_iQuality = 1;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps12(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_iQuality == 1);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps13() 
{
	m_iQuality = 2;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps13(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_iQuality == 2);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps14() 
{
	m_iQuality = 3;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps14(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_iQuality == 3);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps15() 
{
	m_iQuality = 4;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps15(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_iQuality == 4);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps21() 
{
	m_iQuality = -1;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMicoWindow::CanAdjustFPS(int fps)
{
	int r = m_pVisionView->GetRequestedFPS(GetServerID());
	int c = GetRequestedFPS();
	int n = r - c + fps;

	return fps <=m_iSoftFPS && n <= m_iSoftFPS;
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps21(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanAdjustFPS(2));
	pCmdUI->SetCheck(m_iQuality == -1);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps31() 
{
	m_iQuality = -2;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps31(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanAdjustFPS(3));
	pCmdUI->SetCheck(m_iQuality == -2);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps41() 
{
	m_iQuality = -3;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps41(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanAdjustFPS(4));
	pCmdUI->SetCheck(m_iQuality == -3);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps61() 
{
	m_iQuality = -5;
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps61(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanAdjustFPS(6));
	pCmdUI->SetCheck(m_iQuality == -5);
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnFps121() 
{
#ifdef _DEBUG
	m_iQuality = -24;
#else
	m_iQuality = -11;
#endif
	DoRequestPictures();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnUpdateFps121(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanAdjustFPS(12));
	pCmdUI->SetCheck(m_iQuality == -11);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMicoWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1;
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CMicoWindow::OnVideoSaveReference() 
{
	CString sLocalPath = GetLocalReferenceFileName();
	CString sRemotePath = GetRemoteReferenceFileName();
	CFile file;
	BOOL bRet = TRUE;
	BOOL bOverWrite = TRUE;

	if (DoesFileExist(sRemotePath))
	{
		CString s,sMessage;
		s.LoadString(IDS_OVERWRITE_REFERENCE);
		sMessage.Format(s,GetName());
		bOverWrite = (IDYES==AfxMessageBox(sMessage,MB_ICONQUESTION|MB_YESNO));
	}
	if (bOverWrite)
	{
		// Local or remote write myself
		BYTE* pBuffer = NULL;
		DWORD dwLen = 0;

		if (m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG)
		{
			dwLen = m_pPictureRecord->GetDataLength();
			pBuffer = new BYTE[dwLen];
			CopyMemory(pBuffer,(BYTE*)m_pPictureRecord->GetData(),dwLen);
		}
		else if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422
			     || m_pPictureRecord->GetCompressionType() == COMPRESSION_RGB_24)
		{
			CJpeg jpeg1,jpeg2;
			HANDLE hDib;

			jpeg1.SetDIBData((LPBITMAPINFO)m_pPictureRecord->GetData());
			hDib = jpeg1.CreateDIB();
			dwLen = 128*1024;
			pBuffer = new BYTE[dwLen];
			jpeg2.EncodeJpegToMemory(hDib,pBuffer,dwLen);
			GlobalFree(hDib);

		}
		if (pBuffer && dwLen>0)
		{
			TRY
			{
				if (file.Open(sRemotePath,CFile::modeCreate|CFile::modeWrite))
				{
					file.Write(pBuffer,dwLen);
					file.Close();

				}
			}
			CATCH (CFileException, e)
			{
				bRet = FALSE;
			}
			END_CATCH

			WK_DELETE_ARRAY(pBuffer);
		}
	}
	
	if (!m_pServer->IsLocal())
	{
		// remote update file on server
		CIPCInputVision* pInput = m_pServer->GetInput();
		if (pInput)
		{
			pInput->DoRequestFileUpdate(RFU_FULLPATH,sLocalPath,
				(BYTE*)m_pPictureRecord->GetData(),
				m_pPictureRecord->GetDataLength(),
				FALSE);
		}
	}

	CReferenceWindow* pRW = m_pVisionView->GetReferenceWindow(GetServerID(),
															  m_orOutput.GetName());
	if (WK_IS_WINDOW(pRW))
	{
		pRW->Reload();
	}
}

void CMicoWindow::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow)
	{
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
			m_csPictureRecord.Lock();
			if (m_pPictureRecord)
			{
				if (m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG)
				{
					if (!m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength()))
					{
						TRACE(_T("DecodeJpegFromMemory failed %s\n"),GetName());
					}
				}
				else if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422
					|| m_pPictureRecord->GetCompressionType() == COMPRESSION_RGB_24)
				{
					m_pJpeg->SetDIBData((LPBITMAPINFO)m_pPictureRecord->GetData());
				}
			}
			m_csPictureRecord.Unlock();
		}
	}
	else
	{
		WK_DELETE(m_pJpeg);
	}
	CDisplayWindow::OnShowWindow(bShow, nStatus);
}
