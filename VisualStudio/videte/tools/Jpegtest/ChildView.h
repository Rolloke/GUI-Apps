// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__B338933B_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
#define AFX_CHILDVIEW_H__B338933B_4A42_11D3_8D9F_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CDecodeThread.h"

#define WPARAM_IMAGE_DECODED	0x0001
/////////////////////////////////////////////////////////////////////////////
// CChildView window
class CDecodeThread;

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	BOOL IsInterpolating(){return m_bInterpolate;}
	BOOL AllowDoubleFieldMode(){return m_bAllowDoubleFieldMode;}
	BOOL AllowNoiseReduction(){return m_bNoiseReduction;}
	BOOL AllowLumCorrection(){return m_bLumCorrection;}
	BOOL DecodeToScreen(){return m_bDecodeToScreen;}
	BOOL DecodeToHDD(){return m_bDecodeToHDD;}
	BOOL H263(){return m_bH263;}
	CString GetJPEGPath(){return m_sJPEGPath;};
	UINT GetFPS() {return m_iFPS;}

	int  GetScale(){return m_iScale;}
	int  GetWidth(){return m_iWidth;}
	int  GetHeight(){return m_iHeight;}

	// Operations
public:
	BOOL OpenDocumentFile(LPCTSTR lpszFileName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnPlay();
	afx_msg void OnOriginalSize();
	afx_msg void OnUpdateOriginalSize(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnInterpolate();
	afx_msg void OnUpdateInterpolate(CCmdUI* pCmdUI);
	afx_msg void OnAllow2field();
	afx_msg void OnUpdateAllow2field(CCmdUI* pCmdUI);
	afx_msg void OnNoiseReduction();
	afx_msg void OnUpdateNoiseReduction(CCmdUI* pCmdUI);
	afx_msg void OnLumcorrection();
	afx_msg void OnUpdateLumcorrection(CCmdUI* pCmdUI);
	afx_msg void OnStop();
	afx_msg void OnUpdatePlay(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnOptions();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CDecodeThread*	m_pDecodeThread;
	BOOL			m_bOriginalSize;
	BOOL			m_bInterpolate;
	BOOL			m_bAllowDoubleFieldMode;
	BOOL			m_bNoiseReduction;
	BOOL			m_bLumCorrection;
	CString			m_sJPEGPath;

	// Optionen
	BOOL			m_bDecodeToScreen;
	BOOL			m_bDecodeToHDD;
	BOOL			m_bH263;
	int				m_iScale;
	UINT			m_iHeight;
	UINT			m_iWidth;
	UINT			m_iFPS;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__B338933B_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
