// MiCoAdlg.h : header file
//
#include "resource.h"
#include "MicoReg.h"
#include <afxmt.h>

UINT PollProzessThread(LPVOID pData);
UINT PollEncoderThread(LPVOID pData);
UINT SwitchThread(LPVOID pData);

/////////////////////////////////////////////////////////////////////////////
// CMiCoUnitDlg dialog
class CMiCo;
class CMegra;
class CIPCInputMiCoUnit;
class CIPCOutputMiCoUnit;
class CSettings;
class CUser;
class CPermission;
class CVideoControl;

/////////////////////////////////////////////////////////////////////////////

class CMiCoUnitDlg : public CDialog
{			  
	// Construction
public:
	CMiCoUnitDlg();	// standard constructor

	// Attributes
public:
	CMiCo*		GetMiCo(){return m_pMiCo;}

	// Operations
public:
	BOOL Create();
	
	void PollProzess();
	// Tread der Daten an den Decoder schickt.

	void PollEncoder();
	// Thread, der auf den Encoder pollt.
	
	void SwitchThread();
	// Test Thread, der zum zyklichen Umschalten der Kameras dient

	// Dialog Data
	//{{AFX_DATA(CMiCoUnitDlg)
	enum { IDD = IDD_MICOAPP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiCoUnitDlg)
	public:
	virtual BOOL OnQueryOpen();
   	virtual void OnSysCommand(UINT nID, LPARAM lParam);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
//OOPS protected:
	public:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMiCoUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBeNtscCCIR();
	afx_msg void OnBePalCCIR();
	afx_msg void OnBeNtscSquare();
	afx_msg void OnBePalSquare();
	afx_msg void OnDlgSettings();
	afx_msg void OnExtcard0();
	afx_msg void OnExtcard1();
	afx_msg void OnExtcard2();
	afx_msg void OnExtcard3();
	afx_msg void OnFeFbas();
	afx_msg void OnFeNtscCCIR();
	afx_msg void OnFePalCCIR();
	afx_msg void OnFeNtscSquare();
	afx_msg void OnFePalSquare();
	afx_msg void OnFeSvhs();
	afx_msg void OnSource1();
	afx_msg void OnSource2();
	afx_msg void OnSource3();
	afx_msg void OnSource4();
	afx_msg void OnSource5();
	afx_msg void OnSource6();
	afx_msg void OnSource7();
	afx_msg void OnSource8();
	afx_msg void OnBeComposite();
	afx_msg void OnBeRgb();
	afx_msg void OnFeLNotch();
	afx_msg void OnFeLDec();
	afx_msg void OnEdge7();
	afx_msg void OnEdge6();
	afx_msg void OnEdge5();
	afx_msg void OnEdge4();
	afx_msg void OnEdge3();
	afx_msg void OnEdge2();
	afx_msg void OnEdge1();
	afx_msg void OnEdge0();
	afx_msg void OnAck0();
	afx_msg void OnAck1();
	afx_msg void OnAck2();
	afx_msg void OnAck3();
	afx_msg void OnAck4();
	afx_msg void OnAck5();
	afx_msg void OnAck6();
	afx_msg void OnAck7();
	afx_msg void OnRelais0();
	afx_msg void OnRelais1();
	afx_msg void OnRelais2();
	afx_msg void OnRelais3();
	afx_msg void OnDecStart();
	afx_msg void OnDecStop();
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
	//}}AFX_MSG

	afx_msg long OnWmAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmLostField(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmUnvalidField(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmUnvalidBuffer(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmUnvalidFrameLen(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmIRQRekursion(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	// implementation
private:
	BOOL UpdateDlg();
	// Bringt die Dialogelemente auf den neusten Stand

	void VideoDataService();
	// Transportiert Daten an den Decoder

	BOOL SetDlgText(UINT nID, const char*	szText);
	
	BOOL IsValid();

	// members
private:
	BOOL					m_bRun;
	BOOL					m_bOn;

	CWinThread*				m_pPollProzessThread;	// Pollt auf den Decoder
	CWinThread*				m_pPollEncoderThread;	// Pollt auf den Encoder
	CWinThread*				m_pSwitchThread;		// Test-Thread

	CMiCo*					m_pMiCo;				// Pointer auf das MiCo-Objekt

	DWORD					m_dwDecodedFrames;		// Anzahl der empfangenen JPEG'S
	DWORD					m_dwEncodedFrames;		// Anzahl der verschickten JPEG'S
	DWORD					m_dwSourceSwitch;		// Anzahl der Kameraumschaltungen
	int						m_nEncoderState;		// ENCODER_ON, ENCODER_OFF
	int						m_nDecoderState;		// DECODER_ON, DECODER_OFF

	BOOL					m_bOk;					// TRUE -> Objekt ok
	WORD					m_wFormat;				// Auflösung
	WORD					m_wFPS;					// Frames per second
	DWORD					m_dwBPF;				// Bytes per Frame
	CSettings				*m_pSettings;			// Pointer auf Settingsdialog

	BOOL					m_bTraceFps;			// Framerate Tracen ein/aus
	CString					m_sJpegPath;			// Pfad zum Speichern der Jpegs
	
	HANDLE					m_hSyncEvent;			// Eventhandle für MiCo.vxd
 
	friend UINT PollProzessThread(LPVOID pData);
	friend UINT PollEncoderThread(LPVOID pData);
	friend UINT SwitchThread(LPVOID pData);
};
