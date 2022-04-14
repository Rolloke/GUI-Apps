#if !defined(AFX_MDCONFIGDLG_H_INCLUDED_)
#define AFX_MDCONFIGDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MDConfigDlg.h : header file
//
#include <direct.h>

#define MAX_CAMERAS 16

class CDisplayWindow;
/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog

class CMDConfigDlg : public CDialog
{
// Construction
public:
	CMDConfigDlg(CSecID camID, CDisplayWindow* pParent);   // standard constructor
	virtual ~CMDConfigDlg();

	// operations
public:
	void ConfirmGetMask(CSecID camID, const CIPCActivityMask& mask);
	void ConfirmSetMask(CSecID camID, MaskType type);
	void OnIndicateError(DWORD dwCmd,CIPCError error, int iErrorCode);

// Dialog Data
	//{{AFX_DATA(CMDConfigDlg)
	enum { IDD = IDD_MD };
	CButton	m_ctrlShowAdaptive;
	CButton	m_ctrlOK;
	CButton	m_ctrlInvert;
	CButton	m_ctrlDel;
	CFrame	m_ctrlInlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMDConfigDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void ClearPermanentMask();
	afx_msg void InvertPermanentMask();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMdShowAdaptive();
	afx_msg void OnMdOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SetPixelInMask(int nXM, int nYM, BOOL bFlag);
	void LoadMask();
	BOOL SaveMask();
	void ShowFrame();

private:
	CImageList	m_ilDel;
	CImageList	m_ilInvert;
	CImageList	m_ilOK;
	CImageList	m_ilAdaptiveMask;

	CDisplayWindow* m_pDisplayWindow;
	CSecID		m_camID;

	int			m_nXMaskOffset;
	int			m_nYMaskOffset;
	CRect		m_rcMask;
	CRect		m_rcDlg;

	CCriticalSection	m_csMasks;
	CIPCActivityMask	m_PermMask;
	CIPCActivityMask	m_AdapMask;
	CIPCError m_CIPCError;
	
	CSize		m_ImageSize;
	CString		m_sMaskPath;
	CWindowDC*	m_pDCInlayWnd;
    BYTE*		m_pDibImageDataInlay;
	BITMAPINFO	m_bmiInlay;
	BOOL		m_bShowAdaptive;

	BOOL		m_bMaskLoaded;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCONFIGDLG_H__C7935943_E79D_4B89_B2C7_7D496599CCD3__INCLUDED_)
