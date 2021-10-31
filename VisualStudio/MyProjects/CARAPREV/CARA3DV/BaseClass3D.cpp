// BaseClass3D.cpp: Implementierung der Klasse CBaseClass3D.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseClass3D.h"

#include "Cara3DGL.h"
#include "LevelColor.h"
#include "Pins.h"

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

CBaseClass3D::CBaseClass3D()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CBaseClass3D()");
#endif
   m_nClass3DType  = 0;
   m_nFilmPictures = 0;
   m_nFlags        = 0;
   SetNormFactor(1.0);
   m_dOffset       = 0.0;
   m_dMinValue     = 0.0;
   m_dMaxValue     = 0.0;
   m_bClipped      = false;
}

CBaseClass3D::~CBaseClass3D()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("~CBaseClass3D()");
#endif
}

// Zeichnen
void CBaseClass3D::FlatLevelTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float  fColor[4]    = {0, 0, 0, 1};
   double dLevel       = (Vy(*pV1)+Vy(*pV2)+Vy(*pV3))/3.0;
   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((dLevel-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);

   glVertex3dv((double*)pV1);
   glVertex3dv((double*)pV2);
   glVertex3dv((double*)pV3);
}
void CBaseClass3D::FlatLightLevelTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT((pN1!=NULL) && (pN2!=NULL) && (pN3!=NULL));
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float fColor[4]     = {0, 0, 0, 1};
   double dLevel       = (Vy(*pV1)+Vy(*pV2)+Vy(*pV3))/3.0;
   CVector vNorm       = Norm((*pN1)+(*pN2)+(*pN3));

   glNormal3dv((double*)&vNorm);

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((dLevel-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   CGlLight::MultnFloat(3, fColor, LIGHT_COLOR_FACTOR);

   if (pBC3D->LightTwoSided())
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, fColor);
   else
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fColor);

   glVertex3dv((double*)pV1);
   glVertex3dv((double*)pV2);
   glVertex3dv((double*)pV3);
}
void CBaseClass3D::SmoothLevelTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float fColor[4]     = {0, 0, 0, 1};

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Vy(*pV1)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV1);
   
   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Vy(*pV2)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV2);

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Vy(*pV3)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV3);
}

void CBaseClass3D::SmoothLightLevelTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT((pN1!=NULL) && (pN2!=NULL) && (pN3!=NULL));
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   CVector *pV[3]      = {pV1, pV2, pV3};
   CVector *pN[3]      = {pN1, pN2, pN3};
   float fColor[4]     = {0, 0, 0, 1};

   for (int i=0; i<3; i++)
   {
      glNormal3dv((double*)pN[i]);
      CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Vy(*pV[i])-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
      glColor4fv(fColor);
      CGlLight::MultnFloat(3, fColor, LIGHT_COLOR_FACTOR);
      if (pBC3D->LightTwoSided())
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, fColor);
      else
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fColor);
      glVertex3dv((double*)pV[i]);
   }
}

void CBaseClass3D::FlatRadiusTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float  fColor[4]    = {0, 0, 0, 1};
   double dLevel = Betrag(*pV1 + *pV2 + *pV3) / 3.0;
   dLevel = (dLevel-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor;

   COLORREF col = pBC3D->GetLevelColors()->GetColorFromLevel(dLevel);
   CGlLight::ColorToFloat(col, fColor);
   glColor4fv(fColor);

   glVertex3dv((double*)pV1);
   glVertex3dv((double*)pV2);
   glVertex3dv((double*)pV3);
}
void CBaseClass3D::FlatLightRadiusTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT((pN1!=NULL) && (pN2!=NULL) && (pN3!=NULL));
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float   fColor[4] = {0, 0, 0, 1};
   CVector vNorm     = Norm((*pN1)+(*pN2)+(*pN3));
   double  dLevel    = Betrag(*pV1 + *pV2 + *pV3) / 3.0;
   dLevel = (dLevel-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor;

   glNormal3dv((double*)&vNorm);

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel(dLevel), fColor);
   glColor4fv(fColor);
   CGlLight::MultnFloat(3, fColor, LIGHT_COLOR_FACTOR);

   if (pBC3D->LightTwoSided())
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, fColor);
   else
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fColor);

   glVertex3dv((double*)pV1);
   glVertex3dv((double*)pV2);
   glVertex3dv((double*)pV3);
}

void CBaseClass3D::SmoothRadiusTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   float fColor[4]     = {0, 0, 0, 1};

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Betrag(*pV1)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV1);
   
   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Betrag(*pV2)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV2);

   CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Betrag(*pV3)-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
   glColor4fv(fColor);
   glVertex3dv((double*)pV3);
}
void CBaseClass3D::SmoothLightRadiusTr(CVector *pV1, CVector *pV2, CVector *pV3, CVector *pN1, CVector *pN2, CVector *pN3, void *pParam)
{
   ASSERT((pN1!=NULL) && (pN2!=NULL) && (pN3!=NULL));
   ASSERT(pParam!=NULL);
   CBaseClass3D *pBC3D = (CBaseClass3D*)pParam;
   CVector *pV[3]      = {pV1, pV2, pV3};
   CVector *pN[3]      = {pN1, pN2, pN3};
   float fColor[4]     = {0, 0, 0, 1};
   double dInvNormFact = 1.0 / pBC3D->GetNormFactor();

   for (int i=0; i<3; i++)
   {
      glNormal3dv((double*)pN[i]);
      CGlLight::ColorToFloat(pBC3D->GetLevelColors()->GetColorFromLevel((Betrag(*pV[i])-pBC3D->m_dOffset)*pBC3D->m_dInvNormFactor), fColor);
      glColor4fv(fColor);
      CGlLight::MultnFloat(3, fColor, LIGHT_COLOR_FACTOR);
      if (pBC3D->LightTwoSided())
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, fColor);
      else
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fColor);
      glVertex3dv((double*)pV[i]);
   }
}

const ETS3DGL_Volume *CBaseClass3D::GetObjectVolume()
{
   if (m_ObjectVolume.Left == m_ObjectVolume.Right)
   {
      CheckMinMax();
   }
   return &m_ObjectVolume;
}

void CBaseClass3D::SetNormFactor(double dNorm)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CBaseClass3D::SetNormFactor");
#endif
   if (dNorm == 1.0)
   {
      m_dNormFactor = m_dInvNormFactor = dNorm;
   }
   else if (dNorm != 0)
   {
      m_dNormFactor    = dNorm;
      m_dInvNormFactor = 1.0 / dNorm;
   }
   else
   {
      ASSERT(false);
      m_dNormFactor = m_dInvNormFactor = 1.0;
   }
}

void  CBaseClass3D::DefaultLevel()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CBaseClass3D::DefaultLevel");
#endif
   GetLevelColors()->SetMinMaxLevel(m_dMinValue, m_dMaxValue);
}

int CBaseClass3D::GetNoOfPictures()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CBaseClass3D::GetNoOfPictures");
#endif
   if (PhaseValuesCalculated())
   {
      return NO_OF_PHASEVALUES;
   }
   else
   {
      return (m_nFilmPictures!=0) ? m_nFilmPictures : 1;
   }
}

void CBaseClass3D::InvalidateVolume()
{
   m_ObjectVolume.Left = m_ObjectVolume.Right = 0;
}
