/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DlgDatabaseInfo.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/DlgDatabaseInfo.h $
// CHECKIN:		$Date: 10.06.04 13:25 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 10.06.04 10:53 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CDlgDatabaseInfo_H__
#define __CDlgDatabaseInfo_H__

class CServer;

/////////////////////////////////////////////////////////////////////////////
// CDlgDatabaseInfo dialog
class CDlgDatabaseInfo : public CSkinDialog
{
// Construction
public:
	CDlgDatabaseInfo(CWnd* pParent = NULL, CServer* pServer = NULL);   // standard constructor
	~CDlgDatabaseInfo();

// Dialog Data
	//{{AFX_DATA(CDlgDatabaseInfo)
	enum  { IDD = IDD_DATABASE_INFO };
	CSkinListCtrlX	m_ArchivList;
	//}}AFX_DATA

public:
	inline	CServer*	GetServer();
			void		FillArchivList();

public:
	virtual BOOL StretchElements() {return TRUE;}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDatabaseInfo)
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
	//{{AFX_MSG(CDlgDatabaseInfo)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList m_Images;
};
inline CServer* CDlgDatabaseInfo::GetServer()
{
	return m_pServer;	
}

typedef CDlgDatabaseInfo *CDlgDatabaseInfoPtr;

WK_PTR_ARRAY(CDlgDatabaseInfoArray, CDlgDatabaseInfoPtr);

#endif
