// Sequences.cpp: implementation of the CSequences class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sequences.h"

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
// ExchangeSort Code to
// accelerate sorting
void CSequences::Sort(BOOL& bCancel)
{
	int iCur, iMin, iNext, nCount;
	CTape* swap;
	
	Lock();
	
	nCount = GetSize();
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
