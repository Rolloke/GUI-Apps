/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: timerlist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/timerlist.cpp $
// CHECKIN:		$Date: 27.06.03 15:53 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 17.06.03 15:33 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "util.h"
#include "timerlist.h"
#include "WK_Profile.h"
#include "globals.h"


/////////////////////////////////////////////////////////////////////////////
CTimer::CTimer()
{
	//	m_ID;
	//  m_sName;
	memset(&Day,0,sizeof(Day));
}

/////////////////////////////////////////////////////////////////////////////
CTimer::CTimer(CTimer* pTimer)
{
	m_ID	= pTimer->GetID();
	m_sName	= pTimer->GetName();
	for ( int i=0 ; i<T_MAX_INP ; i++ )
	{
		idInput[i]	= pTimer->idInput[i]; 
	}
}

/////////////////////////////////////////////////////////////////////////////
CTimerList::CTimerList()
{
}

/////////////////////////////////////////////////////////////////////////////
CTimerList::~CTimerList()
{
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
CTimer*	CTimerList::GetTimerByID(CSecID id) const
{
	int iCount = GetSize();
	for (int i=0 ; i<iCount ; i++)
	{
		CTimer* p = GetAt(i);
		if (p && p->GetID()==id)
		{
			return p;	// EXIT
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerList::AddTimer(CTimer* pTimer)
{
	ASSERT( pTimer->GetID() == SECID_NO_ID );
	pTimer->SetID( GetFreeTimerID() );
	Add(pTimer);
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::DeleteTimerByID(CSecID id)
{
	int iCount = GetSize();
	CTimer* p = NULL;
	for (int i=0 ; i<iCount ; i++)
	{ 
		p = GetAt(i);
		if (p && p->GetID()==id)
		{
			WK_DELETE(p);
			RemoveAt(i);
			break;
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
CSecID CTimerList::GetMaxTimerID() const
{
	CSecID maxID(SECID_GROUP_TIMER,0);
	CSecID id;
	int iCount = GetSize();
	for (int i=0 ; i<iCount ; i++)
	{
		CTimer* p = GetAt(i);
		id = p->GetID();
		if ( p && id!=SECID_NO_ID && id.GetID()>maxID.GetID() )
		{
			maxID = id;
		}
	}
	return maxID;
}

/////////////////////////////////////////////////////////////////////////////
CSecID CTimerList::GetFreeTimerID() const
{
	CSecID NewID(SECID_GROUP_TIMER,1);
	BOOL bFound = TRUE;
	int iCount = GetSize();
	while ( bFound )
	{
		bFound = FALSE;
		for (int i=0 ; i<iCount ; i++)
		{
			CTimer* pTimer = GetAt(i);
			if ( pTimer)
			{
				if ( NewID == pTimer->GetID() )
				{
					bFound = TRUE;
					NewID = NewID.GetID() + 1;
					ASSERT( NewID.IsTimerID() );
					break;
				}
			}
		}
	}
	return NewID;
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::Load(CWK_Profile& wkp)
{
	Reset();
	CString sMax = wkp.GetString(SEC_NAME_TIMER, ENT_NAME_MAX_TIMER, _T("0") );
	int iMaxTimer = atoi(sMax);
	for (int i=0;i<iMaxTimer;i++)	
	{					
		CString sTmp = wkp.GetString(SEC_NAME_TIMER, i+1, _T(""));
		if (sTmp.GetLength()) 
		{
			CSecID newID = (DWORD)wkp.GetHexFromString(sTmp, _T("\\ID"), SECID_NO_ID) ;
			if ( newID.IsTimerID() )
			{
				// OOPS not checked for doublettes
				CTimer* pTimer = new CTimer();
				if ( pTimer )
				{
					pTimer->SetID(newID);
					CString sName = wkp.GetStringFromString(sTmp, _T("\\CO"), _T("unbenannt"));
					pTimer->SetName(sName);
					for (UINT j=0;j<T_MAX_INP;j++)
					{
						CString sAl;
						sAl.Format(_T("\\AL%u="), j+1);
						CSecID idInput = (DWORD)wkp.GetHexFromString(sTmp, sAl, SECID_NO_ID);
						pTimer->idInput[j] = idInput; 
					}
					for (j=0;j<T_MAX_DAYS;j++)			// 7 Tage
					{
						for (UINT n=0;n<T_MAXOFF;n++)  	// 4 mal off pro tag
						{
							CString sSub;
							sSub.Format(_T("\\OT%u%u="), j+1, n+1);	
							CString sTimes = wkp.GetStringFromString(sTmp, sSub, _T(""));
							if (sTimes.GetLength())
							{
								::String2Tnr(sTimes, pTimer->Day[j].b[n].wOff, pTimer->Day[j].b[n].wOn, n);
							}
							else
							{
								pTimer->Day[j].b[n].wOn  = 0;
								pTimer->Day[j].b[n].wOff = 0;
							}
						}
					}
					Add(pTimer);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::Save(CWK_Profile& wkp)
{
	ClearTimers(wkp);
	int iCount = GetSize();
	for (int i=0 ; i<iCount ; i++)
	{
		CTimer* pT = GetAt(i);
		if (pT)
		{
			SaveTimer(i+1, pT,wkp);
		}
	}
	//
	CString sTmp;	// OOPS still a string
	sTmp.Format(_T("%d"),iCount);
	wkp.WriteString(SEC_NAME_TIMER, ENT_NAME_MAX_TIMER, sTmp);
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::ClearTimers(CWK_Profile& wkp)
{
	CString sTmp = wkp.GetString(SEC_NAME_TIMER, ENT_NAME_MAX_TIMER,_T("0"));
	int iMaxTimer = atoi(sTmp);
	for (int i=1;i<=iMaxTimer;i++)
	{
		wkp.WriteStringIndex(SEC_NAME_TIMER,i,NULL);
	} 
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::SaveTimer(int iEntry, CTimer* pT,CWK_Profile& wkp)
{
	CString s, s1;
	s.Format( _T("\\ID%x\\CO%s"), pT->GetID().GetID(), pT->GetName() );
	for (int i=0;i<T_MAX_INP;i++)
	{
		CSecID id = pT->idInput[i];
		if ( id!=SECID_NO_ID && id.IsInputID() )
		{
			s1.Format( _T("\\AL%u=%x"), i+1, (pT->idInput[i]).GetID() );	
			s += s1;
		}
	}
	for (i=0;i<T_MAX_DAYS;i++)					// 7 Tage
	{
		for (int n=0;n<T_MAXOFF;n++)  	// 4 mal off pro tag
		{
			if (pT->Day[i].b[n].wOff && pT->Day[i].b[n].wOn)
			{
				char s2[1000];	// NOT YET improved Tnr2String
				s1.Format( _T("\\OT%u%u=%s"), i+1, n+1, ::Tnr2String(s2, pT->Day[i].b[n].wOff, pT->Day[i].b[n].wOn) );	
				s += s1;
			}
			else {
				// kein break, damit auch hoehere Auszeiten gespeichert werden
//				break;
			}
		}
	}
	wkp.WriteStringIndex(SEC_NAME_TIMER, iEntry,s); 
}

/////////////////////////////////////////////////////////////////////////////
// iNr:		Die Nummer des Timers [1...Max Timer.]
void CTimerList::DeleteTimer(int iIndex)
{
	if (iIndex>=0 && iIndex<GetSize())
	{ 
		delete GetAt(iIndex);
		RemoveAt(iIndex);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CTimerList::Reset()
{
	DeleteAll();
}

