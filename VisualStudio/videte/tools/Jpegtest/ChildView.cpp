// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Jpegtest.h"
#include "ChildView.h"
#include "CDecodeThread.h"
#include "OptionsDialog.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	m_pDecodeThread = new CDecodeThread(this);
	m_bOriginalSize			= TRUE;
	m_bInterpolate			= TRUE;
	m_bNoiseReduction		= TRUE;
	m_bLumCorrection		= FALSE;
	m_bAllowDoubleFieldMode = TRUE;
	m_bDecodeToScreen = TRUE;
	m_bDecodeToHDD = FALSE;
	m_bH263 = FALSE;
	m_iScale = 0;
	m_iHeight = 0;
	m_iWidth = 0;
	m_iFPS = 6;

	CWK_Profile wkp;
	m_bOriginalSize = wkp.GetInt("JpegTest","OriginalSize",m_bOriginalSize);
	m_bInterpolate = wkp.GetInt("JpegTest","Interpolate",m_bInterpolate);
	m_bNoiseReduction = wkp.GetInt("JpegTest","NoiseReduction",m_bNoiseReduction);
	m_bLumCorrection = wkp.GetInt("JpegTest","LumCorrection",m_bLumCorrection);
	m_bAllowDoubleFieldMode = wkp.GetInt("JpegTest","AllowDoubleFieldMode",m_bAllowDoubleFieldMode);
	// Pfad zum Speichern der Jpeg-dateien einlesen
	m_sJPEGPath = wkp.GetString("JaCobUnit\\Device1\\Debug", "JpegPath", ".\\Jpeg\\");

	m_bDecodeToScreen = wkp.GetInt("JpegTest","DecodeToScreen",m_bDecodeToScreen);
	m_bDecodeToHDD = wkp.GetInt("JpegTest","DecodeToHDD",m_bDecodeToHDD);
	m_bH263  = wkp.GetInt("JpegTest","H263",m_bH263);
	m_iScale = wkp.GetInt("JpegTest","Scale",m_iScale);
	m_iHeight = wkp.GetInt("JpegTest","Height",m_iHeight);
	m_iWidth = wkp.GetInt("JpegTest","Width",m_iWidth);
	m_iFPS = wkp.GetInt("JpegTest","FPS",m_iFPS);

}

CChildView::~CChildView()
{
	CWK_Profile wkp;
	wkp.WriteInt("JpegTest","OriginalSize",m_bOriginalSize);
	wkp.WriteInt("JpegTest","Interpolate",m_bInterpolate);
	wkp.WriteInt("JpegTest","NoiseReduction",m_bNoiseReduction);
	wkp.WriteInt("JpegTest","LumCorrection",m_bLumCorrection);
	wkp.WriteInt("JpegTest","AllowDoubleFieldMode",m_bAllowDoubleFieldMode);
	wkp.WriteInt("JpegTest","DecodeToScreen",m_bDecodeToScreen);
	wkp.WriteInt("JpegTest","DecodeToHDD",m_bDecodeToHDD);
	wkp.WriteInt("JpegTest","H263",m_bH263);
	wkp.WriteInt("JpegTest","Scale",m_iScale);
	wkp.WriteInt("JpegTest","Height",m_iHeight);
	wkp.WriteInt("JpegTest","Width",m_iWidth);
	wkp.WriteInt("JpegTest","FPS",m_iFPS);

	WK_DELETE(m_pDecodeThread);
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(IDM_1_1, OnOriginalSize)
	ON_UPDATE_COMMAND_UI(IDM_1_1, OnUpdateOriginalSize)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDM_INTERPOLATE, OnInterpolate)
	ON_UPDATE_COMMAND_UI(IDM_INTERPOLATE, OnUpdateInterpolate)
	ON_COMMAND(IDM_ALLOW2FIELD, OnAllow2field)
	ON_UPDATE_COMMAND_UI(IDM_ALLOW2FIELD, OnUpdateAllow2field)
	ON_COMMAND(IDM_NR, OnNoiseReduction)
	ON_UPDATE_COMMAND_UI(IDM_NR, OnUpdateNoiseReduction)
	ON_COMMAND(IDM_LUMCORRECTION, OnLumcorrection)
	ON_UPDATE_COMMAND_UI(IDM_LUMCORRECTION, OnUpdateLumcorrection)
	ON_COMMAND(ID_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_WM_DESTROY()
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS, OnUpdateOptions)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CRect rc(0,0,0,0);

	if (!m_bOriginalSize)
		GetClientRect(rc);

	if (DecodeToScreen())
		m_pDecodeThread->OnDraw(&dc, rc);

	if (H263() && !DecodeToScreen())
	{
		if (m_pDecodeThread->GetCurrentFrame()>0
			&& m_pDecodeThread->IsRunning())
		{
			CString s;
			s.Format("encoding frame %d",m_pDecodeThread->GetCurrentFrame());
			dc.TextOut(5,5,s);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnPlay() 
{
	if (!m_pDecodeThread->IsRunning())
	{
		m_pDecodeThread->StartThread();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnOriginalSize() 
{
	m_bOriginalSize = !m_bOriginalSize;
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateOriginalSize(CCmdUI* pCmdUI) 
{
	if (pCmdUI)
		pCmdUI->SetCheck(m_bOriginalSize);
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnInterpolate() 
{
	m_bInterpolate = !m_bInterpolate;	
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateInterpolate(CCmdUI* pCmdUI) 
{
	if (pCmdUI)
		pCmdUI->SetCheck(m_bInterpolate);
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnAllow2field() 
{
	m_bAllowDoubleFieldMode = !m_bAllowDoubleFieldMode;	
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateAllow2field(CCmdUI* pCmdUI) 
{
	if (pCmdUI)
		pCmdUI->SetCheck(m_bAllowDoubleFieldMode);
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rcClient;
	GetClientRect(rcClient);

	point.x = point.x * 704 / rcClient.Width();
	point.y = point.y * 280 / rcClient.Height();
	
	CRect rcSelection(max(point.x-50, 0),
					  max(point.y-25, 0),
					  min(point.x+50, 704),
					  min(point.y+25, 280));

	//rcSelection = CRect(0,0,704,280);
	//m_DecodeThread.SetSelectionRect(rcSelection);

	CWnd ::OnMouseMove(nFlags, point);
}


void CChildView::OnNoiseReduction() 
{
	m_bNoiseReduction = !m_bNoiseReduction;	
}

void CChildView::OnUpdateNoiseReduction(CCmdUI* pCmdUI) 
{
	if (pCmdUI)
		pCmdUI->SetCheck(m_bNoiseReduction);
}
////////////////////////////////////////////////////////////////////////////
void CChildView::OnLumcorrection() 
{
	m_bLumCorrection = !m_bLumCorrection;	
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateLumcorrection(CCmdUI* pCmdUI) 
{
	if (pCmdUI)
		pCmdUI->SetCheck(m_bLumCorrection);
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnStop() 
{
	// TODO: Add your command handler code here

	m_pDecodeThread->StopThread();
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdatePlay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pDecodeThread->IsRunning());
}
/////////////////////////////////////////////////////////////////////////////
static char BASED_CODE szFilter[] = "JPEG Dateien (J_.jpg)|J_??????.jpg||";
void CChildView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CString sDir = m_sJPEGPath.Left(m_sJPEGPath.GetLength()-1);
	CFileDialog dlg(TRUE,"jpg",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	dlg.m_ofn.lpstrInitialDir = sDir;
	if (dlg.DoModal())
	{
		CString sPathname = dlg.GetPathName();
		theApp.AddToRecentFileList(sPathname);
		m_sJPEGPath = sPathname.Left(sPathname.ReverseFind('\\')+1);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_pDecodeThread->IsRunning());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CChildView::OpenDocumentFile(LPCTSTR lpszFileName)
{
	if (DoesFileExist(lpszFileName))
	{
		CString sPathname = lpszFileName;
		m_sJPEGPath = sPathname.Left(sPathname.ReverseFind('\\')+1);
		TRACE("JPEGPath is now %s\n",m_sJPEGPath);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnDestroy() 
{
	CWnd ::OnDestroy();
	
	if (m_pDecodeThread->IsRunning())
	{
		m_pDecodeThread->StopThread();
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnOptions() 
{
	COptionsDialog dlg;

	dlg.m_bDecodeToHDD = m_bDecodeToHDD;
	dlg.m_bDecodeToScreen = m_bDecodeToScreen;
	dlg.m_bNoiseReduction = m_bNoiseReduction;
	dlg.m_bBrightness = m_bLumCorrection;
	dlg.m_bH263 = m_bH263;
	dlg.m_iHeight = m_iHeight;
	dlg.m_iWidth = m_iWidth;
	dlg.m_iScale = m_iScale;
	dlg.m_iFPS = m_iFPS;

	if (dlg.DoModal())
	{
		m_bDecodeToHDD = dlg.m_bDecodeToHDD;
		m_bDecodeToScreen = dlg.m_bDecodeToScreen;
		m_bNoiseReduction = dlg.m_bNoiseReduction;
		m_bLumCorrection = dlg.m_bBrightness;
		m_bH263 = dlg.m_bH263;
		m_iHeight = dlg.m_iHeight;
		m_iWidth = dlg.m_iWidth;
		m_iScale = dlg.m_iScale;
		m_iFPS = dlg.m_iFPS;
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_pDecodeThread->IsRunning());
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CChildView::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WPARAM_IMAGE_DECODED:
		{
			CSize s = m_pDecodeThread->GetJPEG().GetImageDims();
			CMainFrame* pMF = (CMainFrame*)GetParentFrame();
			CRect newCR,oldCR,oldFR,newFR;
			newCR.left = 0;
			newCR.top = 0;
			newCR.right = s.cx;
			newCR.bottom = s.cy;
			GetClientRect(oldCR);
			pMF->GetWindowRect(oldFR);
			newFR = oldFR;
			newFR.right -= oldCR.Width() - newCR.Width();
			newFR.bottom -= oldCR.Height() - newCR.Height();
			if (newFR != oldFR)
			{
				pMF->SetWindowPos(NULL,newFR.left,newFR.top,newFR.Width(),newFR.Height(),0);
			}
		}
		InvalidateRect(NULL,FALSE);
		break;
	}

	return 0L;
}
