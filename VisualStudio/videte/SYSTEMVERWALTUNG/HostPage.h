/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: HostPage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/HostPage.h $
// CHECKIN:		$Date: 31.08.05 15:53 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 31.08.05 15:45 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
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
	afx_msg void OnHostHasPin();
	afx_msg void OnChangeHostPin();
	afx_msg void OnChangeEditDisconnect();
	afx_msg void OnHdnItemclickListHost(NMHDR *pNMHDR, LRESULT *pResult);
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

	BOOL	CheckProcessesWithHost(CSecID hostID);
	BOOL	CheckDeleteArchivesForHost(CHost* pHost);
	BOOL	CheckNewArchivesForHost(CHost* pHost);

	CString	TrimLeadingZerosIP(const CString& s);

	 int   CheckArchiveForHost(CArchivInfoArray& ai, 
									 int r, 
									 CHost* pHost,
									 DWORD dwAvailableMB);
	 int   AskForAutomaticArchive(CArchivInfoArray& ai,
									    int r,  
										UINT nQuestion,  
										UINT nExtension,  
										CHost* pHost,  
										CSecID& id,
										DWORD dwAvailableMB);   
	static int CALLBACK HostCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
// Dialog Data
	//{{AFX_DATA(CHostPage)
	enum { IDD = IDD_HOSTLIST };
	CEdit	m_editDisconnect;
	CEdit	m_editPIN;
	CButton	m_btnPINRequired;
	CButton	m_btnHostArchives;
	CEdit	m_editNumber;
	CEdit	m_editName;
	CComboBox	m_cbTyp;
	CListCtrl	m_Hosts;
	CString	m_sName;
	CString	m_sNumber;
	CString	m_sStationsName;
	BOOL	m_bPINRequired;
	CString	m_sPIN;
	UINT	m_dwDisconnect;
	//}}AFX_DATA

	CProcessList*		m_pProcessList;
	BOOL				m_bProcessesChanged;
	CArchivInfoArray*	m_pArchivInfoArray;
	BOOL				m_bArchivesChanged;
	CInputList*			m_pInputList;
	BOOL				m_bInputsChanged;
	CIPCDrives*			m_pDrives;

	CHostArray* m_pHostArray;
	CHost*		m_pSelectedHost;
	int			m_iSelectedItem;
	int			m_iSortColumn;
	int			m_iMaxHosts;

	int			m_iISDNIndex;
	int			m_iTCPIPIndex;
	int			m_iVCSIndex;
	int			m_iPresIndex;
	int			m_iSMSIndex;
	int			m_iEMailIndex;
	int			m_iFAXIndex;
	int			m_iTOBSIndex;

	BOOL		m_bISDN;
	BOOL		m_bTCPIP;
	BOOL		m_bVCS;
	BOOL		m_bPres;
	BOOL		m_bSMS;
	BOOL		m_bEMail;
	BOOL		m_bFAX;
	BOOL		m_bTOBS;

	BOOL		m_bPINAccessable;
};
#endif
/////////////////////////////////////////////////////////////////////////////
// end of $Workfile: HostPage.h $
