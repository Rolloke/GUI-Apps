// VideteStartupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "replall.h"
#include "VideteStartupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideteStartupDlg dialog
#define CLOSE_STARTUP_EVENT   0x00023591

CVideteStartupDlg::CVideteStartupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideteStartupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideteStartupDlg)
	//}}AFX_DATA_INIT
   m_hFontObj    = NULL;
   m_hBrush      = NULL;
   m_bAutoDelete = false;
   m_nCloseStartupTimeout = AfxGetApp()->GetProfileInt(DEFAULT_SECTION, STARTUP_TIMEOUT, -1);
	if (m_nCloseStartupTimeout == -1)
	{
		AfxGetApp()->WriteProfileInt(DEFAULT_SECTION, STARTUP_TIMEOUT, 4000);
	}
   if (m_nCloseStartupTimeout < 0) m_nCloseStartupTimeout = 0;
}

void CVideteStartupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideteStartupDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideteStartupDlg, CDialog)
	//{{AFX_MSG_MAP(CVideteStartupDlg)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CK_START_DISABLE, OnCkStartDisable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideteStartupDlg message handlers

/*********************************************************************************************
 Klasse  : CVideteStartupDlg
 Funktion: OnInitDialog
 Zweck   : Lädt das Startubbanner (bmp) und Startet den Timer für das Schließen des Dialoges

 Parameter: Keine

 Rückgabewert: Parameter initialisiert (BOOL)
 erstellt am: 28. Mai 2002
 <TITLE OnInitDialog>
*********************************************************************************************/
BOOL CVideteStartupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   CRect rc;
   CString strStartup;

   OSVERSIONINFO os;
   os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&os);
   if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
   {
      HBITMAP hBitmap = NULL;
      CWinApp *pApp = AfxGetApp();
      strStartup = pApp->GetProfileString(DEFAULT_SECTION, "StartUpBmp", "empty");
      if (strStartup == "empty")
      {
         char szFileName[_MAX_PATH];
         GetModuleFileName(AfxGetInstanceHandle(), szFileName, _MAX_PATH-1);
         int nLen = strlen(szFileName);
         szFileName[nLen-3] = 0;
         strncat(szFileName, "bmp", _MAX_PATH-1);
         if (::GetFileAttributes(szFileName) != -1)
         {
            strStartup = _T(szFileName);
            pApp->WriteProfileString(DEFAULT_SECTION, "StartUpBmp", strStartup);
         }
      }
      if (::GetFileAttributes(strStartup) != -1)
      {
         hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), strStartup, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTCOLOR);
      }
      if (hBitmap)
      {
         LOGBRUSH lb = {BS_PATTERN,DIB_RGB_COLORS,(long)hBitmap};
         m_hBrush = ::CreateBrushIndirect(&lb);
         ::DeleteObject(hBitmap);
      }
   }

   CWnd *pWnd = GetDlgItem(IDC_TXT_DOKU_TOOL);
   if (pWnd)
   {
      CFont *pFont = pWnd->GetFont();
      LOGFONT lf;
      pFont->GetLogFont(&lf);
      lf.lfHeight *= 2; // MulDiv(lf.lfHeight, 8, 6);
      lf.lfWeight = FW_BOLD;
      strcpy(lf.lfFaceName, "Arial");
      pFont->Detach();
      pFont->CreateFontIndirect(&lf);
      pWnd->SetFont(pFont);
/*
      pWnd = GetDlgItem(IDC_CK_START_DISABLE);
      pWnd->SetFont(pFont);
      pWnd = GetDlgItem(IDC_TXT_DOKU_TOOL1);
      pWnd->SetFont(pFont);
      pWnd = GetDlgItem(IDC_TXT_DOKU_TOOL2);
      pWnd->SetFont(pFont);
*/
      m_hFontObj = (HFONT) pFont->Detach();
   }

   ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
   SetTimer(CLOSE_STARTUP_EVENT, m_nCloseStartupTimeout, NULL);
   UpdateWindow();
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideteStartupDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
   if (CLOSE_STARTUP_EVENT == nIDEvent)
   {
      KillTimer(nIDEvent);
      OnCancel();
      DestroyWindow();
      if (m_hFontObj) ::DeleteObject(m_hFontObj);
      m_hFontObj = NULL;
      if (m_hBrush) ::DeleteObject(m_hBrush);
      m_hBrush = NULL;
   }
}

/*********************************************************************************************
 Klasse  : CVideteStartupDlg
 Funktion: OnCtlColor
 Zweck   : Setzt den Hintergrund des Dialogfeldes und den Hintergrundmodus der
           Darstellungelemente auf transparent.

 Parameter:  
  pDC      : (EA): Gerätekontexthandle zum Zeichnen  (CDC*)
  pWnd     : (E): Fensterhandle  (CWnd*)
  nCtlColor: (E): Art des Elementes, das gezeichnet wird  (UINT)

 Rückgabewert: Hintergrundpinsel (HBRUSH)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE OnCtlColor>
*********************************************************************************************/
HBRUSH CVideteStartupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   if (nCtlColor == CTLCOLOR_STATIC)
   {
   	pDC->SetBkMode(TRANSPARENT);
   }
   if (m_hBrush) return m_hBrush;
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

void CVideteStartupDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_bAutoDelete)
   {
   	delete this;
   }
}

void CVideteStartupDlg::OnCkStartDisable() 
{
   AfxGetApp()->WriteProfileInt(DEFAULT_SECTION, STARTUP_TIMEOUT, 0);
}
