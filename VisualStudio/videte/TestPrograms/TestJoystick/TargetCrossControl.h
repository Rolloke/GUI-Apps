#pragma once


// CTargetCrossControl

class CTargetCrossControl : public CStatic
{
	DECLARE_DYNAMIC(CTargetCrossControl)

// Construction / destruction
public:
	CTargetCrossControl();
	virtual ~CTargetCrossControl();

// attributes
public:
	inline COLORREF GetBkGndColor();
	inline CPoint GetHorzRange();
	inline CPoint GetVertRange();
	inline CPoint GetPos(); 

// operations
public:
	void SetBkGndColor(COLORREF col);
	void SetCrossColor(COLORREF col);
	void SetZeroRangeColor(COLORREF col);
	void SetHorzRange(int nX1, int nX2);
	void SetHorzRange(CPoint szHorz);

	void SetVertRange(CPoint szVert);
	void SetVertRange(int nY1, int nY2);

	void SetPos(CPoint ptPos); 
	void SetPos(int nX, int nY); 
	void SetZeroRange(int nRange); 

// implementation
protected:
	CPoint TransformPoint(CRect& rcClient,CPoint pt);

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

// Data
protected:
	COLORREF	m_cBkGndColor;
	COLORREF	m_cCrossColor;
	COLORREF	m_cZeroRangeColor;
	CPoint		m_ptHorzRange;
	CPoint		m_ptVertRange;
	CPoint		m_ptPos;
	CSize		m_szCross;
	int			m_nZeroRange;
public:
};


inline COLORREF CTargetCrossControl::GetBkGndColor()
{
	return m_cBkGndColor;
}
inline CPoint CTargetCrossControl::GetHorzRange()
{
	return m_ptHorzRange;
}
inline CPoint CTargetCrossControl::GetVertRange()
{
	return m_ptVertRange;
}
inline CPoint CTargetCrossControl::GetPos()
{
	return m_ptPos;
}
