/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CSavicException.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CSavicException.h $
// CHECKIN:		$Date: 24.06.02 13:52 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 13:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CSAVICEXCEPTION_H__
#define __CSAVICEXCEPTION_H__
#include "UnhandledException.h"

class CSaVicException : public CUnhandledException
{
public:
	CSaVicException(WK_ApplicationId AppId, PVOID pContext = NULL);

	virtual LONG OnExceptionHandler(LPEXCEPTION_POINTERS pExc);

private:
	BOOL m_bHandleCurrentException;
};


#endif // __CSAVICEXCEPTION_H__
