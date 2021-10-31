// CreateCurve.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CreateCurve.h"
#include "..\TreeDlg.h"
#include "..\LabelContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"
#include "EtsAppWndMsg.h"
#include ".\createcurve.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCreateCurve 

extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);

#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) \
      ListView_SetItemState(hwndLV, i, \
      INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif

#define RESULT_TYPE_REAL          0
#define RESULT_TYPE_AMPL_PHASE    1
#define RESULT_TYPE_AMPL_PHASE_DB 2
#define RESULT_TYPE_LOCUS         3
#define RESULT_TYPE_REAL_IMAG     4

#define TEXT_SIZE 64

#define FILE_TYPE_EQUATION "equ"

class CCallDelete
{
public:
   CCallDelete(void* pParam, void (*pFunc)(void*))
   {
      ASSERT(pParam != NULL);
      ASSERT(pFunc  != NULL);
      m_pParam = pParam;
      m_pFunc  = pFunc;
   };
   ~CCallDelete()
   {
      m_pFunc(m_pParam);
   };

private:
   void *m_pParam;
   void (*m_pFunc)(void*);
};

CCreateCurve::CCreateCurve(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CCreateCurve::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCreateCurve)
   m_nNoOfValues   = 100;
   m_bLogarithmic  = FALSE;
   m_nResultType   = 0;
	m_strResultUnit = _T("Y");
	m_strRunVarUnit = _T("X");
	//}}AFX_DATA_INIT
   m_dFrom        = 0.0;
	m_dTo          = 1.0;
	m_strRunVar    = _T("");
	m_strError     = _T("");
   m_nRunVar      = 0;
   m_bCurves      = false;
   m_bNoSetCurve  = true;
   m_p2ndCurve    = NULL;
}

CCreateCurve::~CCreateCurve()
{
   m_pCurve = NULL;
}

void CCreateCurve::DoDataExchange(CDataExchange* pDX)
{
	CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateCurve)
	DDX_Control(pDX, IDC_FC_FUNCTION, m_Function);
	DDX_Control(pDX, IDC_FC_VARLIST, m_cVarList);
	DDX_Control(pDX, IDC_FC_CURVELIST, m_cCurveList);
	DDX_Text(pDX, IDC_FC_FUNCTION, m_strFunction);
	DDX_Text(pDX, IDC_FC_NO_OF_VAL, m_nNoOfValues);
   DDV_MinMaxInt(pDX, m_nNoOfValues, 2, 10000000);
	DDX_Check(pDX, IDC_FC_LOGARITHMIC, m_bLogarithmic);
	DDX_CBIndex(pDX, IDC_FC_COMBO_RESULT_FKT, m_nResultType);
	DDX_Text(pDX, IDC_FC_RESULT_UNIT, m_strResultUnit);
	DDX_Text(pDX, IDC_FC_RUNVAR_UNIT, m_strRunVarUnit);
	//}}AFX_DATA_MAP

	DDX_TextVar(pDX, IDC_FC_TO  , DBL_DIG, m_dTo);
   DDX_TextVar(pDX, IDC_FC_FROM, DBL_DIG, m_dFrom);

   if (pDX->m_bSaveAndValidate)
   {
      if (m_dFrom > m_dTo)
      {
         swap(m_dFrom, m_dTo);
         pDX->m_bSaveAndValidate = false;
         DDX_Text(pDX, IDC_FC_FROM, m_dFrom);
	      DDX_Text(pDX, IDC_FC_TO  , m_dTo);
         pDX->m_bSaveAndValidate = true;
      }
      if (m_bLogarithmic)
      {
	      DDV_MinMaxDouble(pDX, m_dFrom, 1e-20, 1e20);
      }
   }
   else if (!m_strRunVarText.IsEmpty() && !m_strRunVar.IsEmpty())
   {
      int nPos = m_strRunVar.Find('.', 0);
      CString strRunVar;
      CString str = m_strRunVar;
      if ((m_pCurve != NULL) && (m_pCurve->GetLocus() != 0) && (nPos!=-1))
      {
         str = m_strRunVar.Left(nPos);
      }
      else str = m_strRunVar;
      strRunVar.Format(m_strRunVarText, str);
   	DDX_Text(pDX, IDC_FC_RUNVARSTR, strRunVar);
      DDX_Text(pDX, IDC_FC_RUNVAR_UNIT_TXT, str);
   	DDX_Text(pDX, IDC_FC_ERROR    , m_strError);
   }
}


BEGIN_MESSAGE_MAP(CCreateCurve, CCurveDialog)
	//{{AFX_MSG_MAP(CCreateCurve)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FC_VARLIST, OnEndlabeleditFcVarlist)
	ON_EN_CHANGE(IDC_FC_FUNCTION, OnChangeFcFunction)
	ON_NOTIFY(NM_DBLCLK, IDC_FC_VARLIST, OnDblclkFcVarlist)
	ON_EN_KILLFOCUS(IDC_FC_FROM, OnUpdateCtrl)
	ON_BN_CLICKED(IDC_FC_LOGARITHMIC, OnBtnClickedLog)
	ON_EN_CHANGE(IDC_FC_NO_OF_VAL, OnChangeParameter)
	ON_NOTIFY(NM_RCLICK, IDC_FC_VARLIST, OnRclickFcVarlist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_FC_VARLIST, OnBeginlabeleditFcVarlist)
	ON_BN_CLICKED(IDC_FC_TEST, OnFcTest)
	ON_NOTIFY(NM_DBLCLK, IDC_FC_CURVELIST, OnDblclkFcCurvelist)
	ON_NOTIFY(NM_SETFOCUS, IDC_FC_CURVELIST, OnSetfocusFcCurvelist)
	ON_NOTIFY(NM_SETFOCUS, IDC_FC_VARLIST, OnSetfocusFcCurvelist)
	ON_NOTIFY(NM_RDBLCLK, IDC_FC_VARLIST, OnRclickFcVarlist)
	ON_NOTIFY(NM_RCLICK, IDC_FC_CURVELIST, OnRclickFcVarlist)
	ON_NOTIFY(NM_RDBLCLK, IDC_FC_CURVELIST, OnRclickFcVarlist)
   ON_EN_KILLFOCUS(IDC_FC_TO, OnUpdateCtrl)
	ON_EN_CHANGE(IDC_FC_TO, OnChangeParameter)
	ON_EN_CHANGE(IDC_FC_FROM, OnChangeParameter)
   ON_MESSAGE(WM_UPDATE_ERROR, OnUpdateError)
	ON_CBN_SELCHANGE(IDC_FC_COMBO_RESULT_FKT, OnChangeParameter)
	ON_EN_CHANGE(IDC_FC_RESULT_UNIT, OnChangeParameter)
	ON_EN_CHANGE(IDC_FC_RUNVAR_UNIT, OnChangeParameter)
	//}}AFX_MSG_MAP
   ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCreateCurve 
LRESULT CCreateCurve::OnUpdateError(WPARAM, LPARAM)
{
/*
   if (m_strError.GetLength() > 39)
   {
      HWND hwndFocus = ::GetFocus();
      AfxMessageBox(m_strError, MB_ICONEXCLAMATION);
      ::SetFocus(hwndFocus);
   }
   else
   {
*/
      CDataExchange dx(this, false);
      DDX_Text(&dx, IDC_FC_ERROR, m_strError);
//   }
      return 0;
}
void CCreateCurve::OnOK() 
{
   if (UpdateData(true))                           // Rechenparameter ermitteln
   {
      m_pThread = AfxBeginThread(ThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
      if (m_pThread)
      {
         m_pThread->m_bAutoDelete = TRUE;
         m_pThread->ResumeThread();
      }
      GetDlgItem(IDC_CALCULATE)->EnableWindow(false);
   }
}

void CCreateCurve::OnCancel() 
{
   m_pCurve = NULL;	
	CCurveDialog::OnCancel();
}

BOOL CCreateCurve::OnInitDialog()
{
	CCurveDialog::OnInitDialog();
   CString str;

   GetDlgItem(IDC_NEW_NAME_STR)->ShowWindow(SW_SHOW);

   m_cVarList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT|LVS_EX_CHECKBOXES,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT|LVS_EX_CHECKBOXES);

   str.LoadString(IDS_CR_COLUMN_VALUE);
   m_cVarList.InsertColumn(0, str, LVCFMT_LEFT, 80, 0);
   str.LoadString(IDS_CR_COLUMN_VAR);
   m_cVarList.InsertColumn(1, str, LVCFMT_LEFT, 59, 0);

   m_cCurveList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);

   str.LoadString(IDS_FIT_COLUMN_VAR);
   m_cCurveList.InsertColumn(0, str, LVCFMT_LEFT, 40, 0);
   str.LoadString(IDS_CR_COLUMN_CURVE);
   m_cCurveList.InsertColumn(1, str, LVCFMT_LEFT, 99, 0);

	m_strFunction  = _T("");
   m_strNewName.LoadString(IDS_CREATE_CURVE);
   
   CWnd *pWnd = GetDlgItem(IDC_FC_RUNVARSTR);
   if (pWnd)
   {
      pWnd->GetWindowText(m_strRunVarText);
      pWnd->SetWindowText("");
   }
   SetCurvesToList();
   m_cVarList.ShowWindow(SW_HIDE);
   m_cCurveList.ShowWindow(SW_SHOW);

   UpdateData(false);
   GetDlgItem(IDC_CALCULATE)->EnableWindow(false);
   GetDlgItem(IDC_FC_TEST)->EnableWindow(FALSE);
/*
   //  Fonts von Controls setzen
   NONCLIENTMETRICS ncm;
   ncm.cbSize = sizeof(NONCLIENTMETRICS);
   SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
   HFONT hFont = CreateFontIndirect(&ncm.lfMessageFont);
   if (hFont) GetDlgItem(IDC_FC_FUNCTION)->SetFont(CFont::FromHandle(hFont));
   // Achtung das FontObject muß wieder gelöscht werden !!
*/
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CCreateCurve::OnEndlabeleditFcVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (m_cVarList.UpdateData(true))
   {
      Complex *pd  = NULL;
      CString str = m_cVarList.GetItemText(pDispInfo->item.iItem, 1);
      if (!str.IsEmpty()) pd = m_Equation.GetVarAdress((char*)LPCTSTR(str));
      if (pd) *pd = m_cVarList.m_cValue;
      *pResult = 1;
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }
   else
	   *pResult = 0;
}

void CCreateCurve::DeleteParam(void *pParam)
{
   if (pParam)
   {
      CCreateCurve *pCrCu = (CCreateCurve*) pParam;            // Klassenzeiger steht in pParam
      sRunCurve    *psRC;
      while (!pCrCu->m_RunList.IsEmpty())                      // Liste der Kurvenvariable löschen
      {
         psRC = (sRunCurve*) pCrCu->m_RunList.RemoveHead();
         if (psRC)
         {
            if (psRC->pl) delete psRC->pl;                     // Struktur
            delete psRC;                                       // Kurve
         }
      }
      if (pCrCu->m_p2ndCurve)
      {
         delete pCrCu->m_p2ndCurve;
         pCrCu->m_p2ndCurve = NULL;
      }
   }
}

UINT CCreateCurve::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CCreateCurve *pCrCu = (CCreateCurve*) pParam;            // Klassenzeiger steht in pParam
      CCallDelete Calldelete(pParam, CCreateCurve::DeleteParam);
      sRunCurve *psRC;
      int        i, nCount, nError, nPStep, nPCount = 0, nPProg = 0, nDiv;
      char       text[TEXT_SIZE];
      Complex    dValue, *pdRun, dRun;
      double     dStep, dOrg;
      POSITION   pos;
      LVITEM     lvI;
      UINT       nCurveOk;
      CString    strRes, strSPC(", ");

      CFloatPrecision _fp;
      pCrCu->m_Function.EnableWindow(false);
      pCrCu->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);
      pCrCu->GetDlgItem(IDC_NEW_NAME_STR)->ShowWindow(SW_HIDE);
      try
      {                                                        // Adresse der Laufvariablen ermitteln
         pdRun = pCrCu->m_Equation.GetVarAdress((char*)LPCTSTR(pCrCu->m_strRunVar));
         if (pdRun)
         {
            pdRun->realteil = 0;
            pdRun->imagteil = 0;
         }
         else
         {
            pCrCu->ReportError(IDS_EQUATION_CONST, true);
            pdRun = &dRun;
         }

         lvI.mask       = LVIF_TEXT|LVIF_PARAM;                // Listenparameter ermitteln
         lvI.pszText    = text;
         lvI.cchTextMax = TEXT_SIZE;
         lvI.iSubItem   = 1;
         nCount = pCrCu->m_cVarList.GetItemCount();            // Variablenliste durchsuchen
         for (lvI.iItem =0; lvI.iItem<nCount; lvI.iItem++)
         {
            pCrCu->m_cVarList.GetItem(&lvI);
            if (lvI.lParam)                                    // In lParam steht der Zeiger auf die Kurve
            {
               pos  = pCrCu->m_RunList.GetHeadPosition();
               psRC = NULL;
               while (pos)
               {
                  psRC = (sRunCurve*)pCrCu->m_RunList.GetNext(pos);     // Kurve in der Liste suchen
                  if ((LPARAM)psRC->pl == lvI.lParam) break;
                  else                                psRC = NULL;
               }
               if (psRC==NULL)                                 // Kurve nicht gefunden : neu allozieren
               {
                  CCurveLabel *pcl = (CCurveLabel*) lvI.lParam;// Kurve noch in der Liste ?
                  if (pCrCu->m_pCurves->GetIndex(pcl) == -1)   throw (int)10;// Nein: Fehler
                  psRC     = new sRunCurve;                    // Parameterstruktur allozieren
                  if (!psRC)                                   throw (int)11;
                  pCrCu->m_RunList.AddHead(psRC);              // In die Liste einfügen
                  ZeroMemory(psRC, sizeof(sRunCurve));
                  psRC->pl = new CCurveLabel(pcl);             // neue Kurve allozieren -> Threadsicherheit !
                  if (!psRC->pl)                               throw (int)12;
                  bool bEquallySpaced = pcl->AreXvaluesEqualySpaced();
                  if (bEquallySpaced) psRC->pl->ChangeFormat(CF_CHANGE_TO_SINGLE);
                  psRC->pl->CopyValues(pcl);                   // Kurvenwerte Kopieren
               }
                                                               // Zeiger auf die Kurvenvariablen speichern
               if (strstr(lvI.pszText, ".x"))                  // x Variable
               {
                  psRC->pdRunX = pCrCu->m_Equation.GetVarAdress(lvI.pszText);
               }
               else if (strstr(lvI.pszText, ".y"))             // y Variable
               {
                  psRC->pdRunY = pCrCu->m_Equation.GetVarAdress(lvI.pszText);
                  if (pdRun == psRC->pdRunY) pdRun = &dRun;
               }
               else                                            // Ortskurvenvariable
               {
                  psRC->pdRunC = pCrCu->m_Equation.GetVarAdress(lvI.pszText);
                  if (pdRun == psRC->pdRunC) pdRun = &dRun;
               }
            }
         }
                                                               // zu erzeugende Kurve allozieren
         if (pCrCu->m_pCurve) pCrCu->m_pNewCurve = new CCurveLabel(pCrCu->m_pCurve);
         else                 pCrCu->m_pNewCurve = new CCurveLabel;
         if (!pCrCu->m_pNewCurve)                              throw (int)13;

         pCrCu->m_pNewCurve->SetTargetFuncNo(0);
         pCrCu->m_pNewCurve->SetColor(0xffffffff);             // Kurven-Parameter setzen
         pCrCu->m_pNewCurve->SetWriteProtect(false);

         if (pCrCu->m_pCurve)                                  // Initialisierung durch Kurve
         {
            pCrCu->OptimizeFormat(pCrCu->m_pCurve, pCrCu->m_pNewCurve);
            if (pCrCu->m_pCurve->IsWriteProtect())             // Schreibgeschützte Kurven werden 
               pCrCu->m_pNewCurve->SetAsCopy(true);            // als kopiert gekennzeichnet !
            if (pCrCu->m_bLogarithmic)
            {
               if (pCrCu->m_pNewCurve->GetXDivision() <= ETSDIV_LINEAR)
                  pCrCu->m_pNewCurve->SetXDivision(ETSDIV_LOGARITHMIC);
            }
            else
            {
               if (pCrCu->m_pNewCurve->GetXDivision() > ETSDIV_LINEAR)
                  pCrCu->m_pNewCurve->SetXDivision(ETSDIV_LINEAR);
            }
            if (pCrCu->m_nResultType != RESULT_TYPE_LOCUS)
            {
               pCrCu->m_pNewCurve->SetSorted(true);
               pCrCu->m_pNewCurve->SetSortable(true);
               pCrCu->m_pNewCurve->SetLocus(0);
            }
         }
         else
         {
            if (pCrCu->m_nResultType == RESULT_TYPE_LOCUS)
            {
               pCrCu->m_pNewCurve->SetFormat(CF_DOUBLE);       // Ortskurve : keine äquidistanten X-Werte
               pCrCu->m_pNewCurve->SetLocus((pCrCu->m_bLogarithmic!=0) ? CCURVE_LOGARITHMIC:CCURVE_LINEAR);
            }
            else
            {
               pCrCu->m_pNewCurve->SetFormat(CF_SINGLE_DOUBLE);// sonst ist Speicherformat Single double
               if (pCrCu->m_bLogarithmic) pCrCu->m_pNewCurve->SetXDivision(ETSDIV_LOGARITHMIC);
               pCrCu->m_pNewCurve->SetSorted(true);
               pCrCu->m_pNewCurve->SetSortable(true);
            }
         }

         if (pCrCu->m_bLogarithmic)                            // logarithmisch
         {
            dOrg  = pCrCu->m_dFrom;
            pCrCu->m_pNewCurve->SetXOrigin(dOrg);
            dStep = pow(pCrCu->m_dTo/pCrCu->m_dFrom, 1.0/(double)(pCrCu->m_nNoOfValues-1));
            pCrCu->m_pNewCurve->SetXStep(dStep);
         }
         else                                                  // linear
         {
            dOrg  = pCrCu->m_dFrom;
            pCrCu->m_pNewCurve->SetXOrigin(pCrCu->m_dFrom);
            dStep = (pCrCu->m_dTo - pCrCu->m_dFrom) / (pCrCu->m_nNoOfValues-1);
            pCrCu->m_pNewCurve->SetXStep(dStep);
         }

         nDiv = pCrCu->m_pNewCurve->GetXDivision();

         if (!pCrCu->m_pNewCurve->SetSize(pCrCu->m_nNoOfValues)) throw (int)14;

         if ((pCrCu->m_nResultType == RESULT_TYPE_AMPL_PHASE)||(pCrCu->m_nResultType == RESULT_TYPE_AMPL_PHASE_DB)||(pCrCu->m_nResultType == RESULT_TYPE_REAL_IMAG))
         {                                                     // zweite Kurve erzeugen
            if (pCrCu->m_p2ndCurve == NULL) pCrCu->m_p2ndCurve = new CCurveLabel(pCrCu->m_pNewCurve);
            if (!pCrCu->m_p2ndCurve)                               throw (int)13;
            if (!pCrCu->m_p2ndCurve->SetSize(pCrCu->m_nNoOfValues))throw (int)14;
            pCrCu->m_p2ndCurve->SetTargetFuncNo(0);
            pCrCu->m_p2ndCurve->SetXStep(dStep);
            pCrCu->m_p2ndCurve->SetXOrigin(dOrg);
         }

         switch (pCrCu->m_nResultType)
         {
            case RESULT_TYPE_REAL:
               pCrCu->m_pNewCurve->SetText((char*)LPCTSTR(pCrCu->m_strNewName));
               pCrCu->m_pNewCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_pNewCurve->SetYUnit(pCrCu->m_strResultUnit);// Y Einheit gilt für die Ergebnisvariable
               break;
            case RESULT_TYPE_AMPL_PHASE_DB:
            {
               CString strYUnit = _T("dB");
               strYUnit += pCrCu->m_strResultUnit;
               pCrCu->m_pNewCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_pNewCurve->SetYUnit("deg");            // Y Einheit gilt für die Ergebnisvariable
               strRes.LoadString(IDS_PHASE);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_pNewCurve->SetText((char*)LPCTSTR(strRes));

               pCrCu->m_p2ndCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_p2ndCurve->SetYUnit(strYUnit);         // Y Einheit gilt für die Ergebnisvariable
               strRes.LoadString(IDS_AMPLITUDE);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_p2ndCurve->SetText((char*)LPCTSTR(strRes));
            } break;
            case RESULT_TYPE_AMPL_PHASE:
               pCrCu->m_pNewCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_pNewCurve->SetYUnit("deg");            // Y Einheit gilt für die Ergebnisvariable
               strRes.LoadString(IDS_PHASE);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_pNewCurve->SetText((char*)LPCTSTR(strRes));

               pCrCu->m_p2ndCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_p2ndCurve->SetYUnit(pCrCu->m_strResultUnit);// Y Einheit gilt für die Ergebnisvariable
               strRes.LoadString(IDS_AMPLITUDE);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_p2ndCurve->SetText((char*)LPCTSTR(strRes));
               break;
            case RESULT_TYPE_LOCUS:
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_pNewCurve->SetText((char*)LPCTSTR(strRes));
               pCrCu->m_pNewCurve->SetXDivision(ETSDIV_LINEAR);
               pCrCu->m_pNewCurve->SetYDivision(ETSDIV_LINEAR);
               pCrCu->m_pNewCurve->SetLocusUnit(pCrCu->m_strRunVarUnit);// Ortskurveneinheit gilt für die Laufvariable
               pCrCu->m_pNewCurve->SetXUnit(pCrCu->m_strResultUnit);// Einheit für X und Y Achse gleich der
               pCrCu->m_pNewCurve->SetYUnit(pCrCu->m_strResultUnit);// für die Ergebnisvariable
               break;
            case RESULT_TYPE_REAL_IMAG: 
               strRes.LoadString(IDS_REAL_PART);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_p2ndCurve->SetText((char*)LPCTSTR(strRes));
               pCrCu->m_p2ndCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_p2ndCurve->SetYUnit(pCrCu->m_strResultUnit);// Y Einheit gilt für die Ergebnisvariable
               strRes.LoadString(IDS_IMAG_PART);
               strRes = pCrCu->m_strNewName + strSPC + strRes;
               pCrCu->m_pNewCurve->SetText((char*)LPCTSTR(strRes));
               pCrCu->m_pNewCurve->SetXUnit(pCrCu->m_strRunVarUnit);// X Einheit gilt für die Laufvariable
               pCrCu->m_pNewCurve->SetYUnit(pCrCu->m_strResultUnit);// Y Einheit gilt für die Ergebnisvariable
               break;
         }

         CProgressCtrl *pProg = (CProgressCtrl*) pCrCu->GetDlgItem(IDC_PROGRESS);
         pProg->SetRange(0, CURVE_PROGRESS_RANGE);             // Fortschrittanzeige
         pProg->ShowWindow(SW_SHOW);
         nPStep = (pCrCu->m_nNoOfValues-1) >> 7;               // Fortschrittsanzeige num / 128
         if (!nPStep) nPStep++;

         for (i=0; i<pCrCu->m_nNoOfValues; i++)
         {
            if (++nPCount>=nPStep)                             // Fortschrittanzeige updaten
            {
               pProg->SetPos(++nPProg);
               nPCount=0;
            }
            if (pdRun)                                         // Laufvariable vorhanden
            {
               switch(nDiv)
               {
                  case ETSDIV_LINEAR:
                     pdRun->realteil = dOrg + i * dStep;
                     break;
                  case ETSDIV_LOGARITHMIC: case ETSDIV_FREQUENCY:
                     pdRun->realteil = dOrg * pow(dStep, i);
                     break;
               }
               pdRun->imagteil = 0;
            }
            pos = pCrCu->m_RunList.GetHeadPosition();          // Kurvenvariablen updaten
            while (pos)
            {
               psRC = (sRunCurve*)pCrCu->m_RunList.GetNext(pos);
               if ((psRC->pdRunX != NULL) && (psRC->pdRunX != pdRun))
               {
                  psRC->pdRunX->realteil = psRC->pl->Get_X_Value(pdRun->realteil);
               }
               if  (psRC->pdRunY)
               {
                  psRC->pdRunY->realteil = psRC->pl->Get_Y_Value(pdRun->realteil);
               }
               if ((psRC->pdRunC != NULL) && (psRC->pdRunC != pdRun))
               {
                  SCurve sc = psRC->pl->Get_Locus_Value(pdRun->realteil);
                  *psRC->pdRunC = *((Complex*)&sc);
               }
            }

            dValue = pCrCu->m_Equation.Calculate();            // Gleichung berechnen
            nError = pCrCu->m_Equation.GetError();
            if (nError != 0)                                   // bei Fehler 
            {
               pCrCu->ReportError(nError, true);               // Fehlermeldung ausgeben
               REPORT("%s\n", pCrCu->m_strError);
               dValue = Complex(0,0);                          // Kurvenwert auf 0 setzen
            }

            switch (pCrCu->m_nResultType)
            {
               case RESULT_TYPE_REAL:
                  pCrCu->m_pNewCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_pNewCurve->SetY_Value(i, dValue.realteil);
                  break;
               case RESULT_TYPE_AMPL_PHASE:
                  pCrCu->m_pNewCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_pNewCurve->SetY_Value(i, Phase(dValue));
                  pCrCu->m_p2ndCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_p2ndCurve->SetY_Value(i, Betrag(dValue));
                  break;
               case RESULT_TYPE_AMPL_PHASE_DB:
                  pCrCu->m_pNewCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_pNewCurve->SetY_Value(i, Phase(dValue));
                  pCrCu->m_p2ndCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_p2ndCurve->SetY_Value(i, 20*log10(Betrag(dValue)));
                  break;
               case RESULT_TYPE_LOCUS:
                  pCrCu->m_pNewCurve->SetX_Value(i, dValue.realteil);
                  pCrCu->m_pNewCurve->SetY_Value(i, dValue.imagteil);
                  break;
               case RESULT_TYPE_REAL_IMAG:
                  pCrCu->m_pNewCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_pNewCurve->SetY_Value(i, dValue.imagteil);
                  pCrCu->m_p2ndCurve->SetX_Value(i, pdRun->realteil);
                  pCrCu->m_p2ndCurve->SetY_Value(i, dValue.realteil);
                  break;
            }
         }
         nCurveOk = NEWCURVE_IS_OK;                            // Parameter den Nachricht 
         uResult  = 0;                                         // Rückgabewert der Funktion
      }
      catch(int nError)                                        // Fehlerbehandlung
      {
         REPORT("%s(%d) :Error %d, %x:", __FILE__, __LINE__, nError, GetLastError());
         pCrCu->ReportError(nError, true);
         uResult  = 1;
         nCurveOk = NEWCURVE_IS_NOTOK;
      }

      pCrCu->GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);    // Fortschrittanzeige verstecken
      pCrCu->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_SHOW);    // Textfeld für Kurvennamen
      pCrCu->GetDlgItem(IDC_NEW_NAME_STR)->ShowWindow(SW_SHOW);// Textfeld für Kurvennamen
      pCrCu->m_Function.EnableWindow(true);
      pCrCu->PostMessage(WM_ENDTHREAD, 0, nCurveOk);           // 

      if (pCrCu->m_p2ndCurve)
      {
         if (nCurveOk == NEWCURVE_IS_OK)
         {
            CWnd *pWnd = pCrCu->GetParent();
            if (pWnd) pWnd->PostMessage(WM_NEWCURVE, (LPARAM) pCrCu->m_p2ndCurve, 1);
         }
         pCrCu->m_p2ndCurve = NULL;
      }
      pCrCu->m_pThread = NULL;
   }
   return uResult;
}

void CCreateCurve::OnFcTest() 
{
   if (UpdateData(TRUE))
   {
      m_Equation.Delete();
      if (!m_strFunction.IsEmpty())
      {
         DWORD nTest = m_Equation.SetEquation(m_strFunction);
         if (nTest == 0)
         {
            SetVariablesToList();
            GetDlgItem(IDC_FC_TEST)->EnableWindow(FALSE);
            GetDlgItem(IDC_CALCULATE)->EnableWindow(TRUE);
            m_strError.LoadString(IDS_EQUATION_OK);
         }
         else
         {
            OnChangeFcFunction();
            ReportError(m_Equation.GetError(), true);
            m_Equation.Delete();
            m_Function.SetSel(nTest);
            m_Function.SetFocus();
            GetDlgItem(IDC_CALCULATE)->EnableWindow(FALSE);
         }
      }
      else
      {
         m_strError.LoadString(IDS_ERROR_NOEQUATION);
      }
      UpdateData(FALSE);
   }
   else GetDlgItem(IDC_CALCULATE)->EnableWindow(FALSE);
}

void CCreateCurve::OnChangeFcFunction() 
{
   CWnd *pFcTest = GetDlgItem(IDC_FC_TEST);
   if (pFcTest && !pFcTest->IsWindowEnabled())
   {
      pFcTest->EnableWindow(TRUE);
      GetDlgItem(IDC_CALCULATE)->EnableWindow(FALSE);
      if (m_bCurves)
      {
         LRESULT lResult = 1;
         NMHDR   nh;
         OnSetfocusFcCurvelist(&nh, &lResult);
      }
      else
      {
         m_cVarList.DeleteAllItems();
      }
      if (!m_strRunVar.IsEmpty())
      {
         m_strRunVar.Empty();
         GetDlgItem(IDC_FC_RUNVARSTR)->SetWindowText("");
      }
      m_cVarList.ShowWindow(SW_HIDE);
      m_cCurveList.ShowWindow(SW_SHOW);
   }
}

void CCreateCurve::OnChangeParameter() 
{
   if (m_Equation.GetError() == 0)
   {
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }
}

void CCreateCurve::ReportError(int nError, bool bUpdate)
{
   switch (nError)
   {
      case 1 : nError = IDS_ERROR_BRACE;           break;
      case 2 : nError = IDS_ERROR_OUT_OF_RANGE;    break;
      case 3 : nError = IDS_ERROR_OPERATOR;        break;
      case 4 : nError = IDS_ERROR_NOEQUATION;      break;
      case 5 : nError = IDS_ERROR_DIVIDE_BY_ZERO;  break;
      case 6 : nError = IDS_ERROR_NOFUNCTION;      break;
      case 7 : nError = IDS_ERROR_NUMBER;          break;
      case 9 : nError = IDS_ERROR_NO_COMPLEX_IMPL; break;
      case 10: nError = IDS_ERROR_CURVE_NOT_IN_LST;break;
      case 20: nError = IDS_ERROR_DECIMAL_POINT;   break;
      case 11: case 12: case 13: case 14:
               nError = IDE_OUT_OF_MEMORY;         break;
   }
   m_strError.LoadString(nError);
   if (bUpdate)
   {
      PostMessage(WM_UPDATE_ERROR);
   }
}

int CCreateCurve::SetFcToTree(CTreeDlg *pDlg, int nReason)
{
   if (nReason != ONINITDIALOG) return 0;
   CArithmetic *pAr = (CArithmetic *) pDlg->m_pParam;
   pAr->Show(&pDlg->m_TreeCtrl);
   return 1;
}

void CCreateCurve::OnUpdateCtrl() 
{
   UpdateData(true);	
}

void CCreateCurve::OnBtnClickedLog()
{
   if (UpdateData(true))
   {
      OnChangeParameter();
   }
   else
   {
      m_bLogarithmic = false;
      UpdateData(false);
   }
}

void CCreateCurve::OnDblclkFcVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_Equation.GetError() == 0)
   {
      int nItem = ListView_GetSelectionMark(pNMHDR->hwndFrom);
      if ((nItem >= 0) && (ListView_GetCheckState(pNMHDR->hwndFrom, nItem) == 0))
         ListView_EditLabel(pNMHDR->hwndFrom, nItem);		
   }
	*pResult = 1;
}

void CCreateCurve::OnDblclkFcCurvelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   int nItem = ListView_GetSelectionMark(pNMHDR->hwndFrom);
   if (nItem >= 0)
   {
      if (UpdateData(true))
      {
         CString str = m_cCurveList.GetItemText(nItem, 0);
         GetDlgItem(IDC_FC_FUNCTION)->SendMessage(EM_REPLACESEL, 1, (LPARAM)LPCTSTR(str));
         OnChangeFcFunction();
      }
   }
	
	*pResult = 0;
}


void CCreateCurve::OnRclickFcVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint ptScreen;
   CMenu Menu, *pContext;
   Menu.LoadMenu(IDR_CONTEXT_MENUES);
   pContext = Menu.GetSubMenu(0);

   GetCursorPos(&ptScreen);

   pContext = Menu.GetSubMenu(0);
   if (pContext)
   {
      if (m_cCurveList.IsWindowVisible()) // Kurvenliste
      {
         pContext->EnableMenuItem(ID_SHOW_EQUATION_STRUCTURE, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
         pContext->EnableMenuItem(ID_SAVE_EQUATION, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
      }
      BOOL bReturn = pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this);
      switch (bReturn)
      {
         case ID_SHOW_EQUATION_STRUCTURE:
         {
            CTreeDlg Tdlg;
            Tdlg.m_strWndTitle.LoadString(IDS_FUNCTIONTREE);
            Tdlg.m_pParam    = (void*) &m_Equation;
            Tdlg.m_pCallBack = SetFcToTree;
            Tdlg.DoModal();
         } break;
         case ID_SAVE_EQUATION:
         case ID_LOAD_EQUATION:
         {
            CString        title;
            CString        curvefilter, otherFilter;
            CCARA2DVApp   *pApp  = (CCARA2DVApp*) AfxGetApp();
            int nCustMaxFilter = 1;

            otherFilter.LoadString(IDS_EQU_FILTER);
            curvefilter += otherFilter;
            curvefilter += _T("|*.");
            curvefilter += _T(FILE_TYPE_EQUATION);
            curvefilter += (TCHAR)'|';
            nCustMaxFilter++;
            CFileDialog fd((bReturn==ID_SAVE_EQUATION) ? FALSE:TRUE, FILE_TYPE_EQUATION, m_strNewName, OFN_OVERWRITEPROMPT|OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_EXPLORER, LPCTSTR(curvefilter), AfxGetApp()->m_pMainWnd);
            fd.m_ofn.nMaxCustFilter = nCustMaxFilter;

            title.LoadString((bReturn==ID_SAVE_EQUATION) ? AFX_IDS_SAVEFILE : AFX_IDS_OPENFILE);
            fd.m_ofn.lpstrTitle     = LPCTSTR(title);
            fd.m_ofn.nFilterIndex   = 1;
            fd.m_ofn.nFileExtension = 1;

            if (!pApp->m_strDefaultCurveDir.IsEmpty())
            {
               fd.m_ofn.lpstrInitialDir = pApp->m_strDefaultCurveDir;
            }

            if (fd.DoModal()==IDOK)
            {
               pApp->m_strDefaultCurveDir = fd.GetPathName();
               int nPos = pApp->m_strDefaultCurveDir.ReverseFind('\\');
               if (nPos != -1) pApp->m_strDefaultCurveDir = pApp->m_strDefaultCurveDir.Left(nPos);
               if (bReturn==ID_SAVE_EQUATION)
               {
                  CFile    file(fd.GetPathName(), CFile::modeCreate|CFile::modeWrite);
                  CArchive ar(&file, CArchive::store);
                  CString  str;
                  Complex  *pC;
                  CFileHeader fh(FILE_TYPE_EQUATION, "2001", 100);
                  int i, n = m_cVarList.GetItemCount();
                  ar.Write(&fh, sizeof(CFileHeader));
                  ar << m_strFunction;
                  for (i=0; i<n; i++)
                  {
                     str = m_cVarList.GetItemText(i, 1);
                     pC = m_Equation.GetVarAdress(str);
                     if (pC) ar.Write(pC, sizeof(Complex));
                  }
               }
               else
               {
                  CFile    file(fd.GetPathName(), CFile::modeRead);
                  CArchive ar(&file, CArchive::load);
                  CString  str;
                  Complex  *pC;
                  CFileHeader fh;
                  ar.Read(&fh, sizeof(CFileHeader));
                  if (fh.IsType(FILE_TYPE_EQUATION) && (fh.GetVersion() == 100))
                  {
                     ar >> m_strFunction;
                     m_Equation.Delete();
                     DWORD nTest = m_Equation.SetEquation(m_strFunction);
                     if (nTest == 0)
                     {
                        char szList[4];
                        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
                        SetVariablesToList();
                        GetDlgItem(IDC_FC_TEST)->EnableWindow(FALSE);
                        GetDlgItem(IDC_CALCULATE)->EnableWindow(TRUE);
                        m_strError.LoadString(IDS_EQUATION_OK);
                        int i, n = m_cVarList.GetItemCount();
                        for (i=0; i<n; i++)
                        {
                           str = m_cVarList.GetItemText(i, 1);
                           pC = m_Equation.GetVarAdress(str);
                           if (pC)
                           {
                              ar.Read(pC, sizeof(Complex));
                              if (pC->imagteil)
                                 str.Format("(%e%c %e)", pC->realteil, szList[0], pC->imagteil);
                              else
                                 str.Format("%e", pC->realteil);
                              m_cVarList.SetItemText(i, 0, str);
                           }
                        }
                     }
                  }
               }
            }
         } break;
      }
   }
	*pResult = 0;

}

BOOL CCreateCurve::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   NMHDR *pnmhdr = (NMHDR *)lParam;

//   if (pnmhdr->idFrom==IDC_FC_VARLIST) TRACE("%d\n", pnmhdr->code);
   if ((pnmhdr->idFrom==IDC_FC_VARLIST) && (pnmhdr->code==(UINT)-12))
   {
      int i, nCount = m_cVarList.GetItemCount();
      for (i=0; i<nCount; i++)
      {
         if (ListView_GetCheckState(m_cVarList.m_hWnd, i))
         {
            if (i == m_nRunVar) continue;
            else
            {
               if (m_nRunVar != -1) 
               {
                  ListView_SetCheckState(m_cVarList.m_hWnd, m_nRunVar, 0);
               }
               m_nRunVar   = i;
               m_strRunVar = m_cVarList.GetItemText(i, 1);
               DWORD dwData = m_cVarList.GetItemData(i);
               if (dwData)
               {
                  m_pCurve = (CCurveLabel*) dwData;
                  m_nNoOfValues   = m_pCurve->GetNumValues();
                  if (m_pCurve->GetLocus() != 0)
                  {
                     m_dFrom         = m_pCurve->GetLocusValue(0);
                     m_dTo           = m_pCurve->GetLocusValue(m_nNoOfValues-1);
                     m_strRunVarUnit = CString(m_pCurve->GetLocusUnit());
                  }
                  else
                  {
                     if (m_pdMarker)
                     {
                        m_dFrom = m_pdMarker[0];
                        m_dTo   = m_pdMarker[1];
                     }
                     else
                     {
                        m_dFrom         = m_pCurve->GetX_Value(0);
                        m_dTo           = m_pCurve->GetX_Value(m_nNoOfValues-1);
                     }
                     m_strRunVarUnit = CString(m_pCurve->GetXUnit());
                  }
                  m_strResultUnit = CString(m_pCurve->GetYUnit());
                  m_bLogarithmic  = (m_pCurve->GetXDivision() == ETSDIV_LINEAR) ? false : true;
               }
               else
               {
                  m_pCurve = NULL;
               }
               UpdateData(false);
               GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
            }
            break;
         }
      }
      ListView_SetCheckState(m_cVarList.m_hWnd, m_nRunVar, 1);
   }
	return CCurveDialog::OnNotify(wParam, lParam, pResult);
}

void CCreateCurve::OnBeginlabeleditFcVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   if (pDispInfo->item.iItem >= 0)
   {
      DWORD dwData = m_cVarList.GetItemData(pDispInfo->item.iItem);
      if (dwData)                                                                    *pResult = 1;
	   else if (ListView_GetCheckState(pNMHDR->hwndFrom, pDispInfo->item.iItem) != 0) *pResult = 1;
      else                                                                           *pResult = 0;
   }
   else
      *pResult = 0;
}

bool CCreateCurve::SetCurvesToList()
{
   m_bCurves = (m_pCurves && m_pCurves->GetCount()) ? true: false;
   m_cCurveList.DeleteAllItems();
   if (m_bCurves)
   {
      CLabel  *pl;
      CString  strCurve;
      int      i, j, nCount = m_pCurves->GetCount();
      CString  str;

      for (i=0, j=0; i<nCount; i++)
      {
         pl = m_pCurves->GetLabel(i);
         if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            CCurveLabel *pcl = (CCurveLabel*)pl;
            if (pcl->IsInterupted()) continue;

            strCurve.Format(IDS_CR_CURVE_FMT, i+1, 'x');
            m_cCurveList.InsertItem(j, strCurve);
            m_cCurveList.SetItem(j++, 1, LVIF_TEXT, pcl->GetText(), 0, 0, 0, NULL);
            strCurve.Format(IDS_CR_CURVE_FMT, i+1, 'y');
            m_cCurveList.InsertItem(j, strCurve);
            m_cCurveList.SetItem(j++, 1, LVIF_TEXT, pcl->GetText(), 0, 0, 0, NULL);
            if (pcl->GetLocus() != 0)
            {
               strCurve.Format(IDS_CR_CURVE_FMT, i+1, 'c');
               m_cCurveList.InsertItem(j, strCurve);
               m_cCurveList.SetItem(j++, 1, LVIF_TEXT, pcl->GetText(), 0, 0, 0, NULL);
            }
         }
      }
   }
   return m_bCurves;
}

void CCreateCurve::SetVariablesToList()
{
   CString  strCurveFmt;
   int      i, j, n, nCurveFmt, nVar = m_Equation.GetNumVar();
   bool     bInserted;
   LVITEM   lvItem;
   lvItem.mask = LVIF_PARAM|LVIF_TEXT;

   strCurveFmt.LoadString(IDS_CR_CURVE_FMT);
   n = strCurveFmt.Find('%');
   if (n != -1) strCurveFmt = strCurveFmt.Left(n);
   nCurveFmt = strCurveFmt.GetLength();

   m_cVarList.DeleteAllItems();
   char szBuffer[8];
   _stprintf(szBuffer, _T("%.*g"), 1, 0.0);

   LVFINDINFO lvfi = {LVFI_STRING, NULL, 0, {0, 0}, 0};
   for (i=0, j=0; i<nVar; i++)
   {
      lvfi.psz = m_Equation.GetVarName(i);
      if (lvfi.psz)
      {
         bInserted = false;
         if (m_bCurves && (strstr(lvfi.psz, LPCTSTR(strCurveFmt)) != NULL))
         {
            char *ptr = (char*)strstr(&lvfi.psz[nCurveFmt], ".");
            if (ptr && ((ptr[1] == 'x') || (ptr[1] == 'y') || (ptr[1] == 'c')))
            {
               ptr[0] = 0;
               n = atoi(&lvfi.psz[nCurveFmt])-1;
               ptr[0] = '.';
               CLabel *pl = m_pCurves->GetLabel(n);
               if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
               {
                  lvItem.pszText    = (char*)((CCurveLabel*)pl)->GetText();
                  lvItem.cchTextMax = strlen(lvItem.pszText);
                  lvItem.lParam     = (LPARAM)(CCurveLabel*)pl;
                  lvItem.iItem      = j;
                  lvItem.iSubItem   = 0;
                  m_cVarList.InsertItem(&lvItem);
                  bInserted = true;
               }
            }
         }

         if (!bInserted)
         {
            m_cVarList.InsertItem(j, szBuffer);
         }
         m_cVarList.SetItem(j++, 1, LVIF_TEXT, lvfi.psz, 0, 0, 0, NULL);
      }
   }

   if (nVar > 0)
   {
      m_cCurveList.ShowWindow(SW_HIDE);
      m_cVarList.ShowWindow(SW_SHOW);
      m_nRunVar = -1;
      ListView_SetCheckState(m_cVarList.m_hWnd, 0, 1);
   }
}

void CCreateCurve::OnSetfocusFcCurvelist(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (!CheckCurveList())
      SetCurvesToList();
	*pResult = 0;
}

void CCreateCurve::OnSetFocus(CWnd* pOldWnd)
{
   CCurveDialog::OnSetFocus(pOldWnd);
   if (!CheckCurveList())
      SetCurvesToList();
}

bool CCreateCurve::CheckCurveList()
{
   int i, nCount    = 0,
          nVarCount = m_cCurveList.GetItemCount()>>1;
   CLabel*pl;
   CString strCurve, strVar;
   if (m_pCurves) nCount = m_pCurves->GetCount();
   if (nVarCount != nCount) return false;
   for (i=0; i<nCount; i++)
   {
      pl = m_pCurves->GetLabel(i);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         strCurve = _T(((CCurveLabel*)pl)->GetText());
      strVar   = m_cCurveList.GetItemText(i<<1, 1);
      if (strCurve != strVar) return false;
   }
   return true;
}

void CCreateCurve::OptimizeFormat(CCurveLabel *pclIn, CCurveLabel *pclSet)
{
   ASSERT(pclIn  != NULL);
   ASSERT(pclSet != NULL);
   if (m_nResultType == RESULT_TYPE_LOCUS)   // Ortskurven oder Kurven, deren X-Werte nicht konstant steigen
   {
      switch (pclIn->GetFormat())            // Singleformat bei Ortskurve nicht möglich, keine äquidistanten X-Werte
      {
         case CF_DOUBLE: case CF_SINGLE_DOUBLE: pclSet->SetFormat(CF_DOUBLE); break;
         case CF_SHORT:  case CF_SINGLE_SHORT:  pclSet->SetFormat(CF_SHORT);  break;
         case CF_FLOAT:  case CF_SINGLE_FLOAT:  pclSet->SetFormat(CF_FLOAT);  break;
      }
   }
   else                                      // andere Kurven
   {
      switch (pclIn->GetFormat())            // Speicher sparen, Singleformat verwenden
      {
         case CF_SINGLE_DOUBLE: case CF_DOUBLE: pclSet->SetFormat(CF_SINGLE_DOUBLE); break;
         case CF_SINGLE_SHORT:  case CF_SHORT:  pclSet->SetFormat(CF_SINGLE_SHORT);  break;
         case CF_SINGLE_FLOAT:  case CF_FLOAT:  pclSet->SetFormat(CF_SINGLE_FLOAT);  break;
      }
   }
}
