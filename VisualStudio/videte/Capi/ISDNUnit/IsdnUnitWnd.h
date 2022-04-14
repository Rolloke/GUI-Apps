/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: IsdnUnitWnd.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/IsdnUnitWnd.h $
// CHECKIN:		$Date: 3.03.06 11:46 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 3.03.06 11:19 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef IsdnUnitWnd_H
#define IsdnUnitWnd_H

/////////////////////////////////////////////////////////////////////////////
class CIsdnUnitWnd : public CWnd
{
public:
	CIsdnUnitWnd();

// Attributes
public:
// Operations

//	BOOL	Login();	// Disabled because only used by OnSettings

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIsdnUnitWnd)
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIsdnUnitWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void	UpdateIcon(BOOL bEnabled);

// Generated message map functions
protected:
	//{{AFX_MSG(CIsdnUnitWnd)
	afx_msg void OnDestroy();
	afx_msg void OnAbout();
	afx_msg void OnAppExit();
	afx_msg void OnCloseAllIsdnConnections();
	afx_msg void OnUpdateCloseAllIsdnConnections(CCmdUI* pCmdUI);
	afx_msg void OnCloseOneIsdnChannel();
	afx_msg void OnUpdateCloseOneIsdnChannel(CCmdUI* pCmdUI);
	afx_msg void OnCloseModul();
	afx_msg void OnClose();
	afx_msg void OnShowState();
	afx_msg void OnDevModeChange(LPTSTR lpDeviceName);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnISDN_B_Channels(WPARAM wParam, LPARAM lParam);
	afx_msg long OnCapiGetMessageError(WPARAM wParam, LPARAM lParam);
	afx_msg long OnUpdateIcon(WPARAM wParam, LPARAM lParam);	// wParam = bEnabled
	afx_msg void OnOpenSecondBChannel();
	afx_msg void OnUpdateOpenSecondBChannel(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL			m_bFirstIconUpdate;
	CDevIntEx*	m_pdbdExDevice;
public:
};

#endif // IsdnUnitWnd_H
