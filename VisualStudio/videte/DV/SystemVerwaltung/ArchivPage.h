/* GlobalReplace: CDVRDrive --> CIPCDrive */
#if !defined(AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_)
#define AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchivPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CArchivPage dialog

class CArchivPage : public CSVPage
{
// Construction
public:
	CArchivPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArchivPage)
	enum { IDD = IDD_ARCHIV };
	CEdit	m_editSize;
	CEdit	m_editName;
	CListCtrl	m_Archivs;
	CString	m_sName;
	UINT	m_dwSize;
	CString	m_sMax;
	CString	m_sUsed;
	CString	m_sWhole;
	//}}AFX_DATA

// Overrides
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchivPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchivPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnChangeEditSize();
	afx_msg void OnClickArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickArchive(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkArchive(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();
			CString GetTypString(ArchivType at);
			int	 GetTypImage(ArchivType at);
			void EnableExceptNew();
			void SelectItem(int i);
			void FillArchivListBox();
			int	 InsertItem(CArchivInfo* pAI, BOOL bSelectIt = FALSE);
			void ClearArchivListBox();
			void SelectedItemToControl();
			void OnChange();
			void CopyActualSettingsToNew(CArchivInfo* pAI);
			void UpdateSizeInfo();

private:
	CArchivInfoArray* m_pArchivs;
	CIPCDrives*		  m_pDrives;
	CHostArray*		  m_pHostArray;

	int				  m_iSelectedItem;
	CArchivInfo*	  m_pSelectedArchivInfo;
	CImageList		  m_Images;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVPAGE_H__4505D623_7575_11D2_B4EA_00C095EC9EFA__INCLUDED_)
