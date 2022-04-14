/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuUnitDlg.h $
// CHECKIN:		$Date: 17.01.01 9:28 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 17.01.01 9:21 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitDlg dialog

class CAku;
class CIPCInputAkuUnit;
class CIPCOutputAkuUnit;

class CAkuUnitDlg : public CDialog
{
// Construction
public:
	CAkuUnitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAkuUnitDlg)
	enum { IDD = IDD_AKUUNIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAkuUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAkuUnitDlg)
	virtual BOOL OnInitDialog();
	virtual BOOL OnQueryOpen();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg long OnWmAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);

	void OnAbout();

private:
	CAku*	m_pAku;
	CIPCInputAkuUnit*		m_pInput;	// Pointer auf CIPC-Input 
	CIPCOutputAkuUnit*		m_pOutput;	// Pointer auf CIPC-Output
	BOOL					m_bOk;
};
