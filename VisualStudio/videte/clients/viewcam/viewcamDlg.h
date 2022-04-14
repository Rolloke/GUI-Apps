// viewcamDlg.h : header file
//
#include"IPCOutputViewcam.h"
#if !defined(AFX_VIEWCAMDLG_H__1FB7502C_F0AF_11D2_8948_004005A1D890__INCLUDED_)
#define AFX_VIEWCAMDLG_H__1FB7502C_F0AF_11D2_8948_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CIPCamera;
class CIPCOutputViewcam;
/////////////////////////////////////////////////////////////////////////////
// CViewcamDlg dialog

class CViewcamDlg : public CDialog
{
// Construction
public:
	CViewcamDlg(CWnd* pParent = NULL);	// standard constructor
	~CViewcamDlg();

// Dialog Data
	//{{AFX_DATA(CViewcamDlg)
	enum { IDD = IDD_VIEWCAM_DIALOG };

	CProgressCtrl	m_prPictures;
	CListBox	m_lbCamera;
	CString	m_eTarget_Host_Address;
	int	m_PictureCount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewcamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CViewcamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectbutton();
	afx_msg void OnDisconnect();
	afx_msg void OnSelchangeListCamera();
	afx_msg void OnButtonGetpicture();
	afx_msg void OnChangeEditAddress();
	afx_msg void OnSetfocusEditAddress();
	afx_msg void OnChangeEditPictureCount();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	CIPCOutputViewcam* m_pCIPCOutputViewcam;
	int iSelectCamera;//Anzahl der selektierten Kameras
	LPINT *ip_CameraIndex;
	
private:
	int i_CameraNr;
	int i_PictureCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWCAMDLG_H__1FB7502C_F0AF_11D2_8948_004005A1D890__INCLUDED_)
