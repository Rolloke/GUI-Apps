/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
/* GlobalReplace: line %s --> line %d */
// Sequences.cpp: implementation of the CSequences class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Sequences.h"
#include "WK_Timer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// #define _USE_TIMER 1

#define _USE_Q_SORT 1


#ifdef _USE_TIMER
CWK_Timer theTimer;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSequences::CSequences()
{
	m_wID = 0;
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.TraceLocks() ? 0 : -1;
#endif
}
//////////////////////////////////////////////////////////////////////
CSequences::~CSequences()
{
}
//////////////////////////////////////////////////////////////////////////////
CSequence* CSequences::GetSequenceIndex(int i)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	CSequence* pSequence = NULL;
	if (i<GetSize() && i>=0)
	{
		pSequence = GetAtFast(i);
	}
	acs.Unlock();
	return pSequence;
}
//////////////////////////////////////////////////////////////////////
CSequence* CSequences::GetSequenceID(WORD wID)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			return GetAtFast(i);
		}
	}
	acs.Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CSequence* CSequences::GetFirstSequence(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (0 == GetAtFast(i)->GetDrive().CompareNoCase(pDrive->GetRootString()))
		{
			return GetAtFast(i);
			break;
		}
	}
	acs.Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CSequences::GetSizeBytes()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 r = 0;
	CIPCInt64 s = 0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		s = GetAtFast(i)->GetSizeBytes();
/*		TRACE(_T("seq nr %04x %04x = %I64d bytes\n"),
			GetAtFast(i)->GetArchivNr(),
			GetAtFast(i)->GetID(),
			s.GetInt64());*/
		r += s;
	}

	acs.Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
DWORD CSequences::GetSizeMB()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	return GetSizeBytes().GetInMB();
}
//////////////////////////////////////////////////////////////////////
DWORD CSequences::GetNumberOfPictures()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD n = 0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		n += GetAtFast(i)->GetNrOfPictures();
	}

	acs.Unlock();

	return n;
}
//////////////////////////////////////////////////////////////////////
void CSequences::AddScannedSequence(CSequence* pSequence)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	Add(pSequence);
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
WORD CSequences::GetNewID()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence=NULL;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	do
	{
		m_wID++;
		pSequence = GetSequenceID(m_wID);
	}
	while (pSequence!=NULL);

	acs.Unlock();

	return m_wID;
}
//////////////////////////////////////////////////////////////////////
CSequence* CSequences::AddNewSequence(BOOL bSuspect, CArchiv* pArchiv, CIPCDrive* pDrive,CSequence* pSourceSequence/*=NULL*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence=NULL;
	WORD wID = 0;

	wID = GetNewID();
	pSequence = new CSequence((BYTE)(bSuspect ? CAR_IS_SUSPECT : 0),pArchiv,pDrive,wID);
	
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	if (pSourceSequence)
	{
		// insert sorted!
		BOOL bInserted = FALSE;
		pSequence->m_stFirst = pSourceSequence->m_stFirst;
		pSequence->m_stLast = pSourceSequence->m_stLast;
		for (int i=0;i<GetSize();i++)
		{
			CSequence* pS = GetAtFast(i);
			if (   pS
				&& pS->m_stLast<=pSequence->m_stFirst)
			{
				InsertAt(i+1,pSequence);
				bInserted = TRUE;
				break;
			}
		}
		if (!bInserted)
		{
			InsertAt(0,pSequence);
		}
	}
	else
	{
		Add(pSequence);
	}

	acs.Unlock();

	return pSequence;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequences::DeleteOldestSequence()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	CSequence* pSequence = NULL;

	if (GetSize()>0)
	{
		pSequence = GetAtFast(0);
		RemoveAt(0);
	}

	acs.Unlock();
	
	if (pSequence)
	{
		bRet = pSequence->Delete();
		if (bRet)
		{
			theApp.m_Clients.DoIndicateDeleteSequence(*pSequence);
		}
		WK_DELETE(pSequence);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequences::Trace()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence = NULL;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (int i=0;i<GetSize();i++)
	{
		pSequence = GetAtFast(i);
		pSequence->Trace();
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequences::Finalize(BOOL bForceClose, DWORD dwMaxStorageTime)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence = NULL;
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	// erlaubte Zeit berechnen
	CSystemTime allowedTime;
	allowedTime.GetLocalTime();
	allowedTime.DecrementTime(0,dwMaxStorageTime,0,0,0);

	for (int i=GetSize()-1;i>=0;i--)
	{
		pSequence = GetAtFast(i);
		if (pSequence->IsAlarm())
		{
			bRet &= pSequence->Flush();
		}
		bRet &= pSequence->Close(bForceClose);

		// ist eine Max Speicherzeit eingestellt und
		// hat niemand diese Sequence offen ?
		if (   (dwMaxStorageTime>0)
			&& !pSequence->IsOpenByClient(0)
			&& (pSequence->GetLastTime() < allowedTime)
			)
		{
			DeleteSequence(pSequence->GetNr());
		}
	}

	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
// ExchangeSort Code to
// accelerate sorting

#ifdef _USE_Q_SORT
int __cdecl CompareSequenceUp(const void *p1, const void *p2) 
{
	CSequence* s1 = (CSequence*)((UINT_PTR*)p1)[0];
	CSequence* s2 = (CSequence*)((UINT_PTR*)p2)[0];
	if (s1->GetFirstTime() < s2->GetFirstTime()) return  -1;
	else	return 1;
//	if (s1->GetFirstTime() > s2->GetFirstTime()) return   1;
//	return 0;
}
#endif // _USE_Q_SORT
void CSequences::Sort(BOOL& bCancel)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	int nCount = GetSize();

#ifdef _USE_TIMER
	LARGE_INTEGER li1 = theTimer.GetMicroTicks();
#endif // _USE_TIMER

#ifdef _USE_Q_SORT
	// qsort is on average 1.34 times faster than exchange sort
	void**pData = GetData();
	qsort(pData, nCount, sizeof(CSequencePtr), CompareSequenceUp);

#else // use exchange sort

	int iCur, iMin, iNext;
	CSequence* swap;

	for (iCur=0; iCur<nCount && !bCancel; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount && !bCancel; iNext++)
		{
			if (*GetAtFast(iNext) < *GetAtFast(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = GetAtFast(iCur);
			SetAt(iCur,GetAtFast(iMin));
			SetAt(iMin,swap);
		}
	}

#endif // !_USE_Q_SORT

#ifdef _USE_TIMER
	LARGE_INTEGER li2 = theTimer.GetMicroTicks();
	li2.QuadPart = li2.QuadPart - li1.QuadPart;
	TRACE(_T("time to sort %d µs for %d entries\n"), li2.LowPart, nCount);

// check whethter it is really sorted
//	for (int i=0;i<GetSize();i++)
//	{
//		CSequence *ps = GetAtFast(i);
//		TRACE(_T("%s\n"), ps->GetFirstTime().ToString());
//	}
#endif // _USE_TIMER
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequences::DeleteSequence(WORD wID)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;
	CSequence* pSequence = NULL;
	BOOL bRet = FALSE;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			pSequence = GetAtFast(i);
			RemoveAt(i);
			break;
		}
	}
	acs.Unlock();

	if (pSequence)
	{
		if (pSequence->Delete())
		{
			theApp.m_Clients.DoIndicateDeleteSequence(*pSequence);
			bRet = TRUE;
		}
		WK_DELETE(pSequence);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequences::RemoveSequence(WORD wID)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;
	CSequence* pSequence = NULL;
	BOOL bRet = FALSE;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			pSequence = GetAtFast(i);
			RemoveAt(i);
			bRet = TRUE;
			break;
		}
	}
	acs.Unlock();

	return bRet;
}
