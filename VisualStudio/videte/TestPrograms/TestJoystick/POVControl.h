#pragma once


// CPOVControl

class CPOVControl : public CStatic
{
	DECLARE_DYNAMIC(CPOVControl)

public:
	CPOVControl();
	virtual ~CPOVControl();

// attributes
public:
	inline COLORREF GetBkGndColor();
	inline COLORREF GetPointColor();
	inline long		GetAngle();

// operations
public:
	void SetBkGndColor(COLORREF col);
	void SetPointColor(COLORREF col);
	void SetAngle(long lAngle);

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

// Data
protected:
	COLORREF	m_cBkGndColor;
	COLORREF	m_cPointColor;
	long		m_lAngle;
};


inline COLORREF CPOVControl::GetBkGndColor()
{
	return m_cBkGndColor;
}

inline COLORREF CPOVControl::GetPointColor()
{
	return m_cPointColor;
}
inline long	CPOVControl::GetAngle()
{
	return m_lAngle;
}

