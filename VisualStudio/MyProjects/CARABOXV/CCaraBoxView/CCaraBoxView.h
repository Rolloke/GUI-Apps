/*******************************************************************************
                                 CCaraBoxView
  Zweck:
   3D-Darstellung von Boxen in eine unabhängigen Dialogbox.
  Header:            Bibliotheken:        DLL´s:         Dateien:
   ----              EtsBind.lib          ETSBIND.dll
   ----              CCaraBocView.lib     CARABOXV.dll
   ----              ----                 CARAWALK.dll   TurnBox.cfg
   ----              ----                 CARAMAT.dll    CaraRefl.mat
   ----              ----                 ETS3DGL.dll
   
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die weiteren sind sog.
   "load on call-Dll´s". D.h., sie werden erst geladen, wenn sie das erste 
   mal benötigt werden.
   Funktionen :
   Objekt.Create(hwndParent)
      Erzeugt die Dialogbox.
   Objekt.DrawBox3DView("C:\W98\Temp\Boxname.TMP")
      Setzt die Boxendaten. Die Datei muß im Windows-Temp-Verzeichnis stehen.
   Objekt.Destructor()
      Entfernt die Dll aus dem Speicher. Diese Funktion wird vor dem
      zerstören der letzten Objektinstanz aufgerufen.
   Objekt.IsWindow()
      Ermittelt ob das Dialogfenster vorhanden ist.
   Objekt.PostMessage()
      Postet eine Message zum Dialogfenster.
   Objekt.SendMessage()
      Sendet eine Message zum Dialogfenster.

  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 28.09.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/
#ifndef _CCARABOXVIEW_H_
#define _CCARABOXVIEW_H_

#ifndef STRICT
   #define  STRICT
#endif

#include <WINDOWS.H>

class CCaraBoxView
{
public:
   CCaraBoxView();
   ~CCaraBoxView();
   void     Destructor();
   bool     Create(HWND);
   BOOL     IsWindow() {return ::IsWindow(m_hWndDlg);};
   LRESULT  PostMessage(UINT, WPARAM, LPARAM);
   LRESULT  SendMessage(UINT, WPARAM, LPARAM);
   void     DrawBox3DView(char *);

private:
   void CloseDialog();

   volatile HWND m_hWndDlg; // Threadsicherheit
   HANDLE m_hThread;
   ULONG  m_nThreadID;
   HWND   m_hWndParent;
};

#endif