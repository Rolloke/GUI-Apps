///////////////////////////////////////////////////////////////
// FILE    :	$Workfile: TickCount.cpp $
// ARCHIVE :	$Archive: /Project/Tools/ClientTester/TickCount.cpp $
// CHECKIN :	$Date: 23.04.97 19:01 $
// VERSION :	$Revision: 1 $
// LAST    :	$Modtime: 14.04.97 11:19 $
// AUTHOR  :	$Author: Hajo $
//         : $Nokeywords:$
/**modulref ***************************************************
 * name    :  TickCount.cpp
 * class   : CTickCounter
 *         : 
 * uses    : GetTickCount(); in start and stop
 * members : 
 * protect : DWORD m_dwFirstTick;
 *         : DWORD m_dwLastTick;
 * public  : DWORD GetDeltaTicks():returns m_dwLastTick-m_dwFirstTick
 *         : DWORD GetFirstTick() :returns m_dwFirstTick
 *         : DWORD GetLastTick()  :returns m_dwLastTick
 * purpose : 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
#include "StdAfx.h"
#include "TickCount.h"

CTickCounter::CTickCounter()
{
 m_dwFirstTick = 0;
 m_dwLastTick  = 0;
}
CTickCounter::~CTickCounter()
{
}

/**funcref ****************************************************
 * class   : CTickCounter
 * name    : void StartCount(
 * input   : void);
 *         : 
 * purpose : sets m_dwFirstTick
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
void CTickCounter::StartCount(void) 
{
 m_dwFirstTick=GetTickCount(); 
}

/**funcref ****************************************************
 * class   : CTickCounter
 * name    : void StopCount(
 * input   : void);
 *         : 
 * purpose : sets m_dwLastTick
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
void CTickCounter::StopCount(void)
{
 m_dwLastTick=GetTickCount(); 
}
