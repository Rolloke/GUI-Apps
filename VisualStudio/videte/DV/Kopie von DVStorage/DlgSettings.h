#if !defined(__DVSTORAGE_CDLGSETTINGS_INCLUDED__)
#define __DVSTORAGE_CDLGSETTINGS_INCLUDED__


// CDlgSettings dialog
class COScopeCtrl;

class CDlgSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgSettings)

public:
	CDlgSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSettings();

// Dialog Data
public:
	enum { IDD = IDD_SETTINGS };
protected:
	UINT	m_uTimer;
	COScopeCtrl *m_pScopeCtrlStore;
	COScopeCtrl *m_pScopeCtrlFps;
	CWK_Average<DWORD> m_dwAvgStore;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStorageApp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDlgSettings)
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnFastReset();
	afx_msg void OnBnClickedBtnClearOutput();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnCopyOutput();
	afx_msg void OnBnClickedRdDebugTrace();
	afx_msg void OnBnClickedRdDiagram();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
#endif //__DVSTORAGE_CDLGSETTINGS_INCLUDED__