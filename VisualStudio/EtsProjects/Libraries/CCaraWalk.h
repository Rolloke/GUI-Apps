/*******************************************************************************
                                 CCaraWalk
  Zweck:
   Klasse zur 3-dimensionalen Darstellung des Raumes in CaraCad und der
   Lautsprecherboxen im Raum bzw. in den Dialogboxen.
   Die Funktionen sind in der CARAWALK.dll implementiert.
  Header:            Bibliotheken:        DLL´s:         Dateien
   ----              EtsBind.lib          ETSBIND.dll    ----
   ----              ----                 CARAWALK.dll   TurnBox.cfg
   ----              ----                 CARAMAT.dll    CaraRefl.mat
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.10 Stand 23.09.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined(AFX_CARAWALK_H__573CA688_DDD9_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_CARAWALK_H__573CA688_DDD9_11D2_9E4E_0000B458D891__INCLUDED_

#include "EtsAppWndMsg.h"

// Definitionen für bool DoDataExchange(int nSize, void *ptr);
#define CARAWALK_GETHINSTANCE          1
// nSize = sizeof(HINSTANCE), ptr = &hInstance
#define CARAWALK_GETTOOLBAR_ID         2
#define CARAWALK_GETMENU_ID            3
#define CARAWALK_GETACCELERATOR_ID     4
// nSize = sizeof(int), ptr = &nID
#define CARAWALK_SETROOMDATA           5
// nSize = Länge der Daten, ptr = Speicherbereich mit Daten
#define CARAWALK_SETANGLE              6  // 
// wParam Bit 2 : 0 = Setzen der Winkel         , lParam = Zeiger auf Struktur CARAWALK_BoxPos
//              : 1 = Setzen des Abstandsvektors, lParam = Zeiger CVector
#define CARAWALK_SETDISTANCE_VECTOR   0x0002

#define CARAWALK_SETBOXDATA           7 // Setzt die Boxendaten für den BoxView
// wParam Bit 0 : Darstellung mit (0 = User-Reference-Point, 1 = Maßstab)
//        Bit 1 : Darstellung der (0 = Box Einfach, 1 = Box Zweifach)
//        Bit 16-32 Nummer des Materials oder 0xffff
// lParam = pszPath auf Boxendatei
#define CARAWALK_SHOW_SCALE                0x0001
#define CARAWALK_SHOW_TWICE                0x0002
#define CARAWALK_SHOW_URP_AND_SCALE        0x0004

#define CARAWALK_GETCOMMAND_IDS        8  // liefert die Command IDs für WM_COMMAND

#define CARAWALK_SET3DMODE             9
// nSize = sizeof(DWORD)*2
// DWORD dwModes[2];
// ptr   = &dwModes => dwMode[0] = SetMode
//                  => dwMode[1] = RemoveMode

#define BOX_VIEW     "Boxview"
#define SCREEN_SAVER "ScreenSaver"

struct CARAWALK_BoxPos              // Struktur für die Boxenposition
{
   long          nPhi;              // Phiwinkel der Positionen
   long          nTheta;            // Thetawinkel der Positionen
   long          nPsi;              // Psiwinkel der Positionen
   double        pdPos[3];          // Positionen der Box
};

class CCaraWalk  
{
public:
	CCaraWalk();
   ~CCaraWalk();
	void Destructor();
	HWND Create(LPCTSTR, DWORD, RECT*, HWND); // WindowText, dwStyle, prcWnd, hwndParent
	bool DoDataExchange(DWORD, int, void*);   // dwType, nSize, pParam
	BOOL MoveWindow(int, int, int, int, bool);// left, top, width, height, bRedraw
private:
   void *m_pData;               // Platzhalter für Datenzeiger
};

#endif // !defined(AFX_CARAWALK_H__573CA688_DDD9_11D2_9E4E_0000B458D891__INCLUDED_)
