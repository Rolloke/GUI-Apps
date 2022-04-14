// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//  IPCameraUnit.pch ist der vorcompilierte Header
//  stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"


void RemoveLastCharacter(CString&s, TCHAR c)
{
	if (s.GetLength() && s.GetAt(s.GetLength()-1) == c)
	{
		s = s.Left(s.GetLength()-1);
	}
}
