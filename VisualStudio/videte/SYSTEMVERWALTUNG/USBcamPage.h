#if !defined(AFX_USB_CAM_PAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_USB_CAM_PAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SoftwarePage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CUSBcamPage dialog

class CUSBcamPage : public CSVPage
{
	DECLARE_DYNAMIC(CUSBcamPage)

// Construction
public:
	CUSBcamPage(CSVView*  pParent, int);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUSBcamPage)
	enum { IDD = IDD_USB_CAMERA };
	CListCtrl	m_cCameraList;
	BOOL	m_bCameraDetectors;
	BOOL	m_bMotionDetection;
	BOOL	m_bUnitActive;
	int		m_nFramesPerSecond;
	int		m_nFrameSize;
	//}}AFX_DATA
	int m_nPageNo;

public:
	void ReadRegistry();
	void Create();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();

	static BOOL    InitCameraList(CWK_Profile&, int, CListCtrl*pCameraList=NULL);
	static BOOL    IsUnitActive(CWK_Profile&, int);
	static BOOL    CanActivateUnit(CWK_Profile&, int);
	static LPCTSTR GetUnitName(int);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBcamPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUSBcamPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnClickListCameras(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL IsDataValid();

private:
	BOOL    m_bIsLocal;
	CString m_sBase;
	CString m_sDevice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USB_CAM_PAGE_H__56936202_0E5A_11D1_93D9_00C095ECA33E__INCLUDED_)
