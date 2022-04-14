#if !defined(AFX_QUERYDIALOG_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_)
#define AFX_QUERYDIALOG_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryDialog.h : header file
//

class CObjectView;

#include "QueryFieldDialog.h"
#include "QueryFieldCharDialog.h"
#include "QueryFieldDateDialog.h"
#include "QueryFieldNumDialog.h"
#include "QueryFieldTimeDialog.h"
#include "RechercheView.h"
/////////////////////////////////////////////////////////////////////////////
// CQueryDialog dialog

class CQueryDialog : public CWK_Dialog
{
// Construction
public:
	CQueryDialog(CObjectView* pParent = NULL);   // standard constructor
	virtual ~CQueryDialog();

// Dialog Data
	//{{AFX_DATA(CQueryDialog)
	enum { IDD = IDD_QUERY };
	CButton	m_ctrlAll;
	CButton	m_ctrlSearchArea;
	CButton	m_ctrlCheckTime;
	CButton	m_ctrlCheckDate;
	CButton	m_ctrlCheckTimeFrom;
	CStatic	m_ctrlPCDateEndText;
	CStatic	m_ctrlTimeEndText;
	CDateTimeCtrl	m_ctrlTimeEnd;
	CDateTimeCtrl	m_ctrlPCDateEnd;
	CButton	m_ctrlCopyPictures;
	CButton	m_grpPCTime;
	CButton	m_grpFields;
	CButton	m_ctrlOK;
	CScrollBar	m_ScrollBar;
	CDateTimeCtrl	m_ctrlExactTime;
	CDateTimeCtrl	m_ctrlPCDate;
	CTreeCtrl	m_ServerArchives;
	BOOL	m_bAll;
	BOOL	m_bShowPictures;
	BOOL	m_bCopyPictures;
	BOOL	m_bExactDate;
	BOOL	m_bExactTime;
	BOOL	m_bTimeFromTo;
	int		m_iScroll;
	BOOL	m_bSearchArea;
	//}}AFX_DATA

public:
	void InitServerArchives();
	void Disconnect(DWORD dwServerID);
	void ShowHideRectangle(BOOL bShow); 
	// Overrides
	virtual BOOL StretchElements() {return TRUE;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryDialog)
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

private:
	CObjectView* m_pObjectView;
	CIPCFields   m_FieldNames;
	CIPCFields   m_FieldValues;
	CImageList	 m_States;

	int			 m_iServersChecked;

	CQueryFieldDialogs m_QueryFieldDialogs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYDIALOG_H__7FAC1893_A6C6_11D2_B551_004005A19028__INCLUDED_)
