/********************************************************************/
/* Funktionen der Klasse CBoxObj                                    */
/********************************************************************/
#include "StdAfx.h"

#include "BoxDscr.h"
#include "FileHeader.h"
#include "Cara3DTr.h"
#include "Ets3dgl.h"
#include "CaraWalkDll.h"
#include "CustErr.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

float   CBoxObj::gm_pfBoxColor[4]     = {0.1f, 0.1f, 0.7f, 1.0f};
float   CBoxObj::gm_pfSelBoxColor[4]  = {0.7f, 0.1f, 0.1f, 1.0f};
float   CBoxObj::gm_pfChassisColor[4] = {0.6f, 0.6f, 0.6f, 1.0f};
float   CBoxObj::gm_pfSpecular[4]     = {0.0f, 0.0f, 0.0f, 1.0f};

#define MATERIAL_DATA              0x03
#define MATERIAL_TEXTURE_IN_FILE   0x01
#define MATERIAL_TEXTURE_FROM_FILE 0x02
#define SHAPE_POLYGON_DATA         0x04


CBoxObj::CBoxObj()
{
   BEGIN("CBoxObj()");
   Init();
   m_BoxPosition.SetDestroyMode(true);
   m_BoxPosition.SetDeleteFunction(CBoxPosition::DeleteFunction);
}

CBoxObj::~CBoxObj()
{
   BEGIN("~CBoxObj()");
   DeleteData();
}

void CBoxObj::DeleteData()
{
   if (m_pvPolyPoints)
   {
      int i, nPoly = GetNoOfPolygons();
      for (i=0; i<nPoly; i++)
      {
         if (m_pPolygons[i].pnPoints     ) delete[] m_pPolygons[i].pnPoints;
         if (m_pPolygons[i].pvShapePoints) delete[] m_pPolygons[i].pvShapePoints;
      }
      m_nPolygons = 0;
      delete[] m_pvPolyPoints;
   }

   if (m_pPolygons)         delete[] m_pPolygons;
   if (m_pvSphereLocations) delete[] m_pvSphereLocations;
   if (m_pfSphereRadius)    delete[] m_pfSphereRadius;
   if (m_ppMaterial)        delete[] m_ppMaterial;

   m_pvPolyPoints       = NULL;
   m_pPolygons          = NULL;
   m_pvSphereLocations  = NULL;
   m_pfSphereRadius     = NULL;
   m_ppMaterial         = NULL;
}

void CBoxObj::Init()
{
   ZeroMemory(m_szFileName, sizeof(_MAX_FNAME));
   ZeroMemory(m_szBoxName, BOX_NAMELENGTH);
   ZeroMemory(&m_vZDRP, sizeof(CVector));
   ZeroMemory(&m_vURP , sizeof(CVector));
   ZeroMemory(&m_vMYRP, sizeof(CVector));
   ZeroMemory(&m_vMin , sizeof(CVector));
   ZeroMemory(&m_vMax , sizeof(CVector));
   m_nBoxType           = 0;
   m_nRotate            = 0;
   m_nPolyPoints        = 0;
   m_nBoxPoints         = 0;
   m_nPolygons          = 0;
   m_nChassisPolygons   = 0;
   m_nSpheres           = 0;
   m_pvPolyPoints       = NULL;
   m_pPolygons          = NULL;
   m_pvSphereLocations  = NULL;
   m_pfSphereRadius     = NULL;
   m_pMaterial          = NULL;
   m_ppMaterial         = NULL;
   m_bShowURP           = false;

   m_nSelectedPolygon   = -1;
}

void CBoxObj::DrawFan(int nCount, CVector *pV, void *pParam)
{
   int i;
   if (nCount == 3) glBegin(GL_TRIANGLES);
   else             glBegin(GL_TRIANGLE_FAN);

   for (i=0; i<nCount; i++)
   {
      if (pParam) glNormal3dv((double*)pParam);
      glVertex3dv((double*)&pV[i]);
   }

   glEnd();
}

void CBoxObj::SetGlobalBoxMaterial(bool bTexture, bool bLight, int /*nDetail*/, HPALETTE hPal, bool /*bDrawShadow*/, double /*dDist*/)
{
/*
   if (bTexture)
   {
      glEnable(GL_TEXTURE_2D);
      if (CCaraWalkDll::gm_bTextureObject && !m_pMaterial->IsTextureObject())
      {
         m_pMaterial->CreateTextureObject();
      }
   }
   else glDisable(GL_TEXTURE_2D);
*/

   if (m_pMaterial)
   {
      m_pMaterial->ImageMaterialAndLight(bTexture, bLight, hPal);
   }
   else if (hPal)                                           // Materialparameter für das Gehäuse
   {
      int nIndex[3];
      nIndex[0] = GetNearestPaletteIndex(hPal, RGB(0, 0, 255));
      nIndex[2] = nIndex[1] = nIndex[0];
      glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, nIndex);
      glIndexi(GetNearestPaletteIndex(hPal, RGB(0, 0, 255)));
   }
   else
   {
      glColor3fv((float*)gm_pfBoxColor);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)gm_pfBoxColor);
      glMaterialfv(GL_FRONT, GL_SPECULAR, (float*) gm_pfSpecular);
   }
}

void CBoxObj::DrawBox(bool bTexture, bool bLight, int nDetail, HPALETTE hPal, bool bDrawShadow, double dDist)
{
   BEGIN("DrawBox");
   int      nP0, nPolyPoint, nPoly, i, nPolygons, nPolyPoints, *pnPoints;
   double   dMax   = 0.05 + 0.5 / (double)(1 << nDetail);
   CVector  vDist(0,0,0), vV;
   CEtsList list;
   bool     bGlobalBoxMaterial = false;
   list.SetDestroyMode(false);

   ETS3DGL_Fan Fan;
   Fan.dStep  = dMax;
   Fan.pFn    = DrawFan;

   if (!m_pMaterial)
   {
      glDisable(GL_TEXTURE_2D);
      bTexture = false;
   }

   if (bDrawShadow)                                            // Schatten brauchen keine Materialparameter
   {
      bLight = false;                                          // und kein Licht
   }
   else
   {
      SetGlobalBoxMaterial(bTexture, bLight, nDetail, hPal, bDrawShadow, dDist);
      bGlobalBoxMaterial = true;
   }

   REPORT("Cabinet");
   nPolygons = m_nPolygons;
   for (nPoly=0; nPoly<nPolygons; nPoly++)
   {
      pnPoints    = m_pPolygons[nPoly].pnPoints;
      nP0         = pnPoints[0];
      nPolyPoints = m_pPolygons[nPoly].nPoints-2;

      ASSERT((nP0 >= 0) && (nP0<m_nPolyPoints));
      list.Destroy();
      list.ADDHead(&m_pvPolyPoints[nP0]);

      if (CCaraWalkDll::gm_bBugFixNormalVector)                // Normalenvectoren immer setzen
      {
         Fan.pParam = &m_pPolygons[nPoly].vFlnv;
      }
      else                                                     // Normalenvector nur einmal setzen
      {
         Fan.pParam = NULL;
         if (bLight) glNormal3dv((double*) &m_pPolygons[nPoly].vFlnv);
      }

      if (bDrawShadow)                                         // Schatten zeichnen
      {
         bGlobalBoxMaterial = true;                            // Keine Material / Lichtparameter
      }
      else if (m_nSelectedPolygon == nPoly)                    // Selektiertes Polygon
      {
         glColor3fv((float*)gm_pfSelBoxColor);                 // Selectionsfarbe setzen
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)gm_pfSelBoxColor);
         bGlobalBoxMaterial = false;
      }
      else if ((m_ppMaterial != NULL) && (m_ppMaterial[nPoly] != NULL))
      {                                                        // Mehrere Materialien
         bGlobalBoxMaterial = false;
         m_ppMaterial[nPoly]->ImageMaterialAndLight(bTexture, bLight, hPal);
         if (!m_ppMaterial[nPoly]->m_bGenerateCoordinates && (m_pPolygons[nPoly].nPoints == 4))
         {                                                     // Textur in einem Rechteck anzeigen
            CVector vPoint;
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glBegin(GL_QUADS);
               glTexCoord2d(0.0                                 , 0.0);
               glVertex3dv((double*)&m_pvPolyPoints[pnPoints[0]]);
               glTexCoord2d(m_ppMaterial[nPoly]->m_fsScaleFactor, 0.0);
               glVertex3dv((double*)&m_pvPolyPoints[pnPoints[1]]);
               glTexCoord2d(m_ppMaterial[nPoly]->m_fsScaleFactor, m_ppMaterial[nPoly]->m_ftScaleFactor);
               glVertex3dv((double*)&m_pvPolyPoints[pnPoints[2]]);
               glTexCoord2d(0.0                                 , m_ppMaterial[nPoly]->m_ftScaleFactor);
               glVertex3dv((double*)&m_pvPolyPoints[pnPoints[3]]);
            glEnd();
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            continue;
         }
         else                                                  // Texturkoordinaten automatisch ausrichten
         {
            m_ppMaterial[nPoly]->AlignTexture(m_pPolygons[nPoly].vFlnv);
         }
      }
      else if (!bGlobalBoxMaterial)                            // Globale Parameter wieder setzen
      {
         SetGlobalBoxMaterial(bTexture, bLight, nDetail, hPal, bDrawShadow, dDist);
         bGlobalBoxMaterial = true;
      }

      if (bGlobalBoxMaterial && bTexture)                      // Texturen anzeigen, ja :
      {
         m_pMaterial->AlignTexture(m_pPolygons[nPoly].vFlnv);  // Texturkoordinaten anhand des Normalenvektors der Wand ausrichten
      }

      for (nPolyPoint=0; nPolyPoint<nPolyPoints; nPolyPoint++)
      {
         ASSERT((pnPoints[nPolyPoint+1]>=0) && (pnPoints[nPolyPoint+1]<m_nPolyPoints));
         ASSERT((pnPoints[nPolyPoint+2]>=0) && (pnPoints[nPolyPoint+2]<m_nPolyPoints));
         if (CCaraWalkDll::IsTriangleNull(m_pvPolyPoints[nP0], m_pvPolyPoints[pnPoints[nPolyPoint+1]], m_pvPolyPoints[pnPoints[nPolyPoint+2]]))
         {
            REPORT("Null-Dreieck(0,%d,%d)", nPolyPoint+1, nPolyPoint+2);
            continue;
         }
         if (bLight)                                           // beleuchtete Box zeichnen
         {
            CEts3DGL::DrawTriangleFan(&m_pvPolyPoints[nP0], &m_pvPolyPoints[pnPoints[nPolyPoint+1]], &m_pvPolyPoints[pnPoints[nPolyPoint+2]], &Fan);
         }
         else
         {
            if (!list.InList(&m_pvPolyPoints[pnPoints[nPolyPoint+1]])) list.ADDHead(&m_pvPolyPoints[pnPoints[nPolyPoint+1]]);
            if (!list.InList(&m_pvPolyPoints[pnPoints[nPolyPoint+2]])) list.ADDHead(&m_pvPolyPoints[pnPoints[nPolyPoint+2]]);
         }
      }

      if (!bLight)
      {
         double *p;
         if (list.GetCounter() == 3) glBegin(GL_TRIANGLES);
         else                        glBegin(GL_TRIANGLE_FAN);
         for (p = (double*)list.GetFirst(); p!= NULL; p = (double*)list.GetNext())
         {
            glVertex3dv(p);
         }
         glEnd();
      }
   }

   if (bTexture) glDisable(GL_TEXTURE_2D);

   bGlobalBoxMaterial = false;
   if (!bDrawShadow)                                           // Die Chassis werden für den Schatten nicht gebraucht
   {
      REPORT("Chassis");
      bGlobalBoxMaterial = false;
      if (CCaraWalkDll::gm_nPolygonOffset) glEnable(GL_POLYGON_OFFSET_FILL);
      nPolygons = GetNoOfPolygons();
      for (nPoly=m_nPolygons; nPoly<nPolygons; nPoly++)
      {
         CVector     vN;
         double      dFactor = 1.0;
         pnPoints    = m_pPolygons[nPoly].pnPoints;
         nPolyPoints = m_pPolygons[nPoly].nPoints;

         if (m_nSelectedPolygon == nPoly)                         // Selektiertes Polygon
         {
            glColor3fv((float*)gm_pfSelBoxColor);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)gm_pfSelBoxColor);
            bGlobalBoxMaterial = false;
         }
         else if ((m_ppMaterial != NULL) && (m_ppMaterial[nPoly] != NULL))
         {                                                        // Mehrere Materialien
            m_ppMaterial[nPoly]->ImageMaterialAndLight(bTexture, bLight, hPal);
            m_ppMaterial[nPoly]->AlignTexture(m_pPolygons[nPoly].vFlnv);
            bGlobalBoxMaterial = false;
         }
         else if (!bGlobalBoxMaterial)
         {
            if (hPal)                                                // Materialparameter für die Chassis
            {
               int nIndex[3];
               nIndex[0] = GetNearestPaletteIndex(hPal, RGB(128, 128, 128));
               nIndex[2] = nIndex[1] = nIndex[0];
               glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, nIndex);
               glIndexi(GetNearestPaletteIndex(hPal, RGB(128, 128, 128)));
            }
            else
            {
               glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)gm_pfChassisColor);
               glMaterialfv(GL_FRONT, GL_SPECULAR, (float*) gm_pfSpecular);
               glColor3fv((float*)gm_pfChassisColor);
            }
            bGlobalBoxMaterial = true;
         }

         if (CCaraWalkDll::gm_nPolygonOffset)                  // Offset im Depthbuffer
            glPolygonOffset(CCaraWalkDll::gm_fPolygonFactor, (float)(-CCaraWalkDll::gm_nPolygonOffset));
         else                                                  // Offset als Abstand
            vDist = m_pPolygons[nPoly].vFlnv * dDist;

         nP0         = pnPoints[0];
         ASSERT((nP0 >= 0) && (nP0<m_nPolyPoints));
         list.Destroy();
         if (nPolyPoints == 36)
         {
            vV = (m_pvPolyPoints[nP0] + m_pvPolyPoints[pnPoints[18]]) * 0.5;
            dFactor = Betrag(m_pvPolyPoints[nP0] - m_pvPolyPoints[pnPoints[18]]) * 0.1;
            nPolyPoint = -1;
         }
         else
         {
            vV         = m_pvPolyPoints[nP0];
            vN         = m_pPolygons[nPoly].vFlnv;
            nPolyPoint = 0;
         }

         Fan.pParam = &vN;
         list.ADDHead(&vV);

         CVector vV1 = vV + vDist;
         if (m_pPolygons[nPoly].nShapePoints)
         {
            vV1 = vV + m_pPolygons[nPoly].vFlnv * (Vy(m_pPolygons[nPoly].pvShapePoints[0])+dDist);
         }
         for (; nPolyPoint<nPolyPoints-2; nPolyPoint++)
         {
            ASSERT((pnPoints[nPolyPoint+1]>=0) && (pnPoints[nPolyPoint+1]<m_nPolyPoints));
            ASSERT((pnPoints[nPolyPoint+2]>=0) && (pnPoints[nPolyPoint+2]<m_nPolyPoints));
            if (CCaraWalkDll::IsTriangleNull(vV, m_pvPolyPoints[pnPoints[nPolyPoint+1]], m_pvPolyPoints[pnPoints[nPolyPoint+2]]))
            {
               REPORT("Null-Dreieck(0,%d,%d)", nPolyPoint+1, nPolyPoint+2);
               continue;
            }
            if (bLight)
            {
               CVector vV2;
               CVector vV3;
               if (m_pPolygons[nPoly].nShapePoints == 0)
               {
                  vV2 = m_pvPolyPoints[pnPoints[nPolyPoint+1]] + vDist;
                  vV3 = m_pvPolyPoints[pnPoints[nPolyPoint+2]] + vDist;
               }
               if (m_pPolygons[nPoly].nShapePoints)
               {
                  int nShape;
                  CVector vV2s, vV3s;
                  for (nShape=m_pPolygons[nPoly].nShapePoints-1; nShape>0; nShape--)
                  {
                     vV2  = vV + (m_pvPolyPoints[pnPoints[nPolyPoint+1]] - vV) * Vx(m_pPolygons[nPoly].pvShapePoints[nShape]);
                     vV2 += m_pPolygons[nPoly].vFlnv * (Vy(m_pPolygons[nPoly].pvShapePoints[nShape])+dDist);
                     vV3  = vV + (m_pvPolyPoints[pnPoints[nPolyPoint+2]] - vV) * Vx(m_pPolygons[nPoly].pvShapePoints[nShape]);
                     vV3 += m_pPolygons[nPoly].vFlnv * (Vy(m_pPolygons[nPoly].pvShapePoints[nShape])+dDist);
                     if (nShape<m_pPolygons[nPoly].nShapePoints-1)
                     {
                        vN = Norm(Product(vV2s-vV3, vV2s-vV2));
                        CEts3DGL::DrawTriangleFan(&vV2s, &vV3, &vV2, &Fan);
                        CEts3DGL::DrawTriangleFan(&vV3s, &vV3, &vV2s, &Fan);
                     }
                     vV2s = vV2;
                     vV3s = vV3;
                  }
                  vN = Norm(Product(vV1-vV2, vV1-vV3));
               }
               else if (nPolyPoints == 36)
               {
                  vN = vV1 - m_pPolygons[nPoly].vFlnv * dFactor;
                  vN = Norm(Product(vV2-vN, vV3-vN));
               }
               CEts3DGL::DrawTriangleFan(&vV1, &vV2, &vV3, &Fan);
            }
            else
            {
               if (!list.InList(&m_pvPolyPoints[pnPoints[nPolyPoint+1]])) list.ADDHead(&m_pvPolyPoints[pnPoints[nPolyPoint+1]]);
               if (!list.InList(&m_pvPolyPoints[pnPoints[nPolyPoint+2]])) list.ADDHead(&m_pvPolyPoints[pnPoints[nPolyPoint+2]]);
            }
         }

         if (nPolyPoints == 36)
         {
            if (bLight)
            {
               CVector vV2;
               CVector vV3;
               if (m_pPolygons[nPoly].nShapePoints == 0)
               {
                  vV2 = m_pvPolyPoints[pnPoints[nPolyPoints-1]] + vDist;
                  vV3 = m_pvPolyPoints[pnPoints[0            ]] + vDist;
                  vN = vV1 - m_pPolygons[nPoly].vFlnv * dFactor;
                  vN = Norm(Product(vV2-vN, vV3-vN));
               }
               else 
               {
                  int nShape;
                  CVector vV2s, vV3s;
                  for (nShape=m_pPolygons[nPoly].nShapePoints-1; nShape>0; nShape--)
                  {
                     vV2  = vV + (m_pvPolyPoints[pnPoints[nPolyPoints-1]] - vV) * Vx(m_pPolygons[nPoly].pvShapePoints[nShape]);
                     vV2 += m_pPolygons[nPoly].vFlnv * (Vy(m_pPolygons[nPoly].pvShapePoints[nShape])+dDist);
                     vV3  = vV + (m_pvPolyPoints[pnPoints[0            ]] - vV) * Vx(m_pPolygons[nPoly].pvShapePoints[nShape]);
                     vV3 += m_pPolygons[nPoly].vFlnv * (Vy(m_pPolygons[nPoly].pvShapePoints[nShape])+dDist);
                     if (nShape<m_pPolygons[nPoly].nShapePoints-1)
                     {
                        vN = Norm(Product(vV2s-vV3, vV2s-vV2));
                        CEts3DGL::DrawTriangleFan(&vV2s, &vV3, &vV2, &Fan);
                        CEts3DGL::DrawTriangleFan(&vV3s, &vV3, &vV2s, &Fan);
                     }
                     vV2s = vV2;
                     vV3s = vV3;
                  }
                  vN = Norm(Product(vV1-vV2, vV1-vV3));
               }

               CEts3DGL::DrawTriangleFan(&vV1, &vV2, &vV3, &Fan);
            }
            else
            {
               list.ADDHead(&m_pvPolyPoints[pnPoints[0]]);
            }
         }

         if (!bLight)
         {
            CVector *pV;
            if (list.GetCounter() == 3) glBegin(GL_TRIANGLES);
            else                        glBegin(GL_TRIANGLE_FAN);
            for (pV = (CVector*)list.GetFirst(); pV!= NULL; pV = (CVector*)list.GetNext())
            {
               vV =  *pV + vDist;
               glVertex3dv((double*)&vV);
            }
            glEnd();
         }
      }
      if (CCaraWalkDll::gm_nPolygonOffset) glDisable(GL_POLYGON_OFFSET_FILL);
   }

   if (!bGlobalBoxMaterial && !bDrawShadow)
   {
      if (hPal)                                                // Materialparameter für die Chassis
      {
         int nIndex[3];
         nIndex[0] = GetNearestPaletteIndex(hPal, RGB(128, 128, 128));
         nIndex[2] = nIndex[1] = nIndex[0];
         glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, nIndex);
         glIndexi(GetNearestPaletteIndex(hPal, RGB(128, 128, 128)));
      }
      else
      {
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)gm_pfChassisColor);
         glMaterialfv(GL_FRONT, GL_SPECULAR, (float*) gm_pfSpecular);
         glColor3fv((float*)gm_pfChassisColor);
      }
   }

   REPORT("Spheres");
   for (i=0;i<m_nSpheres;i++)                               // und hier alle KugelChassis !
   {
      glPushMatrix();
      glTranslated(Vx(m_pvSphereLocations[i]), Vy(m_pvSphereLocations[i]), Vz(m_pvSphereLocations[i]));
      CEts3DGL::Draw(ETS3D_SPHERE|ETS3D_SOLID_FRAME|nDetail, m_pfSphereRadius[i]);
      glPopMatrix();
   }

   if (m_bShowURP && !bDrawShadow)
   {
      float pfColor[] = {1.0, 0.0, 0.0};
      if (bLight)
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pfColor);
      else
         glColor3fv(pfColor);

      glPushMatrix();
      glTranslated(Vx(m_vURP), Vy(m_vURP)+0.011, Vz(m_vURP));
      CEts3DGL::Draw(ETS3D_CUBE|ETS3D_SOLID_FRAME, 0.02f);
      glPopMatrix();
   }
}

bool CBoxObj::MoveToPosition(int nPos)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      glMultMatrixf(pB->m_pfMatrix);
      glTranslated(pB->m_pdPos[0], pB->m_pdPos[1], pB->m_pdPos[2]);
      glRotatef((float) pB->m_nPhi  ,0.0, 1.0, 0.0);
      glRotatef((float)-pB->m_nTheta,1.0, 0.0, 0.0); // theta und xi in OpenGL umgedreht
      glRotatef((float) pB->m_nPsi  ,0.0, 0.0, 1.0);
      return true;
   }

   return false;
}

void CBoxObj::DrawSelection(bool bLight, int nPos)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB && pB->m_bSelected)
   {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      float pfColor[] = {0.8f, 0.0f, 0.8f, 0.6f};
      if (bLight) glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (float*)pfColor);
      glColor4fv((float*)pfColor);
      CEts3DGL::Draw(ETS3D_SPHERE|ETS3D_SOLID_FRAME|2, (float)(Betrag(m_vMax-m_vMin)*0.5));
      glDisable(GL_BLEND);
   }
}

bool CBoxObj::SetMatrix(int nPos, double *pdM)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      int i;
      for (i=0; i<16; i++) pB->m_pfMatrix[i] = (float) pdM[i];
      return true;
   }
   return false;
}

bool CBoxObj::GetMatrix(int nPos, double *pdM)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      int i;
      for (i=0; i<16; i++) pdM[i] = (double) pB->m_pfMatrix[i];
      return true;
   }
   return false;
}


bool CBoxObj::AddPosition(CARAWALK_BoxPos& Bnew)
{
   CBoxPosition *pB = new CBoxPosition;
   if (pB)
   {
      m_BoxPosition.ADDHead(pB);
      pB->m_nPhi     = Bnew.nPhi;
      pB->m_nPsi     = Bnew.nPsi;
      pB->m_nTheta   = Bnew.nTheta;
      pB->m_pdPos[0] = Bnew.pdPos[0];
      pB->m_pdPos[1] = Bnew.pdPos[1];
      pB->m_pdPos[2] = Bnew.pdPos[2];
      return true;
   }
   return false;
}
void CBoxObj::DeleteAllPositions()
{
   m_BoxPosition.Destroy();
}

bool CBoxObj::SetPosition(int nPos, CVector vPos, long nTheta, long nPhi, long  nPsi)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      pB->m_pdPos[0] = Vx(vPos);
      pB->m_pdPos[1] = Vy(vPos);
      pB->m_pdPos[2] = Vz(vPos);
      pB->m_nTheta   = nTheta;
      pB->m_nPsi     = nPsi;
      pB->m_nPhi     = nPhi;
      return true;
   }
   return false;
}

bool CBoxObj::SetPosition(int nPos, CARAWALK_BoxPos &BPos)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      pB->m_nPhi     = BPos.nPhi;
      pB->m_nPsi     = BPos.nPsi;
      pB->m_nTheta   = BPos.nTheta;
      pB->m_pdPos[0] = BPos.pdPos[0];
      pB->m_pdPos[1] = BPos.pdPos[1];
      pB->m_pdPos[2] = BPos.pdPos[2];
      return true;
   }
   return false;
}

void CBoxObj::Select(int nPos, bool bSelect)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      pB->m_bSelected = bSelect;
   }
}

bool CBoxObj::GetPosition(int nPos, CARAWALK_BoxPos &BPos)
{
   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   {
      BPos.nPhi     = pB->m_nPhi;
      BPos.nPsi     = pB->m_nPsi;
      BPos.nTheta   = pB->m_nTheta;
      BPos.pdPos[0] = pB->m_pdPos[0];
      BPos.pdPos[1] = pB->m_pdPos[1];
      BPos.pdPos[2] = pB->m_pdPos[2];
      return true;
   }
   return false;
}

CBoxPosition *CBoxObj::GetpPosition(int nPos)
{
   CBoxPosition*pPos = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   return pPos;
}

bool CBoxObj::ReadType(HANDLE hFile, CFileHeader *pFH)
{
   char     szName[32];
   long     nDummy, i;
   try
   {
      if (!pFH) throw (DWORD)FILE_HEADER_ERROR;
      CCaraWalkDll::ReadFileFH(hFile, &m_nBoxType, sizeof(int)    , pFH);
      CCaraWalkDll::ReadFileFH(hFile, &m_nRotate , sizeof(int)    , pFH);
      CCaraWalkDll::ReadFileFH(hFile, m_szBoxName, sizeof(char)*32, pFH);

      for (i=0; i<16; i++) // Dummys lesen und Checksumme berechnen
      {
         CCaraWalkDll::ReadFileFH(hFile, &nDummy, sizeof(long), pFH);
      }
   
      CCaraWalkDll::ReadFileFH(hFile, &nDummy, sizeof(long), pFH);
      for (i=0; i<nDummy; i++) // nFiles lesen und Checksumme berechnen
      {
         CCaraWalkDll::ReadFileFH(hFile, szName, sizeof(char)*32, pFH);
      }
   }
   catch (DWORD nError)
   {
      REPORT("ReadType()-Error: %08x", nError);
      return false;
   }
   return true;
}

bool CBoxObj::ReadData(HANDLE hFile, CFileHeader *pFH)
{
   int      i, nPoints, nPoly, nPolyTotal, m;
   float    pf[3];

   try
   {
      if (!pFH) throw (DWORD)FILE_HEADER_ERROR;
      if (hFile == INVALID_HANDLE_VALUE) throw (DWORD)ERR_INVALID_HANDLE;
      CCaraWalkDll::ReadFileFH(hFile, &m_nBoxPoints      , sizeof(int), pFH);
      CCaraWalkDll::ReadFileFH(hFile, &m_nPolyPoints     , sizeof(int), pFH);
      CCaraWalkDll::ReadFileFH(hFile, &m_nPolygons       , sizeof(int), pFH);
      CCaraWalkDll::ReadFileFH(hFile, &m_nChassisPolygons, sizeof(int), pFH);
      CCaraWalkDll::ReadFileFH(hFile, &m_nSpheres        , sizeof(int), pFH);

      m_pvPolyPoints = new CVector[m_nPolyPoints];

      for (i=0; i<m_nPolyPoints; i++)
      {
         CCaraWalkDll::ReadFileFH(hFile, &pf[0], sizeof(float)*3, pFH);
         m_pvPolyPoints[i] = CVector(pf[1], pf[2], pf[0]); // nach OpenGL transformieren !
      }

      nPolyTotal  = GetNoOfPolygons();
      m_pPolygons = new SPolygon[nPolyTotal];

      nPoly = 0;
      for (i=0; i<nPolyTotal; i++)
      {	
         CCaraWalkDll::ReadFileFH(hFile, &nPoints, sizeof(int), pFH);
         m_pPolygons[nPoly].nPoints  = nPoints;
         m_pPolygons[nPoly].pnPoints = new int[nPoints];
         m_pPolygons[nPoly].nShapePoints  = 0;
         m_pPolygons[nPoly].pvShapePoints = NULL;

         CCaraWalkDll::ReadFileFH(hFile, m_pPolygons[nPoly].pnPoints, sizeof(int)*nPoints, pFH);

         CVector vPn, vP1, vP2, vP3;
         for (m=2; m<nPoints; m++)
         {
            vP1 = m_pvPolyPoints[m_pPolygons[nPoly].pnPoints[m-2]];
            vP2 = m_pvPolyPoints[m_pPolygons[nPoly].pnPoints[m-1]];
            vP3 = m_pvPolyPoints[m_pPolygons[nPoly].pnPoints[m  ]];
            vPn = Product(vP2-vP1, vP3-vP1);
//            vPn = Product(vP1-vP2, vP3-vP1);
            double db = Betrag(vPn);
            if (db > 1e-13)                                       // Betrag != 0
            {
               vPn /= db;
               break;
            }
         }
         m_pPolygons[nPoly].vFlnv = vPn;
         nPoly++;
      }
      if (m_nSpheres > 0)
      {
         m_pvSphereLocations = new CVector[m_nSpheres];
         m_pfSphereRadius    = new float[m_nSpheres];

         for (i=0; i<m_nSpheres; i++)
         {
            CCaraWalkDll::ReadFileFH(hFile, pf, sizeof(float)*3, pFH);
            m_pvSphereLocations[i] = CVector(pf[1],pf[2], pf[0]); // gleich nach OpenGL transformieren !!
            CCaraWalkDll::ReadFileFH(hFile, &m_pfSphereRadius[i], sizeof(float), pFH);
         }
      }
      CCaraWalkDll::ReadFileFH(hFile, pf, sizeof(float)*3, pFH);
      m_vZDRP = CVector(pf[1], pf[2], pf[0]);

      CCaraWalkDll::ReadFileFH(hFile, pf, sizeof(float)*3, pFH);
      m_vURP = CVector(pf[1], pf[2], pf[0]);
   }
   catch (DWORD nError)
   {
      REPORT("ReadData()-Error : %08x", nError);
      return false;
   }
   CalcMinMaxDimension();
   return true;
}

bool CBoxObj::ReadMaterial(HANDLE hFile, CFileHeader *pFH, CCaraWalkDll*pCW)
{
   long i, nMats = GetNoOfPolygons();
   BYTE bReadMaterial;
   CMaterial *pM = NULL;
   bool bReturn  = true;
   long *pnFind  = new long[nMats+1];
   try
   {
      if (pnFind==0) throw (DWORD)1;

      for (i=0; i<=nMats; i++)                                 // auch das globale Material
      {                                                        // Flags lesen
         CCaraWalkDll::ReadFileFH(hFile, &bReadMaterial, sizeof(BYTE), pFH);
         if (bReadMaterial & MATERIAL_DATA)
         {                                                     // Materialnummer lesen
            CCaraWalkDll::ReadFileFH(hFile, &pnFind[i], sizeof(long), pFH);
         }
         else pnFind[i] = -1;
         
         if ((i>=m_nPolygons) && (i<nMats) &&                  // hier nur die Chassispolygone
             (bReadMaterial & SHAPE_POLYGON_DATA))
         {                                                     // Membranform lesen (max 30 Punkte !)
            CCaraWalkDll::ReadFileFH(hFile, &m_pPolygons[i].nShapePoints, sizeof(int), pFH);
            if ((m_pPolygons[i].nShapePoints>0) && (m_pPolygons[i].nShapePoints<30))
            {                                                  // eine höhere Punktanzahl ist nicht sinnvoll,
               double pdTemp[2];                               // da sie in der Grafik kaum zu erkennen wäre.
               int n;
               m_pPolygons[i].pvShapePoints = new CVector2D[m_pPolygons[i].nShapePoints];
               for (n=0; n<m_pPolygons[i].nShapePoints; n++)
               {
                  CCaraWalkDll::ReadFileFH(hFile, pdTemp, sizeof(double)*2, pFH);
                  m_pPolygons[i].pvShapePoints[n].SetX() = pdTemp[0];
                  m_pPolygons[i].pvShapePoints[n].SetY() = pdTemp[1];
               }
            }
            else
            {
               ASSERT(false);                                  // Fehler: danach kommt wohl nur noch Müll !
               throw (DWORD)2;                                 // abhauen
            }
         }
      }
      CEtsList list;
      list.SetDestroyMode(false);                              // Anzahl der Materialien lesen
      CCaraWalkDll::ReadFileFH(hFile, &nMats, sizeof(long), pFH);
      for (i=0; i<nMats; i++)
      {                                                        // Flags lesen
         CCaraWalkDll::ReadFileFH(hFile, &bReadMaterial, sizeof(BYTE), pFH);
         pM = NULL;
         if (bReadMaterial & MATERIAL_TEXTURE_IN_FILE)         // Lesen der Textur
         {
            pM = pCW->GetMaterial(NULL, true);
            pM->ReadFile(hFile);
         }
         else if (bReadMaterial & MATERIAL_TEXTURE_FROM_FILE)  // Lesen der Bitmap
         {
            CMaterial m;
            m.ReadFile(hFile);
            char *pszName = (char*)m.GetBitmapFileName();
            pM = pCW->GetMaterial(pszName, (pszName==NULL) ? true : false);
            if (pM)
            {
               pM->SetBitmapFileName(m.GetBitmapFileName());
               memcpy(pM->m_pfAmbient , m.m_pfAmbient, sizeof(float)*4);
               memcpy(pM->m_pfDiffuse , m.m_pfDiffuse, sizeof(float)*4);
               memcpy(pM->m_pfSpecular, m.m_pfSpecular, sizeof(float)*4);
               memcpy(pM->m_pfEmision , m.m_pfEmision, sizeof(float)*4);
               pM->m_fShininess           = m.m_fShininess;
               pM->m_bBorder              = m.m_bBorder;
               pM->m_fsScaleFactor        = m.m_fsScaleFactor;
               pM->m_ftScaleFactor        = m.m_ftScaleFactor;
               pM->m_bGenerateCoordinates = m.m_bGenerateCoordinates;
               pM->m_cBorder              = m.m_cBorder;
               pM->m_bSaveBmpAsFile       = m.m_bSaveBmpAsFile;
            }
         }
         if (pM) list.ADDHead(pM);                             // Material am Ende der Liste einfügen
      }
      if (nMats > 0)
      {
         nMats = GetNoOfPolygons();
         for (i=0; i<=nMats; i++)
         {                                                     // Gültige Positon in der Liste ?
            if (pnFind[i]!=-1)                                 // Material setzen
               SetMaterial(i, (CMaterial*)list.GetAt(pnFind[i]));
         }
      }
   }
   catch (DWORD nError)
   {
      REPORT("Error Reading Material: %d", nError);
      if (nError) bReturn = false;
   }

   if (pnFind) delete[] pnFind;                                // Array mit Materialnummern löschen

   return bReturn;
}

bool CBoxObj::WriteMaterial(HANDLE hFile, CFileHeader *pFH)
{
   long i, nFind, nMats = GetNoOfPolygons();
   BYTE bWriteMaterial;
   CEtsList list;
   list.SetDestroyMode(false);
   for (i=0; i<=nMats; i++)                                     // Einzelmaterialien für die Polygone
   {
      bWriteMaterial = 0;
      CMaterial *pM = GetMaterial(i);
      if (pM)                               bWriteMaterial |= MATERIAL_DATA;
      if (m_pPolygons[i].nShapePoints != 0) bWriteMaterial |= SHAPE_POLYGON_DATA;
                                                               // Flags speichern
      CCaraWalkDll::WriteFileFH(hFile, &bWriteMaterial, sizeof(BYTE), pFH);
      if (bWriteMaterial & MATERIAL_DATA)
      {
         nFind = list.FindElement(NULL, pM);                   // Materialnummer suchen
         if (nFind == -1)                                      // nicht gefunden ?
         {
            list.ADDHead(pM);                                  // Material am Ende der Liste einfügen
            nFind = list.GetCounter()-1;                       // Materialnummer ist n-1
         }
         CCaraWalkDll::WriteFileFH(hFile, &nFind, sizeof(long), pFH);
      }
      if (bWriteMaterial & SHAPE_POLYGON_DATA)
      {
         double pdTemp[2];
         CCaraWalkDll::WriteFileFH(hFile, &m_pPolygons->nShapePoints, sizeof(int), pFH);
         int n;
         for (n=0; n<m_pPolygons->nShapePoints; n++)
         {
            pdTemp[0] = Vx(m_pPolygons->pvShapePoints[n]);
            pdTemp[1] = Vy(m_pPolygons->pvShapePoints[n]);
            CCaraWalkDll::WriteFileFH(hFile, pdTemp, sizeof(double)*2, pFH);
         }
      }
   }
   nFind = list.GetCounter();                                  // Anzahl unterschiedlicher Materialien
   CCaraWalkDll::WriteFileFH(hFile, &nFind, sizeof(long), pFH);// speichern
   for (i=0; i<nFind; i++)
   {
      CMaterial *pM = (CMaterial*) list.GetAt(i);
      if (pM)                                                  // Materialien speichern
      {
         bWriteMaterial = 0;
         if (pM->m_bSaveBmpAsFile)                             // Speichern des Dateinamens
            bWriteMaterial |= MATERIAL_TEXTURE_FROM_FILE;
         else                                                  // Speichern der Textur
            bWriteMaterial |= MATERIAL_TEXTURE_IN_FILE;         
         CCaraWalkDll::WriteFileFH(hFile, &bWriteMaterial, sizeof(BYTE), pFH);
         pM->SaveFile(hFile);
      }
   }
   return true;
}

void CBoxObj::CalcMinMaxDimension(int nPos, CVector *pvMin, CVector *pvMax, bool bInit, double dScale)
{
   int     i, j;
   CVector  vSphere, vTest, vMin, vMax;
   double *pdPoint    = (double*)&vTest,
          *pdMin      = (double*)&vMin,
          *pdMax      = (double*)&vMax;
   bool    bTransform = false;
   Array44 m;

   CBoxPosition *pB = (CBoxPosition*)m_BoxPosition.GetAt(nPos);
   if (pB)
   if ((pB != NULL) && (pvMin != NULL) && (pvMax != NULL))
   {
      bTransform = true;
      LoadIdentity();
      for (i=0; i<16; i++) ((double*)&m)[i] = pB->m_pfMatrix[i];
      MultMatrix(m);
      Translate(*((CVector*)pB->m_pdPos));
      Rotate_y(RAD( pB->m_nPhi));
      Rotate_x(RAD(-pB->m_nTheta));
      Rotate_z(RAD( pB->m_nPsi));
   }
   if (bTransform)
   {
      if (bInit)
      {
         vTest = m_pvPolyPoints[0];
         TransformVector(vTest);
         m_vMYRP = vTest;
         vMin = vMax = vTest;
      }
      else
      {
         vMin = *pvMin;
         vMax = *pvMax;
      }
   }
   else vMin = vMax = m_pvPolyPoints[0];

   for (i=1; i<m_nPolyPoints; i++)
   {
      vTest = m_pvPolyPoints[i];
      if (bTransform) TransformVector(vTest);

      for (j=0; j<3; j++)
      {
         if (pdMin[j] > pdPoint[j]) pdMin[j] = pdPoint[j];
         if (pdMax[j] < pdPoint[j])
         {
            pdMax[j] = pdPoint[j];
            if (bTransform && (j==1)) m_vMYRP = vTest;
         }
      }
   }

   if (bTransform)
   {
      for (i=0; i<m_nPolyPoints; i++)
      {
         vTest = m_pvPolyPoints[i];
         TransformVector(vTest);
         if (fabs(Vy(vTest)-Vy(m_vMYRP)) < 1e-7)
         {
            if (Vz(vTest) > Vz(m_vMYRP))
            {
               m_vMYRP.SetZ() = Vz(vTest);
               m_vMYRP.SetX() = Vx(vTest);
            }
         }
      }
   }

   if ((!bTransform) && (Vx(m_vURP)==0.0) && (Vy(m_vURP)==0.0) && (Vz(m_vURP)==0.0))
   {
      double dZmaxYmin = 1e-32;
      for (i=0; i<m_nPolyPoints; i++)
      {
         vTest = m_pvPolyPoints[i];
         if (fabs(Vy(vTest)-Vy(vMin)) < 1e-7)
         {
            if (dZmaxYmin < Vz(vTest)) dZmaxYmin = Vz(vTest);
         }
      }
      m_vURP = CVector(Vx(m_vZDRP), Vy(vMin), dZmaxYmin);
   }
   
   for (i=0; i<m_nSpheres; i++)
   {
      double r = (double) m_pfSphereRadius[i] * dScale;
      CVector vR(r, r, r);
      vSphere = m_pvSphereLocations[i];
      if (bTransform) TransformVector(vSphere);
      vSphere += vR;
      pdPoint = (double*) &vSphere;
      for (j=0; j<3; j++)
      {
         if (pdMax[j] < pdPoint[j])
         {
            pdMax[j] = pdPoint[j];
            if ((bTransform) && (j==1))
            {
               m_vMYRP = m_pvSphereLocations[i];
               TransformVector(m_vMYRP);
               m_vMYRP.SetY() = pdMax[j];
            }
         }
      }

      vSphere = m_pvSphereLocations[i];
      if (bTransform) TransformVector(vSphere);
      vSphere -= vR;
      pdPoint = (double*) &vSphere;
      for (j=0; j<3; j++)
      {
         if (pdMin[j] > pdPoint[j]) pdMin[j] = pdPoint[j];
      }
   }
   if (bTransform)
   {
      REPORT("MYRP : %f, %f, %f", Vx(m_vMYRP), Vy(m_vMYRP), Vz(m_vMYRP));
      *pvMin = vMin;
      *pvMax = vMax;
   }
   else
   {
      m_vMin = vMin;
      m_vMax = vMax;
   }
}

void CBoxObj::SetMaterial(CMaterial * pM)
{
   m_pMaterial = pM;
}

void CBoxObj::SetMaterial(int nPoly, CMaterial *pM)
{
   int nMats = GetNoOfPolygons();
   if (nPoly == BOX_MAIN_MATERIAL) nPoly = nMats;
   if (nMats == nPoly) SetMaterial(pM);
   if ((nPoly >= 0) && (nPoly < nMats))
   {
      if (m_ppMaterial == NULL)
      {
         int nSize    = sizeof(CMaterial*)*nMats;
         m_ppMaterial = (CMaterial**) new BYTE[nSize];
         ZeroMemory(m_ppMaterial, nSize);
      }
      m_ppMaterial[nPoly] = pM;
   }
}

CMaterial *CBoxObj::GetMaterial(int nPoly)
{
   int nMats = GetNoOfPolygons();
   if (nPoly == BOX_MAIN_MATERIAL) nPoly = nMats;
   if (nPoly == nMats) return m_pMaterial;
   if ((nPoly >= 0) && (nPoly < nMats))
   {
      if (m_ppMaterial)
      {
         return m_ppMaterial[nPoly];
      }
   }
   return NULL;
}

void CBoxObj::DeleteFunction(void *p)
{
   delete ((CBoxObj*)p);
}

CVector CBoxObj::SelectPolygon(int nPoly)
{
   int nMats = GetNoOfPolygons();
   if ((nPoly >= 0) && (nPoly < nMats))
   {
      m_nSelectedPolygon = nPoly;
      return m_pPolygons[m_nSelectedPolygon].vFlnv;
   }
   else
   {
      m_nSelectedPolygon = -1;
   }
   return CVector(0,0,0);
}
