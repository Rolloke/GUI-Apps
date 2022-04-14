// AudioInputMixerProperties.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "AudioInputMixerProperties.h"
#include ".\audioinputmixerproperties.h"


// CAudioInputMixerProperties dialog

IMPLEMENT_DYNAMIC(CAudioInputMixerProperties, CPropertyPage)
CAudioInputMixerProperties::CAudioInputMixerProperties()
	: CPropertyPage(CAudioInputMixerProperties::IDD)
{
   m_pPin = NULL;
   m_pAudioInputMixer = NULL;
}

CAudioInputMixerProperties::~CAudioInputMixerProperties()
{
   RELEASE_OBJECT(m_pPin);
   RELEASE_OBJECT(m_pAudioInputMixer);
}

void CAudioInputMixerProperties::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SLIDER_LEVEL, m_cLevel);
   DDX_Control(pDX, IDC_SLIDER_BALANCE, m_cBalance);
   DDX_Control(pDX, IDC_SLIDER_BASS, m_cBass);
   DDX_Control(pDX, IDC_SLIDER_TREBLE, m_cTreble);
}


BEGIN_MESSAGE_MAP(CAudioInputMixerProperties, CPropertyPage)
   ON_WM_DESTROY()
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_CK_INPUT, OnBnClickedCkInput)
END_MESSAGE_MAP()

void CAudioInputMixerProperties::SetName(CString&sName)
{
   m_strCaption = sName;
   m_psp.pszTitle = m_strCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

void CAudioInputMixerProperties::SetPin(IPin*pPin, int i)
{
   m_pPin = pPin;
   if (m_pPin)
   {
      m_pPin->AddRef();
   }
   m_iPin = i;
}


// CAudioInputMixerProperties message handlers

BOOL CAudioInputMixerProperties::OnApply()
{
   // TODO: Add your specialized code here and/or call the base class

   return CPropertyPage::OnApply();
}

BOOL CAudioInputMixerProperties::OnInitDialog()
{
   CPropertyPage::OnInitDialog();
   HRESULT hr;
   if (m_pPin)
   {
      hr = m_pPin->QueryInterface(IID_IAMAudioInputMixer, (void**)&m_pAudioInputMixer);
      if (SUCCEEDED(hr))
      {
			double dLevel;
         BOOL bOn;
         m_cLevel.EnableWindow(TRUE);
         m_cLevel.SetRangeMin(0);
         m_cLevel.SetRangeMax(1000);
			hr = m_pAudioInputMixer->get_MixLevel(&dLevel);
			if (SUCCEEDED(hr))
			{
				m_cLevel.SetPos((int)(dLevel*1000));
         }
         hr = m_pAudioInputMixer->get_Enable(&bOn);
			if (SUCCEEDED(hr))
         {
            CheckDlgButton(IDC_CK_INPUT, bOn ? MF_CHECKED : MF_UNCHECKED);
            GetDlgItem(IDC_CK_INPUT)->EnableWindow(TRUE);
         }
         hr = m_pAudioInputMixer->get_Pan(&dLevel);
         if (SUCCEEDED(hr))
         {
            m_cBalance.SetPos((int)(dLevel*1000));
            m_cBalance.EnableWindow(TRUE);
         }
         hr = m_pAudioInputMixer->get_Bass(&dLevel);
         if (SUCCEEDED(hr))
         {
            m_cBass.SetPos((int)(dLevel*1000));
            m_cBass.EnableWindow(TRUE);
         }
         hr = m_pAudioInputMixer->get_Treble(&dLevel);
         if (SUCCEEDED(hr))
         {
            m_cTreble.SetPos((int)(dLevel*1000));
            m_cTreble.EnableWindow(TRUE);
         }

         hr = m_pAudioInputMixer->get_Loudness(&bOn);
         if (SUCCEEDED(hr))
         {
            CheckDlgButton(IDC_CK_LOUDNESS, bOn ? MF_CHECKED : MF_UNCHECKED);
            GetDlgItem(IDC_CK_LOUDNESS)->EnableWindow(TRUE);
         }
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioInputMixerProperties::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if (pScrollBar->m_hWnd == m_cLevel.m_hWnd && m_cLevel.IsWindowEnabled())
   {
      if (m_pAudioInputMixer)
      {
         double dLevel = (m_cLevel.GetPos()) / 1000.0;
		   m_pAudioInputMixer->put_MixLevel(dLevel);
      }
   }

   CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CAudioInputMixerProperties::OnBnClickedCkInput()
{
   if (m_pAudioInputMixer)
   {
      m_pAudioInputMixer->put_Enable(IsDlgButtonChecked(IDC_CK_INPUT));
   }
}

void CAudioInputMixerProperties::OnDestroy()
{
   CPropertyPage::OnDestroy();
}

