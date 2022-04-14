// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"

#include "Vision.h"

#include "MainFrm.h"

#include "VisionDoc.h"
#include "VisionView.h"
#include "DisplayWindow.h"
#include "CIPCDataBaseVision.h"
#include "MDConfigDlg.h"
#include "CoCoWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow

CDisplayWindow::CDisplayWindow(CServer* pServer, const CIPCOutputRecord &or) 
	: CSmallWindow(pServer)
{
	m_orOutput  = or;
	m_hostID = pServer->GetHostID();
	m_pVisionView = NULL;
	m_ccType = CCT_UNKNOWN;
	m_bTimeValid = FALSE;
	m_dwPTZFunctions = PTZF_DEFAULT;
	m_pPictureRecord = NULL;
	
	m_bWasSomeTimesActive = FALSE;
	m_bIsRequesting = FALSE;
	m_bWaitingForRequest = FALSE;
	m_iWaitingForPicture = 0;
	
	m_bIPicture = FALSE;
	m_iX = 12;
	m_iY = 9;
	m_LastCameraControlCmd     = CCC_INVALID;
	m_LastCameraControlCmdStop = CCC_INVALID;
	m_dwLastPictureConfirmedTime = GetTickCount();

	m_Resolution = RESOLUTION_HIGH;
	m_bStoring   = pServer->IsAlarm();
	if (pServer->IsControl())
	{
		m_bStoring = pServer->IsControlSave();
	}

	m_bMarkedForShowColorDialog = FALSE;
	m_b1to1 = FALSE;
	m_iContrast = 0;
	m_iBrightness = 0;
	m_iSaturation = 0;
	m_iQuality = 0;
	m_pCSBDialog = NULL;
	m_pVideoSNDialog = NULL;
	m_pSNConfigDialog = NULL;
	m_pMDConfigDlg = NULL;

	m_wMD_X = 0;
	m_wMD_Y = 0;

	m_rcZoom = CRect(0,0,0,0);
	m_iCountZooms = 0;
	m_dwPTZSpeed = 5;
	m_bIsDecoding = FALSE;

	m_pContextMenu = NULL;
	m_uTimerTest = NULL;
	m_uTimerInit = NULL;
	m_dwPTZTemp = PTZF_DEFAULT;
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow::~CDisplayWindow()
{
	m_csPictureRecord.Lock();
	WK_DELETE(m_pPictureRecord);
	m_csPictureRecord.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDisplayWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CDisplayWindow)
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
	ON_COMMAND(ID_VIDEO_SN, OnVideoSn)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN, OnUpdateVideoSn)
	ON_COMMAND(ID_VIDEO_SN_1, OnVideoSn1)
	ON_COMMAND(ID_VIDEO_SN_2, OnVideoSn2)
	ON_COMMAND(ID_VIDEO_SN_3, OnVideoSn3)
	ON_COMMAND(ID_VIDEO_SN_4, OnVideoSn4)
	ON_COMMAND(ID_VIDEO_SN_5, OnVideoSn5)
	ON_COMMAND(ID_VIDEO_SN_6, OnVideoSn6)
	ON_COMMAND(ID_VIDEO_SN_7, OnVideoSn7)
	ON_COMMAND(ID_VIDEO_SN_8, OnVideoSn8)
	ON_COMMAND(ID_VIDEO_SN_9, OnVideoSn9)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_1, OnUpdateVideoSn1)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_2, OnUpdateVideoSn2)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_3, OnUpdateVideoSn3)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_4, OnUpdateVideoSn4)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_5, OnUpdateVideoSn5)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_6, OnUpdateVideoSn6)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_7, OnUpdateVideoSn7)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_8, OnUpdateVideoSn8)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_9, OnUpdateVideoSn9)
	ON_COMMAND(ID_VIDEO_STOP, OnVideoStop)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIDEO_SN_CONFIG, OnVideoSnConfig)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_CONFIG, OnUpdateVideoSnConfig)
	ON_COMMAND(ID_VIDEO_SN_HOME, OnVideoSnHome)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SN_HOME, OnUpdateVideoSnHome)
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
	ON_UPDATE_COMMAND_UI(ID_SMALL_CLOSE, OnUpdateSmallClose)
	ON_COMMAND(ID_TOGGLE_RESOLUTION, OnToggleResolution)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CONTEXT, OnUpdateSmallContext)
	//}}AFX_MSG_MAP
	ON_WM_ENTERMENULOOP()
	ON_MESSAGE(WM_USER,OnUser)
	ON_WM_EXITMENULOOP()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetOutputRecord(const CIPCOutputRecord &or)
{
	m_orOutput  = or;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetCIPCOutputWindow()
{
	// overwritten in derived class
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::Create(const RECT& rect, CVisionView* pParentWnd)
{
	CWK_Profile wkp;
	CString sSection;
	sSection.Format(_T("%s\\%08lx\\%08lx\\%08lx"),
		VISION_SETTINGS,
		theApp.m_pUser->GetID().GetID(),
		m_hostID.GetID(),
		m_orOutput.GetID().GetID());
	m_iQuality = wkp.GetInt(sSection,_T("Quality"),m_iQuality);
	if (   IsCoco()
		&& (m_pServer->GetConfirmedOutputOptions() & SCO_JPEG_AS_H263)
		)
	{
		if (m_iQuality<0)
		{
			m_iQuality = 2;
		}
	}

	m_Resolution = (Resolution)wkp.GetInt(sSection,_T("Resolution"),m_Resolution);

	sSection.Format(_T("%s\\%08lx\\%08lx"),	VISION_SETTINGS, m_hostID.GetID(), m_orOutput.GetID().GetID());
	m_dwMonitor = wkp.GetInt(sSection, USE_MONITOR, 0);

	m_pVisionView = pParentWnd;
	m_b1to1 = m_pVisionView->m_b1to1;
	//SetDlgCtrlID(m_orOutput.GetID().GetID());

	return CWnd::Create(DISPLAY_WINDOW_CLASS,m_orOutput.GetName(), 
						WS_CHILD | WS_BORDER | WS_VISIBLE | 
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						rect, pParentWnd,
						(int)m_orOutput.GetID().GetID()); 

}
/////////////////////////////////////////////////////////////////////////////
CSecID CDisplayWindow::GetID()
{
	return m_orOutput.GetID();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawBitmap(CDC* pDC, const CRect &rcDest)
{
	// base class does nothing
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime,sDateTime;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	m_csPictureRecord.Lock();
	if (m_bTimeValid)
	{
		sDateTime = m_stTimeStamp.GetDateTime();
		sTime = m_stTimeStamp.GetTime();
	}
	m_csPictureRecord.Unlock();
	sTitle = GetServerName() + _T(": ") + m_orOutput.GetName() + _T(", ") + sDateTime;
#ifdef _DEBUG
	CString sID;
	sID.Format(_T("%08lx"),m_orOutput.GetID().GetID());
	sTitle = sID + _T(",") + sTitle;
	if (m_pPictureRecord)
	{
		switch(m_pPictureRecord->GetCompressionType())
		{
		case COMPRESSION_JPEG:
			sTitle = _T("J ")+ sTitle;
			break;
		case COMPRESSION_H263:
			sTitle = _T("H ")+ sTitle;
			break;
		case COMPRESSION_YUV_422:
			sTitle = _T("Y ")+ sTitle;
			break;
		}
	}
	if (IsCoco())
	{
		sTitle = _T("C ") + sTitle;
	}
	if (IsMico())
	{
		sTitle = _T("M ") + sTitle;
	}
#endif

	size = pDC->GetOutputTextExtent(sTitle);
	if (size.cx > (rect.Width()-m_ToolBarSize.cx))
	{
		sTitle = m_orOutput.GetName() + _T(", ") + sDateTime;
		size = pDC->GetOutputTextExtent(sTitle);
		if (size.cx > (rect.Width()-m_ToolBarSize.cx))
		{
			sTitle = m_orOutput.GetName() + _T(", ") + sTime;
			size = pDC->GetOutputTextExtent(sTitle);
			if (size.cx > (rect.Width()-m_ToolBarSize.cx))
			{
				sTitle = m_orOutput.GetName();
			}
		}
	}
#ifdef _DEBUG
	CString sMon;
	sMon.Format(_T("#%d "), LOWORD(m_dwMonitor));
	sTitle = sMon + sTitle;
#endif
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);

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
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawFace(CDC* pDC)
{
	CRect rect;
	GetFrameRect(rect);
	pDC->FillRect(rect,&m_pVisionView->m_CinemaBrush);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawCinema(CDC* pDC, LPRECT lpRectKeyColor /*= NULL*/)
{
	CRect vr;
	CRect cr;
	CRect fr;

	if (lpRectKeyColor)
	{
		vr = lpRectKeyColor;
	}
	else
	{
		GetVideoClientRect(vr);
	}

	GetFrameRect(fr);

	cr = fr;
	if (vr.Width()+1<fr.Width())
	{
		// links und rechts einen Rahmen
		cr.right = vr.left;
		pDC->FillRect(cr,&m_pVisionView->m_CinemaBrush); // links
		cr.right = fr.right;
		cr.left = vr.right;
		pDC->FillRect(cr,&m_pVisionView->m_CinemaBrush); // rechts
	}
	cr = fr;
	if (vr.Height()<fr.Height())
	{
		// oben und unten einen Rahmen
		cr.bottom = vr.top;
		pDC->FillRect(cr,&m_pVisionView->m_CinemaBrush); // oben
		cr.top = vr.bottom;
		cr.bottom = fr.bottom;
		pDC->FillRect(cr,&m_pVisionView->m_CinemaBrush); // unten
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DrawCross(CDC* pDC, WORD wX, WORD wY)
{
	if (theApp.m_bTargetDisplay)
	{
		// Den Zoom ber¸cksichtigen.
		if (!(m_rcZoom.IsRectEmpty()))
		{
			int x,y;
			CSize ImageSize(GetPictureSize());
			x = (wX * ImageSize.cx) / 1000 - m_rcZoom.left;
			y = (wY * ImageSize.cy) / 1000 - m_rcZoom.top;
			
			wX = (WORD)(1000 * x / m_rcZoom.Width());
			wY = (WORD)(1000 * y / m_rcZoom.Height());
		}
		
		CRect rect;
		GetVideoClientRect(rect);
		int x,y;
		x = (wX * rect.Width()) / 1000 + rect.left;
		y = (wY * rect.Height()) / 1000 + rect.top;
		
		CPen pen;
		CPen* pOldPen;
		
		pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		pOldPen = pDC->SelectObject(&pen);
		
		pDC->MoveTo(rect.left,y);
		pDC->LineTo(rect.right,y);
		
		pDC->MoveTo(x,rect.top);
		pDC->LineTo(x,rect.bottom);
		
		int r = 4;
		
		pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);
		
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::GetVideoClientRect(LPRECT lpRect)
{
	CSize s = GetPictureSize();
	CSmallWindow::GetVideoClientRect(lpRect,s,m_b1to1);
} 
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::StorePicture(const CIPCPictureRecord &pict)
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

		if (pPicture->GetPictureType()==SPT_FULL_PICTURE)
		{
			m_bIPicture = TRUE;
		}
		if (m_bIPicture)
		{
			CVisionDoc* pDoc = m_pVisionView->GetDocument();
			CIPCDataBaseVision* pDataBase = pDoc->GetDatabase();

			if (pDataBase)
			{
				CIPCFields fields;
				fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,
										 m_pServer->GetName(),32,'C'));
				fields.SafeAdd(new CIPCField(CIPCField::m_sfCaNm,
										 m_orOutput.GetName(),32,'C'));
				pDataBase->DoRequestNewSavePictureForHost(m_pServer->GetHostID(),
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
BOOL CDisplayWindow::PictureData(const CIPCPictureRecord &pict)
{
	// Achtung in CIPCThread !
	if (!m_bIsDecoding)
	{
		m_csPictureRecord.Lock();
		WK_DELETE(m_pPictureRecord);
		m_pPictureRecord = new CIPCPictureRecord(pict);
		m_stTimeStamp = m_pPictureRecord->GetTimeStamp();
		m_bTimeValid = TRUE;
		m_csPictureRecord.Unlock();
		StorePicture(pict);
		m_dwLastPictureConfirmedTime = GetTickCount();
		PostMessage(WM_COMMAND,ID_CHANGE_VIDEO);
		// message goes to OnChangeVideo
		m_iWaitingForPicture = 0;
	}
	else
	{
		TRACE(_T("dropping frame %s\n"),GetName());
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::RequestSetContrast(int contrast)
{
	CIPCOutputVision* pOutput = m_pServer->GetOutput();
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
void CDisplayWindow::RequestSetBrightness(int brightness)
{
	CIPCOutputVision* pOutput = m_pServer->GetOutput();
	
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
void CDisplayWindow::RequestSetColor(int color)
{
	CIPCOutputVision* pOutput = m_pServer->GetOutput();
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
void CDisplayWindow::RequestCSBUpdate()
{
	CIPCOutputVision* pOutput = m_pServer->GetOutput();
	if (pOutput)
	{
		pOutput->RequestCSB(GetCamID());
	}
}
////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::InitialRequest()
{
	if (m_pServer->IsConnected())
	{
// gf todo raus
// Was haben PTZ-Typ und -Funktion mit Input zu tun?
// Nur weil die CommUnit l‰uft, muss die Kamera doch kein PTZ sein
// Auﬂerdem wird jede Kamera abgefragt, auch wenn sie gar keine PTZ ist
// Das ist doch eine Output-Info (OREC_CAMERA_IS_FIXED == FALSE)
// Dann ist auch das Problem gelˆst, dass falls OutputInfo vor InputInfo kommt,
// die PTZ-Infos gar nicht abgefragt werden, weil GetCommID() == SECID_NO_ID ist!
// (siehe MNO Problem)
//		CIPCInputVision* pInput = m_pServer->GetInput();
/*		if (pInput)
		{
			CString sKey;
			CSecID cid = pInput->GetCommID();
			if (cid!=SECID_NO_ID)
			{
//				WK_TRACE(_T("RequestPTZFunctions %08x\n"), GetCamID().GetID());
				sKey.Format(CSD_CAM_CONTROL_TYPE_FMT,GetCamID().GetID());
				pInput->DoRequestGetValue(cid,sKey);
				sKey.Format(CSD_CAM_CONTROL_FKT_FMT,GetCamID().GetID());
				pInput->DoRequestGetValue(cid,sKey);
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
*/		
		CIPCOutputVision* pOutput = m_pServer->GetOutput();
		if (pOutput)
		{
			if (pOutput->IsPTZCamera(GetCamID()))
			{
				CIPCInputVision* pInput = m_pServer->GetInput();
				if (pInput)
				{
					CString sKey;
					CSecID cid = pInput->GetCommID();
					if (cid!=SECID_NO_ID)
					{
//						WK_TRACE(_T("RequestPTZFunctions %08x\n"), GetCamID().GetID());
						sKey.Format(CSD_CAM_CONTROL_TYPE_FMT,GetCamID().GetID());
						pInput->DoRequestGetValue(cid,sKey);
						sKey.Format(CSD_CAM_CONTROL_FKT_FMT,GetCamID().GetID());
						pInput->DoRequestGetValue(cid,sKey);
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
		SetStore(m_bStoring || m_pVisionView->GetDocument()->GetStore());
	}
	else
	{
		WK_TRACE(_T("Server not connected at CDisplayWindow::InitialRequest\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
int CDisplayWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_DISPLAY))
	{
		TRACE(_T("Failed to create display toolbar\n"));
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndToolBar.GetBarStyle();
	dwStyle &= ~(CBRS_BORDER_RIGHT|CBRS_BORDER_TOP|CBRS_BORDER_LEFT|CBRS_BORDER_BOTTOM);
	dwStyle |= CBRS_TOOLTIPS | CBRS_SIZE_FIXED;
	m_wndToolBar.SetBarStyle(dwStyle);
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);
	CSize sizeButton(14,14);
	CSize sizeImage(12,12);
	m_wndToolBar.SetBorders(1,1,1,1);
	m_wndToolBar.SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
	m_wndToolBar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
	m_wndToolBar.GetToolBarCtrl().AddBitmap(1,IDB_SMALL_STOP);
	m_ToolBarSize.cx = m_wndToolBar.GetToolBarCtrl().GetButtonCount()*(sizeButton.cx+4)+4;
	m_ToolBarSize.cy = sizeButton.cy+6;

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ActivateCamera()
{
	// FENSTER AKTIVIEREN
	// Auf jeden Fall neues Bild anfordern
	if (   m_pVisionView->SetRequestedDisplayWindow(this)
		|| CanMultipleRequests())
	{
		if (CanMultipleRequests())
		{
			m_bIsRequesting = TRUE;
		}
		DoRequestPictures();
	}
	else
	{
		m_bWaitingForRequest = TRUE;
		TRACE(_T("waiting for request %s\n"),GetName());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::DoRequestPictures()
{
	if (m_pServer->IsConnected())
	{
		CIPCOutputVision* pOutput = m_pServer->GetOutput();
		pOutput->RequestPictures(GetCamID());
		m_bWaitingForRequest = FALSE;
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
#ifndef _DTS
BOOL CDisplayWindow::DoRequestVideoOut()
{
	if (   m_pServer->IsLocal()
		&& m_pServer->IsConnected()
	)
	{
		CIPCOutputVision* pOutput = m_pServer->GetOutput();
		pOutput->DoRequestSetValue(GetCamID(), CSD_V_OUT, "");
		return TRUE;
	}
	return FALSE;
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::DeactivateCamera()
{
//ML 29.9.99 Begin Insertation{
//	if (m_pServer->IsControlConnected())
//	{
//		m_pServer->GetControl()->ConfirmCamera(GetCamID(), Off);
//	}
//ML 29.9.99 End Insertation{
	if (m_pServer->IsConnected())
	{
		CIPCOutputVision* pCIPC = m_pServer->GetOutput();
		BOOL bForceCocoStop = FALSE;
		if (   IsCoco()
			&& m_pServer->IsDTS()
			&& m_Resolution == RESOLUTION_LOW)
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
void CDisplayWindow::ClearWasSomeTimesActive()
{
	// base class does nothing
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetMode(const CString& sMode)
{
	m_sMode = sMode;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetMDValues(WORD wX, WORD wY)
{
	m_wMD_X = wX;
	m_wMD_Y = wY;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoFein() 
{
	m_rcZoom = CRect(0,0,0,0); // Zoom aus.
	m_Resolution = RESOLUTION_HIGH;
	if (IsStreaming())
	{
		DoRequestPictures();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoGrob() 
{
	m_rcZoom = CRect(0,0,0,0); // Zoom aus.
	BOOL bWasStreaming = IsStreaming();
	m_Resolution = RESOLUTION_LOW;
	if (   bWasStreaming
		|| IsStreaming())
	{
		DoRequestPictures();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnToggleResolution()
{
	if (!m_pServer->IsDTS())
	{
		if (m_Resolution == RESOLUTION_LOW)
		{
			OnVideoFein();
		}
		else if (m_Resolution == RESOLUTION_HIGH)
		{
			OnVideoGrob();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoFein(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Resolution == RESOLUTION_HIGH);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoGrob(CCmdUI* pCmdUI) 
{
	TRACE(_T("OnUpdateVideoGrob %x\n"), this);
	pCmdUI->SetCheck(m_Resolution == RESOLUTION_LOW);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSave() 
{
	SetStore(!m_bStoring);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSave(CCmdUI* pCmdUI) 
{
	CVisionDoc* pDoc = m_pVisionView->GetDocument();
	if (!theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(pDoc->IsDatabaseConnected());
	}
	else
	{
		pCmdUI->Enable(  pDoc->IsDatabaseConnected()
					   &&!IsFullScreen());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (IsSn())
	{
		HCURSOR hC = theApp.m_hArrow;
		m_LastCameraControlCmd = CCC_INVALID;
		m_LastCameraControlCmdStop = CCC_INVALID;
		// -------------------
		//|\    Tilt Up      /|
		//| \ ------------- / |
		//|  P| Zoom| Zoom| P |
		//|  L|  In | Out | R |
		//| / ------------- \ |
		//|/    Tilt Down    \|
		// -------------------
		CRect rect;
		CRgn  prof;
		int cx,cy;
		GetVideoClientRect(rect);
		cx = rect.Width() / 3;
		cy = rect.Height() / 3;
		CPoint pts[4];

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
			m_LastCameraControlCmd = CCC_TILT_UP;
			m_LastCameraControlCmdStop = CCC_TILT_STOP;
		}
		else
		{
			prof.DeleteObject();
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
				m_LastCameraControlCmd = CCC_TILT_DOWN;
				m_LastCameraControlCmdStop = CCC_TILT_STOP;
			}
			else
			{
				// PanLeft
				prof.DeleteObject();
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
					m_LastCameraControlCmd = CCC_PAN_LEFT;
					m_LastCameraControlCmdStop = CCC_PAN_STOP;
				}
				else
				{
					prof.DeleteObject();
					// PanRight
					pts[0].x = rect.right;
					pts[0].y = rect.top;
					pts[1] = rect.BottomRight();
					pts[2].x = rect.right-cx;
					pts[2].y = rect.bottom-cy;
					pts[3].x = rect.right-cx;
					pts[3].y = rect.top+cy;
					prof.CreatePolygonRgn(pts,4,WINDING);
					if (prof.PtInRegion(point))
					{
						prof.DeleteObject();
						hC = theApp.m_hPanRight;
						m_LastCameraControlCmd = CCC_PAN_RIGHT;
						m_LastCameraControlCmdStop = CCC_PAN_STOP;
					}
					else
					{
						prof.DeleteObject();
						pts[0].x = rect.left+cx;
						pts[0].y = rect.top+cy;
						pts[1].x = rect.right-cx;
						pts[1].y = rect.top+cy;
						pts[2].x = rect.right-cx;
						pts[2].y = rect.bottom-cy;
						pts[3].x = rect.left+cx;
						pts[3].y = rect.bottom-cy;
						prof.CreatePolygonRgn(pts,4,WINDING);
						if (prof.PtInRegion(point))
						{
//							if (nFlags & MK_CONTROL)
							if (point.x>((rect.left + rect.right)/2))
							{
								hC = theApp.m_hZoomOut;
								m_LastCameraControlCmd = CCC_ZOOM_OUT;
								m_LastCameraControlCmdStop = CCC_ZOOM_STOP;
							}
							else
							{
								hC = theApp.m_hZoomIn;
								m_LastCameraControlCmd = CCC_ZOOM_IN;
								m_LastCameraControlCmdStop = CCC_ZOOM_STOP;
							}
						}
						prof.DeleteObject();
					}
				}
			}
		}
		SetCursor(hC);
	}

	else
	{
		CRect rcRect, rcTitle, rcFrame;
		GetVideoClientRect(rcRect);
		GetTitleRect(rcTitle);

		if(rcRect.PtInRect(point) && !rcTitle.PtInRect(point))
		{
			HCURSOR hC = theApp.LoadCursor(IDC_GLASS);
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);
			SetCursor(hC);
		}
		else
		{
			HCURSOR hC = theApp.LoadStandardCursor(IDC_ARROW);
			SetClassLong(m_hWnd, GCL_HCURSOR, (DWORD)hC);
			SetCursor(hC);
		}
	}

	CSmallWindow::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcVideo;
	GetVideoClientRect(rcVideo);

	if (rcVideo.PtInRect(point))
	{
		TRACE(_T("CDisplayWindow::OnLButtonDown\n"));
		if (IsSn() && (m_LastCameraControlCmd != CCC_INVALID))
		{
			RequestPTZ(m_LastCameraControlCmd, m_dwPTZSpeed, 0xFFFFFFFF, TRUE);
		}
		else if (   m_bWasSomeTimesActive
			     && !theApp.m_bDisableZoom)
		{

			// Liegt der Punkt im Videorect?
			if (rcVideo.PtInRect(point))
			{
				if (!ScrollDisplayWindow())
				{
					if(m_iCountZooms < 3)
					{
						ZoomDisplayWindow(point);
						m_iCountZooms++;
					}
					else
					{
						OnZoomOut();
					}
				}
			}
		}
		if (!m_pVisionView->IsView1plus())
		{
			CSmallWindow::OnLButtonDown(nFlags,point);
		}
	}
	else
	{
		CSmallWindow::OnLButtonDown(nFlags,point);
	}

// darf erst bei OnTimer() aufgerufen werden, siehe CSmallWindow()
// da dort SetBigSmallWindow() aufgerufen wird
//	m_pVisionView->SetRequestedDisplayWindow(this);

}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (IsSn() && (m_LastCameraControlCmdStop != CCC_INVALID))
	{
		RequestPTZ(m_LastCameraControlCmdStop, 0, 0xFFFFFFFF, FALSE);
	}
	
	CSmallWindow::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::PopupMenu(CPoint pt)
{
	CMenu menu;
//	CMenu* pMenu;
	CMenu* pFPSMenu;
	CMenu* pSNMenu;

	menu.LoadMenu(IDR_CONTEXT);
	m_pContextMenu = menu.GetSubMenu(SUB_MENU_CAMERA);	// Kamera

	if (m_pServer->IsDTS())
	{
		m_pContextMenu->DeleteMenu(ID_VIDEO_SAVE_REFERENCE,MF_BYCOMMAND);
		m_pContextMenu->DeleteMenu(ID_VIDEO_SHOW_REFERENCE,MF_BYCOMMAND);
		if (   !m_pServer->Can_DV_H263_QCIF()
			|| IsMico())
		{
			m_pContextMenu->DeleteMenu(ID_VIDEO_FEIN,MF_BYCOMMAND);
			m_pContextMenu->DeleteMenu(ID_VIDEO_GROB,MF_BYCOMMAND);
		}
		m_pContextMenu->DeleteMenu(ID_VIDEO_CSB,MF_BYCOMMAND);
		m_pContextMenu->DeleteMenu(ID_SEPARATOR,MF_BYCOMMAND);
	}
	else
	{
		if (IsMico())
		{
			CString sL;
			sL.LoadString(IDS_FPS);
			pFPSMenu = menu.GetSubMenu(SUB_MENU_MICO);	// Mico
			m_pContextMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu,sL);
		}
		if (IsCoco())
		{
			CString sL;
			sL.LoadString(IDS_FPS);
			pFPSMenu = menu.GetSubMenu(SUB_MENU_COCO);	// Coco
			m_pContextMenu->AppendMenu(MF_POPUP,(UINT)pFPSMenu->m_hMenu,sL);
		}
	}
	if (((CMainFrame*)GetParentFrame())->GetNoOfExternalFrames()>0)
	{
		CString sL;
		pFPSMenu = menu.GetSubMenu(SUB_MENU_MONITOR);	// Monitor
		m_pContextMenu->AppendMenu(MF_SEPARATOR, 0, _T(""));
		int i, nCount = pFPSMenu->GetMenuItemCount();
		for (i=0; i<nCount; i++)
		{
			pFPSMenu->GetMenuString(i, sL, MF_BYPOSITION);
			m_pContextMenu->AppendMenu(MF_STRING, pFPSMenu->GetMenuItemID(i), sL);
		}
	}

	if (IsSn())
	{
		CString sL;
		sL.LoadString(IDS_SN);
		pSNMenu = menu.GetSubMenu(SUB_MENU_PTZ);	// Schwenk/Neige
		m_pContextMenu->AppendMenu(MF_POPUP,(UINT)pSNMenu->m_hMenu,sL);
	}

	COemGuiApi::DoUpdatePopupMenu(this,m_pContextMenu);

//	m_pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y, this);	

// Die Beschreibungen f¸r die Statuszeile	m¸ssen an das MainWnd gehen
// Die Updatehandler werden trotzdem an das richtige (aktive) Fenster weitergeroutet
// auﬂerdem werden sie vorher durch die Funktion 
//	_T("COemGuiApi::DoUpdatePopupMenu(this,m_pContextMenu);") abgearbeitet.

  	m_pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, AfxGetMainWnd());		
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnSmallContext() 
{
	CRect trect;
	CRect brect;
	CPoint pt;

	m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(ID_SMALL_CONTEXT),brect);
	m_wndToolBar.GetWindowRect(trect);
	ScreenToClient(trect);
	pt.x = trect.left + brect.left;
	pt.y = trect.bottom;
	ClientToScreen(&pt);
	
	// Fenster aktivieren f¸r den Updatehandler und die korrekte Nachrichtenverarbeitung
	m_pVisionView->SetCmdActiveSmallWindow(this);

	PopupMenu(pt);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoCsb() 
{
	if (!m_pServer->IsDTS())
	{
		if (m_pCSBDialog)
		{
			m_pCSBDialog->DestroyWindow();
			m_pCSBDialog = NULL;
			m_bMarkedForShowColorDialog = FALSE;
		}
		else
		{
			// first request the actual parameters
			// then show the dialog
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
			if (pOutput)
			{
				pOutput->RequestCSB(GetCamID());
				m_bMarkedForShowColorDialog = TRUE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoCsb(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pCSBDialog!=NULL);
	pCmdUI->Enable(!m_pServer->IsDTS());
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDisplayWindow::OnUser(WPARAM wParam, LPARAM lParam) 
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
		if (WK_IS_WINDOW(m_pVideoSNDialog))
		{
			m_pVideoSNDialog->SetPTZType(m_ccType);
		}
		break;
	case WPARAM_CCF:
#if 1
		m_dwPTZFunctions = (DWORD)lParam;
		WK_TRACE(_T("%s CC Func %08lx\n"),GetName(),m_dwPTZFunctions);
		if (   m_pContextMenu
			&& m_pContextMenu->GetSafeHmenu()
			)
		{
			COemGuiApi::DoUpdatePopupMenu(this,m_pContextMenu);
		}
		if (WK_IS_WINDOW(m_pVideoSNDialog))
		{
			m_pVideoSNDialog->SetPTZFunctions(m_dwPTZFunctions);
		}
#else
		m_dwPTZTemp = (DWORD)lParam;
		if (m_uTimerTest == NULL)
		{
			m_uTimerTest = SetTimer(815, 10000, NULL);
		}
#endif
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnCSBChanged()
{
	if (m_pCSBDialog)
	{
		m_pCSBDialog->ActualizeSettings((m_iSaturation-255)/9,(m_iBrightness-128)/4,(m_iContrast-255)/9);
	}
	else
	{
		if (m_bMarkedForShowColorDialog)
		{
			m_bMarkedForShowColorDialog = FALSE;
			m_pCSBDialog = new CCSBDialog(this,(m_iSaturation-255)/9,(m_iBrightness-128)/4,(m_iContrast-255)/9);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnDestroy() 
{
	CSmallWindow::OnDestroy();
	CWK_Profile wkp;
	CString sSection;

	if (m_pCSBDialog)
	{
		m_pCSBDialog->DestroyWindow();
		m_pCSBDialog = NULL;
	}
	if (m_pVideoSNDialog)
	{
		m_pVideoSNDialog->DestroyWindow();
		m_pVideoSNDialog = NULL;
	}
	if (m_pSNConfigDialog)
	{
		m_pSNConfigDialog->DestroyWindow();
		m_pSNConfigDialog = NULL;
	}
	if (m_pMDConfigDlg)
	{
		m_pMDConfigDlg->DestroyWindow();
		m_pMDConfigDlg = NULL;
	}

	if (theApp.m_pUser)
	{
		sSection.Format(_T("%s\\%08lx\\%08lx\\%08lx"),
			VISION_SETTINGS,
			theApp.m_pUser->GetID().GetID(),
			m_hostID.GetID(),
			m_orOutput.GetID().GetID());
		wkp.WriteInt(sSection,_T("Quality"),m_iQuality);
		wkp.WriteInt(sSection,_T("Resolution"),m_Resolution);

	}
	sSection.Format(_T("%s\\%08lx\\%08lx"), VISION_SETTINGS, m_hostID.GetID(), m_orOutput.GetID().GetID());
	wkp.WriteInt(sSection, USE_MONITOR, m_dwMonitor);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn() 
{
	if (IsSn())
	{
		if (m_pVideoSNDialog)
		{
			m_pVideoSNDialog->DestroyWindow();
			m_pVideoSNDialog = NULL;
		}
		else
		{
			CIPCInputVision* pInput = m_pServer->GetInput();
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
			m_pVideoSNDialog = new CVideoSNDialog(pInput,pOutput,m_ccType,m_dwPTZFunctions,this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSn() && (m_pSNConfigDialog==NULL);

	pCmdUI->Enable(bEnable);	
	if (IsSn())
	{
		pCmdUI->SetCheck(m_pVideoSNDialog!=NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::RequestPTZ(CameraControlCmd ccc, DWORD dwValue, DWORD dwFlags, BOOL bOn /*=FALSE*/)
{
	if (m_dwPTZFunctions & dwFlags)
	{
		if (m_ccType==CCT_RELAY_PTZ)
		{
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
			if (pOutput)
			{
				pOutput->RequestCameraControl(m_LastCameraControlCmd,bOn);
			}
		}
		else if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d"), (int)ccc);
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
			if (pOutput)
			{
				pOutput->DoRequestSetValue(GetCamID(), CSD_CAMERA_CONTROL, sValue);
			}
		}
		else
		{
			CIPCInputVision* pInput = m_pServer->GetInput();
			if (pInput)
			{
				pInput->DoRequestCameraControl(pInput->GetCommID(),GetCamID(),ccc,dwValue);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn1() 
{
	RequestPTZ(CCC_POS_1, 0L, PTZF_POS_1);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn2() 
{
	RequestPTZ(CCC_POS_2, 0L, PTZF_POS_2);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn3() 
{
	RequestPTZ(CCC_POS_3, 0L, PTZF_POS_3);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn4() 
{
	RequestPTZ(CCC_POS_4, 0L, PTZF_POS_4);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn5() 
{
	RequestPTZ(CCC_POS_5, 0L, PTZF_POS_5);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn6() 
{
	RequestPTZ(CCC_POS_6, 0L, PTZF_POS_6);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn7() 
{
	RequestPTZ(CCC_POS_7, 0L, PTZF_POS_7);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn8() 
{
	RequestPTZ(CCC_POS_8, 0L, PTZF_POS_8);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSn9() 
{
	RequestPTZ(CCC_POS_9, 0L, PTZF_POS_9);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSnHome() 
{
	RequestPTZ(CCC_POS_HOME, 0L, PTZF_POS_HOME);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_1);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_2);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn3(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_3);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_4);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn5(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_5);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn6(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_6);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn7(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_7);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn8(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_8);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSn9(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_9);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSnHome(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_POS_HOME);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoStop() 
{
	// attention delete's myself
	m_pVisionView->DeleteSmallWindow(GetServerID(),m_orOutput.GetID());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CSmallWindow::OnRButtonDown(nFlags, point);
	if (!theApp.m_bDisableRightClick || !IsFullScreen())
	{
		ClientToScreen(&point);
		PopupMenu(point);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoSnConfig() 
{
	if (IsSn())
	{
		if (m_pSNConfigDialog)
		{
			m_pSNConfigDialog->DestroyWindow();
			m_pSNConfigDialog = NULL;
		}
		else
		{
			CIPCInputVision* pInput = m_pServer->GetInput();
			CIPCOutputVision* pOutput = m_pServer->GetOutput();
			m_pSNConfigDialog = new CSNConfigDialog(pInput,pOutput,m_ccType,m_dwPTZFunctions,this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSnConfig(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_CONFIG);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::SetStore(BOOL bOn)
{
	m_bStoring = bOn;
	UINT nID;
	UINT nStyle; 
	int iImage;

	m_wndToolBar.GetButtonInfo(0,nID,nStyle,iImage);
	iImage = m_bStoring ? 3 : 0;
	m_wndToolBar.SetButtonInfo(0,nID,nStyle,iImage);

	if (WasSometimesActive())
	{
		DoRequestPictures();
	}
	m_wndToolBar.InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoSaveReference(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer 
				   && m_orOutput.IsReference()
		           && (!m_pServer->IsB3())
				   );
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoShowReference(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer 
				   && m_orOutput.IsReference()
		           && (!m_pServer->IsB3())
				   );
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetRemoteReferenceFileName()
{
	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sCam,sPath;
	sHost.Format(_T("%08lx"),m_pServer->GetHostID());
	sCam.Format(_T("%08lx"),GetCamID().GetID());
	sPath = sDir + '\\' + sHost + '\\' + sCam + _T(".jpg");
	return sPath;
}
/////////////////////////////////////////////////////////////////////////////
CString CDisplayWindow::GetLocalReferenceFileName()
{
	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sCam,sPath;
	sHost.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sCam.Format(_T("%08lx"),GetCamID().GetID());
	sPath = sDir + '\\' + sHost + '\\' + sCam + _T(".jpg");
	return sPath;
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnVideoShowReference() 
{
//	m_pVisionView->AddHtmlWindow(m_pServer,GetCamID(),m_orOutput.GetName());
	m_pVisionView->AddReferenceWindow(m_pServer,GetCamID(),m_orOutput.GetName());
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ZoomDisplayWindow(const CPoint &point)
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
BOOL CDisplayWindow::ScrollDisplayWindow()
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

			InvalidateRect(NULL);
			UpdateWindow();
		}
		ShowCursor(TRUE);

		return (StartPoint != CurrentPoint);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnZoomOut() 
{
	m_iCountZooms = 0;
	m_rcZoom.left = m_rcZoom.top = m_rcZoom.right = m_rcZoom.bottom = 0;
 	InvalidateRect(NULL);
 	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_rcZoom.IsRectEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnSmallOriginal() 
{
	if (m_pVisionView->GetNrOfDisplayWindows()==1)
	{
		m_pVisionView->OnViewOriginal();
	}
	else
	{
		m_b1to1 = !m_b1to1;
		Set1to1(m_b1to1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateSmallOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::CheckVideo(DWORD dwCurrentTime) 
{
	if (IsRequesting())
	{
		// 5 s (=5000 ms) since the last request are to long
		DWORD dwVideoTimeout = 10000;
		DWORD dwTimeSpan = GetTimeSpan(m_dwLastPictureConfirmedTime, dwCurrentTime);
		if (dwTimeSpan > dwVideoTimeout)
		{
			WK_TRACE_WARNING(_T("%s VideoTimeout %lu ms, request new picture\n"),
									GetName(), dwTimeSpan);
			DoRequestPictures();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnIndicateError(DWORD dwCmd,CIPCError error,int iErrorCode)
{
	switch (dwCmd)
	{
	case CIPC_OUTPUT_REQUEST_GET_MASK:
		if (WK_IS_WINDOW(m_pMDConfigDlg))
		{
			m_pMDConfigDlg->OnIndicateError(dwCmd,error,iErrorCode);
		}
		break;
	case CIPC_OUTPUT_REQUEST_SET_MASK:
		if (WK_IS_WINDOW(m_pMDConfigDlg))
		{
			m_pMDConfigDlg->OnIndicateError(dwCmd,error,iErrorCode);
		}
		break;
	default:
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::CanMultipleRequests()
{
	if (GetServer()->IsDTS())
	{
		if (    GetServer()->IsISDN()
			||  GetServer()->IsDun()
			||  (   GetServer()->IsTCPIP()
			     &&	IsCoco())
			)
		{
			if (GetResolution() == RESOLUTION_HIGH)
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
			if (IsCoco())
			{
				CCocoWindow* pCW = (CCocoWindow*)this;
				return pCW->IsJPEG2H263();
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::SetActiveRequested(BOOL bActive/*=TRUE*/)
{
	if (m_bIsRequesting != bActive)
	{
		if (bActive == FALSE)
		{
			SetActive(FALSE);
			if (CanMultipleRequests())
			{
				// ich will nicht deaktiviert werden
//				TRACE(_T("%s cannot be deactivated by multiple requests allowed\n"),GetName());
				return FALSE;
			}
		}
		m_bIsRequesting = bActive;

		InvalidateRect(NULL,FALSE);

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
void CDisplayWindow::OnVideoMask() 
{
	if (m_pMDConfigDlg)
	{
		m_pMDConfigDlg->BringWindowToTop();
	}
	else
	{
		m_pMDConfigDlg = new CMDConfigDlg(GetID(),this);
	}
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateVideoMask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pServer!=NULL 
				   && m_pServer->CanActivityMask()
				   && m_sMode == CSD_MD);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ConfirmGetMask(const CIPCActivityMask& mask)
{
	if (WK_IS_WINDOW(m_pMDConfigDlg))
	{
		m_pMDConfigDlg->ConfirmGetMask(GetID(),mask);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm get mask no MD Config Dialog %08lx\n"),GetID().GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayWindow::ConfirmSetMask(MaskType type)
{
	if (WK_IS_WINDOW(m_pMDConfigDlg))
	{
		m_pMDConfigDlg->ConfirmSetMask(GetID(),type);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm set mask no MD Config Dialog %08lx\n"),GetID().GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
int CDisplayWindow::GetRequestedFPS()
{
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::IsStreaming()
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
///////////////////////////////////////////////////////////////////
BOOL CDisplayWindow::operator < (CDisplayWindow & second)
{
	if (m_pVisionView->GetCmdActiveDisplayWindow() == this)
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
void CDisplayWindow::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_uTimerID)
	{
		CSmallWindow::OnTimer(nIDEvent);
		m_pVisionView->SetRequestedDisplayWindow(this);
	}
	if (nIDEvent == m_uTimerTest)
	{
		if (KillTimer(m_uTimerTest))
		{
			m_uTimerTest = NULL;
		}
		m_dwPTZFunctions = m_dwPTZTemp;
		WK_TRACE(_T("%s CC Func %08lx\n"),GetName(),m_dwPTZFunctions);
		if (   m_pContextMenu
			&& m_pContextMenu->GetSafeHmenu( )
			)
		{
			COemGuiApi::DoUpdatePopupMenu(this,m_pContextMenu);
		}
		if (WK_IS_WINDOW(m_pVideoSNDialog))
		{
			m_pVideoSNDialog->SetPTZFunctions(m_dwPTZFunctions);
		}
	}
}
///////////////////////////////////////////////////////////////////
void CDisplayWindow::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	// Not used, was needed for testing
//	TRACE(_T("%s Enter menu\n"),GetName());
}
///////////////////////////////////////////////////////////////////
void CDisplayWindow::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
//	TRACE(_T("%s Exit menu\n"),GetName());
	// if the menu is exited, the pointer has to be released
	m_pContextMenu = NULL;
}
///////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!IsFullScreen());
	}
}
///////////////////////////////////////////////////////////////////
void CDisplayWindow::OnUpdateSmallContext(CCmdUI* pCmdUI) 
{
	if (theApp.m_bDisableSmall)
	{
		pCmdUI->Enable(!IsFullScreen());
	}
}
