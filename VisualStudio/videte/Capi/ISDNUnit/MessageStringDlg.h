/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MessageStringDlg.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/MessageStringDlg.h $
// CHECKIN:		$Date: 6.05.98 12:05 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 6.05.98 12:03 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#ifndef _MSGSTRINGDLG_H_
#define _MSGSTRINGDLG_H_

/////////////////////////////////////////////////////////////////////////////
// CMessageStringDlg dialog
class CMessageStringDlg : public CDialog
{
// Construction
public:
	CMessageStringDlg(CWnd* pParent = NULL);
	~CMessageStringDlg();

	void Show(LPCSTR pStringMsg=NULL);
	void Show(UINT nMsg);
	void SetText(CString strMsg);
	void SetText(UINT nMsg);
		// Setzt den Text der Dialogbox
	void Hide();

// Dialog Data
protected:
	//{{AFX_DATA(CMessageStringDlg)
	enum { IDD = IDD_MSGBOX_OK };
	CString	m_sMessage;
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageStringDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
private:
	void InstallHideTimer();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMessageStringDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWnd*		m_pWndParent;
	CString		m_strMessage;
};
#endif
/////////////////////////////////////////////////////////////////////////////
// end of $Workfile: MessageStringDlg.h $
