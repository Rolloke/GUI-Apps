// HealthControlDlg.h : Headerdatei
//

#pragma once


// CHealthControlDlg Dialogfeld
class CECBase;
class CHealthControlDlg : public CDialog
{
// Konstruktion
public:
	CHealthControlDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CHealthControlDlg)
	enum { IDD = IDD_HEALTHCONTROL_DIALOG };
	int m_nFan1RPM;
	int m_nFan2RPM;
	int m_nFan3RPM;
    int m_nTemperature1;
	int m_nTemperature2;
	int m_nTemperature3;
	float m_fVCoreA;
	float m_fVCoreB;
	float m_f12V;
	float m_fN5V;
	float m_f3_3V;
	float m_f5V;
	float m_fN12V;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHealthControlDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL


// Implementierung
private:
#if WKCLASSES == 1
	int GetProfInt(CWK_Profile& wkp, LPCTSTR szSection, LPCTSTR szValue, int nDefault, BOOL bWrite=FALSE);
#endif
	BOOL CheckVoltages();
	void CheckVoltage(float fV, float fVmin, float fVmax, LPCTSTR szValue, DWORD dwFlag, BOOL &bTresholdFailureReported);
	BOOL CheckTemperatures();
	void CheckTemperature(int nTemp, int nMaxTemp, int nDevice, LPCTSTR szValue, DWORD dwFlag, int &nLastTemp);
	BOOL CheckFans();
	void CheckFan(int nRpm, int nMinRpm, int nDevice, LPCTSTR szValue, DWORD dwFlag, int &nLastRpm);
	void UpdateNames();
	void RemoveToolTip(LPCTSTR szValue);
	void UpdateToolTip();
	CECBase* CreateMonitorObject();
protected:

	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CHealthControlDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSettings();
	afx_msg void OnUpdateSettings(CCmdUI *pCmdUI);
	afx_msg void OnAbout();
	afx_msg void OnUpdateAbout(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedCkStandAlone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

protected:
	HICON m_hIcon;
	HICON m_hNotHealthy;
	CStringArray m_saToolTip;
	int			 m_nLastToolTips;
private:

	CECBase*	m_pECMon;
	BOOL	m_bChanged;
	BOOL m_bSendFanRTE;
	int	m_nFan1RpmThreshold;
	int	m_nFan2RpmThreshold;
	int	m_nFan3RpmThreshold;
	int	m_nFan1Device;
	int	m_nFan2Device;
	int	m_nFan3Device;
	int m_nLastFan1RPM;
	int m_nLastFan2RPM;
	int m_nLastFan3RPM;

	BOOL m_bSendTemperatureRTE;
	int m_nTemperature1Threshold;
	int m_nTemperature2Threshold;
	int m_nTemperature3Threshold;
	int m_nTemperature1Device;
	int m_nTemperature2Device;
	int m_nTemperature3Device;
    int m_nLastTemperature1;
	int m_nLastTemperature2;
	int m_nLastTemperature3;

	BOOL  m_bSendVoltageRTE;
	float m_fVCoreAMinThreshold;
	float m_fVCoreAMaxThreshold;

	float m_fVCoreBMinThreshold;
	float m_fVCoreBMaxThreshold;

	float m_f12VMinThreshold;
	float m_f12VMaxThreshold;

	float m_f5VMinThreshold;
	float m_f5VMaxThreshold;

	float m_f3_3VMinThreshold;
	float m_f3_3VMaxThreshold;

	float m_fN12VMinThreshold;
	float m_fN12VMaxThreshold;

	float m_fN5VMinThreshold;
	float m_fN5VMaxThreshold;

	BOOL m_bVCoreATresholdFailureReported;
	BOOL m_bVCoreBTresholdFailureReported;
	BOOL m_b3_3VTresholdFailureReported;
	BOOL m_b5VTresholdFailureReported;
	BOOL m_bN5VTresholdFailureReported;
	BOOL m_b12VTresholdFailureReported;
	BOOL m_bN12VTresholdFailureReported;
};
