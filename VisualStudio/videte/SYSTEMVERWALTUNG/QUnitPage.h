#pragma once


#include "SVPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;
// CQUnitPage dialog

class CQUnitPage : public CSVPage
{
	DECLARE_DYNAMIC(CQUnitPage)

public:
	CQUnitPage(CSVView* pParent);   // standard constructor
	virtual ~CQUnitPage();
	void SaveNewDongleFeaturesAfterExpansion();

// Dialog Data
	enum { IDD = IDD_QUNIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckQ();
	DECLARE_MESSAGE_MAP()
	

	// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	void EnableDlgControlsQ();
	void LoadQSettings();
	void SaveQSettings();
	void GetDlgQData();

private:
	CInputList*		m_pInputList;
	COutputList*	m_pOutputList;
	BOOL			m_bQ;
	int				m_iVideoNorm;
	int				m_iCurrentVideoNorm;
	int				m_iVideoFormat;
	int				m_iVideoAGC;
	BOOL			m_bRestartQUnit;
	CString			m_sAppName;
public:
	afx_msg void OnBnClickedRadioPal();
	afx_msg void OnBnClickedRadioNtsc();
	afx_msg void OnBnClickedRadioJpeg();
	afx_msg void OnBnClickedRadioMpeg4();
	afx_msg void OnBnClickedRadioAgcOn();
	afx_msg void OnBnClickedRadioAgcOff();
};
