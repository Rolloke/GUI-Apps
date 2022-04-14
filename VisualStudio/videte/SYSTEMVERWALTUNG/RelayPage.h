#if !defined(AFX_RELAYPAGE_H__EB0311A2_1E32_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_RELAYPAGE_H__EB0311A2_1E32_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RelayPage.h : header file
//

#include "SecID.h"
#include "Output.h"
#include "SVPage.h"

#include "RelayDialog.h"


class COutputGroup;
class COutputList;
class CInputList;
/////////////////////////////////////////////////////////////////////////////
// CRelayPage dialog

class CRelayPage : public CSVPage
{
// Construction
public:
	CRelayPage(CSVView* pParent,COutputGroup* pOutputGroup);

// Dialog Data
	//{{AFX_DATA(CRelayPage)
	enum { IDD = IDD_RELAYS };
	CStatic	m_staticOutputNr;
	CScrollBar	m_ScrollBar;
	int		m_iScroll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelayPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overrides
protected:
	virtual BOOL StretchElements();

protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL GetToolTip(UINT nID, CString&sText);

protected:
	void	Resize();
	void	Reset(BOOL bDirection);
	void	CreateOutputDialogs();
	void	ShowHideStuff();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRelayPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeltaposSpinTypeRelay(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString				m_sGroup;
	CSecID				m_idGroup;
	COutputList*		m_pOutputList;	// Outputliste mit den Daten
	COutputGroup*		m_pOutputGroup;	// Aktuell selektierte Outputgruppe
	CInputList*			m_pInputList;	// Inputliste mit den Daten

	CRelayDialogArray	m_RelayDialogs;
	int					m_iBoard;


	friend class CRelayDialog;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELAYPAGE_H__EB0311A2_1E32_11D1_93E0_00C095ECA33E__INCLUDED_)
