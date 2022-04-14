/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndMico.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndMico.cpp $
// CHECKIN:		$Date: 3.07.06 16:26 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 3.07.06 16:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"

#include "WndMico.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "Server.h"
#include "CIPCOutputIdipClient.h"
#include "WndReference.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Mico Resolutions
// RESOLUTION_QCIF 384 x 288, 12:9
// RESOLUTION_CIF 768 x 288, 24:9
// RESOLUTION_2CIF 768 x 576, 12:9

IMPLEMENT_DYNAMIC(CWndMico, CWndLive)
/////////////////////////////////////////////////////////////////////////////
// CWndMico
CWndMico::CWndMico(CServer* pServer, const CIPCOutputRecord &or) 
	: CWndLive(pServer, or)
{
//	m_iX = 4;
//	m_iY = 3;

	m_dwSum = 0;
	m_nCount = 0;
	m_iSoftFPS = 1;

	if (pServer->IsLocal())
	{
		m_iQuality = 0;
	}
	else
	{
		m_iQuality = 1;
	}

	if (or.CameraIsYUV())
	{
		m_iMaxFPS = 25;	// presume 25 fps
		m_iSoftFPS = 0;
	}
	else
	{
		m_iMaxFPS = 12;
	}
	m_nType = WST_MICO;
}
/////////////////////////////////////////////////////////////////////////////
CWndMico::~CWndMico()
{
	WK_DELETE(m_pJpeg);
}
/////////////////////////////////////////////////////////////////////////////
int CWndMico::GetRequestedFPS()
{
	if (m_iQuality < 0)
	{
		return -m_iQuality+1;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::SetRequestedFPS(int nFps)
{
	m_iQuality = -nFps+1;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndMico, CWndLive)
	//{{AFX_MSG_MAP(CWndMico)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnFps_X_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FPS_1_1, ID_FPS_25_1, OnUpdateFps_X_X)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIDEO_SAVE_REFERENCE, OnVideoSaveReference)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_CHANGE_VIDEO, OnChangeVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CSize CWndMico::GetPictureSize()
{
	CSize s(0,0);
	m_csPictureRecord.Lock();
	if (   m_pJpeg 
		&& m_pJpeg->IsValid())
	{
		s = m_pJpeg->GetImageDims();
	}

	// AH Vorschlag (nicht getestet!): MPEG4Decoder-Implementierung
	//if (m_pMPEG4Decoder) 
	//{
	//	s = m_pMPEG4Decoder->GetImageDims();
	//}

	m_csPictureRecord.Unlock();
	return s;
}
////////////////////////////////////////////////////////////////////////////
int CWndMico::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndLive::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetVideoClientRect(rect);
	CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		if (IsYUV())
		{
			if (GetServer()->m_nMaxRequestedFps == 0)
			{
				GetServer()->m_nMaxRequestedFps = 100;
				CSecID id(GetCamID().GetGroupID(), SECID_NO_SUBID);
				pOutput->DoRequestGetValue(id, CSD_FPS, 0); // ask the unit for the real value
			}
		}
		else
		{
			m_iMaxFPS = pOutput->GetMaxMicoFPS();
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndMico::OnChangeVideo(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	m_bIsDecoding = TRUE;
	m_csPictureRecord.Lock();
	if (m_pPictureRecord)
	{
		if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_JPEG
			|| m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422)

			// AH Vorschlag (nicht getestet!): MPEG4Decoder-Implementierung
			// ||  m_pPictureRecord->GetCompressionType() == COMPRESSION_MPEG4)
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
					// AH Vorschlag (nicht getestet!): MPEG4Decoder-Implementierung
					// if(m_pJpeg)
					m_pJpeg->SetDIBData((LPBITMAPINFO)m_pPictureRecord->GetData());
				}

				m_bWasSomeTimesActive = TRUE;

				CRect rc;
				GetFrameRect(&rc);
				InvalidateRect(&rc,FALSE);
//				Invalidate();
				UpdateWindow();

				DWORD dwDiff = WK_GetTimeSpan(dwT);
				// auf volle 10er aufrunden
				dwDiff = dwDiff + 10 - (dwDiff%10);
				dwDiff *= 2;

				if (IsYUV())
				{
					if (m_iSoftFPS == 0)
					{
						m_iSoftFPS = 1;
						m_pViewIdipClient->CalculateRequestedFPS(GetServer());
					}
				}
				else
				{
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
	//					TRACE(_T("SoftFPS:%d = %d / %d\n"), m_iSoftFPS, m_dwSum, m_nCount);
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
							SetRequestedFPS(m_iSoftFPS);
							if (IsRequesting())
							{
								DoRequestPictures();
							}
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
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		GetVideoClientRect(rect);
		DrawCinema(pDC, &rect);
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
		if (   (m_wMD_X>0)
			&& (m_wMD_Y>0)
			&& IsRequesting())
		{
			DrawCross(pDC, m_wMD_X, m_wMD_Y, &rect);
		}
	}
	else
	{
		DrawFace(pDC);
	}
	DrawTitle(pDC);
	DrawError(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndMico::GetTitleText(CDC* pDC)
{
	if (theApp.m_bShowFPS)
	{
		CString s;
		s.Format(_T(", FPS:%d"), GetRequestedFPS());
		return CWndLive::GetTitleText(pDC) + s;
	}
	return CWndLive::GetTitleText(pDC);
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CWndMico::OnIdle()
{
/*
	int nFps = GetRequestedFPS();
	if (nFps > 2)
	{
		int nTimeout = 1000 / nFps;
		int nTimeSpan = GetTimeSpanSigned(m_dwLastRequest);
		if (nTimeSpan > nTimeout)
		{
			TRACE(_T("DoRequestPictures %d > %d\n"), nTimeSpan, nTimeout);
			DoRequestPictures();
		}
	}
*/
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CWndMico::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	if (   m_pJpeg
		&& m_pJpeg->IsValid())
	{
		m_pJpeg->OnDraw(pDC,rcDest);
	}

	// AH Vorschlag (nicht getestet!): MPEG4Decoder-Implementierung
	// if(m_pMPEG4Decoder)
	// {
	//     m_pMPEG4Decoder->OnDraw(pDC,rcDest);
	// }
}
/////////////////////////////////////////////////////////////////////////////
// iQuality = 0 high speed, low quality, high compression
// iQuality = 4 low speed, high quality, low compression
void CWndMico::OnFps_X_X(UINT nID) 
{
	switch (nID)
	{
		case ID_FPS_1_1:  m_iQuality = 0; break;
		case ID_FPS_1_2:  m_iQuality = 1; break;
		case ID_FPS_1_3:  m_iQuality = 2; break;
		case ID_FPS_1_4:  m_iQuality = 3; break;
		case ID_FPS_1_5:  m_iQuality = 4; break;
		case ID_FPS_2_1:  m_iQuality = -1; break;
		case ID_FPS_3_1:  m_iQuality = -2; break;
		case ID_FPS_4_1:  m_iQuality = -3; break;
		case ID_FPS_6_1:  m_iQuality = -5; break;
		case ID_FPS_12_1: m_iQuality = -11; break;
		case ID_FPS_25_1: m_iQuality = -24; break;
/*
#ifdef _DEBUG
		case ID_FPS_12_1: m_iQuality = -24; break;
#else
		case ID_FPS_12_1: m_iQuality = -11; break;
#endif
*/
		default: ASSERT(FALSE); return;
	}
	
	DoRequestPictures(RQ_FORCE_ONE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::OnUpdateFps_X_X(CCmdUI* pCmdUI) 
{
	int nQuality = -1;
	int nFps = 1, nMin = 0, nMax = 0;
	switch (pCmdUI->m_nID)
	{
		case ID_FPS_1_1:  nQuality = 0; break;
		case ID_FPS_1_2:  nQuality = 1; break;
		case ID_FPS_1_3:  nQuality = 2; break;
		case ID_FPS_1_4:  nQuality = 3; break;
		case ID_FPS_1_5:  nQuality = 4; break;
		case ID_FPS_2_1:  nQuality =  -1; nFps = 2; break;
		case ID_FPS_3_1:  nQuality =  -2; nFps = 3; break;
		case ID_FPS_4_1:  nQuality =  -3; nFps = 4; break;
		case ID_FPS_6_1:  nQuality =  -5; nFps = 5;  nMin =  5; nMax =  9; break;
		case ID_FPS_12_1: nQuality = -11; nFps = 12; nMin = 10; nMax = 19; break;
		case ID_FPS_25_1: nQuality = -24; nFps = 25; nMin = 20; nMax = 25; break;
/*
#ifdef _DEBUG
		case ID_FPS_12_1: nQuality = -24; nFps = 12; break;
#else
		case ID_FPS_12_1: nQuality = -11; nFps = 12; break;
#endif
*/
		default: ASSERT(FALSE); return;
	}

	if (IsYUV())
	{
		if (nQuality < 0)
		{
			int nReq = GetRequestedFPS();
			pCmdUI->Enable(CanAdjustFPS(nFps));
			if (nFps == nReq)
			{
				pCmdUI->SetCheck(TRUE);
			}
			else
			{
				pCmdUI->SetCheck(IsBetween(nReq, nMin, nMax));
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		if (nQuality < 0)
		{
			pCmdUI->Enable(CanAdjustFPS(nFps));
		}
		else
		{
			pCmdUI->Enable();
		}
		pCmdUI->SetCheck(m_iQuality == nQuality);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMico::CanAdjustFPS(int fps)
{
	int r = m_pViewIdipClient->GetRequestedFPS(GetServerID(), WST_MICO);
	int c = GetRequestedFPS();
	if (IsYUV())
	{
		r -= c;						// remove own fps value
		int nLeft = GetServer()->GetMaxFPS() - r;	// calculate possible fps left
		return fps <= nLeft;
	}
	else
	{
		int n = r - c + fps;
		return fps <= m_iSoftFPS && n <= m_iSoftFPS;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndMico::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::Set1to1(BOOL b1to1)
{
	CWndSmall::Set1to1(b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::OnVideoSaveReference() 
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
			/*if(dwLen > 100*1024*1024)
			{
				WK_TRACE(_T("#### DateLength: %d\n"), dwLen);
			}
			*/
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
		CIPCInputIdipClient* pInput = m_pServer->GetInput();
		if (pInput)
		{
			pInput->DoRequestFileUpdate(RFU_FULLPATH,sLocalPath,
				(BYTE*)m_pPictureRecord->GetData(),
				m_pPictureRecord->GetDataLength(),
				FALSE);
		}
	}

	CWndReference* pRW = m_pViewIdipClient->GetWndReference(GetServerID(), m_orOutput.GetName());
	if (WK_IS_WINDOW(pRW))
	{
		pRW->Reload();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndMico::OnShowWindow(BOOL bShow, UINT nStatus) 
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
	CWndLive::OnShowWindow(bShow, nStatus);
}
/////////////////////////////////////////////////////////////////////////////
