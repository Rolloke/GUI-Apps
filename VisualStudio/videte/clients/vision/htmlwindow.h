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

/////////////////////////////////////////////////////////////////////////////
// CHtmlWindow window
#ifdef _HTML_ELEMENTS
interface IHTMLElement;
interface IHTMLTextContainer;
#endif

// global flags
#define HTML_ELEMENT_SHOW_HOSTS     0x00000001
#define HTML_ELEMENT_SHOW_CAMERAS   0x00000002
#define HTML_ELEMENT_SHOW_DETECTORS 0x00000004
#define HTML_ELEMENT_SHOW_RELAIS    0x00000008
#define HTML_ELEMENT_SHOW_MEDIAS		0x00000010
#define HTML_ELEMENT_SHOW_ALL       (HTML_ELEMENT_SHOW_HOSTS|HTML_ELEMENT_SHOW_CAMERAS|HTML_ELEMENT_SHOW_DETECTORS|HTML_ELEMENT_SHOW_RELAIS|HTML_ELEMENT_SHOW_MEDIAS)
#define HTML_ELEMENT_BLINK_STATE    0x00000020
#define HTML_ELEMENT_UPDATE         0x00000040
#define HTML_ELEMENT_SOUND				0x00000080
// Updateflags
#define HTML_ELEMENT_UPDATE_PAGE    0x00001100
#define HTML_ELEMENT_MENU			   0x00000200
#define HTML_ELEMENT_FIND			   0x00000400
#define HTML_ELEMENT_SELECT		   0x00000800
#define HTML_ELEMENT_UPDATE_STATE   0x00001000
#define HTML_ELEMENT_DISCONNECT     0x00002000
#define HTML_ELEMENT_CHECK_NAME     0x00004000
#define HTML_ELEMENT_DELETED        0x00008000
// element flags
#define HTML_ELEMENT_BLINK          0x00010000
#define HTML_ELEMENT_ACTIVE         0x00020000
#define HTML_ELEMENT_OPEN           0x00040000
#define HTML_ELEMENT_SHOW           0x00080000
#define HTML_ELEMENT_ENABLED        0x00100000
#define HTML_ELEMENT_OK             0x00200000
#define HTML_ELEMENT_CAM_COLOR      0x00400000
#define HTML_ELEMENT_CAM_FIXED      0x00800000
#define HTML_ELEMENT_NOT_CONNECTED  0x20000000
#define HTML_ELEMENT_NAME_CHANGED   0x40000000

#define HTML_ELEMENT_IS_HOST        0x01000000
#define HTML_ELEMENT_IS_CAMERA      0x02000000
#define HTML_ELEMENT_IS_MEDIA       0x03000000
#define HTML_ELEMENT_IS_DETECTOR    0x04000000
#define HTML_ELEMENT_IS_AUDIO_IN    0x05000000
#define HTML_ELEMENT_IS_AUDIO_OUT   0x06000000
#define HTML_ELEMENT_IS_RELAY       0x08000000
#define HTML_ELEMENT_TYPE_FLAGS		0x1f000000
#define HE_TYPE(TP)	      ((DWORD)(TP&HTML_ELEMENT_TYPE_FLAGS))



#define HTML_TYPE_UNDEFINED			0
#define HTML_TYPE_HOST_MAP				1
#define HTML_TYPE_CAMERA_MAP			2
#define HTML_TYPE_INPUT_TYPES			3
#define HTML_TYPE_INPUTS				4
#define HTML_TYPE_INPUT					5
#define HTML_TYPE_CAMERA_CARDS		6
#define HTML_TYPE_CAMERAS				7
#define HTML_TYPE_CAMERA				8
#define HTML_TYPE_FRAME_SET			9
#define HTML_TYPE_NOTIFICATION	  10

#define HOSTID_OF    HIWORD
#define SERVERID_OF  LOWORD

class CHTML_Link 
{
public:
	CHTML_Link() 
	{
		dwServer = 0;
		dwFlags  = 0;
		nItem    = 0;
		rcShape.SetRectEmpty();
	};

	CString sType;
	CString sName;
	CString sInfo;
	CSecID  secID;
	DWORD   dwServer;
	DWORD   dwFlags;
	int     nItem;
	CRect   rcShape;
};

typedef CHTML_Link * CHTML_LinkPtr;
WK_PTR_LIST(CHTML_LinksPlain, CHTML_LinkPtr);

class CHTML_Links : public CHTML_LinksPlain
{
public:
	CHTML_Links();
	virtual ~CHTML_Links();

	CHTML_Link *HitLink(CPoint pt);
	CHTML_Link *Contains(CSecID id, DWORD dwServer);
protected:
	
private:
};

class CHtmlPageHint : public CObject
{
	DECLARE_DYNCREATE(CHtmlPageHint)		
public:
	CHtmlPageHint(CObject*pRec, WORD wHost, WORD wServer, WORD wFlags);
	CSecID   GetHostSecID();
	CObject *GetRecord()   { return m_pRecord;}
	WORD     GetHostID()   { return m_wHost;}
	WORD     GetServerID() { return m_wServer;}
	WORD     GetFlags()    { return m_wFlags;}
	
private:
	CHtmlPageHint() {};
	CObject *m_pRecord;
	WORD m_wHost;
	WORD m_wServer;
	WORD m_wFlags;
};

class CHtmlWindow : public CSmallWindow
{
// Construction
public:
	CHtmlWindow(const CSecID id, CString &sURL);
	CHtmlWindow(const CSecID id, CString &sURL, CString &sTitle, CString &sPicture, int nZoom, int nType);
	CHtmlWindow(CServer* pServer, const CString& sURL, const CSecID id);
	CHtmlWindow(CServer* pServer, const CSecID id, const CString& sName);

// Attributes
public:
	inline  CString	GetURL();
	BOOL	IsURL(const CString& sURL);
	inline	BOOL	IsMapWindow() const;
				
// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CVisionView* pParentWnd);
	BOOL NavigateTo(const CString& sURL);
	void Refresh();
	void SetServer(CServer *pServer);

// Overrides
public:
	virtual	BOOL	IsHtmlWindow();
	virtual	CSecID	GetID();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlWindow)
	//}}AFX_VIRTUAL

protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual void OnDraw(CDC* pDC);

// Implementation
public:
	virtual ~CHtmlWindow();


	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHtmlHome(); 
	afx_msg void OnHtmlNext(); 
	afx_msg void OnHtmlPrev(); 
	afx_msg void OnDestroy();
	afx_msg void OnUpdateHtmlNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHtmlPrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHtmlHome(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
#ifdef _HTML_ELEMENTS
	afx_msg void OnUpdateViewMedias(CCmdUI* pCmdUI);
	afx_msg void OnViewMedias();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditClearMap();
	afx_msg void OnUpdateEditClearMap(CCmdUI* pCmdUI);
	afx_msg void OnConnectHost();
	afx_msg void OnUpdateConnectHost(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnViewHosts();
	afx_msg void OnUpdateViewHosts(CCmdUI* pCmdUI);
	afx_msg void OnViewCameras();
	afx_msg void OnUpdateViewCameras(CCmdUI* pCmdUI);
	afx_msg void OnViewDetectors();
	afx_msg void OnUpdateViewDetectors(CCmdUI* pCmdUI);
	afx_msg void OnViewRelais();
	afx_msg void OnUpdateViewRelais(CCmdUI* pCmdUI);
	afx_msg void OnAlarmSound();
	afx_msg void OnUpdateAlarmSound(CCmdUI* pCmdUI);
	afx_msg void OnViewRefresh();
	afx_msg void OnUpdateViewRefresh(CCmdUI* pCmdUI);
	afx_msg void OnHtmlSave();
	afx_msg void OnUpdateHtmlSave(CCmdUI* pCmdUI);
	afx_msg void OnSmallContext();
	afx_msg void OnUpdateSmallContext(CCmdUI* pCmdUI);
	afx_msg void OnEditHtmlLinkProperties();
	afx_msg void OnUpdateEditHtmlLinkProperties(CCmdUI* pCmdUI);
#endif
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

#ifdef _HTML_ELEMENTS
public:
	void OnFileSaveHtmlText(CSize&);
	BOOL OnEditHtmlPage();
	void OnUpdateEditHtmlPage(CCmdUI* pCmdUI);
	void OnEditClear();
	void OnUpdateEditClear(CCmdUI* pCmdUI);
	void OnEditHtmlMapProperties();
	void OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI);

	BOOL OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnMouseMove(UINT nFlags, CPoint point);
	UINT OnNcHitTest(CPoint point);
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnRequestSave();

	BOOL IsEditing() { return m_bEditPage; };
	void InsertElement(CSecID id, CString sName, WORD wServer, WORD wHost, CObject*pObj);
	int  GetHTMLwndType() { return m_nHTMLtype;};
	void CheckDragScroll(CPoint* pptClient);
	BOOL DraggedInto(CPoint pt);
#ifdef _UNICODE
	virtual int OnToolHitTest( CPoint point, TOOLINFOW* pTI ) const;
#else
	virtual int OnToolHitTest( CPoint point, TOOLINFOA* pTI ) const;
#endif
private:
	void   CalculateClientRect();
	void   ReleaseHTMLelements();
	void   SetHTMLelementID(IHTMLElement*, CSecID, DWORD, DWORD);
	CSecID GetHTMLelementID(IHTMLElement*, DWORD &, DWORD&);
	CString GetHTMLelementName(IHTMLElement*, int *pnType=NULL, CString *psType=NULL);
	CPoint GetHTMLelementPosition(IHTMLElement*, UINT, CRect*prcBound=NULL);
	BOOL   ExtractNameFromOuterHTML(const CString&, CString&);
	void   UpdateHTMLElements();
	IHTMLElement* FindElementByName(CString&, DWORD dwFlags);
	IHTMLElement* FindElementFromPoint(CPoint pt, DWORD dwSearch);
	void   InvalidateRectRgn(CRect*);
	void   InvalidateElementRgn(IHTMLElement*);
	void   InvalidateLinkRgn(CHTML_Link*);
	CRect  SetLinkShapeRect(CPoint ptClient);
	void   HitEditLink(CPoint point, CHTML_Link**, bool bUpdate=true);
	BOOL   IsHTMLElememtVisible(const CPoint& point, const CRect& rect);
	
	static BOOL CALLBACK CHtmlWindow::FindIEServer(HWND, LPARAM);
	static LRESULT CALLBACK CHtmlWindow::IEServerwndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK CHtmlWindow::HTMLwndProc(HWND, UINT, WPARAM, LPARAM);
#endif
	
protected:
	void Init();

#ifdef _HTML_ELEMENTS
	void OnSelectCamera(IHTMLElement*pHE);
	void OnSelectDetector(IHTMLElement*pHE);
	void OnSelectRelay(IHTMLElement*pHE);
	void OnSelectMedia(IHTMLElement*pHE);
	void OnSelectHost(IHTMLElement*pHE);
#else
	void OnSelectCamera(const CString& sCamURL);
	void OnSelectHost(const CString& sHostURL);
#endif
	BOOL ExtractNameFromURL(const CString &sURL, CString &sName, WORD &wHost);
	CString GetSection();

protected:
	CSecID			m_ID;
	CWebBrowser2	m_WebBrowser;
	CString			m_sName;
	CString			m_sURL;
	CString			m_sHome;
	BOOL				m_bForward;
	BOOL				m_bBack;
	BOOL				m_bMap;
	CString			m_sHTMLTitle;

#ifdef _HTML_ELEMENTS
	CPtrList            m_Cameras;
	CPtrList            m_Hosts;
	CPtrList            m_Detectors;
	CPtrList            m_Relais;
	CPtrList            m_Medias;
	CHTML_Links         m_EditLinks;
	CHTML_Link			 *m_pSelected;
	CHTML_Link			 *m_pToolTip;
	UINT                m_uToolTipTimer;
	DWORD               m_dwShow;
	CImageList	        m_Images;
	HWND                m_hwndIEServer;
	IHTMLTextContainer *m_pHTMLTextContainer;
	IHTMLElement       *m_pMAPImage;
	IHTMLElement       *m_pHESelected;
	int					  m_nZoomValue;
	int					  m_nNewZoomValue;
	int                 m_nHTMLtype;
	BOOL                m_bEditPage;
	BOOL                m_bDragging;
	CString             m_sPictureSource;
	long                m_lIEServerWndProc;
	CRect               m_rcClient;
	CRect               m_rcMapImage;
	CPoint              m_ptDiff;
	DWORD               m_dwChangeOptions;
	CString             m_sGenerator;
#endif
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
