// FILE: $Workfile: TickCount.h $ from $Archive: /Project/Tools/ClientTester/TickCount.h $
// VERSION: $Date: 23.04.97 19:01 $ $Revision: 1 $
// LAST:	$Modtime: 19.03.97 14:48 $ by $Author: Hajo $
//$Nokeywords:$

/**headerRef***************************************************
 * header  :  TickCount.h
 * class   : CTickCounter
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
#ifndef CTICKCOUNTER_H
	#define CTICKCOUNTER_H

/**classref ***************************************************
 * class   : CTickCounter
 *         : 
 * uses    : GetTickCount(); in start and stop
 * members : 
 * protect : DWORD m_dwFirstTick;
 *         : DWORD m_dwLastTick;
 * public  : DWORD GetDeltaTicks():returns m_LastTick-m_FirstTick
 *         : DWORD GetFirstTick() :returns m_FirstTick
 *         : DWORD GetLastTick()  :returns m_LastTick
 *         : 
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
class CTickCounter
{
public:
	CTickCounter();
	virtual ~CTickCounter();
	//
	void	StartCount(void);
	void	StopCount(void);
	inline	DWORD	GetDeltaTicks() const;
	inline	DWORD	GetFirstTick() const;
	inline	DWORD	GetLastTick() const;
	// data
protected:
	DWORD	m_dwFirstTick;
	DWORD	m_dwLastTick;

};

inline DWORD CTickCounter::GetDeltaTicks() const
{
	return (m_dwLastTick - m_dwFirstTick);
}
inline DWORD CTickCounter::GetFirstTick() const
{
	return m_dwFirstTick;
}
inline DWORD CTickCounter::GetLastTick() const
{
	return m_dwLastTick;
}

#endif
