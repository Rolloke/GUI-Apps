// WndText.h: interface for the CWndText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WndText_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_)
#define AFX_WndText_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSmall.h"

class CWndText : public CWndSmall
{
	DECLARE_DYNAMIC(CWndText)	
	// construction/destruction
public:
	CWndText(CServer* pServer, const CSecID id, const CString& sMessage);
	virtual ~CWndText();

public:
	inline const CString& GetText() const;
	BOOL IsDebuggerWindow();

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd);
	void SetNotificationText(const CString& sMessage);
private:
	void CheckMessageFont(void);
	int  GetNoOfLines();
	CString  GetLine(int n);


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndText)
	// Overrides
public:
	virtual UINT	GetToolBarID();
	virtual	CSecID	GetID();
	virtual	BOOL CanPrint();
protected:
	virtual void OnDraw(CDC* pDC);
	virtual CString GetTitleText(CDC* pDC);
	virtual	void	PopupMenu(CPoint pt);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndText)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateSmallContext(CCmdUI* pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSecID			m_ID;
	CString			m_sMessage;
	CRichEditCtrl	m_RichEditCtrl;
	CFont			m_Font;
	int				m_nItemsPerPage;
	int				m_nItemHeight;
public:
};
///////////////////////////////////////////////////////////////
inline const CString& CWndText::GetText() const
{
	return m_sMessage;
}

#endif // !defined(AFX_WndText_H__DE352DE1_32C9_11D3_9962_004005A19028__INCLUDED_)
