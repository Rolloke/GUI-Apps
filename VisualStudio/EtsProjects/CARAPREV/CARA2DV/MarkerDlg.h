#pragma once


// CMarkerDlg dialog

class CMarkerDlg : public CDialog
{
	DECLARE_DYNAMIC(CMarkerDlg)

public:
	CMarkerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMarkerDlg();

// Dialog Data
	enum { IDD = IDD_MARKER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
public:
   double m_dMin;
   double m_dMax;
   double m_dMarker;
};
