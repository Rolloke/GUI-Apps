// SDIConfigurationDialog.h : header file
//

#if !defined(AFX_SDICONFIGURATIONIADOLG_H__920F9085_1703_11D2_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDICONFIGURATIONIADOLG_H__920F9085_1703_11D2_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"

#include "wkclasses\SDIControl.h"
#include "wkclasses\UnhandledException.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIProtocolArray;
class CSDIProtocol;
class CSDIProtocolData;
class CSDIConfigurationRS232;
class CSDIAlarmTypeArray;
class CSVView;
class CSVPage;

#define SDI_SAMPLE_READ_IN		WM_USER + 1

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationDialog dialog
class CSDIConfigurationDialog : public CWK_Dialog
{
// Construction
public:
	CSDIConfigurationDialog(SDIControlType eType,
							int iCom,
							CSDIProtocolArray* pProtocolArray,
							CSDIAlarmTypeArray* pAlarmTypeArray,
							CComParameters* pComParameters,
							CSVView* pView);
	virtual ~CSDIConfigurationDialog();

// Overrides
public:
	virtual	BOOL	StretchElements() {return TRUE;}
	virtual	BOOL	CanNewProtocol() {return TRUE;}
	virtual void	InitListControlSampleData() PURE_VIRTUAL(;)
	virtual	void	EnableControls() PURE_VIRTUAL(;)
	virtual CSDIConfigurationRS232*	CreateRS232() PURE_VIRTUAL(NULL)
	virtual int		InsertSampleData(CString sOneSample) PURE_VIRTUAL(0)
	virtual void	UpdateSampleDataString() PURE_VIRTUAL(;)

	virtual void	SetDefaultValues();
	virtual	void	SetProtocolDefaultValues();
	virtual	void	SetPortraitAlarm();
	virtual	void	SetMoneyAlarm();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigurationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
protected:
	// Generated message map functions
	//{{AFX_MSG(CSDIConfigurationDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListProtocol();
	afx_msg void OnChangeEditName();
	afx_msg void OnRadioProtocolFormat();
	afx_msg void OnRadioReadOrder();
	afx_msg void OnCheckDifferenceTotalLength();
	afx_msg void OnChangeEditDifferenceTotalLength();
	afx_msg void OnCheckDifferenceStringAtPosition();
	afx_msg void OnChangeEditDifferencePositionOfString();
	afx_msg void OnChangeEditDifferenceStringAtPosition();
	afx_msg void OnButtonProtocolNew();
	afx_msg void OnButtonProtocolDelete();
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboDataType();
	afx_msg void OnChangeEditPosition();
	afx_msg void OnChangeEditDataLength();
	afx_msg void OnCheckDataLengthVariable();
	afx_msg void OnChangeEditSeperator();
	afx_msg void OnChangeEditTag();
	afx_msg void OnSelchangeComboDataFormat();
	afx_msg void OnButtonDataNew();
	afx_msg void OnButtonDataDelete();
	afx_msg void OnButtonDataUp();
	afx_msg void OnButtonDataDown();
	afx_msg void OnButtonProtocolUp();
	afx_msg void OnButtonProtocolDown();
	afx_msg void OnButtonSampleTakeover();
	afx_msg void OnButtonSpecialCharactersIdentifyString();
	afx_msg void OnButtonSpecialCharactersSeperator();
	afx_msg void OnButtonDefault();
	afx_msg void OnCheckReadSample();
	afx_msg void OnItemchangedListSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonPortrait();
	afx_msg void OnButtonMoney();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListSample(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnReadInData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	DWORD			GetParamTypeFromDatabaseField(CIPCField* pField);
	const CString	GetUIStringOfDataTypeID(CSecID idParam);
	const CString	GetUIStringOfDataFormat(eSDIDataFormat eFormat);
	void			InitDlgControls();
	void			InitBitmapButtons();
	void			InitListControlData();
	void			FillComboboxDataType();
	void			FillComboboxDataFormat();
	void			EnableControlsBase();
	BOOL			IsProtocolDataValid(BOOL bOnNew=FALSE);
	BOOL			IsProtocolDataValidForMultiple();
	BOOL			IsProtocolDataValidForSingle();
	BOOL			IsDataValid(int iActualData);
	BOOL			IsDataLengthValid(int iActualData);
	void			GetDialogData();
	void			SetDialogData();
	void			LoadFromSDIProtocolData();
	void			ClearData();
	void			FillData();
	int				InsertData(int iIndex, const CSDIProtocolData* pProtocolData);
	void			SelectData(int i, BOOL bSetSel=TRUE);
	void			LoadFromSDIProtocol();
	CString			ChangeSpecialCharactersTokenToAscii(CString& sToken);
	CString			ChangeSpecialCharactersAsciiToToken(const CString& sAscii);
	void			ClearSampleData();
	void			SelectSampleData(int i, BOOL bSetSel=TRUE);
	void			UpdateSampleStringPlaceholder();
	void			MoveProtocol(BOOL bUp);
	void			MoveProtocolData(BOOL bUp);
	void			PrepareSDIUnitForStop();
	void			PrepareSDIUnitForStart();

// Dialog Data
protected:
	//{{AFX_DATA(CSDIConfigurationDialog)
	enum { IDD = IDD_SDI_CONFIGURATION };
	CAnimateCtrl	m_animReadSample;
	CListCtrl	m_lcSample;
	CButton	m_btnAlarmMoney;
	CButton	m_btnAlarmPortrait;
	CButton	m_btnDefault;
	CButton	m_checkReadSample;
	CButton	m_btnSpecialCharactersIdentifyString;
	CEdit	m_editSampleRead;
	CButton	m_btnSampleTakeover;
	CButton	m_btnProtocolDown;
	CButton	m_btnDataDown;
	CListBox	m_lbProtocol;
	CEdit	m_editName;
	CButton	m_btnIdentifyByPosLen;
	CButton	m_btnIdentifyBySeperator;
	CButton	m_btnIdentifyByTag;
	CButton	m_btnReadOrderLeft;
	CButton	m_btnReadOrderRight;
	CButton	m_btnTotalLength;
	CEdit	m_editDifferenceTotalLength;
	CButton	m_btnStringAtPosition;
	CEdit	m_editDifferencePositionOfString;
	CEdit	m_editDifferenceStringAtPosition;
	CButton	m_btnProtocolNew;
	CButton	m_btnProtocolDelete;
	CButton	m_btnProtocolUp;
	CEdit	m_editSample;
	CComboBox	m_cbDataType;
	CEdit	m_editPosition;
	CEdit	m_editDataLength;
	CButton	m_btnDataLengthVariable;
	CEdit	m_editSeperator;
	CButton	m_btnSpecialCharactersSeperator;
	CEdit	m_editTag;
	CComboBox	m_cbDataFormat;
	CButton	m_btnDataNew;
	CButton	m_btnDataDelete;
	CButton	m_btnDataUp;
	CListCtrl	m_lcData;
	BOOL	m_bSampleReadActive;
	CString	m_sSampleRead;
	int		m_iProtocol;
	CString	m_sName;
	int		m_iProtocolFormat;
	int		m_iReadOrder;
	BOOL	m_bDifferenceTotalLength;
	DWORD	m_dwDifferenceTotalLength;
	BOOL	m_bDifferenceStringAtPosition;
	DWORD	m_dwDifferencePosition;
	CString	m_sDifferenceString;
	CString	m_sSample;
	int		m_iDataType;
	DWORD	m_dwPosition;
	DWORD	m_dwDataLength;
	BOOL	m_bDataLengthVariable;
	CString	m_sSeperator;
	CString	m_sTag;
	int		m_iDataFormat;
	//}}AFX_DATA
protected:
	CSVView*				m_pView;
	CSVPage*				m_pPage;
	CSDIProtocolArray*		m_pCSDIProtocolArray;
	CSDIProtocol*			m_pCSDIProtocol;
	CSDIProtocolData*		m_pCSDIProtocolData;
	SDIControlType			m_eType;
	int						m_iCom;
	int						m_iSelectedItemData;
	int						m_iSelectedItemSample;
/*   
	HBITMAP					m_hUp;
	HBITMAP					m_hDown;
*/
	CSDIConfigurationRS232* m_pRS232;
	CSDIAlarmTypeArray*		m_pAlarmTypeArray;
	CComParameters*			m_pComParameters;
	CIPCFields*				m_pDatabaseFields;
private:
	BOOL	m_bSDIUnitStart;
	CRect	m_rcEditSampleRead;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGURATIONIADOLG_H__920F9085_1703_11D2_9F29_0000C036AC0D__INCLUDED_)
