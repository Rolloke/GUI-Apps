// ChildView.h : Schnittstelle der Klasse CChildView
//


#pragma once


// CChildView-Fenster

class CChildView : public CWnd
{
// Konstruktion
public:
	CChildView();

// Attribute
public:

// Operationen
public:
protected:
	void	OnDraw(CDC*pDC);
	void	DrawBackGround(CDC*pDC, CRect&rc);
	void	DrawMovingBlock(CDC*pDC);
	void	DrawColorsRGBCMY(CDC*pDC);
	void	DrawShadeWhiteToBlack(CDC*pDC);
	void	DrawBlinkWhiteAndBlack(CDC*pDC);
	void	DrawLines(CDC*pDC);
	void	TestPicture5(CDC*pDC);
	void	TestPicture6(CDC*pDC);
	void	TestPicture7(CDC*pDC);
	void	TestPicture8(CDC*pDC);
	void	TestPicture9(CDC*pDC);
// Überschreibungen
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CChildView();

	// Generated message map functions
	//{{AFX_MSG(CChildView)
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnApplyNow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	UINT	m_nTimer;
	CPoint	m_ptPosition;
	CPoint	m_ptDirection;
	BOOL	m_bBlack;
public:
};

