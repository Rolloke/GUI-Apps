// PinProperties.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "InPinProperties.h"

#include "common\mfcutil.h"
#include "common\NAMEDGUID.h"
#include ".\inpinproperties.h"


// CInPinProperties dialog

IMPLEMENT_DYNAMIC(CInPinProperties, CPropertyPage)
CInPinProperties::CInPinProperties()
	: CPropertyPage(CInPinProperties::IDD)
{
   m_pPin = NULL;
}

CInPinProperties::~CInPinProperties()
{
   RELEASE_OBJECT(m_pPin);
}

void CInPinProperties::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_COMBO_MEDIA_TYPES, m_cMediaTypes);
}

BEGIN_MESSAGE_MAP(CInPinProperties, CPropertyPage)
   ON_WM_DESTROY()
END_MESSAGE_MAP()

void CInPinProperties::SetName(CString&sName)
{
   m_strCaption = sName;
   m_psp.pszTitle = m_strCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

void CInPinProperties::SetPin(IPin*pPin, int i)
{
   m_pPin = pPin;
   if (m_pPin)
   {
      m_pPin->AddRef();
   }
   m_iPin = i;
}

// CInPinProperties message handlers

BOOL CInPinProperties::OnInitDialog()
{
   CPropertyPage::OnInitDialog();
   HRESULT hr;
   int nItem;
   IEnumMediaTypes *pEnum = NULL;
   CString s1, s2, s3, sep(", ");
   ULONG uFetched;
   if (m_pPin)
   {
      hr = m_pPin->EnumMediaTypes(&pEnum);
      if (SUCCEEDED(hr))
      {
         AM_MEDIA_TYPE *pmt;
         while (SUCCEEDED(pEnum->Next(1, &pmt, &uFetched)) && uFetched)
         {
            GetGUIDString(s1, &pmt->majortype, TRUE);
            GetGUIDString(s2, &pmt->subtype, TRUE);
            GetGUIDString(s3, &pmt->formattype, TRUE);
            nItem = m_cMediaTypes.AddString(s1+sep+s2+sep+s3);
            m_cMediaTypes.SetItemData(nItem, (DWORD_PTR)pmt);
         }
         m_cMediaTypes.SetCurSel(0);
      }
   }
   RELEASE_OBJECT(pEnum);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}



void CInPinProperties::OnDestroy()
{
   int i, n;
   AM_MEDIA_TYPE *pmt;
   n=m_cMediaTypes.GetCount();
   for (i=0; i<n; i++)
   {
      pmt = (AM_MEDIA_TYPE*)m_cMediaTypes.GetItemData(i);
      DeleteMediaType(pmt);

   }
   CPropertyPage::OnDestroy();
}


BOOL CInPinProperties::OnApply()
{

   return CPropertyPage::OnApply();
}
