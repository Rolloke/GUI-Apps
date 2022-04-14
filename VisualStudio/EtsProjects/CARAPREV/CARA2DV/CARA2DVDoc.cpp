// CARA2DVDoc.cpp : Implementierung der Klasse CCARA2DVDoc
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CARA2DVDoc.h"
#include "CARA2DVView.h"
#include "MainFrm.h"
#include "..\CurveLabel.h"
#include "..\PlotLabel.h"
#include "..\CaraMenu.h"
#include "..\resource.h"
#include "..\TextEdit.h"
#include "SaveCurveAsWave.h"
#include "CurveFileDlg.h"

#ifdef ETS_OLE_SERVER
#include "SrvrItem.h"
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"
#include ".\cara2dvdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define NO_CHECKSUM 0x0815

#define CARA2D_FILE     "2DV"
#define PLOT_FILE       "PLT"
#define EDITPLOT_FILE   "EDT"
#define CURVE_FILE      "CRV"
#define UPDATE_FILE     "UPT"

extern "C" __declspec(dllimport) int g_nFileVersion;

/////////////////////////////////////////////////////////////////////////////
// CInputSocket

struct CurveData
{
    enum eID 
    {
        Identifier = 0x000c0b0a, IdMask = 0x00ffffff, IdShift=24,
        IdSimpleCurve=1, IdExtendedCurve=2, IdCurveTypeMask=0xf,
        IdNoUpdate=0x10
    };
    long identifier;
    short name_length;
    short curve_format;
    long  no_of_data;
    int GetID(bool &bBigEndian)
    {
        if ((identifier&IdMask) == Identifier)
        {
            return identifier >> IdShift;  // 
        }
        if (!bBigEndian)
        {
            bBigEndian = true;
            identifier   = htonl(identifier);
            curve_format = htons(curve_format);
            name_length  = htons(name_length);
            no_of_data   = htonl(no_of_data);
            return GetID(bBigEndian);
        }
        return 0;
    }
};

void endianconvert(BYTE *buf, int size)
{
    BYTE temp;
    int i, j, n = size / 2;
    for (i=0, j=size-1; i<n; i++, j--)
    {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
}

IMPLEMENT_DYNAMIC(CInputSocket, CSocket)

CInputSocket::CInputSocket(CCARA2DVDoc* pDoc) : m_pDoc(pDoc), m_State(eDefault), m_CurveFormat(CF_SINGLE_FLOAT)
{
}

CInputSocket::~CInputSocket()
{
}

void CInputSocket::OnAccept(int nError)
{
   TRACE("OnAccept(%d)\n", nError);

   SOCKADDR addr;
   int nlen = sizeof(addr);
   CSocket soc;
   Accept(soc, &addr, &nlen);
   Close();
   Attach(soc.Detach());
   //char buff=0;
   //Send(&buff, 1);
}

void CInputSocket::endianconvert(BYTE *buf, int size)
{
    BYTE temp;
    int i, j, n = size / 2;
    for (i=0, j=size-1; i<n; i++, j--)
    {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
}

void CInputSocket::OnReceive(int nError)
{
    CString name;
    CurveData cd;
    int received;
    int nID;
    bool bigendian = false;
    bool update    = true;
    do
    {   // receive identifying structure
        received = Receive(&cd, sizeof(CurveData));
        if (received == sizeof(CurveData))
        {
            nID = cd.GetID(bigendian);  // identify valid data
            if (nID)
            {
                CCurveLabel *pcurve = 0;
                int i, size, n = 0, factor = 1;
                BYTE array[sizeof(double)*2];   // maximum possible size
                if (nID & CurveData::IdNoUpdate) update = false;
                nID &= CurveData::IdCurveTypeMask;// remove flags

                switch (cd.curve_format)        // set parameters of curve format
                {
                    case CF_DOUBLE:        size = sizeof(double); factor = 2; break;
                    case CF_SINGLE_DOUBLE: size = sizeof(double); break;
                    case CF_FLOAT:         size = sizeof(float);  factor = 2; break;
                    case CF_SINGLE_FLOAT:  size = sizeof(float);  break;
                    case CF_SHORT:         size = sizeof(short);  factor = 2; break;
                    case CF_SINGLE_SHORT:  size = sizeof(short);  break;
                }

                Receive(name.GetBufferSetLength(cd.name_length), cd.name_length);
                name.ReleaseBuffer();   // received name of curve

                pcurve = new CCurveLabel;
                pcurve->SetText(name);

                // set parameters
                pcurve->SetFormat(cd.curve_format);
                pcurve->SetSize(cd.no_of_data);
                n = pcurve->GetArraySize();

                BYTE*buf = (BYTE*)pcurve->GetArrayPointer();
                i = 0;
                do
                {   // receive all values
                    i += Receive((void*)&buf[i], n-i);
                }while (i < n);

                if (nID == CurveData::IdExtendedCurve)
                {
                    // receive start and stop range
                    Receive(&array[0], size*2);
                }

                if (bigendian)  // convert byte order
                {
                    int n = cd.no_of_data*factor;
                    buf = (BYTE*)pcurve->GetArrayPointer();
                    for (i=0; i<n; i++)
                    {   // convert each single value
                        endianconvert(buf, size);
                        buf += size;
                    }
                    if (nID == CurveData::IdExtendedCurve)
                    {   // convert start and stop range
                        buf = &array[0];
                        endianconvert(buf, size);
                        buf += size;
                        endianconvert(buf, size);
                    }
                }

                if (nID == CurveData::IdExtendedCurve)
                {   // set the range of a single curve
                    buf = &array[0];
                    switch (cd.curve_format)
                    {
                        case CF_SINGLE_DOUBLE: 
                            pcurve->SetX_Value(0, *(double*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(double*)(buf+size));
                            break;
                        case CF_SINGLE_FLOAT:
                            pcurve->SetX_Value(0, *(float*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(float*)(buf+size));
                            break;
                        case CF_SINGLE_SHORT:
                            pcurve->SetX_Value(0, *(short*)buf);
                            pcurve->SetX_Value(cd.no_of_data-1, *(short*)(buf+size));
                            break;
                    }
                }
                theApp.m_pMainWnd->PostMessage(WM_NEWCURVE, (WPARAM) pcurve, 1);
                pcurve = 0;
            }
        }
    }
    while (received != sizeof(CurveData));
}

/*
{
    static CMapStringToPtr map;
    if (map.GetCount() == 0)
    {
        map.SetAt("title", (void*)eTitle);
        map.SetAt("title_x", (void*)eUnitX);
        map.SetAt("title_y", (void*)eUnitY);
        map.SetAt("color", (void*)eColor);
        map.SetAt("red", (void*)eRed);
        map.SetAt("green", (void*)eGreen);
        map.SetAt("blue", (void*)eBlue);
        map.SetAt("yellow", (void*)eYellow);
        map.SetAt("magenta", (void*)eMagenta);
        map.SetAt("cyan", (void*)eCyan);
        map.SetAt("black", (void*)eBlack);
        map.SetAt("floatvalues", (void*)eFloatValues);
        map.SetAt("NEXT", (void*)eNext);
    }
    CCurveLabel *pC = NULL;
    CString sReceive;
    int maxlength = 1024;
    int rcv = 0;
    if (m_State == eExpectingValues)
    {
        CLabel*pL = m_Container.GetLabel(0);
        if (pL->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
        {
            pC = (CCurveLabel*)pL;
        }
        BYTE*dest = (BYTE*)pC->GetArrayPointer();
        Receive(dest, pC->GetNumValues() * sizeof(float));
        m_State = eDefault;
    }
    else
    {
        rcv = Receive(sReceive.GetBufferSetLength(maxlength), maxlength);
    }
    if (rcv)
    {
        sReceive.ReleaseBufferSetLength(rcv);
        int nFind = sReceive.Find("=");
        int nValue = -1;
        if (m_Container.GetCount() == 0)
        {
            pC = new CCurveLabel();
            //pC->SetTargetFuncNo(-1);
            m_Container.InsertLabel(pC);
        }
        else
        {
            CLabel*pL = m_Container.GetLabel(0);
            if (pL->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
                pC = (CCurveLabel*)pL;
            }
        }
        if (pC)
        {
            if (nFind != -1) 
            {
                if (map.Lookup(LPCTSTR(sReceive.Left(nFind)), (void*&)nValue))
                {
                switch(nValue)
                {
                    case eTitle: pC->SetText(sReceive.Mid(nFind+1)); break;
                    case eUnitX: pC->SetXUnit(sReceive.Mid(nFind+1)); break;
                    case eUnitY: pC->SetYUnit(sReceive.Mid(nFind+1)); break;
                    case eFloatValues:
                    {
                        int n, len = strlen(LPCTSTR(sReceive));
                        nFind++;
                        n = atoi(LPCTSTR(sReceive.Mid(nFind, len-nFind)));
                        if (n > 0 && n < 1000000)
                        {
                            pC->SetFormat(CF_SINGLE_FLOAT);
                            pC->SetSize(n);
                            m_State = eExpectingValues;
                        }
                    }  break;
                    case eColor:
                        if (map.Lookup(LPCTSTR(sReceive.Mid(nFind+1)), (void*&)nValue))
                        {
                            switch(nValue)
                            {
                                case eRed: pC->SetColor(RGB(255,0,0)); break;
                                case eGreen: pC->SetColor(RGB(0,255,0)); break;
                                case eBlue: pC->SetColor(RGB(0,0,255)); break;
                                case eYellow: pC->SetColor(RGB(0,255,255)); break;
                                case eMagenta: pC->SetColor(RGB(255,0,255)); break;
                                case eCyan: pC->SetColor(RGB(0,0,0)); break;
                                case eBlack: pC->SetColor(RGB(0,0,0)); break;
                                default: break;
                            }
                        }
                    case eNext: 
                        m_pDoc->SendAndRemoveCurves((CMainFrame*) AfxGetApp()->m_pMainWnd, &m_Container);
                        break;
                    default:
                        TRACE("\nUnknown command");
                }
                }
            }
            else
            {
                double dX, dY;
                int i = pC->GetCount();
                int n = sscanf(sReceive, LPCTSTR("%f, %f"), &dX, &dY);
                if (n == 1)
                {
                    pC->SetSize(i+1);
                    pC->SetY_Value(i, dX);
                }
                else
                {
                    pC->SetSize(i+1);
                    pC->SetX_Value(i, dX);
                    pC->SetY_Value(i, dY);
                }
            }
        }
    }

    TRACE("OnReceive(%d):%s, %d\n", nError, LPCTSTR(sReceive), rcv);
    char buff = 1;
    if (m_State = eExpectingValues)
    {
        buff = 2;
    }
    Send(&buff, 1);
}
*/
void CInputSocket::OnClose(int nError)
{
   if (m_Container.GetCount())
   {
      m_pDoc->SendAndRemoveCurves((CMainFrame*) AfxGetApp()->m_pMainWnd, &m_Container);
   }
   Close();

   TRACE("OnClose(%d)\n", nError);
}
#ifdef _DEBUG
void CInputSocket::AssertValid() const
{
	CSocket::AssertValid();
}

void CInputSocket::Dump(CDumpContext& dc) const
{
	CSocket::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CCARA2DVDoc
IMPLEMENT_DYNCREATE(CCARA2DVDoc, CCARADoc)

BEGIN_MESSAGE_MAP(CCARA2DVDoc, CCARADoc)
	//{{AFX_MSG_MAP(CCARA2DVDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_DIRECT, OnUpdateFilePrintDirect)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FUNC_MAKROEDIT, OnFuncMakroedit)
	ON_COMMAND(ID_FUNC_SAVE_CURVE, OnFuncSaveCurve)
	ON_COMMAND(ID_FUNC_LOAD_CURVE, OnFuncLoadCurve)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_FUNC_LOAD_CURVE, OnUpdateFuncLoadCurve)
	ON_COMMAND(IDX_NOCECKSUM, OnNocecksum)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
    ON_COMMAND(ID_FILE_LOAD_READ_MACROS, OnFileLoadReadMacros)
    ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_READ_MACROS, OnUpdateFileLoadReadMacros)
    ON_COMMAND(ID_FILE_SAVE_READ_MACROS, OnFileSaveReadMacros)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_READ_MACROS, pp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVDoc Konstruktion/Destruktion

CCARA2DVDoc::CCARA2DVDoc()
{
   m_Container.SetDeleteContent(true);
   m_pPlotMenu         = NULL;
   m_pFile             = NULL;
   m_nPlotRects        = 0;
   m_pPlotRects        = NULL;
   m_nInvalidStates    = 0;
   m_bLoadDocument     = false;
   m_pInputSocket      = NULL;
   ((CCARA2DVApp*)AfxGetApp())->LoadDefaultSettings();
#ifdef ETS_OLE_SERVER
   m_bOleChanged       = false;
#endif
}

CCARA2DVDoc::~CCARA2DVDoc()
{
   SAFE_DELETE(m_pPlotMenu); 
   SAFE_DELETE(m_pInputSocket);
   m_LsmbDoc.Destructor();
}

BOOL CCARA2DVDoc::OnNewDocument()
{

#ifdef ETS_OLE_SERVER
   m_bOleChanged = false;
#endif

   ((CCARA2DVApp*)AfxGetApp())->LoadDefaultSettings();
   BOOL bResult = CCARADoc::OnNewDocument();

   if (m_pInputSocket == NULL)
   {
      m_pInputSocket = new CInputSocket(this);
   }
   UINT Port = 4711;
   if (m_pInputSocket->Create(Port))
   {
      m_pInputSocket->AsyncSelect(FD_READ|FD_ACCEPT|FD_CLOSE);
      if (m_pInputSocket->Listen())
      {
         TRACE("Listening on Port %d\n", Port);
      }
   }

   return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVDoc Serialisierung
void CCARA2DVDoc::Serialize(CArchive& ar)
{
   BEGIN("CCARA2DVDoc::Serialize");
   CCARA2DVApp *pApp = (CCARA2DVApp*)AfxGetApp();
//   ASSERT(false);
   if (ar.IsStoring())
   {
      CLabel::SetFileVersion(g_nFileVersion);
      REPORT("Storing");
      if (pApp->m_CallingWindowwParam == TARGET_FUNCTION_CHANGEABLE)
      {
         REPORT("Targetfunction");
         int i, nCount = m_Container.GetCount();               // Schnittstelle zu CARACALC
         if (pApp->m_bEditMode) m_FileHeader.SetType(EDITPLOT_FILE);// Filetyp EDT für die Zielfunktionen
         else                   m_FileHeader.SetType(PLOT_FILE);// Filetyp PLT für nicht editierbare Plots
         CPlotLabel *ppl;
         CCARADoc::Serialize(ar);
         m_FileHeader.CalcChecksum();                          // Checksumme initialisieren
         CLabel::ArchiveWrite(ar, &nCount, sizeof(int));
         ar.Flush();
         for (i=0; i<nCount; i++)
         {
            CLabel * pl = m_Container.GetLabel(i);
            if (pl && pl->IsKindOf(RUNTIME_CLASS(CPlotLabel))) // nur die Plots übergeben
            {
               ppl = (CPlotLabel*)pl;
               ppl->Write((HANDLE)ar.GetFile()->m_hFile);
               ppl->GetChecksum(&m_FileHeader);
            }
         }
         WriteFileHeader(ar, false);
      }
      else
      {
         REPORT("CaraViewLabel");
         m_FileHeader.SetType(LABEL_FILE);                     // Labeldateityp setzen
         RestorePlotRects(NULL, CRect(0,0,0,0));
         CCARADoc::Serialize(ar);                              // Label schreiben
         SavePlotRects();
         m_FileHeader.SetType(CARA2D_FILE);                    // 2DViewDatei
         m_FileHeader.CalcChecksum();
         WriteFileHeader(ar);
         CLabel::ArchiveWrite(ar, &m_nPlotRects, sizeof(int)); // Anzahl der Druckrechtecke speichern
         if ((m_nPlotRects>0) && (m_pPlotRects))               // die Druckrechtecke, wenn vorhanden, speichern
            CLabel::ArchiveWrite(ar, m_pPlotRects, sizeof(CRect)*m_nPlotRects);
         WriteFileHeader(ar, false);                           // Checksumme schreiben
                                                               // Plot-Größen wiederherstellen
         CView *pView = ((CMainFrame*)pApp->m_pMainWnd)->GetActiveView();
         if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA2DVView)))
            ((CCARA2DVView*)pView)->ResizePlotsToView();
      }
   }
   else
   {
      REPORT("Loading");
      CCARADoc::Serialize(ar);                                 // Label lesen
      unsigned long lChecksum = m_FileHeader.GetChecksum();
      m_FileHeader.CalcChecksum();                             // Checksumme zurücksetzen
      if (m_FileHeader.IsType(PLOT_FILE) || m_FileHeader.IsType(EDITPLOT_FILE))
      {
         REPORT("Plots");
         CPlotLabel *ppl;
         int i, nCount;
         CLabel::ArchiveRead(ar, &nCount, sizeof(int), true);
         ar.Flush();
         for (i=0; i<nCount; i++)
         {
            ppl = new CPlotLabel;
            InsertPlot(ppl);
            ppl->Read((HANDLE)ar.GetFile()->m_hFile);
            ppl->GetChecksum(&m_FileHeader);
            ppl->CheckCurveColors(true);
            if (pApp->m_nCallingWindowMessage == WM_USER_OPT_UPDATE)
               ppl->SetPlotID(i+1);
         }
         if (!m_FileHeader.IsValid(lChecksum))
            AfxThrowArchiveException(CRC_EXCEPTION, ar.GetFile()->GetFileName());

         if (pApp->m_hCallingWindowHandle && pApp->m_nCallingWindowMessage)
            pApp->m_bEditMode = m_FileHeader.IsType(EDITPLOT_FILE) ? true : false;
         else
            pApp->m_bEditMode = false;

         UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
      }
      else if(m_FileHeader.IsType(UPDATE_FILE))
      {
         if (m_FileHeader.GetVersion() > 100) AfxThrowArchiveException(VERSION_EXCEPTION);
         CLabelContainer AutoDeleteContainer;
         CCurveLabel *pcl    = NULL;
         CCurveLabel *pCurve = NULL;
         CPlotLabel  *ppl    = NULL;
         char    *pszHeading = NULL;
         ProcessLabel procl;
         AutoDeleteContainer.SetDeleteContent(true);
         long nPlots, nPlot, iPlot, nCurves, nCurve, iCurve, nFlag;
         CLabel::ArchiveRead(ar, &nPlots, sizeof(long), true); // Anzahl der Plots
         for (iPlot=0; iPlot<nPlots; iPlot++)
         {
            CLabel::ArchiveRead(ar, &nPlot, sizeof(long), true);// Nummer des Plots, der geändert werden soll
            procl.nPos = nPlot;
            procl.pl   = NULL;
            GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &procl, CCARA2DVDoc::FindPlotOrCurveByID);
            if (procl.pl) ppl = (CPlotLabel*) procl.pl;        // diesen Plot verändern
            else          ppl = NULL;
            pszHeading = CLabel::ReadVariableString(ar, true); // Überschrift lesen
            if (ppl && pszHeading) ppl->SetHeading(pszHeading);// und setzen
            SAFE_DELETE(pszHeading);
            CLabel::ArchiveRead(ar, &nCurves, sizeof(long), true);// Anzahl der Kurven in dem Plot, die geändert werden sollen
            for (iCurve=0; iCurve<nCurves; iCurve++)
            {
               CLabel::ArchiveRead(ar, &nCurve, sizeof(long), true);// Nummer der Kurve, die ...
               CLabel::ArchiveRead(ar, &nFlag, sizeof(long), true);// Änderungsflags
               if (nFlag & 0x00000001)                         // Kurve wird in der Datei übergeben
               {
                  ar.Flush();                                  // File-Pointer updaten
                  pcl = new CCurveLabel;
                  if (!pcl) AfxThrowArchiveException(CArchiveException::genericException);
                  AutoDeleteContainer.InsertLabel(pcl);        // Kurve lesen
                  pcl->Read((HANDLE)ar.GetFile()->m_hFile);
                  pcl->GetChecksum(&m_FileHeader);
                  pcl->SetTargetFuncNo(nCurve);
                  if (!pcl->IsValid()) AfxThrowArchiveException(CArchiveException::badIndex);
               }
               if (ppl)
               {
                  if (pcl)
                  {
                     AutoDeleteContainer.RemoveLabel(pcl);     // neue Kurve aus dem Container entfernen
                     pApp->m_bNoUndo = true;
                     if (!ppl->InsertCurve(pcl, false, MAKELONG(-1, FIT_INSERTED_CURVE_ID|REPLACE_CURVE_TEXT)))// Kurve einfügen bzw. ersetzen
                        delete pcl;                            // wenn nicht eingefügt, löschen
                     pApp->m_bNoUndo = false;
                     pcl = NULL;                               // Zeiger auf NULL !
                  }
                  if (nFlag & (0x00000002|0x00000008))         // Sichtbarkeit setzen oder Kurve löschen                  
                  {
                     procl.nPos = nCurve;
                     procl.pl   = NULL;
                     ppl->GetCurveList()->ProcessWithLabels(RUNTIME_CLASS(CCurveLabel), &procl, CCARA2DVDoc::FindPlotOrCurveByID);
                     if (procl.pl)                             // ist die Kurve schon vorhanden
                     {
                        pCurve = (CCurveLabel*)procl.pl;
                        pCurve->SetVisible((nFlag & 0x00000004) ? true : false);
                        if (nFlag & 0x00000008)                // Löschen
                        {
                           int nPos;
                           if (ppl->RemoveCurve(pCurve, nPos))
                              delete pCurve;
                        }
                     }
                  }
               }
            }
            if (ppl) UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO, ppl);
         }
         if (!m_FileHeader.IsValid(lChecksum))
            AfxThrowArchiveException(CRC_EXCEPTION, ar.GetFile()->GetFileName());
      }
      else if(m_FileHeader.IsType(CURVE_FILE))
      {
         REPORT("Curves");
         ar.Flush();
         CLabelContainer AutoDeleteContainer;
         CCurveLabel *pcl = new CCurveLabel;
         if (!pcl) AfxThrowArchiveException(CArchiveException::genericException);
         AutoDeleteContainer.InsertLabel(pcl);
         AutoDeleteContainer.SetDeleteContent(true);

         if (pcl)
         {
            CPlotLabel *pActual = NULL;
            if (m_FileHeader.GetVersion() == 100)              // Version 1.00
            {
               pcl->Read((HANDLE)ar.GetFile()->m_hFile);
               pcl->GetChecksum(&m_FileHeader);
            }
            else                                               // Version >= 1.10
            {
               pcl->Serialize(ar);
            }

            if (!m_FileHeader.IsValid(lChecksum))              // stimmt die Checksumme
               AfxThrowArchiveException(CRC_EXCEPTION, ar.GetFile()->GetFileName());

            if ((pActual == NULL) && (pApp->m_pMainWnd != NULL))
            {
               CView *pView = ((CMainFrame*)pApp->m_pMainWnd)->GetActiveView();
               if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA2DVView)))
               {
                  pActual = ((CCARA2DVView*)pView)->GetPickedPlot();
               }
            }

            bool bTargetFunction = (pcl->GetTargetFuncNo() != 0) ? true : false;
            CPlotLabel *ppl = InsertCurve(pcl, pActual);

            if (ppl)                                           // wurde die Kurve eingefügt
            {
               AutoDeleteContainer.RemoveLabel(pcl);           // Kurve aus dem Container entfernen
               if (!bTargetFunction && !m_bLoadDocument)       // UndoCommand setzen, wenn Kurve geladen wird
                  pApp->SetUndoCmd(ID_EDIT_PASTE, pcl, NULL, -1, ppl);
            }
            UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO, ppl);
         }
      }
      else if (m_FileHeader.IsType(CARA2D_FILE))               // Version >= 1.10
      {
         CLabel::ArchiveRead(ar, &m_nPlotRects,sizeof(int), true);
         if ((m_nPlotRects > 0) && (m_nPlotRects < 100))
         {
            m_pPlotRects = new CRect[m_nPlotRects];
            if (m_pPlotRects)
               CLabel::ArchiveRead(ar, m_pPlotRects, sizeof(CRect)*m_nPlotRects, true);// Druckrechtecke laden
         }
         SavePlotRects();

         if (!m_FileHeader.IsValid(lChecksum))
         {
            AfxThrowArchiveException(CRC_EXCEPTION, ar.GetFile()->GetFileName());
         }

         UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
      }
      else if (m_FileHeader.IsType(LABEL_FILE))                // Version 1.00
      {
         CLabel::LoadGDIObjects(ar);
         m_Container.Serialize(ar);
         m_Container.GetChecksum(&m_FileHeader);
         CLabel::DeleteLOGArray();
         ASSERT_VALID(&m_Container);
         CLabel::ArchiveRead(ar, &m_nPlotRects,sizeof(int), true);
         if ((m_nPlotRects > 0) && (m_nPlotRects < 100))
         {
            m_pPlotRects = new CRect[m_nPlotRects];
            if (m_pPlotRects)
               CLabel::ArchiveRead(ar, m_pPlotRects, sizeof(CRect)*m_nPlotRects, true);
         }

         if (!m_FileHeader.IsValid(lChecksum))
            AfxThrowArchiveException(CRC_EXCEPTION, ar.GetFile()->GetFileName());

         pApp->ReadPrinterData10(ar);

         UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
      }
      else                                                     // kein Fileheadertyp
      {
         ar.Flush();
         CString name = ar.GetFile()->GetFileName();
         ar.GetFile()->SeekToBegin();
         name.MakeUpper();
         if      (strstr(LPCTSTR(name),".UEB")!=NULL)          // Cale.ueb-Datei
         {
            ReadCalgraphData((HANDLE)ar.GetFile()->m_hFile);
            UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
         }
         else if (strstr(LPCTSTR(name),".WAV")!=NULL)          // Wave-Datei
         {
            m_pFile = ar.GetFile()->Duplicate();
            m_pFile->SeekToBegin();
         }
         else
         {
            AfxThrowArchiveException(CArchiveException::badIndex, name);
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVDoc Diagnose
 
#ifdef _DEBUG
void CCARA2DVDoc::AssertValid() const
{
	CDocument::AssertValid();
   ASSERT_VALID(&m_Container);
}

void CCARA2DVDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVDoc Befehle
/******************************************************************************
* Name       : ReadCalgraphData                                               *
* Definition : BOOL ReadCalgraphData(HANDLE);                                 *
* Zweck      : Einlesen des Datenformates von dem CALE-Program CALGRAPH.      *
* Aufruf     : ReadCalgraphData(hFile);                                       *
* Parameter  :                                                                *
* hFile  (E) : Handle der geöffneten Datei                        (HANDLE)    *
* Funktionswert : (true, false)                                   (BOOL)      * 
******************************************************************************/
void CCARA2DVDoc::ReadCalgraphData(HANDLE hfile)
{
   short       nvalue;
   float       fvalue;
   char        xtext[20], ytext[20], text[160], plottext[160] = "";
   int         numPlots, numCurves, i, j, k, xdivision;
   CPlotLabel *pPlot  = NULL;
   CCurveLabel*pCurve = NULL;
   RANGE       range;

   SetFilePointer(hfile, 0, NULL, FILE_BEGIN);

   ReadFile(hfile, &nvalue, sizeof(short));           // gdriver
   if ((nvalue!=9) && (nvalue!=3))  AfxThrowArchiveException(CArchiveException::badIndex, NULL);
   ReadFile(hfile, &nvalue, sizeof(short));           // gmode
   if ((nvalue!=2) && (nvalue!=1))  AfxThrowArchiveException(CArchiveException::badIndex, NULL);
   ReadFile(hfile, &nvalue, sizeof(short));           // windows
   numPlots = nvalue;
   if ((nvalue<0) || (nvalue>4))    AfxThrowArchiveException(CArchiveException::badIndex, NULL);
   ReadFile(hfile, &nvalue, sizeof(short));           // sort
   if ((nvalue!=0) && (nvalue!=1))  AfxThrowArchiveException(CArchiveException::badIndex, NULL);
   ReadFile(hfile, text,    160          );           // hardcopytext

   for (i=0; i<8; i++) 
   {
      strcat(plottext, &text[i*20]);
      strcat(plottext, " ");
   }
   DosToWindowText(plottext);

   for (i=0; i<numPlots; i++)                         // für alle Plots
   {
      pPlot = new CPlotLabel;
      if (!pPlot)                   AfxThrowMemoryException();
      InsertPlot(pPlot);
      pPlot->SetHeading(plottext);
      ReadFile(hfile, &nvalue, sizeof(short));        // rasterfarbe
      ReadFile(hfile, &nvalue, sizeof(short));        // achsenfarbe
      ReadFile(hfile, &nvalue, sizeof(short));        // anzahl der Kurven
      if ((nvalue<0) || (nvalue>16)) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
      numCurves = nvalue;
      ReadFile(hfile, &nvalue, sizeof(short));        // kurvenart
      ReadFile(hfile, &nvalue, sizeof(short));        // Achsenteilung
      if ((nvalue!=0) && (nvalue!=1)) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
      if (nvalue == 0) xdivision = ETSDIV_LINEAR;
      else             xdivision = ETSDIV_FREQUENCY;
      ReadFile(hfile, xtext,   8            );        // xtext
      DosToWindowText(xtext);
      ReadFile(hfile, ytext,   8            );        // ytext
      DosToWindowText(ytext);
      ReadFile(hfile, &fvalue, sizeof(float));        // maxx
      ReadFile(hfile, &fvalue, sizeof(float));        // minx
      ReadFile(hfile, &fvalue, sizeof(float));        // maxy
      ReadFile(hfile, &fvalue, sizeof(float));        // miny

      ReadFile(hfile, &fvalue, sizeof(float));        // default_maxx
      range.x2 = fvalue;
      ReadFile(hfile, &fvalue, sizeof(float));        // default_minx
      range.x1 = fvalue;
      ReadFile(hfile, &fvalue, sizeof(float));        // default_maxy
      range.y2 = fvalue;
      ReadFile(hfile, &fvalue, sizeof(float));        // default_miny
      range.y1 = fvalue;
      pPlot->SetXNumMode(ETSDIV_NM_STD);              // Zahlendarstellung
      pPlot->SetYNumMode(ETSDIV_NM_STD);
      for (j=0; j<numCurves; j++)
      {
         pCurve = new CCurveLabel;
         if (!pCurve) AfxThrowMemoryException();
         pCurve->SetXDivision(xdivision);             // bei calgraph ist nur die x-Achsenteilung relevant
         pCurve->SetXUnit(xtext);
         pCurve->SetYUnit(ytext);
         if (!pPlot->InsertCurve(pCurve)) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
         ReadFile(hfile, &nvalue, sizeof(short));     // kurvenfarbe
         pCurve->SetColor(0xffffffff);
         ReadFile(hfile, text,    20           );     // kurventext
         DosToWindowText(text);
         pCurve->SetDescription(text);
         ReadFile(hfile, &nvalue, sizeof(short));     // anzahlwerte
         pCurve->SetTargetFuncNo(0);                  // Zielfunktion kann es nicht sein
         if (!pCurve->SetSize(nvalue)) AfxThrowMemoryException();
         for (k=0; k<nvalue; k++)
         {
            ReadFile(hfile, &fvalue, sizeof(float));  // x-Wert
            pCurve->SetX_Value(k, (double) fvalue);
         }
         for (k=0; k<nvalue; k++)
         {
            ReadFile(hfile, &fvalue, sizeof(float));  // y-Wert
            pCurve->SetY_Value(k, (double) fvalue);
         }
      }
      pPlot->SetDefaultRange(&range);
      pPlot->CheckCurveColors(true);
   }
}

void CCARA2DVDoc::DeleteContents() 
{
   BEGIN("DeleteContents");
   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
   DeletePlots();
   if (m_pPlotRects)
   {
      delete[] m_pPlotRects;
      m_pPlotRects = NULL;
      m_nPlotRects = 0;
   }
   UpdateAllViews(NULL, UPDATE_DELETE_CONTENTS);
   CCARADoc::DeleteContents();
}

void CCARA2DVDoc::SaveDocument(LPCTSTR szPathName)
{
   OnSaveDocument(szPathName);
}

BOOL CCARA2DVDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
   BEGIN("CCARA2DVDoc::OnSaveDocument");
   CCARA2DVApp * pApp = (CCARA2DVApp*)AfxGetApp();

   REPORT("%s", lpszPathName);
   BOOL bReturn;
   bReturn = CCARADoc::OnSaveDocument(lpszPathName);

   if (bReturn)
   {
      pApp->m_CallingWindowwParam |= 0x00010000;      // HIWORD(wParam) auf 1 setzen
      SetModifiedFlag(false);
   }
   return bReturn;
}

void CCARA2DVDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
   CCARA2DVApp * pApp = (CCARA2DVApp*)AfxGetApp();
   if (pApp->m_bEditMode)
   {
      pApp->m_strEditPathName = _T(lpszPathName);
      bAddToMRU = false;
   }
   CCARADoc::SetPathName(lpszPathName, bAddToMRU);
}

// Einfügen oder löschen von Plots und Kurven
/******************************************************************************
* Name       : InsertPlot                                                     *
* Definition : void InsertPlot(CPlotLabel *, int);                            *
* Zweck      : Einfügen eines Plots in das Dokument.                          *
* Aufruf     : InsertPlot(ppl, nPos);                                         *
* Parameter  :                                                                *
* ppl    (E) : Einzufügender PlotLabel                          (CPlotLabel*) *
* nPos   (E) : Einfügeposition 0,..,x<Anzahl; -1 : am Ende              (int) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVDoc::InsertPlot(CPlotLabel *ppl, int nPos)     // Klassenfunktion
{
   BEGIN("InsertPlot");
   bool bInsert = false;
   ProcessLabel lt = {ppl, -1, &bInsert, &nPos};
   m_Container.ProcessWithLabels(NULL, &lt, InsertPlot);
   if (!bInsert) bInsert = m_Container.InsertLabel(ppl, -1);
   if (bInsert)
   {
      m_nInvalidStates |= (MENU_STATES|EDIT_STATES);
      SetModifiedFlag(true);                                // modified setzen
   }
}
int CCARA2DVDoc::InsertPlot(CLabel *pl, void *pParam)       // statische Funktion für die Rekursion
{
   BEGIN("InsertPlot R");
   if (pl && !pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      plt->nPos++;                                          // Position zählen
      if (plt->nPos == *((int*)plt->pParam2))
      {
         *((bool*)plt->pParam1) = true;                     // Plot gefunden
         return INSERT_LABEL_BEFORE|STOP_PROCESSING;        // einfügen und Suche beenden
      }
   }
   return 0;
}

/******************************************************************************
* Name       : RemovePlot                                                     *
* Definition : void RemovePlot(CPlotLabel*, bool);                            *
* Zweck      : Entfernen eines PlotLabels aus dem Dokument.                   *
* Aufruf     : RemovePlot(ppl, bDelete);                                      *
* Parameter  :                                                                *
* ppl    (E) : Zu entfernender PlotLabel                        (CPlotLabel*) *
* bDelete(E) : Löschen des PlotLabel (true, false)                     (bool) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVDoc::RemovePlot(CPlotLabel *ppl, int &nPos)
{
   BEGIN("RemovePlot");
   bool bFound = false;
   ProcessLabel lt = {ppl, -1, &bFound, NULL};
   m_Container.ProcessWithLabels(NULL, &lt, RemovePlot);
   if (bFound)
   {
      nPos = lt.nPos;
      m_nInvalidStates |= (MENU_STATES|EDIT_STATES);
      UpdateAllViews(NULL, UPDATE_DELETE_LABEL, ppl);
      SetModifiedFlag(true);                                   // modified setzen
   }
   else nPos = -1;
}
/******************************************************************************
* Name       : RemovePlot                                                     *
* Definition : static int RemovePlot(CLabel *pl, void *pParam);               *
* Zweck      : Entfernen eines Plots aus dem Container des Dokumentes.        *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Struktur ProcessLabel                                          *
*  pl    (A) : PlotLabel, aus dem die Kurve entfernt wurde          (CLabel*) *
*  nPos  (A) : Position des Plots im Document                           (int) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::RemovePlot(CLabel *pl, void *pParam)
{
   BEGIN("RemovePlot R");
   if (pl && !pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      plt->nPos++;                                          // Position zählen
      if (plt->pl == pl)
      {                                                     // war enthalten ?
         *((bool*)plt->pParam1) = true;                     // Plot gefunden
         return REMOVE_LABEL|STOP_PROCESSING;               // enfernen und Suche beenden
      }
   }
   return 0;
}
/******************************************************************************
* Name       : RemoveCurve                                                    *
* Definition : CPlotLabel* RemoveCurve(CCurveLabel*,int&);                    *
* Zweck      : Entfernen einer Kurve aus dem Dokument.                        *
* Aufruf     : RemoveCurve(pcl, nPos);                                        *
* Parameter  :                                                                *
* pcl    (E) : Zu entfernender CurveLabel                      (CCurveLabel*) *
* nPos   (A) : Position des entfernten CurveLabel                      (int&) *
* Funktionswert : PlotLabel, aus dem entfernt wurde.             (PlotLabel*) *
*                 NULL bei Fehler.
******************************************************************************/
CPlotLabel* CCARA2DVDoc::RemoveCurve(CCurveLabel *pcl, int &nPos)
{
   ProcessLabel lt = {pcl, nPos, NULL, NULL};
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &lt, RemoveCurve);
   if (lt.pl && lt.pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
   {
      nPos = lt.nPos;
      return (CPlotLabel*) lt.pl;
   }
   return NULL;
}
/******************************************************************************
* Name       : RemoveCurve                                                    *
* Definition : static int RemoveCurve(CLabel *pl, void *pParam);              *
* Zweck      : Entfernen einer Kurve aus den PlotLabeln in dem Container des  *
*              Dokumentes.                                                    *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Struktur ProcessLabel                                          *
*  pl    (A) : PlotLabel, aus dem die Kurve entfernt wurde          (CLabel*) *
*  nPos  (A) : Position der Kurve in dem Plot                           (int) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::RemoveCurve(CLabel *pl, void *pParam)
{
   BEGIN("RemoveCurve R");
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if ((plt->pl != NULL) &&                                 // Zeiger != 0 ?
          ((CPlotLabel*)pl)->RemoveCurve((CCurveLabel*)plt->pl, plt->nPos))
      {                                                        // war enthalten ?
         plt->pl = pl;                                         // diesen Plot zurückliefern
         return STOP_PROCESSING;                               // nicht weiter suchen
      }
   }
   return 0;
}

/******************************************************************************
* Name       : InsertCurve                                                    *
* Definition : CPlotLabel *InsertCurve(CLabel*, CPlotLabel*, int);            *
* Zweck      : Einfügen einer Kurve in das Dokument nach bestimmten Kriterien *
* Aufruf     : InsertCurve(pl, pActual, nPos);                                *
* Parameter  :                                                                *
* pl     (E) : Einzufügende Kurve.                                 (CLabel*)  *
* pActual(E) : Zu bevorzugender PlotLabel für das einfügen.    (CPlotLabel*)  *
* nPos   (E) : Einfügeposition in den PlotLabel                        (int)  *
* Funktionswert : Zeiger auf Plotlabel, in den eingefügt wurde.(CPlotLabel*)  *
******************************************************************************/
CPlotLabel *CCARA2DVDoc::InsertCurve(CCurveLabel *pcl, CPlotLabel *pActual, long nPos)
{
   BEGIN("InsertCurve");
   bool bModified = IsModified();                                // alten Zustand sichern
   SetModifiedFlag(true);                                      // modified setzen
   if (pActual && pActual->InsertCurve(pcl, false, nPos))      // erstmal den gepickten Plot nehmen 
   {
      pActual->CheckCurveColors();
      m_nInvalidStates |= EDIT_STATES;
      return pActual;
   }

   ProcessLabel lt = {pcl, nPos, NULL, NULL};
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &lt, CCARA2DVDoc::InsertCurve);
   if (lt.pl && lt.pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
   {
      m_nInvalidStates |= EDIT_STATES;
      return (CPlotLabel*) lt.pl;
   }

   CPlotLabel *ppl = new CPlotLabel;                           // wenn's nicht anders geht, einen neuen Plot erzeugen
   if (ppl)
   {
      if (ppl->InsertCurve(pcl))
      {
         ppl->CheckCurveColors();
         ppl->DeterminDefaultRange();
         ppl->SetHeading(pcl->GetText());
         InsertPlot(ppl);
         CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();

         if (!m_bLoadDocument)
            pApp->SetUndoCmd(ID_EDIT_PASTE, ppl, NULL, -1, &m_Container);

         UpdateAllViews(NULL, UPDATE_INSERT_LABEL, ppl);
         return ppl;
      }
      else delete ppl;
   }

   SetModifiedFlag(bModified);                                 // wenn nicht passiert ist alten Zustand wieder setzen
   return NULL;
}
/******************************************************************************
* Name       : InsertCurve                                                    *
* Definition : static int InsertCurve(CLabel *pl, void *pParam);              *
* Zweck      : Einfügen einer Kurve in einen passenden PlotLabel aus dem      *
*              Container des Dokumentes.                                      *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Struktur ProcessLabel                                          *
*  pl    (A) : PlotLabel, in den die Kurve eingefügt wurde          (CLabel*) *
*  nPos  (A) : Einfügeposition der Kurve in dem Plot                    (int) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::InsertCurve(CLabel *pl, void *pParam)
{
   BEGIN("InsertCurve");
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if ((plt->pl != NULL) &&                                 // Zeiger != 0 ?
           plt->pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)) &&    // CurveLabel ?
          ((CPlotLabel*)pl)->InsertCurve((CCurveLabel*)plt->pl, (plt->pParam1 != NULL), plt->nPos))
      {                                                        // läßt sich einfügen ?
         ((CPlotLabel*)pl)->CheckCurveColors();                // Kurvenfarben prüfen
         plt->pl = pl;                                         // diesen Plotlabel zurückliefern
         return STOP_PROCESSING;
      }
   }
   return 0;
}

void CCARA2DVDoc::DeletePlots()
{
   BEGIN("DeletePlots");
   if (!m_Container.IsEmpty())
   {
      m_Container.DeleteAll();
      DeletePlotMenu();
   }
   m_nInvalidStates |= (MENU_STATES|EDIT_STATES);
}

void CCARA2DVDoc::DeletePlotMenu()
{
   if (m_pPlotMenu)
   {
      CWnd *pMainWnd = AfxGetApp()->m_pMainWnd;
      if (!pMainWnd)    return;
      CMenu *pMenu     = pMainWnd->GetMenu();
      if (!pMenu)       return;

      int i, nCount = m_pPlotMenu->GetMenuItemCount();
      for (i=0; i<nCount; i++)
      {
         m_pPlotMenu->RemoveMenu(0, MF_BYPOSITION);
      }
   }
}

void CCARA2DVDoc::BuildNewPlotMenu()
{
   BEGIN("BuildNewPlotMenu");

   if (!m_pPlotMenu) AppendPlotMenu();
   if (m_pPlotMenu)
   {
      DeletePlotMenu();
      m_nInvalidStates &= ~MENU_STATES;
      ProcessLabel prl = {NULL, 0, m_pPlotMenu, NULL};
      m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &prl, BuildNewPlotMenu);
   }
}

/******************************************************************************
* Name       : BuildNewPlotMenu                                               *
* Definition : static int BuildNewPlotMenu(CLabel *pl, void *pParam);         *
* Zweck      : Erzeugen eines neuen Plot-Popupmenues für das Hauptmenu "View" *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Struktur ProcessLabel                                          *
*  nPos    (EA): Positionszähler                                        (int) *
*  pParam1 (EA): Menuehandle des Popupmenues                          (HMENU) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::BuildNewPlotMenu(CLabel *pl, void *pParam)
{
//   BEGIN("BuildNewPlotMenu R");
   if (pl && pParam)
   {
      ProcessLabel *pprl  = (ProcessLabel*) pParam;
      CCaraMenu    *pMenu = (CCaraMenu*) pprl->pParam1;
      if (pMenu)
      {
         CString str;
         str.Format(IDS_PLOT_N, pprl->nPos+1);
         pMenu->AppendODMenuItem(ID_VIEW_PLOT+pprl->nPos, str, NULL);
         pprl->nPos++;
      }
   }
   return 0;
}

void CCARA2DVDoc::OnFuncSaveCurve()
{
   CCARA2DVApp  *pApp  = (CCARA2DVApp*) AfxGetApp();
   CCARA2DVView *pView = (CCARA2DVView*)GetCaraView();
   if (!pView) return;
   CCurveLabel  *pcl   =    pView->Get1stPickedCurve();              // zu speichernde Kurve holen

   _tzset();
   time_t     Timer;
   time(&Timer);
   tm *pTm = localtime(&Timer);
   char year[8] = "1999";
   if (pTm)
   {
      int nYear  = pTm->tm_year + ((pTm->tm_year < 17) ? 2000 : 1900);
      wsprintf(year, "%4d", nYear);
   }
   m_FileHeader.Init(CURVE_FILE, year, g_nFileVersion);
   CLabel::SetFileVersion(g_nFileVersion);

   if (!pcl) return;
   CString title;
   CString curvefilter, otherFilter;
   DWORD   dwFlags = OFN_OVERWRITEPROMPT|OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_EXPLORER;

   curvefilter.LoadString(IDS_CURVE_FILTER);
   curvefilter += (TCHAR)'|';
   curvefilter += _T("*.");
   curvefilter += _T(CURVE_EXTENSION);
   curvefilter += (TCHAR)'|';
   int nCustMaxFilter = 1;

   otherFilter.LoadString(IDS_CURVE_FILTER2);
   curvefilter += otherFilter;
   curvefilter += (TCHAR)'|';
   curvefilter += _T("*.");
   curvefilter += _T(CURVE_EXTENSION_2);
   curvefilter += (TCHAR)'|';
   nCustMaxFilter++;

   if (pcl && !pcl->IsWriteProtect())
   {
      CCurveLabel *pcl2 = pView->Get2ndPickedCurve();
      if ((pcl2 == NULL) ||
          (pcl2 != NULL) && !pcl2->IsWriteProtect())
      {
         otherFilter.LoadString(IDS_WAVE_FILTER);
         curvefilter += otherFilter;
         curvefilter += (TCHAR)'|';
         curvefilter += _T("*.WAV");
         curvefilter += (TCHAR)'|';
         nCustMaxFilter++;

         otherFilter.LoadString(IDS_TXT_FILTER);
         curvefilter += otherFilter;
         curvefilter += (TCHAR)'|';
         curvefilter += _T("*.TXT");
         curvefilter += (TCHAR)'|';
         nCustMaxFilter++;
      }
      dwFlags |= OFN_ENABLETEMPLATE;
   }

   CSaveCurveAsWave fd(false, CURVE_EXTENSION, pcl->GetText(), dwFlags, LPCTSTR(curvefilter), AfxGetApp()->m_pMainWnd);
   fd.m_ofn.nMaxCustFilter = nCustMaxFilter;

   title.LoadString(IDS_SAVE_CURVE);
   fd.m_ofn.lpstrTitle     = LPCTSTR(title);
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;

   if (!pApp->m_strDefaultCurveDir.IsEmpty())
   {
      fd.m_ofn.lpstrInitialDir = pApp->m_strDefaultCurveDir;
   }
   fd.m_pclLeftIn  = pcl;
   fd.m_pclRightIn = pView->Get2ndPickedCurve();    // 2. zu speichernde Kurve holen

   if (fd.DoModal()==IDOK)
   {
      pApp->m_strDefaultCurveDir = fd.GetPathName();
      int nPos = pApp->m_strDefaultCurveDir.ReverseFind('\\');
      if (nPos != -1) pApp->m_strDefaultCurveDir = pApp->m_strDefaultCurveDir.Left(nPos);

      CFile file(fd.GetPathName(), CFile::modeCreate|CFile::modeWrite);
      CArchive ar(&file, CArchive::store);
      ar.m_pDocument = this;

      if (fd.m_ofn.nFilterIndex == 3)                          // Wave-Kurvenformat
      {
         SaveCurveAsWave(ar, &fd);
      }
      else if (fd.m_ofn.nFilterIndex == 1)                     // Label-Kurvenformat
      {
         m_FileHeader.CalcChecksum();                       // Checksumme initialisieren
         WriteFileHeader(ar);                               // Fileheader schreiben
         pcl->Serialize(ar);                                // Kurve schreiben
         WriteFileHeader(ar, false);                        // Checksumme schreiben
      }
      else if (fd.m_ofn.nFilterIndex == 2)                  // Einfaches-Kurvenformat
      {
         pcl->Write((HANDLE)file.m_hFile);
         file.Flush();
         ar.Flush();
      }
      else                                                  // Text-Kurvenformat
      {
         int i, nValues = pcl->GetNumValues();
         bool bLocus = (pcl->GetLocus() != 0) ? true : false;
         char szList[4];
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
         CString str;
         if (bLocus) str.Format("%s\r\nLocus:[%s]\r\n[%s]\r\n[%s]\r\n%d\r\n", pcl->GetText(), pcl->GetLocusUnit(), pcl->GetXUnit(), pcl->GetYUnit(), nValues);
         else        str.Format("%s\r\n[%s]\r\n[%s]\r\n%d\r\n", pcl->GetText(), pcl->GetXUnit(), pcl->GetYUnit(), nValues);
         ar.WriteString(str);
         for (i=0; i<nValues; i++)
         {
            if (bLocus) str.Format("%g%c %g%c %g\r\n", pcl->GetLocusValue(i), szList[0], pcl->GetX_Value(i), szList[0], pcl->GetY_Value(i));
            else        str.Format("%g%c %g\r\n", pcl->GetX_Value(i), szList[0], pcl->GetY_Value(i));
            ar.WriteString(str);
         }
      }
   }
}

void CCARA2DVDoc::OnUpdateFuncLoadCurve(CCmdUI* pCmdUI)    {pCmdUI->Enable((m_pFile==NULL) ? true : false);}
void CCARA2DVDoc::OnUpdateEditCopy(CCmdUI* pCmdUI)         {pCmdUI->Enable(m_Container.GetCount());}
void CCARA2DVDoc::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(true);
   if (m_nInvalidStates&MENU_STATES)
   {
      DeletePlotMenu();
      BuildNewPlotMenu();
   }
   CheckEditable();
}
void CCARA2DVDoc::OnUpdateFilePrint(CCmdUI* pCmdUI)        {pCmdUI->Enable(m_Container.GetCount());}
void CCARA2DVDoc::OnUpdateFilePrintDirect(CCmdUI* pCmdUI)  {pCmdUI->Enable(m_Container.GetCount());}
void CCARA2DVDoc::OnUpdateFileSave(CCmdUI* pCmdUI)         {pCmdUI->Enable(IsModified()&&(m_pFile==NULL)&& !((CCARA2DVApp*)AfxGetApp())->m_bEditMode);}
void CCARA2DVDoc::OnUpdateFileOpen(CCmdUI* pCmdUI)         {pCmdUI->Enable((m_pFile==NULL) && !((CCARA2DVApp*)AfxGetApp())->m_bEditMode);}

void CCARA2DVDoc::AppendPlotMenu()
{
   BEGIN("AppendPlotMenu");
   if (!m_pPlotMenu)
   {
      m_pPlotMenu = new CCaraMenu;
      CWinApp *pApp = AfxGetApp();
      if (m_pPlotMenu && pApp && pApp->m_pMainWnd && m_pPlotMenu->CreateMenu())
      {
         CMenu * pMenu  = AfxGetApp()->m_pMainWnd->GetMenu();
         if (pMenu)
         {
            CCaraMenu * pView = (CCaraMenu*) pMenu->GetSubMenu(2);
            if (pView)
            {
               CString string;
               string.LoadString(IDS_PLOTS);
               if (!pView->InsertMenu(1, MF_POPUP|MF_BYPOSITION, (UINT)m_pPlotMenu->m_hMenu, string))
               {
                  delete m_pPlotMenu;
                  m_pPlotMenu = NULL;
               }
            }
         }
      }
   }
}

void CCARA2DVDoc::CheckEditable()
{
   if (m_nInvalidStates & EDIT_STATES)
   {
      m_nInvalidStates &= ~EDIT_STATES;
      CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
      pApp->m_bEditable = false;
      m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pApp->m_bEditable, CheckEditable);
   }
}
/******************************************************************************
* Name       : CheckEditable                                                  *
* Definition : static int CheckEditable(CLabel *pl, void *pParam);            *
* Zweck      : Überprüfung, ob eine Kurve im Container des Documentes editier-*
*              bar ist.                                                       *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Zeiger auf bool-Variable                               (bool*) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::CheckEditable(CLabel *pl, void *pParam)
{
   BEGIN("CheckEditable R");
   if (pl && pParam)
   {
      CPlotLabel      *ppl     = (CPlotLabel*) pl;
      CLabelContainer *pCurves = ppl->GetCurveList();
      int j, numCurves = pCurves->GetCount();
      for (j=0; j<numCurves; j++)                           // alle Kurven in einem Plot durchsuchen
      {
         pl = pCurves->GetLabel(j);
         if (pl && pl->IsVisible() && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)) && ((CCurveLabel*)pl)->IsEditable())
         {
            (*((bool*)pParam)) = true;                      // ist eine editierbar 
            return STOP_PROCESSING;                         // nicht weiter suchen
         }               
      }
   }
   return 0;
}

void CCARA2DVDoc::OnFuncLoadCurve()
{
   if (m_pFile == NULL)
   {
      CFileHeader *pfh = &m_FileHeader;
      CString     title;
      CString     curvefilter;
      CString     otherFilter;
      CString     strFormat;

      CCurveFileDlg fd(true);

      curvefilter.LoadString(IDS_CURVE_FILTER);
      curvefilter += (TCHAR)'\0';
      curvefilter += _T("*.");
      curvefilter += _T(CURVE_EXTENSION);
      curvefilter += (TCHAR)'\0';
      fd.m_ofn.Flags |= OFN_SHOWHELP;

      int i, nPos, nCount = m_TextMacros.m_Descriptions.GetSize();
      for (i=0; i<nCount; i++)
      {
         fd.m_ofn.nMaxCustFilter++;
         strFormat.LoadString(IDS_TEXT_FILTER);
         otherFilter.Format(LPCTSTR(strFormat), LPCTSTR(m_TextMacros.m_Descriptions.GetAt(i)));
         curvefilter += otherFilter;
         nPos = otherFilter.Find("(*.");
         if (nPos != -1)
         {
            otherFilter = otherFilter.Right(otherFilter.GetLength()-nPos-1);
            nPos = otherFilter.Find(")");
            otherFilter = otherFilter.Left((nPos==-1) ? 5 : nPos);
         }
         else otherFilter = _T("*.TXT");
         curvefilter += (TCHAR)'\0';
         curvefilter += otherFilter;
         curvefilter += (TCHAR)'\0';
      }

      fd.m_ofn.nMaxCustFilter++;
      otherFilter.LoadString(IDS_WAVE_FILTER);
      curvefilter += otherFilter;
      curvefilter += (TCHAR)'\0';
      curvefilter += _T("*.WAV");
      curvefilter += (TCHAR)'\0';

      fd.m_ofn.nMaxCustFilter++;
      otherFilter.LoadString(AFX_IDS_ALLFILTER);
      curvefilter += otherFilter;
      curvefilter += (TCHAR)'\0';
      curvefilter += _T("*.*");
      curvefilter += (TCHAR)'\0';

      fd.m_ofn.nMaxCustFilter++;
      fd.m_ofn.lpstrFilter = curvefilter;
      title.LoadString(IDS_LOAD_CURVE);
      fd.m_ofn.lpstrTitle     = LPCTSTR(title);
      fd.m_ofn.nFilterIndex   = 1;
      fd.m_ofn.nFileExtension = 1;

      fd.m_ofn.Flags         |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

      CCARA2DVApp  *pApp  = (CCARA2DVApp*) AfxGetApp();
      CMainFrame   *pMain = (CMainFrame*) pApp->m_pMainWnd;
      fd.m_nPrecision = pMain->m_nPrecision;

      if (!pApp->m_strDefaultCurveDir.IsEmpty())
      {
         fd.m_ofn.lpstrInitialDir = pApp->m_strDefaultCurveDir;
      }

      if ((pMain != NULL) && (fd.DoModal()==IDOK))
      {
         pMain->m_nPrecision = fd.m_nPrecision;
         pMain->m_bSigned    = (fd.m_bSigned != 0) ? true : false;
         pApp->m_strDefaultCurveDir = fd.GetPathName();
         int nPos = pApp->m_strDefaultCurveDir.ReverseFind('\\');
         if (nPos != -1) pApp->m_strDefaultCurveDir = pApp->m_strDefaultCurveDir.Left(nPos);

         CString ext = fd.GetFileExt();
         ext.MakeUpper();
         if ((nCount!=0) && ((int)fd.m_ofn.nFilterIndex >1) && ((int)fd.m_ofn.nFilterIndex < nCount+2))
         {
            m_nTextMacro = fd.m_ofn.nFilterIndex - 2;
            m_pFile          = GetFile(fd.GetPathName(), CFile::modeRead|CFile::shareDenyWrite, NULL);
            pMain->m_pThread = AfxBeginThread(CCARA2DVDoc::ReadTextThread, m_pFile, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
         }
         else if (ext.Compare("WAV")==0)                          // ist es eine Wavedatei
         {
            m_pFile          = GetFile(fd.GetPathName(), CFile::modeRead|CFile::shareDenyWrite, NULL);
            pMain->m_pThread = AfxBeginThread(CCARA2DVDoc::ReadWaveThread, m_pFile,THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
         }
         else
         {
            CFile file(fd.GetPathName(), CFile::modeRead|CFile::shareDenyWrite);
            CArchive ar(&file, CArchive::load);
            ar.m_pDocument = this;
            TRY
            {
               if (file.GetLength() > 0)
               {
                  Serialize(ar);
                  SetModifiedFlag(true);
               }
            }
            CATCH_ALL(e)
            {
               ReportSaveLoadException(fd.GetPathName(), e, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
               e->Delete();
            }
            END_CATCH_ALL
         }
         if (pMain->m_pThread)
         {
            pMain->m_pThread->m_bAutoDelete = TRUE;
            pMain->m_pThread->ResumeThread();
         }
      }
   }
}

void CCARA2DVDoc::ReadFile(HANDLE hFile, void *p, DWORD dwSize)
{
   DWORD       dwBytes = 0;
   if (!::ReadFile(hFile, p, dwSize, &dwBytes, NULL)) AfxThrowArchiveException(CArchiveException::genericException, NULL);
   if (dwBytes != dwSize) AfxThrowArchiveException(CArchiveException::endOfFile, NULL);
}

UINT CCARA2DVDoc::ListenToPortThread(void *pParam)
{
/*
   UINT nReturn = 0;
   CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
   if (!pMainWnd)
   {
      return 2;
   }
   if (!pParam)
   {
      pMainWnd->PostMessage(WM_ENDTHREAD, 1);
      return 1;
   }
   CView      *pView = pMainWnd->GetActiveView();
   CDocument  *pDoc = NULL;
   CCARA2DVDoc *pCD = NULL;
   CLabelContainer *plc    = NULL;
   CCurveLabel     *pcurve = NULL;

   try
   {
      if (pView) pDoc = pView->GetDocument();
      if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc)))
         pCD = (CCARA2DVDoc*) pDoc;
      if (!pCD)  throw (int)1;

   }
   catch (int nError)
   {
      REPORT("Error: %d", nError);
      CString strCmd;
      CString strError;
      strError.LoadString(nError);
      pMainWnd->PostMessage(WM_ENDTHREAD, (WPARAM)pStr, (LPARAM)pParam);
      nReturn = nError;
   }
   */
   return 0;
}

/******************************************************************************
* Name       : ReadTextThread                                                 *
* Definition : UINT ReadTextThread(void *);                                   *
* Zweck      : Lesen einer Text-Datei mit Kurvenwerten in einem Thread.       *
*              Nachrichten an das Parent-Window:                              *
*              WM_NEWCURVE mit wParam als Zeiger auf den neuen CCurveLabel    *
*              WM_ENDTHREAD mit wParam als Rückgabewert der Funktion.         *
* Aufruf     : ReadTextThread(pParam);                                        *
* Parameter  :                                                                *
* pParam  (E): Zeiger auf CFile-Objekt                                (void*) *
* Funktionswert : 0 : Kurven gelesen.                                 (UINT)  *
*                 1 : Fehler, zu wenig Parameter übergeben.                   *
*                 2 : Kein Main-Window vorhanden.                             *
*                 3 : Lesefehler aufgetreten                                  *
*                 4 : Alocierfehler
******************************************************************************/
#define STRING_LENGTH 512
UINT CCARA2DVDoc::ReadTextThread(void * pParam)
{
   CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
   if (!pMainWnd)
   {
      return 2;
   }
   if (!pParam)
   {
      pMainWnd->PostMessage(WM_ENDTHREAD, 1);
      return 1;
   }

   CFile      *pFile = (CFile*) pParam;
   CArchive    loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
   char        line[STRING_LENGTH];
   int         i, j, nlineIndex, nCurrentUntil=0, nLine = 0, nReturn = 0, nOldIndex;
   bool        bOK, bStopLoop = false,
               bDecimalComma  = false;
   double      dValue;
   SControl    pControl[MAX_CONTROL];
   CView      *pView = pMainWnd->GetActiveView();
   CDocument  *pDoc = NULL;
   CCARA2DVDoc *pCD = NULL;
   CLabelContainer *plc    = NULL;
   CCurveLabel     *pcurve = NULL;

   ETSLSMB_CLSMB sLSMB;
   sLSMB.nLines    = 1;                                        // 1 Textzeile
   sLSMB.nLoadSave = ETSLSMB_LOAD;                             // Ladeanimation anzeigen
   ::LoadString(AfxGetInstanceHandle(), IDS_LOAD_CURVETEXTFILE, sLSMB.szText[0], ETSLSMB_STRINGLENGTH);
   CEtsLsmb *pcLSMB=NULL;
   CArchiveEx*pAr = (CArchiveEx*)&loadArchive;
   DWORD nType = TYPE_UNKNOWN;

   try
   {
      if (pView) pDoc = pView->GetDocument();
      if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc)))
         pCD = (CCARA2DVDoc*) pDoc;
      if (!pCD)  throw (int)1;

      CString string = pCD->m_TextMacros.m_Makros.GetAt(pCD->m_nTextMacro);
      if (pCD->m_TextMacros.CheckSyntax(string, pControl)!=0) throw 2;

      if (pCD->m_TextMacros.m_hWnd)
      {        // Debug Fenster initialisieren
         pCD->m_TextMacros.SendMessage(WM_USER, DEBUG_TMSET_SET_TXT, (LPARAM)pFile);
      }
      else
      {
         pcLSMB = new CEtsLsmb(&sLSMB);         // Fenster erzeugen und anzeigen
      }
      plc = new CLabelContainer;
      if (!plc) throw (int)2;
      plc->SetDeleteContent(true);
      pMainWnd->m_pLabel = plc;

      bOK   = (pAr->ReadString(nType, line, STRING_LENGTH)!=EOF) ? true : false;
      nLine++;
      nlineIndex = 0;
      nOldIndex = 0;

      for (i=0; pControl[i].nCommand != ENDLOOP;)
      {
         if (pCD->m_TextMacros.m_hWnd && pCD->m_TextMacros.m_nRun == DEBUG_TM_BREAK)
         {
            pCD->m_TextMacros.PostMessage(WM_USER, DEBUG_TMSET_SELECT, (LPARAM)&pControl[i]);
            if (pCD->m_TextMacros.m_pTexEdit)
            {
               int nPos = pCD->m_TextMacros.m_pTexEdit->m_cTextEdit.LineIndex(nLine-1);
               int nIndex = nlineIndex;
               if (nOldIndex == nlineIndex) nIndex++;
               pCD->m_TextMacros.m_pTexEdit->m_cTextEdit.SetSel(nPos + nOldIndex, nPos + nIndex);
            }
            while (pCD->m_TextMacros.m_nRun == DEBUG_TM_BREAK)
            {
               Sleep(10);
            }
            if (pCD->m_TextMacros.m_nRun == DEBUG_TM_STEP)
            {
               pCD->m_TextMacros.m_nRun = DEBUG_TM_BREAK;
               pCD->m_TextMacros.PostMessage(WM_USER, DEBUG_TMSET_RUN, 0);
            }
         }
         switch(pControl[i].nCommand)
         {
            case NO_OF_CURVES:                              // Anzahl der Kurven steht immer am Anfang
            {
               if (pControl[i].nNumber <= 0) throw (int)IDS_ERROR_LESS_CURVES;// 0 Kurven sind nicht erlaubt
               SendAndRemoveCurves(pMainWnd, plc);
               AllocateNewCurves(pMainWnd, plc, pControl[i].nNumber);
               i++;
            }  break;
            case NO_OF_VALUES:                              // Anzahl Kurvenwerte für Kurve [n] lesen
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  CString string;
                  nlineIndex += ExtractString(&line[nlineIndex], 0, NULL, pControl, i, string);
                  int nSize = atoi(LPCTSTR(string));
                  if (nSize <= 0) throw (int)IDS_ERROR_LESS_VALUES;// Größe der Arrays muß größer 0 sein
                  pcurve->SetSize(nSize);
               }
               i++;
            }  break;
            case NO_OF_LOOPS:                               // Anzahl Schleifendurchläufe
            {
               if (pControl[i].nNumber == 0)
               {
                  CString string;
                  int nLength = ExtractString(&line[nlineIndex], NUMERIC, " ", pControl, i, string);
                  if (nLength == 0) throw (int)IDS_ERROR_READ_FORMAT;// in dem String muß eine Zahl stehen
                  nlineIndex += nLength;
                  pControl[i].nNumber = atoi(string);       // Anzahl lesen
                  if (pControl[i].nNumber <= 0) throw (int)IDS_ERROR_LESS_LOOPS;
               }
               i++;
            } break;
            case DESCRIPTION:                               // Beschreibung [n] lesen
            case UNITX:                                     // X-Einheit der Kurve [n] lesen
            case UNITY:                                     // Y-Einheit der Kurve [n] lesen
            case UNITLOCUS:                                 // Einheit des Ortskurvenwinkels lesen
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  CString string;
                  int nCommand = pControl[i].nCommand;
                  nlineIndex += ExtractString(&line[nlineIndex], ALPHA_STRING, NULL, pControl, i, string);
                  if      (nCommand == DESCRIPTION)
                     pcurve->SetDescription((char*)LPCTSTR(string));
                  else if (nCommand == UNITX)
                     pcurve->SetXUnit((char*)LPCTSTR(string));
                  else if (nCommand == UNITY)
                     pcurve->SetYUnit((char*)LPCTSTR(string));
                  else if (nCommand == UNITLOCUS)
                     pcurve->SetLocusUnit((char*)LPCTSTR(string));
               }
               i++;
            }  break;
            case XLOGARITHMIC: case YLOGARITHMIC:              // logarithmische Kurvenwerte
            case LOCUS_LINEAR: case LOCUS_LOG:                 // Ortskurven linear, logarithmisch
            case XPOLAR:                                       // Polar Achse
            case ANGLE_GON: case ANGLE_RAD:                    // Winkelformate Gon, Rad
            case FORMAT_DOUBLE: case FORMAT_FLOAT:case FORMAT_SHORT:// Speicherformat
            case SINGLE_FORMAT:                                // nur Y-Werte
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  if (pControl[i].nCommand == XPOLAR      ) pcurve->SetXDivision(CCURVE_POLAR);
                  if (pControl[i].nCommand == XLOGARITHMIC) pcurve->SetXDivision(CCURVE_LOGARITHMIC);
                  if (pControl[i].nCommand == YLOGARITHMIC) pcurve->SetYDivision(CCURVE_LOGARITHMIC);
                  if (pControl[i].nCommand == ANGLE_GON   ) pcurve->SetAngleFormat(CCURVE_GON);
                  if (pControl[i].nCommand == ANGLE_RAD   ) pcurve->SetAngleFormat(CCURVE_RAD);
                  if (pControl[i].nCommand == LOCUS_LINEAR) pcurve->SetLocus(CCURVE_LINEAR);
                  if (pControl[i].nCommand == LOCUS_LOG   ) pcurve->SetLocus(CCURVE_LOGARITHMIC);
                  if (pControl[i].nCommand == FORMAT_DOUBLE) pcurve->ChangeFormat(CF_CHANGE_TO_DOUBLE_TYPE);
                  if (pControl[i].nCommand == FORMAT_FLOAT ) pcurve->ChangeFormat(CF_CHANGE_TO_FLOAT_TYPE);
                  if (pControl[i].nCommand == FORMAT_SHORT ) pcurve->ChangeFormat(CF_CHANGE_TO_SHORT_TYPE);
                  if (pControl[i].nCommand == SINGLE_FORMAT) pcurve->ChangeFormat(CF_CHANGE_TO_SINGLE);
               }
               i++;
            } break;
            case VALUEX:                                    // X-Werte der Kurve [n] lesen
            case VALUEY:                                    // Y-Werte der Kurve [n] lesen
            case VALUELOCUS:
            case START_VALUE: case STEP_VALUE:              // Start und Step-Werte der Kurve [n] lesen
            case OFFSETY:                                   // Offset der Kurve [n] lesen
            {
               int nCurve = pControl[i].nNumber;
               pcurve = (CCurveLabel*) plc->GetLabel(nCurve-1);
               if (pcurve)
               {
                  int nIndex = MacroEditorDlg::GetIndex(pControl, nCurrentUntil, nCurve);
                  if (nIndex < pcurve->GetNumValues())      // Index muß kleiner sein
                  {
                     int nCommand = pControl[i].nCommand;
                     CString string;
                     int nLength = ExtractString(&line[nlineIndex], NUMERIC, (bDecimalComma) ? ",.+-e" : ".+-e", pControl, i, string);
                     if (string.GetLength() == 0) throw (int)IDS_ERROR_READ_FORMAT;
                     nlineIndex += nLength;
                     if (MacroEditorDlg::ExtractdValue((char*)LPCTSTR(string), bDecimalComma, dValue))
                     {
                        switch(nCommand)
                        {
                           case VALUEX:      pcurve->SetX_Value(nIndex, dValue); break;
                           case VALUEY:      pcurve->SetY_Value(nIndex, dValue); break;
                           case VALUELOCUS:  pcurve->SetLocusValue(nIndex, dValue); break;
                           case START_VALUE: pcurve->SetXOrigin(dValue); break;
                           case STEP_VALUE:  pcurve->SetXStep(dValue); break;
                           case OFFSETY:     pcurve->SetOffset(dValue); break;
                        }
                     }
                     else throw (int)IDS_ERROR_NUMBER_FORMAT;
                  }
                  else bOK = false;
               }
               i++;
            }  break;
            case NO_OF_VALUES+SET_VALUE:                    // Anzahl Kurvenwerte setzen
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  i++;
                  if (pControl[i].ptr) pcurve->SetSize(*((int*)pControl[i].ptr));
                  else
                  {
                     CCurveLabel * pcl = (CCurveLabel *)plc->GetLabel(pControl[i].nNumber-1);
                     if (pcl) pcurve->SetSize(pcl->GetNumValues());
                  }
               }
               i++;
            }  break;
            case DESCRIPTION+SET_VALUE:                     // Beschreibung [n] setzen
            case UNITX+SET_VALUE:                           // X-Einheit setzen
            case UNITY+SET_VALUE:                           // Y-Einheit setzen
            case UNITLOCUS+SET_VALUE:                       // Einheit des Ortskurvenwinkels setzen
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  const char * ptemp = GetCurveString(plc, pControl[i+1]);
                  if (ptemp)
                  {
                     if      (pControl[i].nCommand == (DESCRIPTION+SET_VALUE))
                        pcurve->SetDescription(ptemp);
                     else if (pControl[i].nCommand == (UNITX+SET_VALUE))
                        pcurve->SetXUnit(ptemp);
                     else if (pControl[i].nCommand == (UNITY+SET_VALUE))
                        pcurve->SetYUnit(ptemp);
                     else if (pControl[i].nCommand == (UNITLOCUS+SET_VALUE))
                        pcurve->SetLocusUnit(ptemp);
                     i++;
                  }
               }
               i++;
            }  break;
            case EQUAL:
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i-1].nNumber-1);
               if (pcurve)
               {
                  switch (pControl[i-1].nCommand)
                  {
                     case DESCRIPTION+SET_VALUE: pcurve->SetDescription((char*)pControl[i-1].ptr); break;
                     case UNITX+SET_VALUE:       pcurve->SetXUnit((char*)pControl[i-1].ptr); break;
                     case UNITY+SET_VALUE:       pcurve->SetYUnit((char*)pControl[i-1].ptr); break;
                     case UNITLOCUS+SET_VALUE:   pcurve->SetLocusUnit((char*)pControl[i-1].ptr); break;
                     case VALUEX+SET_VALUE:      pcurve->SetX_Value(MacroEditorDlg::GetIndex(pControl, nCurrentUntil, pControl[i].nNumber), *(double*)pControl[i-1].ptr); break;
                     case VALUEY+SET_VALUE:      pcurve->SetY_Value(MacroEditorDlg::GetIndex(pControl, nCurrentUntil, pControl[i].nNumber), *(double*)pControl[i-1].ptr); break;
                     case VALUELOCUS+SET_VALUE:  pcurve->SetLocusValue(MacroEditorDlg::GetIndex(pControl, nCurrentUntil, pControl[i].nNumber), *(double*)pControl[i-1].ptr); break;
                     case START_VALUE+SET_VALUE: pcurve->SetXOrigin(*(double*)pControl[i-1].ptr); break;
                     case STEP_VALUE+SET_VALUE:  pcurve->SetXStep(*(double*)pControl[i-1].ptr); break;
                     case OFFSETY+SET_VALUE:     pcurve->SetOffset(*(double*)pControl[i-1].ptr); break;
                  }
               }
               i++;
            } break;
            case VALUEX+SET_VALUE:                          // X-Werte setzen
            case VALUEY+SET_VALUE:                          // Y-Werte setzen
            case VALUELOCUS+SET_VALUE:                      // Ortskurven-Werte setzen
            case START_VALUE+SET_VALUE:
            case STEP_VALUE+SET_VALUE:
            case OFFSETY+SET_VALUE:
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  int nIndex = MacroEditorDlg::GetIndex(pControl, nCurrentUntil, pControl[i].nNumber);
                  double dtemp = GetCurveValue(plc, pControl[++i], nIndex);
                  switch (pControl[i-1].nCommand)
                  {
                     case VALUEX+SET_VALUE:      pcurve->SetX_Value(nIndex, dtemp); break;
                     case VALUEY+SET_VALUE:      pcurve->SetY_Value(nIndex, dtemp); break;
                     case VALUELOCUS+SET_VALUE:  pcurve->SetLocusValue(nIndex, dtemp); break;
                     case START_VALUE+SET_VALUE: pcurve->SetXOrigin(dtemp); break;
                     case STEP_VALUE+SET_VALUE:  pcurve->SetXStep(dtemp); break;
                     case OFFSETY+SET_VALUE:     pcurve->SetOffset(dtemp); break;
                  }
               }
               i++;
            }  break;
            case INDEX:                                     // Index Setzen
            {
               pcurve = (CCurveLabel*) plc->GetLabel(pControl[i].nNumber-1);
               if (pcurve)
               {
                  int nIndex = MacroEditorDlg::GetIndex(pControl, nCurrentUntil, pControl[i].nNumber);
                  if (nIndex < pcurve->GetNumValues())      // Index muß kleiner sein
                  {
                     pcurve->SetX_Value(nIndex, (double) nIndex);
                  }
               }
               i++;
            } break;
            case STEP_BY:
               nlineIndex += pControl[i].nNumber;
               i++;
               if (nlineIndex < (int)strlen(line))               // if in range
                  break;                                    // go to next command
               // if not in range read new line
            case NEWMAKROLINE:                              // neue Zeile einlesen
            case NEWLINE:                                   // Zeile überlesen
            {
               for (int iLine=0; bOK && (iLine < pControl[i].nNumber); iLine++, nLine++)
                  bOK   = (pAr->ReadString(nType, line, STRING_LENGTH)!=EOF) ? true : false;
               nlineIndex = 0;
               nOldIndex = 0;
               i++;
            }  break;
            case FIND_STRING:
            {
               char *pstr = NULL;
               while (!pstr)
               {
                   pstr = strstr(line, (const char*)pControl[i].ptr);
                   if (pstr)
                   {
                      nlineIndex = (pstr - line) + strlen((const char*)pControl[i].ptr);
                      break;
                   }
                   else
                   {
                      bOK = (pAr->ReadString(nType, line, STRING_LENGTH)!=EOF) ? true : false;
                      nLine++;
                      nlineIndex = 0;
                      nOldIndex = 0;
                      if (!bOK) break;
                   }
               }
               i++;
            } break;
            case DECIMAL_POINT:                             // Umschalten auf Dezimal Punkt
            case DECIMAL_COMMA:                             // Umschalten auf Dezimal Komma
               bDecimalComma = (pControl[i].nCommand == DECIMAL_COMMA) ? true : false;
               i++;
               break;
            case SOMECHARACTER:                             // Zeichen suchen
            {
               for (; (line[nlineIndex] != '\n') && (line[nlineIndex] != 0); nlineIndex++)
               {
                  if (nlineIndex >= STRING_LENGTH) throw (int)IDS_ERROR_ARRAY_BOUNDS;
                  if (line[nlineIndex] == pControl[i].nNumber) {nlineIndex++; break;}
               }
               i++;
            } break;
            case REPEAT:                                    // Anfang einer Schleife
            {
               nCurrentUntil = pControl[i].nNumber;
               if (pControl[nCurrentUntil].nCommand == UNTIL)
               {
                  for (int j=0; j<pControl[nCurrentUntil+1].nNumber; j++) // nach registrierter Kurven suchen
                  {
                     pcurve = (CCurveLabel*) plc->GetLabel(pControl[nCurrentUntil+2+j].nCommand-1);
                     if (pcurve)
                     {
                        if (pcurve->GetNumValues() == 0)    // wenn Autosize
                           pcurve->SetSize(1);              // Größe mit 1 initialisieren
                        pControl[nCurrentUntil+2+j].nNumber = 0;// Werteindex mit 0
                     }
                  }
               }
               i++;
            }  break;
            case UNTIL:                                     // Ende einer Schleife
            {
               bStopLoop = false;
               if (bOK)
               {
                  if (pControl[i+1].nCommand == NO_OF_LOOPS)// Lesewiederholungen
                  {
                     SendAndRemoveCurves(pMainWnd, plc);
                     AllocateNewCurves(pMainWnd, plc, pControl[i+2].nNumber);
                     if (pControl[i+2].nNumber != -1)       // steht der Controllfeldindex in nNumber
                     {                                      // steht die Anzahl der Schleifendurchläufe fest
                        if (pControl[i+2].nCommand < (pControl[pControl[i+2].nNumber].nNumber-1))
                            (*((int*)&pControl[i+2].nCommand))++;
                        else
                            bStopLoop = true;
                     }
                  }
                  else                                      // Schleife über Kurvenwerte
                  {
                     for (j=0; j<pControl[i+1].nNumber; j++)// nach registrierter Kurven suchen
                     {
                        pcurve = (CCurveLabel*) plc->GetLabel(pControl[i+2+j].nCommand-1);
                        if (pcurve)
                        {
                           if (pControl[i+1].nCommand == AUTOSIZE)// Bei Autosize die Kurvengröße erhöhen
                           {
                              if (strlen(line) == 0)
                              {
                                  bStopLoop = true;
                                  break;
                              }
                              else if (pcurve->SetSize(pcurve->GetNumValues()+1))
                                 pControl[i+2+j].nNumber++;
                           }
                           else if (pControl[i+1].nCommand == NO_OF_VALUES)
                           {
                              if (pControl[i+2+j].nNumber < (pcurve->GetNumValues()-1))
                              {                                   // Nur erhöhen, wenn noch kleiner
                                 pControl[i+2+j].nNumber++;
                              }
                              else 
                              {
                                 bStopLoop = true;
                              }
                           }
                        }
                     }
                  }
               }
               else bStopLoop = true;
               if (bStopLoop) 
               {
                  i += pControl[i+1].nNumber+2;             // hinter die Schleife springen
               }
               else
               {
                  i  = pControl[i  ].nNumber+1;             // zum ersten Befehl hinter Repeat springen
               }
            } break;
            default:
            {
               ASSERT(false);
               throw (int) IDS_ERROR_UNKNOWN;
               break;
            }
         }
         nOldIndex = nlineIndex;
      }
      SendAndRemoveCurves(pMainWnd, plc);
      pMainWnd->m_pLabel = NULL;
   }
   catch (int nError)
   {
      REPORT("Error: %d", nError);
      CString strCmd;
      CString strError;
      strError.LoadString(nError);
      MacroEditorDlg::GetCommand(&pControl[i], strCmd);
      CString *pStr = new CString;
      pStr->Format(IDS_ERROR_READING_TEXT_CURVE,
                   strError,
                   LPCTSTR(pFile->GetFileName()), 
                   nLine, nlineIndex,
                   line, 
                   strCmd);
      pMainWnd->PostMessage(WM_ENDTHREAD, (WPARAM)pStr, (LPARAM)pParam);
      nReturn = nError;
   }
   SAFE_DELETE(pcLSMB);
   SAFE_DELETE(plc);
   for (i=0; i<MAX_CONTROL; i++)
   {
      SAFE_DELETE(pControl[i].ptr);
   }
   pMainWnd->m_pLabel = NULL;
   loadArchive.Close();
   pMainWnd->PostMessage(WM_ENDTHREAD, 0, (LPARAM)pParam);
   return 0;
}

void CCARA2DVDoc::SendAndRemoveCurves(CMainFrame *pMainWnd, CLabelContainer* plc)
{
   CCurveLabel *pcurve;
   int i;
   for (i=1; (pcurve = (CCurveLabel*) plc->GetLabel(0))!= NULL; i++)
   {
      if (pcurve->IsEmpty())
      {
         char text[16];
         wsprintf(text, "C(%d)", i);
         pcurve->SetText(text);
      }
      if (pcurve->IsValid())
      {
         pMainWnd->PostMessage(WM_NEWCURVE, (LPARAM)pcurve, plc->GetCount() == 1);
      }
      else
      {
         delete pcurve;
      }
      plc->RemoveLabel(pcurve);
   }
}

void CCARA2DVDoc::AllocateNewCurves(CMainFrame *pMainWnd, CLabelContainer *plc, int nNoOfCurves)
{
   int j;
   CCurveLabel *pcurve;
   for (j=0; j<nNoOfCurves; j++)
   {
      pcurve = new CCurveLabel;
      switch (pMainWnd->m_nPrecision)
      {
         case HIGH_PRECISION:   pcurve->SetFormat(CF_DOUBLE); break;
         case LOW_PRECISION:    pcurve->SetFormat(CF_SHORT);  break;
         default:
         case NORMAL_PRECISION: pcurve->SetFormat(CF_FLOAT);  break;
      }
      if (!pcurve) throw (int)IDS_ERROR_ALLOCATION;
      plc->InsertLabel(pcurve);
      pcurve->SetWriteProtect(false);
      pcurve->SetColor(0xFFFFFFFF);
   }
}

/******************************************************************************
* Name       : GetCurveString                                                 *
* Definition : static char* GetCurveString(CLabelContainer *, SControl);      *
* Zweck      : Ermitteln der Kurventexte für die Zuweisung an weitere Kurven  *
* Aufruf     : GetCurveString(plc, Control);                                  *
* Parameter  :                                                                *
* plc     (E): Container für die Kurven.                    (CLabelContainer*)*
* Control (E): Steuerungeintrag für die Lesesteuerung.      (SControl)        *
* Funktionswert : Textstring                                (char*)           *
******************************************************************************/
const char* CCARA2DVDoc::GetCurveString(CLabelContainer *plc, SControl Control)
{
   CCurveLabel * pcl = (CCurveLabel *)plc->GetLabel(Control.nNumber-1);
   switch(Control.nCommand)
   {
      case DESCRIPTION:
         return pcl->GetText();
      case UNITX:
         return pcl->GetXUnit();
      case UNITY:
         return pcl->GetYUnit();
      case UNITLOCUS:
         return pcl->GetLocusUnit();
      default:
         return (const char*)Control.ptr;
   }
}
/******************************************************************************
* Name       : GetCurveValue                                                  *
* Definition : double GetCurveValue(CLabelContainer*, SControl, int);         *
* Zweck      : Lesen eines indizierten Kurvenwertes aus einer anderen Kurve.  *
*              Dieser Kurvenwert muß vorher schon belegt worden sein !!       *
* Aufruf     : GetCurveValue(plc, Control, nIndex);                           *
* Parameter  :                                                                *
* plc     (E): Container mit allen zu lesenden Kurven     (CLabelContainer*)  *
* Control (E): Kontrolstruktur mit dem Index der Kurve Kommando   (SControl)  *
*              für das Lesen des Wertes.                                      *
* nIndex  (E): Index des Kurvenwertes.                                 (int)  *
* Funktionswert : gelesener Kurvenwert                              (double)  *
******************************************************************************/
double CCARA2DVDoc::GetCurveValue(CLabelContainer *plc, SControl Control, int nIndex)
{
   CCurveLabel * pcl = (CCurveLabel *)plc->GetLabel(Control.nNumber-1);
   switch(Control.nCommand)
   {
      case VALUEX:      return pcl->GetX_Value(nIndex);
      case VALUEY:      return pcl->GetY_Value(nIndex);
      case VALUELOCUS:  return pcl->GetLocusValue(nIndex);
      case START_VALUE: return pcl->GetXOrigin();
      case STEP_VALUE:  return pcl->GetXStep();
      case OFFSETY:     return pcl->GetOffset();
      default:
         if (Control.ptr) return *((double*)Control.ptr);
         return 0.0;
   }
}
/******************************************************************************
* Name       : ExtractString                                                  *
* Definition : int ExtractString(char*,int,char*,SControl*,int&,CString&);    *
* Zweck      : Extrahieren eines Teilzeichenkette aus einer Textzeile nach    *
*              Trennzeichen bzw. Zeichenart.                                  *
* Aufruf     : ExtractString(strInput,nWhat,pstrInclude,pControl,i,string);   *
* Parameter  :                                                                *
* strInput(E): Eingabetextzeile                                      (char*)  *
* nWhat   (E): Zeichenart (NUMERIC, ALPHA, ALPHA_STRING)             (int)    *
* pstrInclude (E): zusätzlich erlaubte Zeichen                       (char*)  *
* pControl(E): Kontrolstruktur mit Trennzeichen und Lesezustand  (SControl*)  *
*              für das Lesen der Textdatei.                                   *
* i      (EA): Index für das Kontrolstrukturfeld für das Lesen der   (int&)   *
*              Textdatei.                                                     *
* string  (A): Extrahierte Zeichenkette.                         (CString&)   *
* Funktionswert : Länge der extrahierten Zeichenkette                (int)    *
******************************************************************************/
int CCARA2DVDoc::ExtractString(char *strInput, int nWhat, char *pstrInclude, SControl *pControl, int &i, CString &string)
{
   char *pstrCut    = NULL;
   char strCut[]    = " ";
   int  nStartIndex = 0;
   if (pControl[i+1].nCommand == SOMECHARACTER)
   {
      strCut[0] = (char)pControl[++i].nNumber;
      pstrCut   = strCut;
      nWhat     = 0;
   }
   else
   {
      if (nWhat&NUMERIC)
      {
         for (;(strInput[nStartIndex] != 0)&&(strInput[nStartIndex] != '\n'); nStartIndex++)
         {
            if (iswdigit(strInput[nStartIndex])!=0) break;
            if (strInput[nStartIndex]=='.')         break;
            if (pstrInclude)                                   // weitere Zeichen eingeschlossen
            {
			   int j;
               for (j=0; pstrInclude[j] != 0; j++)             // bis zum Ende der Include Zeichen suchen
               {
                  if (pstrInclude[j] == strInput[nStartIndex]) break;
               }
               if (pstrInclude[j] != 0) break;                 // Include Zeichen gefunden, suchen Ende
            }
         }
      }
      if (nWhat&ALPHA_STRING)
      {  // wenn dahinter in dieser Zeile noch etwas gelesen werden soll,
         if ((pControl[i+1].nCommand!=NEWMAKROLINE)&&(pControl[i+1].nCommand!=NEWLINE))
            pstrCut = strCut;// wird das Trennzeichen ein Space-Zeichen
      }
   }
   return nStartIndex + MacroEditorDlg::ExtractString(&strInput[nStartIndex], nWhat, string, pstrCut, pstrInclude);
}

/******************************************************************************
* Name       : ReadWaveThread                                                 *
* Definition : UINT ReadWaveThread(void *);                                   *
* Zweck      : Lesen einer Wave-Datei in einem Thread.                        *
*              Nachrichten an das Parent-Window:                              *
*              WM_NEWCURVE mit wParam als Zeiger auf den neuen CCurveLabel    *
*              WM_ENDTHREAD mit wParam als Rückgabewert der Funktion.         *
* Aufruf     : ReadWaveThread(pParam);                                        *
* Parameter  :                                                                *
* pParam  (E): Zeiger auf CFile-Objekt                                (void*) *
* Funktionswert : 0 : Kurven gelesen.                                 (UINT)  *
*                 1 : Fehler kein CFile-Objekt übergeben.                     *
*                 2 : Kein Main-Window vorhanden.                             *
******************************************************************************/
UINT CCARA2DVDoc::ReadWaveThread(void * pParam)
{
   CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
   if (!pMainWnd)
   {
      return 2;
   }
   if (!pParam)
   {
      pMainWnd->PostMessage(WM_ENDTHREAD, 1);
      return 1;
   }
   CString  *pStr  = NULL;
   CFile    *pFile = (CFile*) pParam;
   CCurveLabel     *pcl = NULL;
   CCurveLabel     *pclRead[2] = {NULL, NULL};
   CLabelContainer *plc = NULL;
   CString        strTxt;
   SWaveFile      WaveFile;
   WAVEFORMATEX   pcmWF;
   long           nSampleLength;
   double         dTimeStep = 1;
   int            i, n, nSize, nReturn = 0;
   unsigned short wuRead;
   unsigned char  cuRead;
   WORD wSize;
   ETSLSMB_CLSMB sLSMB;
   sLSMB.nLines    = 1;                                        // 1 Textzeile
   sLSMB.nLoadSave = ETSLSMB_LOAD;                             // Ladeanimation anzeigen
   ::LoadString(AfxGetInstanceHandle(), IDS_LOAD_CURVEWAVEFILE, sLSMB.szText[0], ETSLSMB_STRINGLENGTH);
   CEtsLsmb cLSMB(&sLSMB);                                     // Fenster erzeugen und anzeigen

   try
   {
      plc = new CLabelContainer;
      if (plc == NULL) throw (int)IDS_ERROR_ALLOCATION;
      plc->SetDeleteContent(true);
      pMainWnd->m_pLabel = plc;

      pFile->Read(&WaveFile, sizeof(SWaveFile));
      if (strncmp(WaveFile.sRIFF, "RIFF", 4)         != 0) throw (int)IDS_ERROR_WAVE_FORMAT;
      if (strncmp(WaveFile.sWAVEfmt_, "WAVEfmt ", 8) != 0) throw (int)IDS_ERROR_WAVE_FORMAT;

      pFile->Read(&pcmWF, sizeof(WAVEFORMATEX));
      if (pcmWF.wFormatTag != WAVE_FORMAT_PCM) throw (int)IDS_ERROR_WAVE_FORMAT;         // Nur für PCM-Format
      if ((pcmWF.nChannels < 1)||(pcmWF.nChannels > 2)) throw (int)IDS_ERROR_WAVE_FORMAT; // nur Mono oder Stereo
      if (pcmWF.nSamplesPerSec > 1) dTimeStep = 1000.0 / (double)pcmWF.nSamplesPerSec;
      pFile->Read(&wSize, sizeof(WORD));
      pFile->Read(&nSampleLength, sizeof(long));

      nSize = pcmWF.wBitsPerSample / pcmWF.nBlockAlign;

      nSize = nSampleLength / pcmWF.nBlockAlign;

      pcmWF.nBlockAlign /= pcmWF.nChannels;                      // Blockgröße berechnen
      CWaveParameter *pP = NULL;

      for (n=0; n<pcmWF.nChannels; n++)
      {
         pcl = new CCurveLabel;
         if (pcl)
         {
            pclRead[n] = pcl;
            strTxt.Format("Kanal %d", n+1);
            pcl->SetText((char*)LPCTSTR(strTxt));
            pcl->SetWriteProtect(false);
            pcl->SetXUnit("ms");
            pcl->SetYUnit("V");
            pcl->SetColor(0xFFFFFFFF);
            switch (pMainWnd->m_nPrecision)
            {
               case HIGH_PRECISION:   pcl->SetFormat(CF_SINGLE_DOUBLE); break;
               case LOW_PRECISION:    pcl->SetFormat(CF_SINGLE_SHORT);  break;
               default:
               case NORMAL_PRECISION: pcl->SetFormat(CF_SINGLE_FLOAT);  break;
               case WAVE_DIRECT:
               {
                  if (pP == NULL)
                  {
                     CString sFile = pFile->GetFileName();
                     pFile->Close();
                     pFile->Open(sFile, CFile::modeReadWrite);
                     HANDLE hFileMapping = ::CreateFileMapping(pFile->m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
                     if(hFileMapping != INVALID_HANDLE_VALUE)
                     {
                        pP = new CWaveParameter;
                        pP->hFile = pFile->m_hFile;
                        pFile->m_hFile = INVALID_HANDLE_VALUE;
                        pP->hFileMapping = hFileMapping;
                        pP->pData = (BYTE*)MapViewOfFile(pP->hFileMapping, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, 0);
                        pP->pWaveFile = (SWaveFile*)pP->pData;
                        pP->pWaveFmtEx = (WAVEFORMATEX*)&pP->pData[sizeof(SWaveFile)];
                     }
                  }
                  if (pP)
                  {
                     pcl->SetFormat(CF_ARBITRARY);
                     pcl->SetCrvParamFkt(CCurveLabel::WaveParamFkt, pP);
                     pP->nReference++;
                     pcl->m_nChannel = n;
                     pcl->AttachArray(&pP->pData[sizeof(SWaveFile)+sizeof(WAVEFORMATEX)+sizeof(WORD)+sizeof(long)], nSize, CF_ARBITRARY);
                     pcl->SetSorted(true);
                  }
               }break;
            }
            pcl->SetXStep(dTimeStep);
            pcl->SetXOrigin(0);
            if (pcl->SetSize(nSize)) plc->InsertLabel(pcl);
            else
            {
               delete pcl;
               throw (int)IDS_ERROR_ALLOCATION;
            }
         }
      }
      if (pMainWnd->m_nPrecision != WAVE_DIRECT)
      {
         if (pMainWnd->m_bSigned)
         {
            if (pcmWF.nBlockAlign == 1)
            {
               for (i=0; i<nSize; i++)
               {
                  for (n=0; n<pcmWF.nChannels; n++)
                  {
                     pFile->Read(&cuRead, 1);
                     pclRead[n]->SetY_Value(i, (double)((char)cuRead));
                  }
               }
            }
            else if(pcmWF.nBlockAlign == 2)
            {
               for (i=0; i<nSize; i++)
               {
                  for (n=0; n<pcmWF.nChannels; n++)
                  {
                     pFile->Read(&wuRead, 2);
                     pclRead[n]->SetY_Value(i, (double)((short)wuRead));
                  }
               }
            }
            else throw (int)IDS_ERROR_WAVE_FORMAT;
         }
         else
         {
            if (pcmWF.nBlockAlign == 1)
            {
               for (i=0; i<nSize; i++)
               {
                  for (n=0; n<pcmWF.nChannels; n++)
                  {
                     pFile->Read(&cuRead, 1);
                     pclRead[n]->SetY_Value(i, (double)(cuRead-128));
                  }
               }
            }
            else if(pcmWF.nBlockAlign == 2)
            {
               for (i=0; i<nSize; i++)
               {
                  for (n=0; n<pcmWF.nChannels; n++)
                  {
                     pFile->Read(&wuRead, 2);
                     pclRead[n]->SetY_Value(i, (double)(wuRead-32768));
                  }
               }
            }
            else throw (int)IDS_ERROR_WAVE_FORMAT;
         }
      }
      SendAndRemoveCurves(pMainWnd, plc);
      pMainWnd->m_pLabel = NULL;
   }
   catch (int nError)
   {
      pStr = new CString;
      pStr->LoadString(nError);
      nReturn = nError;
   }
   SAFE_DELETE(plc);
   pMainWnd->m_pLabel = NULL;
   pMainWnd->PostMessage(WM_ENDTHREAD, (WPARAM)pStr, (LPARAM)pParam);
   return nReturn;
}

void CCARA2DVDoc::SaveCurveAsWave(CArchive &ar, CSaveCurveAsWave *pDlg)
{
   ASSERT(pDlg != NULL);
   ASSERT(pDlg->m_pclLeftOut != NULL);

   SWaveFile wf;
   CString str;

   int       i, j=0, nValues = pDlg->m_nCurveValues;
   double    dOrigin1, dOrigin2, dGradient1, dGradient2;
   BYTE     *pByte   = NULL;
   long      lOut    = 0;
   bool      bStereo = false;
   bool      b8Bit   = ((pDlg->m_wfEX.nBlockAlign / pDlg->m_wfEX.nChannels)==1) ? true:false;
   int nWidth = b8Bit ? 8: 16;

   m_FileHeader.Init("WAV", "2001", 100);
   m_FileHeader.SetComment(NULL);
   str.Format(IDS_WAVE_FILE_COMMENT, AfxGetApp()->m_pszAppName, pDlg->m_wfEX.nSamplesPerSec, pDlg->m_wfEX.nChannels, nWidth);
   m_FileHeader.SetComment((char*)LPCTSTR(str)),
   m_FileHeader.CalcChecksum();

   UINT nOldFp = _controlfp(_PC_64, _MCW_PC);

   if (b8Bit)
   {
      dGradient1 = (double)254 / (pDlg->m_dCurve1Max - pDlg->m_dCurve1Min);
      dOrigin1   = (double)127 - dGradient1 * pDlg->m_dCurve1Max;
   }
   else
   {
      dGradient1 = (double)65534 / (pDlg->m_dCurve1Max - pDlg->m_dCurve1Min);
      dOrigin1   = (double)32767 - dGradient1 * pDlg->m_dCurve1Max;
   }

   ZeroMemory(&wf, sizeof(SWaveFile));
   strncpy((char*)&wf.sRIFF    , "RIFF"    , 4);
   strncpy((char*)&wf.sWAVEfmt_, "WAVEfmt ", 8);
   wf.nHeaderLength = sizeof(SWaveFile) - sizeof(long);
   wf.nFileLength   = pDlg->m_nSampleLength + sizeof(WAVEFORMATEX) + sizeof(SWaveFile) + sizeof(CFileHeader) - 8;

   ar.Write(&wf, sizeof(SWaveFile));
   ar.Write(&pDlg->m_wfEX, sizeof(WAVEFORMATEX));

   if (pDlg->m_wfEX.nChannels == 2)   // Stereo
   {
      bStereo = true;
      ASSERT(pDlg->m_pclRightOut != NULL);
      if (b8Bit)
      {
         dGradient2 = (double)254 / (pDlg->m_dCurve2Max - pDlg->m_dCurve2Min);
         dOrigin2   = (double)127 - dGradient2 * pDlg->m_dCurve2Max;
      }
      else
      {
         dGradient2 = (double)65534 / (pDlg->m_dCurve2Max - pDlg->m_dCurve2Min);
         dOrigin2   = (double)32767 - dGradient2 * pDlg->m_dCurve2Max;
      }
   }

   if (pDlg->m_nCurveCount > 1)
   {
      pByte = new BYTE[pDlg->m_wfEX.nBlockAlign * pDlg->m_nCurveValues];
   }

   long nBytes = 0;

   if (b8Bit)
   {
      char cValue;
      double dValue;
      for (i=0; i<nValues; i++)
      {
         dValue = pDlg->m_pclLeftOut->GetY_Value(i);
         cValue = CEtsDiv::Round(dOrigin1 + dGradient1 * dValue);
         ar.Write(&cValue, sizeof(BYTE));
         if (pByte) pByte[j++] = cValue;
         if (bStereo)
         {
            dValue = pDlg->m_pclRightOut->GetY_Value(i);
            cValue = CEtsDiv::Round(dOrigin2 + dGradient2 * dValue);
            if (pByte) pByte[j++] = cValue;
            ar.Write(&cValue, sizeof(BYTE));
         }
      }
   }
   else
   {
      short  nValue;
      double dValue;
      for (i=0; i<nValues; i++)
      {
         dValue = pDlg->m_pclLeftOut->GetY_Value(i);
         nValue = CEtsDiv::Round(dOrigin1 + dGradient1 * dValue);
         ar.Write(&nValue, sizeof(short));
         nBytes += sizeof(short);
         if (pByte) ((short*)pByte)[j++] = nValue;
         if (bStereo)
         {
            dValue = pDlg->m_pclRightOut->GetY_Value(i);
            nValue = CEtsDiv::Round(dOrigin2 + dGradient2 * dValue);
            ar.Write(&nValue, sizeof(short));
            nBytes += sizeof(short);
            if (pByte) ((short*)pByte)[j++] = nValue;
         }
      }
   }

   _controlfp(nOldFp, _MCW_PC);

   if (pDlg->m_nCurveCount > 1)
   {
      long nSize = pDlg->m_wfEX.nBlockAlign * pDlg->m_nCurveValues;
      for (i=1;i<pDlg->m_nCurveCount; i++)
      {
         ar.Write(pByte, nSize);
         nBytes += nSize;
      }
   }
   if (pDlg->m_bConcat1stValue)
   {
      if (pByte)
      {
         ar.Write(pByte, pDlg->m_wfEX.nBlockAlign);
         nBytes += pDlg->m_wfEX.nBlockAlign;
      }
      else
      {
         ar.Write(&lOut, pDlg->m_wfEX.nBlockAlign);
         nBytes += pDlg->m_wfEX.nBlockAlign;
      }
   }

   ASSERT(nBytes == pDlg->m_nSampleLength);

   WriteFileHeader(ar);

   if (pByte) delete[] pByte;
}

void CCARA2DVDoc::ReleaseThreadFile()
{
   if (m_pFile)
   {
      ReleaseFile(m_pFile, (m_pFile->m_hFile == INVALID_HANDLE_VALUE) ? TRUE : FALSE);
      m_pFile = NULL;
   }
}

void CCARA2DVDoc::OnFuncMakroedit() 
{
   m_TextMacros.DoModal();	
}

/******************************************************************************
* Name       : SavePlotRects                                                  *
* Definition : void SavePlotRects();                                          *
* Zweck      : Speichern der in der Druckvorschau eingestellten Plotposition. *
* Aufruf     : SavePlotRects();                                               *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVDoc::SavePlotRects()
{
   int  count    = 0;
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &count, CLabelContainer::CountLabel);
   if (count != m_nPlotRects)
   {
      if (m_pPlotRects)
      {
         delete[] m_pPlotRects;
         m_pPlotRects = NULL;
         m_nPlotRects = 0;
      }
      if (count)
      {
         m_pPlotRects = new CRect[count];
         if (m_pPlotRects) m_nPlotRects = count;
      }
   }

   CRect *pRect = m_pPlotRects;
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pRect, SavePlotRects);
}

/******************************************************************************
* Name       : SavePlotRects                                                  *
* Definition : static int SavePlotRects(CLabel *pl, void *pParam);            *
* Zweck      : Speichern der View-Rechtecke der Plots aus dem PrintPreview in *
*              einem CRect-Feld.                                              *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Zeiger auf CRect Feld                               (*CRect[]) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::SavePlotRects(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      pl->SetPreviewMode(false);
      pl->SetDrawPickPoints(false);
      CRect **pRects = (CRect**) pParam;
      if (pl->IsVisible()) **pRects = pl->GetRect();
      else                 **pRects = CRect(0,0,0,0);
      (*pRects)++;
   }
   return 0;
}

/******************************************************************************
* Name       : RestorePlotRects                                               *
* Definition : bool RestorePlotRects();                                       *
* Zweck      : Wiederherstellen der zuvor in der Druckvorschau eingestellten  *
*              Position der Plots. Label der Druckvorschau wieder sichtbar    *
*              machen für den Ausdruck oder die Vorschau.                     *
* Aufruf     : RestorePlotRects();                                            *
* Parameter  :                                                                *
* Funktionswert : Plotposition konnte wiederhergestellt werden       (bool)   *
*                 (true, false)                                               *
******************************************************************************/
bool CCARA2DVDoc::RestorePlotRects(CDC *pDC, CRect rcMax)
{
   int  i, count = 0;
   bool bReturn  = true;
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &count, CLabelContainer::CountLabel);

   if (count != m_nPlotRects) bReturn = false;
   
   for (i=0; i<m_nPlotRects; i++)                              // Plotrects überprüfen
   {
      if (m_pPlotRects[i].IsRectEmpty())
      {
         ProcessLabel prl;
         prl.pl   = NULL;
         prl.nPos = i;
         m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &prl, CLabelContainer::GetLabelFromIndex);
         if (prl.pl && !prl.pl->IsVisible()) continue;
         bReturn = false;
         break;
      }
   }

   CRect *pRect = m_pPlotRects;
   m_Container.ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), bReturn ? &pRect : NULL, RestorePlotRects);
   if (bReturn && (pDC != NULL)) bReturn = LabelsAreInPrintRect(pDC, rcMax);
   return bReturn;
}

/******************************************************************************
* Name       : RestorePlotRects                                               *
* Definition : static int RestorePlotRects(CLabel *pl, void *pParam);         *
* Zweck      : Wiederherstellen der zuvor gespeicherten View-Rechtecke der    *
*              Plots im PrintPreview.                                         *
* Aufruf     : Als Callbackfunktion in der Funktion ProcessWithLabels         *
* Parameter  :                                                                *
* pl     (E) : PlotLabel des Dokumentes !!                      (CPlotLabel*) *
* pParam (EA): Struktur ProcessLabel                                          *
*  pl    (A) : PlotLabel, aus dem die Kurve entfernt wurde          (CLabel*) *
*  nPos  (A) : Position der Kurve in dem Plot                           (int) *
* Funktionswert : RückgabeFlags                                         (int) *
* 0               = weitersuchen,                                             *
* STOP_PROCESSING = nicht weitersuchen                                        *
* DELETE_LABEL    = diesen Label löschen                                      *
******************************************************************************/
int CCARA2DVDoc::RestorePlotRects(CLabel *pl, void *pParam)
{
   pl->ResetPickStates();
   pl->SetPreviewMode(true);
   pl->SetDrawPickPoints(true);
   if (pParam)
   {
      CRect **pRects = (CRect**) pParam;
      CRect Rect = **pRects;
      ((CPlotLabel*)pl)->SetRect(Rect);
      (*pRects)++;
   }
   return 0;
}

void CCARA2DVDoc::SetTitle(LPCTSTR lpszTitle) 
{
   CCARADoc::SetTitle(lpszTitle);
}

bool CCARA2DVDoc::Undo(CUndoParams *pUps, CLabel *pl, LPARAM lHint)
{
   switch(pUps->nCmd)
   {
      case ID_EDIT_CLEAR:
      {
         if      (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            if (pUps->pFrom && pUps->pFrom->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
            {
               CPlotLabel *ppl = InsertCurve((CCurveLabel*) pl, (CPlotLabel*) pUps->pFrom, MAKELONG(pUps->nPos, 0));
               UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO|lHint, ppl);
               return true;
            }
            else
            {
               CPlotLabel *ppl = InsertCurve((CCurveLabel*) pl);
               if (ppl) UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO|lHint, ppl);
               return true;
            }
         }
         else if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
         {
            InsertPlot((CPlotLabel*) pl, pUps->nPos);
            UpdateAllViews(NULL, UPDATE_INSERT_LABEL|lHint, pl);
            return true;
         }
      } break;
      case ID_EDIT_PASTE:
      if (pUps->pLabel && pUps->pTo)
      {
         if      (pUps->pLabel->IsKindOf(RUNTIME_CLASS(CCurveLabel)) && pUps->pTo->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
         {
            if (((CPlotLabel*)pUps->pTo)->RemoveCurve((CCurveLabel*)pUps->pLabel, pUps->nPos))
            {
               m_nInvalidStates |= EDIT_STATES;
               UpdateAllViews(NULL, UPDATE_DELETE_LABEL|lHint, pUps->pLabel);
               delete pUps->pLabel;
               UpdateAllViews(NULL, UPDATE_DELETE_LABEL_FROM|lHint, pUps->pTo);
               return true;
            }
         }
         else if (pUps->pLabel->IsKindOf(RUNTIME_CLASS(CPlotLabel)) && pUps->pTo->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         {
            int nPos;
            RemovePlot((CPlotLabel*) pUps->pLabel, nPos);
            UpdateAllViews(NULL, UPDATE_DELETE_LABEL|lHint, pUps->pLabel);
            delete pUps->pLabel;
            return true;
         }
      } break;
      case LAST_CMD_MOVE:
      {
         if (pUps->pLabel && pUps->pLabel->IsKindOf(RUNTIME_CLASS(CCurveLabel)) && 
             pUps->pFrom  && pUps->pFrom->IsKindOf(RUNTIME_CLASS(CPlotLabel)) &&
             pUps->pTo    && pUps->pTo->IsKindOf(RUNTIME_CLASS(CPlotLabel)) )
         {
            int nDummy;
            ((CPlotLabel*)pUps->pTo)->RemoveCurve((CCurveLabel*) pUps->pLabel, nDummy);
            UpdateAllViews(NULL, UPDATE_DELETE_LABEL_FROM|lHint, pUps->pTo);
            ((CPlotLabel*)pUps->pFrom)->InsertCurve((CCurveLabel*) pUps->pLabel, false, MAKELONG(pUps->nPos, 0));
            UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO|lHint, pUps->pFrom);
            return true;
         }
      }
   }
   return false;
}

// Liest eine Wavedatei direkt bei Programmstart
void CCARA2DVDoc::StartReadWaveThread()
{
   CCARA2DVApp  *pApp  = (CCARA2DVApp*) AfxGetApp();
   CMainFrame   *pMain = (CMainFrame*) pApp->m_pMainWnd;
   if (m_pFile && pMain)
   {
      pMain->m_pThread = AfxBeginThread(CCARA2DVDoc::ReadWaveThread, m_pFile, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
      if (pMain->m_pThread)
      {
         pMain->m_pThread->m_bAutoDelete = TRUE;
         pMain->m_pThread->ResumeThread();
      }
   }
}

void CCARA2DVDoc::OnNocecksum() 
{
   if (AfxMessageBox("Disable Checksum Examination ?", MB_YESNO|MB_ICONQUESTION, 0)==IDYES)
   {
      m_nTextMacro = NO_CHECKSUM;
   }
}
// Convertierungsfunktion für die alten Dateien

/*
int CCARA2DVDoc::ConvertLabel(CLabel *pl, void *)
{
   pl->SetVisible(true);
   if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
   {
      pl->SetDrawPickPoints(false);
   }
   else
   {
      pl->SetDrawPickPoints(true);
   }
   return 0;
}
*/

BOOL CCARA2DVDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
   BEGIN("OnOpenDocument");
   BOOL bReturn = false;
   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
   if (pApp->m_bEditMode)
   {
      return bReturn;
   }
   m_bDeleteTempFile = (pApp->m_CallingWindowwParam == TARGET_FUNCTION_CHANGEABLE) ? false : true;

   m_bLoadDocument = true;
   bReturn = CCARADoc::OnOpenDocument(lpszPathName);
   m_bLoadDocument = false;

	return bReturn;
}

int CCARA2DVDoc::RemoveReferenceCurve(CLabel *pl, void *pParam)
{
   ASSERT(pParam != NULL);
   ASSERT(pl != NULL);
   ASSERT_KINDOF(CPlotLabel, pl);
   if (pParam == (void*)(((CPlotLabel*)pl)->GetReferenceCurve()))
   {
      ((CPlotLabel*)pl)->SetReferenceCurve(NULL);
   }
   return 0;
}

LRESULT CCARA2DVDoc::OnUserOptUpdate(WPARAM wParam, LPARAM lParam)
{
   char szTempPath[MAX_PATH*2];
   CFile file;
   CArchive ar(&file, CArchive::load);
   GetTempPath( MAX_PATH*2, szTempPath );                      // Pfad der Übergabedatei basteln
   long pArr[3] = {wParam,lParam,0};
   char *pszArr = (char*)pArr;
   strcat(szTempPath, pszArr);
   strcat(szTempPath, ".tmp");

   TRY
   {
      if (file.Open(szTempPath, CFile::modeRead|CFile::shareDenyWrite))
      {
         ar.m_pDocument = this;
         if (file.GetLength() > 0)
         {
            Serialize(ar);
         }
      }
      if (file.m_hFile != (HANDLE)0xffffffff)
      {
         ar.Close();
         file.Close();
      }   
      ::DeleteFile((LPCTSTR)szTempPath);
   }
   CATCH_ALL(e)
   {
      ReportSaveLoadException(szTempPath, e, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
      e->Delete();
   }
   END_CATCH_ALL

   if (file.m_hFile != (HANDLE)0xffffffff)
   {
      ar.Close();
      file.Close();
   }   

   return 1;
}

int CCARA2DVDoc::FindPlotOrCurveByID(CLabel *pl, void *pParam)
{
   if (pl && pParam)
   {
      ProcessLabel *plt = (ProcessLabel*) pParam;
      if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
      {
         CPlotLabel   *ppl = (CPlotLabel*) pl;
         if (plt->nPos == ppl->GetPlotID())
         {
            plt->pl = pl;
            return STOP_PROCESSING;
         }
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel   *pcl = (CCurveLabel*) pl;
         if (plt->nPos == pcl->GetTargetFuncNo())
         {
            plt->pl = pl;
            return STOP_PROCESSING;
         }
      }
   }
   return 0;
}

#ifdef ETS_OLE_SERVER
COleServerItem* CCARA2DVDoc::OnGetEmbeddedItem()
{
   BEGIN("CCARADoc::OnGetEmbeddedItem");
	COleServerSrvrItem* pItem = new COleServerSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

void CCARA2DVDoc::SetModifiedFlag(BOOL bModified)
{
   CCARADoc::SetModifiedFlag(bModified);
   m_bOleChanged = bModified ? true: false;
}

void CCARA2DVDoc::OnDeactivateUI(BOOL bUndoable)
{
/*
   if (m_bOleChanged)
   {
      UpdateAllItems(NULL);
      m_bOleChanged = false;
   }
*/
   CCARADoc::OnDeactivateUI(bUndoable);
}
void CCARA2DVDoc::OnDeactivate()
{
   /*
   if (m_bOleChanged)
   {
      UpdateAllItems(NULL);
      m_bOleChanged = false;
   }
   */
   CCARADoc::OnDeactivate();
}

void CCARA2DVDoc::OnSaveEmbedding(LPSTORAGE lpStorage)
{
   CCARADoc::OnSaveEmbedding(lpStorage);
}

#endif


void CCARA2DVDoc::OnFileLoadReadMacros()
{
   CString sTitle, sFilter= "ini-File|*.ini|";
   CFileDialog fd(TRUE, sFilter.Left(3), NULL, OFN_OVERWRITEPROMPT|OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_EXPLORER, LPCTSTR(sFilter), AfxGetApp()->m_pMainWnd);
   fd.m_ofn.nMaxCustFilter = 2;

   sTitle.LoadString(AFX_IDS_OPENFILE);
   fd.m_ofn.lpstrTitle     = LPCTSTR(sTitle);
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   if (fd.DoModal()==IDOK)
   {
      LPCTSTR sProfile  = theApp.m_pszProfileName;
      LPCTSTR sRegistry = theApp.m_pszRegistryKey;
      theApp.m_pszRegistryKey = NULL;
      sTitle = fd.GetPathName();
      theApp.m_pszProfileName = LPCSTR(sTitle);
      m_TextMacros.LoadFromRegistry();
      theApp.m_pszProfileName = sProfile;
      theApp.m_pszRegistryKey = sRegistry;
      if (m_TextMacros.m_hWnd)
      {
         m_TextMacros.ReloadMacroLists();
      }
   }
}

void CCARA2DVDoc::OnUpdateFileLoadReadMacros(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CCARA2DVDoc::OnFileSaveReadMacros()
{
   CString sTitle, sFilter= "ini-File|*.ini|";
   int nSlash = theApp.m_strPreviewHelpPath.ReverseFind('\\');
   if (nSlash == -1) nSlash = theApp.m_strPreviewHelpPath.GetLength();
   CFileDialog fd(FALSE, sFilter.Left(3), theApp.m_strPreviewHelpPath.Left(nSlash), OFN_OVERWRITEPROMPT|OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_EXPLORER, LPCTSTR(sFilter), AfxGetApp()->m_pMainWnd);
   fd.m_ofn.nMaxCustFilter = 2;

   sTitle.LoadString(AFX_IDS_SAVEFILE);
   fd.m_ofn.lpstrTitle     = LPCTSTR(sTitle);
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   if (fd.DoModal()==IDOK)
   {
      LPCTSTR sProfile  = theApp.m_pszProfileName;
      LPCTSTR sRegistry = theApp.m_pszRegistryKey;
      theApp.m_pszRegistryKey = NULL;
      sTitle = fd.GetPathName();
      theApp.m_pszProfileName = LPCSTR(sTitle);
      m_TextMacros.SaveToRegistry();
      theApp.m_pszProfileName = sProfile;
      theApp.m_pszRegistryKey = sRegistry;
   }
}

void CCARA2DVDoc::pp(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}
