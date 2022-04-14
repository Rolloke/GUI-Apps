// AudioTestDlg.h : header file
//

#if !defined(AFX_AUDIOTESTDLG_H__6BE298E6_9A8E_4B65_9A2C_86F0F99C8F62__INCLUDED_)
#define AFX_AUDIOTESTDLG_H__6BE298E6_9A8E_4B65_9A2C_86F0F99C8F62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Streams.h>
#include "resource.h"
#include "common\Keyprovider.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDlg dialog
struct ErrorStruct;

class CAudioTestDlg : public CDialog
{
// Construction
public:
	CAudioTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAudioTestDlg)
	enum { IDD = IDD_AUDIOTEST_DIALOG };
	CComboBox	m_cInputChannels;
	CComboBox	m_cRenderer;
	CComboBox	m_cCompressor;
	CComboBox	m_cCapture;
	BOOL	m_bTest;
	int		m_nCapture;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	void InitGraphBuilder();
	void ReleaseFilters();
   void ReportError(ErrorStruct*, bool bMsgBox=true);
	void ReleaseGraphFilters(BOOL bCaptureToo=TRUE);
	void ResetInputChannels();

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAudioTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBtnTest();
	afx_msg void OnSelchangeComboCaptureDev();
	afx_msg void OnSelchangeComboInputChannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IGraphBuilder	*m_pGB;
   ICaptureGraphBuilder2 *m_pCaptureGB;
	IBaseFilter		*m_pCapture;
   IMediaControl  *m_pMC;
   IMediaEventEx  *m_pME;
	CKeyProvider    m_Prov;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOTESTDLG_H__6BE298E6_9A8E_4B65_9A2C_86F0F99C8F62__INCLUDED_)
