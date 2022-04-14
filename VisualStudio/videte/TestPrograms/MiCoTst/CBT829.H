/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CBT829.H $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CBT829.H $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CBT829_H__ 
#define __CBT829_H__
#include "ci2c.h"

#define SIZE 			255
#define BT829_MAX_REG  	40
class Cio;

class CBT829 : public CI2C
{
	public:
		CBT829(WORD wIOBase, WORD wFeID);
		~CBT829();
	
	 	BOOL Init();
		BOOL IsValid();
		BOOL CheckIt();
	
		BOOL SetFeVideoFormat(WORD wFeVideoFormat);
		BOOL SetFeSourceType(WORD wFeSourceType, WORD wFeFilter);
		BOOL SetFeBrightness(WORD wFeBrightness);
		BOOL SetFeContrast(WORD wFeContrast);
		BOOL SetFeSaturation(WORD wFeSaturation);
		BOOL SetFeHue(WORD wFeHue);
		WORD SetFeHScale(WORD wHScale);
		WORD SetFeVScale(WORD wVScale);
		WORD SetFeFilter(WORD wFilter);
	
		WORD GetFeVideoFormat();
		WORD GetFeSourceType();
		WORD GetFeBrightness();
		WORD GetFeContrast();
		WORD GetFeSaturation();
		WORD GetFeHue();
		WORD GetFeHScale();
		WORD GetFeVScale();
		WORD GetFeFilter();
		BOOL IsVideoPresent();

	private:
		void CBT829Reset();
		BOOL BT829Out(BYTE bySubAddress, BYTE byData);
		BYTE BT829In(BYTE bySubAddress);
		BYTE m_byBT829Reg[BT829_MAX_REG][4];		

		static int 	m_nBT829InstCnt;
		static CCriticalSection m_csBT829;

		CIo  m_Io;
		WORD m_wIOBase;
		WORD m_wFeID;
		WORD m_wFeVideoFormat;
		WORD m_wFeSourceType;
		WORD m_wFeBrightness;
		WORD m_wFeContrast;
		WORD m_wFeSaturation;
		WORD m_wFeHue;
		WORD m_wFeHScale;
		WORD m_wFeVScale;
		WORD m_wFeFilter;
		BOOL m_bOk;
};
		
#endif // __CBT829_H__


