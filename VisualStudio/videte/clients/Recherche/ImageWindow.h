#if !defined(AFX_IMAGEWINDOW_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_)
#define AFX_IMAGEWINDOW_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageWindow.h : header file
//

#include "SmallWindow.h"
/////////////////////////////////////////////////////////////////////////////
// CImageWindow window

class CImageWindow : public CSmallWindow
{
// Construction
public:
	CImageWindow(CServer* pServer);
	virtual ~CImageWindow();

// Attributes
public:
	inline		 CIPCFields& GetFields();

// Operations
public:

	// abstract overrides
public:
	virtual int		GetCameraNr()=0;
	virtual CString GetDate()=0;
	virtual CString GetTime()=0;
	virtual CString GetCompression()=0;
	virtual CString FormatPicture(BOOL bTabbed = FALSE)=0;
	virtual CString FormatComment()=0;
	virtual int		GetRecNo()=0;
	virtual int		GetRecCount()=0;
	virtual BOOL	Navigate(int nRecNr,int nRecCount = 0)=0;
	virtual	CSize	GetPictureSize()=0;

	// overrides
public:
	virtual	BOOL	IsImageWindow();
	virtual	void	PrintDIBs(CDC* pDC,CRect rect,int iSpace) {};
	virtual CString FormatData(const CString& sD,const CString& sS);
	virtual int		GetNrDibs() {return 1;};
	virtual void GetVideoClientRect(LPRECT lpRect); 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageWindow)
	//}}AFX_VIRTUAL


	// Implementation
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
			void DrawCinema(CDC* pDC);
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
	// Generated message map functions
protected:
	//{{AFX_MSG(CImageWindow)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIPCFields		m_Fields;
	CFieldDialog*	 m_pFieldDialog;
	CNavigationDialog* m_pNavigationDialog;

	friend class CFieldDialog;
	friend class CNavigationDialog;
	friend class CRechercheView;
};

/////////////////////////////////////////////////////////////////////////////
inline CIPCFields& CImageWindow::GetFields()
{
	return m_Fields;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEWINDOW_H__1B533E43_014D_11D3_B5E1_004005A19028__INCLUDED_)
