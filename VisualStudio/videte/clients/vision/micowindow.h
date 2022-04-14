/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MicoWindow.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/MicoWindow.h $
// CHECKIN:		$Date: 13.01.03 15:52 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 13.01.03 11:45 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __MICOWND_H_
#define __MICOWND_H_

#include "DisplayWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CMicoWindow window
class CMicoWindow : public CDisplayWindow
{
// Construction
public:
	CMicoWindow(CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CMicoWindow();

// Attributes
public:
	virtual BOOL IsMico();
	virtual	int  GetRequestedFPS();
	
// Operations
public:
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMicoWindow)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMicoWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangeVideo();
	afx_msg void OnFps11();
	afx_msg void OnUpdateFps11(CCmdUI* pCmdUI);
	afx_msg void OnFps12();
	afx_msg void OnUpdateFps12(CCmdUI* pCmdUI);
	afx_msg void OnFps13();
	afx_msg void OnUpdateFps13(CCmdUI* pCmdUI);
	afx_msg void OnFps14();
	afx_msg void OnUpdateFps14(CCmdUI* pCmdUI);
	afx_msg void OnFps15();
	afx_msg void OnUpdateFps15(CCmdUI* pCmdUI);
	afx_msg void OnFps21();
	afx_msg void OnUpdateFps21(CCmdUI* pCmdUI);
	afx_msg void OnFps31();
	afx_msg void OnUpdateFps31(CCmdUI* pCmdUI);
	afx_msg void OnFps41();
	afx_msg void OnUpdateFps41(CCmdUI* pCmdUI);
	afx_msg void OnFps61();
	afx_msg void OnUpdateFps61(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFps121();
	afx_msg void OnUpdateFps121(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveReference();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	BOOL CanAdjustFPS(int fps);

	// data members
private:

	DWORD			m_dwSum;
	int				m_nCount;
	int				m_iSoftFPS;
	int				m_iMaxFPS;

	CJpeg*			m_pJpeg;
};
/////////////////////////////////////////////////////////////////////////////
#endif
