#if !defined(AFX_CSKINSDEMODLG_H__40CB1113_82C2_11D3_8DDD_004005A11E32__INCLUDED_)
#define AFX_CSKINSDEMODLG_H__40CB1113_82C2_11D3_8DDD_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CSkinsDemoDlg.h : header file
//

#define MAX_CAM 8

/////////////////////////////////////////////////////////////////////////////
// CSkinsDemoDlg dialog
			    
class CSkinsDemoDlg : public CTransparentDialog
{
// Construction
public:
	CSkinsDemoDlg(CWnd* pParent = NULL);   // standard constructor

	typedef enum{InputNo, InputPIN, InputDate, InputTime, InputTimer} INP_STATE;
	enum{View1x1, View2x2, View3x3, View4x4} ViewMode;
	enum{Mo=1, Di=2, Mi=3, Do=4, Fr=5, Sa=6, So=7} WeekDays;
// Dialog Data
	//{{AFX_DATA(CSkinsDemoDlg)
	enum { IDD = IDD_SKINSDEMO_DIALOG };
	CSkinButton	m_Check1;
	CSkinButton	m_Radio2;
	CSkinButton	m_Radio1;
	CBMSlider	m_ctrlSlider1;
	CSkinButton	m_ctrlButtonViewLower;
	CSkinButton	m_ctrlButtonViewHigher;
	CDisplay	m_ctrlStaticView;
	CSkinButton	m_ctrlButtonDowLower;
	CSkinButton	m_ctrlButtonDowHigher;
	CDisplay	m_ctrlStaticTimer;
	CSkinButton	m_ctrlButtonAsterix;
	CSkinButton	m_ctrlButtonSearch;
	CSkinButton	m_ctrlButtonClear;
	CSkinButton	m_ctrlButtonSaturationHigher;
	CSkinButton	m_ctrlButtonSaturationLower;
	CDisplay	m_ctrlStaticSaturation;
	CDisplay	m_ctrlStaticBright;
	CSkinButton	m_ctrlButtonBrightHigher;
	CSkinButton	m_ctrlButtonBrightLower;
	CDisplay	m_ctrlStaticContrast;
	CSkinButton	m_ctrlButtonContrastLower;
	CSkinButton	m_ctrlButtonContrastHigher;
	CSkinButton	m_ctrlButtonStop;
	CSkinButton	m_ctrlButtonRewind;
	CSkinButton	m_ctrlButtonPlay;
	CSkinButton	m_ctrlButtonPause;
	CSkinButton	m_ctrlButtonFF;
	CSkinButton	m_ctrlButtonCamLower;
	CSkinButton	m_ctrlButtonCamHigher;
	CSkinButton	m_ctrlButtonMode;
	CSkinButton	m_ctrlButtonOk;
	CSkinButton	m_ctrlButtonNum9;
	CSkinButton	m_ctrlButtonNum8;
	CSkinButton	m_ctrlButtonNum7;
	CSkinButton	m_ctrlButtonNum6;
	CSkinButton	m_ctrlButtonNum5;
	CSkinButton	m_ctrlButtonNum4;
	CSkinButton	m_ctrlButtonNum3;
	CSkinButton	m_ctrlButtonNum2;
	CSkinButton	m_ctrlButtonNum1;
	CSkinButton	m_ctrlButtonNum0;
	CDisplay	m_ctrlDisplayEmpty2;
	CDisplay	m_ctrlDisplayEmpty1;
	CDisplay	m_ctrlDisplayCamera;
	CDisplay	m_ctrlDisplayAlarm;
	CDisplay	m_ctrlDisplayViewMode;
	CDisplay	m_ctrlDisplayDate;
	CDisplay	m_ctrlDisplayTime;
	CDisplay	m_ctrlDisplayMode;
	CFrame		m_ctrlBorder1;
	CDisplay	m_ctrlStaticCamera;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinsDemoDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSkinsDemoDlg)
	afx_msg void OnButtonOk();
	afx_msg void OnButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonMode();
	afx_msg void OnButtonCamHigher();
	afx_msg void OnButtonCamLower();
	afx_msg void OnButtonNum1();
	afx_msg void OnButtonNum2();
	afx_msg void OnButtonNum3();
	afx_msg void OnButtonNum4();
	afx_msg void OnButtonNum5();
	afx_msg void OnButtonNum6();
	afx_msg void OnButtonNum7();
	afx_msg void OnButtonNum8();
	afx_msg void OnButtonNum9();
	afx_msg void OnButtonFF();
	afx_msg void OnButtonPause();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonRewind();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonContrastLower();
	afx_msg void OnButtonContrastHigher();
	afx_msg void OnButtonBrightnessLower();
	afx_msg void OnButtonBrightnessHigher();
	afx_msg void OnButtonSaturationLower();
	afx_msg void OnButtonSaturationHigher();
	afx_msg void OnButtonNum0();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonSearch();
	afx_msg void OnButtonAsterix();
	afx_msg void OnButtonDayofweekHigher();
	afx_msg void OnButtonDayofweekLower();
	afx_msg void OnButtonViewHigher();
	afx_msg void OnButtonViewLower();
	afx_msg void OnReleasedCaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void OnButtonNum(const CString &s);

private:
	CSkinButtons	m_NumButtons;
	int			m_nMode;
	int			m_nCamNr;
	int			m_nBrightness;
	int			m_nContrast;
	int			m_nSaturation;
	int			m_nDayOfWeek;
	CString		m_sPin;
	CString		m_sDate;
	CString		m_sTime;
	CString		m_sTimer[8];
	CString		m_sDayName[8];
	INP_STATE	m_eInputState;
	int			m_nViewMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSkinsDemoDlg_H__40CB1113_82C2_11D3_8DDD_004005A11E32__INCLUDED_)
