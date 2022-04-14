#pragma once

#include "processlist.h"
#include "SVPage.h"
#include "afxcmn.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
class CProcess;
class CInputList;

// CUploadPage dialog

class CUploadPage : public CSVPage
{
	DECLARE_DYNAMIC(CUploadPage)

public:
	CUploadPage(CSVView* pParent);   // standard constructor
	virtual ~CUploadPage();

// Dialog Data
	enum { IDD = IDD_PROZ_UPLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	// Implementation
public:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();
	virtual BOOL OnInitDialog();
	virtual BOOL IsDataValid();

protected:
	void EnableExceptNew();
	void InitProcessList();
	void FillProcesses();
	int	 InsertProcess(CProcess* pProcess);
	void SetListItemText(int i, CProcess* pProcess);
	void SelectProcessItem(int i);
	void ControlToProcess();
	void ProcessToControl();

private:
	CInputList*			m_pInputList;
	CProcessList*		m_pProcessList;
	CProcess*			m_pSelectedProcess;
	int					m_iSelectedProcessItem;

public:
	CListCtrl m_listProcesses;
	CEdit m_editName;
	CEdit m_editTime;
	CComboBox m_comboResolution;
	CComboBox m_comboCompression;
	CEdit m_editServer;
	CEdit m_editLogin;
	CEdit m_editPassword;
	CEdit m_editDuration;
	CButton m_radioHold;
	CButton m_radioForever;
	DWORD   m_dwRecTime;
	DWORD   m_dwMainTime;
	afx_msg void OnLvnItemchangedListProcesses(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditName();
	afx_msg void OnEnChangeRectime();
	afx_msg void OnCbnSelchangeResolution();
	afx_msg void OnCbnSelchangeCompression();
	afx_msg void OnEnChangeEditServer();
	afx_msg void OnEnChangeEditLogin();
	afx_msg void OnEnChangeEditPassword();
	afx_msg void OnBnClickedSaveHold();
	afx_msg void OnBnClickedSaveForever();
	afx_msg void OnEnChangeSaveMaintime();
	CEdit m_editFilename;
	afx_msg void OnEnChangeEditFilename();
};
