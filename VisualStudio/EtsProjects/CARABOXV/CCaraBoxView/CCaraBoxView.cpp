
#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "CCaraBoxView.h"
#include "EtsAppWndMsg.h"

#define NO_OF_CCARABOXVIEW_FUNCTIONS 1
#define CCARABOXVIEW_MODULENAME "CARABOXV.DLL"

namespace CCaraBoxView_Global
{
   HMODULE hDLL                      = NULL;
   bool    (*pCreate)(CCaraBoxView*) = NULL;
   int     nInstances                = 0;

   bool inline BindDll()                               // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(CCARABOXVIEW_MODULENAME,          // Dateinamen der DLL
                     hDLL,                             // obigen Module Handle für die DLL (reference)
                     (void**)&pCreate,                 // reference auf die Adresse des ersten Funktionspointer
                     NO_OF_CCARABOXVIEW_FUNCTIONS,     // die Anzahl der Funktionspointer
                     false);                           // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll(void *pThis)                  // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(CCARABOXVIEW_MODULENAME,               // Dateinamen der DLL
                hDLL,                                  // obigen Module Handle für die DLL (reference)
                (void**)&pCreate,                      // reference auf die Adresse des ersten Funktionspointer
                NO_OF_CCARABOXVIEW_FUNCTIONS,          // die Anzahl der Funktionspointer
                &nInstances,                           // reference auf Instanzzähler da dynamische Schnittstellenklasse
                pThis);                                // This-Pointer der Klasse
   }

   void inline TestBind()                              // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(CCARABOXVIEW_MODULENAME,           // Dateinamen der DLL
                    hDLL,                              // obigen Module Handle für die DLL (reference)
                    &nInstances);                      // reference auf Instanzzähler da dynamische Schnittstellenklasse
   }
};

CCaraBoxView::CCaraBoxView()                           // Konstructor
{
   CCaraBoxView_Global::nInstances++;
   m_hWndDlg    = NULL;
   m_hThread    = NULL;
   m_nThreadID  = NULL;
   m_hWndParent = NULL;
}
 
CCaraBoxView::~CCaraBoxView()                          // Destructor
{
   CloseDialog();
   CCaraBoxView_Global::nInstances--;
   CCaraBoxView_Global::TestBind();
}

bool CCaraBoxView::Create(HWND hwndParent)
{
   if(CCaraBoxView_Global::hDLL == NULL) CCaraBoxView_Global::BindDll();
   if(CCaraBoxView_Global::pCreate)
   {
      m_hWndParent = hwndParent;
      return CCaraBoxView_Global::pCreate(this);
   }
   return false;
}

void CCaraBoxView::Destructor()
{
   CloseDialog();
   CCaraBoxView_Global::UnBindDll(this);
}

LRESULT CCaraBoxView::SendMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (IsWindow()) return ::SendMessage(m_hWndDlg, nMsg, wParam, lParam);
   return 0;
}

LRESULT CCaraBoxView::PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (IsWindow()) return ::PostMessage(m_hWndDlg, nMsg, wParam, lParam);
   return 0;
}

void CCaraBoxView::CloseDialog()
{
   if (IsWindow() && (m_nThreadID != 0))
   {
      DWORD dwExitCode;
      LRESULT lResult = 0;
      while ((m_hThread != NULL) && GetExitCodeThread(m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
      {
         if (!lResult) lResult = PostMessage(WM_COMMAND, IDCANCEL, 0);
         Sleep(0);
      }
   }
}

void CCaraBoxView::DrawBox3DView(char *pszBoxName)
{
   SendMessage(WM_SETBOXDATA, (WPARAM)pszBoxName, 0);
}
