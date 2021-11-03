// CARADoc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CaraView.h"
#include "CARADoc.h"
#include "resource.h"
#include "CARAPREV.h"
#include "DibSection.h"
#include "TreeDlg.h"
#include "BezierLabel.h"
#include "CircleLabel.h"
#include "ColorLabel.h"
#include "CurveLabel.h"
#include "LineLabel.h"
#include "PictureLabel.h"
#include "PlotLabel.h"
#include "RectLabel.h"
#include "TextLabel.h"
#include "CEtsMapi.h"


#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaraSingleDocTemplate

/////////////////////////////////////////////////////////////////////////////
// CCARADoc
#ifdef ETS_OLE_SERVER
IMPLEMENT_DYNCREATE(CCARADoc, COleServerDoc)
#else
IMPLEMENT_DYNCREATE(CCARADoc, CDocument)
#endif

CCARADoc::CCARADoc()
{
   BEGIN("CCARADoc");
   m_Container.SetDeleteContent(true);
   m_nCurrentFHPos   = 0;
   m_pCaraLogo       = NULL;
   m_pDocPrtParam    = NULL;
   m_nDocPrtParam    = 0;
   m_bIsTempFile     = false;
   m_bDeleteTempFile = true;
   m_pLsmb = NULL;
}

void CCARADoc::CreateCaraLogo()
{
   if (m_pCaraLogo) return;   // nicht mehrmals erzeugen
   m_pCaraLogo     = new CPictureLabel;
   if (m_pCaraLogo)
   {
      CDibSection *pDibSec = new CDibSection;
      if (pDibSec)
      {
         if (pDibSec->LoadFromResource(g_hCaraPreviewInstance, IDB_CARALOGO))
         {
            m_pCaraLogo->AttachDIB(pDibSec, true);
            m_pCaraLogo->SetScale(SCALE_ARBITRARY);
            m_pCaraLogo->SetRopMode(SRCAND);
            ASSERT_VALID(m_pCaraLogo);
            m_pCaraLogo->SetVisible(false);
         }
      }
   }
}

CCARADoc::~CCARADoc()
{
   BEGIN("~CCARADoc");
   if (m_pCaraLogo)
   {
      ASSERT_VALID(m_pCaraLogo);
      CDibSection *pDibSec  = m_pCaraLogo->DetachDIB();
      if (pDibSec) delete pDibSec;
      delete m_pCaraLogo;
   }
   if (m_pDocPrtParam) delete[] m_pDocPrtParam;
   DestroyLSMB();
   m_Lsmb.Destructor();
}

#ifdef ETS_OLE_SERVER
BEGIN_MESSAGE_MAP(CCARADoc, COleServerDoc)
#else
BEGIN_MESSAGE_MAP(CCARADoc, CDocument)
#endif
	//{{AFX_MSG_MAP(CCARADoc)
	ON_COMMAND(IDX_SHOW_LABELSTRUCTURE, OnShowLabelstructure)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Diagnose CCARADoc

#ifdef _DEBUG
void CCARADoc::AssertValid() const
{
#ifdef ETS_OLE_SERVER
   COleServerDoc::AssertValid();
#else
   CDocument::AssertValid();
#endif
}

void CCARADoc::Dump(CDumpContext& dc) const
{
#ifdef ETS_OLE_SERVER
   COleServerDoc::Dump(dc);
#else
	CDocument::Dump(dc);
#endif
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Serialisierung CCARADoc 


/////////////////////////////////////////////////////////////////////////////
// Befehle CCARADoc 
bool CCARADoc::LabelsAreInPrintRect(CDC *pDC, CRect rcMax)
{
   CLabelContainer *plc = GetLabelContainer();
   if (plc == NULL) return false;

   CRect rcTest = plc->GetRect(pDC);
   rcTest.NormalizeRect();
   rcMax.NormalizeRect();
   if (!((rcTest.left   > rcMax.left  )&&
         (rcTest.right  < rcMax.right )&&
         (rcTest.top    > rcMax.top   )&&
         (rcTest.bottom < rcMax.bottom))) return false;
   return true;
}

void CCARADoc::SetTitle(LPCTSTR lpszTitle)
{
   if (lpszTitle)
   {
      if (m_bIsTempFile)                                       // ist es eine Temp-Datei
      {
         CString string;
         string.LoadString(AFX_IDS_UNTITLED);
#ifdef ETS_OLE_SERVER
         COleServerDoc::SetTitle(string);
#else
         CDocument::SetTitle(string);
#endif
      }
      else
      {
         
#ifdef ETS_OLE_SERVER
         COleServerDoc::SetTitle(lpszTitle);
#else
         CDocument::SetTitle(lpszTitle);
#endif
      }
   }
}

void CCARADoc::DestroyLSMB()
{
   if (m_pLsmb)
   {
      delete m_pLsmb;
      m_pLsmb = NULL;
   }
}

BOOL CCARADoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
   BEGIN("OnOpenDocument");
   ETSLSMB_CLSMB sLSMB;
   sLSMB.nLines    = 1;                                        // 1 Textzeile
   sLSMB.nLoadSave = ETSLSMB_LOAD;                             // Ladeanimation anzeigen
   DWORD dwAtr = ::GetFileAttributes(lpszPathName);
   if ((dwAtr != 0xffffffff) && (dwAtr & FILE_ATTRIBUTE_READONLY))
   {
      if (AfxMessageBox(IDS_OPEN_READ_ONLY_FILE, MB_YESNO|MB_ICONQUESTION) == IDNO)
         return false;
   }
   ::LoadString(g_hCaraPreviewInstance, IDS_OPEN_DOCUMENT, sLSMB.szText[0], 64);
   if (AfxGetMainWnd() != NULL)                                // ist das Hauptfenster vorhanden
   {
//      REPORT("LoadSave-Animation");                            
      DestroyLSMB();
      m_pLsmb = new CEtsLsmb(&sLSMB);                          // Fenster erzeugen und anzeigen
      // Die LoadSaveMsg-Box setzt den Focus auf das letzte Fenster zurück.
      // Ist das Hauptfenster noch nicht vorhanden, würde eine anderes Fenster
      // den Focus erhalten.
   }

#ifdef ETS_OLE_SERVER
   BOOL bResult = COleServerDoc::OnOpenDocument(lpszPathName);
#else
   BOOL bResult = CDocument::OnOpenDocument(lpszPathName);
#endif
   if (bResult)
   {
      CString strFileName = _T(lpszPathName);
      strFileName.MakeUpper();
      if (strFileName.Find(".TMP")!=-1)                        // soll es eine temporäre Datei sein
      {                                                        
#ifndef _DEBUG                                                 // im Debugfall nicht !!
         if (m_bDeleteTempFile)
            ::DeleteFile(strFileName);                         // Datei löschen
         REPORT("Temp-File %s", strFileName);
#endif
         m_bIsTempFile = true;
      }
      else  m_bIsTempFile = false;
      SetModifiedFlag(false);
   }
   else OnNewDocument();
   DestroyLSMB();
   return bResult;
}

BOOL CCARADoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
   ETSLSMB_CLSMB sLSMB;
   sLSMB.nLines    = 1;                                        // 1 Textzeile
   sLSMB.nLoadSave = ETSLSMB_SAVE;                             // Speicheranimation anzeigen
   ::LoadString(g_hCaraPreviewInstance, IDS_SAVE_DOCUMENT, sLSMB.szText[0], ETSLSMB_STRINGLENGTH);
   DestroyLSMB();
   m_pLsmb = new CEtsLsmb(&sLSMB);                             // Fenster erzeugen und anzeigen
   m_bIsTempFile = false;
#ifdef ETS_OLE_SERVER
   BOOL bResult = COleServerDoc::OnSaveDocument(lpszPathName);
#else
   BOOL bResult = CDocument::OnSaveDocument(lpszPathName);
#endif
   if (bResult) SetModifiedFlag(false);
   DestroyLSMB();                                              // Fenster zerstören
   return bResult;
}

void CCARADoc::ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault) 
{
   DestroyLSMB();
   if (e!= NULL)
   {
      TCHAR szError[256];
      UINT nID;
      e->GetErrorMessage(szError, 256, &nID);
      REPORT("%s \nNo: %d", szError, nID);
      if (e->IsKindOf(RUNTIME_CLASS(CArchiveException)))
	   {
         CString message;
		   switch (((CArchiveException*)e)->m_cause)
		   {
            case CRC_EXCEPTION:
               message.Format(IDS_CRC_EXCEPTIONERROR, lpszPathName);
               break;
            case VERSION_EXCEPTION:
               message.Format(IDS_VERSION_EXCEPTIONERROR, lpszPathName);
               break;
            default:
#ifdef ETS_OLE_SERVER
               COleServerDoc::ReportSaveLoadException(lpszPathName, e, bSaving, nIDPDefault);
#else
               CDocument::ReportSaveLoadException(lpszPathName, e, bSaving, nIDPDefault);
#endif
            return;
         }
         AfxMessageBox(message, IDOK|MB_ICONEXCLAMATION, nIDPDefault);
      }
   }
}

LPCTSTR CCARADoc::GetDocName()
{
   return LPCTSTR(m_strTitle);
}

void CCARADoc::Serialize(CArchive& ar) 
{
   BEGIN("Serialize");
	if (ar.IsStoring())
	{
      _tzset();
      time_t     Timer;
      time(&Timer);
      tm *pTm = localtime(&Timer);
      TCHAR year[8] = _T("2001");
      if (pTm)
      {
         int nYear  = pTm->tm_year + ((pTm->tm_year < 17) ? 2000 : 1900);
         wsprintf(year, _T("%4d"), nYear);
      }
      m_FileHeader.SetYear(year);
      m_FileHeader.SetVersion(g_nFileVersion);
      m_FileHeader.SetComment(NULL);

      CLabel::SetFileVersion(g_nFileVersion);
      WriteFileHeader(ar);                                     // Fileheader Platzhalter schreiben
      m_FileHeader.CalcChecksum();                             // Checksumme zurücksetzen
      if (m_FileHeader.IsType(LABEL_FILE))                     // Label
      {
         CLabel::SaveGDIObjects(ar);                           // alle GDIObjects speichern
         m_Container.Serialize(ar);                            // alle Label speichern
         WriteFileHeader(ar, false);                           // Checksumme schreiben
         WritePrinterData(ar);                                 // Druckerdaten schreiben
      }
	}
	else
	{
      if (ReadFileHeader(ar))
      { 
         if ((m_FileHeader.GetVersion() > 100) && m_FileHeader.IsType(LABEL_FILE)) // Label
         {
            unsigned long lChecksum = m_FileHeader.GetChecksum();
            m_FileHeader.CalcChecksum();
            CLabel::LoadGDIObjects(ar);                        // GDI-Objekte in den Zwischenspeicher laden
            m_Container.Serialize(ar);                         // LabelContainer
            CLabel::DeleteLOGArray();                          // GDI-Objekt-Zwischenspeicher löschen
            if (!m_FileHeader.IsValid(lChecksum))
            {
               AfxThrowArchiveException(CRC_EXCEPTION, NULL);
            }
            ReadFileHeader(ar);                                // neuen FileHeader lesen
            if (m_FileHeader.IsType(PRINTER_DATA))             // sind es Druckerdaten
            {
               ReadPrinterData(ar);                            // Druckerdaten lesen
               ReadFileHeader(ar);                             // neuen FileHeader lesen
            }
         }
      }
	}
}

void CCARADoc::WritePrinterData(CArchive &ar)
{
   DWORD nLength = 0;
   BYTE *pParam  = NULL;
   m_FileHeader.SetType(PRINTER_DATA);
   m_FileHeader.CalcChecksum();
   CCaraView *pView = GetCaraView();
   if (pView && (pView->m_nPages > 1))
   {
      TCHAR text[64];
      wsprintf(text, _T("Pages: %d"), pView->m_nPages);
      m_FileHeader.SetComment(text);
   }
   else m_FileHeader.SetComment(NULL);
   
   WriteFileHeader(ar);
   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   if (m_pDocPrtParam)                                         // Druckerdaten des Dokumentes speichern, wenn vorhanden
   {
      CLabel::ArchiveWrite(ar, m_pDocPrtParam, m_nDocPrtParam);
   }
   else if (pApp->m_pPrtD)
   {
      pParam = pApp->GetPrinterData(nLength);                  // Druckerdaten ermitteln
      CLabel::ArchiveWrite(ar, pParam, nLength);               // und speichern
      delete[] pParam;
   }
   WriteFileHeader(ar, false);
}

bool CCARADoc::ReadPrinterData(CArchive &ar)
{
   BEGIN("ReadPrinterData");
   bool          bReturn   = false;
   int           nLength   = m_FileHeader.GetLength();
   unsigned long lChecksum = m_FileHeader.GetChecksum();
   BYTE         *pParam    = NULL;
   CCaraWinApp  *pApp      = (CCaraWinApp*) AfxGetApp();
   m_FileHeader.CalcChecksum();
   CCaraView *pView = GetCaraView();
   if (pView)
   {
      const TCHAR *psz = _tcsstr(m_FileHeader.GetComment(), "Pages:");
      if (psz) pView->m_nPages = atoi(&psz[6]);
   }
   if (nLength != 0) pParam = new BYTE[nLength];               // Druckerdaten vorhanden ?
   if (pParam)
   {
      CLabel::ArchiveRead(ar, pParam, nLength, true);          // Druckerdaten lesen
      if (m_FileHeader.IsValid(lChecksum))                     // Druckerdaten Ok ?
      {
         if (pApp->IsAPrinterAvailabel())                      // Drucker auf dem Rechner vorhanden ?
         {
            bReturn = pApp->SetPrinterData(pParam, nLength);   // Drucker setzen
            if (bReturn)                                       // Drucker gesetzt ?
            {
               pApp->GetPageSettings();
            }
            else
            {
               m_pDocPrtParam = pParam;                        // Druckerdaten des Dokumentes speichern
               m_nDocPrtParam = nLength;
               SetDocPageParams();                             // Druckseitenparameter setzen
            }
         }
         else                                                  // kein Drucker vorhanden
         {
            m_pDocPrtParam = pParam;                           // Druckerdaten des Dokumentes speichern
            m_nDocPrtParam = nLength;
            SetDocPageParams();                                // Druckseitenparameter setzen
         }
      }
      delete[] pParam;                                         // Druckerdaten löschen
   }
   if (pApp->m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo)
   {
      ETSPRTD_PARAMETER   param;
      ETSPRTD_SPECIALDATA special;
      ZeroMemory(&param, sizeof(param));
      param.dwFlags      = ETSPRTD_SETALLDATA|ETSPRTD_LOWMETRICS;
      param.dwSize       = sizeof(special);
      param.pPrinterData = &special;

      if (pApp->m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo)
         lstrcpy((LPTSTR)special.szPrinterName, pApp->m_CmdInfo.m_strPrinterName);

      special.rcMargin.left   = pApp->m_Margins.rcDrawLoMetric.left;
      special.rcMargin.top    = pApp->m_Margins.rcDrawLoMetric.top;
      special.rcMargin.right  = pApp->m_Margins.rcPageLoMetric.right  - pApp->m_Margins.rcDrawLoMetric.right;
      special.rcMargin.bottom = pApp->m_Margins.rcPageLoMetric.bottom - pApp->m_Margins.rcDrawLoMetric.bottom;
      special.dmOrientation   = pApp->m_Margins.dmOrientation;
      special.dmPaperSize     = pApp->m_Margins.dmPaperSize;

      switch (CEtsPrtD::DoDataExchange(&param))
      {
         case 3:                       // Drucker, Ränder, Papier und Orientation  mußten geändert werden
            pApp->m_Margins.rcOldDrawLoMetric = pApp->m_Margins.rcDrawLoMetric;
            pApp->GetPageSettings();
            pApp->CheckPageScaling();
         case 0:                       // Ok
         case 2:                       // Drucker wurde nicht gefunden nur durch ETSPRTD_SETALLDATA aber
         bReturn = true;               // alle Ränder und Papiereinstellungen konnten übernommen werden
         case 1: case -1: default: break;
      }
   }
   return bReturn;
}

bool CCARADoc::ReadFileHeader(CArchive &ar)
{
   BEGIN("ReadFileHeader");
   if ((ar.Read(&m_FileHeader, sizeof(CFileHeader)) == sizeof(CFileHeader)) && m_FileHeader.IsFileHeader())
   {
      CheckVersion();
      return true;
   }
   else
   {
      ar.Flush();    // File zurücksetzen
      ar.GetFile()->SeekToBegin();
      m_FileHeader.SetType("   ");
      return false;
   }
}

void CCARADoc::DeleteDocPrtParam()
{
   if (m_pDocPrtParam)
   {
      delete[] m_pDocPrtParam;                                // Druckerdaten des alten Dokumentes löschen, wenn vorhanden
      m_pDocPrtParam = NULL;
      m_nDocPrtParam = 0;
   }
}

void CCARADoc::WriteFileHeader(CArchive& ar, bool bCurrent)
{
   int nOldPos = -1;
   if (bCurrent)                                               // An die aktuelle Position schreiben
   {
      ar.Flush();
      m_nCurrentFHPos = (long)ar.GetFile()->GetPosition();           // Fileheaderposition merken
   }
   else
   {
      ar.Flush();
      nOldPos = (long)ar.GetFile()->GetPosition();                   // alte Position merken
      ar.GetFile()->Seek(m_nCurrentFHPos, CFile::begin);       // zur letzten Fileheaderposition springen
   }
   ar.Write(&m_FileHeader, sizeof(CFileHeader));               // FileHeader schreiben
   if (nOldPos != -1)
   {
      ar.Flush();
      ar.GetFile()->Seek(nOldPos, CFile::begin);               // zur alten Position zurückspringen
   }
}

void CCARADoc::CheckVersion()
{
   int nVersion = m_FileHeader.GetVersion();
   if (nVersion > g_nFileVersion) AfxThrowArchiveException(VERSION_EXCEPTION, NULL);
   CLabel::SetFileVersion(nVersion);
}

void CCARADoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
   if (m_bIsTempFile) bAddToMRU = false;                       // Tempfiles nicht in MRU einfügen
   SetReadOnly(false);                                         // altes Flag löschen

#ifdef ETS_OLE_SERVER
   COleServerDoc::SetPathName(lpszPathName, bAddToMRU);
#else
	CDocument::SetPathName(lpszPathName, bAddToMRU);
#endif
   if (m_bIsTempFile) m_strPathName.Empty();
   else
   {
      SetReadOnly(true);                                       // neues Flag setzen
   }
}

void CCARADoc::SetDocPageParams()
{
   if ((m_pDocPrtParam                            != NULL) &&  // pointer
       (strncmp((TCHAR*)m_pDocPrtParam, "PRTD", 4) ==    0) &&  // ID
       (((DWORD*)m_pDocPrtParam)[1]               ==  100))    // version
   {
      DWORD nOffset = sizeof(DWORD)*2;
      nOffset += _tcsclen((TCHAR*)&((DWORD*)m_pDocPrtParam)[2])+1;
      DEVMODE* pDevMode = (DEVMODE*)&m_pDocPrtParam[nOffset];
      nOffset += pDevMode->dmSize + pDevMode->dmDriverExtra;
      PRINTERPARAMETER *pPP  = (PRINTERPARAMETER*)&m_pDocPrtParam[nOffset];
      CCaraWinApp      *pApp = (CCaraWinApp*)AfxGetApp();
      pApp->m_Margins.OffsetLoMetric.x      = CEtsDiv::Round(pPP->ptOffset.x * pPP->dScaleX2Lom);
      pApp->m_Margins.OffsetLoMetric.y      = CEtsDiv::Round(pPP->ptOffset.y * pPP->dScaleY2Lom);
      pApp->m_Margins.rcPageLoMetric.left   = 0;
      pApp->m_Margins.rcPageLoMetric.top    = 0;
      pApp->m_Margins.rcPageLoMetric.right  =  CEtsDiv::Round(pPP->szPage.cx * pPP->dScaleX2Lom);
      pApp->m_Margins.rcPageLoMetric.bottom = -CEtsDiv::Round(pPP->szPage.cy * pPP->dScaleY2Lom);
      pApp->m_Margins.rcDrawLoMetric.left   =  pPP->rcMargin.left;
      pApp->m_Margins.rcDrawLoMetric.top    = -pPP->rcMargin.top;
      pApp->m_Margins.rcDrawLoMetric.right  = pApp->m_Margins.rcPageLoMetric.right  - pPP->rcMargin.right; 
      pApp->m_Margins.rcDrawLoMetric.bottom = pApp->m_Margins.rcPageLoMetric.bottom + pPP->rcMargin.bottom;
      pApp->m_Margins.dmOrientation = pDevMode->dmOrientation;
      pApp->m_Margins.dmPaperSize   = pDevMode->dmPaperSize;
   }
}

BOOL CCARADoc::OnNewDocument() 
{
   CCaraWinApp*pApp = (CCaraWinApp*)AfxGetApp();
   if (pApp->IsAPrinterAvailabel())
   {
      pApp->m_CmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
      pApp->LoadPrinterSettings();
   }
   if (!m_strPathName.IsEmpty())
   {
      m_strPathName.Empty();
      CString strDocName;
		VERIFY(strDocName.LoadString(AFX_IDS_UNTITLED));
      SetTitle(strDocName);
   }
#ifdef ETS_OLE_SERVER
   return COleServerDoc::OnNewDocument();
#else
	return CDocument::OnNewDocument();
#endif
}

void CCARADoc::DeleteContents() 
{
   BEGIN("CCARADoc::DeleteContents");
   SetReadOnly(false);
   CCaraWinApp*pApp = (CCaraWinApp*)AfxGetApp();
   pApp->DeleteUndoList();
   CLabel::DeleteGraficObjects();
   CLabel::SetShapePen();
   CCaraView *pView = GetCaraView();
   if (pView)
   {
      pView->m_nCurrentPage = -1;
      pView->m_nPages       = 1;
   }
#ifdef ETS_OLE_SERVER
   COleServerDoc::DeleteContents();
#else
	CDocument::DeleteContents();
#endif
}

void CCARADoc::ShowLabelStructure()
{
   CTreeDlg dlg;
   dlg.m_pCallBack = CCARADoc::FillLabelStructure;
   dlg.m_pParam    = GetLabelContainer();
   dlg.m_ImageList.m_hImageList = ImageList_LoadImage(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDB_TREE_IMAGES),
      15, 1, CLR_NONE, IMAGE_BITMAP, LR_DEFAULTCOLOR|LR_LOADTRANSPARENT);
   
   dlg.DoModal();
}

int CCARADoc::ShowLabelStructure(CLabel *pl, void *p)
{
   ASSERT(pl != NULL);
   ASSERT(p  != NULL);
   bool          bContainer = false;
   ProcessLabel *prl = (ProcessLabel*)p;
   CTreeCtrl    *ptc = (CTreeCtrl*)prl->pParam1;
   CPtrList     *pL  = (CPtrList*)prl->pParam2;
   CString       str;
   int *pnItems  = (int*)pL->GetHead();

   if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
   {
      str.Format("CLabelContainer");
      bContainer = true;
   }
   else if (pl->IsKindOf(RUNTIME_CLASS(CBezierLabel ))) str.Format("CBezierLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CCircleLabel ))) str.Format("CCircleLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CColorLabel  ))) str.Format("CColorLabel"); 
   else if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel  ))) str.Format("CCurveLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CLineLabel   ))) str.Format("CLineLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CPictureLabel))) str.Format("CPictureLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel   ))) str.Format("CPlotLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CRectLabel   ))) str.Format("CRectLabel");
   else if (pl->IsKindOf(RUNTIME_CLASS(CTextLabel   ))) str.Format("CTextLabel");

   HTREEITEM hParent = ptc->InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE, str, pl->IsVisible() ? 0 : 1, 0, 0, 0, (LPARAM)pl, (HTREEITEM)pnItems[0], TVI_LAST);

   if (bContainer)
   {
      int *pnNewItems = new int[3];
      if (pnNewItems)
      {
         pnNewItems[0]   = (int)hParent;
         pnNewItems[1]   = 0;
         pnNewItems[2]   = ((CLabelContainer*)pl)->GetCount();
         pL->AddHead(pnNewItems);
      }
   }
   else if (++pnItems[1] == pnItems[2])
   {
      if (pL->GetCount() > 1)
      {
         pnItems = (int*) pL->RemoveHead();
         delete[] pnItems;
      }
   }
   return 0;
}

int CCARADoc::FillLabelStructure(CTreeDlg *pDlg, int nReason)
{
   switch (nReason)
   {
      case ONINITDIALOG:
      {
         ASSERT(pDlg           != NULL);
         ASSERT(pDlg->m_pParam != NULL);
         CLabelContainer *plc = (CLabelContainer*)pDlg->m_pParam;
         ProcessLabel prl;
         CPtrList List;
         int nItems[3] = {(int)TVI_ROOT, 0, plc->GetCount()};

         List.AddHead(&nItems);
         prl.nPos    = 0;
         prl.pParam1 = &pDlg->m_TreeCtrl;
         prl.pParam2 = &List;

         plc->ProcessWithLabels(NULL, &prl, CCARADoc::ShowLabelStructure);
         return 1;
      }
      case ONBEGINDRAGLSTLIST:
         return 1;
   }
   return 0;
}

void CCARADoc::OnShowLabelstructure() 
{
   ShowLabelStructure();
}

void CCARADoc::OnFileSendMail() 
{
   if (IsModified())
   {
#ifdef ETS_OLE_SERVER
      COleServerDoc::OnFileSave();
#else
      CDocument::OnFileSave();
#endif
   }
   else if (m_bIsTempFile)
   {
      OnFileSaveAs();
   }
   if (!IsModified())
      CEtsMapi::Send(AfxGetMainWnd()->m_hWnd, m_strPathName);
}

void CCARADoc::OnUpdateFileSendMail(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(((CCaraWinApp*)AfxGetApp())->m_bMailAvailable && GetLabelContainer()->GetCount());
}

void CCARADoc::StrReplace(TCHAR *pszDest, TCHAR *pszRepl, TCHAR *pszInsert)
{
   TCHAR *pstr = _tcsstr(pszDest, pszRepl); // Theta
   if (pstr)
   {
      int nLen       = _tcsclen(pstr);
      int nReplLen   = _tcsclen(pszRepl);
      int nInsertLen = _tcsclen(pszInsert);
      memmove(&pstr[nInsertLen-nReplLen], pstr, nLen);
      memcpy(pstr, pszInsert, nInsertLen);
      pstr[nLen+nInsertLen-nReplLen] = 0;
   }
}

void CCARADoc::DosToWindowText(TCHAR *text)
{
   StrReplace(text, "å", "Theta");
   StrReplace(text, "ä", "Phi");
   ::OemToChar(text, text);
}

CCaraView* CCARADoc::GetCaraView()
{
   POSITION pos = GetFirstViewPosition();
   while (pos)
   {
      CView *pView = GetNextView(pos);
      if (pView->IsKindOf(RUNTIME_CLASS(CCaraView)))
         return (CCaraView*)pView;
   }
   return NULL;
}

#ifdef ETS_OLE_SERVER
COleServerItem* CCARADoc::OnGetEmbeddedItem()
{
	return NULL;
}
#endif


BOOL CCARADoc::SaveModified() 
{
   SetReadOnly(false);
#ifdef ETS_OLE_SERVER
   return COleServerDoc::SaveModified();
#else
	return CDocument::SaveModified();
#endif
}

void CCARADoc::SetReadOnly(bool bReadOnly)
{
/*
   CCaraWinApp*pApp = (CCaraWinApp*)AfxGetApp();
   if (!pApp->m_CmdInfo.m_bRunAutomated && !pApp->m_CmdInfo.m_bRunEmbedded)
   {
      if (!m_strPathName.IsEmpty())
      {
         DWORD dwAtr = ::GetFileAttributes(m_strPathName);
         if (dwAtr != -1)
         {
            if (bReadOnly) dwAtr |= FILE_ATTRIBUTE_READONLY;
            else           dwAtr &= ~FILE_ATTRIBUTE_READONLY;
            ::SetFileAttributes(m_strPathName, dwAtr);
         }
      }
   }
*/
}

void CCARADoc::OnFileSave() 
{
   SetReadOnly(false);                                         // Schreibschutz löschen
#ifdef ETS_OLE_SERVER
   COleServerDoc::OnFileSave();
#else
   CDocument::OnFileSave();
#endif
}

DWORD CArchiveEx::GetType()
{
   DWORD nType = TYPE_UNIX;
	if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
		FillBuffer(UINT(sizeof(BYTE) - (m_lpBufMax - m_lpBufCur)));
   LPTSTR pTemp = (LPTSTR)m_lpBufCur;
   for (; pTemp < (LPTSTR)m_lpBufMax; pTemp++)
   {
      if (*pTemp == '\r')
      {
         if (pTemp[1] == '\n')
         {
            nType = TYPE_DOS;
            break;
         }
         else
         {
            nType = TYPE_MAC;
            break;
         }
      }
      else if (*pTemp == '\n')
      {
         nType = TYPE_UNIX;
         break;
      }
   }
   return nType;
}

int CArchiveEx::ReadString(DWORD &nType, CString &sRead)
{
   CString s;
   _TCHAR ch;
   _TCHAR cTest[4] = "\n\n\r";
   BOOL bRead = TRUE;
   int i = 0, nLen = 128;
   LPTSTR pBuff = (LPTSTR) s.GetBufferSetLength(nLen);

	TRY
	{
      if (nType == -1)
      {
         nType = GetType();
      }
      do
      {
         *this >> ch;
         if (ch == cTest[nType])
         {
            pBuff[i] = 0;
            if (nType == TYPE_DOS && i > 0 && pBuff[i-1] == '\r')
            {
               pBuff[i-1] = 0;
            }
            break;
         }
         pBuff[i++] = ch;
         if (i == nLen)
         {
            nLen += 128;
            s.ReleaseBuffer();
            pBuff = (LPTSTR) s.GetBufferSetLength(nLen);
         }
      }while (bRead);

      s.ReleaseBuffer();
      sRead = s;
   }
	CATCH_ALL(e)
	{
      i = EOF;
	}
	END_CATCH_ALL

   return i;
}

int CArchiveEx::ReadString(DWORD &nType, TCHAR*psRead, long nLen)
{
   _TCHAR ch;
   _TCHAR cTest[4] = "\n\n\r";
   BOOL bRead = TRUE;
   int i = 0;

   TRY
	{
      if (nType == -1)
      {
         nType = GetType();
      }
      do
      {
         *this >> ch;
         if (ch == cTest[nType])
         {
            psRead[i] = 0;
            if (nType == TYPE_DOS && i > 0 && psRead[i-1] == '\r')
            {
               psRead[i-1] = 0;
            }
            break;
         }
         if (i < nLen)
         {
            psRead[i++] = ch;
         }
         else
         {
            psRead[i] = 0;
            break;
         }
      }while (bRead);
   }
	CATCH_ALL(e)
	{
      i = EOF;
	}
	END_CATCH_ALL

   return i;
}