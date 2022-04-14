#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CInPinProperties dialog

class CInPinProperties : public CPropertyPage
{
	DECLARE_DYNAMIC(CInPinProperties)

public:
	CInPinProperties();
	virtual ~CInPinProperties();

   void SetName(CString&sName);
   void SetPin(IPin*pPin, int i);
// Dialog Data
	enum { IDD = IDD_IN_PIN_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual BOOL OnApply();

   afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
   IPin *m_pPin;
   int   m_iPin;
public:
   CComboBox m_cMediaTypes;
};
