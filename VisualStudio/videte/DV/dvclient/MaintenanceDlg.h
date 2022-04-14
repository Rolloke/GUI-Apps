#pragma once

class CDisplay;
class CSkinButton;
class CFrame;

#define MAX_SYREPORT_LINES  21

#define SECTION_MAINTENANCE			_T("Maintenance")	
#define MAINT_INST_COMPANY			_T("Company")	
#define MAINT_INST_STREET			_T("Street")	
#define MAINT_INST_POSTCODE			_T("Postcode")	
#define MAINT_INST_CITY				_T("City")	
#define MAINT_INST_TELEFON			_T("Telefon")
#define MAINT_INST_EMAIL			_T("Email")	
#define MAINT_INST_COUNTRY			_T("Country")	
#define MAINT_INTERVALL				_T("Intervall")	
#define MAINT_INTERVALL_ONOFF		_T("IntervallOnOff")
#define MAINT_INTERVALL_STARTDATE	_T("IntervallStartDate")

// CMaintenanceDlg

class CMaintenanceDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CMaintenanceDlg)

public:
	CMaintenanceDlg(CPanel* pParent = NULL);
	virtual ~CMaintenanceDlg();

	// Dialog Data
	//{{AFX_DATA(CMaintenanceDlg)
	enum { IDD = IDD_MAINTENANCE_DLG };
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaintenanceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL StretchElements();
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMaintenanceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSettings();
	afx_msg void OnLogin();
	afx_msg void OnBnClickedMaintButtonOk();
	afx_msg void OnBnClickedMaintButtonSave();
	afx_msg void OnBnClickedIntervallOnOff();
	afx_msg void OnBnClickedMaintButtonStartdate();
	afx_msg void OnBnClickedMaintButtonReport();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HCURSOR	m_hCursor;
	CPanel*	m_pPanel;
	void	Initialize();
	void	UpdateDlgItems();
	void	ShowLoginGrp(int iShow);
	void	ShowDlgItems(int nDlg, BOOL bShow = TRUE, BOOL bEnable = TRUE);
	CString GetHDDInformations();
	CString GetSMARTparameters(int nDrive);
	CString GetOEMVersion();
	void	GetSettings();
	void	SetSettings();
	CString InitSysReport();
	void	InitDlgForLoginOnly(); //only show login fields in a small dialog 
	void	ResizeDlgToOriginSize();
	static DWORD CALLBACK EditStreamCallback( DWORD dwCookie, // application-defined value
											  LPBYTE pbBuff,      // data buffer
											  LONG cb,            // number of bytes to read or write
											  LONG *pcb           // number of bytes transferred
											  );

private:
	COLORREF	m_BaseColor;

	CString m_sUserName;
	CString m_sUserPass;
	CString m_sInstallerCompany;
	CString m_sInstallerStreet;
	CString m_sInstallerPostcode;
	CString m_sInstallerCity;
	CString m_sInstallerTelefon;
	CString m_sInstallerEmail;
	CString m_sInstallerCountry;
	CString m_sIntervall;
	BOOL	m_bIntervallOnOff;

	CSkinStatic m_ctrlLoginError;
	CSkinDateTimeCtrl m_ctrlIntervallDateTime;
	CString m_sIntervallStartDate;
	CSkinStatic m_ctrlTextShowMaint;
	CSkinButton m_ctrlCheckShowMaint;
	CComboBox m_ctrlComboIntervall;
	CSkinGroupBox m_ctrlGrpSettingsMaint;
	CSkinGroupBox m_ctrlGrpSystemCheck;
	CSkinGroupBox m_ctrlGrpSettingsInstaller;
	CSkinGroupBox m_ctrlGrpSystemReport;
	CSkinGroupBox m_ctrlGrpLogin;
	CSkinStatic m_ctrlLoginTxtUsername;
	CSkinStatic m_ctrlLoginTxtPassword;
	CSkinEdit m_ctrlLoginUsername;
	CSkinEdit m_ctrlLoginPassword;
	CSkinButton m_ctrlLoginButton;
	CString m_sReport;
	CFile	m_InstructionFile;
	CRichEditCtrl m_ctrlInstruction;
	BOOL m_bResetHDDTemperatures;
	CSkinStatic m_ctrlSystemTestText;
	CSkinButton m_ctrlOK;
	CSkinButton m_ctrlSave;
	CImageList	m_imagelistOK;
	CImageList	m_imagelistSave;

	BOOL	m_bResize;
	CRect m_Rect_CtrlOK_OrigPosition;
};


