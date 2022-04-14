// ETS3DGLDLGDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "ETS3DGLDLGDlg.h"
#include "Ets3dGLRegKeys.h"
#include "ReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGDlg Dialogfeld

CETS3DGLDLGDlg::CETS3DGLDLGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CETS3DGLDLGDlg::IDD, pParent)
{
   m_hIcon = NULL;
   OnDefault();

   //{{AFX_DATA_INIT(CETS3DGLDLGDlg)
	//}}AFX_DATA_INIT

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   pApp->SetProfileName(NULL, CETS3DGL_REGKEY);
   m_bBugFixBufferSize   = pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_BUFFERSIZE  , m_bBugFixBufferSize);
   m_nBugFixBuffer       = pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_BUFFER      , m_nBugFixBuffer);
   m_bBugFixMoveWindow   = pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_MOVEWINDOW  , m_bBugFixMoveWindow);
   m_bBugFixNormalVector = pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_NORMALVECTOR, m_bBugFixNormalVector);
   m_bBugFixLightTexture = pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_LIGHTTEXTURE, m_bBugFixLightTexture);
   m_nPixelFormat        = pApp->GetProfileInt(""           , REGKEY_PIXELFORMAT        , m_nPixelFormat);

   m_nPolygonOffset      = (int)pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_POLYGON_OFFSET, m_nPolygonOffset);
   m_fPolygonFactor      = 0.00001f * (int)pApp->GetProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_DISTANCE_FACTOR, (int)(m_fPolygonFactor*100000));

   if (pApp->SetProfileName(CARAWALK_MODULEFILENAME, CARAWALK_REGKEY))
   {
      m_bCaraWalkAccelerated =!pApp->GetProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_GENERIC       , !m_bCaraWalkAccelerated);
      m_bCaraWalkBmp         =!pApp->GetProfileInt(CARAWALK_GL_SETTINGS, REGKEY_DEFAULT_GLDIRECT, !m_bCaraWalkBmp);
      m_bCaraWalkDoubleBuffer= pApp->GetProfileInt(CARAWALK_GL_SETTINGS, REGKEY_DEFAULT_GLDOUBLE,  m_bCaraWalkDoubleBuffer);
      m_bCaraWalkShadows     = pApp->GetProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_BOX_SHADOWS   ,  m_bCaraWalkShadows);
      m_bInit3Dalways        = pApp->GetProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_INIT_3D_ALWAYS,  m_bInit3Dalways);
   }
   else 
   {
      m_bCaraWalkAccelerated = NOT_AVAILABLE;
   }

   if (pApp->SetProfileName(ETS3DV_MODULEFILENAME, ETS3DV_REGKEY))
   {
      m_bEts3DViewAccelerated =!pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_GLGENERIC, !m_bEts3DViewAccelerated);
      m_bEts3DVBmp            =!pApp->GetProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_GLDIRECT,   !m_bEts3DVBmp);
      m_bEts3DVDoubleBuffer   = pApp->GetProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_GLDOUBLE,    m_bEts3DVDoubleBuffer);
   }
   else
   {
      m_bEts3DViewAccelerated = NOT_AVAILABLE;
   }
}

void CETS3DGLDLGDlg::OnDefault() 
{  // m_hIcon wird als Indikator für das Updaten mißbraucht !!!
   bool bUpdate = (m_hIcon == NULL) ? false : true;

	m_bBugFixBufferSize     = BUGFIX_BUFFERSIZE_DEFAULT;
	m_bBugFixLightTexture   = BUGFIX_LIGHTTEXTURE_DEFAULT;
	m_bBugFixMoveWindow     = BUGFIX_MOVEWINDOW_DEFAULT;
	m_bBugFixNormalVector   = BUGFIX_NORMALVECTOR_DEFAULT;

   m_nPolygonOffset = BUGFIX_POLYGON_OFFSET_DEFAULT;
	m_fPolygonFactor = BUGFIX_DISTANCE_FACTOR_DEFAULT;
	m_nBugFixBuffer  = BUGFIX_BUFFER_DEFAULT;
	m_nPixelFormat   = 0;

   m_bCaraWalkBmp          = !CARAWALK_GLDIRECT_DEFAULT;
	m_bCaraWalkDoubleBuffer =  CARAWALK_GLDOUBLE_DEFAULT;
	m_bCaraWalkAccelerated  = !CARAWALK_GENERIC_DEFAULT;
	m_bCaraWalkShadows      = CARAWALK_BOX_SHADOWS_DEFAULT;
	m_bInit3Dalways         = CARAWALK_INIT_3D_ALWAYS_DEFAULT;

   m_bEts3DVBmp            = !SETTINGS_GLDIRECT_DEFAULT;
	m_bEts3DVDoubleBuffer   =  SETTINGS_GLDOUBLE_DEFAULT;
	m_bEts3DViewAccelerated = !SETTINGS_GLGENERIC_DEFAULT;
   if (bUpdate) UpdateData(false);
   m_bDebugFileChanged = false;
}

CETS3DGLDLGDlg::~CETS3DGLDLGDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}

void CETS3DGLDLGDlg::DoDataExchange(CDataExchange* pDX)
{
   bool bChange   = false;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CETS3DGLDLGDlg)
	DDX_Check(pDX, IDC_BUGFIX_INIT_ALWAYS, m_bInit3Dalways);
	//}}AFX_DATA_MAP

   // Bugfixes für die Grafikteiber
	DDX_Check(pDX, IDC_BUGFIX_BUFFERSIZE, m_bBugFixBufferSize);
	DDX_Check(pDX, IDC_BUGFIX_LIGHTTEXTURE, m_bBugFixLightTexture);
	DDX_Check(pDX, IDC_BUGFIX_MOVEWINDOW, m_bBugFixMoveWindow);
	DDX_Check(pDX, IDC_BUGFIX_NORMALVECTOR, m_bBugFixNormalVector);
   DDX_CBIndex(pDX, IDC_BUGFIX_BUFFER, m_nBugFixBuffer);
   // Ausgabe der Materialpolygone
	DDX_Text(pDX, IDC_POLYGON_OFFSET, m_nPolygonOffset);
   DDV_MinMaxInt(pDX, m_nPolygonOffset, 0, 16);
   DDX_TextVar(pDX, IDC_POLYGON_FACTOR, FLT_DIG, m_fPolygonFactor);
   if (m_nPolygonOffset == 0)
   {  
      if (m_fPolygonFactor <= 0.0001f)
      {
         m_fPolygonFactor = 0.05f;
         bChange          = true;
      }
	   DDV_MinMaxFloat(pDX, m_fPolygonFactor, 0.0001f, 0.5f);
   }
   else
   {
	   DDV_MinMaxFloat(pDX, m_fPolygonFactor, -1.f, 1.f);
   }
   // Raum und Lautsprecheransicht (CARACAD)
	DDX_Check(pDX, IDC_CARAWALK_ACCELERATED  , m_bCaraWalkAccelerated);
	DDX_Check(pDX, IDC_CARAWALK_BMP          , m_bCaraWalkBmp);
	DDX_Check(pDX, IDC_CARAWALK_DOUBLE_BUFFER, m_bCaraWalkDoubleBuffer);
	DDX_Check(pDX, IDC_CARAWALK_SHADOWS      , m_bCaraWalkShadows);
   // Ergebnisdarstellung (ETS3DVIEW)
   DDX_Check(pDX, IDC_ETS3DV_ACCELERATED, m_bEts3DViewAccelerated);
	DDX_Check(pDX, IDC_ETS3DV_BMP, m_bEts3DVBmp);
	DDX_Check(pDX, IDC_ETS3DV_DOUBLE_BUFFER, m_bEts3DVDoubleBuffer);
   // Pixelformat
   DDX_Text(pDX, IDC_PIXELFORMAT, m_nPixelFormat);
	DDV_MinMaxInt(pDX, m_nPixelFormat, 0, 10000);
   
   if (pDX->m_bSaveAndValidate && bChange)
   {
      pDX->m_bSaveAndValidate = false;
   	DDX_Text(pDX, IDC_POLYGON_FACTOR, m_fPolygonFactor);
      pDX->m_bSaveAndValidate = true;
   }
}

BEGIN_MESSAGE_MAP(CETS3DGLDLGDlg, CDialog)
	//{{AFX_MSG_MAP(CETS3DGLDLGDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_REPORT, OnReport)
	ON_BN_CLICKED(IDC_CARAWALK_ACCELERATED, OnCarawalkAccelerated)
	ON_BN_CLICKED(IDC_CARAWALK_BMP, OnCarawalkBmp)
	ON_BN_CLICKED(IDC_ETS3DV_ACCELERATED, OnEts3dvAccelerated)
	ON_BN_CLICKED(IDC_ETS3DV_BMP, OnEts3dvBmp)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	ON_BN_CLICKED(IDC_BUGFIX_INIT_ALWAYS, OnChange)
	ON_BN_CLICKED(IDC_BUGFIX_LIGHTTEXTURE, OnChange)
	ON_BN_CLICKED(IDC_BUGFIX_MOVEWINDOW, OnChange)
	ON_BN_CLICKED(IDC_BUGFIX_NORMALVECTOR, OnChange)
	ON_BN_CLICKED(IDC_CARAWALK_SHADOWS, OnChange)
	ON_EN_CHANGE(IDC_PIXELFORMAT, OnChange)
	ON_EN_CHANGE(IDC_POLYGON_FACTOR, OnChange)
	ON_EN_CHANGE(IDC_POLYGON_OFFSET, OnChange)
	ON_BN_CLICKED(IDC_BUGFIX_BUFFERSIZE, OnChange)
	ON_CBN_SELCHANGE(IDC_BUGFIX_BUFFER, OnChange)
	ON_BN_CLICKED(IDC_CARAWALK_DOUBLE_BUFFER, OnCarawalkDoubleBuffer)
	ON_BN_CLICKED(IDC_ETS3DV_DOUBLE_BUFFER, OnEts3dvDoubleBuffer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CETS3DGLDLGDlg::OnChange() 
{
   GetDlgItem(IDC_APPLY)->EnableWindow(true);
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGDlg Nachrichten-Handler

BOOL CETS3DGLDLGDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   if (!m_bSpecial)
   {
      int nResult;
      for (; ;)
      {
         nResult = SendDlgItemMessage(IDC_BUGFIX_BUFFER, CB_DELETESTRING, 2, 0);
         if (nResult == CB_ERR) break;
      }
      CWnd *pWnd;
      pWnd = GetDlgItem(IDC_PIXELFORMAT_TXT);
      pWnd->ShowWindow(SW_HIDE);
      pWnd->EnableWindow(false);
      pWnd = GetDlgItem(IDC_PIXELFORMAT);
      pWnd->ShowWindow(SW_HIDE);
      pWnd->EnableWindow(false);
   }
	SetIcon(m_hIcon, TRUE);

   if (m_bCaraWalkAccelerated  == NOT_AVAILABLE)
   {
      GetDlgItem(IDC_CARAWALK_ACCELERATED)->EnableWindow(false);
      GetDlgItem(IDC_CARAWALK_BMP)->EnableWindow(false);
      GetDlgItem(IDC_CARAWALK_DOUBLE_BUFFER)->EnableWindow(false);
   }

   if (m_bEts3DViewAccelerated == NOT_AVAILABLE)
   {
      GetDlgItem(IDC_ETS3DV_ACCELERATED)->EnableWindow(false);
      GetDlgItem(IDC_ETS3DV_BMP)->EnableWindow(false);
      GetDlgItem(IDC_ETS3DV_DOUBLE_BUFFER)->EnableWindow(false);
   }
   UpdateInfo();
   GetDlgItem(IDC_APPLY)->EnableWindow(false);

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CETS3DGLDLGDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

void CETS3DGLDLGDlg::OnOK() 
{
   if (UpdateData(true))
   {
      WriteRegistryKeys();
   	CDialog::OnOK();
   }	
}

void CETS3DGLDLGDlg::OnHelp() 
{
   CDialog::OnHelp();
}

void CETS3DGLDLGDlg::OnReport() 
{
	CReportDlg dlg;
	dlg.DoModal();
}

void CETS3DGLDLGDlg::OnCarawalkAccelerated() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CARAWALK_ACCELERATED  , m_bCaraWalkAccelerated);
   if (m_bCaraWalkAccelerated)
   {
      dx.m_bSaveAndValidate = false;
      if (m_bCaraWalkBmp)
      {
         m_bCaraWalkBmp = false;
	      DDX_Check(&dx, IDC_CARAWALK_BMP, m_bCaraWalkBmp);
      }
      if (!m_bCaraWalkDoubleBuffer)
      {
         m_bCaraWalkDoubleBuffer = true;
         DDX_Check(&dx, IDC_CARAWALK_DOUBLE_BUFFER, m_bCaraWalkDoubleBuffer);
      }
   }
   OnChange();
}

void CETS3DGLDLGDlg::OnCarawalkBmp() 
{
	CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_CARAWALK_BMP, m_bCaraWalkBmp);
   if (m_bCaraWalkBmp)
   {
      dx.m_bSaveAndValidate = false;
      if (m_bCaraWalkAccelerated)
      {
         m_bCaraWalkAccelerated = false;
   	   DDX_Check(&dx, IDC_CARAWALK_ACCELERATED, m_bCaraWalkAccelerated);
      }
      if (m_bCaraWalkDoubleBuffer)
      {
         m_bCaraWalkDoubleBuffer = false;
         DDX_Check(&dx, IDC_CARAWALK_DOUBLE_BUFFER, m_bCaraWalkDoubleBuffer);
      }
   }
   OnChange();
}

void CETS3DGLDLGDlg::OnCarawalkDoubleBuffer() 
{
	CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_CARAWALK_DOUBLE_BUFFER, m_bCaraWalkDoubleBuffer);
   OnChange();	
}

void CETS3DGLDLGDlg::OnEts3dvAccelerated() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_ETS3DV_ACCELERATED, m_bEts3DViewAccelerated);
   if (m_bEts3DViewAccelerated)
   {
      dx.m_bSaveAndValidate = false;
      if (m_bEts3DVBmp)
      {
         m_bEts3DVBmp = false;
         DDX_Check(&dx, IDC_ETS3DV_BMP, m_bEts3DVBmp);
      }
      if (!m_bEts3DVDoubleBuffer)
      {
         m_bEts3DVDoubleBuffer = true;
         DDX_Check(&dx, IDC_ETS3DV_DOUBLE_BUFFER, m_bEts3DVDoubleBuffer);
      }
	}
   OnChange();
}

void CETS3DGLDLGDlg::OnEts3dvBmp() 
{
	CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_ETS3DV_BMP, m_bEts3DVBmp);
   if (m_bEts3DVBmp)
   {
      dx.m_bSaveAndValidate   = false;
      if (m_bEts3DViewAccelerated)
      {
         m_bEts3DViewAccelerated = false;
   	   DDX_Check(&dx, IDC_ETS3DV_ACCELERATED  , m_bEts3DViewAccelerated);
      }
      if (m_bEts3DVDoubleBuffer)
      {
         m_bEts3DVDoubleBuffer = false;
         DDX_Check(&dx, IDC_ETS3DV_DOUBLE_BUFFER, m_bEts3DVDoubleBuffer);
      }
   }
   OnChange();
}

void CETS3DGLDLGDlg::OnEts3dvDoubleBuffer() 
{
	CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_ETS3DV_DOUBLE_BUFFER, m_bEts3DVDoubleBuffer);
   OnChange();	
}

void CETS3DGLDLGDlg::OnApply() 
{
   if (UpdateData(true))
   {
      WriteRegistryKeys();
      GetDlgItem(IDC_APPLY)->EnableWindow(false);
   }
}

void CETS3DGLDLGDlg::WriteRegistryKeys()
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   pApp->SetProfileName(NULL, CETS3DGL_REGKEY);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_BUFFERSIZE     , m_bBugFixBufferSize);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_BUFFER         , m_nBugFixBuffer);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_MOVEWINDOW     , m_bBugFixMoveWindow);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_NORMALVECTOR   , m_bBugFixNormalVector);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_LIGHTTEXTURE   , m_bBugFixLightTexture);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_POLYGON_OFFSET , m_nPolygonOffset);
   pApp->WriteProfileInt(REGKEY_BUGFIX, REGKEY_BUGFIX_DISTANCE_FACTOR, (int)(m_fPolygonFactor*100000));
   pApp->WriteProfileInt(""           , REGKEY_PIXELFORMAT           , m_nPixelFormat);

   if (pApp->SetProfileName(CARAWALK_MODULEFILENAME, CARAWALK_REGKEY))
   {
      pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_GENERIC       ,!m_bCaraWalkAccelerated);
      pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, REGKEY_DEFAULT_GLDIRECT,!m_bCaraWalkBmp);
      pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, REGKEY_DEFAULT_GLDOUBLE, m_bCaraWalkDoubleBuffer);
      pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_BOX_SHADOWS   , m_bCaraWalkShadows);
      pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_INIT_3D_ALWAYS, m_bInit3Dalways);
   }

   if (pApp->SetProfileName(ETS3DV_MODULEFILENAME, ETS3DV_REGKEY))
   {
      pApp->WriteProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_GLGENERIC,!m_bEts3DViewAccelerated);
      pApp->WriteProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_GLDIRECT,  !m_bEts3DVBmp);
      pApp->WriteProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_GLDOUBLE,   m_bEts3DVDoubleBuffer);
   }
   if (GetDlgItem(IDC_APPLY)->IsWindowEnabled())
   {
      DeleteFile(ETS3DGL_DEBUG_FILE);
      m_bDebugFileChanged = true;
   }
}

bool CETS3DGLDLGDlg::UpdateInfo()
{
   CWnd *pWndInfo = GetDlgItem(IDC_INFO);
   HANDLE hFile = CreateFile(ETS3DGL_DEBUG_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

   if (hFile != INVALID_HANDLE_VALUE)
   {
      CFile file(hFile);
      CArchive ar(&file, CArchive::load);
      char   *ptr, szExtract[5];                               // Zeile verkürzen, da die Threadnummerierung
      CString str, strline;
      ZeroMemory(szExtract, 5);                                // hier nicht interessiert
      int     i, nLine;
      bool    bOk = true;
      m_nLineOfHeading = 0;
      for (nLine=0;; nLine++)
      {
         if (!ar.ReadString(strline)) break;                   // Zeilenweise lesen
         if (szExtract[0] == 0)                                // in der ersten Zeile :
         {
            if (strline.Find(GLSETUP_INFO_PROPERTIES) == -1)   // muß dieser Text stehen
            {
               bOk = false;
               break;
            }
            ptr = (char*) LPCTSTR(strline);                    // Nummerierung speichern in szExtract
            for (i=0; (ptr[i]!=0) && (i<5); i++)
            {
               if (isdigit(ptr[i]) || (ptr[i]==' '))           // besteht aus Leerzeichen und Zahlen
                  szExtract[i] = ptr[i];
               else break;
            }
         }
         else if (strline.Find(GLSETUP_INFO_ABILITIES) != -1)
         {
            m_nLineOfHeading = nLine;
         }

         ptr = strstr(strline, szExtract);                     // Nummerierung vorhanden ?
         if (ptr) str += _T(&ptr[i]);
         else     str += strline;
         str += _T("\r\n");                                    // Zeilenvorschub
      }

      if (bOk)
      {
         WINDOWPLACEMENT wp;
         wp.length = sizeof(wp);
         HWND hwndEdit = ::GetDlgItem(m_hWnd, IDC_INFO_TEXT);  // Editfenster holen
         ::SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)LPCTSTR(str));// Text und fixed Font setzen
         ::SendMessage(hwndEdit, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_FIXED_FONT), MAKELPARAM(0, 0));
         ::GetWindowPlacement(m_hWnd, &wp);                    // Die Größe des Dialogfensters speichern
         m_szClient.cx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
         m_szClient.cy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
         ::GetWindowPlacement(hwndEdit, &wp);                  // Die Größe des Dialogfensters speichern
         m_szEdit.cx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
         m_szEdit.cy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
         pWndInfo->EnableWindow(true);
         return true;
      }
      else
      {
         pWndInfo->EnableWindow(false);
      }

   }
   else
   {
      pWndInfo->EnableWindow(false);
   }
   return false;
}

void CETS3DGLDLGDlg::OnInfo() 
{
   ShowControls sc;
   CString      strInfoBtn;
   if (m_bDebugFileChanged)
   {
      if (UpdateInfo())                                        // neues Infofile gelesen
         m_bDebugFileChanged = false;                          // unchanged
      else
         GetDlgItem(IDC_INFO)->EnableWindow(true);             // altes Info noch im Edit Fenster
   }
   sc.hwndEdit = ::GetDlgItem(m_hWnd, IDC_INFO_TEXT);
   if (::IsWindowVisible(sc.hwndEdit))
   {
      sc.nShowCmd = SW_SHOW;
      strInfoBtn.LoadString(IDS_SHOW_INFO);
   }
   else
   {
      sc.nShowCmd = SW_HIDE;
      strInfoBtn.LoadString(IDS_HIDE_INFO);
   }
   ::SendMessage(::GetDlgItem(m_hWnd, IDC_INFO), WM_SETTEXT, 0, (LPARAM)LPCTSTR(strInfoBtn));
   
   ::GetWindowRect(sc.hwndEdit, &sc.rcEdit);                   // Alle Controls unter dem EditCtrl verstecken
   ::EnumChildWindows(m_hWnd, EnumChildProc, (LPARAM)&sc);	   // bzw. anzeigen

   if (sc.nShowCmd == SW_SHOW)                                 // Alle anderen Fenster anzeigen
   {
      ::ShowWindow(sc.hwndEdit, SW_HIDE);                      // EditCtrl verstecken 
      ::SetWindowPos(m_hWnd, NULL,  0, 0, m_szClient.cx, m_szClient.cy, SWP_NOZORDER|SWP_NOMOVE);
      ::SendMessage(sc.hwndEdit, EM_LINESCROLL, 0, -m_nLineOfHeading);
      GetDlgItem(IDC_REPORT )->EnableWindow(true);
      GetDlgItem(IDC_DEFAULT)->EnableWindow(true);
   }
   else                                                        // EditCtrl anzeigen und Text Laden
   {                                                           // Dialogbox vergrößern      
      ::SetWindowPos(m_hWnd, NULL,  0, 0, m_szClient.cx+340, m_szClient.cy, SWP_NOZORDER|SWP_NOMOVE);
      ::SetFocus(sc.hwndEdit);                                 // Focus und Cursor setzen
      ::SendMessage(sc.hwndEdit, EM_SETSEL, 0, 0);             // Editwindow vergrößern
      ::SendMessage(sc.hwndEdit, EM_LINESCROLL, 0, m_nLineOfHeading);
      ::SetWindowPos(sc.hwndEdit, NULL,  0, 0, m_szClient.cx+320, m_szEdit.cy, SWP_NOZORDER|SWP_NOMOVE);
      ::ShowWindow(sc.hwndEdit, SW_SHOW);                      // Editfenster anzeigen
      GetDlgItem(IDC_REPORT )->EnableWindow(false);
      GetDlgItem(IDC_DEFAULT)->EnableWindow(false);
   }
}

BOOL CALLBACK CETS3DGLDLGDlg::EnumChildProc(HWND hwnd, LPARAM lParam)
{
   ShowControls*psc = (ShowControls*) lParam;
   if (::IsWindowEnabled(hwnd))
   {
      CRect rcWnd, rc;
      ::GetWindowRect(hwnd, &rcWnd);
      if (rc.IntersectRect(&psc->rcEdit, &rcWnd))              // liegen die Fenster unter dem EditCtrl
      {
         ::ShowWindow(hwnd, psc->nShowCmd);
      }
   }
   return true;
}

