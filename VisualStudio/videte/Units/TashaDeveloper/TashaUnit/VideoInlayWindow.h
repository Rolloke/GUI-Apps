/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: VideoInlayWindow.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaUnit/VideoInlayWindow.h $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
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
#include "jpeglib\CJpeg.h"

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
	void DisableNoiseReduction();
	void EnableNoiseReduction();
	void SetPreviewMode(BOOL bMultyView);
	BOOL DrawImage(DATA_PACKET *pImage, BOOL bDrawAdaptiveMask=FALSE, BOOL bDrawPermanentMask=FALSE, BOOL bDrawDiffImage=FALSE);
	BOOL DrawCross(CDC* pDC, const CRect& rect, int nX, int nY, int nValue, BOOL bMain);
	BOOL DrawAdaptiveMask(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue);
	BOOL DrawPermanentMask(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue);
	BOOL DrawDiffImage(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoInlayWindow)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CVideoInlayWindow();

	// Generated message map functions
	//{{AFX_MSG(CVideoInlayWindow)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LPBITMAPINFO CreateDIB(const DATA_PACKET* pPacket);

	CCodec*			m_pCodec;
	CJpeg			m_Jpeg;
	LPBITMAPINFO	m_lpBMI;
	BOOL			m_bMultyView;
	WORD			m_wSource;
	MASK_STRUCT		m_PaintMask;

	int				m_nLastPaintPosX;
	int				m_nLastPaintPosY;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOINLAYWINDOW_H__95831A44_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
