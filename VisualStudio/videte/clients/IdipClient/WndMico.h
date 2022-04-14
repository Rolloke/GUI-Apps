/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndMico.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndMico.h $
// CHECKIN:		$Date: 6/29/06 8:53a $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 6/29/06 8:43a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MICOWND_H_
#define __MICOWND_H_

#include "WndLive.h"

/////////////////////////////////////////////////////////////////////////////
// CWndMico window
class CWndMico : public CWndLive
{
	DECLARE_DYNAMIC(CWndMico)
// Construction
public:
	CWndMico(CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CWndMico();

// Attributes
public:
protected:
	BOOL CanAdjustFPS(int fps);
	
// Implementation
public:
	void	SetRequestedFPS(int nFps);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndMico)
// Attributes
public:
	virtual	int  GetRequestedFPS();
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);

protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
	virtual CString GetTitleText(CDC* pDC);
public:
#ifdef _DEBUG
	virtual void OnIdle();
#endif
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndMico)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnChangeVideo(WPARAM, LPARAM);
	afx_msg void OnFps_X_X(UINT);
	afx_msg void OnUpdateFps_X_X(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveReference();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	// data members
private:

	DWORD			m_dwSum;
	int				m_nCount;
	int				m_iSoftFPS;
	int				m_iMaxFPS;
};
/////////////////////////////////////////////////////////////////////////////
#endif
