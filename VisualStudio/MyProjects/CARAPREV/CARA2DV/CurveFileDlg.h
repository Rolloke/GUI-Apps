#if !defined(AFX_CURVEFILEDLG_H__C0C06CE1_3D19_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_CURVEFILEDLG_H__C0C06CE1_3D19_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveFileDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveFileDlg 

#define HIGH_PRECISION   0
#define NORMAL_PRECISION 1
#define LOW_PRECISION    2
#define WAVE_DIRECT      3

class CCurveFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CCurveFileDlg)

public:
	CCurveFileDlg(BOOL bOpenFileDialog, // TRUE für FileOpen, FALSE für FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

   virtual BOOL OnFileNameOK();
   virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

   int   m_nPrecision;
   BOOL  m_bSigned;
   int   m_nFileTypeSel;
   int   m_nFileTypeMax;
protected:
	//{{AFX_MSG(CCurveFileDlg)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void GetCurSelFileType(long);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
   static long gm_lOldProc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CURVEFILEDLG_H__C0C06CE1_3D19_11D4_B6EC_0000B458D891__INCLUDED_
