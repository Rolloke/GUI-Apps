// GlLight.cpp: Implementierung der Klasse CGlLight.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ETS3DGL.h"
#include "CVector.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CGlLight::CGlLight()
{
   m_nLight = 0;
   Init();
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CGlLight::CGlLight(int nLight)
{
   m_nLight = nLight;
   Init();
}

void CGlLight::Init()
{
   m_bEnabled = true;
   SetPosition( 0, 0,  1,0);
   SetDirection(0, 0, -1);
   SetAmbientColor( 0.7f, 0.7f, 0.7f, 1.0);
   SetDiffuseColor( 0.7f, 0.7f, 0.7f, 1.0);
   SetSpecularColor(0.0f, 0.0f, 0.0f, 1.0);
   SetAttenuation(1, 0, 0);
   m_fSpotCutOff   = 60;
   m_fSpotExponent = 2.56f;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
BYTE CGlLight::FloatToByte(float f)
{
   float fCol = f * 255;
   short nCol = (BYTE) fCol;
   if ((fCol - (float)nCol) >= 0.5f) nCol++;
   if (nCol > 255) nCol = 255;
   if (nCol <   0) nCol =   0;
   return (BYTE)nCol;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
COLORREF CGlLight::FloatsToColor(float *pfColor)
{
   return RGB(FloatToByte(pfColor[0]), FloatToByte(pfColor[1]), FloatToByte(pfColor[2]));
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
float *CGlLight::ColorToFloat(COLORREF Color, float* fColor)
{
   ASSERT(fColor != NULL);
   float fFactor = 1.0f / 255.0f;
   fColor[0] = fFactor * GetRValue(Color);
   fColor[1] = fFactor * GetGValue(Color);
   fColor[2] = fFactor * GetBValue(Color);
   return fColor;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetPosition(float x, float y, float z, float w)
{
   m_pfPosition[0] = x;
   m_pfPosition[1] = y;
   m_pfPosition[2] = z;
   m_pfPosition[3] = w;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetPosition(float *p)
{
   memcpy(m_pfPosition, p, sizeof(float)*4);
}
void CGlLight::SetPosition(CVector &vPos, float w)
{
   m_pfPosition[0] = (float)Vx(vPos);
   m_pfPosition[1] = (float)Vy(vPos);
   m_pfPosition[2] = (float)Vz(vPos);
   m_pfPosition[3] = w;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CVector CGlLight::GetPostion()
{
   CVector vPos(m_pfPosition[0], m_pfPosition[1], m_pfPosition[2]);
   return vPos;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetDirection(float x, float y, float z)
{
   m_pfDirection[0] = x;
   m_pfDirection[1] = y;
   m_pfDirection[2] = z;
}
void CGlLight::SetDirection(float *p)
{
   memcpy(m_pfDirection, p, sizeof(float)*3);
}
void CGlLight::SetDirection(CVector &vD)
{
   m_pfDirection[0] = (float)Vx(vD);
   m_pfDirection[1] = (float)Vy(vD);
   m_pfDirection[2] = (float)Vz(vD);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CVector CGlLight::GetDirection()
{
   CVector vDir(m_pfDirection[0], m_pfDirection[1], m_pfDirection[2]);
   return vDir;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetAttenuation(float constant, float linear, float quadratic)
{
   m_fAttenuation[0] = constant;
   m_fAttenuation[1] = linear;
   m_fAttenuation[2] = quadratic;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetAmbientColor(COLORREF color)
{
   m_pfAmbientColor[0] = INV_256(GetRValue(color));
   m_pfAmbientColor[1] = INV_256(GetGValue(color));
   m_pfAmbientColor[2] = INV_256(GetBValue(color));
}
void CGlLight::SetAmbientColor(float r, float g, float b, float a)
{
   m_pfAmbientColor[0] = r;
   m_pfAmbientColor[1] = g;
   m_pfAmbientColor[2] = b;
   m_pfAmbientColor[3] = a;
}
void CGlLight::SetAmbientColor(float *p)
{
   memcpy(m_pfAmbientColor, p, sizeof(float)*4);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
COLORREF CGlLight::GetAmbientColor()
{
   return FloatsToColor(m_pfAmbientColor);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetDiffuseColor(COLORREF color)
{
   m_pfDiffuseColor[0] = INV_256(GetRValue(color));
   m_pfDiffuseColor[1] = INV_256(GetGValue(color));
   m_pfDiffuseColor[2] = INV_256(GetBValue(color));
}
void CGlLight::SetDiffuseColor(float r, float g, float b, float a)
{
   m_pfDiffuseColor[0] = r;
   m_pfDiffuseColor[1] = g;
   m_pfDiffuseColor[2] = b;
   m_pfDiffuseColor[3] = a;
}
void CGlLight::SetDiffuseColor(float *p)
{
   memcpy(m_pfDiffuseColor, p, sizeof(float)*4);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
COLORREF CGlLight::GetDiffuseColor()
{
   return FloatsToColor(m_pfDiffuseColor);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::SetSpecularColor(COLORREF color)
{
   m_pfSpecularColor[0] = INV_256(GetRValue(color));
   m_pfSpecularColor[1] = INV_256(GetGValue(color));
   m_pfSpecularColor[2] = INV_256(GetBValue(color));
}
void CGlLight::SetSpecularColor(float r, float g, float b, float a)
{
   m_pfSpecularColor[0] = r;
   m_pfSpecularColor[1] = g;
   m_pfSpecularColor[2] = b;
   m_pfSpecularColor[3] = a;
}
void CGlLight::SetSpecularColor(float *p)
{
   memcpy(m_pfSpecularColor, p, sizeof(float)*4);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
COLORREF CGlLight::GetSpecularColor()
{
   return FloatsToColor(m_pfSpecularColor);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CGlLight::MultnFloat(int nCount, float *p, float fFact)
{
   for (int i=0; i<nCount; i++) p[i] *= fFact;
}

