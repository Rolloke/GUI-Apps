// DocAscom.h: interface for the CDocAscom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCASCOM_H__09F35433_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_DOCASCOM_H__09F35433_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "DocIBM.h"

/////////////////////////////////////////////////////////////////////////////
class CDocAscom : public CDocIbm  
{
	DECLARE_DYNCREATE(CDocAscom)
public:
	CDocAscom();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCASCOM_H__09F35433_5F91_11D3_BA75_00400531137E__INCLUDED_)
