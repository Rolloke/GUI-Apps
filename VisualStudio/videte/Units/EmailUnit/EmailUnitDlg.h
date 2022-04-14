// EmailUnitDlg.h : header file
//

#if !defined(AFX_EMAILUNITDLG_H__662471B7_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_EMAILUNITDLG_H__662471B7_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#include "EmailClient.h"	// Added by ClassView
#include "DialUpNetwork.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEmailClient;
class CDialUpNetwork;
class CIPCServerControlEmail;
class CIPCOutputEmail;
class CMPEG4Decoder;
/////////////////////////////////////////////////////////////////////////////
// CEmailUnitDlg dialog

class CEmailUnitDlg : public CDialog
{

public:

	CEmailUnitDlg(CWnd* pParent = NULL);	// standard constructor
	~CEmailUnitDlg();

	CString GetLocalTime();
	BOOL OnQueryOpen();
	CDialUpNetwork* GetDialUpNetwork();
	void ChangeIcon(int color);

	CEvent			   m_ePictureHasArrived;
	CConnectionRecords m_ConnectionRecords;
// Dialog Data
	//{{AFX_DATA(CEmailUnitDlg)
	enum { IDD = IDD_EMAILUNIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmailUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	
protected:
   
	// Generated message map functions
	//{{AFX_MSG(CEmailUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAbout();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	afx_msg long OnNewEmail(WPARAM wParam, LPARAM lParam);
	afx_msg long OnPicture(WPARAM wParam, LPARAM lParam);
	afx_msg long OnCocoDecoded(WPARAM wParam, LPARAM lParam);
	afx_msg	long OnDialUp(WPARAM wParam, LPARAM lParam);
	afx_msg	long OnMailSend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:

	CString m_sSourceHost;
	CString m_sJobTime;
	CString m_sCameraName;

	HICON m_hIcon;
	static UINT	m_uRasDialEventMsg;

	CDialUpNetwork *m_pDialUpNetwork;
	CIPCServerControlEmail* m_pCIPCServerControlEmail;
	CIPCOutputEmail* m_pCIPCOutputEmail;
	CEmailClient *m_pEmailClient;
	CEmail* m_pEMail;
	CH263*	m_pH263Decoder;
	CMPEG4Decoder	*m_pMp4Decoder;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILUNITDLG_H__662471B7_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
