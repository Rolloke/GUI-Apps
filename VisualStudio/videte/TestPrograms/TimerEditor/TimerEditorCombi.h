#if !defined(AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerEditorCombi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi dialog

class CSecTimer;
class CSecTimerArray;

class CTimerEditorDlg;

class CTimerEditorCombi : public CDialog
{
// Construction
public:
	CTimerEditorCombi(CTimerEditorDlg* pParent);   // standard constructor

	void FillFromTimer(CSecTimer *pTimer,const CSecTimerArray & timers);


// Dialog Data
	//{{AFX_DATA(CTimerEditorCombi)
	enum { IDD = IDD_TIMER_COMBI_EDITOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorCombi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerEditorCombi)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CTimerEditorDlg* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
