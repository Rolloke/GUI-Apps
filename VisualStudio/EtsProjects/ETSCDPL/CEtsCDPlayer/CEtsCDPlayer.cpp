/************************************************************************
                                                                      
          Schnittstellen-CPP/LIB-Datei für die DLL ETSCDPL            
                    (nur für das Aufrufprogramm)                      
               Definition der Aufruf-Klasse CEtsCDPlayer              
                                                                      
   Die Schnittstellen-Funktionen von ETSCDPL werden vom aufrufenden   
   Programm wie statische Klassen-Funktionen aufgerufen:              
       CEtsCDPlayer::publicFunction(..);                              
                                                                      
   Das aufrufende Programm deklariert an einer geeigneten Stelle      
   eine Instanz der Klasse CEtsCDPlayer als lokale (globale) Variable.    
   Die Art und der Ort der Instanzdeklaration richtet sich nach dem   
   Nutzungsbereich der ETSCDPL.DLL. Wenn die Instanz gelöscht wird,   
   wird über den Destructor der Klasse CEtsCDPlayer die DLL freigegeben   
   Das aufrufende Programm 'includes' die CEtsCDPlayer.H und bindet die   
   Dateien CEtsCDPlayer.LIB und CEtsCDPlayer.H in ihr Projekt mit ein.        
                                                                      
                                                                      
       programmed by Dipl. Ing. Rolf Ehlers                           
                                                                      
       Version 1.0          15.05.98                                  
                                                                      
************************************************************************/

#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "CEtsCDPlayer.h"

#define  ETSCDPL_DLLERROR              0xFFFF   // DLL-Datei konnte nicht geladen werden
#ifndef ASSERT
   #define ASSERT
#endif

#define BROWSE_FOR_FOLDER 1
#define FREE_PIDL         2
#define DO_MESSAGE        3
#define DO_TITLEDLG       4
#define GET_CONTEXT_MENU  5
#define GET_WINDOW_HANDLE 6
#define DO_MIXER_DLG      7

#define NO_OF_ETSCDPL_FUNCTIONS 4
#define ETSCDPL_MODULENAME       "ETSCDPL.DLL"

struct BrowseFolderStruct
{
   char       *pszFolderPath;
   char       *pszTitle;
   void       *ppidl;
   UINT        nFlags;
   HWND        hwndParent;
};

namespace CEtsCDPlayer_Global
{
   HMODULE hDLL                  = NULL;
   BOOL (*pDoModal)(HWND, void*) = NULL;
   HWND (*pCreate)(HWND, void*)  = NULL;
   long (*pDoDataExchange)(void*, DWORD dwType, int nSize, void *pParam) = NULL;
   long (*pDestroy)(void*)       = NULL;

   bool pascal BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      ETSBind(ETSCDPL_MODULENAME,                  // Dateinamen der DLL
              hDLL,                                // obigen Module Handle für die DLL (reference)
              (void**)&pDoModal,                   // reference auf die Adresse des ersten Funktionspointer
              NO_OF_ETSCDPL_FUNCTIONS,             // die Anzahl der Funktionspointer
              false);                              // Flag, Programmabbruch wenn DLL nicht gefunden
      return (hDLL != NULL) ? true : false;
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(ETSCDPL_MODULENAME,            // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    NULL);                         // NULL für statische Schnittstellenklasse
   }
};


CEtsCDPlayer::CEtsCDPlayer()                       // Destructor
{
   m_nTitles    = 0;
   m_ppszTitles = NULL;
   m_pParam     = NULL;
}

CEtsCDPlayer::~CEtsCDPlayer()                       // Destructor
{
   if (m_ppszTitles)
   {
      int i;
      for (i=0; i<m_nTitles; i++)
      {
         if (m_ppszTitles[i]) free(m_ppszTitles[i]);
      }
      free(m_ppszTitles);
   }
   if (CEtsCDPlayer_Global::pDestroy && m_pParam)
   {
      CEtsCDPlayer_Global::pDestroy(this);
   }
}
 
bool CEtsCDPlayer::SetNoOfTitles(int nTitles)
{
   m_ppszTitles = (char**)realloc(m_ppszTitles, sizeof(char*)*nTitles);
   if (m_ppszTitles)
   {
      int i;
      for (i=m_nTitles; i<nTitles; i++)
      {
         m_ppszTitles[i] = NULL;
      }
      m_nTitles = nTitles;
      return true;
   }
   return false;
}
bool CEtsCDPlayer::SetTitle(int n, char*szTitle)
{
   if ((n >= 0) && (n < m_nTitles) && (szTitle != NULL))
   {
      m_ppszTitles[n] = (char*)realloc(m_ppszTitles[n], strlen(szTitle)+1);
      if (m_ppszTitles[n])
      {
         strcpy(m_ppszTitles[n], szTitle);
         return true;
      }
   }
   return false;
}

void CEtsCDPlayer::Destructor()
{
   ETSUnBind(ETSCDPL_MODULENAME,                               // Dateinamen der DLL
             CEtsCDPlayer_Global::hDLL,                        // obigen Module Handle für die DLL (reference)
             (void**)&CEtsCDPlayer_Global::pDoModal,           // reference auf die Adresse des ersten Funktionspointer
             NO_OF_ETSCDPL_FUNCTIONS,                          // die Anzahl der Funktionspointer
             NULL,                                             // Anzahl der Instanzen
             this);                                            // Parameter
}

BOOL CEtsCDPlayer::DoModal(HWND hwndParent)
{
   if (CEtsCDPlayer_Global::hDLL == NULL)
   {
      if (!CEtsCDPlayer_Global::BindDll()) return false;
   }
   return (CEtsCDPlayer_Global::pDoModal(hwndParent, this));
}


HWND CEtsCDPlayer::Create(HWND hwndParent)
{
   if (CEtsCDPlayer_Global::hDLL == NULL)
   {
      if (!CEtsCDPlayer_Global::BindDll()) return false;
   }
   return (CEtsCDPlayer_Global::pCreate(hwndParent, this));
}

long CEtsCDPlayer::DoDataExchange(DWORD dwType, int nSize, void *pParam)
{
   if (CEtsCDPlayer_Global::hDLL == NULL)
   {
      if (!CEtsCDPlayer_Global::BindDll()) return 0;
   }
   return (CEtsCDPlayer_Global::pDoDataExchange(this, dwType, nSize, pParam));
}

HMENU CEtsCDPlayer::GetContextMenu()
{
   HMENU hMenu = NULL;
   DoDataExchange(GET_CONTEXT_MENU, sizeof(HMENU), &hMenu);
   return hMenu;
}

HWND CEtsCDPlayer::GetWindowHandle()
{
   HWND hWnd = NULL;
   DoDataExchange(GET_WINDOW_HANDLE, sizeof(HWND), &hWnd);
   return hWnd;
}

bool CEtsCDPlayer::DoTitleDlg(HWND hwndParent, char *pszPath)
{
   BrowseFolderStruct bfs = {pszPath, NULL, NULL, 0, hwndParent};
   return (DoDataExchange(DO_TITLEDLG, sizeof(BrowseFolderStruct), &bfs) != 0) ? true : false;
}

bool CEtsCDPlayer::DoMixerDlg(HWND hwndParent)
{ 
   BrowseFolderStruct bfs = {NULL, NULL, NULL, 0, hwndParent};
   return (DoDataExchange(DO_MIXER_DLG, sizeof(BrowseFolderStruct), &bfs) != 0) ? true : false;
}

HWND CEtsCDPlayer::CreateMixerDialog(HWND hwndParent)
{
   HWND hwnd = NULL;
   BrowseFolderStruct bfs = {NULL, NULL, &hwnd, 1, hwndParent};
   DoDataExchange(DO_MIXER_DLG, sizeof(BrowseFolderStruct), &bfs);
   return hwnd;
}

long CEtsCDPlayer::SendMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   MSG msg = {NULL, nMsg, wParam, lParam, 0, {0, 0}};
   long lResult = DoDataExchange(DO_MESSAGE, sizeof(MSG), &msg);
   if (msg.time != 2) lResult = -1;
   return lResult;
}

long CEtsCDPlayer::PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   MSG msg = {NULL, nMsg, wParam, lParam, 1, {0, 0}};
   long lResult = DoDataExchange(DO_MESSAGE, sizeof(MSG), &msg);
   if (msg.time != 2) lResult = -1;
   return lResult;
}

long CEtsCDPlayer::BrowseFolder(HWND hwndParent, char *pszTitle, UINT nFlags, char *pszFolder, long*pHandle)
{
   BrowseFolderStruct bfs = {pszFolder, pszTitle, pHandle, nFlags, hwndParent};
   return (DoDataExchange(BROWSE_FOR_FOLDER, sizeof(BrowseFolderStruct), &bfs) != 0) ? true : false;
}

bool CEtsCDPlayer::FreeHandle(long lHandle)
{
   BrowseFolderStruct bfs = {NULL, NULL, &lHandle, 0, NULL};
   DoDataExchange(FREE_PIDL, sizeof(BrowseFolderStruct), &bfs);
   return (lHandle==0) ? true : false;
}

