// AudioSOMDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "synthesizerdlg.h"
#include "cololine.h"

interface IGraphBuilder;
interface ICaptureGraphBuilder2;
interface IBaseFilter;
interface IMediaControl;
interface IMediaEventEx;
interface IBasicAudio;
interface ISynthesizer;
interface IMediaSample;

// CAudioSOMDlg dialog
class CAudioSOMDlg : public CDialog
{
// Construction
public:
	CAudioSOMDlg(CWnd* pParent = NULL);	// standard constructor

protected:
// Dialog Data
	enum { IDD = IDD_AUDIOSOM_DIALOG };
	HICON m_hIcon;
   CComboBox m_cAudioInput;
   CComboBox m_cInputPin;
   CComboBox m_cAudioOutput;
   CComboBox m_cSampleLength;
   CComboBox m_cAudioInputCaps;
   CComboBox m_cOutputFrq;
   CSliderCtrl m_cVolume;
   CSliderCtrl m_cBalance;
   CSliderCtrl m_cInputLevel;
   CSliderCtrl m_cMediaPosition;
   BOOL        m_bURL_Input;
   BOOL        m_bURL_Output;
   int         m_nOszi;
   int         m_nFileOut;
   int         m_nAudioOut;
   CString     m_sInputFileName;
   CString     m_sOutputFileName;
   CFile       m_OutputFile;

   CPen m_penOszi;
   CColorLine m_line_Osz1;
   CColorLine m_line_Osz2;
   CColorLine m_line_Osz3;
   CColorLine m_line_Osz4;
   CPen m_penAO;
   CColorLine m_line_AO1;
   CColorLine m_line_AO2;
   CColorLine m_line_AO3;
   CColorLine m_line_AO4;
   CPen m_penFO;
   CColorLine m_line_FO1;
   CColorLine m_line_FO2;
   CColorLine m_line_FO3;
   CColorLine m_line_FO4;

   int m_nAudioInput;
   int m_nAudioOutput;

	IGraphBuilder	*m_pGB;						// Capture Graph
	ICaptureGraphBuilder2 *m_pCaptureGB;	// Graph Builder	

	IMediaControl  *m_pMC;						// Media Control interface
	IMediaEventEx  *m_pME;						// Media Event interface

	IBaseFilter		*m_pInput;				   // Audio Input
	IBaseFilter		*m_pOsciloscope;			// Osciloscope

   IBaseFilter		*m_pSynthesizer;		   // Synthesizer
   ISynthesizer   *m_ISynthesizer;
   IBaseFilter		*m_pOutput;				   // Audio Output
   IBasicAudio    *m_pVolume;

   IBaseFilter		*m_pFileIn;    		   // File Input
   IBaseFilter		*m_pEncode;		         // File Output encoder
   IBaseFilter		*m_pFileOut;		      // File Output

   IBaseFilter		*m_pInfTeeInput;			// InfTee filter
   IBaseFilter		*m_pInfTeeSynth;			// InfTee filter
   IBaseFilter		*m_pInfTeeFile;			// InfTee filter
   CSynthesizerDlg m_SynthesizerDlg;
   DWORD          m_dwROT;
   UINT           m_nUpdatePositionTimer;

	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
   void UpdateSynthesizer(UINT nID);

protected:
   void ReportError(ErrorStruct*ps, bool bMsgBox=true);
	void InitGraphBuilder();
	void ReleaseGraph();
   void ReleaseInputPins();
	void UpdateState();
	void UpdateVolume();
   void InitAudioIn();
   void InitSynthesizer();
   void InitFileIn();
   void ConnectToFileOut(IBaseFilter*pFilterIn, int nPinIn);
   void ConnectToOsziloscope(IBaseFilter*pFilterIn, int nPinIn);
   void ConnectToAudioOut(IBaseFilter*pFilterIn, int nPinIn);
	void ReleaseSynth();
	void ReleaseAudioIn();
	void ReleaseFileIn();
	void ReleaseOutputs();
   void EnableDlgItem(UINT nID, BOOL bEnable=TRUE);
   static HRESULT WINAPI ReceiveMediaSample(IMediaSample*, long, long);

	// Generated message map functions
	//{{AFX_MSG(CAudioSOMDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnDestroy();
   afx_msg void OnBnClickedBtnInitialize();
   afx_msg void OnBnClickedBtnStart();
   afx_msg void OnBnClickedBtnStop();
   afx_msg void OnCbnSelchangeComboAudioInput();
   afx_msg void OnCbnSelchangeComboAudioInputPin();
   afx_msg void OnCbnSelchangeComboOutputFrq();
   afx_msg void OnBnClickedBtnSaveSettings();
   afx_msg void OnBnClickedBtnResetAll();
   afx_msg void OnBnClickedBtnshowSynth();
   afx_msg void OnCbnSelchangeComboAudioOutput();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnBnClickedBtnshowSynthFilter();
   afx_msg void OnRDOutClicked(UINT uID);
   afx_msg void OnBnClickedCkRot();
   afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedBtnFileOutput();
   afx_msg void OnBnClickedBtnFileInput();
   CComboBox m_cOutputBits;
   CComboBox m_cInputBits;
};
