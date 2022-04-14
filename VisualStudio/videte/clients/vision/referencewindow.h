// ReferenceWindow.h: interface for the CReferenceWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFERENCEWINDOW_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_)
#define AFX_REFERENCEWINDOW_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmallWindow.h"

class CReferenceWindow : public CSmallWindow  
{
	// construction / destruction
public:
	CReferenceWindow(CServer* pServer, CSecID id, const CString& sCameraName);
	virtual ~CReferenceWindow();

public:
				 CSecID	  GetID();
	inline const CString& GetCameraName() const;

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CVisionView* pParentWnd);
	void Reload();

	// Overrides
public:
	virtual	BOOL	IsReferenceWindow();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReferenceWindow)
	//}}AFX_VIRTUAL
protected:
	virtual void OnDraw(CDC* pDC);
	virtual CString GetTitleText(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CReferenceWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sCameraName;
	CSecID  m_idCamera;
	CJpeg	m_Jpeg;
};
///////////////////////////////////////////////////////////////
inline const CString& CReferenceWindow::GetCameraName() const
{
	return m_sCameraName;
}

#endif // !defined(AFX_REFERENCEWINDOW_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_)
