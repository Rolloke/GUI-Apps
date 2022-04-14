#pragma once
#include "afxwin.h"

// CSDIStarInterfaceDlg dialog

class CSDIStarInterfaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSDIStarInterfaceDlg)

public:
	CSDIStarInterfaceDlg(SDIControlType eType,
						int iCom,
						CSDIProtocolArray* pProtocolArray,
						CSDIAlarmTypeArray* pAlarmTypeArray,
						CComParameters* pComParameters,
						CSVView* pView);   // standard constructor
	virtual ~CSDIStarInterfaceDlg();

// Dialog Data
	enum { IDD = IDD_SDI_STAR_INTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	void    InitListBox();
	void    FillListBox();
	int     AddToList(CSDIStarDevice* pDevice);
	void    EnableExceptNew();
	void    SelectItem(int i);

public:
	CComboBox m_ctrlID;
	CComboBox m_ctrlType;
	CListCtrl m_ctrlControls;

	CSDIStarDevices m_Devices;

private:
	int m_iSelectedItem;

public:
	virtual BOOL OnInitDialog();
	CEdit m_editName;
	CButton m_btnNew;
	CButton m_btnDelete;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnEnChangeEditName();
	afx_msg void OnCbnSelchangeComboId();
	afx_msg void OnCbnSelchangeComboTyp();
	afx_msg void OnLvnItemchangedListControls(NMHDR *pNMHDR, LRESULT *pResult);
};
