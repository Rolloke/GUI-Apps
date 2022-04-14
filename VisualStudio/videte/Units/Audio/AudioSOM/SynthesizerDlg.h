#pragma once


// CSynthesizerDlg dialog
#define WAVEFORM_SINE         0
#define WAVEFORM_RECT         1
#define WAVEFORM_TRIANGLE     2
#define WAVEFORM_SWEEP        3
#define WAVEFORM_WHITE_NOISE  4
#define WAVEFORM_PINK_NOISE   5


class CSynthesizerDlg : public CDialog
{
   friend class CAudioSOMDlg;
	DECLARE_DYNAMIC(CSynthesizerDlg)
   friend class CAudioSOMDlg;

public:
	CSynthesizerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSynthesizerDlg();

// Dialog Data
public:
	enum { IDD = IDD_SYNTHESIZER };
protected:
   BOOL m_nWaveForm;
   BOOL m_bCorrelated;
   int  m_nFrequency1;
   int  m_nFrequency2;
   int  m_nTime;
   int  m_nPhase;
   int  m_nMagnitude1;
   int  m_nMagnitude2;
   int  m_nOffset1;
   int  m_nOffset2;
   int  m_nMaxFrequency;

protected:
   CImageList m_Images;
public:
   void SetParentWnd(CWnd*pParent);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   virtual void OnOK();

	//{{AFX_MSG(CSynthesizerDlg)
   afx_msg void OnBnClickedBtnWfKorr();
   afx_msg void OnBnClickedBtnWaveForm();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnEnKillFocusEdt(UINT nID);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void DDX_SliderMinMaxInt(CDataExchange* pDX, UINT nIDedt, UINT nIDslider, int &nValue);
   void DDX_Text10(CDataExchange* pDX, UINT nIDedt, int &nValue);
   void UpdateCtrls();
   void UpdateSynthesizer(UINT nID);

protected:
};
