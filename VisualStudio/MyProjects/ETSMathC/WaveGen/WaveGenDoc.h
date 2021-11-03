// WaveGenDoc.h : Schnittstelle der Klasse CWaveGenDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEGENDOC_H__349819BC_B864_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_WAVEGENDOC_H__349819BC_B864_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CaraDoc.h"

#include "Filter.h"
#include "MathCurve.h"
#include "MemoryMappedFile.h"

#define UPDATE_INSERT_CONTENTS         0x1000
#define UPDATE_INSERT_FILTERFUNCTION   0x2000
#define UPDATE_CHANGE_PLOTS            0x3000
#define UPDATE_CHANGE_CONTENTS         0x4000

#define UPDATE_INSERT_HISTORY          0x8000
#define UPDATE_HISTORY_FILTER      0x10000000

class CWaveGenDoc : public CCARADoc
{
   struct sWave16
   {
      short left, right;
   };

   struct sWave8
   {
      char left, right;
   };

protected: // Nur aus Serialisierung erzeugen
	CWaveGenDoc();
	DECLARE_DYNCREATE(CWaveGenDoc)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CWaveGenDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementierung
public:
	bool     IsArrayOk();
	void     CheckFunktionValues();
	void     Init();
	void     CalculateFilter(FilterBox*, UINT);
	void     CalcFilterArray();
	int      GetNoOfFrqSteps();
	double   GetMinFrqStep();
	Complex  PinkNoiseFilter(double, double);
   void     OnCalcFourierThread();
	void     CalcCancel(bool bBreak);
	void     SetArraySizes(bool, bool);
	void     ComplexFFT(int,int,int);

   static unsigned int __stdcall OnCalcNormFrqThread(void *);
   static unsigned int __stdcall OnCalcRandomThread(void*);
   static unsigned int __stdcall OnCalcRandomFrqThread(void*);
   static unsigned int __stdcall OnCalcFourierThread(void *);
   static void MathFilter(Complex, float&, float&, int);

	virtual ~CWaveGenDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void UpdateFrameCounts();

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CWaveGenDoc)
	afx_msg void OnCalcFourier();
	afx_msg void OnCalcRandom();
	afx_msg void OnUpdateCalcNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalcFourier(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalcFilter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalcReFourier(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsFilter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsWavefile(CCmdUI* pCmdUI);
	afx_msg void OnOptionsFilter();
	afx_msg void OnOptionsWavefile();
	afx_msg void OnCalcRandomFrq();
	afx_msg void OnOptionsStandard();
	afx_msg void OnUpdateOptionsStandard(CCmdUI* pCmdUI);
	afx_msg void OnCalcMathFilter(UINT);
	afx_msg void OnUpdateCalcMathFilter(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadLeftcurve();
	afx_msg void OnUpdateFileLoadLeftcurve(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadRightcurve();
	afx_msg void OnUpdateFileLoadRightcurve(CCmdUI* pCmdUI);
	afx_msg void OnCalcNormFrq();
	afx_msg void OnOptionsFilterEdit();
	afx_msg void OnUpdateOptionsFilterEdit(CCmdUI* pCmdUI);
	afx_msg void OnCalcCancel();
	afx_msg void OnUpdateCalcCancel(CCmdUI* pCmdUI);
	afx_msg void OnPlayWave();
	afx_msg void OnUpdatePlayWave(CCmdUI* pCmdUI);
	afx_msg void OnPlayStop();
	afx_msg void OnUpdatePlayStop(CCmdUI* pCmdUI);
	afx_msg void OnRepeatWave();
	afx_msg void OnUpdateRepeatWave(CCmdUI* pCmdUI);
	afx_msg void OnCalcTimeFunction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString & GetTitle();
	static void ProgressCallback(void*, int);
	__WAVEFORMATEX m_wfEX;

   CMemoryMappedFile m_mmfWave;           // Array für das Wave File
   CMemoryMappedFile m_mmfFFT;            // Array für die Transformation

   CMemoryMappedFile m_mmfL;              // Array für die Linke Seite
	double       m_dMaxMagnitudeL;
	double       m_dDCOffsetL;
   double       m_dMaxValueL;
   double       m_dRMS_ValueL;
   bool         m_bInvertL;
   bool         m_bFourierArrayAttachedL;

   CMemoryMappedFile m_mmfR;              // Array für die rechte Seite
   double       m_dMaxMagnitudeR;
	double       m_dDCOffsetR;
   double       m_dMaxValueR;
   double       m_dRMS_ValueR;
   bool         m_bInvertR;
   bool         m_bFourierArrayAttachedR;

   int          m_nOrder;                 // beide Kanäle
	int          m_nValues;
	int          m_nFFTValues;
   int          m_nInput;
   FilterBox    m_FilterL;
   double       m_dBandWidth;
   double       m_dFilterM;
   long         m_lWaveFileSize;
   CMathCurve  *m_pFilterCurve;

   int          m_nLoadOption;
   bool         m_bTimeFunction;          // Steuerung
   bool         m_bTimeFktChanged;
	bool         m_bShowPlot;
   bool         m_bRepeatWave;

private:
   HANDLE       m_hThread;
   unsigned int m_nThreadID;
   bool         m_bNewDocContent;
   CString      m_strHistory;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_WAVEGENDOC_H__349819BC_B864_11D3_B6EC_0000B458D891__INCLUDED_)
