// ViewAscom.h: interface for the CViewAscom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWASCOM_H__09F35434_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_VIEWASCOM_H__09F35434_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "ViewIbm.h"
#include "DocAscom.h"

class CViewAscom : public CViewIbm
{
public:
	CViewAscom();
	DECLARE_DYNCREATE(CViewAscom)

// Attributes
public:
	CDocAscom* GetDocument();

};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in ViewAscom.cpp
inline CDocAscom* CViewAscom::GetDocument()
   { return (CDocAscom*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWASCOM_H__09F35434_5F91_11D3_BA75_00400531137E__INCLUDED_)
