#if !defined(AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WndHTML.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "WndSmall.h"
#include "SmallToolBar.h"
#include "WebBrowser2.h"

/////////////////////////////////////////////////////////////////////////////
// CWndHTML window
interface IHTMLElement;
interface IHTMLTextContainer;

// global flags
#define HTML_ELEMENT_SHOW_HOSTS     0x00000001
#define HTML_ELEMENT_SHOW_CAMERAS   0x00000002
#define HTML_ELEMENT_SHOW_DETECTORS 0x00000004
#define HTML_ELEMENT_SHOW_RELAIS    0x00000008
#define HTML_ELEMENT_SHOW_MEDIAS	0x00000010
#define HTML_ELEMENT_SHOW_ALL       (HTML_ELEMENT_SHOW_HOSTS|HTML_ELEMENT_SHOW_CAMERAS|HTML_ELEMENT_SHOW_DETECTORS|HTML_ELEMENT_SHOW_RELAIS|HTML_ELEMENT_SHOW_MEDIAS)
#define HTML_ELEMENT_BLINK_STATE    0x00000020
#define HTML_ELEMENT_UPDATE         0x00000040
#define HTML_ELEMENT_SOUND			0x00000080

// Updateflags
#define HTML_ELEMENT_UPDATE_PAGE    0x00001100
#define HTML_ELEMENT_MENU			0x00000200
#define HTML_ELEMENT_GET_STATE_FLAG	0x00000400
#define HTML_ELEMENT_SELECT			0x00000800
#define HTML_ELEMENT_UPDATE_STATE   0x00001000
#define HTML_ELEMENT_DISCONNECT     0x00002000
#define HTML_ELEMENT_CHECK_NAME     0x00004000
#define HTML_ELEMENT_DELETED        0x00008000
// element flags
#define HTML_ELEMENT_BLINK          0x00010000	// detectors, relais, links
#define HTML_ELEMENT_MD             0x00010000	// camera
#define HTML_ELEMENT_ACTIVE         0x00020000	// all
#define HTML_ELEMENT_OPEN           0x00040000	// relais
#define HTML_ELEMENT_CAM_REF        0x00040000	// camera
#define HTML_ELEMENT_SHOW           0x00080000	// all
#define HTML_ELEMENT_ENABLED        0x00100000	// all
#define HTML_ELEMENT_OK             0x00200000	// all
#define HTML_ELEMENT_CAM_COLOR      0x00400000	// camera
#define HTML_ELEMENT_CAM_FIXED      0x00800000	// camera
#define HTML_ELEMENT_NOT_CONNECTED  0x20000000	// all
#define HTML_ELEMENT_NAME_CHANGED   0x40000000	// all

// elements were first code bitwise, now the are enumerated in a 5 bit range for 31 different numbers
// *: old flags bitwise coded
// #: new elements enumerated and inserted between the bit coded
#define HTML_ELEMENT_IS_HOST        0x01000000	// *
#define HTML_ELEMENT_IS_CAMERA      0x02000000	// *
#define HTML_ELEMENT_IS_MEDIA       0x03000000	// #
#define HTML_ELEMENT_IS_DETECTOR    0x04000000	// *
#define HTML_ELEMENT_IS_AUDIO_IN    0x05000000	// #
#define HTML_ELEMENT_IS_AUDIO_OUT   0x06000000	// #
#define HTML_ELEMENT_IS_RELAY       0x08000000	// *
#define HTML_ELEMENT_IS_LINK        0x10000000	// *

#define HTML_ELEMENT_TYPE_FLAGS		0x1f000000	// Bitmask for the Element ID´s
#define HE_TYPE(TP)	      ((DWORD)(TP&HTML_ELEMENT_TYPE_FLAGS))


#define MAIN_CAMERAMAP_FILE_NAME	_T("camera.htm")

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
	DECLARE_DYNAMIC(CHtmlPageHint)		
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

class CHtmlElement: public CObject
{
	DECLARE_DYNAMIC(CHtmlElement)		
public:
	CHtmlElement(IHTMLElement*pHE);
	CHtmlElement(CPoint pt);

	IHTMLElement *GetHTMLElement() { return m_pHE;}
	CPoint		  GetPosition() {return m_ptPosition;}
private:
	CHtmlElement() {};
	IHTMLElement *m_pHE;
	CPoint		  m_ptPosition;
};

class CWndHTML : public CWndSmall
{
	DECLARE_DYNAMIC(CWndHTML)		
// Construction
public:
	CWndHTML(const CSecID id, CString &sURL);
	CWndHTML(const CSecID id, CString &sURL, CString &sTitle, CString &sPicture, int nZoom, int nType);
	CWndHTML(CServer* pServer, const CString& sURL, const CSecID id);
	CWndHTML(CServer* pServer, const CSecID id, const CString& sName);

// Attributes
public:
	inline  CString	GetURL();
	BOOL	IsURL(const CString& sURL);
	inline	BOOL	IsMapWindow() const;
	static CString GetHTMLCodePage();
	static CString GetHTMLCodePageMetaTag();

				
// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd);
	BOOL NavigateTo(const CString& sURL);
	void Refresh();
	void SetServer(CServer *pServer);
    
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndHTML)
	virtual UINT	GetToolBarID();
	virtual	CSecID	GetID();
	virtual	BOOL CanPrint();

protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndHTML();


	// Generated message map functions
protected:
	//{{AFX_MSG(CWndHTML)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHtmlHome(); 
	afx_msg void OnHtmlNext(); 
	afx_msg void OnHtmlPrev(); 
	afx_msg void OnDestroy();
	afx_msg void OnUpdateHtmlNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHtmlPrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHtmlHome(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
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
	afx_msg void OnViewRelays();
	afx_msg void OnUpdateViewRelays(CCmdUI* pCmdUI);
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
	afx_msg void OnHtmlNewPage();
	afx_msg void OnUpdateHtmlNewPage(CCmdUI *pCmdUI);
	//}}AFX_MSG
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

	BOOL IsEditing() { return m_bEditPage ? TRUE : FALSE; };
	void InsertElement(CSecID id, CString sName, WORD wServer, WORD wHost, CObject*pObj);
	int  GetHTMLwndType() { return m_nHTMLtype;};
	HCURSOR CheckDragScroll(CPoint* pptClient);
	BOOL DraggedInto(CPoint &pt);
#ifdef _UNICODE
	virtual int OnToolHitTest( CPoint point, TOOLINFOW* pTI ) const;
#else
	virtual int OnToolHitTest( CPoint point, TOOLINFOA* pTI ) const;
#endif
private:
	void   CalculateClientRect();
	void   ReleaseHTMLelements();
	void   SetHTMLelementID(IHTMLElement*, CSecID, DWORD, DWORD, BOOL bForce=FALSE);
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
	void   InsertKnownHostsToCameraView(BOOL bInsert);
	
	static BOOL CALLBACK CWndHTML::FindIEServer(HWND, LPARAM);
	static LRESULT CALLBACK CWndHTML::IEServerwndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK CWndHTML::HTMLwndProc(HWND, UINT, WPARAM, LPARAM);
	
protected:
	void Init();

	void OnSelectCamera(IHTMLElement*pHE);
	void OnSelectDetector(IHTMLElement*pHE);
	void OnSelectRelay(IHTMLElement*pHE);
	void OnSelectMedia(IHTMLElement*pHE);
	void OnSelectHost(IHTMLElement*pHE);

	BOOL ExtractNameFromURL(const CString &sURL, CString &sName, WORD &wHost);
	void ScrollToPosition(CPoint &pt);
	void CheckPathURL(CString &sURL);
	CString GetSection();

protected:
	CSecID			m_ID;
	CWebBrowser2	m_WebBrowser;
	CString			m_sName;
	CString			m_sURL;
	CString			m_sHome;
	bool			m_bForward;
	bool			m_bBack;
	bool			m_bMap;
	bool            m_bEditPage;
	BOOL            m_bDragging;
	CString			m_sHTMLTitle;

	static CImageList	gm_Images;
	CPtrList            m_Cameras;
	CPtrList            m_Hosts;
	CPtrList            m_Detectors;
	CPtrList            m_Relays;
	CPtrList            m_Medias;
	CHTML_Links         m_EditLinks;
	CHTML_Link		   *m_pSelected;
	CHTML_Link		   *m_pToolTip;
	UINT                m_uToolTipTimer;
	UINT                m_uUpdateTimer;
	DWORD               m_dwShow;
	HWND                m_hwndIEServer;
	IHTMLTextContainer *m_pHTMLTextContainer;
	IHTMLElement       *m_pMAPImage;
	IHTMLElement       *m_pHESelected;
	int					m_nZoomValue;
	int					m_nNewZoomValue;
	int                 m_nHTMLtype;
	CString             m_sPictureSource;
	long                m_lIEServerWndProc;
	CRect               m_rcClient;
	CRect               m_rcMapImage;
	CPoint              m_ptDiff;
	CPoint              m_ptLastHit;
	DWORD               m_dwChangeOptions;
	CString             m_sGenerator;
	CMapDWORDToDWORD	m_HTMLelementFlags;
	CMapDWORDToDWORD	m_LinkDetectors;
	int					m_nCurrentBlinkingElement;
	int					m_nBlinkDivider;
public:
};
/////////////////////////////////////////////////////////////////////////////
inline CString CWndHTML::GetURL()
{
	return m_sURL;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndHTML::IsMapWindow() const
{
	return m_bMap;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLWINDOW_H__A18E9306_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
