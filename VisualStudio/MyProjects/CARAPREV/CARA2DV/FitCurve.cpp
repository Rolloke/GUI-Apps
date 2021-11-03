// FitCurve.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "FitCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFitCurve 

extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);

#define POLYNOME_FKT    0
#define POTENZ_FKT      1
#define EXPONENTIAL_FKT 2

#ifndef M_E
 #define M_E    2.71828182845905
#endif
CFitCurve::CFitCurve(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CFitCurve::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFitCurve)
   m_nCombo       = POLYNOME_FKT;
   m_nPolyOrdnung = 1;
   m_dFrom        = 0.0;
   m_dTo          = 0.0;
   m_strDiff      = _T("");
   m_strRange     = _T("");
	//}}AFX_DATA_INIT

   m_dDiff        = -1.0;
	m_nValues      = 0;
   m_nFrom        = 0;
   m_nTo          = 0;
}

CFitCurve::~CFitCurve()
{

}

void CFitCurve::DoDataExchange(CDataExchange* pDX)
{
	CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFitCurve)
	DDX_Control(pDX, IDC_CR_VARLIST, m_cVarList);
	DDX_Text(pDX, IDC_CR_FUNCTION_STR, m_strFunction);
	DDX_CBIndex(pDX, IDC_CR_COMBO, m_nCombo);
	DDX_Text(pDX, IDC_CR_DIFF_STR, m_strDiff);
	DDX_Text(pDX, IDC_CR_RANGE_STR, m_strRange);
	//}}AFX_DATA_MAP
   bool bSwapped   = false;
   int  nNumValues;

	DDX_TextVar(pDX, IDC_CR_FROM, DBL_DIG, m_dFrom);
   DDX_TextVar(pDX, IDC_CR_TO  , DBL_DIG, m_dTo);

   if (pDX->m_bSaveAndValidate)
   {
      double dValue, dLimit;
      if (m_pCurve == NULL) pDX->Fail();
      nNumValues = m_pCurve->GetNumValues();
      if (m_dFrom > m_dTo)
      {
         swap(m_dFrom, m_dTo);
         pDX->m_bSaveAndValidate = false;
      	DDX_Text(pDX, IDC_CR_FROM, m_dFrom);
         DDX_Text(pDX, IDC_CR_TO  , m_dTo);
         pDX->m_bSaveAndValidate = true;
      }

      dValue = m_dFrom;
      if (m_pCurve->GetLocus())
      {
         dLimit = m_pCurve->GetLocusValue(0);
         if (dValue < dLimit) dValue = dLimit;
         m_nFrom = 0;
         m_pCurve->Get_Locus_Value(dValue, &m_nFrom);
      }
      else
      {
         dLimit = m_pCurve->GetX_Value(0);
         if (dValue < dLimit) dValue = dLimit;
         m_nFrom = 0;
         m_pCurve->Get_Y_Value(dValue, &m_nFrom);
      }
      m_nTo   = m_nFrom;
      dValue = m_dTo;
      if (m_pCurve->GetLocus())
      {
         dLimit = m_pCurve->GetLocusValue(nNumValues-1);
         if (dValue > dLimit) dValue = dLimit;
         m_pCurve->Get_Locus_Value(dValue, &m_nTo);
      }
      else
      {
         dLimit = m_pCurve->GetX_Value(nNumValues-1);
         if (dValue > dLimit) dValue = dLimit;
         m_pCurve->Get_Y_Value(dValue, &m_nTo);
      }
      if (m_nTo < (nNumValues-1)) m_nTo++;
      m_nValues = (m_nTo - m_nFrom)+1;
   }
   else
   {
      if (m_dDiff == -1)
      {
         CString str;
         str.LoadString(IDS_NO_RESULT);
         DDX_Text(pDX, IDC_CR_DIFF, str);
      }
      else DDX_Text(pDX, IDC_CR_DIFF, m_dDiff);
   }

   if (m_nCombo == POLYNOME_FKT)
   {
      DDX_Text(pDX, IDC_CR_POLY_ORDN, m_nPolyOrdnung);

      if (pDX->m_bSaveAndValidate)
      {
         nNumValues = (m_nValues > MAX_POLYORDNUNG) ? MAX_POLYORDNUNG-1 : m_nValues-1;
         if (nNumValues >= 1)
         {
            DDV_MinMaxInt(pDX, m_nPolyOrdnung, 1, nNumValues);
         }
         else pDX->Fail();
      }
   }
}


BEGIN_MESSAGE_MAP(CFitCurve, CCurveDialog)
	//{{AFX_MSG_MAP(CFitCurve)
	ON_CBN_SELCHANGE(IDC_CR_COMBO, OnSelchangeCrCombo)
	ON_BN_CLICKED(IDC_CR_CALCULATE, OnCrCalculate)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_RCLICK, IDC_CR_VARLIST, OnRclickCrVarlist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CR_VARLIST, OnEndlabeleditCrVarlist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_CR_VARLIST, OnBeginlabeleditCrVarlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFitCurve 



void CFitCurve::OnSelchangeCrCombo() 
{
   CDataExchange dx(this, true);
	DDX_CBIndex(&dx, IDC_CR_COMBO, m_nCombo);
   SelectFunction();
   UpdateData(false);
}

void CFitCurve::OnCrCalculate() 
{
   int nOK = false;
   if (UpdateData(true))
   {
      int n = 2;
      m_cVarList.DeleteAllItems();
      CFloatPrecision _fp;
      switch(m_nCombo)
      {
         case POLYNOME_FKT:
            nOK = FitPolynome();
            n = m_nPolyOrdnung+1;
            break;
         case POTENZ_FKT:
            nOK = FitPotFkt();
            break;
         case EXPONENTIAL_FKT:
            nOK = FitExpFkt();
            break;
      }
      if ((nOK & 0x06) || (nOK == 0))
      {
         AfxMessageBox(IDS_ERROR_OUT_OF_RANGE, MB_OK|MB_ICONEXCLAMATION);
         nOK =0;
      }
      if (nOK)
      {
         int i;
         char szList[4];
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
         char text[128];
         for (i=0; i<n; i++)
         {
            if (m_dVar[i].imagteil != 0.0)
               sprintf(text, "(%e%c %e)", m_dVar[i].realteil, szList[0], m_dVar[i].imagteil);
            else if (!_isnan(m_dVar[i].realteil))
               sprintf(text, "%e", m_dVar[i].realteil);
            else
            {
               sprintf(text, "--");
               nOK = 0;
            }
            m_cVarList.InsertItem(i, text);
            sprintf(text, "a%d :", i);
            m_cVarList.SetItem(i, 1, LVIF_TEXT, text, 0, 0, 0, NULL);
         }
         if (nOK)
         {
            m_dDiff = CalculateDiff();
         }
         else m_dDiff = -1;
         UpdateData(false);
      }
   }
   GetDlgItem(IDC_CALCULATE)->EnableWindow(nOK);
}

BOOL CFitCurve::OnInitDialog()
{
	CCurveDialog::OnInitDialog();
   GetDlgItem(IDC_CALCULATE)->EnableWindow(false);

   m_cVarList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);

   CString str;
   str.LoadString(IDS_FIT_COLUMN_VALUE);
   m_cVarList.InsertColumn(0, str, LVCFMT_LEFT, 99, 0);
   str.LoadString(IDS_FIT_COLUMN_VAR);
   m_cVarList.InsertColumn(1, str, LVCFMT_LEFT, 40, 0);
   int nOrder[2] = {1, 0};
   m_cVarList.SetColumnOrderArray(2, nOrder);

   SelectFunction();
   UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CFitCurve::InitCurveParams()
{
   if (m_pCurve)
   {
      const char * pszText = m_pCurve->GetText();
      if (pszText) m_strNewName.Format(IDS_FIT_CURVE, pszText);
      pszText = m_pCurve->GetYUnit();
      if (pszText) m_strDiff.Format(IDS_FIT_DIFF_STR, pszText);

      if (m_pCurve->GetLocus())
      {
	      m_dFrom = m_pCurve->GetLocusValue(0);
	      m_dTo   = m_pCurve->GetLocusValue(m_pCurve->GetNumValues()-1);
         pszText = m_pCurve->GetLocusUnit();
         if (pszText) m_strRange.Format(IDS_FIT_RANGE_STR, pszText);
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
	         m_dFrom = m_pCurve->GetX_Value(0);
	         m_dTo   = m_pCurve->GetX_Value(m_pCurve->GetNumValues()-1);
         }
         pszText = m_pCurve->GetXUnit();
         if (pszText) m_strRange.Format(IDS_FIT_RANGE_STR, pszText);
      }

      m_cVarList.DeleteAllItems();
      GetDlgItem(IDC_CALCULATE)->EnableWindow(false);
      m_dDiff = 0;
      UpdateData(false);
   }
}

/************************************************************************/
/* Name    : FitPolynome                                                */
/* Zweck   : Berechnung eines Interpolationspolynoms der Art :          */
/*                                                                      */
/*                              2               m                       */
/*           y = c0 + x * c1 + x  * c2 + ... + x  * cm                  */
/*                                                                      */
/*                                                                      */
/*           mit Hilfe der Gauß'schen Methode der kleinsten Quadrate.   */
/*                                                                      */
/* Aufruf  : FitPolynome();                                             */
/* Funktionswert : 1 bei Erfolg sonst 0					                  */
/************************************************************************/
int CFitCurve::FitPolynome()
{
   int 		i,j,k,
            nReturn   = 0,
            n         = m_nValues,
            m         = m_nPolyOrdnung + 1;
   Complex **ppdMatrix = AllocateMatrix(m, m);
   Complex **ppdValMat = AllocateMatrix(n, m);
   Complex *pdL2       = new Complex[m];

   if (ppdMatrix && ppdValMat && pdL2)
   {
      ZeroMemory(pdL2, sizeof(Complex)*m);
      if (m_pCurve->GetLocus())
      {
         SCurve sc;
         for (i=0;i<m; i++)                                       // füllen der überbestimmten Gleichungen
         {
            for (j=0; j<n; j++)
            {
               if (i==0) ppdValMat[j][i] = Complex(1.0, 0);
               else      ppdValMat[j][i] = ppdValMat[j][i-1]*m_pCurve->GetLocusValue(j+m_nFrom);
            }
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            sc = m_pCurve->GetCurveValue(i+m_nFrom);
            pdL2[i] = ppdValMat[0][i]* *((Complex*)&sc);
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               sc = m_pCurve->GetCurveValue(k+m_nFrom);
               pdL2[i] += ppdValMat[k][i]* *((Complex*)&sc);
            }
         }
      }
      else
      {
         for (i=0;i<m; i++)                                       // füllen der überbestimmten Gleichungen
         {
            for (j=0; j<n; j++)
            {
               if (i==0) ppdValMat[j][i] = Complex(1.0, 0);
               else      ppdValMat[j][i] = ppdValMat[j][i-1]*m_pCurve->GetX_Value(j+m_nFrom);
            }
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            pdL2[i] = ppdValMat[0][i]*m_pCurve->GetY_Value(i+m_nFrom);
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               pdL2[i] += ppdValMat[k][i]*m_pCurve->GetY_Value(k+m_nFrom);
            }
         }
      }
      nReturn = Gauss_elimin(m, ppdMatrix, pdL2, m_dVar);
   }

   if (ppdMatrix) DeleteMatrix(m, ppdMatrix);
   if (ppdValMat) DeleteMatrix(n, ppdValMat);
   if (pdL2)      delete[] pdL2;

   return nReturn;
}

int CFitCurve::FitPotFkt()
{
   int 		i,j,k,
            nReturn   = 0,
            n         = m_nValues,
            m         = 2;
   Complex **ppdMatrix = AllocateMatrix(m, m);
   Complex **ppdValMat = AllocateMatrix(n, m);
   Complex *pdL2       = new Complex[m];
   Complex  dVal;

   if (ppdMatrix && ppdValMat && pdL2)
   {
      ZeroMemory(pdL2, sizeof(Complex)*m);
      if (m_pCurve->GetLocus())
      {
         SCurve sc;
         for (j=0; j<n; j++)
         {
            ppdValMat[j][0] = Complex(1.0, 0);
            dVal = Complex(m_pCurve->GetLocusValue(j+m_nFrom), 0);
            ppdValMat[j][1] = Logn(dVal);
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            sc = m_pCurve->GetCurveValue(i+m_nFrom);
            dVal = Logn(*((Complex*)&sc));
            pdL2[i] = ppdValMat[0][i]*dVal;
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               sc = m_pCurve->GetCurveValue(k+m_nFrom);
               dVal = Logn(*((Complex*)&sc));
               pdL2[i] += ppdValMat[k][i]*dVal;
            }
         }
      }
      else
      {
         for (j=0; j<n; j++)
         {
            ppdValMat[j][0] = Complex(1.0, 0);
            dVal = Complex(m_pCurve->GetX_Value(j+m_nFrom), 0);
            ppdValMat[j][1] = Logn(dVal);
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            dVal = Complex(m_pCurve->GetY_Value(i+m_nFrom), 0);
            dVal = Logn(dVal);
            pdL2[i] = ppdValMat[0][i]*dVal;
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               dVal = Complex(m_pCurve->GetY_Value(k+m_nFrom), 0);
               dVal = Logn(dVal);
               pdL2[i] += ppdValMat[k][i]*dVal;
            }
         }
      }
      if (!nReturn) nReturn = Gauss_elimin(m, ppdMatrix, pdL2, m_dVar);
      if (nReturn & 0x01)
      {
         m_dVar[0] = Pow(Complex(M_E, 0), m_dVar[0]);
      }
   }

   if (ppdMatrix) DeleteMatrix(m, ppdMatrix);
   if (ppdValMat) DeleteMatrix(n, ppdValMat);
   if (pdL2)      delete[] pdL2;

   return nReturn;
}

int CFitCurve::FitExpFkt()
{
   int 		i,j,k,
            nReturn   = 0,
            n         = m_nValues,
            m         = 2;
   Complex **ppdMatrix = AllocateMatrix(m, m);
   Complex **ppdValMat = AllocateMatrix(n, m);
   Complex *pdL2       = new Complex[m];
   Complex  dVal;

   if (ppdMatrix && ppdValMat && pdL2)
   {
      ZeroMemory(pdL2, sizeof(Complex)*m);
      if (m_pCurve->GetLocus())
      {
         SCurve sc;
         for (j=0; j<n; j++)
         {
            ppdValMat[j][0] = Complex(1.0, 0);
            ppdValMat[j][1] = Complex(m_pCurve->GetLocusValue(j+m_nFrom), 0);
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            sc = m_pCurve->GetCurveValue(i+m_nFrom);
            pdL2[i] = ppdValMat[0][i] * Logn(*((Complex*)&sc));
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               sc = m_pCurve->GetCurveValue(k+m_nFrom);
               pdL2[i] += ppdValMat[k][i] * Logn(*((Complex*)&sc));
            }
         }
      }
      else
      {
         for (j=0; j<n; j++)
         {
            ppdValMat[j][0] = Complex(1.0, 0);
            ppdValMat[j][1] = Complex(m_pCurve->GetX_Value(j+m_nFrom), 0);
         }

         GMdKlQuMatrix(n, m, ppdValMat, ppdMatrix);

         for (i=0; i<m; i++)
         {
            pdL2[i] = ppdValMat[0][i]*log(m_pCurve->GetY_Value(i+m_nFrom));
         }

         for (i=0; i<m; i++)
         {
            for (k=1; k<n; k++)
            {
               dVal = Complex(m_pCurve->GetY_Value(k+m_nFrom), 0);
               pdL2[i] += ppdValMat[k][i]*Logn(dVal);
            }
         }
      }
      nReturn = Gauss_elimin(m, ppdMatrix, pdL2, m_dVar);
      if (nReturn)
      {
         m_dVar[0] = Pow(Complex(M_E, 0), m_dVar[0]);
      }
   }

   if (ppdMatrix) DeleteMatrix(m, ppdMatrix);
   if (ppdValMat) DeleteMatrix(n, ppdValMat);
   if (pdL2)      delete[] pdL2;

   return nReturn;
}

void CFitCurve::GMdKlQuMatrix(int n, int m, Complex **ppdValMat, Complex **ppdMatrix)
{
   int i, j, k;
   for (i=0; i<m; i++)
   {
      for (j=i; j<m; j++)
      {
         ppdMatrix[i][j] = ppdValMat[0][i]*ppdValMat[0][j];
      }
   }

   for (i=0; i<m; i++)
   {
      for (j=i; j<m; j++)
      {
         for (k=1; k<n; k++)
            ppdMatrix[i][j] += ppdValMat[k][i]*ppdValMat[k][j];
         ppdMatrix[j][i] = ppdMatrix[i][j];
      }
   }
}

double CFitCurve::CalculateDiff()
{
   double  summe = 0.0;
   Complex diff;
   int   i;
   Complex  (* pFkt)(Complex, Complex*, int);
   switch(m_nCombo)
   {
      case POLYNOME_FKT:    pFkt = CFitCurve::PolynomeValue; break;
      case POTENZ_FKT:      pFkt = CFitCurve::PotFktValue;   break;
      case EXPONENTIAL_FKT: pFkt = CFitCurve::ExpFktValue;   break;
      default: return 0;
   }
   if (m_pCurve->GetLocus())
   {
      for (i=m_nFrom; i<=m_nTo; i++)
      {
         SCurve scv = m_pCurve->GetCurveValue(i);
         diff   = *((Complex*)&scv) - pFkt(m_pCurve->GetLocusValue(i), m_dVar, m_nPolyOrdnung);
         summe += Asqr(diff);     			                           // Abweichungquadrate aufsummieren
      }
   }
   else
   {
      for (i=m_nFrom; i<=m_nTo; i++)
      {
         diff   = Complex(m_pCurve->GetY_Value(i), 0)-pFkt(m_pCurve->GetX_Value(i), m_dVar, m_nPolyOrdnung);
         summe += Asqr(diff);     			                           // Abweichungquadrate aufsummieren
      }
   }
   return sqrt(summe/(m_nValues-1));                           // Standardabweichung berechnen
}

Complex CFitCurve::PolynomeValue(Complex dX, Complex *pdVar, int n)
{
   Complex dSum = pdVar[n];
   int i;
   for (i=n-1; i>=0; i--)
   {
      dSum *= dX;
      dSum += pdVar[i];
   }
   return dSum;
}
Complex CFitCurve::PotFktValue(Complex dX, Complex *pdVar, int n)
{
   UNUSED(n);
   return pdVar[0] * Pow(dX, pdVar[1]);
}
Complex CFitCurve::ExpFktValue(Complex dX, Complex *pdVar, int n)
{
   UNUSED(n);
   return pdVar[0] * Pow(Complex(M_E, 0), dX*pdVar[1]);
}

/***************************************************************************/
/* Name   : Gauss_elimin                                                   */
/* Zweck  : Lösung eines Gleichungssystems mit Hilfe der Gauß'schen        */
/*          Eliminationsmethode                                            */
/* Aufruf : Gauss_elimin(n, ppdMatrix, pVector, pErg);                     */
/* Parameter :                                                             */
/* n       (E) : Grad des Gleichungssystems                    (Complex*)   */
/* ppdMatrix(E) : Koeffizientenmatrix                          (Complex**)  */
/* pVector (E) : Ergebnisvektor für die Gleichungen            (Complex*)   */
/* pErg    (A) : Loesungsvektor des Gleichungssystems          (Complex*)   */
/* Funktionswert : 1 bei Erfolg, 0 bei nicht lösbar	         (int)       */
/***************************************************************************/
int CFitCurve::Gauss_elimin(int n, Complex **ppdMatrix, Complex *pVector, Complex *pErg)
{
   int 	   i,j,k;
   int 	   pivotzeile, laenge;
   int 	   *pnIndex = new int[n];
   Complex 	pivot,faktor;
   Complex 	*ptr_von, *ptr_ist, *ptr_pivot;
   int      bReturn = 0;

   if (!pnIndex) return bReturn;

   for(i=0; i<n; i++) pnIndex[i] = i;

   for (i=0;i<n;i++)
   {
      pivot      = ppdMatrix[pnIndex[i]][i];
      pivotzeile = i;

      for (j=i+1;j<n;j++)                                      // Suche Pivotzeile
      {
         if (Asqr(ppdMatrix[pnIndex[j]][i])>Asqr(pivot))
         {
            pivot      = ppdMatrix[pnIndex[j]][i];
            pivotzeile = j;
         }
      }

      if (i!=pivotzeile)                                       // wenn anderes pivot gefunden Zeilen tauschen
      {
         k                   = pnIndex[i];
         pnIndex[i]          = pnIndex[pivotzeile];
         pnIndex[pivotzeile] = k;
      }

      if (Asqr(pivot)<1e-100) break;                           // Gleichung nicht lösbar

      pivot  = 1.0 / pivot;
      ptr_pivot = &ppdMatrix[pnIndex[i]][i+1];
      laenge = n-i-1;

      for (j=i+1;j<n;j++)
      {
         ptr_ist = &ppdMatrix[pnIndex[j]][i];
         if (Asqr(*ptr_ist)!=0.0)
         {
            faktor  = *(ptr_ist++) * pivot;
            ptr_von = ptr_pivot;

            for (k=0;k<laenge;k++)
               *(ptr_ist++) -= faktor* *(ptr_von++);

            pVector[pnIndex[j]] -= faktor*pVector[pnIndex[i]];
         }
      }
   }

   if (i==n)
   {
      for(i=n-1;i>=0;i--)
      {
         pVector[pnIndex[i]] = faktor = pVector[pnIndex[i]]/ppdMatrix[pnIndex[i]][i];
         for (j=i-1;j>=0;j--)
            pVector[pnIndex[j]] -= faktor * ppdMatrix[pnIndex[j]][i];
      }

      for (i=0; i<n; i++)
         pErg[i] = pVector[pnIndex[i]];
      bReturn = 1;
   }
   delete[] pnIndex;

   return bReturn;
}

Complex** CFitCurve::AllocateMatrix(int n, int m)
{
   int i;
   Complex **ppdMatrix = (Complex**) new char[sizeof(Complex*) * n];
   if (ppdMatrix)
   {
      for (i=0; i<n; i++) ppdMatrix[i] = NULL;
      for (i=0; i<n; i++)
      {
         ppdMatrix[i] = new Complex[m];
         if (!ppdMatrix[i]) break;
         ZeroMemory(ppdMatrix[i], sizeof(Complex) * m);
      }
      if (i!=n)
      {
         DeleteMatrix(n, ppdMatrix);
         ppdMatrix = NULL;
      }
   }
   return ppdMatrix;
}

void CFitCurve::DeleteMatrix(int n, Complex**ppdMatrix)
{
   int i;
   if (ppdMatrix)
   {
      for (i=0; i<n; i++)
      {
         if (ppdMatrix[i])
         {
	         delete[] ppdMatrix[i];
         }
      }
      delete[] ((char*) ppdMatrix);
   }
}
void CFitCurve::OnOK() 
{
   CWnd *pOk = GetDlgItem(IDC_CALCULATE);
   if (pOk && pOk->IsWindowEnabled() && m_pCurve && UpdateData(true)) // Rechenparameter ermitteln
   {
      pOk->EnableWindow(false);
      m_pThread = AfxBeginThread(ThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
      if (m_pThread)
      {
         m_pThread->m_bAutoDelete = TRUE;
         m_pThread->ResumeThread();
      }
   }
   else UpdateData(false);
}

UINT CFitCurve::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CFitCurve *pFit = (CFitCurve*) pParam;
      double dOrg;
      double dStep;
      bool   bLinear = false;
      bool   bLocus  = false;
      int    nValues = 0;
      int    nDiv = 0;
      if (pFit->m_pCurve->GetLocus())
      {
         dOrg   = pFit->m_pCurve->GetLocusValue(0);
         dStep  = pFit->m_pCurve->GetLocusValue(1);
         nDiv   = pFit->m_pCurve->GetLocus();
         bLocus = true;
      }
      else
      {
         dOrg  = pFit->m_pCurve->GetX_Value(0);
         dStep = pFit->m_pCurve->GetX_Value(1);
         nDiv  = pFit->m_pCurve->GetXDivision();
      }
      pFit->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pFit->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);

      if (nDiv <= CCURVE_LINEAR)
      {
         dStep   = dStep - dOrg;
         nValues = (int)(0.5 + (pFit->m_dTo - pFit->m_dFrom) / dStep);
         bLinear = true;
      }
      else
      {
         dStep   = dStep / dOrg;
         nValues = (int)(0.5 + (log(pFit->m_dTo / dOrg) - log(pFit->m_dFrom / dOrg)) / log(dStep));
      }

      pFit->m_pNewCurve = new CCurveLabel(pFit->m_pCurve);     // neue Kurve erzeugen
      if (pFit->m_pNewCurve)
      {
         pFit->m_pNewCurve->SetTargetFuncNo(0);
         pFit->m_pNewCurve->SetColor(0xffffffff);              // Parameter setzen
         pFit->m_pNewCurve->SetText((char*)LPCTSTR(pFit->m_strNewName));
          
         if (pFit->m_pNewCurve->SetSize(nValues))              // Konnte Kurve erzeugt werden
         {
            int i;
            Complex dX;
            Complex  (* pFkt)(Complex, Complex*, int);
            switch(pFit->m_nCombo)
            {
               case POLYNOME_FKT:    pFkt = CFitCurve::PolynomeValue; break;
               case POTENZ_FKT:      pFkt = CFitCurve::PotFktValue;   break;
               case EXPONENTIAL_FKT: pFkt = CFitCurve::ExpFktValue;   break;
               default: return 0;
            }
            CFloatPrecision _fp;
            dOrg = pFit->m_dFrom;

            pFit->m_pNewCurve->SetXOrigin(dOrg);
            pFit->m_pNewCurve->SetXStep(dStep);
            if (bLocus)
            {
               for (i=0; i<nValues; i++)
               {
                  dX.realteil = dOrg;
                  dX.imagteil = 0;
//                  pFit->m_pNewCurve->SetX_Value(i, dX.realteil);
                  dX = pFkt(dX, pFit->m_dVar, pFit->m_nPolyOrdnung);
                  pFit->m_pNewCurve->SetCurveValue(i, *((SCurve*)&dX));
                  if (bLinear) dOrg += dStep;
                  else         dOrg *= dStep;
               }
            }
            else
            {
               for (i=0; i<nValues; i++)
               {
                  dX.realteil = dOrg;
                  dX.imagteil = 0;
//                  pFit->m_pNewCurve->SetX_Value(i, dX.realteil);
                  dX = pFkt(dX, pFit->m_dVar, pFit->m_nPolyOrdnung);
//                  if (dX.realteil) pFit->m_pNewCurve->SetY_Value(i, Betrag(dX)); else 
                  pFit->m_pNewCurve->SetY_Value(i, dX.realteil);
                  if (bLinear) dOrg += dStep;
                  else         dOrg *= dStep;
               }
            }
            pFit->GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);
            pFit->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_SHOW);
            pFit->GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
            pFit->PostMessage(WM_ENDTHREAD, 0, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pFit->PostMessage(WM_ENDTHREAD, 0, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pFit->m_pThread = NULL;
   }
   return uResult;
}
void CFitCurve::SelectFunction()
{
   bool bShowOrdnung = false;
   switch(m_nCombo)
   {
      case POLYNOME_FKT:
         m_strFunction.LoadString(IDS_FIT_POLYNOMFKT);
         bShowOrdnung = true;
         break;
      case POTENZ_FKT:
         m_strFunction.LoadString(IDS_FIT_POT_FKT);
         break;
      case EXPONENTIAL_FKT:
	      m_strFunction.LoadString(IDS_FIT_EXP_FKT);
         break;
      default: ASSERT(false);
   }
   GetDlgItem(IDC_CR_POLY_ORDN_STR)->ShowWindow(bShowOrdnung ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_CR_POLY_ORDN    )->ShowWindow(bShowOrdnung ? SW_SHOW : SW_HIDE);
}
void CFitCurve::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   if (pWnd && (pWnd == GetDlgItem(IDC_CR_VARLIST)))
   {
      return;
   }
   else CDialog::OnContextMenu(pWnd, point);
}


void CFitCurve::OnRclickCrVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint ptScreen;
   CMenu Menu, *pContext;
   Menu.LoadMenu(IDR_CONTEXT_MENUES);
   pContext = Menu.GetSubMenu(0);

   GetCursorPos(&ptScreen);

   pContext = Menu.GetSubMenu(0);
   if (pContext)
   {
      pContext->EnableMenuItem(ID_SHOW_EQUATION_STRUCTURE, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
      pContext->EnableMenuItem(ID_LOAD_EQUATION, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
      if (pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this) == ID_SAVE_EQUATION)
      {
         CString        title;
         CString        curvefilter, otherFilter;
         CCARA2DVApp   *pApp  = (CCARA2DVApp*) AfxGetApp();
         int nCustMaxFilter = 1;

         otherFilter.LoadString(IDS_TXT_FILTER);
         curvefilter += otherFilter;
         curvefilter += _T("|*.TXT|");
         nCustMaxFilter++;
         CFileDialog fd(false, "TXT", m_strNewName, OFN_OVERWRITEPROMPT|OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_EXPLORER, LPCTSTR(curvefilter), AfxGetApp()->m_pMainWnd);
         fd.m_ofn.nMaxCustFilter = nCustMaxFilter;

         title.LoadString(AFX_IDS_SAVEFILE);
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
            CFile    file(fd.GetPathName(), CFile::modeCreate|CFile::modeWrite);
            CArchive ar(&file, CArchive::store);
            CString str;
            int i, n = 2;
            if (m_nCombo == POLYNOME_FKT) n = m_nPolyOrdnung+1;
            str = _T("\r\n");
            ar.WriteString(m_strFunction);
            ar.WriteString(str);
            str.Format(IDS_FIT_DIFF_STR, (m_pCurve != NULL) ? m_pCurve->GetYUnit() : "");
            ar.WriteString(str);
            str.Format(": %.14e\r\n", m_dDiff);
            ar.WriteString(str);
            char szList[4];
            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
            for (i=0; i<n; i++)
            {
               if (m_dVar[i].imagteil)
                  str.Format("a%d :(%.14e%c %.14e)\r\n", i, m_dVar[i].realteil, szList[0], m_dVar[i].imagteil);
               else
                  str.Format("a%d :%.14e\r\n", i, m_dVar[i].realteil);
               ar.WriteString(str);
            }
         }
      }
   }
	*pResult = 0;
}

void CFitCurve::OnEndlabeleditCrVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   
   if ((pDispInfo->item.mask & LVIF_TEXT) && 
       CLabel::IsBetween(pDispInfo->item.iItem, 0, MAX_POLYORDNUNG -1) &&
       m_cVarList.UpdateData(true))
   {
      m_dVar[pDispInfo->item.iItem] = m_cVarList.m_cValue;
      *pResult = 1;
   }
   else
	   *pResult = 0;
}

void CFitCurve::OnBeginlabeleditCrVarlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
}
