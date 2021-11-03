// IpFrame.h : Schnittstelle der Klasse CInPlaceFrame
//

#if !defined(AFX_IPFRAME_H__A8674190_2C1D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_IPFRAME_H__A8674190_2C1D_11D4_B6EC_0000B458D891__INCLUDED_

#include "stdafx.h"
#ifdef ETS_OLE_SERVER

#include "..\CaraToolBar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInPlaceFrame : public COleIPFrameWnd
{
	DECLARE_DYNCREATE(CInPlaceFrame)
public:
	CInPlaceFrame();

// Attribute
public:
	LRESULT OnExitSizeMove(WPARAM, LPARAM);
	LRESULT OnEnterSizeMove(WPARAM, LPARAM);

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CInPlaceFrame)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CInPlaceFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CCaraToolbar   m_wndToolBar;
protected:
	COleDropTarget m_dropTarget;
	COleResizeBar  m_wndResizeBar;

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CInPlaceFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
#endif // ETS_OLE_SERVER

#endif // !defined(AFX_IPFRAME_H__A8674190_2C1D_11D4_B6EC_0000B458D891__INCLUDED_)
