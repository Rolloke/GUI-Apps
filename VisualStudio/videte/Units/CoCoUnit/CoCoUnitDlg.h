/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CoCoUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CoCoUnitDlg.h $
// CHECKIN:		$Date: 29.11.02 8:59 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 29.11.02 8:46 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "ccoco.h"
#include "picturedef.h"
#include "videojob.h"

#define ENCODER_START_REQ		1
#define ENCODER_ON              2
#define ENCODER_STOP_REQ        3
#define ENCODER_OFF             4
#define DECODER_START_REQ		5
#define DECODER_ON              6
#define DECODER_STOP_REQ        7
#define DECODER_OFF             8
#define COCO_INI	"COCOISA.INI"
		   
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg dialog
class CCoCo;
class CMegra;
class CIPCInputCoCoUnit;
class CIPCOutputCoCoUnit;
class CSettings;
//class CVideoJob;
//class CVideoJobs;
class CUser;
class CPermission;

/////////////////////////////////////////////////////////////////////////////

class CCoCoUnitDlg : public CDialog
{			  
// Construction
public:
	CCoCoUnitDlg(CWnd* pParent = NULL);	// standard constructor
	~CCoCoUnitDlg();

	BOOL Create();

	void		PollEncode();
	void		PollDecode();

	BOOL		IsValid();
	CCoCo*		GetCoCo(){return m_pCoCo;}
    CSemaVideoJobs& GetJobQueueEncode() {return m_jobQueueEncode;}
    CSemaVideoJobs& GetJobQueueDecode() {return m_jobQueueDecode;}

	void ClearAllJobs();
	void ClearAllEncodeJobs();
	void ClearCurrentEncodeJob(const CString &s="");
	void ClearAllDecodeJobs();
	void ClearCurrentDecodeJob();

	BOOL		m_bRun;
	BOOL		m_bOn;
	
	// Dialog Data
	//{{AFX_DATA(CCoCoUnitDlg)
	enum { IDD = IDD_COCOAPP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoCoUnitDlg)
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
//OOPS protected:
	public:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCoCoUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnActivateApp(BOOL, DWORD); 
	afx_msg void OnBeNtsc();
	afx_msg void OnBePal();
	afx_msg void OnBeSecam();
	afx_msg void OnDlgSettings();
	afx_msg void OnExtcard0();
	afx_msg void OnExtcard1();
	afx_msg void OnExtcard2();
	afx_msg void OnExtcard3();
	afx_msg void OnFeFbas();
	afx_msg void OnFeNtsc();
	afx_msg void OnFePal();
	afx_msg void OnFeSecam();
	afx_msg void OnFeSvhs();
	afx_msg void OnSource1();
	afx_msg void OnSource2();
	afx_msg void OnSource3();
	afx_msg void OnSource4();
	afx_msg void OnSource5();
	afx_msg void OnSource6();
	afx_msg void OnSource7();
	afx_msg void OnSource8();
	afx_msg void OnSendData();
	afx_msg void OnExtcardMegra();
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
	afx_msg void OnSendMci();
	afx_msg void OnSendScript();
	afx_msg void OnCocoid0();
	afx_msg void OnCocoid1();
	afx_msg void OnCocoid2();
	afx_msg void OnCocoid3();
	afx_msg void OnDecStart();
	afx_msg void OnDecStop();
	afx_msg void OnEncStart();
	afx_msg void OnEncStop();
	afx_msg void OnTest();
	afx_msg void OnSetDecBitrate();
	afx_msg void OnSetEncBitrate();
	afx_msg void OnSelectQcif();
	afx_msg void OnSelectCif();
	afx_msg void OnCompress1();
	afx_msg void OnCompress2();
	afx_msg void OnCompress3();
	afx_msg void OnCompress4();
	afx_msg void OnCompress5();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnAbout();
	//}}AFX_MSG

	afx_msg long OnChangedSource(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedSourceType(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedFeVideoFormat(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedBrightness(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedContrast(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedSaturation(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedHue(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedFeFilter(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedHScale(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedVScale(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedBeVideoFormat(WPARAM wParam, LPARAM lParam);	
   	afx_msg long OnChangedBeVideoType(WPARAM wParam, LPARAM lParam);	
	afx_msg long OnChangedAlarmEdge(WPARAM wParam, LPARAM lParam);	
   	afx_msg long OnChangedAlarmAck(WPARAM wParam, LPARAM lParam);	
   	afx_msg long OnChangedRelais(WPARAM wParam, LPARAM lParam);	
	
	afx_msg long OnChangedEncoderFormat(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedEncoderFramerate(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedEncoderBitrate(WPARAM wParam, LPARAM lParam);
	afx_msg long OnChangedDecoderBitrate(WPARAM wParam, LPARAM lParam);

	afx_msg long OnWmDecStart(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmDecStop(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmEncStart(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmEncStop(WPARAM wParam, LPARAM lParam);

	afx_msg long OnAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmCopyData(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmSleep(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

private:
	CWinThread* m_pEncodeThread;
	CWinThread* m_pDecodeThread;
	CWinThread* m_pTestThread;

	BOOL UpdateDlg();
	BOOL SetEncoderMode();
	void VideoDataService();
	void ProzessEncodeJobs();
	void ProzessDecodeJobs();
	void EncoderTimeOutProzessing();

	BOOL Login();
	BOOL SetDlgText(UINT nID, const char*	szText);

	CCoCo*					m_pCoCo;	// Pointer auf das CoCo-Objekt
	CMegra*					m_pMegra;	// Pointer auf das Megra-Objekt
	CIPCInputCoCoUnit*		m_pInput[MAX_COCO_ID + 1];				// Pointer auf CIPC-Input 
	CIPCOutputCoCoUnit*		m_pOutputCoCoCamera[MAX_COCO_ID + 1];	// Pointer auf CIPC-Output
	CIPCOutputCoCoUnit*		m_pOutputCoCoRelay[MAX_COCO_ID + 1];	// Pointer auf CIPC-Output
	CIPCOutputCoCoUnit*		m_pOutputMegraCamera;					// Pointer auf CIPC-Output

	CVideoJob*				m_pCurrentEncodeJob;	// Aktueller Encoderjob
	CVideoJob*				m_pCurrentDecodeJob;	// Aktueller Decoderjob
	CSemaVideoJobs			m_jobQueueEncode;		// Encoderjobqueue
	CSemaVideoJobs			m_jobQueueDecode;		// Decoderjobqueue

	CFile*					m_pRFile;	
	CFile*					m_pWFile[8];	
	BOOL					m_bOk;
	DWORD					m_dwReCnt;
	DWORD					m_dwTrCnt;
	BOOL					m_bStandalone;
	DWORD					m_dwDecBitrate;
	DWORD					m_dwEncBitrate;

	DWORD					m_dwEncodeCounter;
	BOOL					m_bNewEncodeJob;	// TRUE->Neuer Job
	CSettings				*m_pSettings;
	Resolution				m_PicFormat;	// LOW, MID, HIGH
	Compression				m_Compress;		// COMPRESSION_1...COMPRESSION_26
	BYTE					m_byCoCoID;		// Aktueller Codec

	CUser*					m_pUser;
	CPermission*			m_pPermission;

	HANDLE					m_hSyncSemaphore;					// for special communication with server
   	DWORD					m_dwWatchDogTimeout;				// Zeit bis CoCunit beendet wird, wenn keine Daten kommen
	DWORD					m_dwEncoderJobTimeout;				// Encoder Job Timeout in ms
	DWORD					m_dwDataReceivedTime[COCO_MAX_CAM]; // last compress time for every cam
	DWORD					m_dwLastWMCopyDataTime;				// Zeitpunkt der letzten WM_COPY_DATA message
	CCriticalSection		m_csEncode;
	CCriticalSection		m_csDecode;
};
