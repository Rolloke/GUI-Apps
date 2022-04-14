#if !defined(AFX_AUDIO_CHANNEL_DIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_AUDIO_CHANNEL_DIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InputDialog.h : header file
//

class CAudioChannelPage;
class CAudioChannel;

/////////////////////////////////////////////////////////////////////////////
// CAudioChannelDialog dialog

class CAudioChannelDialog : public CWK_Dialog
{
// Construction
public:
	CAudioChannelDialog(CAudioChannelPage* pPage);

// Dialog Data
	//{{AFX_DATA(CAudioChannelDialog)
	enum { IDD = IDD_AUDIO_CHANNEL };
	CStatic	m_StaticTypeAudio;
	CSpinButtonCtrl	m_SpinTypeAudio;
	CString	m_sName;
	BOOL	m_bActive;
	BOOL  m_bDefault;
	//}}AFX_DATA
	int   m_nNo;
	BOOL  m_bDrawLine;
	int   m_nEditCtrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioChannelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	virtual BOOL StretchElements();

// Implementation
protected:
	void CreateDlg();
	void SetChannel(CAudioChannel* pChannel, bool &bLeft, bool &bNext, int &nCount);
	void ChannelToControl();
	BOOL ControlToChannel();
	void ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CAudioChannelDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnChangeName();
	afx_msg void OnAucRdDefault();
	afx_msg void OnChangeEditTypeAudio();
	//}}AFX_MSG
//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:
	CAudioChannelPage*	m_pChannelPage;
	CAudioChannel*			m_pChannel;
	CString              m_sStereoChannel;
	int                  m_nStereoChannel;
   bool        m_bSetPos;
   static int gm_nList[3];
	friend class CAudioChannelPage;
};

WK_PTR_ARRAY(CAudioChannelDialogArray,CAudioChannelDialog*);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // AFX_AUDIO_CHANNEL_DIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_
