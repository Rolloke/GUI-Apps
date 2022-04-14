// Relay.h: interface for the CRelay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELAY_H__4B70B0C4_092F_11D4_A0CD_004005A19028__INCLUDED_)
#define AFX_RELAY_H__4B70B0C4_092F_11D4_A0CD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Output.h"

class CRelay : public COutput  
{
	// construction / destruction
public:
	CRelay(COutputGroup* pGroup, CSecID id);
	virtual ~CRelay();

	// attributes
public:
	virtual CString Format();
};

#endif // !defined(AFX_RELAY_H__4B70B0C4_092F_11D4_A0CD_004005A19028__INCLUDED_)
