#pragma once

class COScopeCtrl;
// CDlgStatistic dialog

class CDlgStatistic : public CDialog
{
	DECLARE_DYNAMIC(CDlgStatistic)

public:
	CDlgStatistic(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStatistic();
	void AddBitRate(double dSent, double dReceived, int iSendQueue);

// Dialog Data
	enum { IDD = IDD_STATISTIC };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

public:
	COScopeCtrl *m_pScopeCtrl;
	int			m_nTimeStep;
protected:
};
