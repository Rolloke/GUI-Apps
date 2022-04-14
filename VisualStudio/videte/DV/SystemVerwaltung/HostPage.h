/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: HostPage.h $
// ARCHIVE:		$Archive: /DV/SystemVerwaltung/HostPage.h $
// CHECKIN:		$Date: 30.08.02 12:40 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 30.08.02 12:35 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef HostPage_H
#define HostPage_H

#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
class CHost;
class CHostArray;

//#define _HOST_VCS
//#define _HOST_PRES
/////////////////////////////////////////////////////////////////////////////
// CHostPage dialog

class CHostPage : public CSVPage
{
	DECLARE_DYNAMIC(CHostPage)

// Construction
public:
	CHostPage(CSVView* pParent);
	~CHostPage();

// Dialog Data
	//{{AFX_DATA(CHostPage)
	enum { IDD = IDD_HOSTLIST };
	CButton	m_btnHostArchives;
	CEdit	m_editNumber;
	CEdit	m_editName;
	CComboBox	m_cbTyp;
	CListCtrl	m_Hosts;
	CString	m_Name;
	CString	m_Number;
	CString	m_sStationsName;
	//}}AFX_DATA

public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHostPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHostPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnChangeNumber();
	afx_msg void OnItemchangedListHost(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeStationsName();
	afx_msg void OnSelchangeComboTyp();
	afx_msg void OnClickListHost(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListHost(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListHost(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListHost(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHostArchives();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

private:
	int		InsertItem(CHost* pHost, BOOL bSelectIt = FALSE);
	void	ClearHostListBox();
	void	FillHostListBox();
	void    EnableExceptNew();
	void    SelectItem(int i);

	BOOL   CheckDeleteArchivesForHost(CHost* pHost);
	BOOL   CheckNewArchivesForHost(CHost* pHost);

	CString	TrimLeadingZerosIP(const CString& s);

	static int   CheckArchiveForHost(CArchivInfoArray& ai, 
									 int r, 
									 CHost* pHost,
									 DWORD dwAvailableMB);
	static int   AskForAutomaticArchive(CArchivInfoArray& ai,
									    int r,  
										UINT nQuestion,  
										UINT nExtension,  
										CHost* pHost,  
										CSecID& id,
										DWORD dwAvailableMB);   

private:
	CArchivInfoArray*	m_pArchivInfoArray;
	CIPCDrives*			m_pDrives;
	CHostArray* m_pHostArray;
	CHost*		m_pSelectedHost;
	int			m_iSelectedItem;

	int			m_iISDNIndex;
	int			m_iTCPIPIndex;

	BOOL		m_bISDN;
	BOOL		m_bTCPIP;
};
#endif
/////////////////////////////////////////////////////////////////////////////
// end of $Workfile: HostPage.h $
