/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CTashaException.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CTashaException.h $
// CHECKIN:		$Date: 29.03.04 9:37 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 29.03.04 8:30 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CTASHAEXCEPTION_H__
#define __CTASHAEXCEPTION_H__
#include "UnhandledException.h"

class CTashaException : public CUnhandledException
{
public:
	CTashaException(WK_ApplicationId AppId, PVOID pContext = NULL);

	virtual LONG OnExceptionHandler(LPEXCEPTION_POINTERS pExc);

private:
	BOOL m_bHandleCurrentException;
};


#endif // __CTASHAEXCEPTION_H__
