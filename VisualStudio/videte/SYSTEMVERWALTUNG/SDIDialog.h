#if !defined(AFX_SDIDIALOG_H__2A5EB2A2_2040_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_SDIDIALOG_H__2A5EB2A2_2040_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SDIDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "wkclasses\SDIControl.h"
#include "SDIAlarmType.h"

enum eSdiInterface
{
	SDI_INTERFACE_COM = 0,
	SDI_INTERFACE_UDP = 1,
	SDI_INTERFACE_TCP = 2
};
/////////////////////////////////////////////////////////////////////////////
class CSVView;
class CSDIPage;
class CSDIProtocolArray;
class CComParameters;
class CAlarmTypeArray;
class CInputGroup;
class CSDIAlarmNumbers;
class CInput;
class CInputList;

/////////////////////////////////////////////////////////////////////////////
// CSDIDialog dialog
class CSDIDialog : public CWK_Dialog
{
// Construction
public:
	CSDIDialog(CSVView* pView, CSDIPage* pPage, int iPortNr, eSdiInterface nInterface=SDI_INTERFACE_COM);   // standard constructor
	virtual ~CSDIDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	virtual BOOL StretchElements();

// Implementation
public:
	BOOL	IsDataValid();
	void	SaveChanges();
	void	CancelChanges();
	void	SetModified();
	void	SetLocationNo(int nLocNo);
	void	SwitchOn(BOOL bOn);

	inline eSdiInterface	GetInterface();
	inline int				GetPortNo();
	inline BOOL				IsOn();

protected:
	void	InitProtocolListbox();
	void	EnableControls();
	void	ClearData();
	void	ProtocolChanged();
	
	void	SetDefaultValues();

	static void	SetSpecialInputNames(SDIControlType eProtocol,
									 CInputGroup* pInputGroup,
									 CSDIAlarmNumbers& alarmNumbers,
									 CSDIProtocolArray* pSDIProtocolArray);
	static void	SetSpecialInputNamesNCR(CInputGroup* pInputGroup);
	static void	SetSpecialInputNamesSNI(CInputGroup* pInputGroup);
	static void	SetSpecialInputNamesGeneric(CInputGroup* pInputGroup, 
											CSDIAlarmNumbers& alarmNumbers,
											CSDIProtocolArray* pSDIProtocolArray);
	static void SetSpecialInputNamesStarInterface(CInputGroup* pInputGroup);

	static BOOL	IsEqual(SDIControlType eProtocol,
						CInput* pInput,
						CSDIAlarmNumbers& alarmNumbers,
						int iAlarmNumberIndex);
	static void UpdateInputs(SDIControlType eProtocol,
							 CInputGroup* pInputGroup,
							 CSDIAlarmNumbers& alarmNumbers);
	static void	AddDatabaseFields(CIPCFields* pDatabaseFields,SDIControlType eType);
public:
	static CInputGroup* SaveChangesInputGroup(SDIControlType& eInitialProtocol,
										SDIControlType eProtocol,
										CInputList* pInputList,
										CIPCFields* pDatabaseFields,
										CSDIAlarmNumbers& alarmNumbers,
										CSDIProtocolArray* pCSDIProtocolArray,
										const CString& sShmName, 
										const CString& sGroupName,
										const CString& sInitialComment,
										int iNrOfInputs);
protected:
	static void	SaveChangesComment(CInputGroup* pInputGroup, 
								   CString sInitialComment,
								   CString sNewGroupName);

	void	DeleteDatabaseFields(SDIControlType eType);
	void	SetAlarmTypes(CInputGroup* pInputGroup);

	CString GetShmName();

	// Generated message map functions
	//{{AFX_MSG(CSDIDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCheckOn();
	afx_msg void OnButtonConfigurateCom();
	afx_msg void OnSelchangeComboProtocol();
	afx_msg void OnChangeEdit();
	afx_msg void OnButtonConfigurateProtocol();
	afx_msg void OnSelchangeComboCurrency();
	afx_msg void OnEditchangeComboCurrency();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// Dialog Data
protected:
	//{{AFX_DATA(CSDIDialog)
	enum { IDD = IDD_SDI_ONE };
	CComboBox	m_cbCurrency;
	CStatic	m_staticPort;
	CButton	m_btnConfigurateCom;
	CComboBox	m_cbProtocol;
	CEdit	m_editComment;
	CButton	m_btnConfigurateProtocol;
	BOOL	m_bOn;
	CString	m_sPort;
	int		m_iIndexCBProtocol;
	CString	m_sComment;
	CString	m_sCurrency;
	//}}AFX_DATA

private:
	CSVView*					m_pView;
	CSDIPage*					m_pPage;
	int							m_iPortNr;
	int							m_iInitialPortNo;
	SDIControlType				m_eProtocol;
	SDIControlType				m_eInitialProtocol;
	CString						m_sInitialComment;
	CString						m_sSection;
	CSDIProtocolArray*			m_pCSDIProtocolArray;
	CComParameters*				m_pComParameters;
	CSDIAlarmTypeArray			m_AlarmTypeArray;
	CSDIStarDevices				m_StarDevices;
	eSdiInterface				m_nInterface;
	CString						m_sIPaddress;
	CString						m_sSubnetMask;
	CSDIAlarmNumbers			m_AlarmNumbers;
};
WK_PTR_ARRAY(CSDIDialogArray,CSDIDialog*);

/////////////////////////////////////////////////////////////////////////////
inline eSdiInterface CSDIDialog::GetInterface()
{
	return m_nInterface;
}
/////////////////////////////////////////////////////////////////////////////
inline int	CSDIDialog::GetPortNo()
{
	return m_iPortNr;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIDialog::IsOn()
{
	return m_bOn;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDIDIALOG_H__2A5EB2A2_2040_11D1_93E0_00C095ECA33E__INCLUDED_)
