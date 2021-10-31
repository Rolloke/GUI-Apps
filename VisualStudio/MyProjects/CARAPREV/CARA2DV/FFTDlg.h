#pragma once

#include "CurveDialog.h"

// CFFTDlg dialog

class CFFTDlg : public CCurveDialog
{
	DECLARE_DYNAMIC(CFFTDlg)

public:
	CFFTDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFFTDlg();

// Dialog Data
	enum { IDD = IDD_FFT };

private:
	static UINT ThreadFunction(void*);

protected:
	//{{AFX_VIRTUAL(CFFTDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFFTDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   BOOL m_bTime;
};
