// SVPage.h : header file
//
#ifndef SVPage_H
#define SVPage_H

#include "SVDoc.h"
#include "SVView.h"
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

	// operations
public:
	virtual void SetModified(BOOL bModified = TRUE ,BOOL bServerInit = TRUE);
			BOOL IsModified();
			void DisableAll();
			void ActivateTooltips(BOOL bActivate);

public:
	virtual void OnItemChangedStatusList(int nDlgItemID, int nListItem);

public:
	virtual void Initialize();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	virtual void OnNew();
	virtual void OnDelete();
	
	virtual BOOL CanNew();
	virtual BOOL CanDelete();

public:	
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:
	CSVView*		m_pParent;
private:
	BOOL			m_bModified;
};

inline CSVPage::IsModified()
{
	return m_bModified;
}


#endif
