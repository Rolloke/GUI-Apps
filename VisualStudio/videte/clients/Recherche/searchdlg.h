/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: searchdlg.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/searchdlg.h $
// CHECKIN:		$Date: 11.01.99 15:51 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 11.01.99 10:53 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$
/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog
#ifndef __CSearchDlg_H__
#define	__CSearchDlg_H__

#define	ENTRY_NOT_USED	1
#define	ENTRY_USED		2
#define	ENTRY_FALSE		3


class CServer;
class CObjectView;

enum FIELD_TYPE
{
	FT_INVALID=0,
	FT_DATE=1,
	FT_TIME=2,
	FT_NUMBER=3,
};

class CSearchDlg : public CDialog
{
// Construction
public:
	CSearchDlg(CServer* pServer, CObjectView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSearchDlg)
	enum { IDD = IDD_SEARCH };
	CEdit	m_EditTANummer;
	CEdit	m_EditGANummer;
	CEdit	m_EditBlz;
	CEdit	m_EditBetrag;
	CEdit	m_EditKtoNr;
	BOOL	m_bArchiv;
	CButton m_btnArchiv;
	CEdit	m_EditVonZeit;
	CEdit	m_EditBisZeit;
	CEdit	m_EditZeit;
	CEdit	m_EditDatum;
	CCheckListBox	m_lbArchivList;
	CString	m_sBetragText;
	CString	m_sDatumText;
	CString	m_sBlzText;
	CString	m_sGaNrText;
	CString	m_sKtoNrText;
	CString	m_sTaNrText;
	CString	m_sZeitText;
	CString	m_sKameraNrText;
	CString	m_sRecTime;
	CString	m_sRecTimeFrom;
	CString	m_sRecTimeTo;
	CString	m_sDate;
	//}}AFX_DATA

	int		m_DbGesamt;
	int		m_OpenFiles;
	int		m_Archiv;

	BOOL InitListBox();

	char	m_szFoundFile[_MAX_PATH];
	CString	m_sQuery;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void BuildBackupQueryString();
	void InitBackupListBox();
	void InitNormalListBox();

	CString	SearchForArchivNr();

	BOOL	Validate(CWnd* pWnd, FIELD_TYPE ft, CString& sValue, int iLeastLen=0);
	CString	BuildFieldQueryC(const CString& sValue, const CString& sField);
	CString	BuildFieldQueryN(const CString& sValue, const CString& sField);
	void	AddToQueryString(const CString& sField);

	virtual void	StartSearch();

	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnArchiv();
	afx_msg void OnOpenFiles();
	afx_msg void OnDbGesamt();
	afx_msg void OnChangeSearchZeit();
	afx_msg void OnChangeSearchZeitBis();
	afx_msg void OnChangeSearchZeitVon();
	afx_msg void OnChangeSearchDatum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation
protected:
	CString GetCorrectNr(const CString& s, int iLeast);
	CString NoSpace (const CString& s);
	CString OnlyDigits(const CString& s);

private:
	CServer*	 m_pServer;
	CObjectView* m_pParent;
};

#endif
