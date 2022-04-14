// StringArrayOnce.cpp: implementation of the CStringArrayOnce class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractBuildLog.h"
#include "StringArrayOnce.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CStringArrayOnce::AddOnce(CString& sEntry)
{
	BOOL bRet = TRUE;
	for (int i=0 ; i<GetSize() ; i++)
	{
		if (GetAt(i) == sEntry)
		{
			bRet = FALSE;
			break;
		}
	}
	if (bRet)
	{
		Add(sEntry);
	}
	return bRet;
}
