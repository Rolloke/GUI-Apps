/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecID.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/SecID.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 19.01.06 16:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$


#include "stdcipc.h"
#include "SecID.h"
#include "wkclasses\WK_Trace.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CSecID::Set(WORD wInvalidID)
{
	// never called from outside, it's private!
	WK_TRACE_ERROR(_T("CSecID::Set(WORD wInvalidID) WRONG Parameter WORD %08x\n"), wInvalidID);
}
CSecID::CSecID(WORD wInvalidID)
{
	// never called from outside, it's private!
	WK_TRACE(_T("ERROR: CSecID(WORD wInvalidID) WRONG Parameter WORD %08x\n"), wInvalidID);
}


void CSecID::InternalErrorIdRange(DWORD dwFullID)
{
	WK_TRACE_ERROR(_T("SecID range error %08x\n"), dwFullID);
}
//NOTE: CSecID's one ond only not-inlined fn is here:
// class check:
BOOL CSecID::IsClassOkay()
{
	BOOL okaySize,okayOrder;
	// condition1: union must have sizeof(DWORD);
	okaySize= (sizeof(CSecID)==sizeof(DWORD));
	if (okaySize==FALSE) 
	{
		WK_TRACE(_T("CSecID sizeError CSecID %d!= DWORD %d\n"), sizeof(CSecID), sizeof(DWORD));
		ASSERT(0);
	}
	Set(1,2);
	okayOrder = (GetID()==0x010002);
	if (okayOrder==FALSE)
	{
		WK_TRACE(_T("CSecID orderError GetID %d!= DWORD %d\n"), GetID(), 0x010002);
		ASSERT(0);
	}
	
	return (okaySize && okayOrder);
}
