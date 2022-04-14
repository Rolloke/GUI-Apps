// GemosUnit.h : main header file for the GEMOSUNIT application
//

#if !defined(AFX_GEMOSUNIT_H__B1CFA5E5_4B32_11D3_9978_004005A19028__INCLUDED_)
#define AFX_GEMOSUNIT_H__B1CFA5E5_4B32_11D3_9978_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

static TCHAR szSection[] = _T("GemosUnit");

class CIPCServerControlGemos;
class CIPCGemosToVision;
class CRS232Gemos;
/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp:
// See GemosUnit.cpp for the implementation of this class
//
#define ALARM_LINE	1000
#define RELAY_LINE	3000
#define CAM_LINE	4000

class CGemosUnitApp : public CWinApp
{
	// construction / destruction
public:
	CGemosUnitApp();

	// attributes
public:
	inline BOOL IsResetting() const;
	inline CRS232Gemos* GetRS232Gemos();
		   BOOL IsConnected();
		   BOOL IsDisconnected();

	// operations
public:
	void OnDispatchCommand(WORD wLine, WORD wInput, WORD wState, const CString &sText);
	void DoConfirmLine(WORD wLine, WORD wInput, WORD wState);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGemosUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CGemosUnitApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileReset();
	afx_msg void OnEnableConfirming();
	afx_msg void OnUpdateConfirming(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	void OnDispatchAlarmCommand(WORD wLine, WORD wInput, WORD wState);
	void OnDispatchRelaisCommand(WORD wLine, WORD wInput, WORD wState);
	void OnDispatchCameraCommand(WORD wLine, WORD wInput, WORD wState);
	void Reset();
	void CleanUp();
	void Init();
	void ReadSettings();
	void Connect(WORD wLine, WORD wInput, WORD wState);
	void Disconnect();
	void WriteCommand(WORD wLine, WORD wInput, WORD wState, LPCSTR szText = NULL);

	// Member
private:
	CIPCServerControlGemos* m_pCIPCServerControlGemos;
	CIPCGemosToVision*      m_pCIPCGemosToVision;
	CRS232Gemos* m_pRS232;
	CComParameters m_ComParameters;
	int  m_iCom;
	BOOL m_bResetting;
	WORD m_wHost;
	BOOL m_bGEMOS;				// TRUE -> GEMOS seite
	int	 m_nLastCameraInput;	// Die letzte aktive Kamera. Wichtig in 'Disconnect'
};
extern CGemosUnitApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline BOOL CGemosUnitApp::IsResetting() const
{
	return m_bResetting;
}
/////////////////////////////////////////////////////////////////////////////
inline CRS232Gemos* CGemosUnitApp::GetRS232Gemos()
{
	return m_pRS232;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEMOSUNIT_H__B1CFA5E5_4B32_11D3_9978_004005A19028__INCLUDED_)
