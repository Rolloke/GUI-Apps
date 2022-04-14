#pragma once

#include "EdtSubItemListCtrl.h"

// CSDIAlarmNumberDialog dialog
class CEdtSubItemListCtrl;
class CInputGroup;

class CSDIAlarmNumberDialog : public CWK_Dialog
{
	DECLARE_DYNAMIC(CSDIAlarmNumberDialog)

public:
	CSDIAlarmNumberDialog(SDIControlType eType, CSDIAlarmNumbers* pAlarmNumbers, CInputGroup* pInputGroup);   // standard constructor
	virtual ~CSDIAlarmNumberDialog();

// Dialog Data
	enum { IDD = IDD_SDI_ALARM_NUMBERS };

protected:
    //{{AFX_VIRTUAL(CSDIAlarmNumberDialog)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	void InitList();
	void SaveList();

    //{{AFX_MSG(CSDIAlarmNumberDialog)
	afx_msg void OnBnClickedSdiBtnNew();
	afx_msg void OnBnClickedSdiBtnDelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemActivateSdiListAlarmNo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSdiBtnSave();
	afx_msg void OnBnClickedSdiBtnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CEdtSubItemListCtrl*	m_pList;
	SDIControlType			m_eType;
public:
	CSDIAlarmNumbers*		m_pAlarmNumbers;
	CInputGroup*			m_pInputGroup;
};
