/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CBT856.H $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CBT856.H $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CBT856_H__ 
#define __CBT856_H__
#include "ci2c.h"

#define SIZE 			255
#define BT856_MAX_REG   0xde

class CIo;

class CBT856 : public CI2C
{
	public:
		CBT856(WORD wIOBase);
		~CBT856();
		
		BOOL Init();
		BOOL IsValid();
		BOOL CheckIt();
	
		WORD SetBeVideoFormat(WORD wBeVideoFormat);
		WORD GetBeVideoFormat();

	private:
		void CBT856Reset();
		BOOL BT856Out(BYTE bySubAddress, BYTE byData);
		BYTE BT856In(BYTE bySubAddress);

		BYTE m_byBT856Reg[BT856_MAX_REG+1];		
		WORD m_wBeVideoFormat;
		CIo  m_Io;
		WORD m_wIOBase;
		BOOL m_bOk;

		CCriticalSection m_csBT856;
};
		
#endif // __CBT856_H__


