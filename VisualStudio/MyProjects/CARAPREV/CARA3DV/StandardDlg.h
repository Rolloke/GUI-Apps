#ifndef AFX_STANDARDDLG_H__1F2B2A07_170A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_STANDARDDLG_H__1F2B2A07_170A_11D2_9DB9_0000B458D891__INCLUDED_

// StandardDlg.h : Header-Datei
//
#include "NonModalDlg.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CStandardDlg 
#define STDDLG_GRIDSTEP       0x00000001
#define STDDLG_UPDATEGRIDSTEP 0x00000002
#define STDDLG_GRIDROUND      0x00000004
#define STDDLG_GRIDFONT       0x00000008
#define STDDLG_EFFECTS        0x00000010
#define STDDLG_OUTPUTPARAM    0x00000020
#define STDDLG_GRID_TYPE      0x00000040

class CStandardDlg : public CNonModalDlg
{
// Konstruktion
public:
	CStandardDlg(CWnd* pParent = NULL);   // Standardkonstruktor

   // Dialogfelddaten
   //{{AFX_DATA(CStandardDlg)
	enum { IDD = IDD_STANDARD };
   int      m_nNumMode;
   double   m_dGridStepX;
   double   m_dGridStepLevel;
   double   m_dGridStepZ;
   int      m_nRounding;
   int      m_nRoundingZ;
   int	   m_nNumModeZ;
   BOOL     m_bEffect;
	BOOL     m_bIntelliGrid;
	//}}AFX_DATA
   LOGFONT  m_GridFont;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CStandardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	bool   OnApplyNow();

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CStandardDlg)
   virtual void   OnOK();
   virtual BOOL   OnInitDialog();
   afx_msg void   OnGridFont();
	afx_msg void   OnOutputParam();
	afx_msg void   OnChangeGridStep();
	afx_msg void   OnReduce();
	afx_msg void   OnApplyNowCmd();
	afx_msg void OnStdIntelliGrid();
	afx_msg void OnStdNumMode();
	afx_msg void OnStdNumModeZ();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_STANDARDDLG_H__1F2B2A07_170A_11D2_9DB9_0000B458D891__INCLUDED_
