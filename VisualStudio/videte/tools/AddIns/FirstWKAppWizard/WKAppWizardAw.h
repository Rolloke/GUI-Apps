#if !defined(AFX_WKAPPWIZARDAW_H__764816EE_3034_11D2_B916_00C095ECA33E__INCLUDED_)
#define AFX_WKAPPWIZARDAW_H__764816EE_3034_11D2_B916_00C095ECA33E__INCLUDED_

// WKAppWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see WKAppWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CWKAppWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

protected:
	CDialogChooser* m_pChooser;
};

// This declares the one instance of the CWKAppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global WKAppWizardaw.  (Its definition is in WKAppWizardaw.cpp.)
extern CWKAppWizardAppWiz WKAppWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WKAPPWIZARDAW_H__764816EE_3034_11D2_B916_00C095ECA33E__INCLUDED_)
