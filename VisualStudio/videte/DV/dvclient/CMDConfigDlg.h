#if !defined(AFX_CMDCONFIGDLG_H__C7935943_E79D_4B89_B2C7_7D496599CCD3__INCLUDED_)
#define AFX_CMDCONFIGDLG_H__C7935943_E79D_4B89_B2C7_7D496599CCD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMDConfigDlg.h : header file
//
#include <direct.h>

#define MAX_CAMERAS 16

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog

class CMDConfigDlg : public CTransparentDialog
{
// Construction
public:
	CMDConfigDlg(CSecID camID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMDConfigDlg();

	// operations
public:
	void ConfirmGetMask(CSecID camID, const CIPCActivityMask& mask);
	void ConfirmSetMask(CSecID camID, MaskType type);

// Dialog Data
	//{{AFX_DATA(CMDConfigDlg)
	enum { IDD = IDD_MD };
	CSkinButton	m_ctrlShowAdaptiveMask;
	CSkinButton	m_ctrlOK;
	CSkinButton	m_ctrlInvert;
	CSkinButton	m_ctrlDel;
	CFrame		m_ctrlInlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDConfigDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SetPixelInMask(int nXM, int nYM, BOOL bFlag);
	BOOL LoadMask();
	BOOL SaveMask();
	void ShowFrame();

private:
	CImageList	m_ilDel;
	CImageList	m_ilInvert;
	CImageList	m_ilOK;
	CImageList	m_ilAdaptiveMask;

	HCURSOR			m_hCursor;
	CPanel*			m_pPanel;
	CJpeg*			m_pJpeg;
	CMPEG4Decoder*	m_pMpeg;
	CSystemTime		m_stPictureTime;
	CSecID			m_camID;

	int			m_nXMaskOffset;
	int			m_nYMaskOffset;
	CRect		m_rcMask;
	CRect		m_rcDlg;

	CCriticalSection	m_csMasks;
	CIPCActivityMask	m_PermMask;
	CIPCActivityMask	m_AdapMask;
	CEvent	m_eventSetPermMask;
	CEvent	m_eventGetPermMask;
	CEvent	m_eventSetAdapMask;
	CEvent	m_eventGetAdapMask;
	
	CSize		m_JpegSize;
	CString		m_sMaskPath;
	CWindowDC*	m_pDCInlayWnd;
    BYTE*		m_pDibImageDataInlay;
	BITMAPINFO	m_bmiInlay;
	BOOL		m_bShowAdaptive;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCONFIGDLG_H__C7935943_E79D_4B89_B2C7_7D496599CCD3__INCLUDED_)
