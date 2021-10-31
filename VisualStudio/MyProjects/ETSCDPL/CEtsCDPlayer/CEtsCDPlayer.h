/*******************************************************************************
                                 CEtsCDPlayer
  Zweck:
  Abspielen von Wavedateien mit Hilfe der Windows Multimedia-Funktionen
  
  Header:            Bibliotheken:        DLL´s:
   ----              EtsBind.lib          ETSBIND.dll
   ----              ----                 ETSCDPL.dll
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die ETSCDPL.dll ist eine
   "load on call-Dll". D.h., sie wird erst geladen, wenn sie das erste mal be-
   nötigt wird.

Funktionen :

   bool  SetNoOfTitles(int);      // Setzt die Anzahl der übergebenen Wave-Pfade.
   bool  SetTitle(int, char*);    // Setzt den n-ten Wave-Pfad.
   BOOL  DoModal(HWND);           // Ruft den Dialog Modal auf.
   HWND  Create(HWND);            // Ruft den Dialog nicht-Modal auf.
   HMENU GetContextMenu();        // liefert das Contextmenü des CD-Spielers
   bool  DoTitleDlg(HWND, char*); // Öffnet die übergebene Titeldatei mit dem Dialog
   bool  DoMixerDlg(HWND);        // Ruft den Mixerdialog Modal auf
   HWND  CreateMixerDialog(HWND); // Ruft den Mixerdialog nicht-Modal auf

   wird der CD-Dialog mit Create aufgerufen, so kann mit folgenden Funktionen
   auf den nicht modalen Dialog zugegriffen werden.
      HWND  GetWindowHandle();
	   long  PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	   long  SendMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);

   zusätzlich exportierte Funktionen um den Shellfolderdialog auzurufen
      long  BrowseFolder(HWND hwndParent, char*pszTitle, UINT nFlags, char*pszPath, long*pHandle=NULL);
      bool  FreeHandle(long);

  Copyright:
   (C) 2001 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 31.10.2001
                                                programmed by Rolf Kary-Ehlers
*******************************************************************************/


#ifndef _CETSCDPL_H_
#define _CETSCDPL_H_

#include <windows.h>

#define FOLDERPATH_CONCAT_SLASH  0x00000001
#define FOLDERPATH_NO_BROWSER    0x00000002
#define FOLDERPATH_RETURN_FILES  0x00000004
#define FOLDERPATH_RETURN_HANDLE 0x00000008

#define TITLES_FOR_RECORDING      0x80000000

// Command ID´s für die Dialogfeld Buttons
#define IDC_CD_EJECT       1002 // Auswurf
#define IDC_CD_PLAY        1003 // Start
#define IDC_CD_STOP        1004 // Stop
#define IDC_CD_PAUSE       1005 // Pause
#define IDC_CD_NEXT_TRACK  1006 // nächster Titel
#define IDC_CD_PREV_TRACK  1007 // vorhergehender Titel
#define IDC_CD_FORWARD     1008 // vorspulen
#define IDC_CD_REWIND      1009 // zurückspulen

class CEtsCDPlayer
{
public:
      CEtsCDPlayer();
      ~CEtsCDPlayer();
      void  Destructor();

      bool  SetNoOfTitles(int);
	   bool  SetTitle(int, char*);
      BOOL  DoModal(HWND);
      HWND  Create(HWND);
   	long  DoDataExchange(DWORD, int, void*);   // dwType, nSize, pParam

   	HWND  GetWindowHandle();
	   HMENU GetContextMenu();
	   bool  DoTitleDlg(HWND, char*);
	   long  PostMessage(UINT, WPARAM, LPARAM);
	   long  SendMessage(UINT, WPARAM, LPARAM);
   	long  BrowseFolder(HWND, char*, UINT, char*, long*pHandle=NULL);
   	bool  FreeHandle(long);
	   bool  DoMixerDlg(HWND);
	   HWND  CreateMixerDialog(HWND);

private:
   int    m_nTitles;
   char **m_ppszTitles;
   void  *m_pParam;
};

#endif // _CETSCDPL_H_

