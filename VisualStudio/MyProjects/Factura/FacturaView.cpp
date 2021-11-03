// FacturaView.cpp : Implementierung der Klasse CFacturaView
//

#include "stdafx.h"
#include "Factura.h"

#include "FacturaDoc.h"
#include "FacturaView.h"
#include "Customer.h"
#include "Invoice.h"
#include "Product.h"
#include "PrintAdr.h"
#include "States.h"
#include "EditFormDlg.h"
#include "PayMode.h"
#include "CurrencyUnit.h"
#include "BaseData.h"
#include "FindCustomer.h"
#include "ETS3DGLRegKeys.h"
#include "PrintFromTo.h"
#include "SendEmail.h"

#include "TextLabel.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFacturaView

IMPLEMENT_DYNCREATE(CFacturaView, CCaraView)

BEGIN_MESSAGE_MAP(CFacturaView, CCaraView)
	//{{AFX_MSG_MAP(CFacturaView)
	ON_COMMAND(ID_EDIT_NEWCUSTOMER, OnEditNewcustomer)
	ON_COMMAND(ID_EDIT_NEWINVOICE, OnEditNewinvoice)
	ON_COMMAND(ID_EDIT_NEWPRODUCTS, OnEditNewproducts)
	ON_COMMAND(ID_EXTRA_PRINT_ADR, OnExtraPrintAdr)
	ON_COMMAND(ID_EXTRA_PRINT_BANK, OnExtraPrintBank)
	ON_COMMAND(ID_EDIT_STATES, OnEditStates)
	ON_COMMAND(ID_FILE_PRINT_LOGFILE, OnFilePrintLogfile)
	ON_COMMAND(ID_EXTRA_EDIT_FORM, OnExtraEditForm)
	ON_COMMAND(ID_EDIT_PAYMODES, OnEditPaymodes)
	ON_COMMAND(ID_EDIT_CURRENCYUNITS, OnEditCurrencyunits)
	ON_COMMAND(ID_EDIT_BASE_DATA, OnEditBaseData)
	ON_COMMAND(ID_EXTRA_PRINT_FORM_DIRECT, OnExtraPrintFormDirect)
	ON_COMMAND(ID_EXTRA_PRINT_STANDARD_LETTER, OnExtraPrintStandardLetter)
	ON_COMMAND(ID_DATA_STATISTIC, OnDataStatistic)
	ON_WM_TIMER()
	ON_COMMAND(ID_EXTRA_PRINT_PRODUCTS, OnExtraPrintProducts)
	ON_COMMAND(ID_EXTRA_PRINT_FORM, OnExtraPrintForm)
	ON_COMMAND(ID_EXTRA_SEND_MAIL, OnExtraSendMail)
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
// Update Command Message
/*
   ON_UPDATE_COMMAND_UI(ID_EDIT_NEWCUSTOMER, OnUpdateEditNewcustomer)
   ON_UPDATE_COMMAND_UI(ID_EDIT_NEWINVOICE, OnUpdateEditNewinvoice)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEWPRODUCTS, OnUpdateEditNewproducts)
	ON_UPDATE_COMMAND_UI(ID_EXTRA_PRINT_ADR, OnUpdateExtraPrintAdr)
	ON_UPDATE_COMMAND_UI(ID_EXTRA_PRINT_BANK, OnUpdateExtraPrintBank)
	ON_UPDATE_COMMAND_UI(ID_EDIT_STATES, OnUpdateEditStates)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PAYMODES, OnUpdateEditPaymodes)
	ON_UPDATE_COMMAND_UI(ID_EXTRA_PRINT_STANDARD_LETTER, OnUpdateExtraPrintStandardLetter)
*/


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFacturaView Konstruktion/Destruktion

#define EDIT_FORM_LABELS   0x00010000
#define PRINT_FORM_LABELS  0x00020000
#define FORM_MASK          0x0000ffff

#define REGKEY_DEFAULT_AUTOSAVE "AutoSave"
#define ID_AUTOSAVE_TIMER       1000

CFacturaView::CFacturaView()
{
   m_nPrepareFormLabels = 0;
   m_bPrintDirect       = false;
   m_nPrintForm         = -1;
}

CFacturaView::~CFacturaView()
{
   BEGIN("~CFacturaView()");
}

BOOL CFacturaView::PreCreateWindow(CREATESTRUCT& cs)
{
   return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaView Zeichnen

void CFacturaView::OnDraw(CDC* pDC)
{
   if (pDC->IsPrinting())
      CCaraView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaView Drucken


BOOL CFacturaView::OnPreparePrinting(CPrintInfo* pInfo)
{
   return CCaraView::OnPreparePrinting(pInfo);
}

void CFacturaView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   CFacturaDoc     *pDoc   = GetDocument();
   CLabelContainer *plcDoc = pDoc->GetLabelContainer();
   CLabel          *pl;

   int nLabel = (m_nPrepareFormLabels & FORM_MASK) -1;
   if (nLabel != -1)
   {
      if (m_nPrepareFormLabels & EDIT_FORM_LABELS)
      {
         pl = plcDoc->GetLabel(nLabel);
         if (pl && !pl->IsVisible() && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         {
            CLabelContainer *plc = (CLabelContainer*) pl;
            pl = plc->GetLabel(0);
            if (pl && !pl->IsVisible() && pl->IsKindOf(RUNTIME_CLASS(CTextLabel)))
            {
               plc->RemoveLabel(pl);
               delete pl;
               plc->ProcessWithLabels(NULL, NULL, CFacturaView::RemoveDummyTextLabels);
               plcDoc->InsertList(plc, false);
               plcDoc->RemoveLabel(plc);
               plc->RemoveAll();
               delete plc;
            }
         }
      }
      else if (m_nPrepareFormLabels & PRINT_FORM_LABELS)
      {
         pDoc->CheckActualPointers();
         pl = plcDoc->GetLabel(nLabel);
         if (pl && !pl->IsVisible() && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         {
            CLabelContainer *plc = (CLabelContainer*) pl;
            ProcessLabel prl = {NULL, 0, GetDocument(), NULL};
            plc->ProcessWithLabels(NULL, &prl, CFacturaView::FillDummyTextLabels);
            pl->SetVisible(true);
            pl->SetFixed(true);
         }
      }
   }
   CCaraView::OnBeginPrinting(pDC, pInfo);
   if (pDoc->m_pCaraLogo) pDoc->m_pCaraLogo->SetVisible(false);
}

void CFacturaView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   CLabelContainer *plcDoc = GetDocument()->GetLabelContainer();
   CLabel          *pl;

   int nLabel = (m_nPrepareFormLabels & FORM_MASK)-1;
   if (nLabel != -1)
   {
      if (m_nPrepareFormLabels & EDIT_FORM_LABELS)
      {
         CLabelContainer *plc = new CLabelContainer;
         if (plc)
         {
            ProcessLabel prl = {plc, 0, NULL, NULL};
            plc->SetDeleteContent(true);
            plc->SetVisible(false);
            CPoint pt(0,0);
            CTextLabel *ptl = new CTextLabel(&pt, (char*)LPCTSTR(m_strFormLabelName));
            if (ptl)
            {
               plc->InsertLabel(ptl);
               ptl->SetVisible(false);
               plcDoc->ProcessWithLabels(NULL, &prl, CFacturaView::InsertVisibleLabels);
               if (prl.nPos > 0)
               {
                  prl.pl = NULL;
                  plc->ProcessWithLabels(NULL, &prl, CFacturaView::InsertDummyTextLabels);
                  plcDoc->InsertLabel(plc, nLabel);
               }
               else delete plc;
            }
         }
      }
      else if (m_nPrepareFormLabels & PRINT_FORM_LABELS)
      {
         pl = plcDoc->GetLabel(nLabel);
         if (pl && pl->IsVisible() && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         {
            pl->SetVisible(false);
         }
      }
   }
   m_nPrepareFormLabels = 0;
   CCaraView::OnEndPrinting(pDC, pInfo);
}

int CFacturaView::FillDummyTextLabels(CLabel *pl, void *p)
{
   ASSERT(p  != NULL);
   ASSERT(pl != NULL);
   if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) && (((CLabelContainer*)pl)->GetCount() == 2))
   {
      CLabelContainer *plc     = (CLabelContainer*) pl;
      CLabel          *plText1 = plc->GetLabel(0);
      CLabel          *plText2 = plc->GetLabel(1);
      if (plText1 && plText1->IsKindOf(RUNTIME_CLASS(CTextLabel)) &&
          plText2 && plText2->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         CTextLabel   *pTL  = (CTextLabel*)plText1;
         const char   *ptxt = pTL->GetText();
         if (ptxt != NULL)
         {
            const char *pstr = strstr(ptxt, FORMAT_TEXT_SIGN);
            if (pstr)
            {
               CString strText;
               ASSERT_KINDOF(CFacturaDoc,(CObject*)((ProcessLabel*)p)->pParam1);
               ((CFacturaDoc*)((ProcessLabel*)p)->pParam1)->FormatDataString(pstr, strText);
               ((CTextLabel*)plText2)->SetText((char*)LPCTSTR(strText));
            }
         }
      }
   }
   return 0;
}

int CFacturaView::RemoveDummyTextLabels(CLabel *pl, void *p)
{
   UNUSED(p);
   ASSERT(pl != NULL);
   if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) && (((CLabelContainer*)pl)->GetCount() == 2))
   {
      CLabelContainer *plc     = (CLabelContainer*) pl;
      CLabel          *plText1 = plc->GetLabel(0);
      CLabel          *plText2 = plc->GetLabel(1);
      if (plText1 && plText1->IsKindOf(RUNTIME_CLASS(CTextLabel)) &&
          plText2 && plText2->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         CTextLabel   *pTL  = (CTextLabel*)plText1;
         const char   *ptxt = pTL->GetText();
         if (ptxt != NULL)
         {
            const char *pstr = strstr(ptxt, FORMAT_TEXT_SIGN);
            if (pstr)
            {
               plText1->SetVisible(true);
               plc->RemoveLabel(plText2);
               delete plText2;
            }
         }
      }
   }
   return 0;
}

int CFacturaView::InsertDummyTextLabels(CLabel *pl, void *p)
{
   ASSERT(p != NULL);
   ASSERT(pl != NULL);
   if (pl->IsKindOf(RUNTIME_CLASS(CTextLabel)))
   {
      CTextLabel   *pTL  = (CTextLabel*)   pl;
      const char   *ptxt = pTL->GetText();
      if (ptxt != NULL)
      {
         const char *pstr = strstr(ptxt, FORMAT_TEXT_SIGN);
         if (pstr)
         {
            CLabelContainer *pLC  = new CLabelContainer;
            pTL                   = new CTextLabel(pTL);
            if (pLC && pTL)
            {
               pTL->SetText(NULL);
               ((ProcessLabel*) p)->pl = pLC;
               pLC->SetDeleteContent(true);
               pl->SetVisible(false);
               pLC->InsertLabel(pl);
               pLC->InsertLabel(pTL);
               return (REMOVE_LABEL|INSERT_LABEL_BEFORE);
            }
            if (pLC) delete pLC;
            if (pTL) delete pTL;
         }
      }
   }
   return 0;
}

int CFacturaView::InsertVisibleLabels(CLabel *pl, void *p)
{
   ASSERT(pl != NULL);
   ASSERT(p  != NULL);
   int nReturn = 0;
   ProcessLabel *prl = (ProcessLabel*)p;
   if (pl && pl->IsVisible())
   {
      if (((CLabelContainer*)prl->pl)->InsertLabel(pl))
      {
         nReturn = REMOVE_LABEL;
         prl->nPos++;
      }
   }
   if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
   {
      nReturn |= DONT_PROCESS_CONTENT;
   }
   return nReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaView Diagnose

#ifdef _DEBUG
void CFacturaView::AssertValid() const
{
	CView::AssertValid();
}

void CFacturaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFacturaDoc* CFacturaView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFacturaDoc)));
	return (CFacturaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFacturaView Nachrichten-Handler
void CFacturaView::OnEditNewcustomer() 
{
   CCustomer dlg;
   dlg.DoModal();
}
void CFacturaView::OnEditNewinvoice() 
{
   CInvoice dlg;
   dlg.m_pParent = this;
   dlg.DoModal();
}
void CFacturaView::OnEditNewproducts() 
{
   CProduct dlg;
   dlg.DoModal();
}

void CFacturaView::OnExtraPrintAdr() 
{
   CPrintAdr dlg;
   dlg.DoModal();
}

void CFacturaView::OnExtraPrintBank() 
{
   GetDocument()->m_dlgPrintBankKl.DoModal();
}

void CFacturaView::OnEditStates() 
{
   CStates dlg;
   dlg.DoModal();
}

void CFacturaView::OnEditPaymodes() 
{
   CPayMode dlg;
   dlg.DoModal();
}

void CFacturaView::OnEditCurrencyunits() 
{
   CCurrencyUnit dlg;
   dlg.DoModal();
}

void CFacturaView::OnFilePrintLogfile() 
{
   CFile file;
   char szTempPath[_MAX_PATH];
   CString strFilename;
   GetTempPath(_MAX_PATH, szTempPath);
   if (strFilename.LoadString(IDS_LOG_FILE_NAME))
      strcat(szTempPath, strFilename);
   BOOL bOpen = file.Open(szTempPath, CFile::modeRead);
   if (bOpen)
   {
      int nLength = file.GetLength();
      char *pszFile = new char[nLength];
      if (pszFile)
      {
         file.Read(pszFile, nLength);
         CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
         pApp->SetActualPrinter(LOGFILE_PRINTER);
         if (pApp->OpenPrinter())
         {
            int i;
            for (i=0; i<nLength; i++)
            {
               pApp->StandardPrint(0, &pszFile[i], 0, 0, 1);
            }
            pApp->CloseStandardPrinter();
         }
         delete[] pszFile;
      }
      file.Close();
      if (AfxMessageBox(IDS_DELETE_LOGFILE, MB_YESNO)== IDYES)
      {
         CFile::Remove(szTempPath);
      }
   }
}

void CFacturaView::OnExtraEditForm() 
{
   CEditFormDlg dlg;
   dlg.m_pParent = this;
   if (dlg.DoModal() == IDOK)
   {
      m_strFormLabelName   = dlg.m_strCombo;
      m_nPrepareFormLabels = (dlg.m_nPosition+1) | EDIT_FORM_LABELS;
      OnFilePrintPreview();
   }
}

void CFacturaView::OnExtraPrintFormDirect() 
{
	m_bPrintDirect = true;
   OnExtraPrintForm();
	m_bPrintDirect = false;
}

void CFacturaView::OnExtraPrintForm() 
{
   CEditFormDlg dlg;
   dlg.m_pParent     = this;
   dlg.m_bNoNewForms = true;
   dlg.m_nPosition   = m_nPrintForm;
   if (m_bPrintDirect) dlg.m_strCaption.LoadString(IDS_PRINT_DLG_CAPTION);
   else                dlg.m_strCaption.LoadString(IDS_VIEW_DLG_CAPTION);
   if (dlg.DoModal() == IDOK)
   {
      m_nPrintForm         = dlg.m_nPosition;
      m_strFormLabelName   = dlg.m_strCombo;
      m_nPrepareFormLabels = (dlg.m_nPosition+1) | PRINT_FORM_LABELS;
      if (m_bPrintDirect) OnFilePrint();
      else                OnFilePrintPreview();
   }
}

void CFacturaView::OnExtraPrintStandardLetter() 
{
   CEditFormDlg dlgEF;
   dlgEF.m_pParent     = this;
   dlgEF.m_bNoNewForms = true;
   dlgEF.m_nPosition   = m_nPrintForm;
   dlgEF.m_strCaption.LoadString(IDS_PRINT_DLG_CAPTION);
   if (dlgEF.DoModal() == IDOK)
   {
      m_nPrintForm         = dlgEF.m_nPosition;
      m_strFormLabelName   = dlgEF.m_strCombo;

      CFindCustomer dlgFC;
      dlgFC.m_nMultiSelect = 1;
      if (dlgFC.DoModal() == IDOK)
      {
         int        nCount = dlgFC.m_nMultiSelect;
         Customer **ppList = dlgFC.GetCustomerList();
         if (ppList)
         {
            CFacturaDoc * pDoc = GetDocument();
            for (int i=0; i<nCount; i++)
            {
               pDoc->SetActualCustomer(ppList[i]);
               m_nPrepareFormLabels = (dlgEF.m_nPosition+1) | PRINT_FORM_LABELS;
               if (i==0)
                  SendMessage(WM_COMMAND, MAKELONG(ID_FILE_PRINT, 0), NULL);
               else
                  SendMessage(WM_COMMAND, MAKELONG(ID_FILE_PRINT_DIRECT, 0), NULL);
            }
         }
      }
   }	
}

void CFacturaView::OnEditBaseData() 
{
   CFacturaDoc *pDoc = GetDocument();
   BaseData dlg;
   dlg.m_fNNAmount = pDoc->m_fNNAmount;
   dlg.m_fTax      = pDoc->m_fTax * 100.0f;
   if (dlg.DoModal() == IDOK)
   {
      pDoc->m_fNNAmount = dlg.m_fNNAmount;
      pDoc->m_fTax      = dlg.m_fTax * 0.01f;
   }
}


void CFacturaView::OnDataStatistic() 
{
   CFacturaDoc *pDoc = GetDocument();
   CString strInfo;
   strInfo.Format("Kunden \t\t: %d\nArtikel \t\t: %d\nRechnungen \t: %d",
                  pDoc->m_Customers.GetCount(),
                  pDoc->m_Products.GetCount(),
                  pDoc->m_Invoices.GetCount());
   if (::MessageBox(AfxGetMainWnd()->m_hWnd, strInfo, "Statistik", MB_OKCANCEL))
   {
      // PlotView..
   }
}

BOOL CFacturaView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
   BOOL bOK = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
   if (bOK)
   {
      int nTime = AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_AUTOSAVE, 0);
      if (nTime > 0)
	      SetTimer(ID_AUTOSAVE_TIMER, nTime*1000, NULL);
   }
	return bOK;
}

void CFacturaView::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == ID_AUTOSAVE_TIMER)
   {
      if (GetDocument()->IsModified())
      {
         AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKELONG(ID_FILE_SAVE, 0), (LPARAM)m_hWnd);
      }
   }
	CCaraView::OnTimer(nIDEvent);
}

void CFacturaView::OnExtraPrintProducts() 
{
   CFacturaDoc *pDoc   = GetDocument();
   CPrintFromTo dlg;
   POSITION  pos = pDoc->m_Products.GetTailPosition();
   Product  *pProduct = NULL;
   dlg.m_nRangeFrom = 0;
   dlg.m_nRangeTo   = pDoc->m_Products.GetCount(); 
   if (pos)
   {
     pProduct = (Product*)pDoc->m_Products.GetPrev(pos);
     dlg.m_nRangeTo = pProduct->nNo;
   }
   if (dlg.DoModal() == IDOK)
   {
      CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
      pApp->SetActualPrinter(LOGFILE_PRINTER);
      if (pApp->OpenPrinter())
      {
         int  nProd = 0;
         char szTextOut[256];
         pos = pDoc->m_Products.GetHeadPosition();
         for (;pos!=0; nProd++)
         {
            pProduct = (Product*)pDoc->m_Products.GetNext(pos);
            if (pProduct->nNo < dlg.m_nFrom) continue;
            if (pProduct->nNo > dlg.m_nTo  ) break;
            if (pProduct)
            {
               if (!dlg.m_bPrintHidden)
               {
                  if (pProduct->sEAN_Code.ucPriceGroup & PRICE_GROUP_ARTIKEL_HIDDEN)
                     continue;
               }
               wsprintf(szTextOut, "Art Nr(%3d): %02d%1d%02d %d", pProduct->nNo, pProduct->sEAN_Code.ucProductGroup, pProduct->sEAN_Code.ucReleaseH, pProduct->sEAN_Code.ucReleaseL, pProduct->sEAN_Code.GetEAN());
               pApp->StandardPrint(0, szTextOut, 0, 0, dlg.m_nLingeLength);
               pApp->StandardPrint(2, pProduct->szDescription, 0, 0, dlg.m_nLingeLength);
               int i, nPrices = pProduct->GetNoOfPrices();
               int nLines = 1;
               for (i=0; i<nPrices; i++)
               {
                  ProductOrPriceGroup *pPOP = pDoc->GetPriceGroup(pProduct->GetPriceGroup(i));
                  if (pPOP)
                  {
                     pApp->StandardPrint(0, "", 0, nLines, dlg.m_nLingeLength);
                     if (nLines == 0) nLines = 1;
                     pApp->StandardPrint(5, pPOP->strDescription, 0, 0, dlg.m_nLingeLength);
                     sprintf(szTextOut, ": %.3f", pProduct->GetPrice((BYTE)pPOP->nNo));
                     pApp->StandardPrint(1, szTextOut, 0, 0, dlg.m_nLingeLength);
                     pApp->StandardPrint(2, pPOP->strCurrency, 0, 0, dlg.m_nLingeLength);
                     if (i==nPrices-1) pApp->StandardPrint(0, "", 0, 2, dlg.m_nLingeLength);
                  }
                  else
                  {
                     sprintf(szTextOut, ": %.3f DM", pProduct->GetPrice(0));
                     pApp->StandardPrint(1, szTextOut, 0, 1, dlg.m_nLingeLength);
                     nLines = 0;
                  }
               }
            }
         }
        
         pApp->StandardPrint(1, "\f", 0, 1, dlg.m_nLingeLength);
         pApp->CloseStandardPrinter();
      }
   }
}

void CFacturaView::OnExtraSendMail() 
{
   CSendEmail dlg;
   dlg.DoModal();
}
