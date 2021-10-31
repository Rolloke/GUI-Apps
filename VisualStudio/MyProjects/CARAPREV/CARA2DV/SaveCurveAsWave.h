#if !defined(AFX_SAVECURVEASWAVE_H__732C2F42_AE18_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_SAVECURVEASWAVE_H__732C2F42_AE18_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveCurveAsWave.h : Header-Datei
//
#include <mmsystem.h>
class CCurveLabel;
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSaveCurveAsWave 

class CSaveCurveAsWave : public CFileDialog
{
	DECLARE_DYNAMIC(CSaveCurveAsWave)
// Konstruktion
public:
	CSaveCurveAsWave(BOOL bOpenFileDialog, // TRUE für FileOpen, FALSE für FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
   virtual ~CSaveCurveAsWave();
// Dialogfelddaten
	//{{AFX_DATA(CSaveCurveAsWave)
	CSpinButtonCtrl	m_cCurveCountSpin;
	CComboBox   m_cRightChannel;
	CComboBox   m_cLeftChannel;
	int         m_nBlockAlign;
	int         m_nSampleRate;
	int         m_nChannels;
	int         m_nLeftChannel;
	int         m_nRightChannel;
	//}}AFX_DATA
   int   m_nFileTypeMax;
   int   m_nFileTypeSel;

   double	    m_dCurve1Min;
   double	    m_dCurve1Max;
   double	    m_dCurve2Min;
   double	    m_dCurve2Max;

   double	    m_dCurve1Offset;
   double	    m_dCurve1Level;
   double	    m_dCurve2Offset;
   double	    m_dCurve2Level;

   float        m_fFrequency;
	float	       m_fWaveTime;
   long         m_nCurveValues;
   long         m_nCurveCount;
	long         m_nFileSize;
   bool         m_bOffsetOrLevel1Changed;
   bool         m_bMinOrMax1Changed;
   bool         m_bOffsetOrLevel2Changed;
   bool         m_bMinOrMax2Changed;
   CCurveLabel *m_pclLeftIn;
   CCurveLabel *m_pclRightIn;
   CCurveLabel *m_pclLeftOut;
   CCurveLabel *m_pclRightOut;
   BOOL         m_bConcat1stValue;
   WAVEFORMATEX m_wfEX;
   long         m_nSampleLength;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSaveCurveAsWave)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void GetLevelAndOffset();
   void GetMinMaxValues();
	void GetOffset(CCurveLabel*, double&);
	void OnSetChannels();
	void GetMinMaxZeroLevel(CCurveLabel*, double&, double&);
	void CalcFileSize();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSaveCurveAsWave)
	virtual BOOL OnInitDialog();
   virtual BOOL OnFileNameOK();
	afx_msg void OnWdCannels();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeParams();
	afx_msg void OnWdDefaultSettings();
	afx_msg void OnChangeWdCurveCount();
	afx_msg void OnKillfocusOffsetAndLevel();
	afx_msg void OnKillfocusWdCurveMinMax();
   afx_msg void OnChangeOffsetAndLevel1();
   afx_msg void OnChangeOffsetAndLevel2();
   afx_msg void OnChangeMinMax1();
   afx_msg void OnChangeMinMax2();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	static BOOL CALLBACK EnableItems(HWND hwnd, LPARAM lParam);
   void GetCurSelFileType(long);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
   static long gm_lOldProc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SAVECURVEASWAVE_H__732C2F42_AE18_11D3_B6EC_0000B458D891__INCLUDED_
