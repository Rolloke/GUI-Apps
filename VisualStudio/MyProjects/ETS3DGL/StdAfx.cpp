// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	CARA3D.pch ist die vorkompilierte Header-Datei
//	stdafx.obj enthält die vorkompilierte Typinformation

#include "stdafx.h"

// ZU ERLEDIGEN: Verweis auf alle zusätzlichen Header-Dateien, die Sie in STDAFX.H
// und nicht in dieser Datei benötigen
#if defined _DEBUG
void  ReportLastWinError()
{
   LPVOID lpMsgBuf;
   FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, 
                 NULL, GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                 (LPTSTR) &lpMsgBuf, 0, NULL );
   MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
   LocalFree( lpMsgBuf );
}
#endif

