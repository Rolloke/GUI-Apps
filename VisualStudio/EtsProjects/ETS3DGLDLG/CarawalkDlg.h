#if !defined(AFX_CARAWALKDLG_H__AF802220_830D_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_CARAWALKDLG_H__AF802220_830D_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarawalkDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCarawalkDlg 

class CCarawalkDlg : public CDialog
{
// Konstruktion
public:
	CCarawalkDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	~CCarawalkDlg();

// Dialogfelddaten
	//{{AFX_DATA(CCarawalkDlg)
	enum { IDD = IDD_CARAWALK_DLG };
	int		m_nDepthBits;
	BOOL	   m_bGenericAccelerated;
	int		m_nStencilBits;
	int		m_nColorBits;
	int		m_nAlphaBits;
	int		m_nAccumulationBits;
	int		m_nRefreshTime;
	int		m_nPrespectiveAngle;
	float	   m_fAmbientColFact;
	float	   m_fAvgColFrom;
	float	   m_fAvgColTo;
	//}}AFX_DATA
   COLORREF m_cTexAmbient;
   COLORREF m_cTexDiffuse;
   COLORREF m_cTexSpecular;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCarawalkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCarawalkDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTexAmbient();
	afx_msg void OnTexDiffuse();
	afx_msg void OnTexSpecular();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CARAWALKDLG_H__AF802220_830D_11D3_B6EC_0000B458D891__INCLUDED_
