#if !defined(AFX_FILTERBUTTON_H__69D35287_2774_4A60_B10D_9C3BDFA33833__INCLUDED_)
#define AFX_FILTERBUTTON_H__69D35287_2774_4A60_B10D_9C3BDFA33833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterButton.h : header file
//

#include "BaseCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterCtrl window
interface IBaseFilter;
class CFilterCtrl : public CBaseCtrl
{
	friend class CGraphCreatorView;
	friend class CInPinProperties;
	friend class COutPinProperties;
	friend class CPinConnectionCtrl;
// Construction

	CFilterCtrl() {};
public:

	CFilterCtrl(CGraphCreatorView*pParent, IMoniker*pMoniker, CString& sFilterName);
	CFilterCtrl(CGraphCreatorView*pParent, IBaseFilter *pFilter, CString& sFilterName);
	DECLARE_DYNCREATE(CFilterCtrl)

// Attributes
public:
	IBaseFilter*	GetFilter() {return m_pFilter;}
	CSize			EnumPins();
	IPin*			GetInPin(int i, HRESULT &hr);
	IPin*			GetOutPin(int i, HRESULT &hr);
	int			GetInPinCount() { return m_sInPins.GetCount(); }
	int			GetOutPinCount() { return m_sOutPins.GetCount(); }
	int			GetPinNo(IPin*, bool bInPin, HRESULT &hr);
	CPoint		GetInPinPos(int i);
	CPoint		GetOutPinPos(int i);
	int			GetCurrentPinIndex( bool& bInPin);
	int			PinIndexFromPosition(CPoint pt, bool& bInPin);
   CSize			GetFilterTextSize() { return m_szFilterText; };

// Operations
public:
	void			CheckConnections();
	void			InvalidateConnections();
protected:
	HRESULT			InitDumpFilter(IBaseFilter*pDump);
	HRESULT			ReceiveMediaSample(IMediaSample *pSample, long nPackageNo);
	static HRESULT WINAPI ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterCtrl)
	protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFilterCtrl();


	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPinRender();
	afx_msg void OnUpdatePinShowmediatypes(CCmdUI *pCmdUI);
	afx_msg void OnPinShowmediatypes();
	afx_msg void OnFilterProperties();
	afx_msg void OnUpdateFilterProperties(CCmdUI *pCmdUI);
	afx_msg void OnFilterInterfaces();
	afx_msg void OnUpdateFilterInterfaces(CCmdUI *pCmdUI);
	afx_msg void OnPinInterfaces();
	afx_msg void OnUpdatePinInterfaces(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilterRefresh(CCmdUI *pCmdUI);
	afx_msg void OnFilterRefresh();
	afx_msg void OnUpdateFilterFilename(CCmdUI *pCmdUI);
	afx_msg void OnFilterFilename();
   afx_msg void OnFiltersPinProperties();
   afx_msg void OnUpdateFiltersPinProperties(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	IBaseFilter*	m_pFilter;
	CStringArray	m_sInPins;
	CStringArray	m_sOutPins;
   CWordArray     m_MediatypeInPin;
   CWordArray     m_MediatypeOutPin;
	CSize			m_szFilterText;
	bool			m_bInPin;
	bool			m_bSingleLine;
	WORD			m_wOnPin;
	WORD			m_wOffset1;
	WORD			m_wOffset2;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERBUTTON_H__69D35287_2774_4A60_B10D_9C3BDFA33833__INCLUDED_)
