#if !defined(AFX_BITMAPTEXTBUTTON_H__C9ABE957_5EBA_11D2_B5C5_00C095EC9EFA__INCLUDED_)
#define AFX_BITMAPTEXTBUTTON_H__C9ABE957_5EBA_11D2_B5C5_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapTextButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapTextButton window

class CBitmapTextButton : public CButton
{
// Construction
public:
	CBitmapTextButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapTextButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapTextButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapTextButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CImageList m_Images;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPTEXTBUTTON_H__C9ABE957_5EBA_11D2_B5C5_00C095EC9EFA__INCLUDED_)
