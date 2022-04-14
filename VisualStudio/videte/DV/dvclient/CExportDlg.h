#if !defined(AFX_CEXPORTDLG_H__65CACA53_3DDC_11D4_8F12_004005A11E32__INCLUDED_)
#define AFX_CEXPORTDLG_H__65CACA53_3DDC_11D4_8F12_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

class CPanel;
class CDisplay;
class CExportDlg  : public CTransparentDialog
{
private:
	typedef enum
	{				
		InputNo, 
		InputExportTimeFrom,
		InputExportTimeTo,
	} 
	INP_STATE;
// Construction
public:
	void GetExportData(CString& sStart, CString &sEnd, DWORD &dwCamMask);
	void GetExportData(CTime &TimeFrom, CTime &TimeTo, DWORD &dwCamMask);
	
	CExportDlg(CDisplay* pDisplay, CPanel* pPanel = NULL);  

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_EXPORT };
	CFrame		m_ctrlBorder;
	CDisplay	m_ctrlDisplayTo;
	CDisplay	m_ctrlDisplayFrom;
	CDisplay	m_ctrlStaticExport;
	CSkinButton	m_ctrlButtonTimer;
	CSkinButton	m_ctrlButtonOk;
	CSkinButton	m_ctrlButtonCancel;
	CSkinButton m_ctrlButtonCam9;
	CSkinButton	m_ctrlButtonCam8;
	CSkinButton	m_ctrlButtonCam7;
	CSkinButton	m_ctrlButtonCam6;
	CSkinButton	m_ctrlButtonCam4;
	CSkinButton	m_ctrlButtonCam5;
	CSkinButton	m_ctrlButtonCam3;
	CSkinButton	m_ctrlButtonCam2;
	CSkinButton	m_ctrlButtonCam16;
	CSkinButton	m_ctrlButtonCam15;
	CSkinButton	m_ctrlButtonCam14;
	CSkinButton	m_ctrlButtonCam13;
	CSkinButton	m_ctrlButtonCam12;
	CSkinButton	m_ctrlButtonCam11;
	CSkinButton	m_ctrlButtonCam10;
	CSkinButton	m_ctrlButtonCam1;
	CSkinButton	m_ctrlButtonNum9;
	CSkinButton	m_ctrlButtonNum8;
	CSkinButton	m_ctrlButtonNum7;
	CSkinButton	m_ctrlButtonNum6;
	CSkinButton	m_ctrlButtonNum5;
	CSkinButton	m_ctrlButtonNum4;
	CSkinButton	m_ctrlButtonNum3;
	CSkinButton	m_ctrlButtonNum2;
	CSkinButton	m_ctrlButtonNum1;
	CSkinButton	m_ctrlButtonNum0;
	CSkinButton	m_ctrlButtonClear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton0();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg void OnButtonClear();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnInputClock();
	afx_msg void OnCheckExportCam();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnButtonPressed(const CString &sChar);
	BOOL ValidateDateTime(CString &sInput, CTime &time);

private:
	CDisplay*		m_pDisplay;
	CPanel*			m_pPanel;
	HCURSOR			m_hCursor;
	CSkinButtons	m_CamButtons;
	CSkinButtons	m_NumButtons;
	COLORREF		m_BaseColor;
	CString			m_sInput;
	CString			m_sFormat;
	CString			m_sStartTime;
	CString			m_sEndTime;
	CImageList		m_ilOK;
	CImageList		m_ilCancel;
	CImageList		m_ilTimer;
	CTime			m_TimeFrom;
	CTime			m_TimeTo;
	INP_STATE		m_eInputState;
	DWORD			m_dwCamMask;
	BOOL			m_bDDMMYY;
};
	
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEXPORTDLG_H__65CACA53_3DDC_11D4_8F12_004005A11E32__INCLUDED_)
