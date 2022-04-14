/* GlobalReplace: WeekDayForEndTime --> DynamicPatternForEndTime */
/* GlobalReplace: m_bHasDynamicPatternForEndTime --> m_bHasDynamicEndTime */
/* GlobalReplace: HasWeekDayForStartTime --> HasDynamicStartTime */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecTimer.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/SecTimer.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 83 $
// LAST CHANGE:	$Modtime: 19.01.06 20:30 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"
#include "SecTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FAR_FUTURE CTime(2038,1,1,1,1,1)
#define T_MAX_DAYS 7
#define T_MAXOFF 4


/**/
/*{CSecTimer Overview|Overview,CSecTimer}
 {members|CSecTimer}, {CSecTimeSpan},
{WK_PTR_ARRAY}

CSecTimer is a pointer array of {CSecTimeSpan}s.
*/
/* 
 {overview|Overview,CSecTimer},
{CSecTimeSpan}
*/
////////////////////////
// full copy, no pointer copy
/*Function: */
CSecTimer::CSecTimer()
{
	m_nextModeChange= FAR_FUTURE;
	// m_timerIdsPlus
	// m_timerIdsMinus
	
	m_pTimersPlus = new CSecTimerArray;
	m_pTimersMinus = new CSecTimerArray;
	m_pTimersRestrict = new CSecTimerArray;
	// do not delete via these pointers
	m_pTimersPlus->SetAutoDelete(FALSE);
	m_pTimersMinus->SetAutoDelete(FALSE);
	m_pTimersRestrict->SetAutoDelete(FALSE);
}
/*Function:T{full copy} copy constructor, which performs a full copy of the
{CSecTimeSpan}s, NOT a pointer copy. For both the plain spans
and the merged ones
*/
CSecTimer::CSecTimer(const CSecTimer &src)
{
	int i=0;
	for (i=0;i<src.m_spansPlain.GetSize();i++) 
	{
		m_spansPlain.Add(new CSecTimeSpan(*src.m_spansPlain.GetAtFast(i)));
	}
	for (i=0;i<src.m_spansTotal.GetSize();i++) 
	{
		m_spansTotal.Add(new CSecTimeSpan(*src.m_spansTotal.GetAtFast(i)));
	}

	m_nextModeChange=src.m_nextModeChange;
	for (i=0;i<src.m_timerIdsPlus.GetSize();i++) 
	{
		m_timerIdsPlus.Add(src.m_timerIdsPlus.GetAtFast(i).GetID());
	}
	for (i=0;i<src.m_timerIdsMinus.GetSize();i++) 
	{
		m_timerIdsMinus.Add(src.m_timerIdsMinus.GetAtFast(i).GetID());
	}
	for (i=0;i<src.m_timerIdsRestrict.GetSize();i++)
	{
		m_timerIdsRestrict.Add(src.m_timerIdsRestrict.GetAtFast(i).GetID());
	}

	m_pTimersPlus->RemoveAll();
	m_pTimersMinus->RemoveAll();
	m_pTimersRestrict->RemoveAll();

	m_pTimersPlus->Append(*src.m_pTimersPlus);
	m_pTimersMinus->Append(*src.m_pTimersMinus);
	m_pTimersRestrict->Append(*src.m_pTimersRestrict);
}


/*Function:{performs DeleteAll()} performs DeleteAll()*/
CSecTimer::~CSecTimer()
{
	m_spansTotal.DeleteAll();
	m_spansPlain.DeleteAll();

	m_pTimersPlus->RemoveAll();
	m_pTimersMinus->RemoveAll();
	m_pTimersRestrict->RemoveAll();

	WK_DELETE(m_pTimersPlus);
	WK_DELETE(m_pTimersMinus);
	WK_DELETE(m_pTimersRestrict);
}

/*Function: */
void CSecTimer::SetID(CSecID id)
{
	m_id = id;
}
/*Function: */
void CSecTimer::SetName(const CString &sName)
{
	m_sName = sName;
	// exclude special chars
	m_sName.Remove(',');
	m_sName.Remove(':');
	m_sName.Remove('\\');
}

/*Function: */
void CSecTimer::AddLockID(CSecID lockID)
{
	m_lockIDs.Add(lockID.GetID());
}
/*Function: */
void CSecTimer::UpdateSpans(const CTime &t)
{
	for (int i=0;i<m_spansTotal.GetSize();i++) 
	{
		m_spansTotal.GetAtFast(i)->UpdateTimes(t);
	}
	UpdateNextModeChange(t);
}


void CSecTimer::UpdateNextModeChange(const CTime &ct)
{
	int i=0;
	m_nextModeChange = FAR_FUTURE;	// max value for time

	// OOPS overlaping start/ends
	// search for minimum, which is > than the given ct
	if (m_spansTotal.GetSize()) 
	{
		CTime tempModeChange = FAR_FUTURE;
		for (i=0;i<m_spansTotal.GetSize();i++) 
		{
			CSecTimeSpan &span = *m_spansTotal.GetAtFast(i);
			// GF new 03.12.2002
			// new method
			// will update the time span individually if necessary
			// and delivers the next mode change
			tempModeChange = span.GetNextModeChange(ct);
			if (tempModeChange < m_nextModeChange)
			{
				m_nextModeChange = tempModeChange;
			}
/*
			if (   s.GetStartTime() >= ct
				&& s.GetStartTime() < m_nextModeChange) {
					m_nextModeChange = s.GetStartTime();
			}
			if (   s.GetEndTime() >= ct
				&& s.GetEndTime() < m_nextModeChange) {
					// end event after the span
					m_nextModeChange = s.GetEndTime()+CTimeSpan(0,0,0,1);
			}
*/
		}
	}
	else
	{
		CSecTimer *pTimer=NULL;
		for (i=0;i<m_pTimersPlus->GetSize();i++)
		{
			pTimer = m_pTimersPlus->GetAtFast(i);
			// make sure the time is up to date (we are within a tree)
			pTimer->UpdateNextModeChange(ct);
			if ( pTimer->GetNextModeChange() < m_nextModeChange)
			{
					// end event after the span
					m_nextModeChange = 	pTimer->GetNextModeChange();
			}
		}	// end of loop over plus timers

		for (i=0;i<m_pTimersMinus->GetSize();i++)
		{
			pTimer = m_pTimersMinus->GetAtFast(i);
			// make sure the time is up to date (we are within a tree)
			pTimer->UpdateNextModeChange(ct);
			if ( pTimer->GetNextModeChange() < m_nextModeChange) {
					// end event after the span
					m_nextModeChange = 	pTimer->GetNextModeChange();
			}
		}	// end of loop over plus timers

		for (i=0;i<m_pTimersRestrict->GetSize();i++)
		{
			pTimer = m_pTimersRestrict->GetAtFast(i);
			// make sure the time is up to date (we are within a tree)
			pTimer->UpdateNextModeChange(ct);
			if ( pTimer->GetNextModeChange() < m_nextModeChange) {
					// end event after the span
					m_nextModeChange = 	pTimer->GetNextModeChange();
			}
		}	// end of loop over plus timers
	}	// end of combi

}
/*Function: */
BOOL CSecTimer::IsIncluded(const CTime aTime) const
{
	BOOL bIncluded=FALSE;
	int i=0;

	if (m_spansTotal.GetSize()) {
		// a) check the plain spans
		for (i=0;i<m_spansTotal.GetSize() && bIncluded==FALSE;i++) 
		{
			bIncluded = m_spansTotal.GetAtFast(i)->IsIncluded(aTime);
		}
	}
	else 
	{
		// b) check combi timers
		// first include
		for (i=0;i<m_pTimersPlus->GetSize() && bIncluded==FALSE;i++)
		{
			bIncluded = m_pTimersPlus->GetAtFast(i)->IsIncluded(aTime);
		}
		// WK_TRACE(_T("Combi check a) returned %d\n"),bIncluded);
		// then exclude with inverted logic
		for (i=0;i<m_pTimersMinus->GetSize() && bIncluded==TRUE;i++)
		{
			if (m_pTimersMinus->GetAtFast(i)->IsIncluded(aTime))
			{
				bIncluded = FALSE;
			}
		}
		// WK_TRACE(_T("Combi check b) returned %d\n"),bIncluded);
		// then restrictions, it's only included if it has passed the previous checks
		// AND is inside the restricted time
		if (bIncluded)
		{
			BOOL bIncludedInRestriction=FALSE;
			for (i=0;i<m_pTimersRestrict->GetSize() && bIncludedInRestriction==FALSE;i++)
			{
				if (m_pTimersRestrict->GetAtFast(i)->IsIncluded(aTime))
				{
					bIncludedInRestriction = TRUE;
				}
				if (bIncludedInRestriction) 
				{
					bIncluded = TRUE;
				} 
				else 
				{
					bIncluded = FALSE;
				}
			}
			// WK_TRACE(_T("Combi check c) returned %d\n"),bIncluded);
		}
	}

	return bIncluded;
}

/**/
/*{CSecTimerArray Overview|Overview,CSecTimerArray}
 {members|CSecTimerArray}, {CSecTimer}

*/
/* 
 {overview|Overview,CSecTimerArray},
{CSecTimer}
*/
const CSecTimer *CSecTimerArray::GetTimerByID(CSecID id) const
{
	const CSecTimer *pResult=NULL;
	CSecTimerArray *self=(CSecTimerArray *)this;	// this should be a const fn.

	self->Lock();	// caller should be responsible
	for (int i=0;i<GetSize() && pResult==NULL;i++)
	{
		if (GetAtFast(i)->GetID()==id)
		{
			pResult=GetAtFast(i);
		}
	}
	self->Unlock();

	return pResult;
}

const CSecTimer *CSecTimerArray::GetTimerByName(const CString &sName) const
{
	const CSecTimer *pResult=NULL;
	CSecTimerArray *self=(CSecTimerArray *)this;	// this should be a const fn.

	self->Lock();	// caller should be responsible
	for (int i=0;i<GetSize() && pResult==NULL;i++)
	{
		if (sName == GetAtFast(i)->GetName())
		{
			pResult=GetAtFast(i);
		}
	}
	self->Unlock();

	return pResult;
}

void CSecTimerArray::UpdateTimers( const CTime &t)
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->UpdateSpans(t);
	}
	Unlock();
}


/*Function: */
void CSecTimerArray::Load(const CTime &ct,CWK_Profile& wkProfile, BOOL bKeepEmptyTimers)	// deletes existing entries
{
	Lock();
	DeleteAll();	// delete extisting records

	CString sMax = wkProfile.GetString(SEC_NAME_TIMER,ENT_NAME_MAX_TIMER, _T("0"));
	int iMaxTimer = _ttoi(sMax);
	
	for (int iTimer=0 ; iTimer<iMaxTimer ; iTimer++) 
	{
		CString sTmp = wkProfile.GetString(SEC_NAME_TIMER,iTimer+1, _T(""));
		if (sTmp.GetLength()) 
		{
			CSecID newID = (DWORD)wkProfile.GetHexFromString(sTmp,  _T("\\ID"), SECID_NO_ID);
			if ( newID.IsTimerID() ) 
			{
				// OOPS not checked for doublettes
				CSecTimer* pTimer = new CSecTimer;
				pTimer->SetID(newID);
				
				CString sName = wkProfile.GetStringFromString(sTmp,  _T("\\CO"),  _T("unbenannt"));
				pTimer->SetName(sName);

				// read the lockIDs
				// OOPS limit
				for (UINT uLock=0 ; uLock<15 ; uLock++) {
					CString sAl;
					sAl.Format(_T("\\AL%u="), uLock+1);
					CSecID idInput = (DWORD)wkProfile.GetHexFromString(sTmp, sAl, SECID_NO_ID);
					if (idInput!=SECID_NO_ID) {
						pTimer->AddLockID(idInput);
					} else {
						// no error msg, since only active lock are in the registry
					}
				}	// end of loop over possible lock IDs

				// check for new style first

				// read old (pre 4.0) format
				for (UINT uDay=0 ; uDay<8 ; uDay++)			// 7 Tage + Test
				{
					for (UINT uOff=0 ; uOff<T_MAXOFF ; uOff++)  	// 4 mal off pro tag
					{
						CString sOT;
						sOT.Format(_T("\\OT%u%u="), uDay+1, uOff+1);	
						CString sTmpOT = wkProfile.GetStringFromString(sTmp,sOT, _T(""));
						if (sTmpOT.GetLength()) 
						{
							CString sSpan = sTmpOT;
							int ix = sSpan.Find('-');
							CString sStart = sSpan.Left(ix);
							CString sEnd= sSpan.Right(sSpan.GetLength()-ix-1);
							CString sDay;
							switch (uDay) 
							{
								case 0: sDay=_T("MO"); break;
								case 1: sDay=_T("DI"); break;
								case 2: sDay=_T("MI"); break;
								case 3: sDay=_T("DO"); break;
								case 4: sDay=_T("FR"); break;
								case 5: sDay=_T("SA"); break;
								case 6: sDay=_T("SO"); break;
								case 7: sDay=_T("DT"); break;
								default:
									WK_TRACE_ERROR(_T("Invalid day %d\n"), uDay);
									
							}
							sStart = sDay + _T(":") + sStart;
							sEnd = sDay + _T(":") + sEnd;
							// WK_TRACE(_T("TIME %s - %s\n"),sStart,sEnd);
							CSecTimeSpan *pSpan = new CSecTimeSpan(ct, sStart,sEnd);
							if (pSpan->IsValid()) 
							{
								pTimer->AddTimeSpan(*pSpan);	// does a copy
							} 
							else 
							{
								WK_TRACE_ERROR(_T("Invalid time span %s-%s\n"),sStart,sEnd);
							}
							WK_DELETE(pSpan);
						}
					}
				}	// end of loop over 7+1
				// new (4.0) format
				// how many spans do we have ?
				int iNumSpans = (int) wkProfile.GetNrFromString(sTmp,_T("\\NS="),0);
				for (int iSpan=0 ; iSpan<iNumSpans ; iSpan++) {	// loop over all spans
					CString sOT;
					sOT.Format(_T("\\TS%u="), iSpan);	
					CString sTmpOT = wkProfile.GetStringFromString(sTmp,sOT,_T(""));

					if (sTmpOT.GetLength())
					{
						CString sSpan = sTmpOT;
						CString sStart;
						CString sEnd;
						CString sDate;

						sSpan.TrimLeft();
						sSpan.TrimRight();

						int ix=0;
						ix = sSpan.Find('-');
						if (ix != -1)
						{
							sStart = sSpan.Left(ix);
							sEnd = sSpan.Mid(ix+1);
						}
						else
						{
							// invalid format
							WK_TRACE_ERROR(_T("Invalid day format in '%s'\n"),sSpan);
						}
					
						CSecTimeSpan *pSpan = new CSecTimeSpan(ct, sStart,sEnd);
						if (pSpan->IsValid()) {
							pTimer->AddTimeSpan(*pSpan);	// does a copy
						} else {
							WK_TRACE_ERROR(_T("Invalid time span %s-%s\n"),sStart,sEnd);
						}
						WK_DELETE(pSpan);
					} else {
						// empty string in registry silent ignore
					}
				}	// end of loop over 'new' spans'

				CString sTimerIDs;
				CStringArray saIDs;
				DWORD dwID=0;
				int iNumRead=0;
				
				
				sTimerIDs = wkProfile.GetStringFromString(sTmp,_T("\\IT"),_T(""));
				saIDs.RemoveAll();
				SplitString(sTimerIDs,saIDs,',');
				int iID = 0;
				for (iID=0 ; iID<saIDs.GetSize() ; iID++) 
				{
					CString sOne = saIDs.GetAt(iID);
					iNumRead = _stscanf(sOne,_T("%lx"),&dwID);
					if (iNumRead==1) 
					{
						pTimer->AddPlusID(dwID);
					} 
					else 
					{
						// could not read hex number
					}
				}

				sTimerIDs = wkProfile.GetStringFromString(sTmp,_T("\\ET"),_T(""));
				saIDs.RemoveAll();
				SplitString(sTimerIDs,saIDs,',');
				for (iID=0 ; iID<saIDs.GetSize() ; iID++) 
				{
					CString sOne = saIDs.GetAt(iID);
					iNumRead = _stscanf(sOne,_T("%lx"),&dwID);
						if (iNumRead==1) 
						{
							pTimer->AddMinusID(dwID);
						}
						else 
						{
							// could not read hex number
						}
				}

				sTimerIDs = wkProfile.GetStringFromString(sTmp,_T("\\RT"),_T(""));
				saIDs.RemoveAll();
				SplitString(sTimerIDs,saIDs,',');
				for (iID=0 ; iID<saIDs.GetSize() ; iID++) 
				{
					CString sOne = saIDs.GetAt(iID);
					iNumRead = _stscanf(sOne,_T("%lx"),&dwID);
					if (iNumRead==1) 
					{
						pTimer->AddRestrictID(dwID);
					} 
					else 
					{
						// could not read hex number
					}
				}

				if (pTimer->GetSpanCount() 
					|| pTimer->GetTimerCount() 
					|| bKeepEmptyTimers) {
					pTimer->UpdateNextModeChange(ct);
					Add(pTimer);
				} 
				else 
				{
					WK_TRACE(_T("Empty timer %s deleted\n"), LPCTSTR(pTimer->GetName()));
					WK_DELETE(pTimer);
				}
			} 
			else
			{
				// no timer ID
			}
		} 
		else 
		{
			WK_TRACE(_T("Empty timer entry %d\n"),iTimer+1);
		}

	}	// end of loop over maxTimer

	CalcTotalSpans(*this);
	Unlock();
}


/*Function: NYD */
CString CSecTimer::CreateRegistryString() const
{
	int i=0;
	CString sResult;
	sResult.Format(_T("\\ID%x\\CO%s"),
			GetID(), 
			LPCTSTR(GetName())	// NOT YET check for \ or @
			);

	for (i=0;i<m_lockIDs.GetSize();i++)
	{
		CSecID id = m_lockIDs[i];
		if ( id!=SECID_NO_ID && id.IsInputID() )
		{
			CString s1;
			s1.Format( _T("\\AL%u=%x"), i+1, id.GetID() );	
			sResult += s1;
		}
	}

	// (4.0) format
	if (GetTimerCount()==0) {	// plain spans
		CString sNum;
		sNum.Format(_T("\\NS=%d"),m_spansPlain.GetSize());
		sResult += sNum;
		for (i=0;i<m_spansPlain.GetSize();i++)
		{
			const CSecTimeSpan & oneSpan = * m_spansPlain.GetAtFast(i);

			CString sSpan;
			sSpan.Format(_T("\\TS%d=%s-%s"),
				i,
				LPCTSTR(oneSpan.GetStartString()),
				LPCTSTR(oneSpan.GetEndString())
				);
			sResult += sSpan;
		}
	} else {	// combi timer
		if (m_timerIdsPlus.GetSize()) 
		{
			sResult += _T("\\IT");	// include timers
			sResult += m_timerIdsPlus.CreateString();
		}
		if (m_timerIdsMinus.GetSize()) 
		{
			sResult += _T("\\ET");	// exclude timers
			sResult += m_timerIdsMinus.CreateString();
		}
		if (m_timerIdsRestrict.GetSize())
		{
			sResult += _T("\\RT");	// restriction timers
			sResult += m_timerIdsRestrict.CreateString();
		}
	}

	return sResult;
}

void CSecTimerArray::Save(CWK_Profile& wkProfile)
{
	wkProfile.DeleteSection(SEC_NAME_TIMER);
	CString sMax;

	sMax.Format(_T("%d"),GetSize());
	wkProfile.WriteString(SEC_NAME_TIMER,ENT_NAME_MAX_TIMER,sMax);

	for (int i=0;i<GetSize();i++) 
	{
		const CSecTimer &oneTimer = * GetAtFast(i);
		CString sTimer = oneTimer.CreateRegistryString();
		wkProfile.WriteStringIndex(SEC_NAME_TIMER,i+1,sTimer);
	}
}

	
void CSecTimer::AddTimeSpan(const CSecTimeSpan &span)
{
	// create a copy
	CSecTimeSpan *pSpan = new CSecTimeSpan(span);
	m_spansPlain.Add( pSpan );
}



void CSecTimer::CalcTotalSpans(const CSecTimerArray &root)	// merge 'plain' spans and timers
{
	m_spansTotal.DeleteAll();
	for (int i=0;i<m_spansPlain.GetSize();i++)
	{
		m_spansTotal.Add(new CSecTimeSpan( * m_spansPlain.GetAtFast(i)));
	}

	// resolve ids to pointers
	m_pTimersPlus->RemoveAll();
	m_pTimersMinus->RemoveAll();
	m_pTimersRestrict->RemoveAll();

	const CSecTimer *pTimer = NULL;
	CSecID id;
	for (i=0;i<m_timerIdsPlus.GetSize();i++) {
		id = m_timerIdsPlus[i];
		pTimer = root.GetTimerByID(id);
		if (pTimer) {
			m_pTimersPlus->Add((CSecTimer *)pTimer);	// OOPS deconst
		} else {
			WK_TRACE_WARNING(_T("Could not find timer for id %08x\n"),id.GetID());
		}
	}
	for (i=0;i<m_timerIdsMinus.GetSize();i++) {
		id = m_timerIdsMinus[i];
		pTimer = root.GetTimerByID(id);
		if (pTimer) {
			m_pTimersMinus->Add((CSecTimer *)pTimer);	// OOPS deconst
		} else {
			WK_TRACE_WARNING(_T("Could not find timer for id %08x\n"),id.GetID());
		}
	}
	for (i=0;i<m_timerIdsRestrict.GetSize();i++) {
		id = m_timerIdsRestrict[i];
		pTimer = root.GetTimerByID(id);
		if (pTimer) {
			m_pTimersRestrict->Add((CSecTimer *)pTimer);	// OOPS deconst
		} else {
			WK_TRACE_WARNING(_T("Could not find timer for id %08x\n"),id.GetID());
		}
	}
}

void CSecTimerArray::CalcTotalSpans(const CSecTimerArray &root)	// merge 'plain' spans and timers
{
	// loop over all elements, each elemnt will distinguish between plain and combis
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->CalcTotalSpans(root);
	}
	Unlock();
}

void CSecTimer::RemoveAllLockIDs()
{
	m_lockIDs.RemoveAll();
}


void CSecTimer::RemoveAllTimeSpans()
{
	m_spansPlain.DeleteAll();
	m_spansTotal.DeleteAll();
}

void CSecTimer::RemoveAllCombiIDs()
{
	m_timerIdsPlus.RemoveAll();
	m_timerIdsMinus.RemoveAll();
	m_timerIdsRestrict.RemoveAll();
}


// Function: NYD
void CSecTimer::AddPlusID(CSecID timerID)
{
	if (m_timerIdsPlus.Has(timerID)) 
	{
		WK_TRACE_ERROR(_T("SecTimer[08x,%s]:already have %08x in plusIDs\n"),GetID().GetID(),LPCTSTR(GetName()),timerID.GetID());
	}
	else
	{
		m_timerIdsPlus.Add(timerID.GetID());
	}
}

// Function: NYD
void CSecTimer::AddMinusID(CSecID timerID)
{
	if (m_timerIdsMinus.Has(timerID))
	{
		WK_TRACE_ERROR(_T("SecTimer[08x,%s]:already have %08x in minusIDs\n"),GetID().GetID(),LPCTSTR(GetName()),timerID.GetID());
	} 
	else 
	{
		m_timerIdsMinus.Add(timerID.GetID());
	}
}

// Function: NYD
void CSecTimer::AddRestrictID(CSecID timerID)
{
	if (m_timerIdsRestrict.Has(timerID))
	{
		WK_TRACE_ERROR(_T("SecTimer[08x,%s]:already have %08x in restrictIDs\n"),GetID().GetID(),LPCTSTR(GetName()),timerID.GetID());
	}
	else
	{
		m_timerIdsRestrict.Add(timerID.GetID());
	}
}

// Function: NYD
void CSecTimer::RemovePlusID(CSecID timerID)
{
	// UNUSED BOOL bRemoved = 
	m_timerIdsPlus.Remove(timerID);
}

// Function: NYD
void CSecTimer::RemoveMinusID(CSecID timerID)
{
	// UNUSED BOOL bRemoved = 
	m_timerIdsMinus.Remove(timerID);
}

// Function: NYD
void CSecTimer::RemoveRestrictID(CSecID timerID)
{
	// UNUSED BOOL bRemoved = 
	m_timerIdsRestrict.Remove(timerID);
}


BOOL CSecTimer::IsTimerUsed(CSecID id, const CSecTimerArray &root) const
{
	BOOL bUsed=FALSE;
	const CSecTimer *pTimer = NULL;

	if (id==m_id) {	// check for own id
		bUsed=TRUE;
	} else {
		if (GetTimerCount()) {
			// first level check, search and compare ids only
			int i=0;
			for (i=0;i<m_timerIdsPlus.GetSize() && bUsed==FALSE;i++) {
				if (id==m_timerIdsPlus[i]) {
					bUsed=TRUE;
				}
			}
			for (i=0;i<m_timerIdsMinus.GetSize() && bUsed==FALSE;i++) {
				if (id==m_timerIdsMinus[i]) {
					bUsed=TRUE;
				}
			}
			for (i=0;i<m_timerIdsRestrict.GetSize() && bUsed==FALSE;i++) {
				if (id==m_timerIdsRestrict[i]) {
					bUsed=TRUE;
				}
			}

			// second level check, resolve pointers and check these (recursion)
			for (i=0;i<m_timerIdsPlus.GetSize() && bUsed==FALSE;i++) {
				pTimer = root.GetTimerByID(m_timerIdsPlus[i]);
				if (pTimer) {
					bUsed=pTimer->IsTimerUsed(id,root);
				}
			}
			for (i=0;i<m_timerIdsMinus.GetSize() && bUsed==FALSE;i++) {
				pTimer = root.GetTimerByID(m_timerIdsMinus[i]);
				if (pTimer) {
					bUsed=pTimer->IsTimerUsed(id,root);
				}			}
			for (i=0;i<m_timerIdsRestrict.GetSize() && bUsed==FALSE;i++) {
				pTimer = root.GetTimerByID(m_timerIdsRestrict[i]);
				if (pTimer) {
					bUsed=pTimer->IsTimerUsed(id,root);
				}			}
		} else {
			// plain spans
		}
	}
	return bUsed;
}

void CSecTimer::DropCombiTimer(CSecTimerArray &root, CSecID id)
{
	if (GetTimerCount()) {
		// first level check, search and compare ids only
		int i=0;
		for (i=0;i<m_timerIdsPlus.GetSize();i++) {
			if (id==m_timerIdsPlus[i]) {
				m_timerIdsPlus.RemoveAt(i);
				i--;
			}
		}
		for (i=0;i<m_timerIdsMinus.GetSize();i++) {
			if (id==m_timerIdsMinus[i]) {
				m_timerIdsMinus.RemoveAt(i);
				i--;
			}
		}
		for (i=0;i<m_timerIdsRestrict.GetSize();i++) {
			if (id==m_timerIdsRestrict[i]) {
				m_timerIdsRestrict.RemoveAt(i);
				i--;
			}
		}
		CalcTotalSpans(root);
	} else {
		// plain spans
	}
}
