// RelayGroup.cpp: implementation of the CRelayGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RelayGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelayGroup::CRelayGroup(LPCTSTR shmName, int wGroup, int size)
  : COutputGroup(shmName,wGroup)
{
	Lock();
	SetSize(size);
	for (WORD s=0;s<size;s++)
	{
		SetAt(s,new CRelay(this,CSecID(GetID(),s)));
	}
	Unlock();
}

CRelayGroup::~CRelayGroup()
{
	SafeDeleteAll();
}
