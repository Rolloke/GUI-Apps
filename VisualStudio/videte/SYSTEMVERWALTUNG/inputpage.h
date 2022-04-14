/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputpage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputpage.h $
// CHECKIN:		$Date: 10.07.06 16:52 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 10.07.06 16:04 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef INPUTPAGE_H
#define INPUTPAGE_H

#include "input.h"
#include "SecID.h"
#include "SVPage.h"

#include "InputDialog.h"

class CInputGroup;
class CInputList;
class COutputList;
class CProzList;
class CTimerList;
/////////////////////////////////////////////////////////////////////////////
// CInputPage dialog

class CInputPage : public CSVPage
{
	DECLARE_DYNAMIC(CInputPage)

// Construction
public:
	CInputPage(CSVView* pParent, CInputGroup* pInputGroup);
	~CInputPage();

// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInputPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL StretchElements();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual int  GetNoOfSubDlgs();
	virtual CWK_Dialog* GetSubDlg(int);
	virtual BOOL GetToolTip(UINT nID, CString&sText);

// Implementation
public:
	inline CListBox*	GetListBoxInputs() const;

protected:

protected:
	void	Resize();
	void	Reset(BOOL bDirection);
	void	CreateInputDialogs();
	void	ShowHideStuff();

	// Generated message map functions
	//{{AFX_MSG(CInputPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeltaposSpinTypeInput(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	//{{AFX_DATA(CInputPage)
	enum { IDD = IDD_INPUTS };
	CStatic	m_staticTempDeactivate;
	CScrollBar	m_ScrollBar;
	CStatic	m_staticInputNr;
	CStatic	m_staticActivate;
	CStatic	m_staticEdge;
	CStatic	m_staticComment;
	int		m_iScroll;
	//}}AFX_DATA

	CString				m_sGroup;
	CInputList*			m_pInputList;	// Inputliste mit den Daten
	CInputGroup*		m_pInputGroup;			// Aktuell selektierte Inputgruppe
	CSecID				m_idGroup;		// ID der aktuell selektierten Inputgruppe

	CInputDialogArray	m_InputDialogs;
	int					m_iBoard;


	friend class CInputDialog;

private:
	BOOL IsInputAllowed(CInput*);
public:
};

#endif // INPUTPAGE_H
