// USBCamUnitDlg.h : header file
//

#if !defined(AFX_USBCAMUNITDLG_H__7011433A_AB44_497C_9F7C_AED4F41FD42D__INCLUDED_)
#define AFX_USBCAMUNITDLG_H__7011433A_AB44_497C_9F7C_AED4F41FD42D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ICON_NOCOLOR		0
#define ICON_MAIN			1
#define ICON_RUNNING		2
#define ICON_RECORDING	3

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitDlg dialog
struct ErrorStruct;
class CMotionDetection;
class	CIPCInputUSBcamUnit;
class	CIPCInputUSBcamUnitMDAlarm;

#include "CMDPoints.h"
#include "resource.h"

class CUSBCamUnitDlg : public CDialog
{
	friend class CIPCInputUSBcamUnit;
	friend class CIPCOutputUSBcamUnit;
// Construction
public:
	CUSBCamUnitDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CUSBCamUnitDlg();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUSBCamUnitDlg)
	enum { IDD = IDD_USBCAMUNIT_DIALOG };
	CComboBox	m_cOutputFormats;
	CComboBox	m_cMediaType;
	CStatic	m_cPicture;
	CComboBox	m_cComboVideoInput;
	BOOL	m_bColor;
	BOOL	m_bShowRenderer;
	BOOL	m_bHalftone;
	int		m_nSelectedCam;
	CString	m_sCamera;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBCamUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ClearAllJobs();
	void RequestOutputReset();
	void RequestInputReset();

	void ReleaseAllFilters();
	void SetButtonIcon(int, int);
	void ControlStartStopBtn(bool);

	bool GetVideoSubtype(IPin*, AM_MEDIA_TYPE&);
	void SetUSBCamIcon(WORD wColor, BOOL bType=FALSE);
	void ReleaseFilters(WORD);
	void InitVideoCam(WORD); 
	void InitVideoCamEx(WORD); 
	HRESULT InitFilterGraph();
	void EnumDevices(REFCLSID, CComboBox&, CString&);
	void ReportError(ErrorStruct*, bool bMsgBox=true);
	void ComboResetContent(CComboBox&);
	HRESULT GetPin( IBaseFilter *, PIN_DIRECTION, int, IPin **);
	HRESULT ShowFilterPropertyPage(IBaseFilter *, bool);
	HRESULT ConnectFilters(IGraphBuilder*, IBaseFilter*, int, IBaseFilter*, int, AM_MEDIA_TYPE *pMT=NULL);
	HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, LPCTSTR, LPCWSTR);
	HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, CGuid&, LPCWSTR);
	HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, IMoniker*, LPCWSTR);
	HRESULT FindMatchingFilter(DWORD, IPin*, BOOL, GUID, GUID, BOOL, GUID, GUID*, int, BOOL, IBaseFilter **, AM_MEDIA_TYPE **);
	
	inline int CheckRange(int nValue, int nMin, int nMax)
	{
		if      (nValue  > nMax) return nMax;
		else if (nValue  < nMin) return nMin;
		return nValue;
	}

	// Generated message map functions
	//{{AFX_MSG(CUSBCamUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBtnSourceProperty();
	afx_msg void OnBtnEncoderProperty();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnInitVideoCam();
	afx_msg void OnAppExit();
	afx_msg void OnSettings();
	afx_msg void OnApplyNow();
	afx_msg void OnBtnRendererProperties();
	afx_msg void OnCkColor();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCkLastFilter();
	afx_msg void OnHalftone();
	afx_msg void OnCkMd();
	afx_msg void OnBtnMdProperties();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeVideoSource();
	afx_msg void OnSelchangeComboOutputformat();
	afx_msg void OnCkEnableCam();
	afx_msg void OnPause();
	//}}AFX_MSG
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon[3];
	UINT m_nIconTimer;
	BOOL				 m_bStandalone;

	CIPCInputUSBcamUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputUSBcamUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input

	ICreateDevEnum *m_pSysDevEnum;
	IGraphBuilder	*m_pGB;
   ICaptureGraphBuilder2 *m_pCaptureGB;

	struct SourceCtrl
	{
		CUSBCamUnitDlg *pThis;
		WORD wSource;
	}m_sCamSources[MAX_CAMERAS];

	IBaseFilter		*m_pCapture[MAX_CAMERAS];
	IBaseFilter    *m_pRemovedCapture[MAX_CAMERAS];
	IBaseFilter		*m_pInfTee[MAX_CAMERAS];
	IBaseFilter		*m_pDecode[MAX_CAMERAS];
	IBaseFilter		*m_pEncode[MAX_CAMERAS];
	IBaseFilter		*m_pDump[MAX_CAMERAS];
//	IBaseFilter		*m_pRender[MAX_CAMERAS];
	IVideoWindow   *m_pVideoWindow;
	BOOL	          m_bMotionDetection[MAX_CAMERAS];
	int             m_nOutputFormat[MAX_CAMERAS];
	CMDPoints       m_mdPoints[MAX_CAMERAS];
	BOOL	          m_bEnableCam[MAX_CAMERAS];
	CString         m_strVideoInput[MAX_CAMERAS];

   IMediaControl  *m_pMC;
   IMediaEventEx  *m_pME;

	CString         m_strOutputFileName;
	BYTE           *m_pRGBBuffer;
	BYTE           *m_p2ndBuffer;
	BOOL            m_bIsVisible;
	int             m_nMediaType;
	int             m_nCurrSelVideoSource;
	CMotionDetection*m_pMD;

	HRESULT CheckData(IMediaSample*, WORD);
	
	static  HRESULT WINAPI ReceiveMediaSample(IMediaSample*, long, long);
	static  HRESULT WINAPI CheckData(IMediaSample*, long);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBCAMUNITDLG_H__7011433A_AB44_497C_9F7C_AED4F41FD42D__INCLUDED_)
