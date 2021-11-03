// CreateTimeFunction.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "WaveGen.h"
#include "CreateTimeFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCreateTimeFunction 


CCreateTimeFunction::CCreateTimeFunction(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateTimeFunction::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateTimeFunction)
	m_dFrq1  = 1000.0;
	m_dFrq2  = 1000.0;
	m_dT1    = 50.0;
	m_dT2    = 50.0;
	m_dTG1   = 0.001;
	m_dTG2   = 0.001;
	m_bSweep = FALSE;
	m_nFkt   = 0;
	m_fSweepLength = 10.0f;
	//}}AFX_DATA_INIT
	m_dMinFrq =     0.1;
	m_dMaxFrq = 20480.0;
	m_dMinTG  = 1.0 / m_dMaxFrq;
	m_dMaxTG  = 1.0 / m_dMinFrq;
}


void CCreateTimeFunction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateTimeFunction)
	DDX_Text(pDX, IDC_CF_EDT_FRQ1, m_dFrq1);
	DDX_Text(pDX, IDC_CF_EDT_FRQ2, m_dFrq2);
	DDX_Text(pDX, IDC_CF_EDT_T1, m_dT1);
	DDX_Text(pDX, IDC_CF_EDT_T2, m_dT2);
	DDX_Text(pDX, IDC_CF_EDT_TG1, m_dTG1);
	DDX_Text(pDX, IDC_CF_EDT_TG2, m_dTG2);
	DDX_Check(pDX, IDC_CF_CK_SWEEP, m_bSweep);
	DDX_Radio(pDX, IDC_CF_R_SINE, m_nFkt);
	DDX_Text(pDX, IDC_CF_EDT_SWEEP_LENGTH, m_fSweepLength);
	DDV_MinMaxFloat(pDX, m_fSweepLength, 0.1f, 1000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateTimeFunction, CDialog)
	//{{AFX_MSG_MAP(CCreateTimeFunction)
	ON_BN_CLICKED(IDC_CF_CK_SWEEP, OnCfCkSweep)
	ON_EN_KILLFOCUS(IDC_CF_EDT_FRQ1, OnKillfocusCfEdtFrq1)
	ON_EN_KILLFOCUS(IDC_CF_EDT_FRQ2, OnKillfocusCfEdtFrq2)
	ON_EN_KILLFOCUS(IDC_CF_EDT_TG1, OnKillfocusCfEdtTg1)
	ON_EN_KILLFOCUS(IDC_CF_EDT_TG2, OnKillfocusCfEdtTg2)
	ON_EN_KILLFOCUS(IDC_CF_EDT_T1, OnKillfocusCfEdtT1)
	ON_EN_KILLFOCUS(IDC_CF_EDT_T2, OnKillfocusCfEdtT2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCreateTimeFunction 

BOOL CCreateTimeFunction::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CButton *pBtn = (CButton*)	GetDlgItem(IDC_CF_R_SINE);
   pBtn->SetIcon(::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CF_SINE)));
	pBtn = (CButton*)	GetDlgItem(IDC_CF_R_RECT);
   pBtn->SetIcon(::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CF_RECT)));
	pBtn = (CButton*)	GetDlgItem(IDC_CF_R_TRIANGLE);
   pBtn->SetIcon(::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CF_TRIANGLE)));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CCreateTimeFunction::OnCfCkSweep() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CF_CK_SWEEP, m_bSweep);

   GetDlgItem(IDC_CF_EDT_FRQ2)->EnableWindow(m_bSweep);
   GetDlgItem(IDC_CF_EDT_TG2)->EnableWindow(m_bSweep);
   GetDlgItem(IDC_CF_EDT_SWEEP_LENGTH)->EnableWindow(m_bSweep);
}

void CCreateTimeFunction::CalcInverseValue(int nID1, int nID2, bool bComplement, double dMin, double dMax, double &dVal1, double &dVal2)
{
	BOOL bOK = FALSE;       // assume failure
	TRY
	{
      CEdit *pEdt = (CEdit*)GetDlgItem(nID1);
      if (pEdt && pEdt->GetModify())
      {
         CDataExchange dx(this, true);
         DDX_Text(&dx, nID1, dVal1);
      	DDV_MinMaxDouble(&dx, dVal1, dMin, dMax);
         pEdt->SetModify(false);
         if (bComplement)
            dVal2 = 100.0-dVal1;
         else
            dVal2 = 1.0/dVal1;
         dx.m_bSaveAndValidate = false;
         DDX_Text(&dx, nID2, dVal2);
         pEdt = (CEdit*)GetDlgItem(nID2);
         if (pEdt) pEdt->SetModify(false);
      }
	}
	CATCH(CUserException, e)
	{
		// validation failed - user already alerted, fall through
		ASSERT(!bOK);
		// Note: DELETE_EXCEPTION_(e) not required
	}
	AND_CATCH_ALL(e)
	{
		// validation failed due to OOM or other resource failure
		e->ReportError(MB_ICONEXCLAMATION, AFX_IDP_INTERNAL_FAILURE);
		ASSERT(!bOK);
      e->Delete();
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL
}

void CCreateTimeFunction::OnKillfocusCfEdtFrq1()
{
   CalcInverseValue(IDC_CF_EDT_FRQ1, IDC_CF_EDT_TG1, false, m_dMinFrq, m_dMaxFrq, m_dFrq1, m_dTG1);
}

void CCreateTimeFunction::OnKillfocusCfEdtFrq2()
{
   CalcInverseValue(IDC_CF_EDT_FRQ2, IDC_CF_EDT_TG2, false, m_dMinFrq, m_dMaxFrq, m_dFrq2, m_dTG2);
}

void CCreateTimeFunction::OnKillfocusCfEdtTg1()
{
   CalcInverseValue(IDC_CF_EDT_TG1, IDC_CF_EDT_FRQ1, false, m_dMinTG, m_dMaxTG, m_dTG1, m_dFrq1);
}

void CCreateTimeFunction::OnKillfocusCfEdtTg2()
{
   CalcInverseValue(IDC_CF_EDT_TG2, IDC_CF_EDT_FRQ2, false, m_dMinTG, m_dMaxTG, m_dTG2, m_dFrq2);
}

void CCreateTimeFunction::OnKillfocusCfEdtT1()
{
   CalcInverseValue(IDC_CF_EDT_T1, IDC_CF_EDT_T2, true, 1.0, 99.0, m_dT1, m_dT2);
}

void CCreateTimeFunction::OnKillfocusCfEdtT2()
{
   CalcInverseValue(IDC_CF_EDT_T2, IDC_CF_EDT_T1, true, 1.0, 99.0, m_dT2, m_dT1);
}

void CCreateTimeFunction::OnOK()
{
	CDialog::OnOK();
}

