// SimUnitDlg.h
// w+k Sample Application
// Author : Uwe Freiwald
//
// CSimUnitDlg is the application main window class, which performs
// all UI operations
// The dialog contains all controls for Input and Output to
// Security ,two Edit Controls of IO history, 4 alarm state check
// buttons and 4 relay controls, which should illustrate relays.
//
// It's implemented as a modeless dialog box, for further information
// see Win SDK documentation.
//

/////////////////////////////////////////////////////////////////////////////
class CSimUnitApp;

/////////////////////////////////////////////////////////////////////////////
#define ALARM_RANDOM	0
#define ALARM_REGULAR	1
#define ALARM_ALL		2

#define INTERVAL_REGULAR	0
#define INTERVAL_RANDOM		1

#define HOLD_REGULAR	0
#define HOLD_RANDOM		1

#define NO_OF_ALARM_BTN		16
/////////////////////////////////////////////////////////////////////////////
// CSimUnitDlg dialog
class CSimUnitDlg : public CSkinDialog
{
// Construction
public:
	CSimUnitDlg(CSimUnitApp* pApp);
	~CSimUnitDlg();

// Attributes
	inline DWORD GetRelayMask() const;
	inline BOOL	HasRandomAlarms() const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL GetToolTip(UINT nID, CString&sText);
	//}}AFX_VIRTUAL

// Implementation
public:
	void	InputHistory(UINT nID);
	void	InputHistory(LPCTSTR lpszMessage);
	void	OutputHistory(UINT nID);
	void	OutputHistory(LPCTSTR lpszMessage);

	void	ResetAlarms(DWORD dwAlarmMask, DWORD dwOpenMask);
	void	ActualizeRelay(int iNr, BOOL bState);

	void	CheckAlarms();

	// Generated message map functions
	//{{AFX_MSG(CSimUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnAbout();
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCheckRandomAlarm();
	afx_msg void OnRadioAlarm();
	afx_msg void OnRadioHold();
	afx_msg void OnRadioInterval();
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnAlarm(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
	void	ShowHide();
	int		EnableAlarmButtons(BOOL bEnable) ;
	void	InitNextAlarmStart(int iPreviousAlarm);
	void	InitNextAlarmEnd(int iAlarm);
	void	StopRandomAlarms();
	void	LoadSettings();
	void	SaveSettings();
	BOOL	IsDataValid();
	void	DDX_TextMS(CDataExchange* pDX, UINT nID, DWORD &dwValue);

// Dialog Data
protected:
	//{{AFX_DATA(CSimUnitDlg)
	enum { IDD = IDD_SIMUNIT_DIALOG };
	CButton	m_btnAlarmRandom;
	CButton	m_btnAlarmRegular;
	CButton	m_btnAlarmAll;
	CButton	m_btnIntervalRegular;
	CButton	m_btnIntervalRandom;
	CButton	m_btnHoldRegular;
	CButton	m_btnHoldRandom;
	CStatic	m_staticLine;
	CEdit	m_editIntervalRandomMax;
	CEdit	m_editHoldRandomMax;
	CEdit	m_editIntervalRegular;
	CEdit	m_editIntervalRandomMin;
	CEdit	m_editHoldRegular;
	CEdit	m_editHoldRandomMin;
	CButton	m_btnRandomAlarm;
	DWORD	m_dwHoldRegular;
	DWORD	m_dwHoldRandomMin;
	DWORD	m_dwIntervalRandomMin;
	DWORD	m_dwIntervalRegular;
	int		m_iRadioHold;
	int		m_iRadioInterval;
	int		m_iRadioAlarm;
	DWORD	m_dwHoldRandomMax;
	DWORD	m_dwIntervalRandomMax;
	//}}AFX_DATA
	CButton	m_btnAlarm[NO_OF_ALARM_BTN];
	BOOL	m_bAlarm[NO_OF_ALARM_BTN];
	HICON m_hIcon;

	CSimUnitApp*	m_pApp;
	HBITMAP			m_bmOpen;
	HBITMAP			m_bmClose;
	//
	DWORD m_dwEnableMask;
	DWORD m_dwEdgeMask;
	DWORD m_dwRelayMask;

	int			m_iSmallWidth;
	int			m_iBigWidth;
	int			m_iRandomAlarm;
	CSystemTime	m_stStartTime[NO_OF_ALARM_BTN];
	CSystemTime	m_stEndTime[NO_OF_ALARM_BTN];
	CSystemTime	m_stLastStartTime;

	DWORD	m_dwAlarmCounter[NO_OF_ALARM_BTN];
	CStringArray	m_saAlarmBtnTexts;
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSimUnitDlg::GetRelayMask() const
{
	return m_dwRelayMask;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSimUnitDlg::HasRandomAlarms() const
{
	return (m_iRandomAlarm == 1);
}
