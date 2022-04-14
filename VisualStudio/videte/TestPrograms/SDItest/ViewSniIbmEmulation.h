/* GlobalReplace: CViewSniibmEmulation --> CViewIbmEmulation */
/* GlobalReplace: CSDITestSniIbmEmuView --> CViewIbmEmulation */
/* GlobalReplace: CSDITestIbmView --> CViewIbm */
// SDITestSniIbmEmuView.h: interface for the CViewIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDITESTSNIIBMEMUVIEW_H__504559B4_CAF0_11D2_B07D_004005A1D890__INCLUDED_)
#define AFX_SDITESTSNIIBMEMUVIEW_H__504559B4_CAF0_11D2_B07D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViewIbm.h"

class CViewIbmEmulation : public CViewIbm
{
	DECLARE_DYNCREATE(CViewIbmEmulation)

public:
	CViewIbmEmulation();
	virtual ~CViewIbmEmulation();

};

#endif // !defined(AFX_SDITESTSNIIBMEMUVIEW_H__504559B4_CAF0_11D2_B07D_004005A1D890__INCLUDED_)
