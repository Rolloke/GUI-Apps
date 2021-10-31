/************************************************************************/
/*                                                                      */
/*        Schnittstellen-CPP/LIB-Datei für die DLL CARA3D               */
/*                  (nur für das Aufrufprogramm)                        */
/*             Definition der Aufruf-Klasse CEts3DGL                    */
/*                                                                      */
/* Die Schnittstellen-Funktionen von CARA3D werden vom aufrufenden      */
/* Programm wie statische Klassen-Funktionen aufgerufen:                */
/*     CEts3DGL::publicFunction(..);                                    */
/*                                                                      */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle        */
/* eine Instanz der Klasse CEts3DGL als lokale (globale) Variable.      */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem     */
/* Nutzungsbereich der CARA3D.DLL. Wenn die Instanz gelöscht wird,      */
/* wird über den Destructor der Klasse CEts3DGL die DLL freigegeben     */
/* Das aufrufende Programm 'includes' die CEts3DGL.H und bindet die     */
/* Dateien CEts3DGL.LIB und CEts3DGL.H in ihr Projekt mit ein.          */
/*                                                                      */
/*                                                                      */
/*     programmiert von Dipl. Ing. Rolf Ehlers                          */
/*                                                                      */
/*     Version 1.0          01.02.98                                    */
/*                                                                      */
/************************************************************************/
#include "stdafx.h"
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "ETS3DGL.h"

// Flags für States
#define OPEN_GL_INITIALIZED               0x00000001           // OpenGL Initialisiert
#define OPEN_GL_INIT_FAILED               0x00000002           // nicht Initialisiert
#define CS_DIB_INITIALIZED                0x00000004           // Kritische Sektion für DibSection
#define CS_EXT_INITIALIZED                0x00000008           // Kritische Sektion für externe Daten
#define CLEAR_INIT                        0x0000000f           // Löschflags

#define INVALID_SIZE                      0x00000010           // Durch die Funktion OnSize verändert
#define INVALID_VIEW                      0x00000020           // Durch die Funktion OnPaint verändert
#define INVALID_VOLUME                    0x00000040           // Wird fast nur am Anfang gesetzt
#define INVALID_LISTS                     0x00000080           // eine oder mehrere der Listen sind ungültig
#define INVALID_COMMANDS                  0x00000100           // einer oder mehrere der Commands sind ungültig
#define INVALID_BKGND                     0x00000200           // Hintergrundfarbe ungültig
#define DO_NOT_VALIDATE                   0x00000800           // Nicht Validieren
#define CLEAR_INVALID                     0x000007f0           // Löschflags

#define RENDERING_NOW                     0x00001000           // Die Bilddaten werden gerendert
#define TRANSFORMING_NOW                  0x00002000           // Punkte werden Transformiert
#define COPY_FRAME_BMP_NOW                0x00004000           // Der Framebuffer wird kopiert
#define CLEAR_NOW                         0x0000f000           // Löschflags

#define TIME_CHANGED                      0x00010000           // Updatezeit wurde verändert
#define MODE_CHANGED                      0x00020000           // Modi wurden verändert
#define CLEAR_CHANGED                     0x000f0000           // Löschflags

#define RENDERING_SUSPENDED               0x00100000           // Es darf nicht gerendert werden
#define BMP_FORMAT_AVAILABLE              0x00200000           // Bitmap-Rendern möglich
#define CRITICAL_SECTION_ATTACHED         0x00400000           // Externe Critical Section gesetzt

// Commands für SetList
#define INVALID_LIST                      0x00000011           // Invalidieren der Listen
#define INVALID_COMMAND                   0x00000022           // Invalidieren der Listen
#define RENDER_LIST                       0x00000031           // Rendere Liste
#define DONT_RENDER_LIST                  0x00000041           // Rendere Liste nicht
#define RENDER_COMMAND                    0x00000052           // Rendere Command
#define DONT_RENDER_COMMAND               0x00000062           // Rendere Command nicht

#define GWL_PARENTWINDOW   4
#define GWL_THREADWINDOWID 8
#define THREADWINDOWID     0x48375936

#define NO_OF_ETS3DGL_FUNCTIONS 7
#define ETS3DGL_DLL    "ETS3DGL.DLL"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

namespace CEts3DGL_Global
{
   HMODULE   hDLL;
   HWND    (*pCreate )          (CEts3DGL*, LPCTSTR, DWORD, RECT*, HWND)      = NULL;
   void    (*pOnRenderSzene)    (CEts3DGL*)                                   = NULL;
   bool    (*pCreateRenderList) (CEts3DGL*, UINT, LISTFUNCTION, int, bool)    = NULL;
   bool    (*pSetList)          (CEts3DGL*, UINT, UINT)                       = NULL;
   void    (*pTransformPoints)  (CEts3DGL*, CVector *, int, bool)             = NULL;
   void    (*pDraw)             (DWORD, float)                                = NULL;
   void    (*pDrawTriangleFan)  (CVector *,CVector *,CVector *, ETS3DGL_Fan*) = NULL;

   int       nInstances                                                       = 0;
   CEts3DGL *pFirstInstance                                                   = NULL;

   bool pascal BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(ETS3DGL_DLL,                  // Dateinamen der DLL
                     hDLL,                         // obigen Module Handle für die DLL (reference)
                     (void**)&pCreate,             // reference auf die Adresse des ersten Funktionspointer
                     NO_OF_ETS3DGL_FUNCTIONS,      // die Anzahl der Funktionspointer
                     false);                       // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll(void*p)                   // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(ETS3DGL_DLL,                       // Dateinamen der DLL
                hDLL,                              // obigen Module Handle für die DLL (reference)
                (void**)&pCreate,                  // reference auf die Adresse des ersten Funktionspointer
                NO_OF_ETS3DGL_FUNCTIONS,                               // die Anzahl der Funktionspointer
                &nInstances,                       // reference auf Instanzzähler da dynamische Schnittstellenklasse
                p);                                // Zusatzparameter
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      if (nInstances == 0)
         pFirstInstance = NULL;                    //
      ETSTestUnBind(ETS3DGL_DLL,                   // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    &nInstances);                  // reference auf Instanzzähler da dynamische Schnittstellenklasse
   }
};



/******************************************************************************
* Definition : CAutoCriticalSection(CEts3DGL &C3Dgl)
*              CAutoCriticalSection(CRITICAL_SECTION *pCS)
*              ~CAutoCriticalSection()
* Zweck      : Der Konstruktor speichert die CriticalSectionvariable und 
*              entert diese. Im Destruktor wird die Section wieder verlassen
* Aufruf     : z.B.: in einem C-Anweisungsblock { ... }. Beim verlassen des
*              Blocks wird der Destruktor aufgerufen
******************************************************************************/
CAutoCriticalSection::CAutoCriticalSection(CEts3DGL &C3Dgl)
{
   if (C3Dgl.m_pcsExt) ::EnterCriticalSection(C3Dgl.m_pcsExt);
   m_pCS = C3Dgl.m_pcsExt;
}
CAutoCriticalSection::CAutoCriticalSection(CRITICAL_SECTION *pCS)
{
   if (pCS) ::EnterCriticalSection(pCS);
   m_pCS = pCS;
}

CAutoCriticalSection::~CAutoCriticalSection()
{
   if (m_pCS) ::LeaveCriticalSection(m_pCS);
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CEts3DGL::CEts3DGL()
{
   CEts3DGL_Global::nInstances++;
   Init();
}

void CEts3DGL::Init()
{
   m_hWndThread    = NULL;
   m_hDC           = NULL;
   m_hThread       = NULL;
   m_hEvent        = NULL;
   m_hPalette      = NULL;
   m_hGL_RC        = NULL;
   m_nThreadID     = 0;
   m_nModes        = ETS3D_CM_RGB|ETS3D_DC_DIRECT_2|ETS3D_PR_PERSPECTIVE;
   m_nStates       = INVALID_VOLUME|INVALID_SIZE|INVALID_BKGND;
   m_nClearBits    = 0;
   m_nRenderTimer  = 0;
   m_nTime         = 100;
   m_szView.cx     = 100;
   m_szView.cy     = 100;
   m_pDib          = NULL;
   m_dAlpha        = 60.0;
   m_Volume.Left   = -1.0f;
   m_Volume.Top    = -1.0f;
   m_Volume.Right  =  1.0f;
   m_Volume.Bottom =  1.0f;
   m_Volume.Near   =  0.1f;
   m_Volume.Far    = 10.0f;
   m_pRenderList   = NULL;
   m_pCommandList  = NULL;
   m_pcsExt        = NULL;
   m_cBkGnd        = 0;
   ZeroMemory(&m_Bits, sizeof(m_Bits));
   m_Bits.nColor   = 0;
   m_Bits.nDepth   = 32;
   if (CEts3DGL_Global::pFirstInstance == NULL)
   {
      CEts3DGL_Global::pFirstInstance = this;
   }
}

/******************************************************************************
* Definition : CEts3DGL(CEts3DGL *pInit)
* Zweck      : Konstruktor zur Initialisierung durch ein anderes CEts3DGL-objekt
******************************************************************************/
CEts3DGL::CEts3DGL(CEts3DGL *pInit)
{
   CEts3DGL_Global::nInstances ++;
   Init();
   if (pInit)
   {
      m_nModes        = pInit->m_nModes;
      m_nClearBits    = pInit->m_nClearBits;
      m_szView        = pInit->m_szView;
      m_dAlpha        = pInit->m_dAlpha;
      m_Volume        = pInit->m_Volume;
      m_cBkGnd        = pInit->m_cBkGnd;
      m_LookAt        = pInit->m_LookAt;
      m_pcsExt        = pInit->m_pcsExt;
   }
}

CEts3DGL::~CEts3DGL()
{
//   BEGIN("~CEts3DGL");
   CEts3DGL_Global::nInstances--;
   CEts3DGL_Global::TestBind();
}

/******************************************************************************
* Definition : Destructor()
* Zweck      : Freigabe der DLL handles
* Aufruf     : 
******************************************************************************/
void CEts3DGL::Destructor()
{
   CEts3DGL_Global::UnBindDll(this);
}

void CEts3DGL::AutoDelete()
{
   delete this;
}

/******************************************************************************
* Name       : Create                                                         *
* Zweck      : Erstellung des Ausgabefensters für OpenGL                      *
* Aufruf     : Durch den Konstruktor der DLL-Klasse.                          *
* Parameter  :                                                                *
* strWndText (E): Fenstertext, Name des Fensters                     (LPCTSTR)*
* dwFlags    (E): Window Flags (CreateWindow())                      (DWORD)  *
* prcWnd     (E): Fensterrechteck                                    (RECT)   *
* hWndParent (E): Elternfenster                                      (HWND)   *
* Funktionswert : Window-Handle                                      (HWND)   *
******************************************************************************/
HWND CEts3DGL::Create(LPCTSTR strWndText, DWORD dwFlags, RECT *prcWnd, HWND hWndParent)
{
   if (IsWindow(hWndParent) ==0)
   {
      ASSERT(false);
      return NULL; // Elternfenster muß vorhanden sein !
   }
   if ((!CEts3DGL_Global::pCreate) && (CEts3DGL_Global::BindDll())) return NULL;

   void (*pSetFirstInstance)(CEts3DGL*) = (void (*) (CEts3DGL*))GetProcAddress(CEts3DGL_Global::hDLL, MAKEINTRESOURCE(9));
   if (pSetFirstInstance && CEts3DGL_Global::pFirstInstance)
   {
      pSetFirstInstance(CEts3DGL_Global::pFirstInstance);
   }
   return CEts3DGL_Global::pCreate(this, strWndText, dwFlags, prcWnd, hWndParent);
}

/******************************************************************************
* Definition : OnRenderSzene()
* Zweck      : Aufruf der Renderfunktion
******************************************************************************/
void CEts3DGL::OnRenderSzene()
{
   if (CEts3DGL_Global::pOnRenderSzene)
      CEts3DGL_Global::pOnRenderSzene(this);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::CreateList(int nID, LISTFUNCTION pfn, int nRange)
{
   ASSERT((pfn != NULL) && (nRange >= 0));
   if (((pfn != NULL) || (nRange >= 0)) && (CEts3DGL_Global::pCreateRenderList))
      return CEts3DGL_Global::pCreateRenderList(this, nID, pfn, nRange, false);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::CreateCommand(int nID, LISTFUNCTION pfn, int nRange)
{
   ASSERT((pfn != NULL) && (nRange >= 0));
   if (((pfn != NULL) || (nRange >= 0)) && (CEts3DGL_Global::pCreateRenderList))
      return CEts3DGL_Global::pCreateRenderList(this, nID, pfn, nRange, true);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::InvalidateList(int nID)
{
   if (CEts3DGL_Global::pSetList)
      return CEts3DGL_Global::pSetList(this, nID, INVALID_LIST);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::InvalidateCommand(int nID)
{
   if (CEts3DGL_Global::pSetList)
      return CEts3DGL_Global::pSetList(this, nID, INVALID_COMMAND);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::InvalidateView()
{
   m_nStates |= INVALID_VIEW;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::PerformList(int nID, bool bOn)
{
   if (CEts3DGL_Global::pSetList)
      return CEts3DGL_Global::pSetList(this, nID, bOn ? RENDER_LIST : DONT_RENDER_LIST);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::PerformCommand(int nID, bool bOn)
{
   if (CEts3DGL_Global::pSetList)
      return CEts3DGL_Global::pSetList(this, nID, bOn ? RENDER_COMMAND : DONT_RENDER_COMMAND);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::TransformPoints(CVector *pv, int nCount, bool bDir)
{
   if (CEts3DGL_Global::pTransformPoints)
      CEts3DGL_Global::pTransformPoints(this, pv, nCount, bDir);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::Draw(DWORD dwMode, float fSize)
{
   if (CEts3DGL_Global::pDraw) CEts3DGL_Global::pDraw(dwMode, fSize);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::DrawTriangleFan(CVector *pv1, CVector *pv2, CVector *pv3, ETS3DGL_Fan *pF)
{
   if (CEts3DGL_Global::pDrawTriangleFan == NULL) CEts3DGL_Global::BindDll();
   if (CEts3DGL_Global::pDrawTriangleFan) CEts3DGL_Global::pDrawTriangleFan(pv1, pv2, pv3, pF);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::SetSize(SIZE szView)
{
   if (m_nModes & ETS3D_SIZE_FIXED) return false;
   ProtectData();
   m_szView   = szView;
   ReleaseData();
   m_nStates |= INVALID_SIZE;
   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void  CEts3DGL::SetVolume(ETS3DGL_Volume &Volume)
{
   ProtectData();
   m_Volume   = Volume;
   ReleaseData();
   m_nStates |= INVALID_VOLUME;
}
void  CEts3DGL::SetVolume(float fleft,float ftop, float fright, float fbottom, float fnear, float ffar)
{
   ProtectData();
   m_Volume.Left     = fleft;
   m_Volume.Top      = ftop;
   m_Volume.Right    = fright;
   m_Volume.Bottom   = fbottom;
   m_Volume.Near     = fnear;
   m_Volume.Far      = ffar;
   ReleaseData();
   m_nStates |= INVALID_VOLUME;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::SetAlpha(double dAlpha)
{
   ProtectData();
   m_dAlpha = dAlpha;
   ReleaseData();
   m_nStates |= INVALID_VOLUME;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::SetLookAt(ETS3DGL_LookAt &la)
{
   if (m_nModes & ETS3D_PR_LOOK_AT)
   {
      ProtectData();
      m_LookAt = la;
      ReleaseData();
      m_nStates |= INVALID_VOLUME;
      return true;
   }
   else
   {
      m_LookAt = la;
      return false;
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::SetModes(DWORD dwSet, DWORD dwRemove)
{
   if (m_nStates & OPEN_GL_INITIALIZED)                        // ist OpenGL initialisiert
   {
      dwSet     &= ETS3D_PR_CLEAR;                            // dürfen nur die Projektionsparameter
      dwRemove  &= ETS3D_PR_CLEAR;                            // geändert werden
      m_nStates |= INVALID_VOLUME;
   }
   DWORD nTest, nModes = m_nModes;
   nModes &= ~dwRemove;
   nModes |=  dwSet;
   nTest   = nModes & ETS3D_CM_CLEAR;                          // Testen der Farb-Modi
   if ((nTest != ETS3D_CM_RGB) && (nTest != ETS3D_CM_4096) && (nTest != ETS3D_CM_256)) return false;
   nTest   = nModes & ETS3D_DC_CLEAR;                          // Testen der Devicecontext-Modi
   if ((nTest != ETS3D_DC_DIRECT_1) && (nTest != ETS3D_DC_DIRECT_2) && (nTest != ETS3D_DC_DIB_1) && (nTest != ETS3D_DC_DIB_2)) return false;
   nTest   = nModes & ETS3D_PR_CLEAR;                          // Testen der Projektions-Modi
   nTest  &= ~ETS3D_PR_LOOK_AT;
   if ((nTest != ETS3D_PR_ORTHOGONAL) && (nTest != ETS3D_PR_PERSPECTIVE) && (nTest != ETS3D_PR_FRUSTUM)) return false;

   ProtectData();
   m_nModes = nModes;
   ReleaseData();
   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool  CEts3DGL::SetRefreshTime(int nTime)
{
   if ((m_nModes & ETS3D_NO_TIMER) || (nTime <= 0)) return false;
   m_nTime = nTime;
   if (m_nRenderTimer !=0) m_nStates |= TIME_CHANGED;
   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::SetClearBits(UINT nClearBits)
{
   m_nClearBits = nClearBits;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::SetBkColor(COLORREF c)
{
   ProtectData();
   m_cBkGnd = c; 
   m_nStates |= INVALID_BKGND;
   ReleaseData();
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::AttachDC(HDC hDC)
{
   ASSERT((m_nStates & OPEN_GL_INITIALIZED)==0);
   if (m_nStates & OPEN_GL_INITIALIZED) return false;
   ASSERT(m_hDC == NULL);
   m_hDC = hDC;
   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HDC CEts3DGL::DetachDC()
{
   HDC hDC = m_hDC;
   m_hDC = NULL;
   return hDC;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
BOOL  CEts3DGL::MoveWindow(int X, int Y, int nWidth, int nHeight, bool bRepaint)
{
   if (m_nThreadID !=0)
   {
      ASSERT(::GetCurrentThreadId() != m_nThreadID);
      if (m_nStates & ETS3D_EXTRA_CHILDWND)
         return ::MoveWindow(GetHWND(), X, Y, nWidth, nHeight, bRepaint);
      else
         return ::PostThreadMessage(m_nThreadID, WM_THREAD_SIZE, 0, 0);
   }
   else
      return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND CEts3DGL::GetHWND() const
{
   if (m_hWndThread)
   {
      return ::GetParent(m_hWndThread);
   }
   return NULL;
} 

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND CEts3DGL::GetTopLevelParent()
{
   long lID = ::GetWindowLong(m_hWndThread, GWL_THREADWINDOWID);
   if (lID == THREADWINDOWID)
   {
      HWND hwndParent = (HWND)::GetWindowLong(m_hWndThread, GWL_PARENTWINDOW);
      ASSERT(::IsWindow(hwndParent));
      return hwndParent;
   }
   return NULL;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
LRESULT CEts3DGL::SendMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (m_nThreadID)
   {
      ::SetMessageExtraInfo((LPARAM)this);
      return ::SendMessage(GetHWND(), nMsg, wParam, lParam);
   }
   else
      return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
BOOL CEts3DGL::PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (m_nThreadID)
      return ::PostMessage(GetHWND(), nMsg, wParam, lParam);
   else
      return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::Destroy()
{
   void (*pDestroy)(CEts3DGL*) = NULL;
   
   if (CEts3DGL_Global::hDLL) pDestroy = (void (*) (CEts3DGL*))GetProcAddress(CEts3DGL_Global::hDLL, MAKEINTRESOURCE(10));

   if (pDestroy)
   {
      m_nTime = CEts3DGL_Global::nInstances;
      pDestroy(this);
   }
   else
   {
      HWND hwndParent = GetHWND();
      if (m_nThreadID)
      {
         ASSERT(::GetCurrentThreadId() != m_nThreadID);
         DWORD dwExitCode;                                  // und auf die Beendigung warten
         ::PostThreadMessage(m_nThreadID, WM_DESTROY, NULL, NULL);
         while (GetExitCodeThread(m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
             Sleep(2);
      }

      if (hwndParent && IsWindow(hwndParent))
      {
         ::SendMessage(hwndParent, WM_REMOVE_PROPERTY, (WPARAM)this, 0);
      }
      m_hWndThread = NULL;
      m_hThread    = NULL;
      m_nThreadID  = 0;
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::ProtectData()
{
   if (m_pcsExt) EnterCriticalSection(m_pcsExt); // Externe Daten schützen
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::ReleaseData()
{
   if (m_pcsExt) LeaveCriticalSection(m_pcsExt); // Externe Daten freigeben
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsRendering()
{
   return ((m_nStates & RENDERING_NOW) ? true : false);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsSuspended()
{
   return ((m_nStates & RENDERING_SUSPENDED) ? true :false);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsInitialized()
{
   return ((m_nStates &OPEN_GL_INITIALIZED) ? true :false);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsBmpFormatAvailable()
{
   return ((m_nStates & BMP_FORMAT_AVAILABLE) ? true : false);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::AttachExternalCS(CRITICAL_SECTION *pcsExt)
{
   ASSERT(m_pcsExt == NULL);
   if (m_pcsExt == NULL)
   {
      m_pcsExt = pcsExt;
      m_nStates |= (CRITICAL_SECTION_ATTACHED|CS_EXT_INITIALIZED);
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CRITICAL_SECTION * CEts3DGL::DetachExternalCS()
{
   CRITICAL_SECTION *pcsExt = m_pcsExt;
   m_pcsExt   = NULL;
   m_nStates &= ~(CRITICAL_SECTION_ATTACHED|CS_EXT_INITIALIZED);
   return pcsExt;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::SetValidation(bool bVal)
{
   if (bVal) m_nStates &= ~DO_NOT_VALIDATE;
   else      m_nStates |=  DO_NOT_VALIDATE;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::DoNotValidate()
{
   return (m_nStates & DO_NOT_VALIDATE) ? true : false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsRenderEvent(long nID)
{
   return (nID == (long)m_nRenderTimer) ? true : false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGL::IsValid()
{
   return (m_nStates & CLEAR_INVALID) ? false: true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGL::ReportGLError()
{
   if (CEts3DGL_Global::pSetList)
      CEts3DGL_Global::pSetList(this, 0, 0);
}

/******************************************************************************
* Name       : GetColorFromIndex                                              *
* Definition : COLORREF GetColorFromIndex(int, float);                        *
* Zweck      : liefert einen RGB-Farbwert aus einem Farb-Index im Bereich     *
*              von 0 bis 1553 welcher einen Farbkreis von rot über "gelb",    *
*              grün, "zyan", blau und "magenta" wieder nach rot umschreibt.   *
*              Die Farben in den Anführungszeichen sind die Mischfarben aus   *
*              den anliegenden Farben. Zwischen den angegebenen Farben liegen *
*              jeweils 255 Farbstufen.                                        *
* Aufruf     : GetColorFromIndex(nColor, fIntens);                            *
* Parameter  :                                                                *
* nColor (E) : Zahl im Bereich von 0 bis 1553.                    (int)       *
* fIntens(E) : Intensität der Farbe                               (float)     *
* Funktionswert : resultierender RGB-Farbwert                     (COLORREF)  *
******************************************************************************/
#define CLMASC 0xff
COLORREF GetColorFromIndex(int nColor, float fIntens)
{
   UINT nValue = nColor & CLMASC,
        nMinus = CLMASC  - nValue,
        nPosition = nColor >> 8;
   BYTE red=0, green=0, blue=0;
   if (nPosition > 5)
      nPosition %= 6;

   switch (nPosition)
   {
      case 0: red = CLMASC; green = nValue;                break;
      case 1: red = nMinus; green = CLMASC;                break;
      case 2:               green = CLMASC; blue = nValue; break;
      case 3:               green = nMinus; blue = CLMASC; break;
      case 4: red = nValue;                 blue = CLMASC; break;
      case 5: red = CLMASC;                 blue = nMinus; break;
   }
   if (fIntens != 1.0f)
   {
      red   = (BYTE)((float)red   * fIntens);
      green = (BYTE)((float)green * fIntens);
      blue  = (BYTE)((float)blue  * fIntens);
   }
   return RGB(red, green, blue);
}

/******************************************************************************
* Name       : GetIndexFromColor                                              *
* Definition : COLORREF GetIndexFromColor(COLORREF, float *pfIntens = NULL);  *
* Zweck      : liefert einen Farb-Index im Bereich von 0 bis 1553 welcher     *
*              einen Farbkreis von rot über "gelb", grün, "zyan", blau und    *
*              "magenta" wieder nach rot umschreibt, aus einem RGB-Farbwert.  *
*              Die Farben in den Anführungszeichen sind die Mischfarben aus   *
*              den anliegenden Farben. Zwischen den angegebenen Farben liegen *
*              jeweils 255 Farbstufen.                                        *
* Aufruf     : GetIndexFromColor(Color, [pfIntens]);                          *
* Parameter  :                                                                *
* Color  (E) : RGB-Farbwert                                       (COLORREF)  *
* fIntens(A) : Intensität der Farbe (0.0,..,1.0)                  (float)     *
* Funktionswert : resultierende Zahl im Bereich von 0 bis 1553    (int)       *
******************************************************************************/
int GetIndexFromColor(COLORREF color, float &fIntens)
{
   BYTE  red   = GetRValue(color),                             // Farben aufteilen
         green = GetGValue(color),
         blue  = GetBValue(color);
   UINT  nmax, nmin,  mincolor = 1;
   nmax = red;                                                 // größte Farbkomponente ermitteln
   if (green > nmax) nmax = green;
   if (blue  > nmax) nmax = blue;
   if (0    == nmax) { fIntens = 0; return 0;}                 // Schwarz hat Intensität 0

   nmin = red;                                                 // kleinste Farbkomponente entfernen
   if (green < nmin) {nmin = green; mincolor = 2;}
   if (blue  < nmin) {nmin = blue;  mincolor = 3;}

   fIntens = (float)CLMASC / (float) nmax;                     // Kehrwert der Intensität berechnen
   if (fIntens != 1.0f)                                        // Farbeintensität normieren
   {
      red     = (BYTE)(fIntens * red);
      green   = (BYTE)(fIntens * green);
      blue    = (BYTE)(fIntens * blue);
      fIntens = 1.0f / fIntens;
   }

   if ((red==255) && (green==255) && (blue==255)) return 0;    // Farbe Weiß bzw. Graustufen 
   else if (mincolor == 3) blue  = 0;                          // nicht im Farbkreis enthaltene,
   else if (mincolor == 2) green = 0;                          // kleinste Farbkomponente entfernen
   else                    red   = 0;

   if ((red   == CLMASC) && (blue  == 0)) return green;        // Farbstufenindex liefern
   if ((green == CLMASC) && (blue  == 0)) return (CLMASC<<1) + 1 - red;
   if ((green == CLMASC) && (red   == 0)) return (CLMASC<<1) + 2 + blue;
   if ((blue  == CLMASC) && (red   == 0)) return (CLMASC<<2) + 3 - green;
   if ((blue  == CLMASC) && (green == 0)) return (CLMASC<<2) + 4 + red;
   if ((red   == CLMASC) && (green == 0)) return (CLMASC<<2) + (CLMASC<<1) + 5 - blue;
   return -1;
}
#undef CLMASC

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void InterpolateColors(int nFirstColor, int nLastColor, COLORREF FirstColor, COLORREF LastColor, COLORREF* pColors)
{
   if (pColors)
   {
      float fRfac = (float)(GetRValue(LastColor)-GetRValue(FirstColor)) / (float)(nLastColor-nFirstColor),
            fRoff = GetRValue(LastColor) - fRfac * nLastColor,
            fGfac = (float)(GetGValue(LastColor)-GetGValue(FirstColor)) / (float)(nLastColor-nFirstColor),
            fGoff = GetGValue(LastColor) - fGfac * nLastColor,
            fBfac = (float)(GetBValue(LastColor)-GetBValue(FirstColor)) / (float)(nLastColor-nFirstColor),
            fBoff = GetBValue(LastColor) - fBfac * nLastColor;
      for (int i=nFirstColor; i<=nLastColor; i++)
      {
         pColors[i] = RGB((BYTE)(fRoff+i*fRfac),(BYTE)(fGoff+i*fGfac),(BYTE)(fBoff+i*fBfac));
      }
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void SetIndexColors(int nFirstColor, int nLastColor, COLORREF FirstColor, COLORREF LastColor, COLORREF* pColors)
{
   if (pColors)
   {
      float fLastIntens, fFirstIntens, fGradientI, fOriginI;
      int   nLastColorIndex  = ::GetIndexFromColor(LastColor , fLastIntens),
            nFirstColorIndex = ::GetIndexFromColor(FirstColor, fFirstIntens), i;
      float fGradientC = (float)(nLastColorIndex-nFirstColorIndex) / (float)(nLastColor-nFirstColor),
            fOriginC   = nLastColorIndex - fGradientC * nLastColor;
      if (fLastIntens == fFirstIntens) 
      {
         fGradientI = 0.0;
         fOriginI   = fLastIntens;
      }
      else
      {
         fGradientI = (fLastIntens - fFirstIntens) / float(nLastColor-nFirstColor);
         fOriginI  = fLastIntens - fGradientI * (float)nLastColor;
      }
      for (i=nFirstColor; i<=nLastColor; i++)
      {
         pColors[i] = GetColorFromIndex((int)(fOriginC+i*fGradientC), fOriginI+i*fGradientI);
      }
   }
}


