#if !defined(AFX_WAVEPROPERTIES_H__EDF7AC80_C417_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_WAVEPROPERTIES_H__EDF7AC80_C417_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaveProperties.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWaveProperties 

#define WAVE_BITS_8           0
#define WAVE_BITS_16          1

#define WAVE_SAMPLE_FRQ_11025 0
#define WAVE_SAMPLE_FRQ_22050 1
#define WAVE_SAMPLE_FRQ_44100 2

#define WAVE_CHANNEL_MONO_L         0
#define WAVE_CHANNEL_MONO_R         1
#define WAVE_CHANNEL_STEREO_LR      2
#define WAVE_CHANNEL_STEREO_LR_INV  3

class CWaveProperties : public CDialog
{
// Konstruktion
public:
	CWaveProperties(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CWaveProperties)
	enum { IDD = IDD_WAVE_PROPERTIES };
	int		m_nBits;
	int		m_nFrequency;
	BOOL	   m_bStereo;
	double	m_dMaxMagnitudeDB_L;
	double	m_dMaxMagnitudeDB_R;
	double	m_dMaxMagnitude_L;
	double	m_dMaxMagnitude_R;
	int		m_nInput;
	BOOL	m_bLInvert;
	BOOL	m_bRInvert;
	BOOL	m_bRMSPegel;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CWaveProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CWaveProperties)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeValues();
	virtual void OnOK();
	afx_msg void OnKillfocusWaveMaxMagnitudeL();
	afx_msg void OnKillfocusWaveMaxMagnitudeDbL();
	afx_msg void OnKillfocusWaveMaxMagnitudeR();
	afx_msg void OnKillfocusWaveMaxMagnitudeDbR();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   int m_nSave;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_WAVEPROPERTIES_H__EDF7AC80_C417_11D3_B6EC_0000B458D891__INCLUDED_
