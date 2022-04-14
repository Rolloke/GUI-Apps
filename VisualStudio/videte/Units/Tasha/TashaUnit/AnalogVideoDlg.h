/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: AnalogVideoDlg.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/AnalogVideoDlg.h $
// CHECKIN:		$Date: 10.03.04 13:27 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 10.03.04 13:24 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ANALOGVIDEODLG_H__F877CEF1_0394_4634_8EAA_460D19482987__INCLUDED_)
#define AFX_ANALOGVIDEODLG_H__F877CEF1_0394_4634_8EAA_460D19482987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnalogVideoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalogVideoDlg dialog
class CCodec;
class CAnalogVideoDlg : public CDialog
{
// Construction
public:
	CAnalogVideoDlg(CWnd* pParent = NULL, CCodec* pCodec = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnalogVideoDlg)
	enum { IDD = IDD_ANALOG_VIDEO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalogVideoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalogVideoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnCrosspoint00();
	afx_msg void OnCrosspoint10();
	afx_msg void OnCrosspoint20();
	afx_msg void OnCrosspoint30();
	afx_msg void OnCrosspoint40();
	afx_msg void OnCrosspoint50();
	afx_msg void OnCrosspoint60();
	afx_msg void OnCrosspoint70();
	afx_msg void OnCrosspoint01();
	afx_msg void OnCrosspoint11();
	afx_msg void OnCrosspoint21();
	afx_msg void OnCrosspoint31();
	afx_msg void OnCrosspoint41();
	afx_msg void OnCrosspoint51();
	afx_msg void OnCrosspoint61();
	afx_msg void OnCrosspoint71();
	afx_msg void OnCrosspoint02();
	afx_msg void OnCrosspoint12();
	afx_msg void OnCrosspoint22();
	afx_msg void OnCrosspoint32();
	afx_msg void OnCrosspoint42();
	afx_msg void OnCrosspoint52();
	afx_msg void OnCrosspoint62();
	afx_msg void OnCrosspoint72();
	afx_msg void OnCrosspoint03();
	afx_msg void OnCrosspoint13();
	afx_msg void OnCrosspoint23();
	afx_msg void OnCrosspoint33();
	afx_msg void OnCrosspoint43();
	afx_msg void OnCrosspoint53();
	afx_msg void OnCrosspoint63();
	afx_msg void OnCrosspoint73();
	afx_msg void OnTermination0();
	afx_msg void OnTermination1();
	afx_msg void OnTermination2();
	afx_msg void OnTermination3();
	afx_msg void OnTermination4();
	afx_msg void OnTermination5();
	afx_msg void OnTermination6();
	afx_msg void OnTermination7();
	afx_msg void OnVideoEnable();
	afx_msg void OnCrosspointEnable();
	afx_msg long OnUpdate(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void ShowConfigDlg(HWND hWndParent);
	void HideConfigDlg();
	BOOL IsVisible(){return m_bVisible;}
	BOOL OnConfirmSetAnalogOut(DWORD dwCrosspointMask);
	BOOL OnConfirmGetAnalogOut(DWORD dwCrosspointMask);
	BOOL OnConfirmSetTerminationState(BYTE byTermMask);
	BOOL OnConfirmGetTerminationState(BYTE byTermMask);
	BOOL OnConfirmSetVideoEnableState(BOOL bState);
	BOOL OnConfirmGetVideoEnableState(BOOL bState);
	BOOL OnConfirmSetCrosspointEnableState(BOOL bState);
	BOOL OnConfirmGetCrosspointEnableState(BOOL bState);


private:
	void DoRequestSetAnalogOut(int nIn, int nOut);
	void DoRequestSetTerminationState(int nPort);
	void UpdateDlg();

private:
	CCodec* m_pCodec;
	BOOL	m_bVisible;

	DWORD	m_dwCrossPointMask;
	BYTE	m_byTermMask;
	BOOL	m_bVideoEnable;
	BOOL	m_bCrosspointEnable;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALOGVIDEODLG_H__F877CEF1_0394_4634_8EAA_460D19482987__INCLUDED_)
