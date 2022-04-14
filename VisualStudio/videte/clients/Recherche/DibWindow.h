// DibWindow.h: interface for the CDibWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIBWINDOW_H__1B876A33_43BF_11D2_B59E_00C095EC9EFA__INCLUDED_)
#define AFX_DIBWINDOW_H__1B876A33_43BF_11D2_B59E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImageWindow.h"

class CDibData;

class CDibWindow : public CImageWindow  
{
	// construction/destruction
public:
	CDibWindow();
	virtual ~CDibWindow();

	// attributes
public:
	inline const CString&  GetFileName() const;
	virtual	CSecID	GetID();
	virtual CString GetTitleText(CDC* pDC);
	virtual	BOOL	IsDibWindow();


	// overridables
public:
	virtual CString FormatPicture(BOOL bTabbed = FALSE);
	virtual CString FormatData(const CString& sD,const CString& sS);
	virtual int GetCameraNr();
	virtual CString GetDate();
	virtual CString GetTime();
	virtual CString FormatComment();
	virtual int		GetRecNo();
	virtual int		GetRecCount();
	virtual BOOL	Navigate(int nRecNr,int nRecCount = 0);
	virtual	CSize	GetPictureSize();
	virtual CString	GetName();
	virtual CString GetCompression();

	// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, CRechercheView* pParentWnd);
	BOOL LoadFile(const CString& sFileName);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDibWindow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDibWindow)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// overrides
protected:
	virtual void OnDraw(CDC* pDC);
	virtual UINT GetToolBarID();
	virtual void OnSetWindowSize();
	virtual int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE);
	virtual void PopupMenu(CPoint pt);

	// implementation	
protected:
	BOOL LoadJPG(const CString& sFileName);
	BOOL LoadJPX(const CString& sFileName);
	BOOL LoadDIB(const CString& sFileName);
	void DateTimeFromInfoString(const CString&  sInfoString1);

private:
	CDibData*	m_pDibData;
	CString		m_sPathname;
	CString		m_sDate;
	CString		m_sTime;
	CString		m_sCompression;
	int			m_iCameraNr;
	CJpeg		m_Jpeg;
};
inline const CString& CDibWindow::GetFileName() const
{
	return m_sPathname;
}

#endif // !defined(AFX_DIBWINDOW_H__1B876A33_43BF_11D2_B59E_00C095EC9EFA__INCLUDED_)
