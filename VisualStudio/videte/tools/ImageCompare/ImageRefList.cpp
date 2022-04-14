/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageRefList.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageRefList.cpp $
// CHECKIN:		$Date: 22.04.99 16:16 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 22.04.99 16:16 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wk.h"
#include "ImageCompare.h"
#include "ImageRef.h"
#include "ImageRefList.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CImageRefList::CImageRefList()
{
}

//////////////////////////////////////////////////////////////////////
CImageRefList::~CImageRefList()
{
	// Alle Einträge aus dem Listenobjekt in die Listbox kopieren
	for (int nI = 0; nI < GetCount(); nI++)
	{
		CImageRef *pImageRef = (CImageRef*)GetAt(FindIndex(nI));
		if (pImageRef)
			WK_DELETE(pImageRef);
	}

	RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CImageRefList::AddImageRef(CImageRef* pImageRef)
{
	if (!pImageRef)
		return FALSE;

	AddTail(pImageRef);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL CImageRefList::AddImageRef(CSR sHostName, CSR sCamName, CSR sImageName1, CSR sImageName2)
{
	CImageRef *pImageRef = new CImageRef(sHostName, sCamName, sImageName1, sImageName2);
	if (!pImageRef)
		return FALSE;

	AddTail(pImageRef);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
CImageRef* CImageRefList::GetImageRef(int nIndex)
{
	return (CImageRef*)GetAt(FindIndex(nIndex));
}