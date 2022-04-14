#if !defined(AFX_SDIDEFAULTMICROLOCKDLG_H__998AF05B_4E43_4C06_B8F8_9C7A1726C85B__INCLUDED_)
#define AFX_SDIDEFAULTMICROLOCKDLG_H__998AF05B_4E43_4C06_B8F8_9C7A1726C85B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SDIDefaultMicrolockDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#define SDI_MICROLOCK_TYPE_SYSTEM1X	0
#define SDI_MICROLOCK_TYPE_SYSTEM1	1

/////////////////////////////////////////////////////////////////////////////
// CSDIDefaultMicrolockDlg dialog
class CSDIDefaultMicrolockDlg : public CDialog
{

// Construction
public:
	CSDIDefaultMicrolockDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	int	GetType();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIDefaultMicrolockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSDIDefaultMicrolockDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
private:
	//{{AFX_DATA(CSDIDefaultMicrolockDlg)
	enum { IDD = IDD_SDI_DEFAULT_MICROLOCK };
	CButton	m_btnSystem1;
	int		m_iType;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SDIDEFAULTMICROLOCKDLG_H__998AF05B_4E43_4C06_B8F8_9C7A1726C85B__INCLUDED_)
