#ifndef _SMALLTOOLBAR_H
#define _SMALLTOOLBAR_H

/////////////////////////////////////////////////////////////////////////////
class CSmallToolBar : public CToolBar 
{
	friend class CWndSmall;
public:
	CSmallToolBar();
	virtual ~CSmallToolBar();

	BOOL			SetTitleText(const CString &sNewTitle);

	inline void		SetBkColor(COLORREF col);
	inline COLORREF GetBkColor();
	inline void		SetTextColor(COLORREF col);
	inline CSize &	GetSize();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallToolBar)
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	DECLARE_DYNAMIC(CSmallToolBar)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CSmallToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
protected:
	COLORREF	m_BkColor;
	COLORREF	m_TextColor;
	CSize		m_Size;
	CString		m_sTitleText;
};

////////////////////////////////////////////////////////////////
inline 	void	CSmallToolBar::SetBkColor(COLORREF col)
{
	m_BkColor = col; 
}
////////////////////////////////////////////////////////////////
inline 	COLORREF CSmallToolBar::GetBkColor()
{
	return m_BkColor; 
}
////////////////////////////////////////////////////////////////
inline 	void	CSmallToolBar::SetTextColor(COLORREF col)
{
	m_TextColor = col; 
}
////////////////////////////////////////////////////////////////
inline 	CSize &CSmallToolBar::GetSize()
{
	return m_Size; 
}

#endif
