// TextWindow.h: interface for the CTextWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTWINDOW_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_)
#define AFX_TEXTWINDOW_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmallWindow.h"

class CTextWindow : public CSmallWindow  
{
	// construction/destruction
public:
	CTextWindow(CServer* pServer, const CSecID id, const CString& sMessage);
	virtual ~CTextWindow();

public:
	inline const CString& GetText() const;

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CVisionView* pParentWnd);
	void SetNotificationText(const CString& sMessage);

	// Overrides
public:
	virtual	BOOL	IsTextWindow();
	virtual	CSecID	GetID();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextWindow)
	//}}AFX_VIRTUAL
protected:
	virtual void OnDraw(CDC* pDC);
	virtual CString GetTitleText(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSecID			m_ID;
	CString			m_sMessage;
	CRichEditCtrl	m_RichEditCtrl;
};
///////////////////////////////////////////////////////////////
inline const CString& CTextWindow::GetText() const
{
	return m_sMessage;
}

#endif // !defined(AFX_TEXTWINDOW_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_)
