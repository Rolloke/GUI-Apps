// WndImageRecherche.h : header file
//

#if !defined(AFX_WndImageRecherche_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_)
#define AFX_WndImageRecherche_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndImage.h"

class CQueryRectTracker;
class CDlgField;
class CDlgNavigation;

/////////////////////////////////////////////////////////////////////////////
// CWndImageRecherche window
class CWndImageRecherche : public CWndImage
{
	DECLARE_DYNAMIC(CWndImageRecherche)

// Construction
public:
	CWndImageRecherche(CServer* pServer);
	virtual ~CWndImageRecherche();

// Attributes
public:
	inline		 CIPCFields& GetFields();

// Operations
public:

	void EnableDlgFields(BOOL bEnable);
	// abstract overrides
public:

	virtual int		GetCameraNr() PURE_VIRTUAL(0)
	virtual CString GetDate() PURE_VIRTUAL(_T(""))
	virtual CString GetTime() PURE_VIRTUAL(_T(""))
	virtual CString GetCompression() PURE_VIRTUAL(_T(""))
	virtual CString FormatPicture(BOOL bTabbed = FALSE) PURE_VIRTUAL(_T(""))
	virtual CString FormatComment() PURE_VIRTUAL(_T(""))
	virtual int		GetRecNo() PURE_VIRTUAL(0)
	virtual int		GetRecCount() PURE_VIRTUAL(0)
	virtual BOOL	Navigate(int nRecNr,int nRecCount = 0) PURE_VIRTUAL(0)
	virtual	CSize	GetPictureSize() PURE_VIRTUAL(0)
	virtual UINT	GetToolBarID() PURE_VIRTUAL(0)

	// overrides
public:
	virtual void	GetFrameRect(LPRECT lpRect);
	virtual	BOOL	IsImageWindow();
	virtual	void	PrintDIBs(CDC* pDC,CRect rect,int iSpace) {};
	virtual CString FormatData(const CString& sD,const CString& sS);
	virtual int		GetNrDibs() {return 1;};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndImageRecherche)
	//}}AFX_VIRTUAL


	// Implementation
public:
	BOOL CanShowImageInOriginalSize();
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
			void UpdateDialogs();

	// printing
	virtual int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE)=0;
	int  PrintInfoRectLandscape(CDC* pDC,CRect rect, CFont* pFont);
	void PrintTitleRectPortrat(CDC* pDC,CRect rect, CFont* pFont);
	void PrintInfoRectPortrat(CDC* pDC,CRect rect, 
							  CFont* pFont, int iSpace,
							  const CString& sPicture,
							  const CString& sData,
							  const CString& sComment);
	void	EnableNavigationSlider(BOOL bEnable);
	// Generated message map functions
protected:
	//{{AFX_MSG(CWndImageRecherche)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIPCFields			m_Fields;
	CDlgField*		m_pDlgField;
	CDlgNavigation*	m_pDlgNavigation;

	friend class CDlgField;
	friend class CDlgNavigation;
	friend class CViewIdipClient;
};

/////////////////////////////////////////////////////////////////////////////
inline CIPCFields& CWndImageRecherche::GetFields()
{
	return m_Fields;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WndImageRecherche_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_)
