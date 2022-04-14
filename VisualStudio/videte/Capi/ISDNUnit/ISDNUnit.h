/* GlobalReplace: CIPCServerControlIsdn --> CIPCServerControlISDN */
/* GlobalReplace: CIsdnConnection --> CISDNConnection */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNUnit.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNUnit.h $
// CHECKIN:		$Date: 20.01.06 12:11 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 20.01.06 10:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef ISDNUnit_H
#define ISDNUnit_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"		// main symbols
#include "IsdnConnection.h"
#include "WK_DebugOptions.h"

/////////////////////////////////////////////////////////////////////////////
#define ISDNUNITWND_CAPI_GET_MESSAGE_ERROR	WM_USER + 0x02
#define ISDNUNITWND_UPDATE_ICON					WM_USER + 0x03

/////////////////////////////////////////////////////////////////////////////
static LPCTSTR szConnectedRemoteNumber	= _T("ConnectedRemoteNumber");
static LPCTSTR szChannelsConnected		= _T("ChannelsConnected");

#define AUTOMATICALLY 2

/////////////////////////////////////////////////////////////////////////////
class CIPCServerControlISDN;
class CConnectionRecord;
class CCapiThread;
class CISDNUnitDlg;
class CAboutBox;

/////////////////////////////////////////////////////////////////////////////
enum ISDNProcessorUsage
{
	IPU_INVALID=0,
	IPU_HIGH=1,
	IPU_MEDIUM=2,
	IPU_LOW=3
};
extern LPCTSTR NameOfEnum(ISDNProcessorUsage usage);

/////////////////////////////////////////////////////////////////////////////
// CISDNUnitApp:
class CISDNUnitApp : public CWinApp
{
// Construction
public:
	CISDNUnitApp();
	~CISDNUnitApp();

// Attributes
	inline WK_ApplicationId			GetApplicationId() const;
	inline ISDNProcessorUsage		GetProcessorUsage() const;
	inline CIPCServerControlISDN*	GetCIPCControl() const;
	inline CISDNConnection*			GetISDNConnection() const;
	inline const CString&			GetProfileSection() const;
	inline const CString&			GetOwnHostName() const;
	inline const CString&			GetOwnNumber() const;
	inline const CString&			GetPrefix() const;
	inline int						GetMinimumPrefix() const;
	inline int						GetNumConfiguredBChannels() const;
	inline BOOL						IsSecondInstanceAllowed() const;
	inline BOOL						IsSecondInstance() const;
	inline BOOL						IsKnockBoxEnabled() const;
	inline BOOL						DoCloseOneChannel() const;
	inline BOOL						RunAsPTUnit() const;
	inline BOOL						IsDeviceFound() const;

	inline BOOL						TraceSentCommands() const;
	inline BOOL						TraceReceivedCommands() const;
	inline BOOL						TracePipes() const;
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISDNUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
public:
//	void	CapiGetMessageError();
	BOOL	InitCapiThread();
	BOOL	StopCapiThread();
	BOOL	CloseAllIsdnConnections(BOOL bWaitForResponse);
	void	CloseModule() ;
	void	UpdateAboutBox();

	void	ShowAboutDlg();
	void	ShowSettingsDlg();
	void	SetProcessorUsage(ISDNProcessorUsage usage);
	void	SetupChanged(const CString& sOwnNr,  int iNumChannels);
	//
	void	SetConnection(CISDNConnection *pConnection);
	void	EnableKnockBox(BOOL bDoIt);
	// 
	void	ClearReservedCallingID();
	//
	void	ReadDebugOptions();
	void	WriteDebugOptions();

private:
	BOOL	InitAppMainWndAndDebugger();
	BOOL	InitCapiDll();
	void	ReadSettings();
	void	SaveSettings();

	//{{AFX_MSG(CISDNUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// OOPS published
	CIPCServerControlISDN*	m_pControl;		// CIPC Control to Clients
private:
	DWORD				m_hDefNotify;
	CISDNConnection*	m_pConnection;
	//
	CCriticalSection	m_csConnection;
	ISDNProcessorUsage	m_processorUsage;
	CWKDebugOptions		m_debugOptions;
	// dialogs:
	CISDNUnitDlg*		m_pSettingsDlg;
	CAboutBox*			m_pAboutDlg;
	//
	CCapiThread*		m_pCapiThread;

	WK_ApplicationId	m_wkAppId;	// set in InitInstance

	CString		m_sProfileSection;	// set in contructor, might be modified in InitInstance
	CString		m_sServerControlShm;
	BOOL		m_bIsSecondInstance;
	BOOL		m_bAllowSecondApp;
	BOOL		m_bRunAsPTUnit;
	BOOL		m_bDeviceFound;

	// settings data:
	CString		m_sOwnHostName;				// set in InitInstance
	CString		m_sOwnNumber;				// set in constructor and SetupChanged
	CString		m_sPrefix;
	int			m_iMinimumPrefix;
	int			m_iNumConfiguredBChannels;	// set in constructor and SetupChanged
	BOOL		m_bEnableKnockBox;
	BOOL		m_bCloseOneChannel;
	//
	BOOL		m_bModuleClose;				// flag is set if module should be closed
	DWORD		m_dwModuleCloseTime;		// time when module should close for timeout

	BOOL		m_bTraceSentCommands;
	BOOL		m_bTraceReceivedCommands;
	BOOL		m_bTracePipes;
};
/////////////////////////////////////////////////////////////////////////////
extern CISDNUnitApp	theApp;
/////////////////////////////////////////////////////////////////////////////
inline CIPCServerControlISDN* CISDNUnitApp::GetCIPCControl() const
{
	return m_pControl;
}
/////////////////////////////////////////////////////////////////////////////
inline CISDNConnection* CISDNUnitApp::GetISDNConnection() const
{
	return m_pConnection;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CISDNUnitApp::GetOwnNumber() const
{
	return m_sOwnNumber;
}
/////////////////////////////////////////////////////////////////////////////
inline int CISDNUnitApp::GetNumConfiguredBChannels() const
{
	return m_iNumConfiguredBChannels;
}
/////////////////////////////////////////////////////////////////////////////
inline CISDNUnitApp *MyGetApp() 
{
	// OLD return (CISDNUnitApp *)AfxGetApp();
	return &theApp;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString &CISDNUnitApp::GetOwnHostName() const
{
	return m_sOwnHostName;	// set in InitInstance
}
/////////////////////////////////////////////////////////////////////////////
inline ISDNProcessorUsage CISDNUnitApp::GetProcessorUsage() const
{
	return m_processorUsage;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::IsKnockBoxEnabled() const
{
	return m_bEnableKnockBox;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::DoCloseOneChannel() const
{
	return m_bCloseOneChannel;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString &CISDNUnitApp::GetProfileSection() const
{
	return m_sProfileSection;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::RunAsPTUnit() const
{
	return m_bRunAsPTUnit;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::IsDeviceFound() const
{
	return m_bDeviceFound;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::IsSecondInstance() const
{
	return m_bIsSecondInstance;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNUnitApp::IsSecondInstanceAllowed() const
{
	return m_bAllowSecondApp;
}
/////////////////////////////////////////////////////////////////////////////
inline WK_ApplicationId CISDNUnitApp::GetApplicationId() const
{
	return m_wkAppId;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CISDNUnitApp::GetPrefix() const
{
	return m_sPrefix;
}
/////////////////////////////////////////////////////////////////////////////
inline int CISDNUnitApp::GetMinimumPrefix() const
{
	return m_iMinimumPrefix;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CISDNUnitApp::TraceSentCommands() const
{
	return m_bTraceSentCommands;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CISDNUnitApp::TraceReceivedCommands() const
{
	return m_bTraceReceivedCommands;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CISDNUnitApp::TracePipes() const
{
	return m_bTracePipes;
}
#endif	// ISDNUnit_H
