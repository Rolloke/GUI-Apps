/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: timerlist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/timerlist.h $
// CHECKIN:		$Date: 3.08.98 11:51 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 3.08.98 11:49 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#ifndef _TIMER_LIST_H
#define _TIMER_LIST_H

#include "wk.h"
#include "SecID.h"

/////////////////////////////////////////////////////////////////////////////
// Timerstrukturen
#define T_MAXOFF		4
#define T_MAX_INP		8
#define T_MAX_DAYS		8

extern PSTR Tnr2String(PSTR p, WORD wOff, WORD wOn);
// Ändert die Minutennummer der on off Minuten in "Stunden:Minuten-Stunden:Minuten" String
// p:		Pointer auf den String
// wOff:	Off-Minute [1...10080]
// wOn:	  	On- Minute [1...10080]

extern void String2Tnr(PCSTR p, WORD& wOff, WORD& wOn, int n);
// Ändert den String "Stunden:Minuten-Stunden:Minuten" int wOff - wOn zahlen um
// p:		Pointer auf String "XX:XX-XX:XX"
// wOff:	Off-Minute [1...10080]
// wOn:	  	On- Minute [1...10080]
// n:		Vochentag [0...6]


/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	WORD wOff;
	WORD wOn;
} ONOFF;	

/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	ONOFF b[T_MAXOFF];	
} ONE_DAY;				
typedef ONE_DAY* PONE_DAY;	

/////////////////////////////////////////////////////////////////////////////
class CTimer : public CObject
{
	// construction
public:
	CTimer();
	CTimer(CTimer* pTimer);

	// access
public:
	inline CSecID	GetID() const;
	inline CString	GetName() const;

	// operations
public:
	inline void	SetID(CSecID id);
	inline void	SetName(PCSTR szName);

public:
	CSecID	idInput[T_MAX_INP];	// Max. 8 eingänge können ein Timer beeinflüßen
	ONE_DAY	Day[T_MAX_DAYS];	// Max. 7 Tage hat die Woche...
private:
	CSecID	m_ID;
	CString	m_sName;
};
/////////////////////////////////////////////////////////////////////////////
inline CSecID CTimer::GetID() const
{
	return (m_ID);
}
/////////////////////////////////////////////////////////////////////////////
inline CString CTimer::GetName() const
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////////////
inline void CTimer::SetID(CSecID id)
{
	m_ID = id;
}
/////////////////////////////////////////////////////////////////////////////
inline void CTimer::SetName(PCSTR szName)
{
	m_sName = szName;
}

/////////////////////////////////////////////////////////////////////////////
typedef CTimer* CTimerPtr;
WK_PTR_ARRAY(CTimerPtrArray,CTimerPtr);
/////////////////////////////////////////////////////////////////////////////
class CTimerList : public CTimerPtrArray
{
public:
	CTimerList();
	~CTimerList();

	// Check for invalid Add
	inline int	Add(CTimer* pTimer);

	CTimer*		GetTimerByID(CSecID id) const;
	void		AddTimer(CTimer* pTimer);
	void		DeleteTimerByID(CSecID id);

	void		Load(CWK_Profile& wkp);
	void		Save(CWK_Profile& wkp);
	void		ClearTimers(CWK_Profile& wkp);
	void		SaveTimer(int iEntry, CTimer* pT,CWK_Profile& wkp);

	void 		DeleteTimer(int iIndex);

	void		Reset();

protected:
	CSecID		GetMaxTimerID() const;
	CSecID		GetFreeTimerID() const;
};
/////////////////////////////////////////////////////////////////////////////
inline int CTimerList::Add(CTimer* pTimer)
{
	ASSERT(pTimer->GetID().GetID() != SECID_NO_ID);
	return CTimerPtrArray::Add(pTimer);
}

#endif
