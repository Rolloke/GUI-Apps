// CARA3DDocFriend.cpp: Implementierung der Klasse CCARA3DDocFriend.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CARA3DDocFriend.h"
#include "CARA3DVDoc.h"
#include "LevelColor.h"
#include "3DGrid.h"
#include "Ets3DGL.h"
#include "ObjectTransformation.h"
#include "PlanGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCARA3DDocFriend::CCARA3DDocFriend()
{
   m_pDoc = NULL;
}
void CCARA3DDocFriend::SetDocument(CCARA3DVDoc *p)
{
   m_pDoc = p;
}

// Zugriff auf LevelColors
CLevelColor* CCARA3DDocFriend::GetLevelColors()
{
   ASSERT(m_pDoc != NULL);
//   m_pDoc->m_LevelColor.Validate();
   return &m_pDoc->m_LevelColor;
}

// Zugriff auf Gitterobjekt
C3DGrid* CCARA3DDocFriend::GetGrid()
{
   ASSERT(m_pDoc != NULL);
   return &m_pDoc->m_Grid;
}

// Zugriff auf Lichtobjekt(e)
CGlLight* CCARA3DDocFriend::GetLight(int nID)
{
   ASSERT(m_pDoc != NULL);
   if ((nID < 0) || (nID >= NO_OF_LIGHTS)) nID = 0;
   return &m_pDoc->m_Light[nID].glLight;
}

bool CCARA3DDocFriend::IsALightOn()
{
   ASSERT(m_pDoc != NULL);
   for (int i=0; i<NO_OF_LIGHTS; i++)
   {
      if (m_pDoc->m_Light[i].glLight.m_bEnabled) return true;
   }
   return false;
}

// Zugriff auf Transformationsobjekt
CObjectTransformation* CCARA3DDocFriend::GetObjectTransformation()
{
   ASSERT(m_pDoc != NULL);
   return &m_pDoc->m_ObjTran;
}

// Zugriff auf Darstellungsobjekt
int CCARA3DDocFriend::GetObjectType()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject)
   {
      return m_pDoc->m_p3DObject->GetClass3DType();
   }
   return 0;
}
void CCARA3DDocFriend::DefaultLevel()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject)
   {
      m_pDoc->m_p3DObject->DefaultLevel();
   }
}

void CCARA3DDocFriend::DrawObject()
{
   ASSERT(m_pDoc != NULL);

   if (m_pDoc->m_p3DObject)
   {
      m_pDoc->m_p3DObject->DrawObject();
   }
}

void CCARA3DDocFriend::DrawPinsText(HDC hDC, int nOff)
{
   ASSERT(m_pDoc != NULL);
   m_pDoc->m_Pins.DrawPinsText(hDC, nOff);
}

const ETS3DGL_Volume * CCARA3DDocFriend::GetObjectVolume() const
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetObjectVolume();
   return NULL;
}

double CCARA3DDocFriend::GetNormFactor()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetNormFactor();
   return 1.0;
}

double CCARA3DDocFriend::GetInverseNormFactor()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetInverseNormFactor();
   return 1.0;
}

double CCARA3DDocFriend::GetOffset()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetOffset();
   return 1.0;
}

double CCARA3DDocFriend::GetObjectDimension()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetMaxDimension();
   return 1.0;
}

void CCARA3DDocFriend::SetObjectColor(COLORREF color)
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject)
   {
      if (m_pDoc->m_p3DObject->GetClass3DType() == PLAN_GRAPH)
      {
         float fColor[4] = {0,0,0,1};
         ((CPlanGraph*)m_pDoc->m_p3DObject)->SetColor(color);
         CGlLight::ColorToFloat(color, fColor);
         CGlLight::MultnFloat(3, fColor, 0.8f);
         ((CPlanGraph*)m_pDoc->m_p3DObject)->SetAmbientAndDiffuse(CGlLight::FloatsToColor(fColor));
      }
   }
}

COLORREF CCARA3DDocFriend::GetObjectColor()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject)
   {
      if (m_pDoc->m_p3DObject->GetClass3DType() == PLAN_GRAPH)
         return ((CPlanGraph*)m_pDoc->m_p3DObject)->GetColor();
   }
   return RGB(255, 255, 255);
}

int CCARA3DDocFriend::GetNoOfPictures()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetNoOfPictures();
   return 1;
}

const char * CCARA3DDocFriend::GetSpecificUnit()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetSpecificUnit();
   return SPACE_STRING;
}

const char * CCARA3DDocFriend::GetLevelUnit()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetLevelUnit();
   return SPACE_STRING;
}

const char * CCARA3DDocFriend::GetUnit()
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetUnit();
   return SPACE_STRING;
}

double CCARA3DDocFriend::GetSpecificValue(int nIndex)
{
   ASSERT(m_pDoc != NULL);
   if (m_pDoc->m_p3DObject) return m_pDoc->m_p3DObject->GetSpecificValue(nIndex);
   return (double) nIndex;
}

// Zugriff auf Pins
bool CCARA3DDocFriend::ShowPins(int nType)
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_Pins.ShowPins(nType);
}

void CCARA3DDocFriend::ShowPins(int nType, bool b)
{
   ASSERT(m_pDoc != NULL);
   m_pDoc->m_Pins.ShowPins(nType, b);
}

int CCARA3DDocFriend::GetNoOfPins(int nType)
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_Pins.GetNoOfPins(nType);
}

void CCARA3DDocFriend::DrawPins()
{
   ASSERT(m_pDoc != NULL);
   m_pDoc->m_Pins.DrawPins();
}

// Zugriff auf Dokumentendaten
bool CCARA3DDocFriend::Is3DObjectAvailable()
{
   if (m_pDoc)
   {
      if (m_pDoc->m_p3DObject) return true;
   }
   return false;
}

bool CCARA3DDocFriend::AreLabelsAvailable()
{
   ASSERT(m_pDoc != NULL);
   return (m_pDoc->GetLabelContainer()->GetCount() > 0) ? true : false;
}

int CCARA3DDocFriend::GetPictureIndex()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_nPictureIndex;
}

void CCARA3DDocFriend::SetPictureIndex(int nI)
{
   ASSERT(m_pDoc != NULL);
   m_pDoc->m_nPictureIndex = nI;
}

int CCARA3DDocFriend::GetDetail()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_nDetail;
}

bool CCARA3DDocFriend::DoInterpolate()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bInterpolate;
}

bool CCARA3DDocFriend::NormalBug()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bNormalBug;
}

void CCARA3DDocFriend::ProtectDocData()
{
   ASSERT(m_pDoc != NULL);
   EnterCriticalSection(m_pDoc->m_pcsDocumentData);
}

void CCARA3DDocFriend::ReleaseDocData()
{
   ASSERT(m_pDoc != NULL);
   LeaveCriticalSection(m_pDoc->m_pcsDocumentData);
}

bool CCARA3DDocFriend::SmoothShading()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bSmoothShading;
}

bool CCARA3DDocFriend::Lighting()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bLighting;
}
bool CCARA3DDocFriend::LightTwoSided()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bLightTwoSided;
}

bool CCARA3DDocFriend::CullFace()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_bCullFace;
}

float* CCARA3DDocFriend::GetAmbientColor()
{
   ASSERT(m_pDoc != NULL);
   return m_pDoc->m_pfAmbientColor;
}


