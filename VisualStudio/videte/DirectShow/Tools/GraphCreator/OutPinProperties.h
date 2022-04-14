#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CFilterCtrl;
// COutPinProperties dialog

class COutPinProperties : public CPropertyPage
{
	DECLARE_DYNAMIC(COutPinProperties)

public:
	COutPinProperties();
	virtual ~COutPinProperties();

   void SetName(CString&sName);
   void SetPin(IPin*pPin, int i, CFilterCtrl*pF);
// Dialog Data
	enum { IDD = IDD_OUT_PIN_PROPERTIES };
   CComboBox m_cCfgCaps;
   CComboBox m_cMediaTypes;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnDestroy();
   virtual BOOL OnApply();
	DECLARE_MESSAGE_MAP()
private:
   IPin *m_pPin;
   int   m_iPin;
   CFilterCtrl *m_pFilter;
public:
};
