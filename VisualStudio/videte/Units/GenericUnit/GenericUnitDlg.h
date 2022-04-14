/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: GenericUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/GenericUnitDlg.h $
// CHECKIN:		$Date: 24.01.01 11:16 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 24.01.01 11:14 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __GENERICUNITDLG_H__
#define __GENERICUNITDLG_H__

#include "resource.h"
#include "videojob.h"

UINT PollProzessThread(LPVOID pData);

#define WM_SETMYTEXT WM_USER +120

/////////////////////////////////////////////////////////////////////////////
// CGenericUnitDlg dialog
class CIPCInputGenericUnit;
class CIPCInputGenericUnitMDAlarm;
class CIPCOutputGenericUnit;

/////////////////////////////////////////////////////////////////////////////

class CGenericUnitDlg : public CDialog
{			  
	// Construction
public:
	CGenericUnitDlg();	// standard constructor
	~CGenericUnitDlg();

	// Attributes
public:
    CSemaVideoJobs& GetJobQueueEncode() {return m_jobQueueEncode;}

	// Operations
public:
	BOOL Create();

	void ClearAllJobs();
	void ClearAllEncodeJobs();
	void ClearCurrentEncodeJob();
	
	void RequestOutputReset();
	void RequestInputReset();

	void PollProzess();

	void OnReceivedJpegData(const JPEG* pJpeg);
	void OnReceivedAlarm(WORD wCurrentAlarmState);
	void OnReceivedMotionAlarm(WORD wSource, CPoint Point);
	
	const CString& GetIniPath(){return m_sIniFile;}

	// Dialog Data
	//{{AFX_DATA(CGenericUnitDlg)
	enum { IDD = IDD_GENERIC_APP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericUnitDlg)
	public:
	virtual BOOL OnQueryOpen();
   	virtual void OnSysCommand(UINT nID, LPARAM lParam);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void Dump( CDumpContext &dc ) const;
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation

	public:
	// Generated message map functions
	//{{AFX_MSG(CGenericUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnFeFbas();
	afx_msg void OnFeNtscCCIR();
	afx_msg void OnFePalCCIR();
	afx_msg void OnFeSvhs();
	afx_msg void OnSource1();
	afx_msg void OnSource2();
	afx_msg void OnSource3();
	afx_msg void OnSource4();
	afx_msg void OnSource5();
	afx_msg void OnSource6();
	afx_msg void OnSource7();
	afx_msg void OnSource8();
	afx_msg void OnRelais0();
	afx_msg void OnRelais1();
	afx_msg void OnRelais2();
	afx_msg void OnRelais3();
	afx_msg void OnEncStart();
	afx_msg void OnEncStop();
	afx_msg void OnTest();
	afx_msg void OnCompress1();
	afx_msg void OnCompress2();
	afx_msg void OnCompress3();
	afx_msg void OnCompress4();
	afx_msg void OnCompress5();
	afx_msg void OnHighResolution();
	afx_msg void OnLowResolution();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnCompress0();
	afx_msg void OnSource9();
	afx_msg void OnSource10();
	afx_msg void OnSource11();
	afx_msg void OnSource12();
	afx_msg void OnSource13();
	afx_msg void OnSource14();
	afx_msg void OnSource15();
	afx_msg void OnSource16();
	afx_msg void OnDlgMdConfig();
	//}}AFX_MSG

	afx_msg long OnWmRequestOutputReset(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmRequestInputReset(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSetDlgText(WPARAM wParam, LPARAM lParam);
	afx_msg long DoOpenMDDlg(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

	// implementation
private:
	BOOL UpdateDlg();
	void ProzessManagement();
	BOOL SetDlgText(UINT nID, const char*	szText);
	BOOL IsValid();
	void SaveJpegFile(const JPEG* pJpeg);
	void SetGenericIcon(WORD wColor, BOOL bType = FALSE);

	
private:
	BOOL					m_bRun;
	BOOL					m_bOn;
	CWinThread*				m_pPollProzessThread;
			   
	HICON					m_hIcon1;
	HICON					m_hIcon2;
	HICON					m_hIcon3;

	CCodec*						m_pCodec;				// Pointer auf das Codec-Objekt
	CIPCInputGenericUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputGenericUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputGenericUnit*		m_pOutputGenericCamera;	// Pointer auf CIPC-Output
	CIPCOutputGenericUnit*		m_pOutputGenericRelay;	// Pointer auf CIPC-Output

	CVideoJob*				m_pCurrentEncodeJob;	// Aktueller Encoderjob
	CSemaVideoJobs			m_jobQueueEncode;		// Encoderjobqueue
	DWORD					m_dwEncodedFrames;		// Anzahl der verschickten JPEG'S
	DWORD					m_dwSourceSwitch;		// Anzahl der Kameraumschaltungen
	int						m_nEncoderState;		// ENCODER_ON, ENCODER_OFF
	BOOL					m_bOk;
	BOOL					m_bStandalone;
	WORD					m_wFormat;				// Auflösung
	WORD					m_wFPS;					// Frames per second
	DWORD					m_dwBPF;				// Bytes per Frame

	BOOL					m_bSaveInLog;			// Jpegdaten nur ins Logfile sichern
	CString					m_sJpegPath;			// Pfad zum Speichern der Jpegs
	CString					m_sIniFile;				// Name der Ini-Datei	
	int						m_nTimePerFrame;		// Zeit zwischen 2 Bildern (20ms/40ms)

	CCriticalSection		m_csEncode;
	
	HANDLE					m_hSyncSemaphore;
	HANDLE					m_hShutDownEvent;

	friend UINT PollProzessThread(LPVOID pData);

protected:
	void OnAbout();
};

#endif // __GENERICUNITDLG_H__
