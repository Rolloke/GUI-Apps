/* GlobalReplace: WeekDayForEndTime --> DynamicPatternForEndTime */
/* GlobalReplace: m_bHasDynamicPatternForEndTime --> m_bHasDynamicEndTime */
/* GlobalReplace: HasWeekDayForStartTime --> HasDynamicStartTime */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecTimer.h $
// ARCHIVE:		$Archive: /Project/CIPC/SecTimer.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 42 $
// LAST CHANGE:	$Modtime: 19.01.06 21:26 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CSecTimer_H
#define _CSecTimer_H

#include "wk.h"
#include "SecTimeSpan.h"
#include "SecIDArray.h"

//

class CSecTimerArray;	// foward declaration 
// old was subclassed from CTimeSpanArray, now it contains a CTimeSpanArray
//  a collection of timespans of timer ids.
// NOT YET more doc.
class AFX_EXT_CLASS CSecTimer 
{
public:
	//  public
	//!ic! default constructor
	CSecTimer();
	//!ic! copy constructor
	CSecTimer(const CSecTimer &src);	// full copy, no pointer copy
	//!ic! destructor
	virtual ~CSecTimer();		// performs DeleteAll()!
	//  id fns.
	//!ic! NYD
	inline CSecID GetID() const;
	//!ic! NYD
	void SetID(CSecID id);
	// name:
	//!ic! NYD
	void SetName(const CString &sName);
	//!ic! NYD
	inline const CString &GetName() const;
	// lock:
	//!ic! NYD
	inline const CDWordArray &GetLockIDs() const;
	//!ic! NYD
	void AddLockID(CSecID lockID);
	//!ic! NYD
	void RemoveAllLockIDs();
	//!ic! NYD
	void AddTimeSpan(const CSecTimeSpan &span);
	//!ic! NYD
	void RemoveAllTimeSpans();
	void RemoveAllCombiIDs();
	void DropCombiTimer(CSecTimerArray &root, CSecID id);
	//  span fns.
	//!ic! NYD
	inline int GetSpanCount() const;	// the 'old' spans from the pointer array
	//!ic! NYD
	inline int GetTotalSpanCount() const;	// after the merge see below
	//!ic! NYD
	inline const CSecTimeSpanArray & GetTotalSpans() const;

	//  timer (combi) fns.
	//!ic! NYD
	inline int GetTimerCount() const;	// timers for combinations
	//!ic! NYD
	inline const CSecIDArray &GetPlusIDs() const;
	//!ic! NYD
	inline const CSecIDArray &GetMinusIDs() const;
	//!ic! NYD
	inline const CSecIDArray &GetRestrictIDs() const;
	//!ic! NYD
	void AddPlusID(CSecID timerID);
	//!ic! NYD
	void AddMinusID(CSecID timerID);
	//!ic! NYD
	void AddRestrictID(CSecID timerID);
	//!ic! NYD
	void RemovePlusID(CSecID timerID);
	//!ic! NYD
	void RemoveMinusID(CSecID timerID);
	//!ic! NYD
	void RemoveRestrictID(CSecID timerID);
	//
	// CAVEAT recursion....
	BOOL IsTimerUsed(CSecID id, const CSecTimerArray &root) const;


	// operations for/on the total spans
	//!ic! NYD
	void CalcTotalSpans(const CSecTimerArray &root);	// merge 'plain' spans and timers
	//!ic! NYD
	void UpdateNextModeChange(const CTime &t);
	//!ic! NYD
	inline const CTime & GetNextModeChange() const;
	//!ic! NYD
	BOOL IsIncluded(const CTime aTime) const;
	//!ic! NYD
	void UpdateSpans(const CTime &t);
	//!ic! NYD
	CString CreateRegistryString() const;
private:
	// basic data
	CSecID m_id;
	CString m_sName;
	CDWordArray m_lockIDs;
	// plain/normal/old timeSpans and TimerIds for combinations
	CSecTimeSpanArray m_spansPlain;
	
	CSecIDArray	m_timerIdsPlus;
	CSecIDArray	m_timerIdsMinus;
	CSecIDArray m_timerIdsRestrict;
	// same resolved to pointers in CalcTotalSpans
	//
	CSecTimerArray *m_pTimersPlus;
	CSecTimerArray *m_pTimersMinus;
	CSecTimerArray *m_pTimersRestrict;

	CTime m_nextModeChange;
	CSecTimeSpanArray m_spansTotal;
};

WK_PTR_ARRAY_CS(CSecTimerArrayPlain, CSecTimer*,CSecTimerArrayCS)

//  a collection of timers
class AFX_EXT_CLASS CSecTimerArray : public CSecTimerArrayCS
{
public:
	//  public
	//!ic! NYD
	const CSecTimer *GetTimerByID(CSecID id) const;
	const CSecTimer *GetTimerByName(const CString& sName) const;
	//!ic! NYD
	void UpdateTimers(const CTime &t);
	//!ic! NYD
	// registry:
	//!ic! NYD
	void Load(const CTime &ct,CWK_Profile& wkProfile, BOOL bKeepEmptyTimers=FALSE);	
	//!ic! NYD
	void Save(CWK_Profile& wkProfile);
	//!ic! NYD
	void CalcTotalSpans(const CSecTimerArray &root);
};

///////////////////////
// inlined functions //
///////////////////////
/*Function: */
inline CSecID CSecTimer::GetID() const
{
	return m_id;
}
// Function: NYD
inline const CString & CSecTimer::GetName() const
{
	return m_sName;
}

// Function: NYD
inline const CTime & CSecTimer::GetNextModeChange() const
{
	return m_nextModeChange;
}

// Function: NYD
inline const CDWordArray &CSecTimer::GetLockIDs() const
{
	return m_lockIDs;
}
// Function: NYD the 'old' spans from the pointer array
inline int CSecTimer::GetSpanCount() const
{
	return m_spansPlain.GetSize();
}
// Function: NYD timers for combinations
inline int CSecTimer::GetTimerCount() const
{
	return m_timerIdsPlus.GetSize() + m_timerIdsMinus.GetSize() + m_timerIdsRestrict.GetSize();
}
// Function: NYD after the merge
inline int CSecTimer::GetTotalSpanCount() const
{
	return m_spansTotal.GetSize();
}
// Function: NYD
inline const CSecTimeSpanArray & CSecTimer::GetTotalSpans() const
{
	return m_spansTotal;
}

// Function: NYD
inline const CSecIDArray &CSecTimer::GetPlusIDs() const
{
	return m_timerIdsPlus;
}
// Function: NYD
inline const CSecIDArray &CSecTimer::GetMinusIDs() const
{
	return m_timerIdsMinus;
}
// Function: NYD
inline const CSecIDArray &CSecTimer::GetRestrictIDs() const
{
	return m_timerIdsRestrict;
}


#endif
