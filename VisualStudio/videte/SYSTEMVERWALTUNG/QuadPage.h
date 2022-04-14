#pragma once


// CQuadPage dialog
#include "SVPage.h"
#include "afxwin.h"

class CQuadPage : public CSVPage
{
	DECLARE_DYNAMIC(CQuadPage)

public:
	CQuadPage(CSVView* pParent);   // standard constructor
	virtual ~CQuadPage();

// Dialog Data
	enum { IDD = IDD_QUAD };

public:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual BOOL GetToolTip(UINT nID, CString&sText);
			void Enable();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	void OnSelChangeCard(int iCard, BOOL bFromLoad);
	void InsertVideoOut(int iIndex, int iCount);

private:
	CComboBox m_ctrlType;
	CComboBox m_ctrlCOM;
	CComboBox m_ctrlVideo[4];
	CComboBox m_ctrlOut[4];
	CButton m_btnCOMSettings;
	COutputList* m_pOutputs;
	CComParameters* m_pComParameters;
public:
	afx_msg void OnCbnSelchangeComboTyp();
	afx_msg void OnCbnSelchangeComboCom();
	afx_msg void OnCbnSelchangeComboV1();
	afx_msg void OnCbnSelchangeComboV2();
	afx_msg void OnCbnSelchangeComboV3();
	afx_msg void OnCbnSelchangeComboV4();
	afx_msg void OnBnClickedButtonComSettings();
	afx_msg void OnCbnSelchangeComboOut();
};
