// WndLive.cpp : implementation file
//

#include "stdafx.h"

#include "IdipClient.h"

#include "MainFrm.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "WndLive.h"
#include "CIPCDatabaseIdipClient.h"
#include "DlgMDConfig.h"
#include "WndCoCo.h"
#include "WndMpeg4.h"
#include "DlgPTZConfig.h"
#include "DlgPTZVideo.h"
#include "DlgColorSettings.h"
#include "DlgComment.h"
#include "DlgBarIdipClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  COMMA_SEPARATED 2
IMPLEMENT_DYNAMIC(CWndLive, CWndImage)

/////////////////////////////////////////////////////////////////////////////
// CWndLive

CWndLive::CWndLive(CServer* pServer, const CIPCOutputRecord &or) 
	: CWndImage(pServer)
{
	m_orOutput  = or;
	m_hostID = pServer->GetHostID();
	m_pViewIdipClient = NULL;
	m_ccType = CCT_UNKNOWN;
	m_bTimeValid = FALSE;
	m_dwPTZFunctions = PTZF_DEFAULT;
	m_dwPTZFunctionsEx = 0;
	m_pPictureRecord = NULL;
	m_pPrintPictureRecord = NULL;
	m_iPictureRequests = 0;
	m_dwLastRequest = GetTickCount();
	
	m_bWasSomeTimesActive = FALSE;
	m_bIsRequesting = FALSE;
	m_bWaitingForRequest = FALSE;
	
	m_iWaitingForPicture = 0;
	m_iWaitingForPictureTreshold = 2;
	if (pServer->IsLowBandwidth())
	{
		m_iWaitingForPictureTreshold = 20;
	}
	
	m_bIPicture = FALSE;
//	m_iX = 12;
//	m_iY = 9;
	m_LastCameraControlCmd     = CCC_INVALID;
	m_LastCameraControlCmdStop = CCC_INVALID;
	m_dwLastPictureConfirmedTime = GetTickCount();

	m_Resolution = RESOLUTION_2CIF;
	m_bStoring   = pServer->IsAlarm();
	if (pServer->IsControl())
	{
		m_bStoring = pServer->IsControlSave();
	}

	m_bMarkedForShowColorDialog = FALSE;
	m_iContrast = 0;
	m_iBrightness = 0;
	m_iSaturation = 0;
	m_iQuality = 0;
	m_pDlgColorSettings = NULL;
	m_pDlgPTZVideo = NULL;
	m_pDlgPTZConfig = NULL;
	m_pDlgMDConfig = NULL;

	m_wMD_X = 0;
	m_wMD_Y = 0;

	m_rcZoom = CRect(0,0,0,0);
	m_iCountZooms = 0;
	m_dwPTZSpeed = 5;
	m_bIsDecoding = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CWndLive::~CWndLive()
{
	m_csPictureRecord.Lock();
	WK_DELETE(m_pPictureRecord);
	WK_DELETE(m_pPrintPictureRecord);
	m_csPictureRecord.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndLive::GetToolBarID()
{
	return IDR_LIVE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndLive, CWndImage)
	//{{AFX_MSG_MAP(CWndLive)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIDEO_FEIN, OnVideoFein)
	ON_COMMAND(ID_VIDEO_GROB, OnVideoGrob)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_FEIN, OnUpdateVideoFein)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_GROB, OnUpdateVideoGrob)
	ON_COMMAND(ID_VIDEO_SAVE, OnVideoSave)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE, OnUpdateVideoSave)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_SMALL_CONTEXT, OnSmallContext)
	ON_COMMAND(ID_VIDEO_CSB, OnVideoCsb)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_CSB, OnUpdateVideoCsb)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIDEO_PTZ, OnVideoSn)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_PTZ, OnUpdateVideoSn)
	ON_COMMAND_RANGE(ID_VIDEO_PTZ_1, ID_VIDEO_PTZ_9, OnVideoPTZ_X)
	ON_COMMAND_RANGE(ID_VIDEO_PTZ_HOME, ID_VIDEO_PTZ_HOME, OnVideoPTZ_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIDEO_PTZ_1, ID_VIDEO_PTZ_9, OnUpdateVideoPTZ_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIDEO_PTZ_HOME, ID_VIDEO_PTZ_HOME, OnUpdateVideoPTZ_X)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIDEO_PTZ_CONFIG, ID_VIDEO_PTZ_CONFIG, OnUpdateVideoPTZ_X)
	ON_COMMAND(ID_VIDEO_STOP, OnVideoStop)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIDEO_PTZ_CONFIG, OnVideoSnConfig)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE_REFERENCE, OnUpdateVideoSaveReference)
	ON_COMMAND(ID_VIDEO_SHOW_REFERENCE, OnVideoShowReference)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SHOW_REFERENCE, OnUpdateVideoShowReference)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut)
	ON_COMMAND(ID_SMALL_ORIGINAL, OnSmallOriginal)
	ON_UPDATE_COMMAND_UI(ID_SMALL_ORIGINAL, OnUpdateSmallOriginal)
	ON_COMMAND(ID_VIDEO_MASK, OnVideoMask)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_MASK, OnUpdateVideoMask)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_THIS, OnUpdateSmallClose)
	ON_COMMAND(ID_TOGGLE_RESOLUTION, OnToggleResolution)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	ON_COMMAND(ID_EDIT_COMMENT, OnEditComment)
	ON_MESSAGE(WM_USER,OnUser)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COMMENT, OnUpdateEditComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetOutputRecord(const CIPCOutputRecord &or)
{
	m_orOutput  = or;
	if (   m_pServer->IsConnectedLive()
		&& IsRequesting())
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		pOutput->RequestPictures(GetCamID(),this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetCIPCOutputWindow()
{
	// overwritten in derived class
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::Create(const RECT& rect, CViewIdipClient* pParentWnd)
{
	CWK_Profile wkp;
	CString sSection;
	sSection.Format(_T("%s\\%08lx\\%08lx\\%08lx"),
		IDIP_CLIENT_SETTINGS,
		theApp.m_pUser->GetID().GetID(),
		m_hostID.GetID(),
		m_orOutput.GetID().GetID());
	int iQuality = wkp.GetInt(sSection,_T("Quality"),m_iQuality);
	if (   GetType() == WST_COCO
		&& (m_pServer->GetConfirmedOutputOptions() & SCO_JPEG_AS_H263)
		)
	{
		if (iQuality<0)
		{
			iQuality = 2;
		}
	}
	if (GetType() == WST_MPEG4)
	{
		// ignore registry value for Tasha/Mpeg4
		iQuality = m_iQuality;
	}
	if (GetType() == WST_MICO)
	{
		iQuality = min(4,m_iQuality);
	}

	m_iQuality = iQuality;

	m_Resolution = (Resolution)wkp.GetInt(sSection,_T("Resolution"),m_Resolution);

	sSection.Format(_T("%s\\%08lx\\%08lx"),	IDIP_CLIENT_SETTINGS, m_hostID.GetID(), m_orOutput.GetID().GetID());
	m_dwMonitor = wkp.GetInt(sSection, USE_MONITOR, 0);

	m_pViewIdipClient = pParentWnd;
	m_b1to1 = m_pViewIdipClient->m_b1to1 ? true : false;
	//SetDlgCtrlID(m_orOutput.GetID().GetID());

	return CWnd::Create(SMALL_WINDOW_CLASS,m_orOutput.GetName(), 
						WS_CHILD | WS_BORDER | WS_VISIBLE | 
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						rect, pParentWnd,
						(int)m_orOutput.GetID().GetID()); 

}
/////////////////////////////////////////////////////////////////////////////
CSecID CWndLive::GetID()
{
	return m_orOutput.GetID();
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	// base class does nothing
}
/////////////////////////////////////////////////////////////////////////////
CString CWndLive::GetTitleText(CDC* pDC)
{
	CString sTitle, sTime, sDateTime, sServer;
	CSize size;
	CRect rect;
	int nWidth, nTextWidth;
	if (m_wndToolBar.GetSize().cy > 0)
	{
		m_wndToolBar.GetClientRect(rect);
		nWidth = rect.Width();
	}
	else if (m_wndToolBar.GetSize().cy < 0)
	{
		GetClientRect(rect);
		nWidth = rect.Width();
	}
	else
	{
		nWidth = 2048;
	}

	if (m_iWaitingForPicture > 1)
	{
		sTitle.LoadString(IDS_WAIT_FOR_IMAGE);
		return sTitle;
	}

	sTitle = m_orOutput.GetName();

	if (theApp.m_bShowTitleOfSmallWindows & SHOW_DEBUG_INFO)
	{
		CString sDebug, sComp, sBoard, sMon;
		sDebug.Format(_T("%08lx "),m_orOutput.GetID().GetID());

		m_csPictureRecord.Lock();
		if (m_pPictureRecord)
		{
			switch(m_pPictureRecord->GetCompressionType())
			{
			case COMPRESSION_JPEG:
				sComp = _T("J ");
				break;
			case COMPRESSION_H263:
				sComp = _T("H ");
				break;
			case COMPRESSION_YUV_422:
				sComp = _T("Y ");
				break;
			case COMPRESSION_MPEG4:
				sComp = _T("M ");
				break;
			}
		}
		m_csPictureRecord.Unlock();

		switch(GetType())
		{
			case WST_MPEG4: sBoard = _T("T "); break;
			case WST_COCO:  sBoard = _T("C "); break;
			case WST_MICO:  sBoard = _T("M "); break;
			case WST_PT:    sBoard = _T("P "); break;
		}

		if (theApp.GetMainFrame()->GetNoOfExternalFrames())
		{
			sMon.Format(_T("#%d "), LOWORD(m_dwMonitor));
		}

		sDebug = sMon + sDebug + sBoard + sComp;
		if (theApp.m_bShowTitleOfSmallWindows & SHOW_DEBUG_MINIMUM)
		{
			sTitle = sDebug;
			if (m_bTimeValid)
			{
				sTitle += m_stTimeStamp.GetTime();
			}
			return sTitle;
		}
		else
		{
			sTitle = sDebug + sTitle;
		}
	}

	size = pDC->GetOutputTextExtent(sTitle);
	nTextWidth = size.cx;
	if (nTextWidth < nWidth)
	{
		if (m_bTimeValid)
		{
			sTime      = _T(", ");
			sDateTime  = sTime;
			sTime     += m_stTimeStamp.GetTime();
			sDateTime += m_stTimeStamp.GetDateTime();
		}
		size = pDC->GetOutputTextExtent(sTime);
		if ((nTextWidth + size.cx)  < nWidth)
		{
			size = pDC->GetOutputTextExtent(sDateTime);
			nTextWidth += size.cx;
			if (nTextWidth < nWidth)
			{
				sTime.Empty();
				sServer = GetServerName() + _T(": ");
				size = pDC->GetOutputTextExtent(sServer);
				nTextWidth += size.cx;
				if (nTextWidth > nWidth)
				{
					sServer.Empty();
				}
			}
			else
			{
				sDateTime.Empty();
			}
		}
		else
		{
			sDateTime.Empty();
			sTime.Empty();
		}
	}
	sTitle = sServer + sTitle + sTime + sDateTime;

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		DrawCinema(pDC);
	}
	else
	{
		DrawFace(pDC);
	}
	if (   (m_wMD_X>0)
		&& (m_wMD_Y>0)
		&& IsRequesting()
		&& WasSometimesActive()
		)
	{
		DrawCross(pDC,m_wMD_X,m_wMD_Y);
	}
	DrawTitle(pDC);
	DrawError(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::DrawFace(CDC* pDC)
{
	CRect rect;
	GetFrameRect(rect);
	pDC->FillRect(rect,&m_pViewIdipClient->m_CinemaBrush);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::DrawError(CDC* pDC)
{
	if (m_iWaitingForPicture >= m_iWaitingForPictureTreshold+1)
	{
		CRect rc;
		GetFrameRect(&rc);
		CPen pen;
		CPen* pOldPen;
		
		pen.CreatePen(PS_SOLID,5,SKIN_COLOR_GREY_MEDIUM_LIGHT);
		pOldPen = pDC->SelectObject(&pen);
		
		pDC->MoveTo(rc.left,rc.top);
		pDC->LineTo(rc.right,rc.bottom);
		
		pDC->MoveTo(rc.right,rc.top);
		pDC->LineTo(rc.left,rc.bottom);
		
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::StorePicture(const CIPCPictureRecord &pict)
{
	if (IsStoring())
	{
		CIPCPictureRecord* pPicture = NULL;

		if (   pict.GetCompressionType() == COMPRESSION_YUV_422
			|| pict.GetCompressionType() == COMPRESSION_RGB_24)
		{
			CJpeg jpeg1,jpeg2;
			HANDLE hDib;
			BYTE* pBuffer = NULL;
			DWORD dwLen = 0;

			jpeg1.SetDIBData((LPBITMAPINFO)pict.GetData());
			hDib = jpeg1.CreateDIB();
			dwLen = 128*1024;
			pBuffer = new BYTE[dwLen];
			jpeg2.EncodeJpegToMemory(hDib,pBuffer,dwLen);
			GlobalFree(hDib);

			pPicture = new CIPCPictureRecord(pict.GetCIPC(),
											 pBuffer,dwLen,
											 GetCamID(),
											 pict.GetResolution(),
											 COMPRESSION_1,
											 COMPRESSION_JPEG,
											 pict.GetTimeStamp(),
											 pict.GetJobTime());
			WK_DELETE_ARRAY(pBuffer);
		}
		else
		{
			pPicture = new CIPCPictureRecord(pict);
		}

		if (   pPicture->GetPictureType()==SPT_FULL_PICTURE
			|| pPicture->GetPictureType()==SPT_GOP_PICTURE)
		{
			m_bIPicture = TRUE;
		}
		if (m_bIPicture)
		{
			CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
			CIPCDatabaseIdipClientLocal* pDatabase = pDoc->GetLocalDatabase();

			if (   pDatabase
				&& pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				CIPCFields fields;
				fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,
										 m_pServer->GetName(),32,'C'));
				fields.SafeAdd(new CIPCField(CIPCField::m_sfCaNm,
										 m_orOutput.GetName(),32,'C'));
				pDatabase->DoRequestNewSavePictureForHost(m_pServer->GetHostID(),
													      m_pServer->IsAlarm() || m_pServer->IsControl(),
													      FALSE,
													      *pPicture,
													      fields);
				pDoc->PictureStored();
			}
		}
		WK_DELETE(pPicture);
	} // IsStoring
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::PictureData(const CIPCPictureRecord &pict, CSecID idArchive, DWORD dwX, DWORD dwY)
{
	// Achtung in CIPCThread !
//	TRACE(_T("PictureData: %s\n"), GetName());
	if (!m_bIsDecoding)
	{
		m_csPictureRecord.Lock();
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = new CIPCPictureRecord(pict);
		m_stTimeStamp = m_pPictureRecord->GetTimeStamp();
		m_bTimeValid = TRUE;
		m_idArchive = idArchive;
		if (m_iPictureRequests>0)
		{
			m_iPictureRequests--;
		}
		m_csPictureRecord.Unlock();
		StorePicture(pict);
		m_dwLastPictureConfirmedTime = GetTickCount();
		if (dwX>0 && dwY>0)
		{
			SetMDValues((WORD)dwX, (WORD)dwY);
		}
		PostMessage(WM_CHANGE_VIDEO);
		m_iWaitingForPicture = 0;
		// message goes to OnChangeVideo
	}
	else
	{
		TRACE(_T("dropping frame %s\n"),GetName());
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
void CWndLive::RequestSetContrast(int contrast)
{
	CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		CString s;

		s.Format(_T("%d"),contrast*9+255);
		pOutput->DoRequestSetValue(GetCamID(),CSD_CONTRAST,s);
		if (theApp.m_pUser)
		{
			WK_TRACE_USER(_T("%s setzt Kontrast von %s auf %s neuer Wert %s\n"),
						  theApp.m_pUser->GetName(),
						  m_orOutput.GetName(),
						  m_pServer->GetName(),
						  s);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndLive::RequestSetBrightness(int brightness)
{
	CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
	
	if (pOutput)
	{
		CString s;

		s.Format(_T("%d"),brightness*4+128);
		pOutput->DoRequestSetValue(GetCamID(),CSD_BRIGHTNESS,s);
		if (theApp.m_pUser)
		{
			WK_TRACE_USER(_T("%s setzt Helligkeit von %s auf %s neuer Wert %s\n"),
						  theApp.m_pUser->GetName(),
						  m_orOutput.GetName(),
						  m_pServer->GetName(),
						  s);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndLive::RequestSetColor(int color)
{
	CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		CString s;

		s.Format(_T("%d"),color*9+255);
		pOutput->DoRequestSetValue(GetCamID(),CSD_SATURATION,s);
		if (theApp.m_pUser)
		{
			WK_TRACE_USER(_T("%s setzt Farbe von %s auf %s neuer Wert %s\n"),
						  theApp.m_pUser->GetName(),
						  m_orOutput.GetName(),
						  m_pServer->GetName(),
						  s);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndLive::RequestCSBUpdate()
{
	CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		pOutput->RequestCSB(GetCamID());
	}
}
////////////////////////////////////////////////////////////////////////////
void CWndLive::InitialRequest()
{
	if (m_pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		if (pOutput)
		{
			if (pOutput->IsPTZCamera(GetCamID()))
			{
				CIPCInputIdipClient* pInput = m_pServer->GetInput();
				if (pInput)
				{
					CString sKey;
					CSecID cid = pInput->GetCommID();
					if (cid!=SECID_NO_ID)
					{
						WK_TRACE(_T("RequestPTZFunctions %08x\n"), GetCamID().GetID());
						sKey.Format(CSD_CAM_CONTROL_TYPE_FMT, GetCamID().GetID());
						pInput->DoRequestGetValue(cid, sKey);
						sKey.Format(CSD_CAM_CONTROL_FKT_FMT, GetCamID().GetID());
						pInput->DoRequestGetValue(cid, sKey);
					}
					else
					{
						TRACE(_T("RequestPTZFunctions SECID_NO_ID\n"));
					}
				}
				else
				{
					WK_TRACE(_T("input not connected at RequestPTZFunctions\n"));
				}
			}
			pOutput->DoRequestGetValue(GetCamID(),CSD_MODE);

			if (pOutput->IsDirectShow(GetCamID()))
			{
				CString sKey;
				sKey.Format(CSD_CAM_CONTROL_TYPE_FMT,GetCamID().GetID());
				pOutput->DoRequestGetValue(GetCamID(),sKey);
				sKey.Format(CSD_CAM_CONTROL_FKT_FMT,GetCamID().GetID());
				pOutput->DoRequestGetValue(GetCamID(),sKey);
			}
		}
		ActivateCamera();

		SetStore(m_bStoring || m_pViewIdipClient->GetDocument()->GetStore());
	}
	else
	{
		WK_TRACE(_T("Server not connected at CWndLive::InitialRequest\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
int CWndLive::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndImage::OnCreate(lpCreateStruct) == -1)
		return -1;
	 //add stopp Button in ToolBar WndLive
	m_wndToolBar.GetToolBarCtrl().AddBitmap(1,IDB_SMALL_STOP);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::ActivateCamera()
{
	// FENSTER AKTIVIEREN
	// Auf jeden Fall neues Bild anfordern
	if (   m_pViewIdipClient->SetWndLiveRequested(this)
		|| CanMultipleRequests())
	{
		if (CanMultipleRequests())
		{
			m_bIsRequesting = TRUE;
		}
		PostMessage(WM_USER, WPARAM_EVENT_DO_REQUEST, RQ_FORCE_ONE);
	}
	else
	{
		m_bWaitingForRequest = TRUE;
		TRACE(_T("waiting for request %s\n"),GetName());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::DoRequestPictures(int iMax /*=RQ_DEFAULT*/)
{
//	TRACE(_T("DoRequestPictures %s\n"), GetName());
	if (m_pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		int iRequested = GetRequestedFPS();
		BOOL bLowBandwidth = m_pServer->IsLowBandwidth();
		m_csPictureRecord.Lock();
		if (iMax == RQ_FORCE_ONE)
		{
			if (m_iPictureRequests > 0)
			{
				m_iPictureRequests--;
			}
			iMax = m_iPictureRequests+1;
		}
		if (iMax == RQ_FORCE)
		{
			m_iPictureRequests = 0;
			iMax = RQ_DEFAULT;
		}
		if (iMax == RQ_DEFAULT)
		{
			iMax = bLowBandwidth ? 2 : (iRequested/4+1);
		}
		if (m_bWasSomeTimesActive && !IsCmdActive())
		{
			// langsam nicht zu viele Bilder
			if (m_iPictureRequests<iMax)
			{
				if (IsCmdActive())
				{
					TRACE(_T("RequestPictures %d %08lx %d fps\n"),
						m_iPictureRequests,GetCamID().GetID(),GetRequestedFPS());
				}
				pOutput->RequestPictures(GetCamID(),this, iRequested);
				m_iPictureRequests++;
			}
		}
		else
		{
			// schnell
			while (m_iPictureRequests<iMax)
			{
/*
				if (IsCmdActive())
				{
					TRACE(_T("RequestPictures %d %08lx %d fps\n"),
						m_iPictureRequests,GetCamID().GetID(),GetRequestedFPS());
				}
*/
//				TRACE(_T("#### schnell PicRequests: %d\n"), m_iPictureRequests);
				pOutput->RequestPictures(GetCamID(),this, iRequested);
				m_iPictureRequests++;
			}
		}
		m_bWaitingForRequest = FALSE;
		m_csPictureRecord.Unlock();
		m_dwLastRequest = GetTickCount();
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
#ifndef _DTS
BOOL CWndLive::DoRequestVideoOut()
{
	if (   m_pServer->IsLocal()
		&& m_pServer->IsConnectedLive()
	)
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		pOutput->DoRequestSetValue(GetCamID(), CSD_V_OUT, "");
		return TRUE;
	}
	return FALSE;
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CWndLive::DeactivateCamera()
{
//ML 29.9.99 Begin Insertation{
//	if (m_pServer->IsControlConnected())
//	{
//		m_pServer->GetControl()->ConfirmCamera(GetCamID(), Off);
//	}
//ML 29.9.99 End Insertation{
	if (m_pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pCIPC = m_pServer->GetOutput();
		BOOL bForceCocoStop = FALSE;
		if (   GetType() == WST_COCO
			&& m_pServer->IsDTS()
			&& m_Resolution == RESOLUTION_QCIF)
		{
			bForceCocoStop = TRUE;
		}
		pCIPC->UnrequestPictures(GetID(),bForceCocoStop);
	}
	else
	{
		TRACE(_T("deactivate camera server no longer connected\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::ClearWasSomeTimesActive()
{
	// base class does nothing
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetMode(const CString& sMode)
{
	m_sMode = sMode;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetMDValues(WORD wX, WORD wY)
{
	m_wMD_X = wX;
	m_wMD_Y = wY;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoFein() 
{
	m_rcZoom = CRect(0,0,0,0); // Zoom aus.

	m_Resolution = RESOLUTION_2CIF;
	if (IsStreaming())
	{
		DoRequestPictures(RQ_FORCE_ONE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoGrob() 
{
	m_rcZoom = CRect(0,0,0,0); // Zoom aus.

	BOOL bWasStreaming = IsStreaming();
	m_Resolution = RESOLUTION_QCIF;
	if (   bWasStreaming
		|| IsStreaming())
	{
		DoRequestPictures(RQ_FORCE_ONE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnToggleResolution()
{
	if (!m_pServer->IsDTS())
	{
		if (m_Resolution == RESOLUTION_QCIF)
		{
			OnVideoFein();
		}
		else if (m_Resolution == RESOLUTION_2CIF)
		{
			OnVideoGrob();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoFein(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Resolution == RESOLUTION_2CIF);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoGrob(CCmdUI* pCmdUI) 
{
	TRACE(_T("OnUpdateVideoGrob %x\n"), this);
	pCmdUI->SetCheck(m_Resolution == RESOLUTION_QCIF);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoSave() 
{
	SetStore(!m_bStoring);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoSave(CCmdUI* pCmdUI) 
{
	CIdipClientDoc* pDoc = m_pViewIdipClient->GetDocument();
	if (!theApp.m_bDisableSmall)
	{
		BOOL bEnable = pDoc->IsLocalDatabaseConnected();

		if(    bEnable
			&& theApp.IsReceiver()
			&& GetServer())
		{	
			//wenn Receiver dann pr¸fen, ob die Alarm anrufende Sendestation im Receiver
			//in der Gegenstationsliste eingetragen ist. Wenn ja, dann die Speicherbutton im
			//LiveFenstern enablen. Ansonsten disablen, da f¸r eine
			//nicht eingetragene Gegenstation kein Archiv angelegt werden kann.

			if (theApp.GetHosts().GetHost(GetServer()->GetHostID()) == NULL)
			{
				bEnable = FALSE;
			}
		}

		pCmdUI->Enable(bEnable);
	}
	else
	{
		pCmdUI->Enable(  pDoc->IsLocalDatabaseConnected()
					   &&!theApp.GetMainFrame()->IsFullScreen());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetVideoClientRect(rect);
	if (UseSetActiveArea())
	{
		rect.top += SET_ACTIVE_AREA_HEIGHT;
	}
	if (IsSn())
	{
		HCURSOR hC = theApp.m_hArrow;
		CRect rcZoom;
		CRgn  prof;
		int cx = 0, cy = 0;
		DWORD dwPTZSpeed = (DWORD)-1;
		if (nFlags & MK_LBUTTON)
		{
			dwPTZSpeed = m_dwPTZSpeed;
		}
		if (rect.PtInRect(point))
		{
			if (m_dwPTZFunctions & (PTZF_PAN))
			{
				cx = rect.Width() / 3;
			}
			if (m_dwPTZFunctions & (PTZF_TILT))
			{
				cy = rect.Height() / 3;
			}
			CPoint pts[4];
			// Zoom
			rcZoom = rect;
			rcZoom.DeflateRect(cx, cy);
			if (rcZoom.PtInRect(point))
			{
				// |-------------------|
				// |                   |
				// |   |-----------|   |
				// |   | Zoom| Zoom|   |
				// |   |  In | Out |   |
				// |   |-----------|   |
				// |                   |
				// |-------------------|
				if (point.x>((rect.left + rect.right)/2))
				{
					hC = theApp.m_hZoomOut;
					SetLastCameraControlCmd(CCC_ZOOM_OUT, CCC_ZOOM_STOP, dwPTZSpeed);
				}
				else
				{
					hC = theApp.m_hZoomIn;
					SetLastCameraControlCmd(CCC_ZOOM_IN, CCC_ZOOM_STOP, dwPTZSpeed);
				}
			}
			else if (m_dwPTZFunctions & PTZF_MOVE_DIAGONAL)
			{
				//|---------------------|
				//| UL | Tilt Up   | UR |
				//|----|-----------|----|
				//| PL |  (Zoom)   | PR |
				//|----|-----------|----|
				//| DL | Tilt Down | DR |
				//|---------------------|
				if (point.y < rcZoom.top)
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_MOVE_LEFT_UP, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hLeftTop;
					}
					else if(point.x > rcZoom.right)
					{
						SetLastCameraControlCmd(CCC_MOVE_RIGHT_UP, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hRightTop;
					}
					else
					{
						SetLastCameraControlCmd(CCC_TILT_UP, CCC_TILT_STOP, dwPTZSpeed);
						hC = theApp.m_hTiltUp;
					}
				}
				else if (point.y > rcZoom.bottom)
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_MOVE_LEFT_DOWN, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hLeftBottom;
					}
					else if(point.x > rcZoom.right)
					{
						SetLastCameraControlCmd(CCC_MOVE_RIGHT_DOWN, CCC_MOVE_STOP, dwPTZSpeed);
						hC = theApp.m_hRightBottom;
					}
					else
					{
						SetLastCameraControlCmd(CCC_TILT_DOWN, CCC_TILT_STOP, dwPTZSpeed);
						hC = theApp.m_hTiltDown;
					}
				}
				else
				{
					if (point.x < rcZoom.left)
					{
						SetLastCameraControlCmd(CCC_PAN_LEFT, CCC_PAN_STOP, dwPTZSpeed);
						hC = theApp.m_hPanLeft;
					}
					else
					{
						SetLastCameraControlCmd(CCC_PAN_RIGHT, CCC_PAN_STOP, dwPTZSpeed);
						hC = theApp.m_hPanRight;
					}
				}
			}
			else
			{
				// |-----------------|
				// |\   Tilt Up     /|
				// | \|-----------|/ |
				// |PL|  (Zoom)   |PR|
				// | /|-----------|\ |
				// |/   Tilt Down   \|
				// |-----------------|
				if (point.y < rcZoom.top)
				{
					// TiltUp
					pts[0] = rect.TopLeft();
					pts[1].x = rect.right;
					pts[1].y = rect.top;
					pts[2].x = rect.right-cx;
					pts[2].y = rect.top+cy;
					pts[3].x = rect.left+cx;
					pts[3].y = rect.top+cy;

					prof.CreatePolygonRgn(pts,4,WINDING);

					if (prof.PtInRegion(point))
					{
						prof.DeleteObject();
						hC = theApp.m_hTiltUp;
						SetLastCameraControlCmd(CCC_TILT_UP, CCC_TILT_STOP, dwPTZSpeed);
					}
				}
				else if (point.y > rcZoom.bottom)
				{
					// TiltDown
					pts[0].x = rect.left;
					pts[0].y = rect.bottom;
					pts[1].x = rect.left+cx;
					pts[1].y = rect.bottom-cy;
					pts[2].x = rect.right-cx;
					pts[2].y = rect.bottom-cy;
					pts[3] = rect.BottomRight();
					prof.CreatePolygonRgn(pts,4,WINDING);
					if (prof.PtInRegion(point))
					{
						prof.DeleteObject();
						hC = theApp.m_hTiltDown;
						SetLastCameraControlCmd(CCC_TILT_DOWN, CCC_TILT_STOP, dwPTZSpeed);
					}
				}

				if (hC == theApp.m_hArrow)
				{
					prof.DeleteObject();
					if (point.x < rcZoom.left)
					{
						// PanLeft
						pts[0] = rect.TopLeft();
						pts[1].x = rect.left+cx;
						pts[1].y = rect.top+cy;
						pts[2].x = rect.left+cx;
						pts[2].y = rect.bottom-cy;
						pts[3].x = rect.left;
						pts[3].y = rect.bottom;
						prof.CreatePolygonRgn(pts,4,WINDING);
						if (prof.PtInRegion(point))
						{
							prof.DeleteObject();
							hC = theApp.m_hPanLeft;
							SetLastCameraControlCmd(CCC_PAN_LEFT, CCC_PAN_STOP, dwPTZSpeed);
						}
					}
					else
					{
						// PanRight
	//					pts[0].x = rect.right;
	//					pts[0].y = rect.top;
	//					pts[1] = rect.BottomRight();
	//					pts[2].x = rect.right-cx;
	//					pts[2].y = rect.bottom-cy;
	//					pts[3].x = rect.right-cx;
	//					pts[3].y = rect.top+cy;
	//					prof.CreatePolygonRgn(pts,4,WINDING);
	//					if (prof.PtInRegion(point))
						{	// do not test the region, it must be here
							prof.DeleteObject();
							hC = theApp.m_hPanRight;
							SetLastCameraControlCmd(CCC_PAN_RIGHT, CCC_PAN_STOP, dwPTZSpeed);
						}
					}
				}
			}
		}
		SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);
		if (hC == theApp.m_hArrow)
		{
			SetLastCameraControlCmd(CCC_INVALID, CCC_INVALID, dwPTZSpeed);
		}
	}
	else
	{
		if(rect.PtInRect(point))
		{
			HCURSOR hC = theApp.LoadCursor(IDC_GLASS);
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);
		}
		else
		{
			HCURSOR hC = theApp.LoadStandardCursor(IDC_ARROW);
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);
		}
	}

	CWndImage::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
// force a second command to vary speed
void CWndLive::ResetLastCameraControlCmd()
{
	m_LastCameraControlCmd = CCC_INVALID;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetLastCameraControlCmd(CameraControlCmd cccC, CameraControlCmd cccStop, DWORD dwValue)
{
	CameraControlCmd cccOldC = m_LastCameraControlCmd;
	m_LastCameraControlCmd = cccC;
	if (dwValue != -1)
	{
		if (cccOldC != cccC)
		{
			if (cccC == CCC_INVALID)
			{
				RequestPTZ(m_LastCameraControlCmdStop, m_dwPTZSpeed, 0xFFFFFFFF, TRUE);
			}
			else
			{
				RequestPTZ(m_LastCameraControlCmd, dwValue, 0xFFFFFFFF, TRUE);
			}
		}
	}
	m_LastCameraControlCmdStop = cccStop;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcVideo;
	GetVideoClientRect(rcVideo);

	if (UseSetActiveArea())
	{
		rcVideo.top += SET_ACTIVE_AREA_HEIGHT;
	}

	if (rcVideo.PtInRect(point))
	{
		TRACE(_T("CWndLive::OnLButtonDown\n"));
		if (IsSn()) //when ccc_invalid was set the digital zoom on ptz was possible
		{
			if(m_LastCameraControlCmd != CCC_INVALID)
			{
				CSkinDialog::TrackMouseEvent(m_hWnd);// Use common controlled TrackMouseEvent function of CSkinDialog
				RequestPTZ(m_LastCameraControlCmd, m_dwPTZSpeed, 0xFFFFFFFF, TRUE);
			}
		}
		else if (   m_bWasSomeTimesActive
			     && !theApp.m_bDisableZoom)
		{
			// Liegt der Punkt im Videorect?
			if (rcVideo.PtInRect(point))
			{
				if (!ScrollWndLive())
				{
					if(m_iCountZooms < 3)
					{
						ZoomWndLive(point);
						m_iCountZooms++;
					}
					else
					{
						OnZoomOut();
					}
				}
			}
		}
		if (!m_pViewIdipClient->IsView1plus())
		{
			CWndImage::OnLButtonDown(nFlags,point);
		}
	}
	else
	{
		CWndImage::OnLButtonDown(nFlags,point);
	}

// darf erst bei OnTimer() aufgerufen werden, siehe CWndSmall()
// da dort SetBigSmallWindow() aufgerufen wird
//	m_pViewIdipClient->SetWndLiveRequested(this);

}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_LastCameraControlCmdStop != CCC_INVALID)
	{
		RequestPTZ(m_LastCameraControlCmdStop, 0, 0xFFFFFFFF, FALSE);
	}
	
	CWndImage::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndLive::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (m_LastCameraControlCmdStop != CCC_INVALID)
	{
		RequestPTZ(m_LastCameraControlCmdStop, 0, 0xFFFFFFFF, FALSE);
		m_LastCameraControlCmdStop = m_LastCameraControlCmd = CCC_INVALID;
	}
	// call static function in CSkinDialog::MouseLeave class to confirm CSkinDialog::TrackMouseEvent
	CSkinDialog::MouseLeave(m_hWnd); 
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::PopupMenu(CPoint pt)
{
	CMenu menu;
	CMenu* pFPSMenu;
	CSkinMenu*	pContextMenu;

	menu.LoadMenu(IDR_MENU_LIVE);
	pContextMenu = (CSkinMenu*) menu.GetSubMenu(SUB_MENU_LIVE_WINDOW);	// Kamera

	// RKE: remove the menu to save live pictures, the function are not yet implemented
	CMainFrame::DeleteSubMenu(pContextMenu, ID_FILE_HTML);
	if (m_pServer->IsDTS())
	{
		pContextMenu->DeleteMenu(ID_VIDEO_SAVE_REFERENCE,MF_BYCOMMAND);
		pContextMenu->DeleteMenu(ID_VIDEO_SHOW_REFERENCE,MF_BYCOMMAND);
		if (   !m_pServer->Can_DV_H263_QCIF()
			|| GetType() == WST_MICO
			|| GetType() == WST_MPEG4) // resolution depends on storing parameters
		{
			CMainFrame::DeleteSubMenu(pContextMenu, ID_VIDEO_FEIN);
		}
		pContextMenu->DeleteMenu(ID_VIDEO_CSB,MF_BYCOMMAND);
		pContextMenu->DeleteMenu(ID_SEPARATOR,MF_BYCOMMAND);
	}
	else
	{
		// RKE: disable resolution always ?
//		if (IsMpeg4())	// and has no storing process !!
//		{				// otherwise resolution can be changed
//			CMainFrame::DeleteSubMenu(pContextMenu, ID_VIDEO_FEIN);
//		}
		if (GetType() == WST_MICO)
		{
			CString sL;
			sL.LoadString(IDS_FPS);
			pFPSMenu = menu.GetSubMenu(SUB_MENU_MICO);	// Mico
			pContextMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu,sL);
		}
		else if (GetType() == WST_COCO)
		{
			CString sL;
			sL.LoadString(IDS_FPS);
			pFPSMenu = menu.GetSubMenu(SUB_MENU_COCO);	// Coco
			pContextMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu,sL);
		}
// TODO! RKE: evtl sp‰ter wieder einf¸gen. Im Moment die die Bildrate nicht einstellbar, d.h.
// sie ist aus Performancegr¸nden fest verdratet. Uwe weis bestimmt eine bessere Lˆsung.
//		else if (IsMpeg4())
//		{
//			CString sL;
//			sL.LoadString(IDS_FPS);
//			pFPSMenu = menu.GetSubMenu(SUB_MENU_TASHA);	// Tasha
//			pContextMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu,sL);
//		}
	}
	if (theApp.GetMainFrame()->GetNoOfExternalFrames()>0)
	{
		CString sL;
		pFPSMenu = menu.GetSubMenu(SUB_MENU_MONITOR);	// Monitor
		pContextMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
		int i, nCount = pFPSMenu->GetMenuItemCount();
		for (i=0; i<nCount; i++)
		{
			pFPSMenu->GetMenuString(i, sL, MF_BYPOSITION);
			pContextMenu->AppendMenu(MF_STRING, pFPSMenu->GetMenuItemID(i), sL);
		}
	}

	if (IsSn())
	{
		CString sL;
		// RKE: inserts only SN-config, the other fkts are implemented in the panel
		UINT nID, nTemp;
		if (m_dwPTZFunctions & PTZF_CONFIG)
		{
			nID = nTemp= ID_VIDEO_PTZ_CONFIG;
			((CSkinMenu*)&menu)->FindMenuString(nTemp, sL);
			pContextMenu->AppendMenu(MF_STRING, nID, sL);
		}
		if (m_dwPTZFunctionsEx & (PTZF_EX_AUXILIARY_OUT))
		{
			nID = nTemp= ID_VIDEO_PTZ;
			((CSkinMenu*)&menu)->FindMenuString(nTemp, sL);
			pContextMenu->AppendMenu(MF_STRING, nID, sL);
		}
// RKE: inserts the whole SN menu
//		sL.LoadString(IDS_PTZ);
//		pSNMenu = menu.GetSubMenu(SUB_MENU_PTZ);	// Schwenk/Neige
//		pContextMenu->AppendMenu(MF_POPUP,(UINT)pSNMenu->m_hMenu,sL);
	}

	COemGuiApi::DoUpdatePopupMenu(this,pContextMenu);

//	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y, this);	

// Die Beschreibungen f¸r die Statuszeile	m¸ssen an das MainWnd gehen
// Die Updatehandler werden trotzdem an das richtige (aktive) Fenster weitergeroutet
// auﬂerdem werden sie vorher durch die Funktion 
//	_T("COemGuiApi::DoUpdatePopupMenu(this,pContextMenu);") abgearbeitet.

	CSkinMenu*pMenuTemp = pContextMenu;
	pMenuTemp->ConvertMenu(TRUE);
  	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, AfxGetMainWnd());		
	pMenuTemp->ConvertMenu(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoCsb() 
{
	if (!m_pServer->IsDTS())
	{
		if (m_pDlgColorSettings)
		{
			m_pDlgColorSettings->DestroyWindow();
			m_pDlgColorSettings = NULL;
			m_bMarkedForShowColorDialog = FALSE;
		}
		else
		{
			// first request the actual parameters
			// then show the dialog
			CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
			if (pOutput)
			{
				pOutput->RequestCSB(GetCamID());
				m_bMarkedForShowColorDialog = TRUE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoCsb(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pDlgColorSettings!=NULL);
	pCmdUI->Enable(!m_pServer->IsDTS());
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CWndLive::OnUser(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case WPARAM_CONTRAST:
		m_iContrast = lParam;
		OnCSBChanged();
		break;
	case WPARAM_BRIGHTNESS:
		m_iBrightness = lParam;
		OnCSBChanged();
		break;
	case WPARAM_SATURATION:
		m_iSaturation = lParam;
		OnCSBChanged();
		break;
	case WPARAM_CCT:
		m_ccType = (CameraControlType)lParam;
		WK_TRACE(_T("%s CC Type %s\n"),GetName(),NameOfEnum(m_ccType));
		if (IsCmdActive())
		{
			theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow(this);
		}
		if (m_pDlgPTZVideo)
		{
			m_pDlgPTZVideo->SetPTZType(m_ccType);
		}
		break;
	case WPARAM_CCF_EX:
		m_dwPTZFunctionsEx = (DWORD)lParam;
		if (m_pDlgPTZVideo)
		{
			m_pDlgPTZVideo->SetPTZFunctionsEx(m_dwPTZFunctionsEx);
		}
		break;
	case WPARAM_CCFKT_NAME:
		if (lParam)
		{
			LPTSTR pszName = (LPTSTR)lParam;
			if (m_pDlgPTZVideo)
			{
				m_pDlgPTZVideo->SetFunctionName(pszName);
			}
			free((void*)pszName);
		}break;
	case WPARAM_CCF:
		m_dwPTZFunctions = (DWORD)lParam;
		WK_TRACE(_T("%s CC Func %08lx\n"),GetName(),m_dwPTZFunctions);
		if (IsCmdActive())
		{
			theApp.GetMainFrame()->GetDlgBarIdipClient()->SetPTZWindow(this);
		}
		break;
	case WPARAM_EVENT_DO_REQUEST:
		DoRequestPictures(lParam);
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnCSBChanged()
{
	if (m_pDlgColorSettings)
	{
		m_pDlgColorSettings->ActualizeSettings((m_iSaturation-255)/9,(m_iBrightness-128)/4,(m_iContrast-255)/9);
	}
	else
	{
		if (m_bMarkedForShowColorDialog)
		{
			m_bMarkedForShowColorDialog = FALSE;
			m_pDlgColorSettings = new CDlgColorSettings(this,(m_iSaturation-255)/9,(m_iBrightness-128)/4,(m_iContrast-255)/9);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnDestroy() 
{
	CWndImage::OnDestroy();
	CWK_Profile wkp;
	CString sSection;

	if (m_pDlgColorSettings)
	{
		m_pDlgColorSettings->DestroyWindow();
		m_pDlgColorSettings = NULL;
	}
	if (m_pDlgPTZVideo)
	{
		m_pDlgPTZVideo->DestroyWindow();
		m_pDlgPTZVideo = NULL;
	}
	if (m_pDlgPTZConfig)
	{
		m_pDlgPTZConfig->DestroyWindow();
		m_pDlgPTZConfig = NULL;
	}
	if (m_pDlgMDConfig)
	{
		m_pDlgMDConfig->DestroyWindow();
		m_pDlgMDConfig = NULL;
	}

	if (theApp.m_pUser)
	{
		sSection.Format(_T("%s\\%08lx\\%08lx\\%08lx"),
						IDIP_CLIENT_SETTINGS,
						theApp.m_pUser->GetID().GetID(),
						m_hostID.GetID(),
						m_orOutput.GetID().GetID());
		wkp.WriteInt(sSection,_T("Quality"),m_iQuality);
		wkp.WriteInt(sSection,_T("Resolution"),m_Resolution);

	}
	sSection.Format(_T("%s\\%08lx\\%08lx"), IDIP_CLIENT_SETTINGS, m_hostID.GetID(), m_orOutput.GetID().GetID());
	wkp.WriteInt(sSection, USE_MONITOR, m_dwMonitor);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoSn() 
{
	if (IsSn())
	{
		if (m_pDlgPTZVideo)
		{
			m_pDlgPTZVideo->DestroyWindow();
			m_pDlgPTZVideo = NULL;
		}
		else
		{
			CIPCInputIdipClient* pInput = m_pServer->GetInput();
			CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
			m_pDlgPTZVideo = new CDlgPTZVideo(pInput,pOutput,m_ccType, m_dwPTZFunctionsEx, this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoSn(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSn() && (m_pDlgPTZConfig==NULL);

	pCmdUI->Enable(bEnable);	
	if (IsSn())
	{
		pCmdUI->SetCheck(m_pDlgPTZVideo!=NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::RequestPTZ(CameraControlCmd ccc, DWORD dwValue, DWORD dwFlags, BOOL bOn /*=FALSE*/)
{
	if (m_dwPTZFunctions & dwFlags)
	{
		if (m_ccType==CCT_RELAY_PTZ)
		{
			CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
			if (pOutput)
			{
				pOutput->RequestCameraControl(m_LastCameraControlCmd,bOn);
			}
		}
		else
		{
			CIPCInputIdipClient* pInput = m_pServer->GetInput();
			if (pInput)
			{
				pInput->DoRequestCameraControl(pInput->GetCommID(),GetCamID(),ccc,dwValue);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoPTZ_X(UINT nID) 
{
	CameraControlCmd ccc = CCC_INVALID;
	DWORD dwFlags = 0;
	switch (nID)
	{
		case ID_VIDEO_PTZ_1:	ccc = CCC_POS_1;	dwFlags = PTZF_POS_1; break;
		case ID_VIDEO_PTZ_2:	ccc = CCC_POS_2;	dwFlags = PTZF_POS_2; break;
		case ID_VIDEO_PTZ_3:	ccc = CCC_POS_3;	dwFlags = PTZF_POS_3; break;
		case ID_VIDEO_PTZ_4:	ccc = CCC_POS_4;	dwFlags = PTZF_POS_4; break;
		case ID_VIDEO_PTZ_5:	ccc = CCC_POS_5;	dwFlags = PTZF_POS_5; break;
		case ID_VIDEO_PTZ_6:	ccc = CCC_POS_6;	dwFlags = PTZF_POS_6; break;
		case ID_VIDEO_PTZ_7:	ccc = CCC_POS_7;	dwFlags = PTZF_POS_7; break;
		case ID_VIDEO_PTZ_8:	ccc = CCC_POS_8;	dwFlags = PTZF_POS_8; break;
		case ID_VIDEO_PTZ_9:	ccc = CCC_POS_8;	dwFlags = PTZF_POS_9; break;
		case ID_VIDEO_PTZ_HOME: ccc = CCC_POS_HOME; dwFlags = PTZF_POS_HOME; break;
	}
	RequestPTZ(ccc, 0L, dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoPTZ_X(CCmdUI* pCmdUI) 
{
	DWORD dwFlags = 0;
	switch (pCmdUI->m_nID)
	{
		case ID_VIDEO_PTZ_1:		dwFlags = PTZF_POS_1;	break;
		case ID_VIDEO_PTZ_2:		dwFlags = PTZF_POS_2;	break;
		case ID_VIDEO_PTZ_3:		dwFlags = PTZF_POS_3;	break;
		case ID_VIDEO_PTZ_4:		dwFlags = PTZF_POS_4;	break;
		case ID_VIDEO_PTZ_5:		dwFlags = PTZF_POS_5;	break;
		case ID_VIDEO_PTZ_6:		dwFlags = PTZF_POS_6;	break;
		case ID_VIDEO_PTZ_7:		dwFlags = PTZF_POS_7;	break;
		case ID_VIDEO_PTZ_8:		dwFlags = PTZF_POS_8;	break;
		case ID_VIDEO_PTZ_9:		dwFlags = PTZF_POS_9;	break;
		case ID_VIDEO_PTZ_HOME:		dwFlags = PTZF_POS_HOME;break;
		case ID_VIDEO_PTZ_CONFIG:	dwFlags = PTZF_CONFIG;	break;
	}
	pCmdUI->Enable(m_dwPTZFunctions & dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoStop() 
{
	// attention delete's myself
	m_pViewIdipClient->DeleteWndSmall(GetServerID(),m_orOutput.GetID());
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWndImage::OnRButtonDown(nFlags, point);
	if (!theApp.m_bDisableRightClick || !theApp.GetMainFrame()->IsFullScreen())
	{
		ClientToScreen(&point);
		PopupMenu(point);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoSnConfig() 
{
	if (IsSn())
	{
		if (m_pDlgPTZConfig)
		{
			m_pDlgPTZConfig->DestroyWindow();
			m_pDlgPTZConfig = NULL;
		}
		else
		{
			CIPCInputIdipClient* pInput = m_pServer->GetInput();
			CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
			m_pDlgPTZConfig = new CDlgPTZConfig(pInput,pOutput,m_ccType, m_dwPTZFunctions, m_dwPTZFunctionsEx, this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::SetStore(BOOL bOn)
{
	m_bStoring = bOn;

	UINT nID;
	UINT nStyle; 
	int iImage;
	m_wndToolBar.GetButtonInfo(0,nID,nStyle,iImage);
	iImage = m_bStoring ? 3 : 0;
	m_wndToolBar.SetButtonInfo(0,nID,nStyle,iImage);

	// a new I-Frame for coco
	if (   WasSometimesActive()
		&& !IsStreaming())
	{
		DoRequestPictures();
	}
	m_wndToolBar.InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoSaveReference(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer 
				   && m_orOutput.IsReference()
		           && (!m_pServer->IsB3())
				   );
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoShowReference(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer 
				   && m_orOutput.IsReference()
		           && (!m_pServer->IsB3())
				   );
}
/////////////////////////////////////////////////////////////////////////////
CString CWndLive::GetRemoteReferenceFileName()
{
	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sCam,sPath;
	sHost.Format(_T("%08lx"),m_pServer->GetHostID());
	sCam.Format(_T("%08lx"),GetCamID().GetID());
	sPath = sDir + '\\' + sHost + '\\' + sCam + _T(".jpg");
	return sPath;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndLive::GetLocalReferenceFileName()
{
	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sCam,sPath;
	sHost.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sCam.Format(_T("%08lx"),GetCamID().GetID());
	sPath = sDir + '\\' + sHost + '\\' + sCam + _T(".jpg");
	return sPath;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoShowReference() 
{
//	m_pViewIdipClient->AddHtmlWindow(m_pServer,GetCamID(),m_orOutput.GetName());
	m_pViewIdipClient->AddReferenceWindow(m_pServer,GetCamID(),m_orOutput.GetName());
}

/////////////////////////////////////////////////////////////////////////////
void CWndLive::ZoomWndLive(const CPoint &point)
{
	if (m_pPictureRecord)
	{
		CRect rcVideo,rcTrack,rcZoom;
		int		nX1, nY1, nX2, nY2;
		CSize s = GetPictureSize();

		GetVideoClientRect(rcVideo);

		if (m_rcZoom.IsRectEmpty())
		{
			m_rcZoom = CRect(0,0, s.cx, s.cy);
		}

		rcTrack.left	= point.x	- rcVideo.Width()  / 4;
		rcTrack.top		= point.y	- rcVideo.Height() / 4;
		rcTrack.right	= point.x	+ rcVideo.Width()  / 4;
		rcTrack.bottom	= point.y	+ rcVideo.Height() / 4;

		int nXOff = 0;
		int nYOff = 0;

		// Liegt das Rechteck auﬂerhalb des sichtbaren Bereiches?
		if (rcTrack.left < rcVideo.left)
			nXOff = rcVideo.left - rcTrack.left;
		else
			nXOff = 0;

		if (rcTrack.top < rcVideo.top)
			nYOff = rcVideo.top - rcTrack.top;
		else
			nYOff = 0;
		rcTrack.OffsetRect(nXOff, nYOff);		
	
		if (rcTrack.right > rcVideo.right)
			nXOff = rcTrack.right - rcVideo.right;
		else
			nXOff = 0;

		if (rcTrack.bottom > rcVideo.bottom)
			nYOff = rcTrack.bottom - rcVideo.bottom;
		else
			nYOff = 0;
		rcTrack.OffsetRect(-nXOff, -nYOff);			

		rcZoom.left		= rcTrack.left	- rcVideo.left;
		rcZoom.right	= rcTrack.right	- rcVideo.left;
		rcZoom.top		= rcTrack.top	- rcVideo.top;
		rcZoom.bottom	= rcTrack.bottom- rcVideo.top;
		rcZoom.NormalizeRect();
		rcZoom.IntersectRect(rcZoom, CRect(0,0, rcVideo.Width(), rcVideo.Height()));
		
		nX1 = m_rcZoom.left + rcZoom.left	* m_rcZoom.Width()  / rcVideo.Width();
		nY1	= m_rcZoom.top	+ rcZoom.top	* m_rcZoom.Height() / rcVideo.Height(); 
		nX2	= m_rcZoom.left + rcZoom.right	* m_rcZoom.Width()  / rcVideo.Width();
		nY2	= m_rcZoom.top	+ rcZoom.bottom	* m_rcZoom.Height() / rcVideo.Height(); 
		m_rcZoom = CRect(nX1, nY1, nX2, nY2);
		
		RedrawWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::ScrollWndLive()
{
	if (m_rcZoom.IsRectEmpty())
	{
		return FALSE;
	}
	else
	{
		CPoint	StartPoint;					 
		CPoint	CurrentPoint;					 
		CRect	rcSavedZoom;
		rcSavedZoom = m_rcZoom;

		SIZE size = GetPictureSize(); 
		CRect rcImage = CRect(0,0, size.cx, size.cy);

		ShowCursor(FALSE);
		GetCursorPos(&StartPoint);

		while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
		{
			GetCursorPos(&CurrentPoint);

			m_rcZoom = rcSavedZoom;
			m_rcZoom.left	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.right	+= StartPoint.x - CurrentPoint.x;
			m_rcZoom.top	+= StartPoint.y - CurrentPoint.y;
			m_rcZoom.bottom	+= StartPoint.y - CurrentPoint.y;
			
			if (m_rcZoom.left<0)
			{
				m_rcZoom.right += abs(m_rcZoom.left);
				m_rcZoom.left	= 0;
			}
			if (m_rcZoom.top<0)
			{
				m_rcZoom.bottom += abs(m_rcZoom.top);
				m_rcZoom.top = 0;
			}
			if (m_rcZoom.right>rcImage.right)
			{
				m_rcZoom.left -= (m_rcZoom.right-rcImage.right);
				m_rcZoom.right = rcImage.right;
			}
			if (m_rcZoom.bottom>rcImage.bottom)
			{
				m_rcZoom.top -= (m_rcZoom.bottom-rcImage.bottom);
				m_rcZoom.bottom  = rcImage.bottom;
			}

			CRect rc;
			GetFrameRect(&rc);
			InvalidateRect(&rc,FALSE);
//			Invalidate();
			UpdateWindow();
		}
		ShowCursor(TRUE);

		return (StartPoint != CurrentPoint);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnZoomOut() 
{
	m_iCountZooms = 0;
	m_rcZoom.left = m_rcZoom.top = m_rcZoom.right = m_rcZoom.bottom = 0;
	CRect rc;
	GetFrameRect(&rc);
	InvalidateRect(&rc,FALSE);
//	Invalidate();
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_rcZoom.IsRectEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnSmallOriginal() 
{
	if (m_pViewIdipClient->GetNrOfLiveWindows() == 1)
	{
		m_pViewIdipClient->OnViewOriginal();
	}
	else
	{
		m_b1to1 = !m_b1to1;
		Set1to1(m_b1to1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateSmallOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::CheckVideo(DWORD dwCurrentTime) 
{
	BOOL bWaiting = FALSE;
	if (IsRequesting())
	{
		// wait timeout for picture requests
		int nVideoTimeout = RQP_TIMEOUT;
		if (GetServer()->m_nRequestState != REQUEST_STATE_NORMAL)
		{
			nVideoTimeout += 2000;
		}
		if (GetType() == WST_MICO && m_iQuality > 0)
		{
			// increase with picture wait timeout
			nVideoTimeout += m_iQuality * 1000;
		}
		int nTimeSpan = GetTimeSpan(m_dwLastPictureConfirmedTime, dwCurrentTime);
		if (nTimeSpan > 0)	// Timer ‹berlauf nicht ber¸cksichtigen
		{
//			TRACE(_T("%d ms, %d\n"), dwTimeSpan, dwPictures);
			if (nTimeSpan > nVideoTimeout)
			{
				m_iWaitingForPicture++;
				if (m_iWaitingForPicture >= m_iWaitingForPictureTreshold)
				{
					m_bUpdateTitle = true;
					InvalidateRect(NULL);
				}
				if (GetServer()->m_nRequestState == REQUEST_STATE_RECOVER)
				{
//					WK_TRACE_WARNING(_T("%s VideoTimeout %lu ms\n"), GetName(), nTimeSpan);
					return TRUE;
				}
				WK_TRACE_WARNING(_T("%s VideoTimeout %lu ms, request new picture\n"), GetName(), nTimeSpan);
				DoRequestPictures(RQ_FORCE_ONE);
				bWaiting = TRUE;
			}
		}
		else if (nTimeSpan < 0)
		{
			TRACE(_T("Timer overflow: %x\n"), nTimeSpan);
		}
	}
	if (m_pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pOutput = m_pServer->GetOutput();
		if (pOutput)
		{
			if (pOutput->IsPTZCamera(GetCamID()) && m_ccType == CCT_UNKNOWN)
			{
				InitialRequest();
			}
		}
	}
	return bWaiting;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::CheckRemotePictureTime(CSystemTime &stLocal)
{
	if (m_bTimeValid)
	{
		int nOffset = GetServer()->m_nRemoteTimeOffset;
		if (INVALID_TIME_OFFSET == nOffset)
		{
			return TRUE;
		}
		CSystemTime stRemote(stLocal);
		if (nOffset > 0)
		{
			LARGE_INTEGER li; // in 100 NanoSekunden
			li.QuadPart = (__int64)10000000 * (__int64)nOffset;
			CSystemTime stOffset(li);
			stRemote = stLocal + stOffset;
		}
		else if (nOffset < 0)
		{
			LARGE_INTEGER li; // in 100 NanoSekunden
			li.QuadPart = (__int64)10000000 * (__int64)-nOffset;
			CSystemTime stOffset(li);
			stRemote = stLocal - stOffset;
		}
		if (stRemote > m_stTimeStamp)
		{
			CSystemTime sSpan = stRemote - m_stTimeStamp;
			signed short nOffset = sSpan.GetSecond() + sSpan.GetMinute()*60 + sSpan.GetHour()*3600;
//			TRACE(_T("Offset: %d ms\n"), nOffset*1000+sSpan.GetMilliseconds());
			GetServer()->m_nMaxPictureTimeOffset = max(nOffset, GetServer()->m_nMaxPictureTimeOffset);
			return nOffset > 2 ? FALSE : TRUE;
		}
/*		else
		{
			CSystemTime sSpan = m_stTimeStamp - stRemote;
			int nOffset = sSpan.GetSecond() + sSpan.GetMinute()*60 + sSpan.GetHour()*3600;
			TRACE(_T("Offset: -%d ms\n"), nOffset*1000+sSpan.GetMilliseconds());
		}
*/
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnIndicateError(DWORD dwCmd,CIPCError error,int iErrorCode)
{
	switch (dwCmd)
	{
	case CIPC_OUTPUT_REQUEST_GET_MASK:
	case CIPC_OUTPUT_REQUEST_SET_MASK:
		if (m_pDlgMDConfig)
		{
			m_pDlgMDConfig->OnIndicateError(dwCmd,error,iErrorCode);
		}
		break;
	default:
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::CanMultipleRequests()
{
	if (GetServer())
	{
		if (GetServer()->IsDTS())
		{
			if (    GetServer()->IsISDN()
				||  GetServer()->IsDun()
				||  (   GetServer()->IsTCPIP()
					&&	(GetType() == WST_COCO))
				)
			{
				if (GetResolution() == RESOLUTION_2CIF)
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		else
		{
			if (GetServer()->IsMultiCamera())
			{
				if (GetType() == WST_COCO)
				{
					CWndCoco* pCW = (CWndCoco*)this;
					return pCW->IsJPEG2H263();
				}
				else
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::SetActiveRequested(BOOL bActive/*=TRUE*/)
{
	if (m_bIsRequesting != bActive)
	{
		if (bActive == FALSE)
		{
			if (CanMultipleRequests())
			{
				// ich will nicht deaktiviert werden
//				TRACE(_T("%s cannot be deactivated by multiple requests allowed\n"),GetName());
				return FALSE;
			}
		}
		m_bIsRequesting = bActive;

		CRect rc;
		GetFrameRect(&rc);
		InvalidateRect(&rc,FALSE);
//		Invalidate();
		UpdateWindow();

//		TRACE(_T("%d %08lx is %s request \n"),GetServerID(),GetID().GetID(),m_bIsRequesting ? _T("active") : _T("not active"));

		if (   IsRequesting() 
			|| IsWaitingForRequest())
		{
			m_iWaitingForPicture++;
			DoRequestPictures();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndLive::OnVideoMask() 
{
	if (m_pDlgMDConfig)
	{
		m_pDlgMDConfig->BringWindowToTop();
	}
	else
	{
		m_pDlgMDConfig = new CDlgMDConfig(GetID(),this);
	}	
}
void CWndLive::ConfirmMaskSensitivity(int nMask, int nSensitivity)
{
	if (m_pDlgMDConfig)
	{
		m_pDlgMDConfig->ConfirmMaskSensitivity(nMask, nSensitivity);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateVideoMask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer!=NULL 
				   && m_pServer->CanActivityMask()
				   && m_sMode == CSD_MD);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndLive::ConfirmGetMask(const CIPCActivityMask& mask)
{
	if (m_pDlgMDConfig)
	{
		m_pDlgMDConfig->ConfirmGetMask(GetID(),mask);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm get mask no MD Config Dialog %08lx\n"),GetID().GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndLive::ConfirmSetMask(MaskType type)
{
	if (m_pDlgMDConfig)
	{
		m_pDlgMDConfig->ConfirmSetMask(GetID(),type);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm set mask no MD Config Dialog %08lx\n"),GetID().GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
int CWndLive::GetRequestedFPS()
{
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::CanPrint()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::IsStreaming()
{
	if (m_pServer->IsDTS())
	{
		return FALSE;
	}
	else
	{
		// idip
		return (m_pServer->GetConfirmedOutputOptions() & SCO_NO_STREAM) == 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::IsWndLive()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::IsWaitingForRequest()
{
	return m_bWaitingForRequest;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::IsRequesting()
{
	return m_bIsRequesting;
}
/////////////////////////////////////////////////////////////////////////////
 CH263* CWndLive::GetH263()
 {
	 return NULL;
 }
///////////////////////////////////////////////////////////////////
BOOL CWndLive::operator < (CWndLive & second)
{
	if (m_pViewIdipClient->GetWndLiveCmdActive() == this)
	{
		return FALSE;
	}
	
	if (CanMultipleRequests())
	{
		return GetID().GetID() < second.GetID().GetID();
	}

	if (m_pPictureRecord)
	{
		if (second.m_pPictureRecord)
		{
			CSystemTime t1,t2;
			t1 = m_pPictureRecord->GetTimeStamp();
			t2 = second.m_pPictureRecord->GetTimeStamp();
			return  t1 < t2;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}
///////////////////////////////////////////////////////////////////
void CWndLive::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_uTimerID)
	{
		CWndImage::OnTimer(nIDEvent);
		m_pViewIdipClient->SetWndLiveRequested(this);
	}
}
///////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!theApp.GetMainFrame()->IsFullScreen());
	}
}
///////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!theApp.GetMainFrame()->IsFullScreen());
	}
}
///////////////////////////////////////////////////////////////////
int CWndLive::PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim /*= FALSE */)
{
	int ret = 0;
	if (GetType() == WST_MPEG4)
	{
		if (!bUseDim)
		{
			rect.bottom = rect.top + (rect.Width() * 9) / 12;
		}
		int nWait = 0;
		while (m_pPrintPictureRecord == NULL && pDC->IsPrinting())
		{
			Sleep(100);
			if (nWait++ == 50)
			{
				break;
			}
		}
		if(m_pPrintPictureRecord)
		{
			CMPEG4Decoder decTemp;
            decTemp.Init(this,0,0,0);
			decTemp.DecodeMpeg4FromMemory((BYTE*)m_pPrintPictureRecord->GetData(), m_pPrintPictureRecord->GetDataLength());
			decTemp.OnDraw(pDC, rect, m_rcZoom);
		}
		ret = rect.Height();
	}
	else if (m_pPictureRecord)
	{
		if (   m_pPictureRecord->GetCompressionType() == COMPRESSION_YUV_422
			|| m_pPictureRecord->GetCompressionType() == COMPRESSION_RGB_24)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}
			if(m_pPrintPictureRecord)
			{
				CJpeg Jpeg;
				if(Jpeg.SetDIBData((LPBITMAPINFO)m_pPrintPictureRecord->GetData()))
				{
					Jpeg.OnDraw(pDC, rect, m_rcZoom);
				}
			}
			ret = rect.Height();
		}
		if (m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}
			if(m_pPrintPictureRecord)
			{	
				CJpeg Jpeg;
				if(Jpeg.DecodeJpegFromMemory((BYTE*)m_pPrintPictureRecord->GetData(),
													m_pPrintPictureRecord->GetDataLength()))
				{
					Jpeg.OnDraw(pDC, rect, m_rcZoom);
				}
			}
			ret = rect.Height();
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 11;
			}
			TRACE(_T("Print H.263 Rect=%d,%d,%d,%d Zoom=%d,%d,%d,%d\n"),
				rect.left,rect.top,rect.right,rect.bottom,
				m_rcZoom.left,m_rcZoom.top,m_rcZoom.right,m_rcZoom.bottom);

			if(m_pPrintPictureRecord)
			{
				if(GetType() == WST_COCO)
				{
					CH263* pH263 = NULL;
					pH263 = GetH263();
					pH263->OnDraw(pDC,rect, m_rcZoom);
				}
			}
			ret = rect.Height();
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE)
		{
			if (!bUseDim)
			{
				rect.bottom = rect.top + (rect.Width() * 9) / 12;
			}

			CPTDecoder PTDecoder;
			CDib dib(PTDecoder.GetBitmapHandle());
			dib.OnDraw(pDC,rect, m_rcZoom);
			ret = rect.Height();
		}
#endif
	}
	return ret;

}

/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect ir;
	CRect pr;
	CRect rect(pInfo->m_rectDraw);
	int iSpace = pDC->GetDeviceCaps(LOGPIXELSX) / 10;

	int iX = rect.Width()/25;
	int iY = rect.Height()/25;

	if (rect.Width()>rect.Height())
	{
		// Landscape = Videoprinter
		CFont font;
		int h;
		ir = rect;
		ir.bottom = ir.top + iY * 2;
		font.CreatePointFont(100, GetVarFontFaceName(), pDC);
		h = PrintInfoRectLandscape(pDC,ir,&font);
		pr = rect;
		pr.top += h;
		PrintPicture(pDC,pr);
	}
	else
	{
		rect.DeflateRect(iX,iY);
		// Portraet = normaler Drucker
		CFont font;
		ir = rect;
		ir.bottom = ir.top + iY * 2 - iSpace;
		font.CreatePointFont(120, GetVarFontFaceName(), pDC);
		PrintTitleRectPortrat(pDC,ir,&font);
		pr = rect;
		pr.top = ir.bottom + iSpace;
		font.DeleteObject();

		if (IsWndLive())
		{
			CString sFormatPicture, sFormatData;
			CIPCFields fields;
			int h = PrintPicture(pDC,pr);
			CFont font;
			ir = rect;
			ir.top = pr.top + h + iSpace;
			font.CreatePointFont(DW_INFO_FONT_SIZE, GetVarFontFaceName(), pDC);

			if(m_pPrintPictureRecord)
			{
				fields.FromString(m_pPrintPictureRecord->GetInfoString());
				sFormatPicture = FormatPicture(m_pPrintPictureRecord, TRUE);
			}
			else
			{
				TRACE(_T("m_pPrintPictureRecord nicht da\n"));
			}
			sFormatData    = FormatData(fields, _T("\n"),_T(":\t"));
//				TRACE(_T("FormatData: %s, Comment: %s\n"), sFormatData, m_sComment);
			PrintInfoRectPortrat(pDC, ir, &font, iSpace, sFormatPicture, sFormatData, m_sComment);
			font.DeleteObject();

		}
		else
		{
			//PrintDIBs(pDC,pr,iSpace);
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::PrintInfoRectPortrat(CDC* pDC,CRect rect, 
											  CFont* pFont, int iSpace,
											  const CString& sPicture,
											  const CString& sData,
											  const CString& sComment)
{
	CFont* pFontOld;
	int h;
	CRect r;

	pFontOld = pDC->SelectObject(pFont);

	r = rect;
	r.right = rect.left + (rect.Width()- iSpace)/2;
	pDC->Rectangle(r);
	r.DeflateRect(iSpace,iSpace);
	
	DRAWTEXTPARAMS dtP = {sizeof(DRAWTEXTPARAMS), 6, 0, 0, 0};
	h = pDC->DrawTextEx(InsertSpacesForTabbedOutput(sPicture),r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);
	r.top += h+iSpace;
	pDC->DrawTextEx(InsertSpacesForTabbedOutput(sData),r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);

	r = rect;
	r.left = rect.left + (rect.Width() + iSpace)/2;
	pDC->Rectangle(r);
	r.DeflateRect(iSpace,iSpace);
	pDC->DrawTextEx(sComment,r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);

	pDC->SelectObject(pFontOld);
}
/////////////////////////////////////////////////////////////////////////////
int CWndLive::PrintInfoRectLandscape(CDC* pDC,CRect rect, CFont* pFont)
{
	CFont* pFontOld;
	CString sTitle,sData;
	CSize size;
	int h = 0;

	sTitle = theApp.GetStationName();
	if (sTitle.IsEmpty())
	{
		sTitle = COemGuiApi::GetProducerName();
	}

	pFontOld = pDC->SelectObject(pFont);

	sTitle += _T(" ") + FormatPicture(m_pPrintPictureRecord, COMMA_SEPARATED);

	size = pDC->GetOutputTextExtent(sTitle);
	h += size.cy+1;
	pDC->TextOut(rect.left+1, rect.top+1, sTitle);

	CIPCFields fields;
	fields.FromString(m_pPrintPictureRecord->GetInfoString());
	sData = FormatData(fields, _T(", "),_T(": "));

	if (!sData.IsEmpty())
	{
		size = pDC->GetOutputTextExtent(sData);
		pDC->TextOut(rect.left+1, rect.top+h+2, sData);
		h += size.cy+3;
	}
	pDC->SelectObject(pFontOld);
	return h;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::PrintTitleRectPortrat(CDC* pDC,CRect rect, CFont* pFont)
{
	CFont* pFontOld;
	CString sTitle;
	CSize size;

	sTitle = theApp.GetStationName();
	if (sTitle.IsEmpty())
	{
		sTitle = COemGuiApi::GetProducerName();
	}

	pDC->Rectangle(rect);
	pFontOld = pDC->SelectObject(pFont);
	size = pDC->GetOutputTextExtent(sTitle);
	pDC->TextOut(rect.left+(rect.Width()-size.cx)/2,
		rect.top+(rect.Height()-size.cy)/2,
		sTitle);
	pDC->SelectObject(pFontOld);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndLive::CopyPictureForPrinting()
{
	//copy the current picture for printing
	BOOL bRet = TRUE;
	if(m_pPictureRecord)
	{
		ASSERT(m_pPrintPictureRecord == NULL);
		WK_DELETE(m_pPrintPictureRecord);
		m_csPictureRecord.Lock();
		m_pPrintPictureRecord = new CIPCPictureRecord(*m_pPictureRecord);
		m_csPictureRecord.Unlock();
		if(!m_pPrintPictureRecord)
		{
			bRet = FALSE;
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::DeletePictureForPrinting()
{
	WK_DELETE(m_pPrintPictureRecord);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndLive::FormatPicture(CIPCPictureRecord* pPictureRecord, BOOL bTabbed /*= FALSE*/)
{
	CString s;

	if (pPictureRecord)
	{	
		CString sFormat,sDate,sTime;
		sDate = pPictureRecord->GetTimeStamp().GetDate();
		sTime = pPictureRecord->GetTimeStamp().GetTime();
		sFormat.LoadString(bTabbed == TRUE ? IDS_PICTURE_TABBED : IDS_PICTURE);
		if (bTabbed == COMMA_SEPARATED)
		{
			sFormat.Replace(_T("%s"), _T("%s, "));
			int nFind = sFormat.Find(_T("%d"));
			if (nFind != -1)
			{
				nFind+=2;
				sFormat = sFormat.Left(nFind) + _T(", ") + sFormat.Mid(nFind);
			}
		}

		CSize size = GetPictureSize();
		s.Format(sFormat, 1+(int)pPictureRecord->GetCamID().GetSubID(),
			sDate,sTime,size.cx,size.cy);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndLive::FormatData(CIPCFields &fields, const CString& sD,const CString& sS)
{
	CString result;

	int i,c;
	CString name,value;
	CIPCField* pField;

	c = fields.GetSize();

	for (i=0;i<c;i++)
	{
		pField = fields.GetAtFast(i);
		name = theApp.GetMappedString(pField->GetName());
		if (name.IsEmpty())
		{
			if	(   (pField->GetName() == CIPCField::m_sfStNm)
				|| (pField->GetName() == CIPCField::m_sfCaNm)
				|| (pField->GetName() == CIPCField::m_sfInNm)
				|| (pField->GetName() == CIPCField::m_sfCurr)
				)
			{
				if (m_pServer)
				{
					name = m_pServer->GetFieldName(pField->GetName());
				}
			}
		}
		value = pField->ToString();
		if (!name.IsEmpty() && !value.IsEmpty())
		{
			value.TrimLeft();
			value.TrimRight();
			if (!value.IsEmpty())
			{
				if (!result.IsEmpty())
				{
					result += sD;
				}
				result += name + sS + value;
			}
		}
	}
	return result;
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnEditComment()
{
	CDlgComment dlg(this);

	dlg.m_sComment = m_sComment;
	if (IDOK==dlg.DoModal())
	{
		m_sComment = dlg.m_sComment;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndLive::OnUpdateEditComment(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
CSystemTime CWndLive::GetTimeStamp()
{
	//This function was implemented by GF, 26.05.2004
	CSystemTime stTimeStamp;
	m_csPictureRecord.Lock();
	stTimeStamp = m_stTimeStamp;
	m_csPictureRecord.Unlock();
	return stTimeStamp;
}
