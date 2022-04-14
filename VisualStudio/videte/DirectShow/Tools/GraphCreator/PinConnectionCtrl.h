#pragma once

#include "BaseCtrl.h"

// CPinConnectionCtrl
class CFilterCtrl;
class CPinConnectionCtrl : public CBaseCtrl
{
	friend class CGraphCreatorView;
	DECLARE_DYNCREATE(CPinConnectionCtrl)

private:
	CPinConnectionCtrl();
public:
	CPinConnectionCtrl(CFilterCtrl*pF1, int nPin1, CFilterCtrl*pF2, int nPin2, AM_MEDIA_TYPE*pMT=NULL);
public:
	virtual ~CPinConnectionCtrl();

// Attributes
public:
	IPin*			GetOutPin()      { return m_pOutPin; };
	IPin*			GetInPin()       { return m_pInPin; };
	CFilterCtrl*	GetPrevFilterCrtl();
	CFilterCtrl*	GetNextFilterCrtl();

public:
	CRect	CalculateRect(CGraphCreatorView *pView);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPinConnectionCtrl)
	protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFilterCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int		m_nOutPin;
	int		m_nInPin;
	IPin*	m_pOutPin;
	IPin*	m_pInPin;
};


