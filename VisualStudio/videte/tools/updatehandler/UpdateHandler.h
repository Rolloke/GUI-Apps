// UpdateHandler.h : main header file for the UPDATEHANDLER application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#pragma once
#include "resource.h"       // main symbols

class CUpdateHandlerInput;
class CUser;
class CPermission;
class CMainFrame;
class CSetupDialog;

// for PostMessage(WM_COMMAND,...)
#define ID_ADD_DIR				0x7000
#define ID_ADD_FILE				0x7001
#define ID_ADD_REPORT			0x7002
#define ID_DEL_DIR				0x7003
#define ID_DEL_FILE				0x7004
#define ID_DEL_REPORT			0x7005
#define ID_CLEAR_DIR			0x7006
#define ID_CLEAR_FILE			0x7007
#define ID_CLEAR_REPORT			0x7008


#define ID_INPUT_CONNECT		0x7500
#define ID_INPUT_DISCONNECT		0x7501
#define ID_CONFIRM_FILE_UPDATE	0x7502
#define ID_FIRST				0x7503
#define ID_CONNECT				0x7504
#define ID_TIMEOUT				0x7505
#define ID_OS_VERSION_INFO		0x7506
#define ID_SYSTEM_INFO			0x7507
#define ID_DRIVES				0x7508

#define ID_TIME_ARRIVED			0x7509
#define ID_DIRECTORY_ARRIVED	0x750A
#define ID_ERROR				0x750B

#define ID_FILE_DELETED			0x750C
#define ID_FILE_EXECUTED		0x750D
#define ID_FILE_GOT				0x750E

#define ID_REG_ENUM_VALUES		0x7510
#define ID_REG_GET_VALUE		0x7511

class CCmdLine
{
public:
	CCmdLine(CString sCmdLineFile, DWORD nWaitTime);
	enum eCmdState
	{
		EXECUTE_CMD		= 1,
		ERROR_EXECUTE	= 2,
		WAITING			= 3,
		REQUEST_FILE	= 4,
		GOT_FILE		= 5,
		DELETE_FILE		= 6,
		FILE_DELETED	= 7
	}m_CmdState;
	CString m_sCmdLineFile;
	DWORD	m_nWaitTime;
};

typedef CCmdLine* CCmdLinePtr;
WK_PTR_ARRAY(CmdLinePlain, CCmdLinePtr);

class CCmdLineArray : public CmdLinePlain
{
public:
	CCmdLinePtr GetCmdLine(const CString&sLine, CCmdLine::eCmdState eNextState);
	CCmdLinePtr GetCmdLine(DWORD nTickCount, CCmdLine::eCmdState eNextState);
};

/////////////////////////////////////////////////////////////////////////////
// CUpdateHandler:
// See UpdateHandler.cpp for the implementation of this class
//
#define REG_PATH_ID _T("Reg\\")

class CUpdateHandler : public CWinApp
{
public:
	CUpdateHandler();
	virtual ~CUpdateHandler();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateHandler)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL


// Implementation
public:
	void OnInputConnect();
	void OnInputDisconnect();

public:
	void AddReportLine(const CString& sLine);
	void AddReportLine(UINT nID);
	void UpdateExecutable(const CString& sExe);
	void MultipleUpdate(const CStringArray& sFiles, BOOL bToDisk = FALSE);
	void OnConfirmFileUpdate(const CString& sName);
	void SetBusy();
	void ClearBusy();
	BOOL Connect();
	void NextUpdate();
	void OnTimeOut();
	void OnDrives();
	void OnTimeArrived();
	void OnError();
	void OnFileDeleted();
	void OnFileExecuted();
	void OnFileGot();

	void Start();
	int  Stop();

	void GetDirectory(const CString& sDir);

	void DeleteFile(const CString& sFile);
	void GetFile(const CString& sFile, DWORD dwSize = 0);
	void ExecuteFile(const CString& sFile);
	int  SendFile(const CString& sSource, const CString& sDest);
	int  SendCommand(const CString& sCommand);
	int  SendUpdateCommand(const CString& sCommand, BOOL bReboot);

	BOOL Login();

public:
	inline CUpdateHandlerInput* GetInput();
	inline CString GetDestination();
	inline CString GetSource();
	inline CString GetHost();
	inline CImageList* 	GetSymbols();
	inline CMainFrame* 	GetMainFrame();

public:
	//{{AFX_MSG(CUpdateHandler)
	afx_msg void OnAppAbout();
	afx_msg void OnVerbinden();
	afx_msg void OnUpdateVerbinden(CCmdUI* pCmdUI);
	afx_msg void OnTrennen();
	afx_msg void OnUpdateTrennen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLauncher();
	afx_msg void OnUpdateSource();
	afx_msg void OnUpdateAku();
	afx_msg void OnUpdateComm();
	afx_msg void OnUpdateDbs();
	afx_msg void OnUpdateExplorer();
	afx_msg void OnUpdateGaunit();
	afx_msg void OnUpdateIsdn();
	afx_msg void OnUpdatePh();
	afx_msg void OnUpdateServer();
	afx_msg void OnUpdateSim();
	afx_msg void OnUpdateSv();
	afx_msg void OnUpdateSdiUnit();
	afx_msg void OnUpdateTcpip();
	afx_msg void OnUpdateUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCoco();
	afx_msg void OnUpdateComplete();
	afx_msg void OnUpdateFile();
	afx_msg void OnUpdateHelp();
	afx_msg void OnUpdateMico();
	afx_msg void OnOptionsSettings();
	afx_msg void OnFileSystemInfo();
	afx_msg void OnUpdateFileSystemInfo(CCmdUI* pCmdUI);
	afx_msg void OnFileRegexport();
	afx_msg void OnUpdateFileRegexport(CCmdUI* pCmdUI);
	afx_msg void OnInfoTime();
	afx_msg void OnUpdateInfoTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSecana();
	afx_msg void OnUpdateReboot();
	afx_msg void OnUpdateZiel();
	afx_msg void OnUpdateDisk();
	afx_msg void OnConnectLocal();
	afx_msg void OnSetup();
	afx_msg void OnConnectionReset();
	afx_msg void OnUpdateConnectionReset(CCmdUI* pCmdUI);
	afx_msg void OnFileExecuteCmdLine();
	afx_msg void OnUpdateFileExecuteCmdLine(CCmdUI* pCmdUI);
	afx_msg void OnOptionPartialTransmission();
	afx_msg void OnUpdateOptionPartialTransmission(CCmdUI* pCmdUI);
	afx_msg void OnFileFetchCmdFile();
	afx_msg void OnUpdateFileFetchCmdFile(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void RegisterWindowClass();

private:
	CSetupDialog* m_pSetupDialog;
	CMainFrame*  m_pMainFrame;
	CUpdateHandlerInput* m_pUpdateHandlerInput;
	CUser*		 m_pUser;
	CPermission* m_pPermission;
	
	CString		 m_sHost;
	CString		 m_sNumber;
	CSecID		 m_idHost;
	
	CString		 m_sCurrentJob;
	CCmdLineArray	m_SentCmdLines;

	CString		 m_sSourceDirectory;
	CString		 m_sDestination;

	CImageList	 m_Symbols;

	int			 m_iCompleteUpdateIndex;

	BOOL		 m_bBusy;
	BOOL		 m_bComplete;

	DWORD		 m_dwJobStartTime;

	BOOL		 m_bDisconnectAtJobEnd;
	BOOL		 m_bOnlyExisting;
	BOOL		 m_bPartialTransmission;

	friend class CMainFrame;
	friend class CUpdateHandlerInput;
public:
};
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CUpdateHandler::GetSymbols()
{
	return &m_Symbols;
}
/////////////////////////////////////////////////////////////////////////////
inline CUpdateHandlerInput* CUpdateHandler::GetInput() 
{
	return m_pUpdateHandlerInput;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CUpdateHandler::GetDestination() 
{
	return m_sDestination;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CUpdateHandler::GetSource() 
{
	return m_sSourceDirectory;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CUpdateHandler::GetHost() 
{
	return m_sHost;
}
/////////////////////////////////////////////////////////////////////////////
inline CMainFrame* 	CUpdateHandler::GetMainFrame()
{
	return m_pMainFrame;
}
/////////////////////////////////////////////////////////////////////////////
extern CUpdateHandler theApp;
/////////////////////////////////////////////////////////////////////////////

