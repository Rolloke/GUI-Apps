#pragma once

#include "SVPage.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"

//////////////////////////////////////////////////////////////////////
class CMaintenancePage : public CSVPage
{
	DECLARE_DYNAMIC(CMaintenancePage)

	// construction / destruction
public:
	CMaintenancePage(CSVView* pParent = NULL);   // standard constructor
	virtual ~CMaintenancePage();

// Dialog Data
	enum { IDD = IDD_MAINTENANCE };

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSettings();
	afx_msg void OnLogin();
	afx_msg void OnBnClickedIntervallOnOff();
	afx_msg void OnBnClickedMaintButtonStartdate();
	afx_msg void OnBnClickedMaintButtonReport();
	afx_msg void OnBnClickedMaintButtonReportExport();
// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void SaveChanges();
	virtual void CancelChanges();

	DECLARE_MESSAGE_MAP()

// functions
private:
	void UpdateDlgItems();
	void ShowLoginGrp(int iShow);
	void ShowDlgItems(int nDlg, BOOL bShow = TRUE, BOOL bEnable = TRUE);	
	void Initialize();
	void GetSettings();
	void SetSettings();
	CString GetHDDInformations();
	CString GetSMARTparameters(int nDrive);

	static DWORD CALLBACK EditStreamCallback( DWORD dwCookie, // application-defined value
											  LPBYTE pbBuff,      // data buffer
											  LONG cb,            // number of bytes to read or write
											  LONG *pcb           // number of bytes transferred
											  );

private:
	CString m_sUserName;
	CString m_sUserPass;
	CString m_sInstallerCompany;
	CString m_sInstallerStreet;
	CString m_sInstallerPostcode;
	CString m_sInstallerCity;
	CString m_sInstallerTelefon;
	CString m_sInstallerEmail;
	CString m_sInstallerCountry;
	CString m_sIntervallStartDate;
	CString m_sIntervall;
	BOOL	m_bIntervallOnOff;

	CStatic m_ctrlLoginError;
	CDateTimeCtrl m_ctrlIntervallDateTime;
	CStatic m_ctrlTextShowMaint;
	CButton m_ctrlCheckShowMaint;
	CStatic m_ctrlGrpSettingsMaint;
	CStatic m_ctrlGrpLogin;
	CStatic m_ctrlLoginTxtUsername;
	CStatic m_ctrlLoginTxtPassword;
	CEdit m_ctrlLoginUsername;
	CEdit m_ctrlLoginPassword;
	CButton m_ctrlLoginButton;
	CString m_sReport;
	CButton m_ctrlSaveReport;
	CFile	m_InstructionFile;
	CRichEditCtrl m_ctrlInstruction;
	BOOL m_bResetHDDTemperatures;
	CStatic m_ctrlSystemTestText;
	CButton m_ctrlSystemTestButton;
public:
	afx_msg void OnBnClickedMaintButtonSystest();

};
