// RelayGroup.h: interface for the CRelayGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELAYGROUP_H__4B70B0C5_092F_11D4_A0CD_004005A19028__INCLUDED_)
#define AFX_RELAYGROUP_H__4B70B0C5_092F_11D4_A0CD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OutputGroup.h"
#include "Relay.h"

class CRelayGroup : public COutputGroup  
{
public:
	CRelayGroup(LPCTSTR shmName, int wGroup, int size);
	virtual ~CRelayGroup();

};

#endif // !defined(AFX_RELAYGROUP_H__4B70B0C5_092F_11D4_A0CD_004005A19028__INCLUDED_)
