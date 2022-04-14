#if !defined(AFX_DECODEWINDOW_H__682F6D81_2474_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_DECODEWINDOW_H__682F6D81_2474_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DecodeWindow.h : header file
//

class CPTDecoder;
/////////////////////////////////////////////////////////////////////////////
// CDecodeWindow window

class CDecodeWindow : public CFrameWnd
{
// Construction
public:
	CDecodeWindow(CPTDecoder* pDecoder);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecodeWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDecodeWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDecodeWindow)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPTDecoder* m_pDecoder;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECODEWINDOW_H__682F6D81_2474_11D1_BC01_00A024D29804__INCLUDED_)
