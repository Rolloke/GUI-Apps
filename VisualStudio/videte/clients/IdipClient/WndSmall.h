// WndSmall.h : header file

#if !defined(AFX_WNDSMALL_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_WNDSMALL_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CViewIdipClient;

#include "SmallToolBar.h"
#include "Server.h"

#define FIELD_WIDTH				108
#define NAVIGATION_HEIGHT		18
#define VW_TEXTOFF				2	
#define DW_INFO_FONT_SIZE		120
#define SET_ACTIVE_AREA_HEIGHT	20
#define TOOLBAR_HEIGHT			24

#define SHOW_TITLE				0x00000001
#define SHOW_TITLE_IN_PICTURE	0x00000010
#define SHOW_DEBUG_INFO			0x00000100
#define SHOW_DEBUG_MINIMUM		0x00001000

typedef enum EnumWndSmallType
{
	WST_ALL			=	1,
	WST_LIVE		=	2,
	WST_PLAY		=	3,
	WST_HTML		=	4,
	WST_TEXT		=	5,
	WST_ALARM_LIST	=	6,
	WST_COCO		=	7,
	WST_MICO		=	8,
	WST_MPEG4		=	9,
	WST_PT			=	10,
	WST_DIB			=	11,
	WST_REFERENCE	=	12,
};


/////////////////////////////////////////////////////////////////////////////
// CWndSmall window
class CWndSmall : public CWnd
{
	friend class CSmallToolBar;
	DECLARE_DYNAMIC(CWndSmall)		
// Construction
public:
	CWndSmall(CServer* pServer);
	CWndSmall() {TRACE(_T("Error Do not use default constructor"));};
	virtual ~CWndSmall();

// Attributes
public:
	inline	WORD		GetServerID();
	inline	CString		GetServerName();
	inline	CServer*	GetServer();
	inline	EnumWndSmallType GetType();
	CServer*			GetSafeServer();
	BOOL				IsCmdActive();
			BOOL		UseSetActiveArea();
	inline	WORD		GetHostID();
			int			GetMonitor();
	inline	DWORD		GetMonitorFlag();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndSmall)
public:
	virtual CSecID	GetID();
	virtual void	OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL	OnPreparePrinting(CPrintInfo* pInfo);
	virtual	BOOL	CanPrint();
	virtual CString	GetTitleText(CDC* pDC);
protected:
	virtual void	GetFrameRect(LPRECT lpRect);
	virtual void	OnDraw(CDC* pDC);
public:
	virtual BOOL	IsRequesting();
	virtual BOOL	IsWndLive();
	virtual	BOOL	IsWndPlay();
	virtual CSystemTime	GetTimeStamp();
	virtual BOOL	Create(const RECT& rect, CViewIdipClient* pParentWnd);

protected:
	virtual CString	GetName();
	virtual void	OnIdle();
	virtual BOOL	PreDestroyWindow(CViewIdipClient* pViewIdipClient);
	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	virtual UINT	GetToolBarID();
	virtual void	OnSetWindowSize();
	virtual	void	PopupMenu(CPoint pt);
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
protected:
	//{{AFX_MSG(CWndSmall)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVideoFullscreen();
	afx_msg void OnUpdateVideoFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnSmallClose();
	afx_msg void OnUpdateSmallClose(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSmallContext(); //Recherche
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowonMonitorExclusive();
	afx_msg void OnUpdateShowonMonitorExclusive(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowOnMonitor(CCmdUI* pCmdUI);
	afx_msg void OnShowOnMonitor(UINT nID);
	afx_msg void OnUpdateEditComment(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
//	void SetActive(BOOL bActive=TRUE);
	BOOL operator < (CWndSmall & second);
	void EnableTitle(BOOL);
	void  Set1to1(BOOL b1to1);
	virtual void OnSetCmdActive(BOOL bActive, CWndSmall* pWndSmall);
	virtual void OnSetSmallBig(BOOL bActive, CWndSmall* pWndSmall);
	void		DrawTitleTransparent(CDC* pDC, CRect*pRect, CString &sTitle);

protected:
	void DrawTitle(CDC* pDC);
	void GetTitleRect(LPRECT lpRect);
	void RedrawTitleToolbar(CDC* pDC, COLORREF crTitleBkGnd, COLORREF crTitleText);
	CString InsertSpacesForTabbedOutput(const CString sIn);

protected:
	CViewIdipClient*	m_pViewIdipClient;
	CServer*			m_pServer;
	WORD				m_wServerID;
	WORD				m_wHostID;
	int					m_iImage;
	CSystemTime			m_stTimeStamp;
	POINT				m_pointAtLeftClick;
	UINT				m_uTimerID;
	DWORD				m_dwMonitor;
	bool				m_bLeftClick;
	bool				m_bLeftDblClick;
	bool				m_b1to1;
	bool				m_bUpdateTitle;
	CSmallToolBar		m_wndToolBar;
	EnumWndSmallType	m_nType;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD	CWndSmall::GetServerID()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return m_wServerID;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD	CWndSmall::GetHostID()
{
	return m_wHostID;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CWndSmall::GetServerName()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return m_pServer ? m_pServer->GetName() : "";
}
/////////////////////////////////////////////////////////////////////////////
inline CServer*	CWndSmall::GetServer()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CWndSmall::GetMonitorFlag()
{
	return m_dwMonitor;
}
inline EnumWndSmallType CWndSmall::GetType()
{
	return m_nType;
}
/////////////////////////////////////////////////////////////////////////////
typedef CWndSmall* CWndSmallPtr;
WK_PTR_ARRAY_CS(CWndSmallArrayBase, CWndSmallPtr, CWndSmallArray);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSMALL_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
