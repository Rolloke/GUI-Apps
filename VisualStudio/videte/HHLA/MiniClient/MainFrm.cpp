// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MiniClient.h"
#include "Cipc.h"
#include "CIPCOutputMiniClient.h"
#include "CIPCServerControl.h"
#include "CIPCFetchResult.h"
#include "CIPCServerControlClientSide.h"
#include "ConnectionRecord.h"
#include "WK_Names.h"
#include "User.h"
#include "Permission.h"
#include "oemgui\oemguiapi.h"
#include "Imagn.h"
#include "Ini2Reg.h"
#include "MiniClientDef.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMiniClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_INITMENU()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_SHOW_IMAGE,			OnShowImage)
	ON_MESSAGE(WM_SET_CLIENT_NUMBER,	OnRequestSetClientNumber)
	ON_MESSAGE(WM_SET_INPUT_SOURCE,		OnRequestSetInputSource)
	ON_MESSAGE(WM_SET_WINDOW_POSITION,	OnRequestSetWindowPosition)
	ON_MESSAGE(WM_SAVE_PICTURE,			OnRequestSavePicture)
	ON_MESSAGE(WM_SET_CAMERA_PARAMETER,	OnRequestSetCameraParameter)
	ON_MESSAGE(WM_REQ_CAMERA_PARAMETER,	OnRequestCameraParameter)
	ON_MESSAGE(WM_CHECK_CAMERA,			OnRequestCheckCamera)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_pUser				= NULL;
	m_pPermission		= NULL;
	m_pOutput			= NULL;
	m_wClientNr			= 1;
	m_bActive			= FALSE;
	m_pPictRecord		= NULL;
	m_sSection			= "";
	m_hWndRemoteControl	= NULL;
	m_sCamName			= "";
	m_nRefreshType		= UNKNOWN;
	m_pJpeg				= NULL;
	m_rcZoom			= CRect(0,0,0,0);

	m_sSection.Format(_T("MiniClient\\Client%u"), m_wClientNr);

	// Informationen ¸ber das Ausgabedevice erfragen
	HDC hDC				= CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	m_nHRes				= GetDeviceCaps(hDC, HORZRES);				// Pixel pro Zeile
	m_nVRes				= GetDeviceCaps(hDC, VERTRES);				// Anzahl der Zeilen
	DeleteDC(hDC);


}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::FetchOutputConnection()
{
	CIPCFetchResult FetchResult;
	CString			sSmName;
	BOOL			bRetry = FALSE;
	WORD			wI = 0;

	if (!m_pUser || !m_pPermission)
	{
		WK_TRACE_ERROR(_T("CMainFrame::FetchOutputConnection\t	m_pUser or m_pPermission == NULL\n"));
		return FALSE;
	}

	CConnectionRecord ConRec(LOCAL_LOOP_BACK, *m_pUser,m_pPermission->GetName(), m_pPermission->GetPassword());
	
	ConRec.SetOptions(SCO_WANT_CAMERAS_COLOR);
	
	// Try to connect
	do{
		FetchResult = NewFetchOutputConnection(ConRec);
		bRetry = FALSE;
		if ((FetchResult.GetError() == CIPC_ERROR_UNABLE_TO_CONNECT) && (FetchResult.GetErrorCode() == 5))
		{
			WK_TRACE_ERROR(_T("Fetch Outputconnection failed, try again\n"));
			bRetry = TRUE;
			int iRandom = (100*rand()) / RAND_MAX;
			Sleep(50+iRandom);
		}
	}while(bRetry || wI++ > 50);

	// Connection not successfull
	if (bRetry)
	{
		WK_TRACE_ERROR(_T("Give up to Fetch Outputconnection\n"));
		return FALSE;
	}
	// Get Sharedmemoryname
	sSmName = FetchResult.GetShmName();

	if (sSmName.IsEmpty())
		return FALSE;

	m_pOutput = new CIPCOutputMiniClient(this, sSmName);

	if (!m_pOutput)
	{
		WK_TRACE_ERROR(_T("CMainFrame::FetchOutputConnection\t	m_pOutput == NULL\n"));
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	WK_DELETE(m_pOutput);
	WK_DELETE(m_pPictRecord);
	WK_DELETE(m_pJpeg);
	WK_DELETE(m_pPermission);
	WK_DELETE(m_pUser);
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pJpeg	= new CJpeg();

	if (!Login())					 
	{
		WK_TRACE_ERROR(_T("Login failed\n"));
		return -1;
	}

	// Outputconnection herstelen
	if (!FetchOutputConnection())
	{
		WK_TRACE_ERROR(_T("FetchOutputConnection failed\n"));
		return -1;
	}
	else
		WK_TRACE(_T("FetchOutputConnection successfull\n"));

	// L‰uft der Client im Testmodus?
	if (theApp.m_bMiniClientTest)
	{
		m_wClientNr = (WORD)theApp.m_nCamNr;
		if (m_pOutput)
			m_pOutput->RequestPictures(m_wClientNr);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.dwExStyle |= WS_EX_TOOLWINDOW;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::Login()
{
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	BOOL bRet = FALSE;
	CWK_Profile prof;

	do
	{
		UserArray.Load(prof);		   

		// Nimm den erstbesten User zum Einloggen.
		CUser *pUser = UserArray.GetUser(UserArray[0]->GetID()); 
		sUser = pUser->GetName();
		sPassword = pUser->GetPassword();

		idUser = UserArray.IsValid(sUser,sPassword);
		
		if (idUser.IsUserID())
		{
			CUser* pUser;
			pUser = UserArray.GetUser(idUser);
			if (pUser)
			{
				CPermission* pPermission;
				CPermissionArray PermissionArray;

				WK_DELETE(m_pUser);
				m_pUser = new CUser(*pUser);
				if (m_pUser)
				{
					PermissionArray.Load(prof);
					pPermission = PermissionArray.GetPermission(m_pUser->GetPermissionID());
					if (pPermission)
					{
						WK_DELETE(m_pPermission);
						m_pPermission = new CPermission(*pPermission);
						bRet = TRUE;
					}
				}
			}
		}

		if (bRet)
		{
			if (m_pUser)
				WK_TRACE(_T("Login Nutzer:%s, Berechtigungsstufe:%s\n"),m_pUser->GetName(),m_pPermission->GetName());
		}
		else
		{
			WK_TRACE_WARNING(_T("Login failed !!! Nutzer:%s\n"),(LPCTSTR)sUser);
		}
	}
	while(!bRet);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnShowImage(WPARAM wParam, LPARAM lParam)
{
	CIPCPictureRecord *pPict = (CIPCPictureRecord*)lParam;
		
	if (pPict && m_pOutput)
	{
		// Das letzte Bild zwecks evtl. Speicherns sichern
		WK_DELETE(m_pPictRecord);
		m_pPictRecord = new CIPCPictureRecord(*pPict);
		
		// Soll das Bild gleich angezeigt werden?
		if (m_nRefreshType != FREEZE_IT)
		{
			// Name der Kamera anzeigen
			m_sCamName = m_pOutput->GetOutputRecordFromSecID(pPict->GetCamID()).GetName();
			SetWindowText(m_sCamName);

			m_pJpeg->DecodeJpegFromMemory((BYTE*)pPict->GetData(), pPict->GetDataLength());
			InvalidateRect(NULL);
			WK_DELETE(pPict)
		}
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestSetInputSource(WPARAM wParam, LPARAM lParam)
{
	if (!m_pOutput)
		return 0;

	// Ist die Kamera ¸berhaupt zugeschaltet (Siehe Telegramm TYP01)
	if (m_bActive)
	{
		m_nRefreshType = lParam;
	
		if (wParam == TRUE)			// Minimized
			ShowWindow(SW_HIDE);
		else 
			ShowWindow(SW_SHOW);	// Default view

		switch (m_nRefreshType)
		{
			case FREEZE_IT:
			case REFRESH_IT:
				m_pOutput->RequestPictures(m_wClientNr);
				break;
			case SHOW_IT:
				OnShowImage(0, (LPARAM)new CIPCPictureRecord(*m_pPictRecord));
				break;
			default:
				m_nRefreshType = UNKNOWN;
				WK_TRACE_ERROR(_T("OnRequestSetInputSource Unknown RefreshType (%lu)\n"), m_nRefreshType);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
	int nX = (short int)LOWORD(wParam);
	int	nY = (short int)HIWORD(wParam);
	int nW = (short int)LOWORD(lParam);
	int nH = (short int)HIWORD(lParam); 
	CRect	rcCurrent(0,0,0,0);

	GetWindowRect(rcCurrent);

	// Output Ok?
	if (!m_pOutput)
		return 0;

	if ((nX == -1) && (nY == -1) && (nW == -1) && (nH == -1))
	{
		SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_HIDEWINDOW);
		m_bActive = FALSE;
	}
	else if ((nW == -1) || (nH == -1))
	{	
		m_pOutput->RequestPictures(m_wClientNr);
		if (rcCurrent != CRect(nX, nY, nX+nW, nY+nH))
			SetWindowPos(&CWnd::wndTop, nX, nY, nW, nH, SWP_SHOWWINDOW | SWP_NOSIZE);
		m_bActive = TRUE;
	}
	else if ((nX == -1) || (nY == -1))
	{
		m_pOutput->RequestPictures(m_wClientNr);
		if (rcCurrent != CRect(nX, nY, nX+nW, nY+nH))
			SetWindowPos(&CWnd::wndTop, nX, nY, nW, nH, SWP_SHOWWINDOW | SWP_NOMOVE);
		m_bActive = TRUE;
	}		  
	else
	{
		m_pOutput->RequestPictures(m_wClientNr);
		if (rcCurrent != CRect(nX, nY, nX+nW, nY+nH))
			SetWindowPos(&CWnd::wndTop, nX, nY, nW, nH, SWP_SHOWWINDOW);
		m_bActive = TRUE;
	}

	// Letzte Fensterposition merken
/*	MyWritePrivateProfileInt(_T("Pos"),	_T("X"), nX, m_sSection);	
	MyWritePrivateProfileInt(_T("Pos"),	_T("Y"), nY, m_sSection);	
	MyWritePrivateProfileInt(_T("Pos"),	_T("W"), nW, m_sSection);	
	MyWritePrivateProfileInt(_T("Pos"),	_T("H"), nH, m_sSection);	
*/
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestSavePicture(WPARAM wParam, LPARAM lParam)
{
	TCHAR   szFile[256];
	CString sFileName = _T("");

	if (!m_pPictRecord || !m_pJpeg)
		return 0;
	
	MyGetPrivateProfileString(_T("JpegFile"),	_T("Path"), _T("C:\\Unknown.jpg"), szFile, 255, m_sSection);	
	sFileName = szFile;
	MyWritePrivateProfileString(_T("JpegFile"),	_T("Path"), _T(""), m_sSection);	

	// Jpegbild speichern.
	TRY
	{
		HANDLE hDIB = m_pJpeg->CreateDIB();
		if (hDIB)
		{
			m_pJpeg->EncodeJpegToFile(hDIB, sFileName);
			GlobalFree(hDIB);		
		}
	}
	CATCH(CFileException, e)
	{
		WK_TRACE_ERROR(_T("Can't save file <%s>\n"), sFileName);
	}
	END_CATCH

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);
	
	m_pJpeg->OnDraw(&dc, rcClient, m_rcZoom);

	// Wenn gezoomt ist, dies in der Titelzeile sichtbar machen
	if (m_rcZoom.IsRectEmpty())
		SetWindowText(m_sCamName);
	else
		SetWindowText(m_sCamName + _T(" (gezoomt)"));
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMove(int x, int y) 
{
	InvalidateRect(NULL);
	CFrameWnd::OnMove(x, y);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	InvalidateRect(NULL);
	CFrameWnd::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestSetClientNumber(WPARAM wParam, LPARAM lParam)
{
	m_wClientNr			= (WORD)wParam;
	m_hWndRemoteControl	= (HWND)lParam;

	if (m_hWndRemoteControl == NULL)
		WK_TRACE_ERROR(_T("OnRequestSetClientNumber\t m_hWndRemoteControl == NULL\n"));

	m_sSection.Format(_T("MiniClient\\Client%u"), m_wClientNr);

	// Debugger ˆffnen
	CString sLogFile;
	sLogFile.Format(_T("MiniClient%d.log"), m_wClientNr);
	InitDebugger(sLogFile, WAI_EXPLORER, NULL);

	WK_TRACE(_T("Starting MiniClient...\n"));

	// Fensterposition laden
/*	int nX = MyGetPrivateProfileInt(_T("Pos"), _T("X"), -1, m_sSection);	
	int nY = MyGetPrivateProfileInt(_T("Pos"), _T("Y"), -1, m_sSection);	
	int nW = MyGetPrivateProfileInt(_T("Pos"), _T("W"), -1, m_sSection);	
	int nH = MyGetPrivateProfileInt(_T("Pos"), _T("H"), -1, m_sSection);	

	PostMessage(WM_SET_WINDOW_POSITION, MAKELONG(nX, nY), MAKELONG(nW, nH));
*/
	// OOPS Zum Test auf 16 Kameras beschr‰nken
//	if (m_wClientNr > 16)
//		m_wClientNr -= 16;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestSetCameraParameter(WPARAM wParam, LPARAM lParam)
{
	int	nBrightness = LOWORD(wParam); 
	int	nContrast	= LOWORD(lParam); 
	int	nSaturation = HIWORD(lParam); 		    

	if (m_pOutput)
	{
		m_pOutput->SetCameraParameter(m_wClientNr,  (nBrightness * 256 + 50) / 100,
													(nContrast   * 512 + 50) / 100,
													(nSaturation * 512 + 50) / 100);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestCameraParameter(WPARAM wParam, LPARAM lParam)
{
	if (m_pOutput)
		m_pOutput->RequestCameraParameter(m_wClientNr);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnRequestCheckCamera(WPARAM wParam, LPARAM lParam)
{
	if (m_pOutput)
		m_pOutput->RequestPictures(m_wClientNr);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!(m_rcZoom.IsRectEmpty()))
	{
		ZoomDisplayWindow(point,FALSE);
	}
	
	CFrameWnd::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	// Fenster d¸rfen nicht aus dem Bildschirm geschoben werden.
	lpwndpos->x = min(max(lpwndpos->x, 0), m_nHRes - lpwndpos->cx);
	lpwndpos->y = min(max(lpwndpos->y, 0), m_nVRes - lpwndpos->cy);

	CFrameWnd::OnWindowPosChanging(lpwndpos);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!ScrollDisplayWindow())
	{
		ZoomDisplayWindow(point, TRUE);
	}
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ZoomDisplayWindow(const CPoint &point, BOOL bIn)
{
	if (!m_pJpeg->IsEmpty())
	{
		CRect rcVideo,rcTrack,rcZoom;
		int		nX1, nY1, nX2, nY2;
		CSize s = m_pJpeg->GetImageDims();

		GetClientRect(rcVideo);
		if (m_rcZoom.IsRectEmpty())
		{
			m_rcZoom = CRect(0,0, s.cx, s.cy);
		}

		if (bIn)
		{
			rcTrack.left	= point.x - rcVideo.Width()  / 4;
			rcTrack.top		= point.y - rcVideo.Height() / 4;
			rcTrack.right	= point.x + rcVideo.Width()  / 4;
			rcTrack.bottom	= point.y + rcVideo.Height() / 4;

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
		
			// Den maximalen Zoomfaktor begrenzen
			if (s.cx / m_rcZoom.Width() < 5)
				m_rcZoom = CRect(nX1, nY1, nX2, nY2);
			else
				MessageBeep(0);
		}
		else
		{
			m_rcZoom=CRect(0,0,0,0);
		}

		RedrawWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ScrollDisplayWindow()
{
	CPoint StartPoint(0,0);					 
	CPoint CurrentPoint(0,0);					 
	CRect  rcSavedZoom;
 	rcSavedZoom = m_rcZoom;

	SIZE size = m_pJpeg->GetImageDims(); 
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

	int dx = abs(StartPoint.x - CurrentPoint.x);
	int dy = abs(StartPoint.y - CurrentPoint.y);
	
	return (dx >= 1 || dy >= 1);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetCursor(theApp.LoadCursor(IDC_CURSOR1));
	
	CFrameWnd::OnMouseMove(nFlags, point);
}
