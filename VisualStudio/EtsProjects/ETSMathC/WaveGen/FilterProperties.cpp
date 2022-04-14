// FilterProperties.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "WaveGen.h"
#include "FilterProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFilterProperties 
#define LOWEST_ORDER      1
#define HIGHEST_ORDER     4

#define LOWEST_QFACTOR     0.01
#define HIGHEST_QFACTOR  100.0

#define LOWEST_FREQUENCY  1e-13
#define HIGHEST_FREQUENCY 1e13


CFilterProperties::CFilterProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterProperties)
	m_nOrderTP           = LOWEST_ORDER;
	m_nOrderHP           = LOWEST_ORDER;
	m_nFilterCharacterTP = 0;
	m_nFilterCharacterHP = 0;
	m_dFrequencyTP       = 1000;
	m_nFilterType        = 0;
	m_dFrequencyHP       = 1000;
	m_dQFactorTP         = 1.0;
	m_dQFactorHP         = 1.0;
	m_dFilterM           = 100.0;
	m_dBandwidth         = 2;
	//}}AFX_DATA_INIT
   m_bBand = false;
}

void CFilterProperties::DoDataExchange(CDataExchange* pDX)
{
   bool bHP = false, bTP = false, bQTP = false, bQHP = false, bPN = false;
   int  nLoOrderTP = LOWEST_ORDER,
        nHiOrderTP = HIGHEST_ORDER,
        nLoOrderHP = LOWEST_ORDER,
        nHiOrderHP = HIGHEST_ORDER;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterProperties)
	DDX_Control(pDX, IDC_FILTER_ORDN_SPIN_TP, m_cOrderTPSpin);
	DDX_Control(pDX, IDC_FILTER_ORDN_SPIN_HP, m_cOrderHPSpin);
   DDX_CBIndex(pDX, IDC_FILTER_CHARACTER_TP, m_nFilterCharacterTP);
	DDX_CBIndex(pDX, IDC_FILTER_CHARACTER_HP, m_nFilterCharacterHP);
	DDX_CBIndex(pDX, IDC_FILTER_TYPE, m_nFilterType);
	//}}AFX_DATA_MAP

   switch(m_nFilterType)
   {
      case FILTER_TYPE_LOWPASS :  bTP       = true; break;
      case FILTER_TYPE_HIGHPASS:  bHP       = true; break;
      case FILTER_TYPE_BANDPASS:  bTP = bHP = true; break;
      case FILTER_TYPE_PINKNOISE:
         bPN = true; 
         nLoOrderTP = nHiOrderTP = nLoOrderHP = nHiOrderHP = 0;
         break;
      default: 
         pDX->Fail(); 
         break;
   }

   switch (m_nFilterCharacterTP)
   {
      case FILTER_CHA_BUTTERWORT_Q:
         bQTP = true;
         nLoOrderTP = 2;
         nHiOrderTP = 3;
         break;
      case FILTER_CHA_LINKWITZ_RILEY:
         nLoOrderTP = 4;
         nHiOrderTP = 4;
         break;
   }

   switch (m_nFilterCharacterHP)
   {
      case FILTER_CHA_BUTTERWORT_Q:
         bQHP = true;
         nLoOrderHP = 2;
         nHiOrderHP = 3;
         break;
      case FILTER_CHA_LINKWITZ_RILEY:
         nLoOrderHP = 4;
         nHiOrderHP = 4;
         break;
   }

   DDX_Text(pDX, IDC_FILTER_ORDN_EDIT_TP, m_nOrderTP);
   DDX_Text(pDX, IDC_FILTER_ORDN_EDIT_HP, m_nOrderHP);

   GetDlgItem(IDC_FILTER_FRQ_TP)->EnableWindow(bTP||bPN);
   GetDlgItem(IDC_FILTER_FRQ_HP)->EnableWindow(bHP);

   GetDlgItem(IDC_FILTER_ORDN_EDIT_TP)->EnableWindow(bTP);
   GetDlgItem(IDC_FILTER_ORDN_EDIT_HP)->EnableWindow(bHP);

   GetDlgItem(IDC_FILTER_ORDN_SPIN_TP)->EnableWindow(bTP);
   GetDlgItem(IDC_FILTER_ORDN_SPIN_HP)->EnableWindow(bHP);

   GetDlgItem(IDC_FILTER_CHARACTER_TP)->EnableWindow(bTP);
   GetDlgItem(IDC_FILTER_CHARACTER_HP)->EnableWindow(bHP);

   GetDlgItem(IDC_FILTER_QFACTOR_TP)->EnableWindow(bTP && bQTP);
   GetDlgItem(IDC_FILTER_QFACTOR_HP)->EnableWindow(bHP && bQHP);

   GetDlgItem(IDC_FILTER_FRQ_M     )->EnableWindow(bTP && bHP);
   GetDlgItem(IDC_FILTER_BAND_WIDTH)->EnableWindow(bTP && bHP);

   if (pDX->m_bSaveAndValidate)
   {
      if (bTP&&bHP)
      {
	      DDX_Text(pDX, IDC_FILTER_FRQ_M, m_dFilterM);
	      DDV_MinMaxDouble(pDX, m_dFilterM, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
	      DDX_Text(pDX, IDC_FILTER_BAND_WIDTH, m_dBandwidth);
	      DDV_MinMaxDouble(pDX, m_dBandwidth, 1.e-10, 1.e1);
      }

      if (m_bBand)
      {
         double dFactor = pow((double)10, log10(m_dBandwidth) * 0.5);
         m_dFrequencyTP = m_dFilterM * dFactor;
         m_dFrequencyHP = m_dFilterM / dFactor;
         pDX->m_bSaveAndValidate = false;
      }

      if (bTP||bPN)
      {
         DDX_Text(pDX, IDC_FILTER_FRQ_TP, m_dFrequencyTP);
         DDV_MinMaxDouble(pDX, m_dFrequencyTP, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
         if ((m_nOrderTP < nLoOrderTP) || (m_nOrderTP > nHiOrderTP))
         {
            m_nOrderTP = nLoOrderTP;
            pDX->m_bSaveAndValidate = false;
            DDX_Text(pDX, IDC_FILTER_ORDN_EDIT_TP, m_nOrderTP);
            pDX->m_bSaveAndValidate = true;
         }
      }
      if (bHP)
      {
	      DDX_Text(pDX, IDC_FILTER_FRQ_HP, m_dFrequencyHP);
         DDV_MinMaxDouble(pDX, m_dFrequencyHP, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
         if ((m_nOrderHP < nLoOrderHP) || (m_nOrderHP > nHiOrderHP))
         {
            m_nOrderHP = nLoOrderHP;
            pDX->m_bSaveAndValidate = false;
            DDX_Text(pDX, IDC_FILTER_ORDN_EDIT_HP, m_nOrderHP);
            pDX->m_bSaveAndValidate = true;
         }
      }
      if (m_bBand) pDX->m_bSaveAndValidate = true;
      else if (bTP&&bHP)
      {
         m_dBandwidth = m_dFrequencyTP / m_dFrequencyHP;
         double dFactor = pow((double)10, log10(m_dBandwidth) * 0.5);
         m_dFilterM   = m_dFrequencyHP * dFactor;
         pDX->m_bSaveAndValidate = false;
	      DDX_Text(pDX, IDC_FILTER_FRQ_M, m_dFilterM);
	      DDX_Text(pDX, IDC_FILTER_BAND_WIDTH, m_dBandwidth);
         pDX->m_bSaveAndValidate = true;
      }

      if (bQTP)
      {
         DDX_Text(pDX, IDC_FILTER_QFACTOR_TP, m_dQFactorTP);
	      DDV_MinMaxDouble(pDX, m_dQFactorTP, LOWEST_QFACTOR, HIGHEST_QFACTOR);
      }
      if (bQHP)
      {
         DDX_Text(pDX, IDC_FILTER_QFACTOR_HP, m_dQFactorHP);
	      DDV_MinMaxDouble(pDX, m_dQFactorHP, LOWEST_QFACTOR, HIGHEST_QFACTOR);
      }
   }
   else
   {
      DDX_Text(pDX, IDC_FILTER_FRQ_TP, m_dFrequencyTP);
      DDV_MinMaxDouble(pDX, m_dFrequencyTP, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
	   DDX_Text(pDX, IDC_FILTER_FRQ_HP, m_dFrequencyHP);
      DDV_MinMaxDouble(pDX, m_dFrequencyHP, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
      DDX_Text(pDX, IDC_FILTER_QFACTOR_TP, m_dQFactorTP);
	   DDV_MinMaxDouble(pDX, m_dQFactorTP, LOWEST_QFACTOR, HIGHEST_QFACTOR);
      DDX_Text(pDX, IDC_FILTER_QFACTOR_HP, m_dQFactorHP);
	   DDV_MinMaxDouble(pDX, m_dQFactorHP, LOWEST_QFACTOR, HIGHEST_QFACTOR);
	   DDX_Text(pDX, IDC_FILTER_FRQ_M, m_dFilterM);
	   DDV_MinMaxDouble(pDX, m_dFilterM, LOWEST_FREQUENCY, HIGHEST_FREQUENCY);
	   DDX_Text(pDX, IDC_FILTER_BAND_WIDTH, m_dBandwidth);
	   DDV_MinMaxDouble(pDX, m_dBandwidth, 1.e-10, 1.e1);
   }
   m_cOrderTPSpin.SetRange32(nLoOrderTP, nHiOrderTP);
   m_cOrderHPSpin.SetRange32(nLoOrderHP, nHiOrderHP);
}


BEGIN_MESSAGE_MAP(CFilterProperties, CDialog)
	//{{AFX_MSG_MAP(CFilterProperties)
	ON_CBN_SELCHANGE(IDC_FILTER_TYPE, OnSelchangeFilterType)
	ON_CBN_SELCHANGE(IDC_FILTER_CHARACTER_TP, OnSelchangeFilterCharacteristicTP)
	ON_CBN_SELCHANGE(IDC_FILTER_CHARACTER_HP, OnSelchangeFilterCharacteristicHP)
	ON_EN_KILLFOCUS(IDC_FILTER_BAND_WIDTH, OnKillfocusFilterBand)
	ON_EN_KILLFOCUS(IDC_FILTER_FRQ_M, OnKillfocusFilterBand)
	ON_EN_KILLFOCUS(IDC_FILTER_FRQ_HP, OnKillfocusFilterFrqHP)
	ON_EN_KILLFOCUS(IDC_FILTER_FRQ_TP, OnKillfocusFilterFrqTP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFilterProperties 

void CFilterProperties::OnOK() 
{
	CDialog::OnOK();

   switch(m_nFilterType)
   {
      case FILTER_TYPE_LOWPASS : m_nOrderHP = 0; break;
      case FILTER_TYPE_HIGHPASS: m_nOrderTP = 0; break;
      case FILTER_TYPE_PINKNOISE:
         m_nOrderTP = m_nOrderHP = 0;
         break;
   }
}

BOOL CFilterProperties::OnInitDialog() 
{
   if ((m_nOrderTP == 0) && (m_nOrderHP == 0))
   {
      m_nFilterType = FILTER_TYPE_PINKNOISE;
   }
   else if (m_nOrderHP == 0)
   {
      m_nFilterType  = FILTER_TYPE_LOWPASS;
   }
   else if (m_nOrderTP == 0)
   {
      m_nFilterType  = FILTER_TYPE_HIGHPASS;
   }
   else
   {
      m_nFilterType = FILTER_TYPE_BANDPASS;
   }

   CDialog::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CFilterProperties::OnSelchangeFilterType() 
{
   UpdateData(true);
}

void CFilterProperties::OnSelchangeFilterCharacteristicTP() 
{
   UpdateData(true);
}

void CFilterProperties::OnSelchangeFilterCharacteristicHP() 
{
   UpdateData(true);
}

void CFilterProperties::OnKillfocusFilterBand() 
{
   m_bBand = true;
   UpdateData(true);
   m_bBand = false;
}

void CFilterProperties::OnKillfocusFilterFrqHP() 
{
   UpdateData(true);
}

void CFilterProperties::OnKillfocusFilterFrqTP() 
{
   UpdateData(true);
}
