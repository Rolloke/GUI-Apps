// Sequences.cpp: implementation of the CSequences class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sequences.h"

// #define _USE_Q_SORT 1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSequences::CSequences()
{

}
//////////////////////////////////////////////////////////////////////
CSequences::~CSequences()
{
	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
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
	int iCur, iMin, iNext, nCount;
	CTape* swap;
	
	Lock();
	nCount = GetSize();

#ifdef _USE_Q_SORT
	// qsort is on average 1.34 times faster than exchange sort
	void**pData = GetData();
	qsort(pData, nCount, sizeof(CSequencePtr), CompareSequenceUp);

#else // use exchange sort

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
	
	Unlock();
}
//////////////////////////////////////////////////////////////////////////////
int CSequences::GetNrTapes()
{
	Lock();
	int r = GetSize();
	Unlock();
	return r;
}
