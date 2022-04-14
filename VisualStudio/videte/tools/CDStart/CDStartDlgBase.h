#if !defined(AFX_CDSTARTDLGBASE_H__A521CE84_DD31_11D3_BAB4_00400531137E__INCLUDED_)
#define AFX_CDSTARTDLGBASE_H__A521CE84_DD31_11D3_BAB4_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDStartDlgBase.h : header file
//

////////////////////////////////////////////////////////////////////////////
//#define COLOR_GOLD_METALLIC_CENTER	RGB(180,180,150)

////////////////////////////////////////////////////////////////////////////
static const TCHAR szFontArialBlack[]	= _T("Arial Black");
static const TCHAR szFontArial[]	= _T("Arial");
static const TCHAR szFontMSSansSerif[]	= _T("MS Sans Serif");

/////////////////////////////////////////////////////////////////////////////
// CCDStartDlgBase dialog
class CCDStartDlgBase : public CTransparentDialog
{
// Construction
public:
	CCDStartDlgBase(CWnd* pParent = NULL);   // standard constructor
	CCDStartDlgBase(UINT nIDTemplate, CWnd* pParent = NULL);
private:
	void	InitConstructorRest();

	static int CALLBACK MyEnumFontFamExProc(
									ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
									NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
									int FontType,             // type of font
									LPARAM lParam             // application-defined data
									);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCDStartDlgBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual BOOL	StretchElements();
	virtual void	InitBackground();
	virtual void	InitTitle();
	virtual void	InitButtons();
	virtual void	InitDisplays();
	virtual void	InitCustomColors();
	virtual void	Init();
	// Diese Funktion wird aufgerufen, sobald das Fenster dargestellt wird

	virtual void	ActionButton1();
	virtual void	ActionButton2();
	virtual void	ActionButton3();
	virtual void	ActionButton4();
	virtual void	ActionButton5();

// Implementation
protected:
	void	CreateIniFileName();
	void	InitDlgControls();
	void	ReadIniFile();
	void	InitWindowSize();
	void	AutoRun();
	CString	GetAllKeys();
	CString	GetOneEntry(CString& sKey);
	CString	GetProgramPathFromEntry(CString& sEntry);
	void	StartProgram(CString sProgram);

	void	DrawBackGroundColorChangeSimple(CDC* pDC, CRect rect);
	void	DrawBackGroundColorChangeBrushed(CDC* pDC, CRect rect);

	void	ShowDisplay(int iDisplay);
	void	HideLastDisplay();

	// Generated message map functions
	//{{AFX_MSG(CCDStartDlgBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnTest();
	//}}AFX_MSG
	afx_msg LRESULT OnInitDlgReady(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void		CollectAllButtons();
	void		CollectAllDisplays();
	BOOL		GetAllSectionEntries(LPCTSTR szSection, CStringArray& sArray);
	CString		GetIniString(LPCTSTR szSection,
							 LPCTSTR szEntry,
							 LPCTSTR szDefault);
	int			GetIniInt(LPCTSTR szSection,
						  LPCTSTR szEntry,
						  int iDefault);
	CString		GetValidFontName(const CString& sFontName, const CString& sDefault);
	COLORREF	GetColorRef(const CString& sColor, COLORREF colDefault);
	BOOL		IsCopy(const CString& sEntry);
	BOOL		IsRecursivCopy(const CString& sEntry);
	BOOL		SplitSourceAndDest(const CString& sEntry, CString& sSource, CString& sDest);
	void		StartCopy(const CString& sSource, const CString& sDest, BOOL bRecursiv);
	enumSkinButtonShape		GetButtonShape(int iShape);
	enumSkinButtonSurface	GetButtonSurface(int iSurface);
	STYLE_BACKGROUND		GetBackgroundStyle(int iStyle);

	void		RGB2YCrCb(COLORREF col, double& dY, double& dCb, double& dCr);
	COLORREF	YCrCb2RGB(const double& dY, const double& dCb, const double& dCr);

// Dialog Data
protected:
	//{{AFX_DATA(CCDStartDlgBase)
	enum { IDD = IDD_CDSTART_DIALOG };
	CStatic	m_txtSize;
	CDisplay	m_Display5;
	CDisplay	m_Display4;
	CDisplay	m_Display3;
	CDisplay	m_Display2;
	CDisplay	m_Display1;
	CDisplay	m_DisplayTitle;
	CSkinButton	m_btnTest1;
	CSkinButton	m_btnTest2;
	CSkinButton	m_btn1;
	CSkinButton	m_btn2;
	CSkinButton	m_btn3;
	CSkinButton	m_btn4;
	CSkinButton	m_btn5;
	CString	m_sSize;
	//}}AFX_DATA
	HICON			m_hIcon;
	BOOL			m_bSkinTest;
	BOOL			m_bAutoRun;
	int				m_iDisplayShownLast;
	int				m_iWindowWidth;
	int				m_iWindowHeight;
	CString			m_sIniFile;
	CString			m_sActualDrive;
	CString			m_sActualDir;
	CSkinButtons	m_Buttons;
	CDisplays		m_Displays;
	CStringArray	m_saFontNames;
	COLORREF		m_CustomColorRefs[16];

	// Background
	STYLE_BACKGROUND	m_eStyleBackground;
	COLORREF			m_BaseColor;
	double			m_dMaxColorChangeRange;

	// Title
	COLORREF		m_colTitle;
	COLORREF		m_colTitleShadow;
	CString			m_sTitleFontName;
	int				m_iTitleFontHeight;
	CString			m_sTitle;

	// Buttons
	CFont					m_fontButtons;
	enumSkinButtonShape		m_eButtonShape;
	enumSkinButtonSurface	m_eButtonSurface;
	COLORREF				m_colButton;
	COLORREF				m_colButtonText;
	COLORREF				m_colButtonTextHighlight;
	CString					m_sButtonFontName;
	int						m_iButtonFontHeight;
	CString					m_sButton1;
	CString					m_sButton2;
	CString					m_sButton3;
	CString					m_sButton4;
	CString					m_sButton5;

	// Displays
	COLORREF		m_colDisplay;
	COLORREF		m_colDisplayShadow;
	COLORREF		m_colDisplayBackGround;
	CString			m_sDisplayFontName;
	int				m_iDisplayFontHeight;
	CString			m_sDisplay1;
	CString			m_sDisplay2;
	CString			m_sDisplay3;
	CString			m_sDisplay4;
	CString			m_sDisplay5;

	// Actions
	CString			m_sProgram1;
	CString			m_sProgram2;
	CString			m_sProgram3;
	CString			m_sProgram4;
	CString			m_sProgram5;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDSTARTDLGBASE_H__A521CE84_DD31_11D3_BAB4_00400531137E__INCLUDED_)
