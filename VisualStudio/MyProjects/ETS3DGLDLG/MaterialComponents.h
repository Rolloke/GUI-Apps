#if !defined(AFX_MATERIALCOMPONENTS_H__606D4122_7F49_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_MATERIALCOMPONENTS_H__606D4122_7F49_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MaterialComponents.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CMaterialComponents 

class CMaterialComponents : public CDialog
{
// Konstruktion
public:
	CString m_strBmpPath;
	bool SaveMaterial();
	CMaterialComponents(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CMaterialComponents)
	enum { IDD = IDD_MATERIAL_CMP };
	BOOL  m_bBorder;
	BOOL  m_bGenerateCoordinates;
	float m_fShinines;
	float m_fsScaleFactor;
	float m_ftScaleFactor;
	//}}AFX_DATA

	int   m_nMatNo;
	float	m_pfAmbient[4];
	float	m_pfDiffuse[4];
	float	m_pfEmission[4];
	float	m_pfSpecular[4];
   COLORREF m_cBorder;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMaterialComponents)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CMaterialComponents)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBorderColor();
	afx_msg void OnAmbientLight();
	afx_msg void OnDiffuseLight();
	afx_msg void OnSpecularLight();
	afx_msg void OnEmissionLight();
	afx_msg void OnBitmapFile();
	afx_msg void OnSelectMaterial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MATERIALCOMPONENTS_H__606D4122_7F49_11D4_B6EC_0000B458D891__INCLUDED_
