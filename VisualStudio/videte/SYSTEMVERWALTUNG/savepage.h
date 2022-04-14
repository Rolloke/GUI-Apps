/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: savepage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/savepage.h $
// CHECKIN:		$Date: 9.08.06 21:02 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 9.08.06 20:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CSavePage_H
#define _CSavePage_H

#include "processlist.h"
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
#define MAX_SELECTED_ARCHIVES	15

/////////////////////////////////////////////////////////////////////////////
class CProcess;
class CInputList;

/////////////////////////////////////////////////////////////////////////////
// CSavePage dialog
class CSavePage : public CSVPage
{
	DECLARE_DYNAMIC(CSavePage)

// Construction
public:
	CSavePage(CSVView* pParent);   // standard constructor
	~CSavePage();

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSavePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();

protected:
	// Generated message map functions
	//{{AFX_MSG(CSavePage)
	afx_msg void OnInterval();
	afx_msg void OnChangeRectime();
	afx_msg void OnChangeNr();
	afx_msg void OnChangeMaintime();
	afx_msg void OnForever();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListproc();
	afx_msg void OnSelchangeCompression();
	afx_msg void OnSelchangeResolution();
	afx_msg void OnChangeName();
	afx_msg void OnHold();
	afx_msg void OnChangePausetime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	ProcessToControl(CProcess* pProcess);
	void	ControlToProcess(CProcess* pProcess);

	void	ShowHide();
	BOOL	IsDataValid();
	void	ClearListBoxProcess();
	void	FillListBoxProcess();
// Dialog Data
protected:
	//{{AFX_DATA(CSavePage)
	enum { IDD = IDD_PROZ_SAVE };
	CComboBox	m_cbResolution;
	CComboBox	m_cbCompression;
	CButton	m_btnInterval;
	CButton	m_btnHold;
	CEdit	m_editName;
	CListBox	m_lbList;
	CStatic	m_sTxtNotForever;
	CStatic	m_sTxtSec0;
	CEdit	m_editRecTime;
	CEdit	m_editNr;
	CStatic	m_sTxtRecTime;
	CStatic	m_sTxtNr;
	CStatic	m_sTxtPause;
	CEdit	m_editPauseTime;
	CEdit	m_editMainTime;
	CStatic	m_sSec2;
	CStatic	m_sSec1;
	CButton	m_btnIntervalGrp;
	CButton	m_btnMainTime;
	CButton	m_btnForever;
	BOOL	m_bHold;
	BOOL	m_bInterval;
	CString	m_sName;
	BOOL	m_bForever;
	int		m_iCompression;
	int		m_iResolution;
	CString	m_sRecTime;
	CString	m_sMainTime;
	CString	m_sPauseTime;
	CString	m_sNrPict;
	//}}AFX_DATA
	CInputList*			m_pInputList;
	CProcessList*		m_pProcessList;
	CArchivInfoArray*	m_pArchivInfoArray;

	CProcess*	m_pSelectedProcess;
	int			m_iSelectedItem;

	BOOL		m_bShowCompression;
};

#endif
