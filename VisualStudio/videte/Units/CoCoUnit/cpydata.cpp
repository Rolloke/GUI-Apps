/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: cpydata.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/cpydata.cpp $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "string.h"
#include "cpydata.h"


/////////////////////////////////////////////////////////////////////////////
// CWmCopyData::CWmCopyData
CWmCopyData::CWmCopyData()
{
//	TRACE("CWmCopyData::CWmCopyData\n");
}

/////////////////////////////////////////////////////////////////////////////
// CWmCopyData::~CWmCopyData
CWmCopyData::~CWmCopyData()
{
//	TRACE("CWmCopyData::~CWmCopyData\n");
}

/////////////////////////////////////////////////////////////////////////////
// CWmCopyData::WriteData
BOOL CWmCopyData::WriteData(HWND hWndTo, PARAMSTRUCT &Param)
{
	if (hWndTo == NULL)
		return FALSE;
	
	if ((Param.dwLen == 0L) || (Param.lpData == NULL))
		return FALSE;

    HANDLE 	hMem;
    LPSTR   lpMem;
	int nSize 	= sizeof(PARAMSTRUCT);
    
	// Transferbuffer für Header + Daten anfordern
	hMem = GlobalAlloc(GHND	,Param.dwLen + (DWORD) nSize);
	if (!hMem)
	{
		OutputDebugString("ERROR:\tCWmCopyData::WriteData\tTransferbufferhandle ungültig\n");
		return FALSE;
    }
    
	lpMem = (LPSTR)GlobalLock(hMem);
	if (!lpMem)
	{
		OutputDebugString("ERROR:\tCWmCopyData::WriteData\tTransferbuffer konnte nicht angelegt werden\n");
	 	GlobalFree(hMem);
	 	return FALSE;
	}

	// Header und Daten in den Transferbuffer kopieren.
	memcpy(lpMem, &Param, (size_t)nSize);
	memcpy(lpMem + (DWORD)nSize, Param.lpData, (size_t)Param.dwLen);
	
	COPYDATASTRUCT CDS;

	CDS.dwData	= Param.dwData;              
	CDS.cbData	= Param.dwLen + (DWORD)nSize;
	CDS.lpData	= (PVOID)lpMem;

	// Daten verschicken
	Param.dwLen = ::SendMessage(hWndTo, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&CDS);

 	GlobalUnlock(hMem);
 	GlobalFree(hMem);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWmCopyData::ReadData
BOOL CWmCopyData::ReadData(LPARAM lParam, PARAMSTRUCT &Param)
{
	LPSTR lpMem;
	int nSize 	= sizeof(PARAMSTRUCT);

    memset(&Param, 0, (size_t)nSize);
	
	COPYDATASTRUCT *pCDS = (PCOPYDATASTRUCT)lParam;
    
	if (pCDS == NULL)
	{
		OutputDebugString("ERROR:\tCWmCopyData::ReadData\tPCOPYDATASTRUCT = NULL\n");
		return FALSE;
	}	
	lpMem	= (LPSTR)pCDS->lpData;
	
	if (!lpMem)
	{
		OutputDebugString("ERROR:\tCWmCopyData::ReadData\tlpMem = NULL\n");
		return FALSE;
	}
	    
	memcpy(&Param, lpMem, (size_t)nSize);

	Param.lpData = lpMem + (DWORD)nSize;

	return TRUE;
}
