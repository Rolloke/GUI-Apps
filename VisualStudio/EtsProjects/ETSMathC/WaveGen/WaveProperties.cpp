// WaveProperties.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "WaveGen.h"
#include "WaveProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWaveProperties 
#define WAVE_MAGN_PC_L    1
#define WAVE_MAGN_DB_L    2
#define WAVE_MAGN_PC_R    3
#define WAVE_MAGN_DB_R    4
#define WAVE_OTHERS       5

CWaveProperties::CWaveProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CWaveProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaveProperties)
	m_nBits             = 1;
	m_nFrequency        = 1;
	m_bStereo           = FALSE;
	m_dMaxMagnitudeDB_L =  1.0;
	m_dMaxMagnitudeDB_R =  1.0;
	m_dMaxMagnitude_L   = -1.0;
	m_dMaxMagnitude_R   = -1.0;
	m_nInput            = -1;
	m_bLInvert = FALSE;
	m_bRInvert = FALSE;
	m_bRMSPegel = FALSE;
	//}}AFX_DATA_INIT
   m_nSave = WAVE_OTHERS;
}


void CWaveProperties::DoDataExchange(CDataExchange* pDX)
{
   bool bLeft    = (m_dMaxMagnitude_L != -2) ? true : false;
   bool bRight   = (m_dMaxMagnitude_R != -2) ? true : false;
   bool bInputCh = false;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveProperties)
	DDX_Radio(pDX, IDC_WAVE_BITS0, m_nBits);
	DDX_Radio(pDX, IDC_WAVE_FREQU0, m_nFrequency);
   DDX_Check(pDX, IDC_WAVE_STEREO, m_bStereo);
	DDX_Check(pDX, IDC_WAVE_L_INV, m_bLInvert);
	DDX_Check(pDX, IDC_WAVE_R_INV, m_bRInvert);
	DDX_Check(pDX, IDC_WAVE_RMS_MAGNITUDE, m_bRMSPegel);
	//}}AFX_DATA_MAP

   GetDlgItem(IDC_WAVE_INPUT0)->EnableWindow(bLeft);
   GetDlgItem(IDC_WAVE_INPUT1)->EnableWindow(bRight);
   GetDlgItem(IDC_WAVE_INPUT2)->EnableWindow(m_bStereo && bLeft && bRight);
   GetDlgItem(IDC_WAVE_INPUT3)->EnableWindow(m_bStereo && bLeft && bRight);

   GetDlgItem(IDC_WAVE_L_INV )->EnableWindow(bLeft || bRight);
   GetDlgItem(IDC_WAVE_R_INV )->EnableWindow(m_bStereo && (bLeft || bRight));

   GetDlgItem(IDC_WAVE_MAX_MAGNITUDE_L)->EnableWindow(bLeft);
   GetDlgItem(IDC_WAVE_MAX_MAGNITUDE_DB_L)->EnableWindow(bLeft);
   GetDlgItem(IDC_WAVE_MAX_MAGNITUDE_R)->EnableWindow(bRight);
   GetDlgItem(IDC_WAVE_MAX_MAGNITUDE_DB_R)->EnableWindow(bRight);

   if ((!m_bStereo || !bLeft || !bRight) &&
       ((m_nInput == 3) || (m_nInput == 2))) m_nInput = 1, bInputCh = true;
   if (!bRight && (m_nInput == 1))           m_nInput--  , bInputCh = true;
   if (!bLeft  && (m_nInput == 0))           m_nInput--  , bInputCh = true;

   if (pDX->m_bSaveAndValidate)
   {
      switch(m_nSave)
      {
         case WAVE_MAGN_PC_L:
	         DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_L, m_dMaxMagnitude_L);
	         DDV_MinMaxDouble(pDX, m_dMaxMagnitude_L, 0., 100.);
            m_dMaxMagnitudeDB_L = 20 * log10(m_dMaxMagnitude_L*0.01);
            pDX->m_bSaveAndValidate = false;
         	DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_L, m_dMaxMagnitudeDB_L);
            pDX->m_bSaveAndValidate = true;
         break;
         case WAVE_MAGN_DB_L:
            DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_L, m_dMaxMagnitudeDB_L);
	         DDV_MinMaxDouble(pDX, m_dMaxMagnitudeDB_L, -120., 0.);
            m_dMaxMagnitude_L = 100.0 * pow((double)10, m_dMaxMagnitudeDB_L / 20.0);
            pDX->m_bSaveAndValidate = false;
         	DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_L, m_dMaxMagnitude_L);
            pDX->m_bSaveAndValidate = true;
         break;

         case WAVE_MAGN_PC_R:
	         DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_R, m_dMaxMagnitude_R);
	         DDV_MinMaxDouble(pDX, m_dMaxMagnitude_R, 0., 100.);
            m_dMaxMagnitudeDB_R = 20 * log10(m_dMaxMagnitude_R*0.01);
            pDX->m_bSaveAndValidate = false;
         	DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_R, m_dMaxMagnitudeDB_R);
            pDX->m_bSaveAndValidate = true;
         break;
         case WAVE_MAGN_DB_R:
            DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_R, m_dMaxMagnitudeDB_R);
	         DDV_MinMaxDouble(pDX, m_dMaxMagnitudeDB_R, -120., 0.);
            m_dMaxMagnitude_R = 100.0 * pow((double)10, m_dMaxMagnitudeDB_R / 20.0);
            pDX->m_bSaveAndValidate = false;
         	DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_R, m_dMaxMagnitude_R);
            pDX->m_bSaveAndValidate = true;
         break;
         case WAVE_OTHERS:
            if (bInputCh)
            {
               pDX->m_bSaveAndValidate = false;
               DDX_Radio(pDX, IDC_WAVE_INPUT0, m_nInput);
               pDX->m_bSaveAndValidate = true;
            }
            else DDX_Radio(pDX, IDC_WAVE_INPUT0, m_nInput);
         break;
      }
   }
   else
   {
      DDX_Radio(pDX, IDC_WAVE_INPUT0, m_nInput);

      DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_L, m_dMaxMagnitude_L);
	   DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_L, m_dMaxMagnitudeDB_L);

      DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_R, m_dMaxMagnitude_R);
	   DDX_Text(pDX, IDC_WAVE_MAX_MAGNITUDE_DB_R, m_dMaxMagnitudeDB_R);
   }
}


BEGIN_MESSAGE_MAP(CWaveProperties, CDialog)
	//{{AFX_MSG_MAP(CWaveProperties)
	ON_BN_CLICKED(IDC_WAVE_BITS0, OnChangeValues)
	ON_EN_KILLFOCUS(IDC_WAVE_MAX_MAGNITUDE_L, OnKillfocusWaveMaxMagnitudeL)
	ON_EN_KILLFOCUS(IDC_WAVE_MAX_MAGNITUDE_DB_L, OnKillfocusWaveMaxMagnitudeDbL)
	ON_EN_KILLFOCUS(IDC_WAVE_MAX_MAGNITUDE_R, OnKillfocusWaveMaxMagnitudeR)
	ON_EN_KILLFOCUS(IDC_WAVE_MAX_MAGNITUDE_DB_R, OnKillfocusWaveMaxMagnitudeDbR)
	ON_BN_CLICKED(IDC_WAVE_BITS1, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_FREQU0, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_FREQU1, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_FREQU2, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_STEREO, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_INPUT0, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_INPUT1, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_INPUT2, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_INPUT3, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_R_INV, OnChangeValues)
	ON_BN_CLICKED(IDC_WAVE_L_INV, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CWaveProperties 

BOOL CWaveProperties::OnInitDialog() 
{
	if      (m_dMaxMagnitude_L == -2);
	else if (m_dMaxMagnitude_L == -1)
   {
      m_dMaxMagnitude_L = pow((double)10, m_dMaxMagnitudeDB_L / 20.0);
   }
   else if (m_dMaxMagnitudeDB_L == 1)
   {
      m_dMaxMagnitudeDB_L = 20 * log10(m_dMaxMagnitude_L*0.01);
      if (m_dMaxMagnitudeDB_L > 0)
      {
         m_dMaxMagnitudeDB_L = -3;
         m_dMaxMagnitude_L = 100.0 * pow((double)10, m_dMaxMagnitudeDB_L / 20.0);
      }
   }

	if      (m_dMaxMagnitude_R == -2);
	else if (m_dMaxMagnitude_R == -1)
   {
      m_dMaxMagnitude_R = pow((double)10, m_dMaxMagnitudeDB_R / 20.0);
   }
   else if (m_dMaxMagnitudeDB_R == 1)
   {
      m_dMaxMagnitudeDB_R = 20 * log10(m_dMaxMagnitude_R*0.01);
      if (m_dMaxMagnitudeDB_R > 0)
      {
         m_dMaxMagnitudeDB_R = -3;
         m_dMaxMagnitude_R = 100.0 * pow((double)10, m_dMaxMagnitudeDB_R / 20.0);
      }
   }
   if (m_nInput == -1)
   {
      if (m_bStereo) m_nInput = 2;
      else           m_nInput = 0;
   }
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CWaveProperties::OnChangeValues() 
{
   UpdateData(true);
}

void CWaveProperties::OnOK() 
{
	
	CDialog::OnOK();
}

void CWaveProperties::OnKillfocusWaveMaxMagnitudeL() 
{
	if (m_dMaxMagnitude_L != -2)
   {
      m_nSave = WAVE_MAGN_PC_L;
      UpdateData(true);
      m_nSave = WAVE_OTHERS;
   }
}

void CWaveProperties::OnKillfocusWaveMaxMagnitudeDbL() 
{
	if (m_dMaxMagnitude_L != -2)
   {
      m_nSave = WAVE_MAGN_DB_L;
      UpdateData(true);
      m_nSave = WAVE_OTHERS;
   }
}

void CWaveProperties::OnKillfocusWaveMaxMagnitudeR() 
{
	if (m_dMaxMagnitude_R != -2)
   {
      m_nSave = WAVE_MAGN_PC_R;
      UpdateData(true);
      m_nSave = WAVE_OTHERS;
   }
}

void CWaveProperties::OnKillfocusWaveMaxMagnitudeDbR() 
{
	if (m_dMaxMagnitude_R != -2)
   {
      m_nSave = WAVE_MAGN_DB_R;
      UpdateData(true);
      m_nSave = WAVE_OTHERS;
   }
}
