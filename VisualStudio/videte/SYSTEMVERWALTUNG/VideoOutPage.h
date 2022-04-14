/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: VideoOutPage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/VideoOutPage.h $
// CHECKIN:		$Date: 29.06.05 7:25 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 29.06.05 6:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_VIDEOOUTPAGE_H__5265FC06_0F7F_4B40_8D3C_DD68C85B37B8__INCLUDED_)
#define AFX_VIDEOOUTPAGE_H__5265FC06_0F7F_4B40_8D3C_DD68C85B37B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusList.h"
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoOutPage dialog
class CVideoOutPage : public CSVPage
{
	DECLARE_DYNAMIC(CVideoOutPage)

// Construction
public:
	CVideoOutPage(CSVView* pParent);
	~CVideoOutPage();

// Attributes
	inline COutputList*	GetOutputList() const {return m_pOutputList;}

// Overrides
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
protected:
	virtual BOOL StretchElements();

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();
	virtual void OnItemChangedStatusList(CStatusList* pStatusList, int nListItem);

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVideoOutPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	static int CALLBACK MyCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	// Generated message map functions
	//{{AFX_MSG(CVideoOutPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeVideoOutEditCameras();
	afx_msg void OnChangeVideoOutEditDwelltime();
	afx_msg void OnChangeVideoOutEditName();
	afx_msg void OnCheckAllCameras();
	afx_msg void OnColumnclickVoutListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	InitListControls();
	void    EnableExceptNew();

	void	FillProcesses();
	void	FillTypes();
	void	FillExtensionCards();
	void	FillOutputs();
	int		InsertProcess(CProcess* pProcess);
	void	SelectProcessItem(int i);
	void	ProcessToControl();
	void	ControlToProcess();
	void	OnItemChangedTypesList(int nListItem);
	void	OnItemChangedExtensionCardList(int nListItem);
	void	OnItemChangedOutputsList(int nListItem);
	void	SetCamerasString();
	void	SetDwellTime();
	void	SelectProcessItemByProcess(CProcess* pProcess);

// Dialog Data
protected:
	//{{AFX_DATA(CVideoOutPage)
	enum { IDD = IDD_PROZ_VIDEO_OUT };
	CButton	m_btnAllCameras;
	CStatic	m_txtType;
	CStatic	m_txtOutput;
	CStatic	m_txtName;
	CStatic	m_txtExtensionCard;
	CStatic	m_txtDwellTime;
	CStatic	m_txtCameras;
	CEdit	m_editDwellTime;
	CEdit	m_editCameras;
	CEdit	m_editName;
	CListCtrl	m_listProcesses;
	CStatusList	m_listTypes;
	CStatusList	m_listExtensionCard;
	CStatusList	m_listOutputs;
	CString	m_sName;
	CString	m_sCameras;
	DWORD	m_dwDwellTime;
	BOOL	m_bAllCameras;
	//}}AFX_DATA

	CProcessList*	m_pProcessList;
	COutputList*	m_pOutputList;
	QuadControlType m_eQuadType;

	CProcess*		m_pSelectedProcess;
	int				m_iSelectedProcessItem;
	CBitmap			m_headerArrowBitmap;
	int				m_iSelectedColumn;
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_VIDEOOUTPAGE_H__5265FC06_0F7F_4B40_8D3C_DD68C85B37B8__INCLUDED_)
/////////////////////////////////////////////////////////////////////////////
// end of $WORKFILE:$
