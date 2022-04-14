// QUnitDlg.h : Headerdatei
//

#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

#define WM_UPDATE_DIALOG	(WM_USER + 1)

class CUDP;
class CVideoInlayWindow;
class CIPCInputQUnit; 
class CIPCInputQUnitMDAlarm;
class CIPCOutputQUnit;
class CQUnitDlg : public CDialog
{
// Konstruktion
public:
	CQUnitDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	void SetActiveCamera(int nActiveCamera);

	BOOL OnCapture(QIMAGE* pQImage);
	BOOL OnVideoState(int nCamera, BOOL bPresent);	
	BOOL OnSensor(DWORD dwSensorMask);
	void OnReceivedMotionAlarm(int nCamera, CMDPoints& Points);

	BOOL OnConfirmSetBrightness(int nCamera, int nValue);
	BOOL OnConfirmSetContrast(int nCamera, int nValue);
	BOOL OnConfirmSetSaturation(int nCamera, int nValue);
	BOOL OnConfirmSetHue(int nCamera, int nValue);
	BOOL OnConfirmSetRelay(DWORD dwMask);
	BOOL OnConfirmSetFramerate(int nCamera, int nFps);
	BOOL OnConfirmSetAlarmEdge(DWORD dwEdgeMask);
	BOOL OnConfirmSetImageSize(int nCamera, CSize ImageSize);

	BOOL OnConfirmGetBrightness(int nCamera, int nValue);
	BOOL OnConfirmGetContrast(int nCamera, int nValue);
	BOOL OnConfirmGetSaturation(int nCamera, int nValue);
	BOOL OnConfirmGetHue(int nCamera, int nValue);
	BOOL OnConfirmGetRelay(DWORD dwMask);
	BOOL OnConfirmGetFramerate(int nCamera, int nFps);
	BOOL OnConfirmGetAlarmEdge(DWORD dwEdgeMask);
	BOOL OnConfirmGetVideoState(int nCamera, BOOL bPresent);
	BOOL OnConfirmGetImageSize(int nCamera, CSize ImageSize);

	// Dialogfelddaten
	enum { IDD = IDD_QUNIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRelay(UINT nID);
	afx_msg void OnCamera(UINT nID);
	afx_msg void OnSelectFps(UINT nID);
	afx_msg void OnBnClickedPreview();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelectResolution();
	afx_msg void OnDlgMdConfig();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileAbout();
	afx_msg void OnFileSettings();
	afx_msg void OnFileExit();
	afx_msg long OnUpdateDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckMulti();
	afx_msg void OnBnClickedFreeFps();
	afx_msg void OnBnClickedRdDeint();
	DECLARE_MESSAGE_MAP()

private:
	BOOL UpdateDialog();
	BOOL CreateCIPCConnection();
	long DoConfirmSelfcheck(WPARAM wParam, LPARAM /*lParam*/);
	BOOL Login();

private:
	CButton		m_ctrlRelay0;
	CButton		m_ctrlAlarm0;
	CButton		m_ctrlCamera0;
	CComboBox	m_ctrlComboFps0;

	CSliderCtrl	m_ctrlBrightness;
	CSliderCtrl	m_ctrlContrast;
	CSliderCtrl	m_ctrlSaturation;

	CString				m_sIniFile;
	CUDP*				m_pUDP;
	CVideoInlayWindow*	m_pVideoInlayWindow;
	CCriticalSection	m_csPreview;

	CIPCInputQUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputQUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputQUnit*		m_pOutputCamera;		// Pointer auf CIPC-Output
	CIPCOutputQUnit*		m_pOutputRelay;			// Pointer auf CIPC-Output

	int					m_nActiveCamera;
	BOOL				m_bStandalone;
	BOOL				m_bMulti;
	BOOL				m_bManualFramerates;
	BOOL m_nDeinterlace;

public:
private:
	CSliderCtrl m_ctrlHue;
public:
};
