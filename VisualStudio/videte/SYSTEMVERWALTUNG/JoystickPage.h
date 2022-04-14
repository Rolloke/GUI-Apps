#pragma once

#include "SVPage.h"
#include "resource.h"
#include "wkclasses\wk_CplApplet.h"


class CTargetCrossControl : public CStatic
{
	DECLARE_DYNCREATE(CTargetCrossControl)

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

// CJoystickPage dialog

class CJoystickPage : public CSVPage
{
	DECLARE_DYNAMIC(CJoystickPage)

// Construction
public:
	CJoystickPage(CSVView* pParent = NULL);   // standard constructor
	virtual ~CJoystickPage();

// Dialog Data
	//{{AFX_DATA(CSMSPage)
	enum { IDD = IDD_JOYSTICK };
	CTargetCrossControl m_ctrlJoyAxis;
	BOOL m_bControlSpeed;
	int				m_nZeroRange;
	//}}AFX_DATA

	UINT m_nJoyTimer;
	CJoyStickDX*	m_pJoyStick;
	DIJOYSTATE*		m_pJoyState;
	int				m_nButtonMap[MAX_JOYSTICK_BUTTONS];
	CWK_CPLApplet*m_pGameControl;

	BOOL			m_bLearnMode;

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguagePage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	virtual void Initialize();
	virtual void SaveChanges();
	virtual void CancelChanges();
// implementation
	void	InitJoystick();
	void	FreeJoystick();

protected:
	// Generated message map functions
	//{{AFX_MSG(CLanguagePage)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedJpBtnChooseJoystick();
	afx_msg void OnBnClickedJpBtnReset();
	afx_msg void OnCkBtn(UINT);
	afx_msg void OnEnChangeJpEdtZeroRange();
	afx_msg void OnBnClickedJpCkControlSpeed();
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
