// PinProperties.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "OutPinProperties.h"
#include "FilterButton.h"

#include <Dvdmedia.h>
#include "common\mfcutil.h"
#include ".\outpinproperties.h"

// COutPinProperties dialog

IMPLEMENT_DYNAMIC(COutPinProperties, CPropertyPage)
COutPinProperties::COutPinProperties()
	: CPropertyPage(COutPinProperties::IDD)
{
   m_pPin = NULL;
   m_pFilter = NULL;
}

COutPinProperties::~COutPinProperties()
{
   RELEASE_OBJECT(m_pPin);
}

void COutPinProperties::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_COMBO_CFG_CAPS, m_cCfgCaps);
   DDX_Control(pDX, IDC_COMBO_MEDIA_TYPES, m_cMediaTypes);
}


BEGIN_MESSAGE_MAP(COutPinProperties, CPropertyPage)
   ON_WM_HSCROLL()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

void COutPinProperties::SetName(CString&sName)
{
   m_strCaption = sName;
   m_psp.pszTitle = m_strCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

void COutPinProperties::SetPin(IPin*pPin, int i, CFilterCtrl*pF)
{
   m_pPin = pPin;
   if (m_pPin)
   {
      m_pPin->AddRef();
   }
   m_iPin = i;
   m_pFilter = pF;
}

// COutPinProperties message handlers

BOOL COutPinProperties::OnInitDialog()
{
   CPropertyPage::OnInitDialog();
   HRESULT hr;
   int nItem, nSel = 0;
   BOOL bSel = FALSE;
   IPin *pPin = NULL;
   AM_MEDIA_TYPE mtConnected = {0};
   IEnumMediaTypes *pEnum = NULL;
   CString s1, s2, s3, sep(", "), sFmt;
   ULONG uFetched;
   if (m_pPin)
   {
      hr = m_pPin->ConnectedTo(&pPin);
      if (SUCCEEDED(hr))
      {
         hr = m_pPin->ConnectionMediaType(&mtConnected);
         m_cMediaTypes.EnableWindow(FALSE);
      }
      hr = m_pPin->EnumMediaTypes(&pEnum);
      if (SUCCEEDED(hr))
      {
         m_cMediaTypes.AddString(_T("None"));
         AM_MEDIA_TYPE *pmt;
         while (SUCCEEDED(pEnum->Next(1, &pmt, &uFetched)) && uFetched)
         {
            bSel = FALSE;
            GetGUIDString(s1, &pmt->majortype, TRUE);
            GetGUIDString(s2, &pmt->subtype, TRUE);
            GetGUIDString(s3, &pmt->formattype, TRUE);
            if (pmt->formattype == FORMAT_WaveFormatEx)
            {
               WAVEFORMATEX *pWF = (WAVEFORMATEX*) pmt->pbFormat;
               WAVEFORMATEX *pWFsel = (WAVEFORMATEX*) mtConnected.pbFormat;
               sFmt.Format(_T("%d Hz, %d C, %d bps"), pWF->nSamplesPerSec, pWF->nChannels, pWF->nAvgBytesPerSec);
               s3 = _T("Wave");
               if (   pWFsel 
                   && pWF->nSamplesPerSec  == pWFsel->nSamplesPerSec
                   && pWF->nChannels       == pWFsel->nChannels
                   && pWF->nAvgBytesPerSec == pWFsel->nAvgBytesPerSec)
               {
                  bSel = TRUE;
               }
            }
            else if (   pmt->formattype == FORMAT_VideoInfo
                     || pmt->formattype == FORMAT_MPEGVideo
                     || pmt->formattype == FORMAT_MPEG2Video)
            {
               VIDEOINFOHEADER*pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
               int nFps = MulDiv(1, 10000000, pVIH->AvgTimePerFrame);
               sFmt.Format(_T("(%d x %d) %d fps"), pVIH->bmiHeader.biWidth, pVIH->bmiHeader.biHeight, nFps);
               s3 = _T("Video");
            }
            else if (pmt->formattype == FORMAT_VideoInfo2)
            {
               VIDEOINFOHEADER2*pVIH = (VIDEOINFOHEADER2*)pmt->pbFormat;
               int nFps = MulDiv(1, 10000000, pVIH->AvgTimePerFrame);
               sFmt.Format(_T("(%d x %d) %d fps"), pVIH->bmiHeader.biWidth, pVIH->bmiHeader.biHeight, nFps);
               s3 = _T("Video");
            }
            else if (pmt->formattype == FORMAT_DvInfo)
            {
               s3 = _T("DV Info");
            }

            nItem = m_cMediaTypes.AddString(s1+sep+s2+sep+s3+sep+sFmt);
            m_cMediaTypes.SetItemData(nItem, (DWORD_PTR)pmt);
            if (bSel)
            {
               m_pFilter->m_MediatypeOutPin[m_iPin] = nItem;
            }
         }
         m_cMediaTypes.SetCurSel(m_pFilter->m_MediatypeOutPin[m_iPin]);
      }
      FreeMediaType(mtConnected);
/*
      IAMStreamConfig *pCfg;
      hr = m_pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
      if (SUCCEEDED(hr))
      {
         pCfg->Release();
         PIN_INFO pi;
         hr = m_pPin->QueryPinInfo(&pi);
         if (SUCCEEDED(hr))
         {
            m_cCfgCaps.ShowWindow(SW_SHOW);
            AddAudioStreamConfigCapsToList(pi.pFilter, m_iPin, m_cCfgCaps);
            m_cCfgCaps.SetCurSel(0);
         }
      }
*/
   }
   RELEASE_OBJECT(pEnum);

   return TRUE;
}

void COutPinProperties::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

   CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COutPinProperties::OnDestroy()
{
   ClearAudioStreamConfigCapsList(m_cCfgCaps);
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

BOOL COutPinProperties::OnApply()
{
   m_pFilter->m_MediatypeOutPin[m_iPin] = m_cMediaTypes.GetCurSel();
   return CPropertyPage::OnApply();
}
