/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: Settings.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/Settings.h $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

class CCoCo;
class CMegra;

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

class CSettings : public CDialog
{
// Construction
public:
	CSettings(CCoCo* pCoCo, CMegra* pMegra, CWnd* pParent = NULL);
	CSettings::~CSettings();
// Dialog Data
	//{{AFX_DATA(CSettings)
	enum { IDD = IDD_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Dump( CDumpContext &dc ) const;

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnAcquisition();
	afx_msg void OnDelays();
	afx_msg void OnLuminance();
	afx_msg void OnPosition();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCoCo			*m_pCoCo;
	CMegra			*m_pMegra;
	static  BOOL	 m_bOK;
};
