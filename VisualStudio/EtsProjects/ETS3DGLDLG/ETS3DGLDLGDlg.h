// ETS3DGLDLGDlg.h : Header-Datei
//

#if !defined(AFX_ETS3DGLDLGDLG_H__2C3EF284_8225_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS3DGLDLGDLG_H__2C3EF284_8225_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGDlg Dialogfeld

class CETS3DGLDLGDlg : public CDialog
{
// Konstruktion
public:
	~CETS3DGLDLGDlg();
	CETS3DGLDLGDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CETS3DGLDLGDlg)
	enum { IDD = IDD_ETS3DGLDLG_DIALOG };
	BOOL	m_bBugFixBufferSize;
	BOOL	m_bBugFixLightTexture;
	BOOL	m_bBugFixMoveWindow;
	BOOL	m_bBugFixNormalVector;
	BOOL	m_bCaraWalkAccelerated;
	BOOL	m_bEts3DViewAccelerated;
	int   m_nPolygonOffset;
	float	m_fPolygonFactor;
	int   m_nBugFixBuffer;
	BOOL	m_bCaraWalkBmp;
	BOOL	m_bCaraWalkDoubleBuffer;
	BOOL	m_bEts3DVBmp;
	BOOL	m_bEts3DVDoubleBuffer;
	int   m_nPixelFormat;
	BOOL	m_bCaraWalkShadows;
	BOOL	m_bInit3Dalways;
	//}}AFX_DATA

   bool m_bSpecial;
   bool m_bDebugFileChanged;
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DGLDLGDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
// Implementierung
protected:
	void  WriteRegistryKeys();
	bool  UpdateInfo();
	static BOOL CALLBACK EnumChildProc(HWND, LPARAM);
	HICON m_hIcon;
   SIZE  m_szClient;
   int   m_nLineOfHeading;
   SIZE  m_szEdit;
	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CETS3DGLDLGDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnDefault();
	afx_msg void OnHelp();
	afx_msg void OnReport();
	afx_msg void OnCarawalkAccelerated();
	afx_msg void OnCarawalkBmp();
	afx_msg void OnEts3dvAccelerated();
	afx_msg void OnEts3dvBmp();
	afx_msg void OnApply();
	afx_msg void OnInfo();
	afx_msg void OnChange();
	afx_msg void OnCarawalkDoubleBuffer();
	afx_msg void OnEts3dvDoubleBuffer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   struct ShowControls
   {
      RECT rcEdit;
      HWND hwndEdit;
      int  nShowCmd;
   };

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS3DGLDLGDLG_H__2C3EF284_8225_11D3_B6EC_0000B458D891__INCLUDED_)
