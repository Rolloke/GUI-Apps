#pragma once
#include "ViewDlg.h"


// CDlgSubstitute dialog

class CDlgSubstitute : public CDlgInView
{
	DECLARE_DYNAMIC(CDlgSubstitute)

public:
	CDlgSubstitute(CWnd* pParent);   // standard constructor

	void SetImageList(CImageList*);
	void SetShowID(UINT nShowID) { m_nShowID = nShowID;};
// Dialog Data
	enum  { IDD = IDD_SUBSTITUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL StretchElements();

	afx_msg void OnBnClickedShowAncestor();
	DECLARE_MESSAGE_MAP()
protected:
	CSkinButton m_btnShowAncestor;
	UINT m_nShowID;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
