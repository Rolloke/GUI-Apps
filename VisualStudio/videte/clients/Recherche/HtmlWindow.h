#if !defined(AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HtmlWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SmallWindow.h"
#include "SmallToolBar.h"
#include "WebBrowser2.h"

interface IHTMLElement;
interface IHTMLTextContainer;

/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow window

class CHtmlWindow : public CSmallWindow
{
// Construction
public:
	CHtmlWindow();

// Attributes
public:
	virtual	CSecID  GetID();
	inline  CString	GetURL();
			BOOL	IsURL(const CString& sURL);
	inline	BOOL	IsMapWindow() const;
				
// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CRechercheView* pParentWnd);
	BOOL NavigateTo(const CString& sURL);
	void Refresh();

	void ReleaseHTMLelements();
	void UpdateHTMLElements();
	CPoint GetHTMLelementPosition(IHTMLElement*, UINT, CRect*prcBound=NULL);
	CSecID GetHTMLelementID(IHTMLElement*, DWORD* pdwFlags=NULL, DWORD *pdwServer=NULL);

	static BOOL CALLBACK CHtmlWindow::FindIEServer(HWND, LPARAM);
	static LRESULT CALLBACK CHtmlWindow::IEServerwndProc(HWND, UINT, WPARAM, LPARAM);

// Overrides
public:
	virtual	BOOL	IsHtmlWindow();
	virtual CString	GetName();
	virtual void GetFrameRect(LPRECT lpRect);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlWindow)
	//}}AFX_VIRTUAL

protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	virtual UINT	GetToolBarID();
	virtual void	OnSetWindowSize();
	virtual	void	PopupMenu(CPoint pt);

// Implementation
public:
	virtual ~CHtmlWindow();


	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHtmlNext(); 
	afx_msg void OnHtmlPrev(); 
	afx_msg void OnHtmlHome(); 
	afx_msg void OnUpdateHtmlNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHtmlPrev(CCmdUI* pCmdUI);
	afx_msg void OnSmallClose();
	//}}AFX_MSG
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

protected:
	// Events
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnPropertyChange(LPCTSTR lpszProperty);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnQuit();
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
		VARIANT* Headers,   BOOL* Cancel);

protected:
	void Init();
	void OnSelectHost(const CString& sHostURL);
	virtual int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE){ return 0;};

protected:
	CSecID			m_ID;
	CWebBrowser2	m_WebBrowser;
	CString			m_sName;
	CString			m_sURL;
	BOOL			m_bForward;
	BOOL			m_bBack;
	CSystemTime		m_stTimeStamp;
	BOOL		    m_bMap;
	CString			m_sHTMLTitle;

	CPtrList            m_Hosts;
	IHTMLTextContainer *m_pHTMLTextContainer;
	IHTMLElement       *m_pMAPImage;
	int					  m_nZoomValue;
	HWND                m_hwndIEServer;
	long                m_lIEServerWndProc;
	CRect               m_rcClient;
	HICON               m_hHostIcon;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CHtmlWindow::GetURL()
{
	return m_sURL;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CHtmlWindow::IsMapWindow() const
{
	return m_bMap;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
