// ModifyStyleDlg.h : Header-Datei
//

#include "afxcmn.h"
#if !defined(AFX_MODIFYSTYLEDLG_H__FCF93CCC_A5FF_409F_A4EE_EBCC9E7ACDC5__INCLUDED_)
#define AFX_MODIFYSTYLEDLG_H__FCF93CCC_A5FF_409F_A4EE_EBCC9E7ACDC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleDlg Dialogfeld
#define  STYLE_SIZE 23

struct WindowStyles
{
   long nStyle;
   _TCHAR *pszStyle;
};

class CModifyStyleDlg : public CDialog
{
// Konstruktion
public:
	HICON m_hModIcon;
	void SetWindowinfo(HWND);
	CModifyStyleDlg(CWnd* pParent = NULL);	// Standard-Konstruktor
	~CModifyStyleDlg();

// Dialogfelddaten
	//{{AFX_DATA(CModifyStyleDlg)
	enum { IDD = IDD_MODIFYSTYLE_DIALOG };
	CComboBox	m_cComboStyles;
	CListCtrl	m_cStyleList;
	CTreeCtrl m_WindowTree;
	CString	m_strWindowInfo;
	int		m_nPosX;
	int		m_nPosY;
	int		m_nSizeX;
	int		m_nSizeY;
	BOOL	m_bTopMost;
	CString	m_strWndTxt;
	BOOL	m_bFindDisabled;
	BOOL	m_bHideWindow;
	BOOL	m_bSetPos;
	BOOL	m_bSetSize;
	int		m_nMessage;
	long	m_nLPARAM;
	UINT	m_uWPARAM;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CModifyStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void ToggleProcess();
	void SetTreeVisible(BOOL);

	HICON		m_hIcon;
	WindowStyles *m_pStyle;
	HWND		m_hModWnd;
	HCURSOR		m_hSearchCursor;
	long		m_nStyle;
	long		m_nGetStyle;
	CImageList	m_ImageList;
	CPtrList	m_Process;
	CPtrList	m_Children;
	HWND		m_hParentOfModWnd;
	int			m_nProcess;
	int			m_nChild;
	int			m_nHeader;
	DWORD		m_dwProcID;
	int			m_iClass;
	bool		m_bShowChildren;
	bool		m_bSetTextViaClipboard;
	bool		m_bShowAllProcesses;
	bool		m_bProcessesCreated;
	HTREEITEM	m_hCurrentTreeItem;

	static BOOL CALLBACK HitChildRect(HWND, LPARAM);
	static LRESULT CALLBACK BtnWndFc(HWND, UINT, WPARAM, LPARAM);
	static long         gm_lOldBtnWndFc;
	static _TCHAR        *gm_szClasses[];
	static WindowStyles gm_ListStyle[];
	static WindowStyles gm_ListStyleEdit[];
	static WindowStyles gm_ListStyleEx[];
	static WindowStyles gm_ListStyleBtn[];
	static WindowStyles gm_ListStyleStatic[];
	static WindowStyles gm_ListStyleDlg[];
	static WindowStyles gm_ListStyleLB[];
	static WindowStyles gm_ListStyleCB[];
	static WindowStyles gm_ListStyleSBAR[];
	static WindowStyles gm_ListStyleLV[];
	static WindowStyles gm_ListStyleLVEx[];
	static WindowStyles gm_ListStyleTV[];
	static WindowStyles gm_ListStyleTC[];
	static WindowStyles gm_ListStyleTCEx[];
	static WindowStyles gm_ListStyleTB[];
	static WindowStyles gm_ListStyleUDC[];
	static WindowStyles gm_ListClassStyle[];

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CModifyStyleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSetStyle();
	afx_msg void OnSelchangeComboStyles();
	afx_msg void OnGetdispinfoStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClickStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillProcess();
	afx_msg void OnGetProcess();
	afx_msg void OnFindWindow();
	afx_msg void OnDblclkStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCkFindDisabled();
	afx_msg void OnGetText();
	afx_msg void OnItemclickStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickStyleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCkPos();
	afx_msg void OnCkSize();
	afx_msg void OnViewProcesses();
	afx_msg void OnUpdateViewProcesses(CCmdUI* pCmdUI);
	afx_msg void OnViewChildwindows();
	afx_msg void OnUpdateViewChildwindows(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle();
	afx_msg void OnUpdateViewStyle(CCmdUI* pCmdUI);
	afx_msg void OnViewStyleEx();
	afx_msg void OnUpdateViewStyleEx(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnViewProcessModules();
	afx_msg void OnUpdateViewProcessModules(CCmdUI* pCmdUI);
	afx_msg void OnViewAllProcesses();
	afx_msg void OnUpdateViewAllProcesses(CCmdUI* pCmdUI);
	afx_msg void OnBtnPostMessage();
	afx_msg void OnFileSaveWndText();
	afx_msg void OnUpdateFileSaveWndText(CCmdUI *pCmdUI);
	afx_msg void OnViewWindowHirarchie();
	afx_msg void OnUpdateViewWindowHirarchie(CCmdUI *pCmdUI);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	afx_msg LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateStartProcess(CCmdUI *pCmdUI);
	afx_msg void OnStartProcess();
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ChangeListHeader(int, LPCTSTR szName=NULL);
	CString GetFileNameFromWindowHandle(HWND hWnd);
	void DeleteProcessData();
	static BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
	static BOOL CALLBACK EnumFirstLevelChildren(HWND, LPARAM);
	static BOOL CALLBACK EnumDesktopWindowsProc(HWND, LPARAM);
public:
	afx_msg void OnTvnSelchangedWndTree(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MODIFYSTYLEDLG_H__FCF93CCC_A5FF_409F_A4EE_EBCC9E7ACDC5__INCLUDED_)
