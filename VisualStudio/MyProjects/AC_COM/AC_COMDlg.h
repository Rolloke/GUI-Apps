// AC_COMDlg.h : Header-Datei
//

#if !defined(AFX_AC_COMDLG_H__37BC0AE4_C14D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_AC_COMDLG_H__37BC0AE4_C14D_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAC_COMDlg Dialogfeld
#include "CommPort.h"
#include "BasicParamDlg.h"
#include "Measurement.h"
#include "EvaluateMeasurements.h"

#define AC_SYSTEMCONFIG_TABS  7
#define AC_SYSTEMCONFIG_CTRLS 5

#define AC_TABLES_TABS        7
#define AC_TABLES_CTRLS       5

class CCurve;
class CEtsCDPlayer;

class PollMeasurementState
{
public:
   PollMeasurementState(){Reset();};
   void Reset()
   {
      Counter         = 0;
      Polling         = false;
      MeasureOnce     = false;
      PollingIsOk     = false;
      PollingWasOk    = false;
      MeasurementRuns = false;
   };
   DWORD Counter         :16; //  1
   DWORD Polling         : 1; // 17
   DWORD MeasureOnce     : 1; // 18
   DWORD PollingIsOk     : 1; // 19
   DWORD PollingWasOk    : 1; // 20
   DWORD MeasurementRuns : 1; // 21
   DWORD Dummy           :11; // 22
};

class CAC_COMDlg : public CDialog
{
// Konstruktion
public:
	CAC_COMDlg(CWnd* pParent = NULL);	// Standard-Konstruktor
   ~CAC_COMDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAC_COMDlg)
	enum { IDD = IDD_AC_COM_DIALOG };
	CComboBox m_cCD_PC;
	CComboBox m_cMeaningOfTestVal;
	CComboBox m_cPeakHold;
	CComboBox m_cCD_RoomC;
	CComboBox m_cCD_RoomB;
	CComboBox m_cCD_RoomA;
	CComboBox m_cPort;
	BOOL      m_bNetwork;
	BOOL      m_bAutoBalanceDeltaT;
	BOOL      m_bAutoBalancedBLevel;
	int		 m_nLoudspeakerWays;
	int		 m_nLevelMeter;
	int		 m_nFilterType;
	int		 m_nRT60;
	int		 m_nLoudspeakerMode;
   CString   m_strNewName;
	BOOL      m_bShowCurveImediately;
	int       m_nAC_Settings;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAC_COMDlg)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung

protected:
	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CAC_COMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnAcBtnGetdata();
	afx_msg void OnSelchangeAcComboCdA();
	afx_msg void OnSelchangeAcComboCdB();
	afx_msg void OnSelchangeAcComboCdC();
	afx_msg void OnDeltaposAcSpinCalibration(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAcTabTables(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingAcTabTables(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAcSpinMemoCounter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAcSpinBadPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAcSpinNoOfSoundsAuto(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAcSpinWeighting(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAcBtnShowCurves();
	afx_msg void OnSelchangeAcTabSystemConfig(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingAcTabSystemConfig(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAcComboPeakHoldTime();
	afx_msg void OnSelchangeAcComboTestPoints();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAppExit();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnHelpFinder();
	afx_msg void OnOptionsPort();
	afx_msg void OnAcBtnPutdata();
	afx_msg void OnChange();
	afx_msg void OnKillfocusAcEdtCurveName();
	afx_msg void OnAcCkNetwork();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAcBtnConfNetwork();
	afx_msg void OnAcBtnStartMeasurement();
	afx_msg void OnContextHelp();
	afx_msg void OnOptionsBasicParams();
	afx_msg void OnAcBtnStartMeasurementOnce();
	afx_msg void OnAcBtnEvaluateMeasurements();
	afx_msg void OnAcCkShowCurves();
	afx_msg void OnEditIrCodes();
	afx_msg void OnFileInsert();
	afx_msg void OnAcSettings();
	afx_msg void OnOptionsFrqRspCor();
	afx_msg void OnOptionsMsgBoxBehaviour();
	afx_msg void OnViewCdPlayer();
	virtual void OnCancel();
	//}}AFX_MSG
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnViewTabs(UINT);
//   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()


private:
	void SaveData();
	void ReadData();
	void SaveChangedData();
	void ShowTextLonger(DWORD);
	void SetData(bool);

   void SetStatusText(int);
	void SetStatusText(const char*);
   void ReportTransmissionError(int);

	void InitDlgCtrls();
   void DeleteProcessInfo(PROCESS_INFORMATION*);

	bool OpenPort();
   int  GetDataFromAC();
	int  SendDataToAC();

   void NotYetImplemented();

	void PollMeasurementEvent(UINT);
   void StopMeasurement();
	void DeleteMeasurements();

   void DestroyCDPlayer(bool);

   LRESULT OnMsgFrom2DView(WPARAM, LPARAM);
   LRESULT OnMenuSelect(WPARAM, LPARAM);
   LRESULT OnDropFiles(WPARAM, LPARAM);
   LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnEnterMenuLoop(WPARAM, LPARAM);

   static BOOL CALLBACK CloseThreadWindow(HWND, LPARAM);
   static LRESULT CALLBACK SubClassProc(HWND,UINT, WPARAM, LPARAM);
   static BOOL CALLBACK EnumChildWndFunc(HWND, LPARAM);
   static BOOL CALLBACK ChildWndFromPt(HWND, LPARAM);

	HICON m_hIcon;

   CAudioControlData*m_pACData;
   CCommPort         m_CommPort;
   CEtsCDPlayer     *m_pCDPlayer;

   short             m_nDisplayDivX;
   bool              m_bHitDisplayDivX;

   bool        m_bDataOk;
   BYTE        m_bDataChanged;
   bool        m_bWaitErrorTime;
   bool        m_bUpdateOnGetData;
   int         m_nLastStatusTextID;

   CEvaluateMeasurements m_EvalMeasurement;
   PollMeasurementState m_PMState;
   int         m_nMeasureNo;

   CString     m_strFileName;

   CPtrList    m_Timers;

   static int  gm_pnTabTables[AC_TABLES_TABS][AC_TABLES_CTRLS];
   static int  gm_pnTabSystemConfig[AC_SYSTEMCONFIG_TABS][AC_SYSTEMCONFIG_CTRLS];
   static HWND gm_hWndActualHit;
public:
	void ShowPlot(int, CPlot *, bool);
   void KillTimerSave(UINT nID);
	UINT SetTimerSave(UINT, UINT, void (CALLBACK EXPORT*)(HWND, UINT, UINT, DWORD));
	void ShowCurve(int );
	void CalcChecksum(CFileHeader &);
   bool SetMenuItem(UINT nItem, UINT nCheck, UINT nEnable, HMENU hMenu=NULL);
	void SetDataChanged(BYTE bReset=0);

   CPtrList    m_CalledProcess;

   CMeasurementList m_Measurements;
   CBasicParamDlg m_BasicParam;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_AC_COMDLG_H__37BC0AE4_C14D_11D4_B6EC_0000B458D891__INCLUDED_)
