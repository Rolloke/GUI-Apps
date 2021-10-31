// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__66199326_3536_49EB_A361_98B930118F60__INCLUDED_)
#define AFX_STDAFX_H__66199326_3536_49EB_A361_98B930118F60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden
#define  STRICT

#include <windows.h>
#include <Mmsystem.h>
#include <stdlib.h>
#include <COMMCTRL.H>
#include <Commdlg.h>
#include <shlobj.h>

#define FOLDERPATH_CONCAT_SLASH  0x00000001
#define FOLDERPATH_NO_BROWSER    0x00000002
#define FOLDERPATH_RETURN_FILES  0x00000004
#define FOLDERPATH_RETURN_HANDLE 0x00000008

#define INVALID_VALUE 0xffffffff

#define WM_EDITCTRL_KEYUP (WM_APP+1)

#ifndef ASSERT
   #ifdef _DEBUG
      #include <crtdbg.h>
      #define ASSERT(expr) \
           do { if (!(expr) && \
                (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
                _CrtDbgBreak(); } while (0)
      #define VERIFY(f) ASSERT(f)
   #else
      #define ASSERT(f)    ((void)(0))
      #define VERIFY(f)    ((void)(f))
   #endif//_DEBUG
#endif

int  GetColorDiff(DWORD, DWORD);
bool GetFolderPath(char *, ITEMIDLIST**, UINT, LPCTSTR, HWND);

int RemoveCharacter(char*, char, bool);
int ConcatCharacter(char*, char, bool);

// ZU ERLEDIGEN: Verweisen Sie hier auf zusätzliche Header-Dateien, die Ihr Programm benötigt

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

#endif // !defined(AFX_STDAFX_H__66199326_3536_49EB_A361_98B930118F60__INCLUDED_)
