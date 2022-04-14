/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNUnitDlg.h $
// CHECKIN:		$Date: 30.09.05 14:43 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 30.09.05 14:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef IsdnUnitDlg_H
#define IsdnUnitDlg_H

#include "resource.h"
#include "SecID.h"

#define MAX_LEN_RUFNUMMER	31

/////////////////////////////////////////////////////////////////////////////
// CISDNUnitDlg dialog
class CISDNUnitDlg : public CDialog
{
// Construction
public:
	CISDNUnitDlg(CISDNConnection* pConnection, CWnd* pParent = NULL);	// standard constructor

	inline int		GetIDD() const;

// Implementation
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISDNUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CISDNUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg void OnAdvancedSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(CISDNUnitDlg)
	enum { IDD = IDD_ISDN_SETTINGS };
	CEdit	m_ctBChannels;
	CButton	m_ctAdvancedSettings;
	CString	m_sRufnummer;
	BYTE	m_byBChannels;
	BOOL	m_bTraceSend;
	BOOL	m_bTraceReceive;
	int		m_processorUsage;
	BOOL	m_bEnableKnockBox;
	//}}AFX_DATA
	CString				m_sCodedRufnummer;
};


/////////////////////////////////////////////////////////////////////////////
inline int CISDNUnitDlg::GetIDD() const
{
	return IDD;
}
/////////////////////////////////////////////////////////////////////////////
#endif // IsdnUnitDlg_H
