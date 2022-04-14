/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: VideoInlayWindow.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/VideoInlayWindow.h $
// CHECKIN:		$Date: 27.03.06 9:30 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 24.03.06 17:43 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VideoInlayWindow.h : header file
//
#include "CUDP.h"
#include "CMDPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoInlayWindow frame

class CUDP;
class CJpeg;
class CVideoInlayWindow : public CFrameWnd
{
//	DECLARE_DYNCREATE(CVideoInlayWindow)
public:
	CVideoInlayWindow(CUDP* pUDP, CQUnitDlg* pMainWnd);

// Attributes
public:

// Operations
public:
	BOOL DrawImage(QIMAGE* pQImage, BOOL bMulti);
	void SetActiveCamera(int nActiveChannel);
	BOOL DrawCross(CDC* pDC, const CRect& rect, int nX, int nY, int nVal, BOOL bMain);
	void SetDeinterlace(BOOL bDeInt);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoInlayWindow)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CVideoInlayWindow();

	// Generated message map functions
	//{{AFX_MSG(CVideoInlayWindow)
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CJpeg*		m_pJpeg;
	CUDP*		m_pUDP;
	CQUnitDlg*	m_pMainWnd;
	int			m_nCameraPos[MAX_CAMERAS];
	int			m_nActiveCamera;
	DWORD		m_dwVideoStatus;
	BOOL		m_bMulti;
	int			m_nVisibleCameras;	// Anzahl der Kameras mit Videosignal.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
