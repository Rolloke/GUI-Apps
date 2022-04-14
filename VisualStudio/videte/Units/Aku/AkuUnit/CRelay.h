/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CRelay.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CRelay.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CRELAY_H__ 
#define __CRELAY_H__

#include "CIo.h"
 
class CRelay
{
	public:
		CRelay(WORD wIOBase, WORD wExtCard);
		~CRelay();

		void SetRelay(BYTE byRelay);
		BYTE GetRelay();

	private:
		WORD m_wIOBase;
		WORD m_wAkuCard;
		CIo  m_IoAccess;
};
		
#endif // __CRELAY_H__


