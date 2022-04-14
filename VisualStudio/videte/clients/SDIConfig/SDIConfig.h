/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfig.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfig.h $
// CHECKIN:		$Date: 25.01.99 11:20 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 25.01.99 11:07 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#if !defined(AFX_SDICONFIG_H__3716C0E8_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_SDICONFIG_H__3716C0E8_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// for PostMessage(WM_COMMAND,...)
//**************************************************
// MSG_SDI_FIRST muss der ersten Message entsprechen
#define MSG_SDI_FIRST						0x7000
//**************************************************
#define MSG_SDI_START_CONNECTION			0x7000
#define MSG_SDI_CONNECTION_READY			0x7001
#define MSG_SDI_CONNECTION_FAILED			0x7002
#define MSG_SDI_STOP_CONNECTION				0x7003
#define MSG_SDI_GET_INFO					0x7004
#define MSG_SDI_UPDATE_CONTROL				0x7005
#define MSG_SDI_ADD_CONTROL					0x7006
#define MSG_SDI_DELETE_CONTROL				0x7007
#define MSG_SDI_UPDATE_STATE				0x7008
#define MSG_SDI_FILE_TRANSFER_START			0x7009
#define MSG_SDI_FILE_TRANSFER_READY			0x7010
#define MSG_SDI_FILE_TRANSFER_FAILED		0x7011
#define MSG_SDI_CONFIGURATION_TRY			0x7012
#define MSG_SDI_CONFIGURATION_START			0x7013
#define MSG_SDI_CONFIGURATION_READY			0x7014
#define MSG_SDI_CONFIGURATION_FAILED		0x7015
#define MSG_SDI_EXTERN_PROGRAM_START		0x7016
#define MSG_SDI_EXTERN_PROGRAM_STOPPED		0x7017
//**************************************************
// MSG_SDI_LAST muss der letzten Message entsprechen
#define MSG_SDI_LAST						0x7017
//**************************************************

/////////////////////////////////////////////////////////////////////////////
class CMainFrame;
class CUser;
class CPermission;

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigApp:
// See SDIConfig.cpp for the implementation of this class
class CSDIConfigApp : public CWinApp
{
public:
	CSDIConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
public:
	inline CMainFrame* GetMainWnd() const;
	inline CUser* GetUser() const;
	inline CPermission* GetPermission() const;
protected:
	void	RegisterWindowClass();
	void	LoadDebugConfiguration();
	BOOL	Login();
	BOOL	LoginAsSuperVisor();

	//{{AFX_MSG(CSDIConfigApp)
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Variables
protected:
	CUser*			m_pUser;		//
	CPermission*	m_pPermission;	//

};
/////////////////////////////////////////////////////////////////////////////
inline CMainFrame* CSDIConfigApp::GetMainWnd() const
{
	return (CMainFrame*)m_pMainWnd;
}
/////////////////////////////////////////////////////////////////////////////
inline CUser* CSDIConfigApp::GetUser() const
{
	return m_pUser;
}
/////////////////////////////////////////////////////////////////////////////
inline CPermission* CSDIConfigApp::GetPermission() const
{
	return m_pPermission;
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIG_H__3716C0E8_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
