// IPCameraUnitDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"
#include "VideoStateCheckThread.h"

#define ICON_NOCOLOR		0
#define ICON_MAIN			1
#define ICON_INIT			2
#define ICON_MD				3

class CIpCamSocket;
class CDsCamera;
class CIPCOutputIPcamUnit;
class CIPCInputIpCamUnit;
class CIPCInputIpCamUnitMDAlarm;
class CStatistics;
class CVideoStateCheckThread;

struct CameraParameters
{
	CameraParameters(DWORD dwUserData, 
		Resolution res=RESOLUTION_NONE,
		Compression comp=COMPRESSION_NONE,
		CompressionType ct=COMPRESSION_UNKNOWN,
		int iFPS=0, int iIFrameInterval=0);
	WORD m_wID;
	DWORD m_dwUserData;
	Resolution m_res;
	Compression m_comp;
	CompressionType m_ct;
	int m_iFPS;
	int m_iIFrameInterval;
};

class CSaveThreadList : public CListCtrl
{
public:
//	BOOL Lock(){return m_cs.Lock();}
//	BOOL Unlock(){return m_cs.Unlock();}
	BOOL Lock(){return TRUE;}
	BOOL Unlock(){return TRUE;}
	CCriticalSection m_cs;
};

//////////////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitDlg Dialogfeld
class CIpCameraUnitDlg : public CDialog
{
	friend class CIpCamDeviceFindCB;
// Konstruktion
public:
	CIpCameraUnitDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CIpCameraUnitDlg();	// Destruktor

// Dialogfelddaten
	//{{AFX_DATA(CIpCameraUnitDlg)
	enum { IDD = IDD_IPCAMERAUNIT_DIALOG };
	BOOL	m_bSet;
	CString m_sURL;// IP-Address or DNS-Name
	CString m_sRequest;
	CEdit	m_ctrlAnswer;
	UINT	m_nPort;
	BOOL	m_nShowCtrl;
	//CListCtrl m_ThreadList;
	CSaveThreadList m_ThreadList;
	CListCtrl m_IOlist;
	CString m_sUserName;
	CString m_sPassword;
	CString m_sSecID;
	CString m_sProxyName;
	CString m_sProxyBypass;
	int		m_nFps;
	CComboBox m_cResolution;
	CComboBox m_cCompressionType;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CIpCameraUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Attribute
public:
	DWORD   GetActiveCameraMask(BOOL bRunning);
	BOOL    IsCameraInitialised(WORD);
	BOOL    IsCameraPresent(WORD);
	BOOL    IsCameraActive(WORD);
	CIPCOutputIPcamUnit*GetOutput(WORD wID=0xffff) const;
	inline BOOL    IsShuttingDown() const;
	inline const CString& GetKey() const;
	CString HTTPRequest(const CString &sUrl, int nPort, const CString &sRequest, DWORD &dwResult, BOOL bPost/*=0*/);
	void	AddPictureStatistic(long lLen);
	void	AddSpanStatistic(long lLen);
	void	UpdateImageSize(CSize sz);
	static  CString ReadHTTPAnswer(CHttpFile*pFile, DWORD &dwResult, int nCount=50);

private:
	CString	GetMediaStateText(OAFilterState State);
	CDsCamera* GetThreadByID(WORD wID);
	CDsCamera* GetSelectedThread();
	void	EnableStateButtons();

// Implementierung
public:
	void	InitResetTimer();
	void	StartInputRequestTimer();
	void	StopInputRequestTimer();
	BOOL    SetCameraActive(CIPCOutputIPcamUnit*pOutput, WORD wSubID, bool bActive, CameraParameters*pCP);
	void	SetTrayIcon(WORD wColor);
	void	SetButtonIcon(int nBtnID, int nIconID);
	BOOL	OnRequestGetValue(CSecID id, const CString &sKey, DWORD dwUserData);
	BOOL	OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);
	void	NotifyIOList(CSecID id, BOOL bOn);
	void	CheckHidden();

private:
	void	StartCaptureCamera(const CString &sName);
	void	ReportError(ErrorStruct*, BOOL bMsgBox=true);
	void	SafeKillTimer(UINT &uTimerID);
	void	ShowHide();
	void	InsertThread(CDsCamera*pThread);
	void	RemoveThread(CDsCamera*pThread);
	void	SetDSThread(int nWhat, long lParam);
	static UINT CALLBACK UpnpThread(void*);
	UINT	StartUpnpThread();
	BOOL	StartVideoStateCheckThread();
	BOOL	StopVideoStateCheckThread();

	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CIpCameraUnitDlg)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedRdText();
	afx_msg void OnBnClickedRdList();
	afx_msg void OnBnClickedBtnCameraSettings();
	afx_msg void OnNMClickThreadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemThreadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAppExit();
	afx_msg void OnUpdateAppExit(CCmdUI *pCmdUI);
	afx_msg void OnSettings();
	afx_msg void OnUpdateSettings(CCmdUI *pCmdUI);
	afx_msg void OnAbout();
	afx_msg void OnUpdateAbout(CCmdUI *pCmdUI);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnHresultError(WPARAM, LPARAM);
	afx_msg LRESULT OnInternetStatus(WPARAM, LPARAM);
	afx_msg LRESULT OnThreadStarted(WPARAM, LPARAM);
	afx_msg LRESULT OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCameraActive(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDeviceChange(UINT, DWORD);
	afx_msg void OnBnClickedRdPreview();
	afx_msg void OnBnClickedCkRegisterGraph();
	afx_msg void OnBnClickedCkLogsToEdt();
	afx_msg void OnBnClickedRdInputList();
	afx_msg void OnBnClickedRdRelaisList();
	afx_msg void OnBnClickedBtnStartThread();
	afx_msg void OnBnClickedBtnStopThread();
	afx_msg void OnBnClickedBtnSetParam();
	afx_msg LRESULT OnWinthreadStopThread(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bUpdateImageSize;
protected:
	HICON m_hIcon[3];
private:
	BOOL				m_bStandalone;
	CHttpConnection*	m_pHttpServer;
	UINT				m_uResetTimer;
	UINT				m_uInputRequestTimer;
	UINT				m_uCheckTimer;
	BOOL				m_bIsShuttingDown;
	BOOL				m_bThreadIsStarting;
	CString				m_strKey;					// Registry Key
	CStatistics*		m_pFrames;
	CStatistics*		m_pSpans;
	CWinThread*			m_pUpnpThread;
	CStringArray		m_saUpnpDevices;
	CIPCOutputIPcamUnit*m_pOutputIPcamera;
	CIPCOutputIPcamUnit*m_pOutputIPrelais;
	CIPCOutputIPcamUnit*m_pOutputControl;
	CIPCInputIpCamUnit*	m_pInputIp;
	CIPCInputIpCamUnitMDAlarm*m_pInputIpMd;
	CVideoStateCheckThread* m_pVideoStateCheck;

public:
};
//////////////////////////////////////////////////////////////////////////////////////
inline BOOL CIpCameraUnitDlg::IsShuttingDown()const
{
	return m_bIsShuttingDown;
}
//////////////////////////////////////////////////////////////////////////////////////
inline const CString& CIpCameraUnitDlg::GetKey() const
{
	return m_strKey;
}
