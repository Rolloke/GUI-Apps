// SVPage.h : header file
//
#ifndef SVPage_H
#define SVPage_H

#include "SVDoc.h"
#include "SVView.h"

class CStatusList;

#define SVP_CO_CHECK        0
#define SVP_CO_CHECK_ONLY   1
#define SVP_CO_CHECK_SILENT 2
/////////////////////////////////////////////////////////////////////////////
// CSVPage dialog

class CSVPage : public CWK_Dialog
{
	DECLARE_DYNCREATE(CSVPage)

	// Construction
public:
	CSVPage(UINT nIDTemplate = 0);
	virtual ~CSVPage();


	// attributes
public:
	CWK_Profile& GetProfile();
	CWK_Dongle&  GetDongle();
	inline BOOL IsLocal();

	// operations
public:
	virtual void SetModified(BOOL bModified = TRUE ,BOOL bServerInit = TRUE);
			BOOL IsModified();
			void DisableAll();
			void ActivateTooltips(BOOL bActivate);
			void SetSelections(int);

			BOOL CheckActivationsWithOutput(CSecID outID, int nIDMsg, int nCheckOption=SVP_CO_CHECK);
			BOOL CheckActivationsWithInputGroup(CSecID idGroup, int nCheckOption=SVP_CO_CHECK);

public:
	virtual void OnItemChangedStatusList(CStatusList* pStatusList, int nListItem);
	virtual void OnStatusChangedStatusList(int nDlgItemID, int nListItem);

public:
	virtual void Initialize();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	virtual void OnNew();
	virtual void OnDelete();
	
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual int  GetNoOfSubDlgs();
	virtual CWK_Dialog* GetSubDlg(int);

	virtual void OnIdleSVPage();

public:	
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVPage)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int  OnToolHitTest(CPoint, TOOLINFO* ) const;
	
	void	SetPageSize(CScrollBar&wndScrollBar, int iSize);
	int		GetPageSize(CScrollBar&wndScrollBar);
	void	OnVScroll(CScrollBar&wndScrollBar, UINT nSBCode, UINT nPos, int nSize, int&iScroll);

	void	DeleteProcessesOfExtensionCards(CProcessList* pProcessList, 
											COutputList* pOutputList,
											CString sGroupName);
	void	SaveInputsAndOutputsAfterCheckingActivations();

protected:
	CSVView*		m_pParent;

private:
	BOOL			m_bModified;
public:
	virtual BOOL OnInitDialog();
};

inline CSVPage::IsModified()
{
	return m_bModified;
}
inline BOOL CSVPage::IsLocal()
{
	return m_pParent->GetDocument()->IsLocal();
}

#endif
