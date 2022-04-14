/* GlobalReplace: CSDITestSniIbmEmuDoc --> CDocSniIbmEmulation */
/* GlobalReplace: CSDITestIbmDoc --> CDocIbm */
// SDITestSniIbmEmuDoc.h: interface for the CDocSniIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDITESTSNIIBMEMUDOC_H__504559B3_CAF0_11D2_B07D_004005A1D890__INCLUDED_)
#define AFX_SDITESTSNIIBMEMUDOC_H__504559B3_CAF0_11D2_B07D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DocIbm.h"

class CDocSniIbmEmulation : public CDocIbm  
{
	DECLARE_DYNCREATE(CDocSniIbmEmulation)

public:
	CDocSniIbmEmulation();
	virtual ~CDocSniIbmEmulation();

	virtual	BOOL	OpenCom();
};

#endif // !defined(AFX_SDITESTSNIIBMEMUDOC_H__504559B3_CAF0_11D2_B07D_004005A1D890__INCLUDED_)
