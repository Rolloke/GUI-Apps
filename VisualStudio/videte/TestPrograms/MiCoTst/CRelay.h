/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CRelay.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CRelay.h $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CRELAY_H__ 
#define __CRELAY_H__

#include "CIo.h"
 
class CRelay
{
	public:
		CRelay(WORD wIOBase, WORD wExtCard, BOOL bIgnoreRelay4);
		~CRelay();

		void SetRelay(BYTE byRelay);
		BYTE GetRelay();
		void BeeperOn();
		void BeeperOff();
		void CheckIt(WORD wNr);
		
	private:
		WORD m_wIOBase;
		WORD m_wExtCard;
		CIo  m_IoAccess;
		BOOL m_bIgnoreRelay4;
};
		
#endif // __CRELAY_H__


