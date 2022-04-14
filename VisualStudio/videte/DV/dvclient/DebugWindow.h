#pragma once

#include "DisplayWindow.h"

// CDebugWindow

class CDebugWindow : public CDisplayWindow
{
	DECLARE_DYNAMIC(CDebugWindow)

public:
	CDebugWindow(CMainFrame* pParent, CServer* pServer, BOOL bUseAsReport = FALSE);
	virtual ~CDebugWindow();


// Implementation
public:
	void	SetTextLine();
	CString GetTextLine();

protected:
	virtual void OnDraw(CDC* pDC);
	virtual CSecID		GetID() const;
	virtual CString		GetName() const;
	virtual BOOL		IsShowable();
	virtual int			GetType() const { return  DISPLAYWINDOW_DEBUG; };
	virtual CString		GetDefaultText();

protected:
	//{{AFX_MSG(CDebugWindow)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CEdit m_ctrlEdit;
	CSkinButton m_BtnCancel;
	CSkinButton m_BtnCopy;
	CSkinButton m_BtnClear;
	BOOL m_bUseAsReport;

};


