#if !defined(AFX_HOURCTRL_H__B9370A05_2789_11D2_B8FD_00C095ECA33E__INCLUDED_)
#define AFX_HOURCTRL_H__B9370A05_2789_11D2_B8FD_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerEditorHourCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorHourCtrl window

class CTimerEditorHourCtrl : public CButton
{
// Construction
public:
	CTimerEditorHourCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorHourCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimerEditorHourCtrl();
		// Generated message map functions
	void FillHoursFromString(const CString &sHours);	// CAVEAT fixed format
	CString GetHoursAsString(const CString & sFiller=" , ") const;
	CEdit *m_pCtlHoursText;
	void UpdateHoursText();
protected:
	//{{AFX_MSG(CTimerEditorHourCtrl)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT pDraw);	// is needed

private:
	void PosToTime(int x,int y,int &hour,int &minute);
	void TimeToPos(int hour,int minute,int &x, int &y);
	//
	void SetMinute(int hour,int minute,BOOL bValue,BOOL bRedraw=FALSE);
	BOOL GetMinute(int hour,int minute) const;
	void RedrawBox(int h, int m);
	void DrawBox(CDC &dc, int h, int m, BOOL bValue);
	void TimeTrack(int h1, int m1,int h2, int m2,BOOL bRedrawOnly, BOOL bUseDragMode);
	void TrackAction(int h, int m,BOOL bDrawOnly,BOOL bUseDragMode);
	void RedrawAxis(CDC &dc);
	void DrawMouseMessage(CDC &dc, const CString &sMsg);
	//
	CPoint m_top;
	CPoint m_mouseLabelPos;
	int m_iBlockWidth;
	int m_iBlockHeight;
	int m_ileftLabelOffset;

	DWORD m_minutes[24][2];

	BOOL m_bButtonDown;
	BOOL m_bIsDragging;
	CPoint m_pointClicked;
	CPoint m_pointDrag;
	BOOL m_bDragMode;
	//
	CBrush m_hourBrush;
	CBrush m_blackBrush;
	CBrush m_emptyBrush;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOURCTRL_H__B9370A05_2789_11D2_B8FD_00C095ECA33E__INCLUDED_)
