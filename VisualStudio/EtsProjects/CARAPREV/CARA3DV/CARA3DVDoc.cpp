// CARA3DVDoc.cpp : Implementierung der Klasse CCARA3DVDoc
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "MainFrm.h"
#include "CARA3DVDoc.h"
#include "CARA3DVView.h"
#include "PlanGraph.h"
#include "PolarGraph.h"
#include "ETS3DGLRegKeys.h"

#include "..\ColorLabel.h"
#include "..\PreviewFileHeader.h"
#include "..\resource.h"

#ifdef ETS_OLE_SERVER
#include "SrvrItem.h"
#endif

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
// CCARA3DVDoc

IMPLEMENT_DYNCREATE(CCARA3DVDoc, CCARADoc)

BEGIN_MESSAGE_MAP(CCARA3DVDoc, CCARADoc)
	//{{AFX_MSG_MAP(CCARA3DVDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVDoc Konstruktion/Destruktion
#define CRC_POSITION       12
#define READ_TESTDATA      1
#define NO_CHECKSUM_TEST   2

#define REGKEY_DEFAULT_LIGHT         "Light%d"
#define REGKEY_DEFAULT_LEVELCOLORS   "LevelColors"
#define REGKEY_DEFAULT_DETAIL        "Detail"
#define REGKEY_DEFAULT_LIGHTING      "Lighting"
#define REGKEY_DEFAULT_INTERPOLATE   "Interpolate"
#define REGKEY_DEFAULT_SMOOTHSHADING "GLSmooth"
#define REGKEY_DEFAULT_OBJ_COLOR     "ObjectColor"
#define REGKEY_DEFAULT_GRAFICFONT    "GraficFont"
#define REGKEY_DEFAULT_GRIDPARAM     "Grid"
#define REGKEY_DEFAULT_AMBIENTCOLOR  "AmbientLight"
#define REGKEY_DEFAULT_PINS          "Pins"

#define CARA3D_FILE        "3DV"
#define PLANGRAPH_NETDATA  "NET"
#define PLANGRAPH_OBJECT   "PGO"
#define WATERFALL_OBJECT   "WFO"
#define DIR_PATTERN_OBJECT "DPO"

extern "C" __declspec(dllimport) int g_nFileVersion;

CCARA3DVDoc::CCARA3DVDoc()
{
   BEGIN("CCARA3DVDoc");
   m_pcsDocumentData = NULL;
   m_pDibLabel       = NULL;
   m_ppHeadLines     = NULL;
   m_pColorLabel     = NULL;
   m_p3DObject       = NULL;
   m_nHeadLines      = 0;
   m_bNormalBug      = BUGFIX_NORMALVECTOR_DEFAULT;
   m_LevelColor.SetDocument(this);
   m_Grid.SetDocument(this);
   m_Pins.SetDocument(this);
#ifdef ETS_OLE_SERVER
   m_bOleChanged       = false;
#endif
   Init();
}

void CCARA3DVDoc::Init()
{
   BEGIN("CCARA3DVDoc::Init");
   CAutoCriticalSection csAuto(m_pcsDocumentData);
   CWinApp *pApp = AfxGetApp();
   BYTE * byteptr=NULL;
   UINT   size;
   bool   bOk;
   ASSERT(NULL == m_pDibLabel);
   ASSERT(NULL == m_ppHeadLines);
   ASSERT(NULL == m_pColorLabel);
   ASSERT(NULL == m_p3DObject);
   m_bCullFace       = true;
   m_bLightTwoSided  = false;
   m_nPictureIndex   =     0;
   m_bInterpolate    = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_INTERPOLATE  , 0) != 0) ? true : false;
   m_bSmoothShading  = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_SMOOTHSHADING, 1) != 0) ? true : false;
   m_bLighting       = (pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_LIGHTING     , 1) != 0) ? true : false;
   m_nDetail         =  pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_DETAIL       , 0);
   if ((m_nDetail < 0) || (m_nDetail > 5)) m_nDetail = 0;      // Wertebereich prüfen

   HKEY hKey;
   char szRegKey[_MAX_PATH];
   wsprintf(szRegKey, "%s\\%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY, REGKEY_BUGFIX);
   if (::RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;
      if (::RegQueryValueEx(hKey, REGKEY_BUGFIX_NORMALVECTOR, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if (dwType== REG_DWORD) m_bNormalBug = (dwValue!=0) ? true : false;
      }
      RegCloseKey(hKey);
   }

   pApp->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_AMBIENTCOLOR, &byteptr, &size);
   bOk = false;
   if (byteptr)
   {
      if (size == sizeof(float)*4)
      {
         memcpy(m_pfAmbientColor, byteptr, size);
         bOk = true;
      }
      delete[] byteptr;
   }
   if (!bOk)
   {
      m_pfAmbientColor[0] = 0.2f;
      m_pfAmbientColor[1] = 0.2f;
      m_pfAmbientColor[2] = 0.2f;
      m_pfAmbientColor[3] = 1.0f;
   }

   for (int i=0; i<NO_OF_LIGHTS; i++)
   {
      char text[64];
      wsprintf(text, REGKEY_DEFAULT_LIGHT, i+1);
      pApp->GetProfileBinary(REGKEY_DEFAULT, text, &byteptr, &size);
      bOk = false;
      if (byteptr)
      {
         if (size == sizeof(Cara3DLight))
         {
            memcpy(&m_Light[i], byteptr, size);
            bOk = true;
         }
         delete[] byteptr;
      }
      if (!bOk)
      {
         InitLight(i);
         if (i != 0) m_Light[i].glLight.m_bEnabled = false;
      }
   }

   m_ObjectColor = pApp->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_OBJ_COLOR, RGB (0,0,255));
   pApp->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_GRAFICFONT , &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(LOGFONT)) m_Grid.SetFont(*((LOGFONT*)byteptr));
      delete[] byteptr;
   }
   pApp->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_GRIDPARAM , &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(C3DGrid::GridParam))
      {
         m_Grid.SetGridParam(*((C3DGrid::GridParam*)byteptr));
      }
      delete[] byteptr;
   }

   pApp->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_PINS, &byteptr, &size);
   if (byteptr)
   {
      if (size == (sizeof(bool)*NO_OF_PINTYPES))
      {
         memcpy(m_Pins.m_bShow, byteptr, size); 
      }
      delete[] byteptr;
   }

   pApp->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_LEVELCOLORS, &byteptr, &size);
   bOk = false;
   if (byteptr)
   {
      if ((size > 0) &&  (size <= sizeof(COLORREF)*4096))
      {
         m_LevelColor.SetColorMode(CV_INTERPOLATE);
         int nColors = size / sizeof(COLORREF);
         m_LevelColor.SetArraySize(nColors);
         m_LevelColor.SetColors((COLORREF*)byteptr, nColors);
         bOk = true;
      }
      delete[] byteptr;
   }
   if (!bOk) SetDefaultColors();
   m_pszTempFileName = NULL;
   m_bPrintInit      = true;
}

void CCARA3DVDoc::InitLight(int nLight)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::InitLight");
#endif
   ASSERT(nLight < NO_OF_LIGHTS);
   m_Light[nLight].fPhi                    = 135.0f;
   m_Light[nLight].fTheta                  =  80.0f;
   m_Light[nLight].fDistance               =  20.0f;
   m_Light[nLight].glLight.m_fSpotCutOff   =  30.0f;
   m_Light[nLight].glLight.m_fSpotExponent =  10.0f;
   m_Light[nLight].glLight.m_nLight        = GL_LIGHT0 + nLight;
   m_Light[nLight].glLight.SetSpecularColor(0.1f, 0.1f, 0.1f, 1.0f);
   m_Light[nLight].glLight.SetPosition(0, 2, 0, 0);
   m_Light[nLight].glLight.SetDirection(0, -0.5f, -1);
}

void CCARA3DVDoc::SetDefaultColors()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::SetDefaultColors");
#endif
   COLORREF C[6] = {RGB(0, 0, 0), RGB(0, 0, 255), RGB(0, 255, 255), RGB(0, 255, 0), RGB(255, 255, 0), RGB(255, 0, 0)};
   m_LevelColor.SetColorMode(CV_INTERPOLATE);
   m_LevelColor.SetArraySize(310);
   m_LevelColor.InterpolateColors(  0,  80, C[0], C[1]);
   m_LevelColor.InterpolateColors( 80, 150, C[1], C[2]);
   m_LevelColor.InterpolateColors(150, 200, C[2], C[3]);
   m_LevelColor.InterpolateColors(200, 250, C[3], C[4]);
   m_LevelColor.InterpolateColors(250, 309, C[4], C[5]);
   m_LevelColor.SetObjectColor(RGB(0,0,255));
}

CCARA3DVDoc::~CCARA3DVDoc()
{
   BEGIN("~CCARA3DVDoc");
   CWinApp *pApp = AfxGetApp();
 
   for (int i=0; i<NO_OF_LIGHTS; i++)
   {
      char text[64];
      wsprintf(text, REGKEY_DEFAULT_LIGHT, i+1);
      pApp->WriteProfileBinary(REGKEY_DEFAULT, text, (BYTE*)&m_Light[i], sizeof(Cara3DLight));
   }

   pApp->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_AMBIENTCOLOR, (BYTE*)m_pfAmbientColor  , sizeof(float)*4);
   const COLORREF *pCol = m_LevelColor.GetColorBuffer();
   pApp->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_LEVELCOLORS, (BYTE*)pCol    , sizeof(COLORREF)*m_LevelColor.GetNumColor());
   LOGFONT lf = m_Grid.GetFont();
   pApp->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_GRAFICFONT , (BYTE*)&lf     , sizeof(LOGFONT));
   C3DGrid::GridParam gp;
   m_Grid.GetGridParam(gp);
   pApp->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_GRIDPARAM , (BYTE*)&gp           , sizeof(C3DGrid::GridParam));
   pApp->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_PINS      , (BYTE*)m_Pins.m_bShow, sizeof(bool)*NO_OF_PINTYPES);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_DETAIL       , m_nDetail);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_INTERPOLATE  , m_bInterpolate   ? 1 : 0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_LIGHTING     , m_bLighting      ? 1 : 0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_SMOOTHSHADING, m_bSmoothShading ? 1 : 0);
   pApp->WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_OBJ_COLOR    , (int)m_ObjectColor);

   DeleteCriticalSectionOfDoc();
   CLabel::DeleteLOGArray();
}

BOOL CCARA3DVDoc::OnNewDocument()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::OnNewDocument");
#endif
#ifdef ETS_OLE_SERVER
   m_bOleChanged       = false;
#endif
   if (!CCARADoc::OnNewDocument()) return FALSE;
   CWinApp *pApp = AfxGetApp();
   if (pApp->m_pMainWnd)
   {
      ((CMainFrame*)pApp->m_pMainWnd)->InvalidateFrameContent();
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVDoc Serialisierung

void CCARA3DVDoc::Serialize(CArchive& ar)
{
   BEGIN("CCARA3DVDoc::Serialize");

   CAutoCriticalSection csAuto(m_pcsDocumentData);
   m_FileHeader.SetType(LABEL_FILE);
   CDibSection *pDib = NULL;
   if (m_pDibLabel) pDib = m_pDibLabel->DetachDIB();
   CCARADoc::Serialize(ar);
   if (m_pDibLabel && pDib) m_pDibLabel->AttachDIB(pDib);

//   ASSERT(false);

   if (ar.IsStoring())
   {
      C3DGrid::GridParam gp;
      int    nLights   = NO_OF_LIGHTS;
      double dMinLevel = m_LevelColor.GetMinLevel(),           // MinMaxLevel ermitteln
             dMaxLevel = m_LevelColor.GetMaxLevel();
      m_Grid.GetGridParam(gp);
      m_FileHeader.SetType(CARA3D_FILE);
      WriteFileHeader(ar);                                     // Fileheader Platzhalter schreiben
      m_FileHeader.CalcChecksum();                             // Checksumme zurücksetzen
      WriteLabelPositions(ar);                                 // feste Labelpositionen schreiben
      CLabel::ArchiveWrite(ar, &m_nDetail       , sizeof(m_nDetail));
      CLabel::ArchiveWrite(ar, &m_bInterpolate  , sizeof(m_bInterpolate));
      CLabel::ArchiveWrite(ar, &m_bSmoothShading, sizeof(m_bSmoothShading));
      CLabel::ArchiveWrite(ar, &m_bLighting     , sizeof(m_bLighting));
      CLabel::ArchiveWrite(ar, &nLights         , sizeof(int));
      CLabel::ArchiveWrite(ar, &m_Light[0]      , sizeof(Cara3DLight)*nLights);
      CLabel::ArchiveWrite(ar, &m_pfAmbientColor, sizeof(float)*4);
      CLabel::ArchiveWrite(ar, &m_nPictureIndex , sizeof(m_nPictureIndex));
      CLabel::ArchiveWrite(ar, &m_nProjection   , sizeof(m_nProjection));
      CLabel::ArchiveWrite(ar, &gp              , sizeof(gp));
      CLabel::ArchiveWrite(ar, &m_ObjTran       , sizeof(m_ObjTran));
      CLabel::ArchiveWrite(ar, &dMinLevel       , sizeof(double));
      CLabel::ArchiveWrite(ar, &dMaxLevel       , sizeof(double));
      CLabel::ArchiveWrite(ar, &m_bPrintInit    , sizeof(m_bPrintInit));
      m_LevelColor.WriteColors(ar);
      m_Pins.WritePins(ar, true);
      WriteFileHeader(ar, false);                              // Checksumme schreiben

      Write3DObject(ar);                                       // 3D-Objekt schreiben
   }
   else
   {
      int nVersion = m_FileHeader.GetVersion();                // Versionsnummer ermitteln
      unsigned long lChecksum = m_FileHeader.GetChecksum();
      if (!m_FileHeader.IsFileHeader())                        // kein FileHeader
      {
         CString sFile = ar.GetFile()->GetFileName();
         sFile.MakeUpper();
         if (sFile.Find(".TXT") != -1)
         {
            CPlanGraph *pPG = new CPlanGraph;
            m_p3DObject = pPG;
            m_p3DObject->SetDocument(this);
            ar.GetFile()->SeekToBegin();
            ar.Flush();
            if (pPG->ReadTextFile(ar))
            {
               m_p3DObject->CheckMinMax();
               if (pPG->IsFlat()) {m_bCullFace = false; m_bLightTwoSided = true; }
               else               {m_bCullFace = true;  m_bLightTwoSided = false;}
               UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
            }
            else
            {
               delete m_p3DObject;
               m_p3DObject = NULL;
               AfxThrowArchiveException(CArchiveException::badIndex, ar.GetFile()->GetFileName());
            }
         }
         else
         {
            m_p3DObject = new CPolarGraph;
            if (m_p3DObject)
            {
               m_p3DObject->SetDocument(this);
               ar.GetFile()->SeekToBegin();
               ar.Flush();
               CString strHeadline;
               if (!((CPolarGraph*)m_p3DObject)->ReadCaleData(ar, strHeadline))
               {
                  delete m_p3DObject;
                  m_p3DObject = NULL;
                  AfxThrowArchiveException(CArchiveException::badIndex, ar.GetFile()->GetFileName());
               }
               else
               {
                  m_nHeadLines  = 1;
                  CRect rc(0,0,1,1);
                  m_ppHeadLines = (CTextLabel**) new BYTE[sizeof(CTextLabel*)*m_nHeadLines];
                  if (m_ppHeadLines)
                  {
                     m_ppHeadLines[0] = new CTextLabel(&rc, (char*)LPCTSTR(strHeadline));
                     if (m_ppHeadLines[0])
                     {
                        m_Container.InsertLabel(m_ppHeadLines[0]);
                     }
                  }
                  else m_nHeadLines = 0;
                  DWORD dwAxes = m_Grid.GetAxes();
                  m_Grid.SetAxes(dwAxes|SHOW_POLAR_GRID);
                  m_bPrintInit = true;
                  UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
               }
            }
         }
      }
      else if (m_FileHeader.IsType(CARA3D_FILE))               // Dateityp 3DV
      {
         if (nVersion == 100)
         {
            ReadVersion1_00(ar);                               // alte Version lesen
            if (!m_FileHeader.IsValid(lChecksum))              // Checksumme überprüfen
               AfxThrowArchiveException(CRC_EXCEPTION, NULL);

            ((CCaraWinApp*)AfxGetApp())->ReadPrinterData10(ar);// Druckerdaten
         }
         else
         {
            C3DGrid::GridParam gp;
            int    nLights = 0;
            double dMinLevel, dMaxLevel;
            m_FileHeader.CalcChecksum();
            ReadLabelPositions(ar);                            // Feste Labelpositionen ermitteln
            CLabel::ArchiveRead(ar, &m_nDetail       , sizeof(m_nDetail));
            CLabel::ArchiveRead(ar, &m_bInterpolate  , sizeof(m_bInterpolate));
            CLabel::ArchiveRead(ar, &m_bSmoothShading, sizeof(m_bSmoothShading));
            CLabel::ArchiveRead(ar, &m_bLighting     , sizeof(m_bLighting));
            CLabel::ArchiveRead(ar, &nLights         , sizeof(int));
            if ((nLights > 0) && (nLights <= NO_OF_LIGHTS))
            {
               CLabel::ArchiveRead(ar, &m_Light[0]   , sizeof(Cara3DLight)*nLights);
               for (int i=nLights; i<NO_OF_LIGHTS; i++) m_Light[i].glLight.m_bEnabled = false;
            }
            else AfxThrowArchiveException(CArchiveException::badIndex, NULL);
            CLabel::ArchiveRead(ar, &m_pfAmbientColor, sizeof(float)*4);
            CLabel::ArchiveRead(ar, &m_nPictureIndex , sizeof(m_nPictureIndex));
            CLabel::ArchiveRead(ar, &m_nProjection   , sizeof(m_nProjection));
            CLabel::ArchiveRead(ar, &gp              , sizeof(gp));
            CLabel::ArchiveRead(ar, &m_ObjTran       , sizeof(m_ObjTran));
            CLabel::ArchiveRead(ar, &dMinLevel       , sizeof(double));
            CLabel::ArchiveRead(ar, &dMaxLevel       , sizeof(double));
            CLabel::ArchiveRead(ar, &m_bPrintInit    , sizeof(m_bPrintInit));
            m_LevelColor.ReadColors(ar);
            m_Pins.ReadPins(ar, true);

            m_Grid.SetGridParam(gp);

            if (!m_FileHeader.IsValid(lChecksum))              // Checksumme überprüfen
               AfxThrowArchiveException(CRC_EXCEPTION, NULL);

            Read3DObject(ar);                                  // 3D-Objekt lesen
            if (m_p3DObject!=NULL)
            {
               if (nVersion == 110) 
               {
                  double dNorm = m_p3DObject->GetNormFactor();
                  if (dNorm != 0) dNorm = 1.0 / dNorm;
                  dMinLevel *= dNorm;
                  dMaxLevel *= dNorm;
               }
               m_LevelColor.SetMinMaxLevel(dMinLevel, dMaxLevel);
               if (m_p3DObject->DoCalculatePhaseValues())
               {
                  m_p3DObject->CalculatePhaseValues();
               }
            }
         }
         UpdateAllViews(NULL, UPDATE_INSERT_DOC);
      }
      else if (m_FileHeader.IsType(PLANGRAPH_NETDATA))         // Daten vom Berechnungsprogramm
      {
         if (ReadNetFromCalcProg(ar))
         {
            m_bPrintInit = true;
            UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
         }
      }
      else if (m_FileHeader.IsType(DIR_PATTERN_OBJECT))
      {
         if (ReadDpoFromCalcProg(ar))
         {
            m_bPrintInit = true;
            UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
         }
      }
   }
}

bool CCARA3DVDoc::ReadLabelObjects1_00(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::ReadLabelObjects1_00");
#endif
   CObject * pObj = ar.ReadObject(RUNTIME_CLASS(CLabelContainer));// Labelobjekte lesen
   if (pObj)                                                      // sind Label vorhanden
   {
      if (pObj->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
      {
         CLabelContainer *pList = (CLabelContainer*) pObj;
         if (m_FileHeader.GetVersion() == 100)
         {
            pList->GetChecksum(&m_FileHeader);
         }
         m_Container.InsertList(pList);
         pList->RemoveAll();
         delete pList;
         return true;
      }
   }
   else
   {
      delete pObj;
      AfxThrowArchiveException(CArchiveException::generic, NULL);
   }
   return false;
}

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVDoc Diagnose

#ifdef _DEBUG
void CCARA3DVDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCARA3DVDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVDoc Befehle

void CCARA3DVDoc::DeleteContents()
{
   BEGIN("CCARA3DVDoc::DeleteContents");
   if (m_ppHeadLines)
   {
      delete[] (BYTE*)m_ppHeadLines;
      m_ppHeadLines = NULL;
      m_nHeadLines  = 0;
   }

   if (m_pDibLabel)
   {
      UpdateAllViews(NULL, UPDATE_DELETE_LABEL, m_pDibLabel);
   }
   if (!m_Container.IsEmpty())
   {
      m_Container.DeleteAll();
      m_pColorLabel = NULL;
      m_pDibLabel   = NULL;
   }

   {
      CAutoCriticalSection csAuto(m_pcsDocumentData);
      if (m_p3DObject)
      {
         delete m_p3DObject;
         m_p3DObject = NULL;
      }
   }

   m_Pins.DeletePins();

   if (m_pszTempFileName)
   {
      REPORT("Remove %s", m_pszTempFileName);
//#ifndef _DEBUG
      ::DeleteFile((LPCTSTR)m_pszTempFileName);   // für Debugzwecke nicht löschen
//#endif
      REPORT("Removed");
      delete[] m_pszTempFileName;
      m_pszTempFileName = NULL;
   }

   UpdateAllViews(NULL, UPDATE_DELETE_CONTENTS);
}

void CCARA3DVDoc::ReadLabelPositions(CArchive &ar)
{
   BEGIN("CCARA3DVDoc::ReadLabelPositions");
   long *pnPos;                                                // Positionen ermitteln
   bool bVersion1_00 = (m_FileHeader.GetVersion()==100) ? true : false;
   int i, nFixedLabel;
   if (bVersion1_00)
   {
      m_nHeadLines = 1;
      nFixedLabel  = 2;
   }
   else
   {
      CLabel::ArchiveRead(ar, &m_nHeadLines, sizeof(long));
      nFixedLabel = 2 + m_nHeadLines;
   }
   ProcessLabel prl;

   pnPos = new long[nFixedLabel];
   ASSERT(pnPos != NULL);

   if (m_nHeadLines)
   {
      m_ppHeadLines = (CTextLabel**) new BYTE[sizeof(CTextLabel*)*m_nHeadLines];
   }
   CLabel::ArchiveRead(ar, pnPos, sizeof(long)*nFixedLabel, true);// feste Labelpositionen aus der Datei lesen
   for (i=0; i< nFixedLabel; i++)
   {
      prl.pl   = NULL;
      if (pnPos[i] != -1)                                      // Position der 3D-Render-Labels
      {
         prl.nPos = pnPos[i];
         m_Container.ProcessWithLabels(NULL, &prl, CLabelContainer::GetLabelFromIndex);
         if (prl.pl)
         {
            if (bVersion1_00)
            {
               if (i==0)
               {
                  ASSERT_KINDOF(CTextLabel, prl.pl); 
                  m_ppHeadLines[0] = (CTextLabel*)prl.pl;
               }
               else
               {
                  ASSERT_KINDOF(CPictureLabel, prl.pl); 
                  m_pDibLabel  = (CPictureLabel*)prl.pl;
                  m_pDibLabel->m_bNoFile = true;
                  m_bPrintInit = (m_pDibLabel->GetRect().IsRectEmpty()) ? true : false;
               }
            }
            else
            {
               if (i==0)
               {
                  ASSERT_KINDOF(CPictureLabel, prl.pl); 
                  m_pDibLabel = (CPictureLabel*)prl.pl;
                  m_pDibLabel->m_bNoFile = true;
               }
               else if (i==1)
               {
                  ASSERT_KINDOF(CColorLabel, prl.pl); 
                  m_pColorLabel = (CColorLabel*)prl.pl;
               }
               else
               {
                  ASSERT_KINDOF(CTextLabel, prl.pl); 
                  m_ppHeadLines[i-2] = (CTextLabel*)prl.pl;
               }
            }
         }
      }
   }
   delete[] pnPos;
}

void CCARA3DVDoc::WriteLabelPositions(CArchive &ar)
{
   BEGIN("CCARA3DVDoc::WriteLabelPositions");
   long *pnPos;                                                // Positionen ermitteln
   int i, nFixedLabel = 2 + m_nHeadLines;
   ProcessLabel prl;

   pnPos = new long[nFixedLabel];
   ASSERT(pnPos != NULL);

   for (i=0; i<nFixedLabel; i++)
   {
      pnPos[i] = -1;
      if (i==0)      prl.pl = m_pDibLabel;
      else if (i==1) prl.pl = m_pColorLabel;
      else           prl.pl = m_ppHeadLines[i-2];
      if (prl.pl)
      {
         prl.nPos = 0;
         m_Container.ProcessWithLabels(NULL, &prl, CLabelContainer::GetIndexFromLabel);
         pnPos[i] = prl.nPos;                          // Position des Render-Dib-Label 
      }
   }

   CLabel::ArchiveWrite(ar, &m_nHeadLines, sizeof(long));
   CLabel::ArchiveWrite(ar, pnPos, sizeof(long)*nFixedLabel);
   delete[] pnPos;
}

bool CCARA3DVDoc::ReadVersion1_00(CArchive &ar)
{
   BEGIN("CCARA3DVDoc::ReadVersion1_00");
   struct tagViewParam
   {
      BOOL        bLightOn;                                    // Licht                (true, false)
      BOOL        bInterpolate;                                // Dreieckinterpolation (true, false)
      BOOL        bInterpolateColor;                           // Farben interpoliert  (true, false)
      int         nProjection;                                 // Projektion           (_GL_PERSPECTIVE, _GL_ORTHOGONAL, _GL_FRUSTUM)
      int         nDetail;                                     // Aufteilungsgrad der Dreiecke (0,...,5)
      int         nShading;                                    // Shading              (GL_FLAT, GL_SMOOTH)
   } ViewParam;

   struct tagLightParam
   {
      float       fLightAngleTheta;                            // Winkel Theta (0 - 180°)
      float       fLightAnglePhi;                              // Winkel Phi   (0 - 360°)
      float       fLightDistance;                              // Abstand der Lichtquelle
      float       fSpotCutOff;                                 // Öffnungswinkel der Lichtquelle
      float       fSpotExponent;                               // Bündelungsfaktor der Lichtquelle
      float       pfAmbient[4],                                // Umgebungslichtkomponente der Lichtquelle
                  pfDiffuse[4],                                // diffuse Lichtkomponente der Lichtquelle
                  pfSpecular[4];                               // Reflexionslichtkomponente der Lichtquelle
      float       fShininess;                                  // Glanz-Faktor
      float       pfPos[4];                                    // Position der Lichtquelle
      float       pfDirection[3];                              // Richtung der Lichtquelle
      float       fConstantAttenuation;                        // entfernungsabhängige Dämpfungs variablen
      float       fLinearAttenuation;                          // d = 1 / (const + lin*x + quad * x^2)
      float       fQuadraticAttenuation;
      int         nLight;                                      // Lichtnummer (1,..,8)
   } LightParam;

   struct tagObjectParam
   {
      float       fAmbient;                                    // Umgebungslichtkomponente bei Objektbeleuchtung
      float       fDiffuse;                                    // Diffuse Lichtkomponente bei Objektbeleuchtung
      float       fSpecular;                                   // Glanz Lichtkomponente bei Objektbeleuchtung
      COLORREF    ObjectColor;                                 // Farbe des Objektes
      double      dAngle_Theta;                                // Drehwinkel Theta
      double      dAngle_Phi;                                  // Drehwinkel Phi
      CVector     ObjectTranslation;                           // Objektverschiebung
      CVector     ObjectAnchor;                                // Drehpunkt des Objektes
      CVector     ObjectZoom;                                  // Vergrößerung des Objektes
      bool        bCalculateColor;                             // Zeigt an, ob die Farben innerhalb des Dreieckgenerators neu berechnet werden sollen
   } ObjectParam;

   DWORD    dwAxes;

   CLabel::LoadGDIObjects(ar);
   m_FileHeader.CalcChecksum();                                // Checksumme zurücksetzen
   ASSERT(NULL == m_p3DObject);
   CPlanGraph *pPG = new CPlanGraph(); 
   if (pPG)
   {
      m_p3DObject = pPG;                             // Object muß CPlanGraph sein !!
      m_p3DObject->SetDocument(this);
      pPG->ReadNetData(ar);
      pPG->ReadUnits(ar);
      m_LevelColor.ReadColors(ar);
      COLORREF color;
      CLabel::ArchiveRead(ar, &color, sizeof(COLORREF)   , true);
      ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(color);
      CLabel::ArchiveRead(ar, &ViewParam     , sizeof(ViewParam)  , true);
      double dMinLevel, dMaxLevel;
      CLabel::ArchiveRead(ar, &dMinLevel     , sizeof(double)     , true);
      CLabel::ArchiveRead(ar, &dMaxLevel     , sizeof(double)     , true);
      CLabel::ArchiveRead(ar, &LightParam    , sizeof(LightParam) , true);
      CLabel::ArchiveRead(ar, &ObjectParam   , sizeof(ObjectParam), true);
      CLabel::ArchiveRead(ar, &dwAxes        , sizeof(DWORD)      , true);
      m_Grid.SetAxes(dwAxes);
      CLabel::ArchiveRead(ar, &m_nPictureIndex , sizeof(int)        , true);
      if (!ReadLabelObjects1_00(ar)) return false;
      ReadLabelPositions(ar);
      long nPictures;
      CLabel::ArchiveRead(ar, &nPictures , sizeof(long), true);
      pPG->ReadSpecificUnit(ar);
      pPG->ReadLevelData(ar, nPictures);
      m_p3DObject->CheckMinMax();
//      dMinLevel *= m_p3DObject->GetNormFactor();
//      dMaxLevel *= m_p3DObject->GetNormFactor();
      m_LevelColor.SetMinMaxLevel(dMinLevel, dMaxLevel);

      m_Light[0].glLight.m_bEnabled = (ViewParam.bLightOn!=0) ? true: false;
      m_bInterpolate             = (ViewParam.bInterpolate!=0) ? true: false;
      m_LevelColor.SetColorMode((ViewParam.bInterpolateColor) ? CV_INTERPOLATE : 0);
      m_nProjection              = ViewParam.nProjection;
      m_nDetail                  = ViewParam.nDetail;
      m_bSmoothShading           = (ViewParam.nShading == GL_SMOOTH) ? true : false;

      m_Light[0].fTheta                  = LightParam.fLightAngleTheta;
      m_Light[0].fPhi                    = LightParam.fLightAnglePhi;
      m_Light[0].fDistance               = LightParam.fLightDistance;
      m_Light[0].glLight.m_fSpotCutOff   = LightParam.fSpotCutOff;
      m_Light[0].glLight.m_fSpotExponent = LightParam.fSpotExponent;
      m_Light[0].glLight.SetAmbientColor(LightParam.pfAmbient);
      m_Light[0].glLight.SetSpecularColor(LightParam.pfSpecular);
      m_Light[0].glLight.SetPosition(LightParam.pfPos);
      m_Light[0].glLight.SetDirection(LightParam.pfDirection);
      m_Light[0].glLight.SetAttenuation(LightParam.fConstantAttenuation, LightParam.fLinearAttenuation, LightParam.fQuadraticAttenuation);
      for (int i=1; i<NO_OF_LIGHTS; i++)
      {
         m_Light[i].glLight.m_bEnabled = false;
      }

      float fColor[4] = {0,0,0,1};
      pPG->SetColor(ObjectParam.ObjectColor);
      CGlLight::ColorToFloat(ObjectParam.ObjectColor, fColor);
      CGlLight::MultnFloat(3, fColor, ObjectParam.fAmbient);
      pPG->SetAmbientAndDiffuse(CGlLight::FloatsToColor(fColor));
      m_ObjTran.m_dTheta       = ObjectParam.dAngle_Theta;
      m_ObjTran.m_dPhi         = ObjectParam.dAngle_Phi;
      m_ObjTran.m_vTranslation = ObjectParam.ObjectTranslation;
      m_ObjTran.m_vAnchor      = ObjectParam.ObjectAnchor;
      m_ObjTran.m_vScale       = ObjectParam.ObjectZoom;
      return true;
   }
   return false;
}

bool CCARA3DVDoc::ReadNetFromCalcProg(CArchive &ar)
{
   BEGIN("CCARA3DVDoc::ReadNetFromCalcProg");
   ASSERT(NULL == m_p3DObject);
   ASSERT(NULL == m_ppHeadLines);
   ASSERT(NULL == m_pDibLabel);

   unsigned long lChecksum = m_FileHeader.GetChecksum();
   m_FileHeader.CalcChecksum();                                // Checksumme zurücksetzen

   CPlanGraph *pPG = new CPlanGraph;
   m_p3DObject = pPG;
   if (m_p3DObject)
   {
      m_p3DObject->SetDocument(this);
      float fColor[4] = {0,0,0,1};
      pPG->SetColor(m_ObjectColor);
      CGlLight::ColorToFloat(m_ObjectColor, fColor);
      CGlLight::MultnFloat(3, fColor, 0.7f);
      pPG->SetAmbientAndDiffuse(CGlLight::FloatsToColor(fColor));
      if (pPG->ReadNetData(ar))
      {
         if (pPG->IsFlat()) {m_bCullFace = false; m_bLightTwoSided = true; }
         else               {m_bCullFace = true;  m_bLightTwoSided = false;}
         if (!m_FileHeader.IsValid(lChecksum)) AfxThrowArchiveException(CRC_EXCEPTION, NULL);
         ar.Read(&m_FileHeader, sizeof(CFileHeader));
         lChecksum = m_FileHeader.GetChecksum();
         m_FileHeader.CalcChecksum();                                // Checksumme zurücksetzen
         if (m_FileHeader.IsType(CARA3D_FILE))
         {
            long nSize;
            CRect rc(0,0,1,1);
            CLabel::ArchiveRead(ar, &m_nHeadLines, sizeof(long), true);
            m_ppHeadLines = (CTextLabel**) new BYTE[sizeof(CTextLabel*)*m_nHeadLines];
            for (int i=0; i<m_nHeadLines; i++)
            {
               CLabel::ArchiveRead(ar, &nSize, sizeof(long), true);
               if (nSize > 0)
               {
                  m_ppHeadLines[i] = new CTextLabel(&rc, NULL);
                  if (m_ppHeadLines[i])
                  {
                     m_Container.InsertLabel(m_ppHeadLines[i]);
                     m_ppHeadLines[i]->SetDrawPickPoints(false);
                     if (!m_ppHeadLines[i]->ReadText(ar, (WORD)nSize, true))
                     {
                        return false;
                     }
                  }
               }
            }
            pPG->ReadUnits(ar);
            int nFlags = 0, nPictures = pPG->ReadSpecificUnit(ar);
            m_pszTempFileName = CLabel::ReadVariableString(ar, true);

            if (!pPG->MapLevelDataFile(m_pszTempFileName, nPictures)) return false;
            if (m_FileHeader.GetVersion() >= 110)
            {
               m_Pins.ReadPins(ar, false);
               pPG->Read_110_Data(ar);
            }
            if (!m_FileHeader.IsValid(lChecksum)) 
               AfxThrowArchiveException(CRC_EXCEPTION, NULL);
            return true;
         }
      }
   }
   return false;
}

bool CCARA3DVDoc::ReadDpoFromCalcProg(CArchive &ar)
{
   BEGIN("CCARA3DVDoc::ReadDpoFromCalcProg");
   ASSERT(NULL == m_p3DObject);
   ASSERT(NULL == m_ppHeadLines);
   ASSERT(NULL == m_pDibLabel);
   int nSize;
   unsigned long lChecksum = m_FileHeader.GetChecksum();
   m_FileHeader.CalcChecksum();                                // Checksumme zurücksetzen

   CPolarGraph *pPG = new CPolarGraph;
   m_p3DObject = pPG;
   if (m_p3DObject)
   {
      m_p3DObject->SetDocument(this);
      CLabel::ArchiveRead(ar, &m_nHeadLines, sizeof(long), true);
      if (m_nHeadLines > 0)
      {
         CRect rc(0,0,1,1);
         m_ppHeadLines = (CTextLabel**) new BYTE[sizeof(CTextLabel*)*m_nHeadLines];
         for (int i=0; i<m_nHeadLines; i++)
         {
            CLabel::ArchiveRead(ar, &nSize, sizeof(long), true);
            if (nSize > 0)
            {
               m_ppHeadLines[i] = new CTextLabel(&rc, NULL);
               if (m_ppHeadLines[i])
               {
                  m_Container.InsertLabel(m_ppHeadLines[i]);
                  m_ppHeadLines[i]->SetDrawPickPoints(false);
                  if (!m_ppHeadLines[i]->ReadText(ar, (WORD)nSize, true))
                     AfxThrowArchiveException(CArchiveException::generic, NULL);
               }
            }
         }
      }
      pPG->ReadPolarData(ar);
      m_pszTempFileName = CLabel::ReadVariableString(ar, true);
      if (!pPG->MapLevelDataFile(m_pszTempFileName)) return false;
      if (!m_FileHeader.IsValid(lChecksum)) 
         AfxThrowArchiveException(CRC_EXCEPTION, NULL);
      return true;
   }
   return false;
}

void CCARA3DVDoc::CalculateLightPosition(int nLight)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::CalculateLightPosition");
#endif
   ASSERT(nLight < NO_OF_LIGHTS);
   double cosTheta;
   CVector vLight, vTemp;
   cosTheta  = cos(m_Light[nLight].fTheta * M_PI_D_180);
   vLight    = CVector(cosTheta * sin(m_Light[nLight].fPhi   * M_PI_D_180),
                                  sin(m_Light[nLight].fTheta * M_PI_D_180),
                       cosTheta * cos(m_Light[nLight].fPhi   * M_PI_D_180));
   m_Light[nLight].glLight.SetDirection(-vLight);
   if (m_Light[nLight].fDistance > 0.0f)
   {
      vLight *= (double)m_Light[nLight].fDistance;
      vLight *= (Vx(m_ObjTran.m_vScale) + Vz(m_ObjTran.m_vScale)) * 0.5;
      vTemp   = m_ObjTran.m_vAnchor + m_ObjTran.m_vTranslation + vLight;
      m_Light[nLight].glLight.SetPosition(vTemp, 1.0f);
      TRACE("Lightposition: %.2f,%.2f,%.2f\n", Vx(vTemp), Vy(vTemp), Vz(vTemp));
      TRACE("Direction    : %.2f,%.2f,%.2f\n", -Vx(vLight), -Vy(vLight), -Vz(vLight));
   }
   else
   {
      m_Light[nLight].glLight.SetPosition(vLight, 0.0f);
   }
}

void CCARA3DVDoc::Write3DObject(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::Write3DObject");
#endif
   if (m_p3DObject)
   {
      if      (m_p3DObject->GetClass3DType() == PLAN_GRAPH ) m_FileHeader.SetType(PLANGRAPH_OBJECT);
      else if (m_p3DObject->GetClass3DType() == POLAR_GRAPH) m_FileHeader.SetType(DIR_PATTERN_OBJECT);
      else return;
      WriteFileHeader(ar);
      m_FileHeader.CalcChecksum();
      if (m_p3DObject) m_p3DObject->WriteObjectData(ar);
      WriteFileHeader(ar, false);      // Checksumme schreiben
   }
}

void CCARA3DVDoc::Read3DObject(CArchive &ar)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::Read3DObject");
#endif
   if (ar.Read(&m_FileHeader, sizeof(CFileHeader)) == sizeof(CFileHeader))
   {
      if (m_FileHeader.IsType(PLANGRAPH_OBJECT))        // PlanGraph-Object
      {
         unsigned long lChecksum = m_FileHeader.GetChecksum();
         m_FileHeader.CalcChecksum();
         CPlanGraph *pPG = new CPlanGraph;
         m_p3DObject = pPG;
         m_p3DObject->SetDocument(this);
         m_p3DObject->ReadObjectData(ar);
         m_p3DObject->CheckMinMax();
         if (!m_FileHeader.IsValid(lChecksum))
            AfxThrowArchiveException(CRC_EXCEPTION, NULL);
         if (pPG->IsFlat()) {m_bCullFace = false; m_bLightTwoSided = true; }
         else               {m_bCullFace = true;  m_bLightTwoSided = false;}
      }
      else if (m_FileHeader.IsType(WATERFALL_OBJECT))       // WaterFall-Objekt
      {
      }
      else if (m_FileHeader.IsType(DIR_PATTERN_OBJECT))     // Directivity-Pattern-Objekt
      {
         unsigned long lChecksum = m_FileHeader.GetChecksum();
         m_FileHeader.CalcChecksum();
         CPolarGraph *pPG = new CPolarGraph;
         m_p3DObject = pPG;
         m_p3DObject->SetDocument(this);
         m_p3DObject->ReadObjectData(ar);
         m_p3DObject->CheckMinMax();
         if (!m_FileHeader.IsValid(lChecksum))
            AfxThrowArchiveException(CRC_EXCEPTION, NULL);
         m_bCullFace = true;
         m_bLightTwoSided = false;
      }
   }
}


void CCARA3DVDoc::ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::ReportSaveLoadException");
#endif
	CCARADoc::ReportSaveLoadException(lpszPathName, e, bSaving, nIDPDefault);
   Init();
}

void CCARA3DVDoc::AttachLevelColor()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::AttachLevelColor");
#endif
   if (m_pColorLabel)
   {
      m_pColorLabel->Attach(&m_LevelColor);
//      double dFactor = 1.0 / m_LevelColor.GetNormFactor();
//      double dOffset = m_LevelColor.GetOffset();
//      m_pColorLabel->SetMinMaxLevel((m_LevelColor.GetMinLevel()-dOffset)*dFactor, (m_LevelColor.GetMaxLevel()-dOffset)*dFactor);
      m_pColorLabel->SetMinMaxLevel(m_LevelColor.GetMinLevel(), m_LevelColor.GetMaxLevel());
      m_pColorLabel->SetUnit((char*)m_Grid.GetLevelUnit());
   }
}

LPCTSTR CCARA3DVDoc::GetHeadLineText()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::GetHeadLineText");
#endif
   int nHeadLine = 0;
   if ((m_p3DObject != NULL) && (m_nHeadLines > 1) && m_p3DObject->PhaseValuesCalculated())
      nHeadLine = 1;

   if (m_ppHeadLines && m_ppHeadLines[nHeadLine]) return m_ppHeadLines[nHeadLine]->GetText();

   return NULL;
}

void CCARA3DVDoc::SetHeadlinePositions(CPoint ptTopLeft)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::SetHeadlinePositions");
#endif
   if (m_ppHeadLines)
   {
      for (int i=0; i<m_nHeadLines; i++)
      {
         if (m_ppHeadLines[i])
         {
            m_ppHeadLines[i]->SetTextAlign(TA_LEFT|TA_TOP);
            CLabel *pl = m_ppHeadLines[i];
            pl->SetPoint(ptTopLeft, 0);
         }
      }
   }
}

void CCARA3DVDoc::SetActiveHeadline()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::SetActiveHeadline");
#endif
   int nHeadLine = 0;
   if ((m_p3DObject != NULL) && (m_nHeadLines > 1) && m_p3DObject->PhaseValuesCalculated())
      nHeadLine = 1;

   if (m_ppHeadLines)
   {
      for (int i=0; i<m_nHeadLines; i++)
      {
         if (m_ppHeadLines[i])
         {
            m_ppHeadLines[i]->SetVisible((i==nHeadLine) ? true : false);
         }
      }
   }
}

BOOL CCARA3DVDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::CanCloseFrame");
#endif
   if (m_bPrintInit) SetModifiedFlag(false);

	BOOL bReturn = CCARADoc::CanCloseFrame(pFrame);
   return bReturn;
}

BOOL CCARA3DVDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
   if (m_p3DObject && m_p3DObject->PhaseValuesAvailable())
   {
      if (m_p3DObject->PhaseValuesCalculated())
      {
         if (AfxMessageBox(IDS_CANNOT_SAVE_AS_TEMPORAL_COARSE, MB_ICONINFORMATION|MB_YESNO)==IDNO)
            return false;
         CCARA3DVView *pView = ((CMainFrame*)AfxGetApp()->m_pMainWnd)->Get3DVView();
         if (pView)
         {
            pView->SwitchPhaseValues();
         }
      }
   }

   return CCARADoc::DoSave(lpszPathName, bReplace);
}

void CCARA3DVDoc::InitCriticalSectionOfDoc()
{
   if (m_pcsDocumentData==NULL)
   {
      m_pcsDocumentData = new CRITICAL_SECTION;
      InitializeCriticalSection(m_pcsDocumentData);
   }
}

void CCARA3DVDoc::DeleteCriticalSectionOfDoc()
{
   if (m_pcsDocumentData)
   {
      DeleteCriticalSection(m_pcsDocumentData);
      delete m_pcsDocumentData;
      m_pcsDocumentData = NULL;
   }
}

BOOL CCARA3DVDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	return CCARADoc::OnSaveDocument(lpszPathName);
}

#ifdef ETS_OLE_SERVER
COleServerItem* CCARA3DVDoc::OnGetEmbeddedItem()
{
   BEGIN("CCARADoc::OnGetEmbeddedItem");
	COleServerSrvrItem* pItem = new COleServerSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}
void CCARA3DVDoc::SetModifiedFlag(BOOL bModified)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVDoc::SetModifiedFlag");
#endif
   CCARADoc::SetModifiedFlag(bModified);
   m_bOleChanged = bModified ? true: false;
}
#endif
