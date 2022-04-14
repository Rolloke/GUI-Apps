#ifndef _SMALLTOOLBAR_H
#define _SMALLTOOLBAR_H

/////////////////////////////////////////////////////////////////////////////
class CSmallToolBar : public CToolBar 
{
public:
	CSmallToolBar();
	virtual ~CSmallToolBar();
protected:
	DECLARE_DYNAMIC(CSmallToolBar)
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CSmallToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
};
#endif
