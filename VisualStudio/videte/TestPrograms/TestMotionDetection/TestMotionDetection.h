// TestMotionDetection.h : Hauptheaderdatei für die TestMotionDetection-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole
#include "afxsock.h"       // Hauptsymbole

#define DEFAULT_COLOR	0xffffffff

#define MOVING_BLOCK_RANDOM		1	// move a black rectangle to random coordinates
#define COLORS_RGBCMY			2	// RGBCMYK
#define SHADE_WHITE_TO_BLACK	3	// shade white to black
#define BLINK_WHITE_AND_BLACK	4	// blink black and white
#define HORZ_AND_VERT_LINES		5	// draw horizontal and vertical lines
#define TEST_PICTURE5			6
#define TEST_PICTURE6			7
#define TEST_PICTURE7			8
#define TEST_PICTURE8			9
#define TEST_PICTURE9			10

#define SETTINGS		_T("Settings")

#define BLOCK_SIZE_X	_T("BlockSizeX")
#define BLOCK_SIZE_Y	_T("BlockSizeY")
#define TIMER_TIMEOUT	_T("TimeOut")
#define DRAW_TYPE		_T("DrawType")
#define BACKGROUNDCOLOR	_T("BackGroundColor")
#define	MONITOR			_T("Monitor")

#define SOCKET_PORT		_T("SocketPort")
#define SOCKET_ADDRESS	_T("SocketAddress")
#define IS_SERVER		_T("IsServer")


// CAboutDlg:
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung

protected:
	void	ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
protected:
	afx_msg void OnBnClickedRdType0();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSize	m_szBlock;
	UINT	m_nTimeOut;
	CString m_sHost;
	long	m_nPort;
	int		m_nType;
};

// CMDTestSocket:
class CMDTestSocket: public CAsyncSocket
{
public:
	CMDTestSocket();
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDTestSocket)
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

public:
	CString m_sHost;
	long	m_nPort;
};

// CTestMotionDetectionApp:
class CTestMotionDetectionApp  : public CWinApp
{
public:
	CTestMotionDetectionApp();
	virtual ~CTestMotionDetectionApp();


// Überschreibungen
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMotionDetectionApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	// Generated message map functions
	//{{AFX_MSG(CTestMotionDetectionApp)
protected:
	afx_msg void OnAppAbout();
	afx_msg void OnFileUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	BOOL	InitSocket();
	BOOL	SendToServer(const CString&sSend);
	BOOL	SendToClient(const CString&sSend);
	BOOL	ScanData(const CString &sData);
	void	DataToDlg();
	void	DlgToData();
	CString FormatData();

public:
	void	OnReceive(const CString&sCmd, const CString&sSubCmd, const CString&sAddress, const CString&sData);

public:
	CSize		m_szBlock;
	UINT		m_nTimeOut;
	int			m_nType;
	COLORREF	m_BackGroundColor;
	int			m_nMonitor;
protected:
	CAboutDlg		m_AboutDlg;
	BOOL			m_bServer;
	CMDTestSocket m_Socket;
	CString			m_sOwnHost;
	CString			m_sRemoteHost;
};


extern CTestMotionDetectionApp theApp;
