// DecodeAppDlg.h : header file
//

#if !defined(AFX_DECODEAPPDLG_H__B651B39D_1A18_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_DECODEAPPDLG_H__B651B39D_1A18_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPTDecoder;
class CDecodeWindow;

/////////////////////////////////////////////////////////////////////////////
// CDecodeAppDlg dialog
			  
class CDecodeAppDlg : public CDialog
{
// Construction
public:
	CDecodeAppDlg(CWnd* pParent = NULL);	// standard constructor
	~CDecodeAppDlg();
	BOOL IsValid();
	DWORD FindPSC(DWORD dwIndex, BOOL &bPictureType);
	DWORD FindPEC(DWORD dwIndex);

// Dialog Data
	//{{AFX_DATA(CDecodeAppDlg)
	enum { IDD = IDD_DECODEAPP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecodeAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDecodeAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnLoad();
	afx_msg void OnPlay();
	afx_msg void OnDifferne();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDecodeWindow*	m_pDecodeView;
	CPTDecoder*		m_pDecoder;
	CFile*			m_pFile;
	BOOL			m_bOK;
	BYTE*			m_pBuffer;
	DWORD			m_dwLen;
	BOOL			m_bShowDifferenz;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECODEAPPDLG_H__B651B39D_1A18_11D1_BC01_00A024D29804__INCLUDED_)
