/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CAlarm.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CAlarm.h $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CALARM_H__ 
#define __CALARM_H__

#include "CIo.h"
 
class CAlarm
{
	public:
		CAlarm(WORD wIOBase, WORD wExtCard);
		~CAlarm();

		BYTE GetCurrentState();
		BYTE GetState();
		void SetEdge(BYTE byState);
		BYTE GetEdge();
		void SetAck(BYTE byAck);
		BYTE GetAck();
		BOOL IsValid();
		BOOL CheckIt();
				
	private:
		WORD m_wExtCard;
		WORD m_wIOBase;
		BYTE m_byAck;
		BYTE m_byEdge;
		CIo  IoAccess;
};
		
#endif // __CALARM_H__


