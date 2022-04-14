/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: databaseinfodlg.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/databaseinfodlg.h $
// CHECKIN:		$Date: 27.01.99 15:54 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 27.01.99 15:34 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef __CDataBaseInfoDlg_H__
#define __CDataBaseInfoDlg_H__

// DataBaseInfoDlg.h : header file
//
class CServer;
/////////////////////////////////////////////////////////////////////////////
// CDataBaseInfoDlg dialog

class CDataBaseInfoDlg : public CWK_Dialog
{
// Construction
public:
	CDataBaseInfoDlg(CWnd* pParent = NULL, CServer* pServer = NULL);   // standard constructor
	~CDataBaseInfoDlg();

// Dialog Data
	//{{AFX_DATA(CDataBaseInfoDlg)
	enum { IDD = IDD_DATABASE_INFO };
	CListCtrl	m_ArchivList;
	//}}AFX_DATA

public:
	inline	CServer*	GetServer();
			void		FillArchivList();

public:
	virtual BOOL StretchElements() {return TRUE;}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataBaseInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitializeListView();
	CServer*	m_pServer;
	BOOL		m_bInfosAvailable;

	// Generated message map functions
	//{{AFX_MSG(CDataBaseInfoDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList m_Images;
};
inline CServer* CDataBaseInfoDlg::GetServer()
{
	return m_pServer;	
}

typedef CDataBaseInfoDlg *CDataBaseInfoDlgPtr;

WK_PTR_ARRAY(CDataBaseInfoDlgArray,CDataBaseInfoDlgPtr);

#endif
