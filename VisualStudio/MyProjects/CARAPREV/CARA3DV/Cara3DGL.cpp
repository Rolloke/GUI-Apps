// Cara3DGL.cpp: Implementierung der Klasse CCara3DGL.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cara3DGL.h"
#include "CARA3DVDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

CCara3DGL::CCara3DGL()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL()");
#endif
   InitCara3D();
}

CCara3DGL::~CCara3DGL()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("~CCara3DGL()");
#endif
//   Destroy();
}

CCara3DGL::CCara3DGL(CCara3DGL*pGL) : CEts3DGL(pGL)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL(..)");
#endif
   InitCara3D();
   m_pCone = pGL->m_pCone;
}

void CCara3DGL::InitCara3D()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::InitCara3D");
#endif
   SetModes(ETS3D_PR_ORTHOGONAL, ETS3D_PR_CLEAR);
   SetRefreshTime(50);
   m_pCone         = NULL;
}

void CCara3DGL::InitGLLists()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::InitGLLists");
#endif
   ASSERT_AND_REPORT_INT(GetLastError());

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   SetClearBits(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
// Commands
   CreateCommand(PRE_CMD   ,(LISTFUNCTION)CCara3DGL::PreCommand  , 1);
   int i;
   for (i=0; i<NO_OF_LIGHTS; i++)
   {
      CreateCommand(LIST_LIGHT+i, (LISTFUNCTION)CCara3DGL::LightCommand, 1);
// Listen
      CreateList(   LIST_LIGHT+i, (LISTFUNCTION)CCara3DGL::ListLight   , 1);
   }
   CreateList(LIST_MATRIX, (LISTFUNCTION)CCara3DGL::ListMatrix, 1);
   CreateList(LIST_GRID       , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_LEFT  , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_TOP   , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_RIGHT , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_BOTTOM, (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_FRONT , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
   CreateList(LIST_GRID_BACK  , (LISTFUNCTION)CCara3DGL::ListGrid  , 1);
//   CreateList(LIST_GRID  , (LISTFUNCTION)CCara3DGL::DirectPostCallListCommands, 0);
   CreateList(LIST_AXES  , (LISTFUNCTION)CCara3DGL::ListAxes  , 1);
//   CreateList(LIST_AXES  , (LISTFUNCTION)CCara3DGL::DirectPostCallListCommands, 0);
   CreateList(LIST_PINS  , (LISTFUNCTION)CCara3DGL::ListPins  , 1);
//   CreateList(LIST_PINS  , (LISTFUNCTION)CCara3DGL::DirectPostCallListCommands, 0);
   CreateList(LIST_OBJECT, (LISTFUNCTION)CCara3DGL::ListObject, LIST_OBJECT_SIZE);
//   CreateList(LIST_OBJECT, (LISTFUNCTION)CCara3DGL::DirectPostCallListCommands, 0);
   ASSERT_GL_ERROR;

   ::PostMessage(GetHWND(), WM_TIMER, EVENT_INVALIDATE_GL, 0);
   ASSERT_AND_REPORT_INT(GetLastError());
}

void CCara3DGL::AutoDelete()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::AutoDelete");
#endif
   if (GetModes() & ETS3D_DETACH_FOREIGN_DATA) DetachData();
   CEts3DGL::AutoDelete();
}

void CCara3DGL::DetachData()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::DetachData");
#endif
   m_pCone = NULL;
}

void CCara3DGL::DestroyGLLists()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::DestroyGLLists");
#endif
   glDeleteLists(LIST_CONE  , 1);
   glDeleteLists(LIST_SPHERE, 1);
   return;
}

void CCara3DGL::OnRenderSzene()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::OnRenderSzene");
#endif
   glLoadIdentity();
   CEts3DGL::OnRenderSzene();
/*
   if (GetGrid()->GetAxes() & SHOW_POLAR_GRID)
   {
      glLoadIdentity();
      GetObjectTransformation()->SetPolarGridTransformation();
   }
*/
   ::PostMessage(GetHWND(), WM_TIMER, EVENT_RENDER_READY, 0);
}

LRESULT CCara3DGL::OnTimer(long nTimerID, BOOL bValidate)
{
   if (nTimerID == (long)GetTimerID())
   {
      ASSERT(nTimerID != EVENT_RENDER_TIMER);
      ::SendMessage(GetHWND(), WM_TIMER, EVENT_RENDER_TIMER, 0);
   }
   return 0;
}

bool CCara3DGL::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::OnMessage");
#endif
   if (GetThreadID() == ::GetCurrentThreadId())
   {
      switch (nMsg)
      {
         case WM_MOUSEWHEEL:
         case WM_MOUSEMOVE:
         case WM_LBUTTONUP:
         case WM_RBUTTONUP:
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_LBUTTONDBLCLK:
         case WM_RBUTTONDBLCLK:
         case WM_DESTROY:
         {
            ::SendMessage(GetHWND(), nMsg, wParam, lParam);
            return true;
         }
      }
   }
   return false;
}

LRESULT CCara3DGL::OnCommand(WORD wID, WORD wNotify, HWND hwndFrom)
{
//   switch(wID)
//   {
//   }
   return 1;
}

void CCara3DGL::PreCommand(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::PreCommand");
#endif
   UNUSED(nListID);
   UNUSED(nRange);
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   float *pcolor = pC->GetAmbientColor();
   if (pcolor[3] != 0)
   {
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, pcolor);
   }
   else
   {
      float color[] = {0,0,0,1};
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
   }

   if (pC->Lighting())
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_NORMALIZE);
   }

   if (pC->SmoothShading()) glShadeModel(GL_SMOOTH);
   else                     glShadeModel(GL_FLAT);

   if (pC->CullFace())      glEnable(GL_CULL_FACE);
   else                     glDisable(GL_CULL_FACE);

   if (pC->LightTwoSided())
   {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
   }
   else
   {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
   }

   ASSERT_GL_ERROR;
}

void CCara3DGL::LightCommand(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::LightCommand");
#endif
   UNUSED(nListID);
   UNUSED(nRange);
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   CGlLight *pL =pC->GetLight(nListID - LIST_LIGHT); // Basisliste abziehen, um den Index zu erhalten
   if (pL)
   {
      if (pL->m_bEnabled) glEnable(pL->m_nLight);
      else                glDisable(pL->m_nLight);
      glLightfv(pL->m_nLight, GL_AMBIENT              , pL->m_pfAmbientColor);
      glLightfv(pL->m_nLight, GL_DIFFUSE              , pL->m_pfDiffuseColor);
      glLightfv(pL->m_nLight, GL_SPECULAR             , pL->m_pfSpecularColor);

      glLightf( pL->m_nLight, GL_SPOT_CUTOFF          , pL->m_fSpotCutOff);
      glLightf( pL->m_nLight, GL_SPOT_EXPONENT        , pL->m_fSpotExponent);

      glLightf( pL->m_nLight, GL_CONSTANT_ATTENUATION , pL->m_fAttenuation[0]);
      glLightf( pL->m_nLight, GL_LINEAR_ATTENUATION   , pL->m_fAttenuation[1]);
      glLightf( pL->m_nLight, GL_QUADRATIC_ATTENUATION, pL->m_fAttenuation[2]);
      glFlush();
      ASSERT_GL_ERROR;
   }
}

void CCara3DGL::ListLight(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListLight");
#endif
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   glDeleteLists(nListID, nRange);
   CGlLight *pL =pC->GetLight(nListID - LIST_LIGHT);
   if (pL->m_bEnabled)
   {
      glNewList(nListID, GL_COMPILE);
      glLightfv(pL->m_nLight, GL_POSITION      , pL->m_pfPosition);
      glLightfv(pL->m_nLight, GL_SPOT_DIRECTION, pL->m_pfDirection);
      glFlush();
      glEndList();
      ASSERT_GL_ERROR;
   }
}

void CCara3DGL::ListMatrix(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListMatrix");
#endif
   ASSERT(pC != NULL);

   CObjectTransformation*pOT = pC->GetObjectTransformation();
   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
   pOT->SetTransformation();   
   glFlush();
   glEndList();
   ASSERT_GL_ERROR;
}

void CCara3DGL::ListObject(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListObject");
#endif
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   glDeleteLists(nListID, nRange);

   glNewList(nListID, GL_COMPILE);
   pC->DrawObject();
   glFlush();
   glEndList();
}

void CCara3DGL::ListPins(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListPins");
#endif
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   glDeleteLists(nListID, nRange);
   CCara3DGL::CreateSphereList((float)pC->GetObjectDimension() * SPERE_RADIUSFACTOR, 1);
   if (pC->ShowPins(-1))
   {
      glNewList(nListID, GL_COMPILE);
      pC->DrawPins();
      glFlush();
      glEndList();
   }
}

void CCara3DGL::ListGrid(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListGrid");
#endif
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   C3DGrid *pG = pC->GetGrid();
   glDeleteLists(nListID, nRange);

   if (pG->DoDrawGrid())
   {
      glNewList(nListID, GL_COMPILE);
      pG->m_nCurrentID = nListID;
      pG->DrawGrid();
      glFlush();
      glEndList();
   }
}

void CCara3DGL::ListAxes(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::ListAxes");
#endif
   ASSERT(pC != NULL);
   ASSERT_AND_REPORT_INT(GetLastError());

   C3DGrid *pG = pC->GetGrid();
   glDeleteLists(nListID, nRange);

   if (pG->DoDrawAxes())
   {
      pC->CreateConeList();

      glNewList(nListID, GL_COMPILE);
      pG->DrawAxes();
      glFlush();
      glEndList();
   }
}

void CCara3DGL::DirectPostCallListCommands(CCara3DGL *pC, int nListID, int nRange)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::DirectPostCallListCommands");
#endif
   if (pC->Lighting())                                            // Das Ojekt kann Beleuchtet sein
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);                               // wegen Skalierung
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_NORMALIZE);
   }
   if (pC->SmoothShading()) glShadeModel(GL_SMOOTH);             // Shading für das Objekt kann smooth sein
   else                     glShadeModel(GL_FLAT);
   if (pC->CullFace())      glEnable(GL_CULL_FACE);              // Objektflächen können einseitig sein
   else                     glDisable(GL_CULL_FACE);
/*
   switch (nListID)
   {
      case LIST_OBJECT:             // Kein Object hat bis jetzt von den globalen Einstellungen
         if (pC->Lighting())                                            // Das Ojekt kann Beleuchtet sein
         {
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);                               // wegen Skalierung
         }
         else
         {
            glDisable(GL_LIGHTING);
            glDisable(GL_NORMALIZE);
         }
         if (pC->SmoothShading()) glShadeModel(GL_SMOOTH);             // Shading für das Objekt kann smooth sein
         else                     glShadeModel(GL_FLAT);
         if (pC->CullFace())      glEnable(GL_CULL_FACE);              // Objektflächen können einseitig sein
         else                     glDisable(GL_CULL_FACE);
         break;                     // unabhängige Gl-Parameter änderungen.
      case LIST_PINS:               
         if (pC->Lighting())        // Die Linien der Pins werden immer ohne Beleuchtung
         {                          // gezeichnet 
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);
         }
         break;
      case LIST_AXES:               
         if (pC->CullFace())        // Die Cones werden immer ohne Cullface gezeichnet
            glEnable(GL_CULL_FACE);
         if (pC->Lighting())        // Die Linien der Axen werden immer ohne Beleuchtung
         {                          // gezeichnet
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);
         }
         break;
      case LIST_GRID:
         if (pC->Lighting())            // Die Linien der Gitter werden immer ohne Beleuchtung
         {                          // gezeichnet
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);
         }
         if (!pC->SmoothShading())      // Die Linien der Gitter werden immer mit Smooth
            glShadeModel(GL_FLAT);  // Shading gezeichnet
         break;
      default:
         break; 
   }
*/
}

void CCara3DGL::OnDrawToRenderDC(HDC hdc, int nOff)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::OnDrawToRenderDC");
#endif
   ASSERT_AND_REPORT_INT(GetLastError());

   C3DGrid *pG = GetGrid();
   if (pG->DoDrawAxes() || pG->DoDrawGrid())
   {
      pG->DrawText(hdc, nOff);
   }

   if (ShowPins(-1))
   {
      DrawPinsText(hdc, nOff);
   }
}

void CCara3DGL::Triangle(STriangle* pTr, CVector *pv1, CVector *pv2, CVector *pv3, CVector *pn1, CVector *pn2, CVector *pn3, int nseite)
{
   ASSERT(pTr->nDetail >= NO_TRIANGLE_DIVISION);               // Wertebereich : nDetail >= -1
   static double l1,l2,l3, max, ds,dn;
   static CVector s1,s2,s3;
   static int seite;
   static bool bNoDiv;
   
   CVector *pna=NULL, v, na;

   if (pTr->nDetail != NO_TRIANGLE_DIVISION)                   // Aufteilung erwünscht ?
   {                                                           // Berechnung durchführen
      if (pTr->nDetail != TRIANGLE_DIVISION_BY_STEP) pTr->nDetail--;
      bNoDiv = false;
      s1 = *pv3 - *pv2;
      s2 = *pv1 - *pv3;
      s3 = *pv2 - *pv1;

      l1 = Betrag(s1);
      ASSERT_AND_REPORT_INT(GetLastError());
      l2 = Betrag(s2);
      ASSERT_AND_REPORT_INT(GetLastError());
      l3 = Betrag(s3);
      ASSERT_AND_REPORT_INT(GetLastError());

      if (l1>l2)                                               // längste Seite ermitteln
      {
         if (l1>l3) max = l1, seite=1;   
         else       max = l3, seite=3;
      }
      else
      {
         if (l2>l3) max = l2, seite=2;
         else       max = l3, seite=3;
      }
      if (pTr->nDetail==TRIANGLE_DIVISION_BY_STEP)
      {
         ASSERT(pTr->dStep > 0);
         if (max < pTr->dStep ) bNoDiv = true;                 // Seitenlänge vergleichen
      }
      else
      {
         if (pTr->nDetail == 0) bNoDiv = true;                 // Rekursionstiefe testen
      }
   }
   else bNoDiv = true;

   if(bNoDiv)                                                   // Teilung noch nötig ?
   {
      pTr->pFn(pv1, pv2, pv3, pn1, pn2, pn3, pTr->pParam);
   }
   else
   {
      switch(seite)                                            // Teile die längste Seite
      {
      case 1:                                                  // s1 = *pv3 - *pv2;
         v = *pv2+0.5*s1;
         if (pn3 && pn2)
         {
            na = Norm(*pn3+*pn2);
            pna = &na;
            if (pTr->bInterpolate && (nseite != 3))            // Außenseite 1 darf nicht interpoliert werden
               v += ((Intersection(pv2, pn2, &v, pna)+
                      Intersection(pv3, pn3, &v, pna))*0.5-v)*0.5;
         }
         ASSERT_AND_REPORT_INT(GetLastError());
         Triangle(pTr, pv1, pv2, &v , pn1, pn2, pna, (nseite==5) ? 4 : nseite);
         Triangle(pTr, pv1, &v , pv3, pn1, pna, pn3, (nseite==6) ? 4 : nseite);
         break;

      case 2:                                                  // s2 = *pv1 - *pv3;
         v = *pv3+0.5*s2;
         if (pn1 && pn3)
         {
            na = Norm(*pn1+*pn3);
            pna = &na;
            if (pTr->bInterpolate && (nseite!=5))              // Außenseite 2 darf nicht interpoliert werden
               v += ((Intersection(pv3, pn3, &v, pna)+
                      Intersection(pv1, pn1, &v, pna))*0.5-v)*0.5;
         }
         ASSERT_AND_REPORT_INT(GetLastError());
         Triangle(pTr, pv1, pv2, &v , pn1, pn2, pna, (nseite==3) ? 2 : nseite);
         Triangle(pTr, &v , pv2, pv3, pna, pn2, pn3, (nseite==6) ? 2 : nseite);
         break;
      case 3:                                                  // s3 = *pv2 - *pv1;
         v = *pv1+0.5*s3;
         if (pn2 && pn1) 
         {
            na  = Norm(*pn2+*pn1);
            pna = &na; 
            if (pTr->bInterpolate && (nseite!=6))              // Außenseite 3 darf nicht interpoliert werden
               v += ((Intersection(pv1, pn1, &v, pna)+
                      Intersection(pv2, pn2, &v, pna))*0.5-v)*0.5;
         }
         ASSERT_AND_REPORT_INT(GetLastError());
         Triangle(pTr, pv1,  &v, pv3, pn1, pna, pn3, (nseite==3) ? 1 : nseite);
         Triangle(pTr, &v , pv2, pv3, pna, pn2, pn3, (nseite==5) ? 1 : nseite);
         break;
      }
   }
   if ((pTr->nDetail != TRIANGLE_DIVISION_BY_STEP) && (pTr->nDetail != NO_TRIANGLE_DIVISION))
      pTr->nDetail++;
}

CVector CCara3DGL::Intersection(CVector *pPlaneOrg, CVector *pPlaneOrth, CVector *pLineOrg, CVector *pLineDir)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::Intersection");
#endif
   double hilf = *pPlaneOrth * *pLineDir;
   if (hilf==0.0) return CVector(0.0,0.0,0.0);
   return *pLineOrg + *pPlaneOrth * (*pPlaneOrg - *pLineOrg) * *pLineDir / hilf;
}

void CCara3DGL::CreateConeList()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::CreateConeList");
#endif
   ASSERT_AND_REPORT_INT(GetLastError());

   if (!m_pCone) m_pCone = gluNewQuadric();
   if (m_pCone)
   {
      float fConeSize = (float)GetObjectDimension() * CONE_SIZEFACTOR;
      glDeleteLists(LIST_CONE, 1);
      glNewList(LIST_CONE, GL_COMPILE);
         gluQuadricDrawStyle(m_pCone, GLU_FILL);
         gluQuadricNormals(m_pCone, GLU_SMOOTH);
         gluCylinder(m_pCone,0.35f*fConeSize, 0.0f, (float)fConeSize, 12, 1);
      glEndList();
   }
}

void CCara3DGL::CreateSphereList(float fRadius, int nDetail)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::CreateSphereList");
#endif
   ASSERT_AND_REPORT_INT(GetLastError());
 
   glDeleteLists(LIST_SPHERE, 1);
   glNewList(LIST_SPHERE, GL_COMPILE);
      Draw(ETS3D_SPHERE|ETS3D_SOLID_FRAME|nDetail, fRadius);
   glEndList();
   ASSERT_GL_ERROR;
}

bool CCara3DGL::TextOut(HDC hdc, double x, double y, double z, UINT nAlign, int nOff, char *pszText)
{
   unsigned char bVal = 0;
   if (!pszText) return false;

   ASSERT_AND_REPORT_INT(GetLastError());

   glRasterPos3d(x, y, z);
   glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &bVal);
   if (bVal)
   {
      double pos[4];
      glGetDoublev(GL_CURRENT_RASTER_POSITION, pos);
/*
      int nVal0 = 0, nVal1 = 0;
      glReadPixels((int)pos[0], (int)pos[1], 1, 1, GL_DEPTH_COMPONENT, GL_INT, &nVal0);
      TRACE("Depth %x\n", nVal0);
      glBegin(GL_LINES);                                       // Linien zeichnen
      glVertex3d(x-1, y-1, z-1);
      glVertex3d(x+1, y+1, z+1);
      glEnd();
      glReadPixels((int)pos[0], (int)pos[1], 1, 1, GL_DEPTH_COMPONENT, GL_INT, &nVal1);
      TRACE("Depth %x\n", nVal1);
      if (nVal0 == nVal1) return false;
*/
      ::SetTextAlign(hdc, nAlign&0xffffffdf);
      if ((nAlign & TA_VCENTER) == TA_VCENTER)
      {
         SIZE size;
         ::GetTextExtentPoint32(hdc, pszText, strlen(pszText), &size);
         pos[1] -=  size.cy >> 1;
      }
      ::TextOut(hdc, (int)pos[0], nOff-(int)pos[1], pszText, strlen(pszText));
      return true;
   }
   else return false;
}

void CCara3DGL::LightedFans(int nPoints, CVector *pvPoints, void *pParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCara3DGL::LightedFans");
#endif
   int i;
   if (nPoints == 3) glBegin(GL_TRIANGLES);
   else              glBegin(GL_TRIANGLE_FAN);

   for (i=0; i<nPoints; i++)
   {
      if (pParam) glNormal3dv((double*)pParam);
      glVertex3dv((double*)&pvPoints[i]);
   }

   glEnd();
}

void CCara3DGL::PerformGridList(bool bDrawGrid, bool bChangeType)
{
   int i;
   if (GetGrid()->GetAxes() & SHOW_INTELLI_GRID)
   {
      CObjectTransformation *pO = GetObjectTransformation();
      bool bLeft, bTop, bRight, bBottom, bFront, bBack;
      bLeft = bTop = bRight = bBottom = bFront = bBack = false;
      if (bDrawGrid)
      {
         if       (pO->m_dPhi ==   0) bBack  = true;
         else if  (pO->m_dPhi ==  90) bRight = true;
         else if  (pO->m_dPhi == 180) bFront = true;
         else if  (pO->m_dPhi == 270) bLeft  = true;
         else if ((pO->m_dPhi >    0) && (pO->m_dPhi <  90)) bRight = bBack  = true;
         else if ((pO->m_dPhi >   90) && (pO->m_dPhi < 180)) bRight = bFront = true;
         else if ((pO->m_dPhi >  180) && (pO->m_dPhi < 270)) bLeft  = bFront = true;
         else if ((pO->m_dPhi >  270) && (pO->m_dPhi < 360)) bLeft  = bBack  = true;
         if (pO->m_dTheta == 0) bBottom = bTop = false;
         else if (pO->m_dTheta ==  90) bTop    = true;
         else if (pO->m_dTheta == -90) bBottom = true;
         else if (pO->m_dTheta >  0)   bBottom = true;
         else                          bTop    = true;
//         if ((pO->m_dTheta > 0) ^ ((pO->m_dPhi >= 90) && (pO->m_dPhi < 270))) bBottom = true;
//         else                                                                 bTop    = true;
      }
      PerformList(LIST_GRID_LEFT  , bLeft);
      PerformList(LIST_GRID_TOP   , bTop);
      PerformList(LIST_GRID_RIGHT , bRight);
      PerformList(LIST_GRID_BOTTOM, bBottom);
      PerformList(LIST_GRID_FRONT , bFront);
      PerformList(LIST_GRID_BACK  , bBack);
      if (bChangeType)
         PerformList(LIST_GRID, false);
   }
   else
   {
      if (bChangeType)
      {
         for (i=1; i<7; i++)
            PerformList(LIST_GRID+i, false);
      }
      PerformList(LIST_GRID, bDrawGrid);
   }
}

void CCara3DGL::InvalidateGridList()
{
   int i=0, nGrids = 1;
   if (GetGrid()->GetAxes() & SHOW_INTELLI_GRID)
   {
      i      = 1;
      nGrids = 7;
   }
   for (; i<nGrids; i++)
      InvalidateList(LIST_GRID+i);
}
