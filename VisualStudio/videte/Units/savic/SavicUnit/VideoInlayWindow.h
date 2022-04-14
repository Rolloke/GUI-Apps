/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: VideoInlayWindow.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/VideoInlayWindow.h $
// CHECKIN:		$Date: 19.11.02 14:07 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 19.11.02 14:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VideoInlayWindow.h : header file
//

class CCodec;
/////////////////////////////////////////////////////////////////////////////
// CVideoInlayWindow frame

class CVideoInlayWindow : public CFrameWnd
{
//	DECLARE_DYNCREATE(CVideoInlayWindow)
public:
	CVideoInlayWindow(CCodec *pCodec, BOOL bPreview);           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCodec	*m_pCodec;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
