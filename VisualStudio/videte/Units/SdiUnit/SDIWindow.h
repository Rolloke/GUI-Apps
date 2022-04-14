/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIWindow.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIWindow.h $
// CHECKIN:		$Date: 15.06.05 10:24 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 15.06.05 10:09 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _SDIWindow_H_
#define _SDIWindow_H_

#include "starinterface.h"
/////////////////////////////////////////////////////////////////////////////
class CTestDlg;

/////////////////////////////////////////////////////////////////////////////
// CSDIWindow
class CSDIWindow : public CWnd
{
// Construction
public:
	CSDIWindow();
	virtual ~CSDIWindow();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIWindow)
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetToolTip(const CString& sComs);
	inline CStarInterface* GetStarInterfaceCtrl();

protected:

	// Generated message map functions
	//{{AFX_MSG(CSDIWindow)
	afx_msg void OnDestroy();
	afx_msg void OnAppExit();
	afx_msg void OnAbout();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnCardEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ReaderIndex, LPCTSTR IDNumber, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus, LPCTSTR FunctionKey);
	afx_msg void OnCommErrorStarinterfacectrl(short nIndex, LPCTSTR BoardIndex);
	afx_msg void OnConverterDisconnectStarinterfacectrl(short nIndex);
	afx_msg void OnDoorOpenEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR DoorIndex, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus);
	afx_msg void OnForcedDoorOpenEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR DoorIndex, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus);
	afx_msg void OnInputEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR InputPort, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus);
	afx_msg void OnIOStatusEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR InputStatus, LPCTSTR OutputStatus, LPCTSTR FunctionKey);
	afx_msg void OnPollingErrorStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ErrorCode);
	afx_msg void OnReceiveDataStarinterfacectrl(short nIndex, LPCTSTR ReceiveString);
	afx_msg void OnSendDataStarinterfacectrl(short nIndex, LPCTSTR SendString);

	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg long OnResetSDI(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

// Attributes
private:
	CStarInterface	m_StarInterface;
};
/////////////////////////////////////////////////////////////////////////////
inline CStarInterface* CSDIWindow::GetStarInterfaceCtrl()
{
	return &m_StarInterface;
}

/////////////////////////////////////////////////////////////////////////////
#endif
