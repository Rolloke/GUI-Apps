// DVSTestDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_DVSTESTDLG_H__C13369D7_8673_11D5_9A29_004005A19028__INCLUDED_)
#define AFX_DVSTESTDLG_H__C13369D7_8673_11D5_9A29_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCDatabaseTest;
/////////////////////////////////////////////////////////////////////////////
// CDVSTestDlg dialog

class CDVSTestDlg : public CWK_Dialog
{
// Construction
public:
	CDVSTestDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDVSTestDlg();

// Dialog Data
	//{{AFX_DATA(CDVSTestDlg)
	enum { IDD = IDD_DVSTEST_DIALOG };
	CButton	m_ctrlCheckOrder;
	CButton	m_btnDisconnect;
	CButton	m_btnConnect;
	CButton	m_btnShow;
	CButton	m_btnDelete;
	CButton	m_btnCalc;
	CButton	m_btnBrowse;
	CButton	m_btnBackup;
	CButton	m_btnAlarmList;
	CEdit	m_editPIN;
	CListCtrl	m_Records;
	CComboBox	m_Hosts;
	CDateTimeCtrl	m_StartTime;
	CDateTimeCtrl	m_StartDate;
	CDateTimeCtrl	m_EndTime;
	CDateTimeCtrl	m_EndDate;
	CListCtrl	m_ListSequences;
	CListCtrl	m_ListArchives;
	CString	m_sVolume;
	CString	m_sPIN;
	CString	m_sRecords;
	CString	m_sPath;
	CString	m_sCopied;
	CString	m_sError;
	BOOL	m_bShowMD;
	BOOL	m_bShowFPS;
	CString m_sTimeDiff;

	CSystemTime m_stMaxDiffOverAll;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVSTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	void CheckPics_RequestPictures();

protected:
	virtual BOOL StretchElements();
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDVSTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnItemchangedListArchives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCalc();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonShow();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonBackup();
	afx_msg void OnAlarmList();
	afx_msg void OnItemchangedListSequences(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnCheckPics();
	afx_msg void OnCheckmd();
	afx_msg void OnCheckFPS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillArchives();
	void FillSequences(WORD wArchiveNr);
	void DeleteSequences(WORD wArchiveNr);
	void DeleteSequence(WORD wArchiveNr, WORD wSequenceNr);
	void FillRecords(WORD wArchiveNr, WORD wSequenceNr);
	void Enable();

	void CheckOrder_CheckRecords(WORD wArchiveNr, WORD wSequenceNr);
protected:
	CIPCDatabaseTest* m_pCIPCDatabaseTest;
	DWORD m_dwCalcTick;
	BOOL  m_bGotAlarmList;
	CDWordArray m_dwaCheckPicsSequences;
	BOOL m_bCheckPics;
	CSystemTime m_stLastRecord;
	DWORD	m_dwWholePics;
private:
	CString m_sChecking;
	int		m_iCountSecondsForFPS;
	int		m_iLastSecond;
	float	m_flEverageFPS_CurrSequence;
	CString m_sFPSEverage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTESTDLG_H__C13369D7_8673_11D5_9A29_004005A19028__INCLUDED_)
