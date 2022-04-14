#pragma once


//#define FBTN_STYLE (WS_VISIBLE|WS_CHILD|BS_FLAT|WS_TABSTOP)
//#define FBTN_STYLE_EX (WS_EX_NOPARENTNOTIFY|WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE)
#define FBTN_STYLE (WS_VISIBLE|WS_CHILD|WS_TABSTOP)
#define FBTN_STYLE_EX (WS_EX_NOPARENTNOTIFY|WS_EX_TRANSPARENT)

// CBaseCtrl
class CGraphCreatorView;


class CBaseCtrl : public CButton
{
	DECLARE_DYNCREATE(CBaseCtrl)

public:
	CBaseCtrl();
	virtual ~CBaseCtrl();
// Operations
public:
	BOOL AddNextCtrl(UINT nID);
	BOOL AddPrevCtrl(UINT nID);
	void RemoveCtrlID(UINT nID);
// Attributes
public:
	CGraphCreatorView*	GetView();
	CBaseCtrl*			GetNextCtrl(int i);
	CBaseCtrl*			GetPrevCtrl(int i);
	int					GetNoOfNext() { return m_dwaNextCtrls.GetSize();};
	int					GetNoOfPrev() { return m_dwaPrevCtrls.GetSize();};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseCtrl)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PostNcDestroy();
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CBaseCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL			m_bFocus;
	CPoint			m_ptMove;
	CDWordArray		m_dwaNextCtrls;
	CDWordArray		m_dwaPrevCtrls;
public:
	static int gm_nPinOffsetY;
	static int gm_nPinDistanceY;
	static int gm_nMinHeight;
	static int gm_nTextDistance;
	static int gm_nFilterDistance;
};


