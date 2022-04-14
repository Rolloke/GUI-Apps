// CurrencyUnit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "CurrencyUnit.h"
#include "FacturaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurrencyUnit 


CCurrencyUnit::CCurrencyUnit(CWnd* pParent /*=NULL*/)
	: CDialog(CCurrencyUnit::IDD, pParent)
{
   m_nIndex = CB_ERR;
   InitData();
}

void CCurrencyUnit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurrencyUnit)
	DDX_Control(pDX, IDC_CU_UNIT, m_cUnit);
	DDX_CBString(pDX, IDC_CU_UNIT, m_strUnit);
	DDV_MaxChars(pDX, m_strUnit, 47);
	DDX_Text(pDX, IDC_CU_UNITSIGN, m_strUnitSign);
	DDV_MaxChars(pDX, m_strUnitSign, 7);
	DDX_Text(pDX, IDC_CU_STATESIGN, m_strStateSign);
	DDV_MaxChars(pDX, m_strStateSign, 3);
	DDX_Text(pDX, IDC_CU_CURRENCY, m_fCurrency);
	DDV_MinMaxFloat(pDX, m_fCurrency, 1.e-013f, 1.e+013f);
	DDX_Text(pDX, IDC_CU_STATE, m_strState);
	DDX_Text(pDX, IDC_CU_REFERENCE, m_strReference);
	//}}AFX_DATA_MAP
   if (pDX->m_bSaveAndValidate && m_strUnit.IsEmpty())
      pDX->Fail();

   GetDlgItem(IDC_CU_DELETE)->EnableWindow(m_nIndex!=CB_ERR);
}


BEGIN_MESSAGE_MAP(CCurrencyUnit, CDialog)
	//{{AFX_MSG_MAP(CCurrencyUnit)
	ON_EN_CHANGE(IDC_CU_CURRENCY, OnChangeValues)
	ON_CBN_SELCHANGE(IDC_CU_UNIT, OnSelchangeCuUnit)
	ON_BN_CLICKED(IDC_CU_DELETE, OnCuDelete)
	ON_BN_CLICKED(IDC_CU_NEW, OnCuNew)
	ON_BN_CLICKED(IDC_CU_SAVE, OnCuSave)
	ON_EN_KILLFOCUS(IDC_CU_STATESIGN, OnKillfocusCuStatesign)
	ON_EN_CHANGE(IDC_CU_STATESIGN, OnChangeValues)
	ON_CBN_EDITCHANGE(IDC_CU_UNIT, OnChangeValues)
	ON_EN_CHANGE(IDC_CU_UNITSIGN, OnChangeValues)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCurrencyUnit 

BOOL CCurrencyUnit::OnInitDialog() 
{
	if (CDialog::OnInitDialog())
   {
      CWinApp *pApp = AfxGetApp();
      if (!pApp)             return false;
      if (!pApp->m_pMainWnd) return false;
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (!pDoc)             return false;
      m_pCurrencyUnit = &pDoc->m_CurrencyUnits;
      m_pStates       = &pDoc->m_States;
      m_nIndex        = m_pCurrencyUnit->GetCount()-1;

      m_strReference  = _T("--");
      POSITION pos = m_pCurrencyUnit->GetHeadPosition();
      while (pos)
      {
         CurrencyUnit *pCU = (CurrencyUnit*) m_pCurrencyUnit->GetNext(pos);
         if (pCU)
         {
            if (pCU->fCurrency == 1.0f) m_strReference.Format("1 %s =",pCU->szUnitSign);
            m_cUnit.AddString(pCU->szUnit);
         }
      }
      InitData();

	   return TRUE;
   }
   return false;
}

void CCurrencyUnit::OnChangeValues() 
{
   GetDlgItem(IDC_CU_SAVE)->EnableWindow(true);
}

void CCurrencyUnit::OnSelchangeCuUnit() 
{
   int nIndex = m_cUnit.GetCurSel();
   if (m_nIndex != nIndex)
   {
      SaveChangedData();
      m_nIndex = nIndex;
      InitData();
   }
}

void CCurrencyUnit::OnCuDelete() 
{
   if (m_nIndex != CB_ERR)
   {
      POSITION pos = m_pCurrencyUnit->FindIndex(m_nIndex);
      if (pos)
      {
         CurrencyUnit *pCU = (CurrencyUnit*) m_pCurrencyUnit->GetAt(pos);
         if (pCU)
         {
            delete pCU;
            m_pCurrencyUnit->RemoveAt(pos);
            m_cUnit.DeleteString(m_nIndex);
            m_nIndex = m_cUnit.SetCurSel(0);
            InitData();
         }
      }
   }
}

void CCurrencyUnit::OnCuNew() 
{
   SaveChangedData();
   if (m_nIndex != CB_ERR)
   {
      m_nIndex = CB_ERR;
      InitData();
   }
}

void CCurrencyUnit::OnCuSave() 
{
   SaveChangedData(false);
}

void CCurrencyUnit::OnCancel() 
{
   if (SaveChangedData(false))
	   CDialog::OnCancel();
}

void CCurrencyUnit::OnOK() 
{
   if (SaveChangedData(false))
   	CDialog::OnOK();
}

void CCurrencyUnit::InitData()
{
   if (m_nIndex != CB_ERR)
   {
      POSITION pos = m_pCurrencyUnit->FindIndex(m_nIndex);
      if (pos)
      {
         CurrencyUnit *pCU = (CurrencyUnit*) m_pCurrencyUnit->GetAt(pos);
         if (pCU)
         {
	         m_strUnit      = _T(pCU->szUnit);
	         m_strUnitSign  = _T(pCU->szUnitSign);
	         m_strStateSign = _T(pCU->szStateSign);
	         m_fCurrency    = pCU->fCurrency;
            SetStateString();
         }
      }
   }
   else
   {
	   //{{AFX_DATA_INIT(CCurrencyUnit)
      m_strUnit      = _T("");
      m_strUnitSign  = _T("");
      m_strStateSign = _T("");
      m_fCurrency    = 1.0f;
      m_strState     = _T("");
   	//}}AFX_DATA_INIT
   }

   if (IsWindow(m_hWnd))
   {
      GetDlgItem(IDC_CU_SAVE)->EnableWindow(false);
      UpdateData(false);
   }
}

bool CCurrencyUnit::SaveChangedData(bool bAsk)
{
   if (GetDlgItem(IDC_CU_SAVE)->IsWindowEnabled())
   {  
      if (!UpdateData(true)) return false;
      int nReturn = IDYES;
      if (bAsk) nReturn = AfxMessageBox(IDS_SAVE_CURRENCY_UNIT, MB_YESNO);
      if (nReturn == IDYES)
      {
         if (m_nIndex == CB_ERR)
         {
            m_nIndex = m_cUnit.AddString(m_strUnit);
            if (m_nIndex != CB_ERR) m_nIndex = m_cUnit.SetCurSel(m_nIndex);
            if (m_nIndex != CB_ERR)
            {
               CurrencyUnit *pCU = new CurrencyUnit;
               if (pCU)
               {
                  ZeroMemory(pCU, sizeof(CurrencyUnit));
                  m_pCurrencyUnit->AddTail(pCU);
                  ASSERT(m_nIndex == (m_pCurrencyUnit->GetCount()-1));
               }
            }
         }
         POSITION pos = m_pCurrencyUnit->FindIndex(m_nIndex);
         if (pos)
         {
            CurrencyUnit *pCU = (CurrencyUnit*) m_pCurrencyUnit->GetAt(pos);
            if (pCU)
            {
               pCU->fCurrency = m_fCurrency;
               strncpy(pCU->szStateSign, LPCTSTR(m_strStateSign),  3);
               pCU->szStateSign[3] = 0;
               strncpy(pCU->szUnit     , LPCTSTR(m_strUnit)     , 47);
               pCU->szUnit[47]     = 0;
               strncpy(pCU->szUnitSign , LPCTSTR(m_strUnitSign) ,  7);
               pCU->szUnitSign[7]  = 0;
            }
         }
      }
      GetDlgItem(IDC_CU_SAVE)->EnableWindow(false);
   }
   return true;
}

void CCurrencyUnit::OnKillfocusCuStatesign() 
{
   if (UpdateData(true))
   {
      if (SetStateString()) UpdateData(false);
   }
}

bool CCurrencyUnit::SetStateString()
{
   m_strState.Empty();
   POSITION pos = m_pStates->GetHeadPosition();
   while (pos)
   {
      State *pState = (State *)m_pStates->GetNext(pos);
      if (pState && (strcmp(pState->szStateSign, m_strStateSign)==0))
      {
         m_strState = _T(pState->szState);
         return true;
      }
   }
   return false;
}

LRESULT CCurrencyUnit::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      ((CFacturaApp*)AfxGetApp())->WinHelp((ULONG)pHI, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }
   return true;
}
