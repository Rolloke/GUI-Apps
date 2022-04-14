/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CWDog.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CWDog.h $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CWATCHDOG_H__
#define __CWATCHDOG_H__

#include "CIO.h"

#define SIZE							255

class CWatchDog
{
public:
	CWatchDog(WORD wIOBase);
	~CWatchDog();

public:
	BOOL Init(WORD wTi);
	BOOL Reset(WORD wTi);
	BOOL Piezo(BOOL bOn);
	BOOL CheckIt();

private:
	WORD GetCurrentPTC0();
	WORD m_wIOBase;
	CIo		m_IO;
};

#endif // __CWATCHDOG_H__