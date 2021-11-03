#ifndef AFX_PICTUREPROPPAGE_H__67210402_220A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PICTUREPROPPAGE_H__67210402_220A_11D2_9DB9_0000B458D891__INCLUDED_

// PicturePropPage.h : Header-Datei
//
#include "PropertyPreview.h"
#include "CaraPropertyPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPicturePropPage 
class CPictureLabel;

class CPicturePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CPicturePropPage)

// Konstruktion
public:
   CPicturePropPage();
   virtual ~CPicturePropPage();
   void SetLabel(CLabel*);

// Dialogfelddaten
	//{{AFX_DATA(CPicturePropPage)
	enum { IDD = IDD_PROPPAGE_DIBLABEL };
	CPropertyPreview m_Preview;
	BOOL             m_bHalftone;
	int              m_nScaling;
	BOOL             m_bShiny;
	BOOL	           m_nOutput;
	BOOL             m_bFile;
	CString	m_strFileName;
	//}}AFX_DATA

private:
   void SetMode();
   int             m_nOldMode;
   int             m_nOldScale;
   DWORD           m_dwOldStretchBltMode;
   CPictureLabel  *m_pLabel;
   bool            m_bNoFile;
// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPicturePropPage)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPicturePropPage)
	afx_msg void OnBnClicked();
	virtual BOOL OnInitDialog();
	afx_msg void OnPictFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PICTUREPROPPAGE_H__67210402_220A_11D2_9DB9_0000B458D891__INCLUDED_
