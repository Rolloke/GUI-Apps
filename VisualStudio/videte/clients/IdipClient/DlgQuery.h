#if !defined(AFX_DLGQUERY_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_)
#define AFX_DLGQUERY_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQuery.h : header file
//

class CViewArchive;

#include "DlgQueryField.h"
#include "DlgQueryFieldChar.h"
#include "DlgQueryFieldDate.h"
#include "DlgQueryFieldNum.h"
#include "DlgQueryFieldTime.h"
#include "ViewIdipClient.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgQuery dialog

class CDlgQuery : public CSkinDialog
{
// Construction
public:
	CDlgQuery(CViewArchive* pParent = NULL);   // standard constructor
	virtual ~CDlgQuery();

public:
	void InitServerArchives();
	void Disconnect(DWORD dwServerID);
	void ShowHideRectangle(BOOL bShow); 
	// Overrides
	virtual BOOL StretchElements() {return TRUE;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQuery)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgQuery)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckAll();
	afx_msg void OnClickServerArchives(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckDate();
	afx_msg void OnCheckExactTime();
	afx_msg void OnCheckTimeFrom();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGrpFields();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void InitFields();

	void Parameters2QueryParameter();
	void StartSearch();
	void ShowHide();
	void ShowHideServerArchives(BOOL bShow);
	void Resize();

	void	SetPageSize(int iSize);
	int		GetPageSize();

	void CheckDateTime();  //checks start date/time and end date/time

	// Dialog Data
private:
	//{{AFX_DATA(CDlgQuery)
	enum  { IDD = IDD_QUERY };
	CSkinButton	m_ctrlAll;
	CSkinButton	m_ctrlSearchArea;
	CSkinButton	m_ctrlCheckTime;
	CSkinButton	m_ctrlCheckDate;
	CSkinButton	m_ctrlCheckTimeFrom;
	CSkinStatic	m_ctrlPCDateEndText;
	CSkinStatic	m_ctrlTimeEndText;
	CSkinDateTimeCtrl	m_ctrlTimeEnd;
	CSkinDateTimeCtrl	m_ctrlPCDateEnd;
	CSkinButton	m_ctrlCopyPictures;
	CSkinGroupBox	m_grpPCTime;
	CSkinGroupBox	m_grpFields;
	CSkinButton	m_ctrlOK;
	CScrollBar	m_ScrollBar;
	CSkinDateTimeCtrl	m_ctrlExactTime;
	CSkinDateTimeCtrl	m_ctrlPCDate;
	CSkinTree	m_ServerArchives;
	BOOL	m_bAll;
	BOOL	m_bShowPictures;
	BOOL	m_bCopyPictures;
	BOOL	m_bExactDate;
	BOOL	m_bExactTime;
	BOOL	m_bTimeFromTo;
	int		m_iScroll;
	BOOL	m_bSearchArea;
	//}}AFX_DATA
	CViewArchive* m_pViewArchive;
	CIPCFields   m_FieldNames;
	CIPCFields   m_FieldValues;
	CImageList	 m_States;

	int			 m_iServersChecked;

	CDlgQueryFields m_QueryFieldDialogs;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERY_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_)
