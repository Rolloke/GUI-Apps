/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DlgPTZVideo.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/DlgPTZVideo.h $Date: 16.08.96 12:49 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 4.10.05 14:01 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _DlgPTZVideo_H
#define _DlgPTZVideo_H

class CWndLive;
class CIPCInputIdipClient;
class CIPCOutputIdipClient;

/////////////////////////////////////////////////////////////////////////////
// CDlgPTZVideo dialog

class CDlgPTZVideo : public CSkinDialog
{
// Construction
public:
	CDlgPTZVideo(	CIPCInputIdipClient* pInput,CIPCOutputIdipClient* pOutput,
					CameraControlType ccType, DWORD dwPTZFunctionsEx,
					CWnd* pParent = NULL);   // standard constructor
	~CDlgPTZVideo();
	DECLARE_DYNAMIC(CDlgPTZVideo)

// Dialog Data
	//{{AFX_DATA(CDlgPTZVideo)
	enum  { IDD = IDD_VIDEO_PTZ };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPTZVideo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

public:
	void SetPTZType(CameraControlType ccType);
	void SetPTZFunctionsEx(DWORD dwPTZ_FktEx);
	void SetFunctionName(LPTSTR pszName);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPTZVideo)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClicked(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	CWndLive* m_pWndLive;
	CIPCInputIdipClient*	m_pInput; 
	CIPCOutputIdipClient*	m_pOutput; 

	CSecID					m_commID;
	CSecID					m_camID;

	CameraControlType		m_ccType;
	DWORD					m_dwPTZFunctionsEx;
};
/////////////////////////////////////////////////////////////////////////////
#endif
