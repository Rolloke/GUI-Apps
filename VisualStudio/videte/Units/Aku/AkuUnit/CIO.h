/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CIO.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CIO.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CIO_H__ 
#define __CIO_H__

#include "cipc.h"

class CIo
{
public:
	BYTE Input(WORD wPort);
	WORD Inputw(WORD wPort);
	void Output(WORD wPort, BYTE byDatabyte);
	void Outputw(WORD wPort, WORD wDatabyte); 
	BYTE ReadFromLatch(WORD wPort);
	void WriteToLatch(WORD wPort, BYTE byDatabyte);

private:
	static CCriticalSection m_csIo;
};

#endif // __CIO_H__