#pragma once

#include "SVPage.h"
#include "afxwin.h"
#include "EdtSubItemListCtrl.h"


// CCameraParameterPage dialog

/////////////////////////////////////////////////////////////////////////////
// : implement DoDataExchange for data validation
class CIPCameraList : public CEdtSubItemListCtrl
{
	friend class CCameraParameterPage;
public:
	CIPCameraList();
	virtual ~CIPCameraList();

	int InsertColumn(LPCTSTR pszName, int nWidth);
protected:
	//{{AFX_VIRTUAL(CIPCameraList)
	virtual void DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam);
	virtual BOOL CanEditSubItem(int nItem, int nSubItem);
	virtual eEditType GetEditType(int nItem, int nSubItem);
	virtual void InitComboBox(CSubItemCombo*pCombo, const CString& sText);
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CIPCameraList)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL	m_bSizeEnabled;
private:
	int*	m_pnWidth;
public:
	CCameraParameterPage* m_pPage;
};


class CIPCOutputIPcamUnit;


class CCameraParameterPage : public CSVPage
{
	friend class CIPCameraList;
	DECLARE_DYNAMIC(CCameraParameterPage)

public:
	CCameraParameterPage(CSVView* pParent = NULL);   // standard constructor
	virtual ~CCameraParameterPage();

	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();
protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();
	virtual BOOL GetToolTip(UINT nID, CString&sText);
	
// Implementierung
public:
protected:
	void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	void OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	void InitCaptureDeviceList();
	void InitStrings(const CString&sCsv, CComboBox&combo, BOOL bSelect=TRUE);
	void RequestParameterSet(CSecID id);
	void ControlUnit(BOOL);
	void CreateList();
	BOOL UpdateParameter();
	void EmptyControls();
	BOOL FindOutput(int nOutput, BOOL bCamera);
	BOOL FindInput(int nOutput, BOOL bMD);
	BOOL CanEditListItem(int nItem, int nSubItem);
	void SaveUserAndPassword();

// Dialog Data
protected:
	//{{AFX_DATA(CCameraParameterPage)
	enum { IDD = IDD_IP_CAMERA};
	CComboBox	m_cTypes;
	CStringArray m_saTypes;
	CStringArray m_saUrls;
	CComboBox	m_cCaptureDevices;
	CComboBox	m_cRotations;
	CComboBox	m_cCompression;
	CComboBox	m_cURL;
	CStatic		m_cActivating;
	CIPCameraList m_cIPItems;
	BOOL		m_bUnitActive;
	CString		m_sUrl;
	UINT		m_uPort;
	CString		m_sDeviceType;
	CString		m_sName;
	CString		m_sCameras;
	CString		m_sInputs;
	CString		m_sRelais;
	CString		m_sCompression;
	CString		m_sRotation;
	CString		m_sUserName;
	CString		m_sPassword;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(CCameraParameterPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCameraParameterPage)
	afx_msg void OnCbnSelchangeComboCaptureDevices();
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnSetName();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnCbnEditchangeComboCaptureDevices();
	afx_msg void OnEnChangeEdtPortOrUrl();
	afx_msg void OnCbnSelchangeComboUrl();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedCkIpCamUnit();
	afx_msg void OnBnClickCkIpItem(UINT nID);
	afx_msg void OnCbnEditchangeComboIpUrl();
	afx_msg void OnLvnItemActivateListIpItems(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnIpPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CIPCOutputIPcamUnit*m_pUnitControl;
	int m_nCurrentCaptureDevice;
	UINT    m_nRequestUnitTimer;
	BOOL	m_bCanNew;
public:
};
