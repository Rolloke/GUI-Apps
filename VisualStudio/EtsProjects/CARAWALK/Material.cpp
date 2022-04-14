// Material.cpp: Implementierung der Klasse CMaterial.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Material.h"
#include "CVector.h"
#include "CaraWalkDll.h"
#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

float CMaterial::gm_pfAmbient[4]  = {0.7f, 0.7f, 0.7f, 1.0f}; // Umgebungslichtkomponente des Materials
float CMaterial::gm_pfDiffuse[4]  = {0.7f, 0.7f, 0.7f, 1.0f}; // Diffuse Lichtkomponente des Materials
float CMaterial::gm_pfSpecular[4] = {0.5f, 0.5f, 0.5f, 1.0f}; // Reflexionseigenschaften des Materials

float CMaterial::gm_fb       = 0.2f;                          // Komprimierung des Intensitätsbereichs von 0.2 bis 0.7
float CMaterial::gm_fm       = 0.5f;
float CMaterial::gm_fAmbient = 0.5;                           // Faktor für Umgebungslichtkomponente des Materials

CMaterial::CMaterial()
{
   BEGIN("CMaterial()");
   ZeroMemory(m_pfAverageImageColor, sizeof(float)*3);
   memcpy(&m_pfAmbient, &gm_pfAmbient, sizeof(float)*4);
   memcpy(&m_pfDiffuse, &gm_pfDiffuse, sizeof(float)*4);
   ZeroMemory(m_pfSpecular, sizeof(float)*4);
   m_pfSpecular[3] = 1.0f;                           // Reflexionskomponente
   ZeroMemory(m_pfEmision , sizeof(float)*4);
   m_fShininess     = 0;
   m_nWidth         = 0;
   m_nHeight        = 0;
   m_bBorder        = 0;
   m_cBorder        = 0;
   m_fsScaleFactor  = 0;
   m_ftScaleFactor  = 0;
   m_pszFileName    = NULL;
   m_pTexImage      = NULL;
   m_nTexture       = 0;
   nMatNo           = -1;
   m_bSaveBmpAsFile = false;
   m_bGenerateCoordinates = true;
   m_Walls.SetDestroyMode(false);
   m_Images.SetDestroyMode(true);
}

CMaterial::~CMaterial()
{
   BEGIN("~CMaterial()");
   if (m_pszFileName) delete[] m_pszFileName;
   if (m_pTexImage)   delete[] m_pTexImage;
   REPORT("MatNo %d", nMatNo);
   REPORT("Walls %d", m_Walls.GetCounter());
   REPORT("Images %d", m_Images.GetCounter());
}
void  CMaterial::SetBitmapFileName(const char *pszFn)
{
   if (m_pszFileName) {delete[] m_pszFileName; m_pszFileName = NULL;}
   if ((pszFn != NULL) && (pszFn[0] != 0))
   {
      int nLen = strlen(pszFn)+1;
      m_pszFileName = new char[nLen+1];
      strcpy(m_pszFileName , pszFn);
   }
}

bool CMaterial::SetTexture(BITMAPINFO *pBmi)
{
   BEGIN("SetTexture");
   BITMAPINFOHEADER *pBmiH = &pBmi->bmiHeader;
   int i, j, k = 0, l = 0;
   COLORREF *pColors = (COLORREF*) &pBmi->bmiColors[0];

   if (m_pTexImage)
   {
      delete[] m_pTexImage;
      m_pTexImage = NULL;
      m_nWidth    = 0;
      m_nHeight   = 0;
   }

   if (!CCaraWalkDll::gm_bColorIndex)
   {
      m_pTexImage = new BYTE[pBmiH->biWidth*pBmiH->biHeight*3];
      if (!m_pTexImage) return false;
      m_nWidth  = pBmiH->biWidth;
      m_nHeight = pBmiH->biHeight;
   }

   m_fsScaleFactor = 2.0f;                           // Texturgröße x
   m_ftScaleFactor = 2.0f;                           // Texturgröße y

   BYTE  *pTexImage      = m_pTexImage;
   float *pfAverageColor = m_pfAverageImageColor;
   pfAverageColor[0] = 0;
   pfAverageColor[1] = 0;
   pfAverageColor[2] = 0;
   for (i=0; i<pBmiH->biWidth; i++)
   {
      for (j=0; j<pBmiH->biHeight; j++)
      {
         COLORREF color     = pColors[k++];
         pfAverageColor[0] += GetBValue(color);
         pfAverageColor[1] += GetGValue(color);
         pfAverageColor[2] += GetRValue(color);
         if (pTexImage == NULL) continue;
         pTexImage[l++]     = GetBValue(color);
         pTexImage[l++]     = GetGValue(color);
         pTexImage[l++]     = GetRValue(color);
      }
   }
   if (CCaraWalkDll::gm_bColorIndex)
   {
      pfAverageColor[0] = pfAverageColor[0] / k;
      pfAverageColor[1] = pfAverageColor[1] / k;
      pfAverageColor[2] = pfAverageColor[2] / k;
   }
   else
   {
      k *= 255;
      pfAverageColor[0] = pfAverageColor[0] / k;
      pfAverageColor[1] = pfAverageColor[1] / k;
      pfAverageColor[2] = pfAverageColor[2] / k;

      pfAverageColor[0] = gm_fb + gm_fm * pfAverageColor[0];
      pfAverageColor[1] = gm_fb + gm_fm * pfAverageColor[1];
      pfAverageColor[2] = gm_fb + gm_fm * pfAverageColor[2];
      memcpy(m_pfAmbient, m_pfAverageImageColor, sizeof(float)*3);
      memcpy(m_pfDiffuse, m_pfAverageImageColor, sizeof(float)*3);
      CGlLight::MultnFloat(3, m_pfAmbient, gm_fAmbient);
   }

   return true;
}

void CMaterial::ImageMaterialAndLight(bool bTexture, bool bLight, HPALETTE hPal, float fAlpha)
{
   if (bTexture && m_pTexImage)                                // Texturen anzeigen, ja :
   {                                                           // die Texturparameter einstellen
      glEnable(GL_TEXTURE_2D);
      if (bLight) glColor4f(0.5f, 0.5f, 0.5f, fAlpha);         // Beleuchtung an
      else        glColor4f(1.0f, 1.0f, 1.0f, fAlpha);         // aus
      
      if (IsTextureObject())
      {
         glBindTexture((m_nHeight == 0) ? GL_TEXTURE_1D : GL_TEXTURE_2D, m_nTexture);
         ASSERT(glGetError() == GL_NO_ERROR);
      }
      else
      {
         if (m_nHeight == 0)
            glTexImage1D(GL_TEXTURE_1D, 0, 3, m_nWidth, m_bBorder, GL_RGB, GL_UNSIGNED_BYTE, m_pTexImage);
         else
            glTexImage2D(GL_TEXTURE_2D, 0, 3, m_nWidth, m_nHeight, m_bBorder, GL_RGB, GL_UNSIGNED_BYTE, m_pTexImage);
         float fBorder[4] = {0, 0, 0, 1};
#ifdef _DEBUG
         GLenum ge = glGetError();
         ASSERT(ge==0);
#endif
         CGlLight::ColorToFloat(m_cBorder, fBorder);
         if (m_bBorder)
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, fBorder);
//      glTexImage3D(GL_TEXTURE_3D, 0, 3, m_nWidth, m_nHeight, 0, m_bBorder, GL_RGB, GL_UNSIGNED_BYTE, m_pTexImage);
      }
   }
   else
   {
      glDisable(GL_TEXTURE_2D);
      if (CCaraWalkDll::gm_bColorIndex && hPal)              // ColorIndex-Mode 
      {
         COLORREF c = CGlLight::FloatsToColor(m_pfAverageImageColor);
         glIndexi(GetNearestPaletteIndex(hPal, c));
      }
      else                                                        // Durchschnittsfarbe des Materials einstellen
      {
         glColor4f(m_pfAverageImageColor[0], m_pfAverageImageColor[1], m_pfAverageImageColor[2], fAlpha);
      }
   }

   if (bLight)                                                 // Beleuchtung an :
   {                                                           // die Materialparameter für die Beleuchtung einstellen
      if (bTexture && m_pTexImage)
      {
         glMaterialfv(GL_FRONT, GL_AMBIENT , (float*)m_pfAmbient);
         glMaterialfv(GL_FRONT, GL_DIFFUSE , (float*)m_pfDiffuse);
         glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)m_pfSpecular);
         glMaterialfv(GL_FRONT, GL_EMISSION, (float*)m_pfEmision);
         glMaterialf( GL_FRONT, GL_SHININESS, m_fShininess);
      }
      else if (CCaraWalkDll::gm_bColorIndex)                   // ColorIndex-Mode
      {
         glMaterialfv(GL_FRONT, GL_COLOR_INDEXES, m_pfAverageImageColor);
      }
      else                                                     // RGB-Mode
      {
         glMaterialfv(GL_FRONT, GL_AMBIENT , (float*)m_pfAmbient);
         glMaterialfv(GL_FRONT, GL_DIFFUSE , (float*)m_pfDiffuse);
         glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)m_pfSpecular);
         glMaterialfv(GL_FRONT, GL_EMISSION, (float*)m_pfEmision);
         glMaterialf( GL_FRONT, GL_SHININESS, m_fShininess);
      }
   }
}

void CMaterial::AlignTexture(CVector &vN)
{
   double pdSParams[4], pdTParams[4], dTheta, dPhi;
   dTheta = M_PI * 0.5 - acos(Vy(vN));
   dPhi   = atan2(Vx(vN), Vz(vN));

   pdSParams[0] =  cos(dPhi) * (double)m_fsScaleFactor;
   pdSParams[1] =  0;
   pdSParams[2] = -sin(dPhi) * (double)m_fsScaleFactor;
   pdSParams[3] = 1.0;

   pdTParams[0] = -sin(dTheta) * sin(dPhi) * (double)m_ftScaleFactor;
   pdTParams[1] =  cos(dTheta) *             (double)m_ftScaleFactor;
   pdTParams[2] = -sin(dTheta) * cos(dPhi) * (double)m_ftScaleFactor;
   pdTParams[3] = 1.0;

   glTexGendv(GL_S, GL_OBJECT_PLANE, pdSParams);
   glTexGendv(GL_T, GL_OBJECT_PLANE, pdTParams);
// REPORT("Texturen S:(%.2f, %.2f, %.2f), T:(%.2f, %.2f, %.2f)\n",pdSParams[0], pdSParams[1], pdSParams[2], pdTParams[0], pdTParams[1], pdTParams[2]);
}

void CMaterial::RandomMaterial()
{
   if (m_pszFileName) {delete[] m_pszFileName; m_pszFileName = NULL;}
   if (m_pTexImage)
   {
      delete[] m_pTexImage;
      m_pTexImage = NULL;
      m_nWidth    = 0;
      m_nHeight   = 0;
   }
   float factor = 0.000030517578125;
   m_pfAverageImageColor[0] = factor * rand();
   m_pfAverageImageColor[1] = factor * rand();
   m_pfAverageImageColor[2] = factor * rand();
   memcpy(m_pfAmbient, m_pfAverageImageColor, sizeof(float)*3);
   memcpy(m_pfDiffuse, m_pfAverageImageColor, sizeof(float)*3);
}

void CMaterial::DeleteFunction(void *p)
{
   CMaterial* pM = (CMaterial*)p;
   if (pM->IsTextureObject())
      glDeleteTextures(1, &pM->m_nTexture);
   delete pM;
}

bool CMaterial::IsTextureObject()
{
   return ((m_nTexture != 0) && (glIsTexture(m_nTexture) == GL_TRUE)) ? true : false;
}

void CMaterial::CreateTextureObject()
{
   if (!m_nTexture)
   {
      glTexImage2D(GL_TEXTURE_2D, 0, 3, m_nWidth, m_nHeight, m_bBorder, GL_RGB, GL_UNSIGNED_BYTE, m_pTexImage);
      glGenTextures(1, &m_nTexture);
      glBindTexture(GL_TEXTURE_2D, m_nTexture);
   }
}

bool CMaterial::ReadFile(HANDLE hFile)
{
   if (hFile != INVALID_HANDLE_VALUE)
   {
      int nLen = 0;
      DWORD dwRead;
      ::ReadFile(hFile, m_pfAmbient            , sizeof(float)*4 , &dwRead, NULL);
      ::ReadFile(hFile, m_pfDiffuse            , sizeof(float)*4 , &dwRead, NULL);
      ::ReadFile(hFile, m_pfSpecular           , sizeof(float)*4 , &dwRead, NULL);
      ::ReadFile(hFile, m_pfEmision            , sizeof(float)*4 , &dwRead, NULL);
      ::ReadFile(hFile, &m_fShininess          , sizeof(float)   , &dwRead, NULL);
      ::ReadFile(hFile, &m_bBorder             , sizeof(BOOL)    , &dwRead, NULL);
      ::ReadFile(hFile, &m_fsScaleFactor       , sizeof(float)   , &dwRead, NULL);
      ::ReadFile(hFile, &m_ftScaleFactor       , sizeof(float)   , &dwRead, NULL);
      ::ReadFile(hFile, &m_bGenerateCoordinates, sizeof(BOOL)    , &dwRead, NULL);
      ::ReadFile(hFile, &m_cBorder             , sizeof(COLORREF), &dwRead, NULL);
      ::ReadFile(hFile, &nLen                  , sizeof(int)     , &dwRead, NULL);

      if ((nLen > 0) && (nLen<_MAX_PATH))                      // Dateiname für die Textur
      {
         m_pfAverageImageColor[0] = (m_pfAmbient[0] + m_pfDiffuse[0])*0.5f;
         m_pfAverageImageColor[1] = (m_pfAmbient[1] + m_pfDiffuse[1])*0.5f;
         m_pfAverageImageColor[2] = (m_pfAmbient[2] + m_pfDiffuse[2])*0.5f;
         m_pszFileName = new char[nLen+1];
         ::ReadFile(hFile, m_pszFileName, nLen, &dwRead, NULL);
         m_pszFileName[nLen] = 0;
         m_bSaveBmpAsFile = true;
      }
      else if (nLen == -1)                                     // Textur lesen
      {                                                        // Durchschnittsfarbe
         ::ReadFile(hFile, m_pfAverageImageColor, sizeof(float)*3, &dwRead, NULL);
         ::ReadFile(hFile, &m_nWidth , sizeof(int), &dwRead, NULL);// Größe der Textur
         ::ReadFile(hFile, &m_nHeight, sizeof(int), &dwRead, NULL);
         nLen = m_nWidth * m_nHeight*3;                        // berechnen
         if (nLen > 0)
         {
            m_pTexImage = new BYTE[nLen];
            ::ReadFile(hFile, m_pTexImage, nLen, &dwRead, NULL);
         }
         ::ReadFile(hFile, &nLen, sizeof(int), &dwRead, NULL); // Dateiname für die Textur
         if ((nLen > 0) && (nLen<_MAX_PATH))
         {
            m_pszFileName = new char[nLen+1];
            ::ReadFile(hFile, m_pszFileName, nLen, &dwRead, NULL);
            m_pszFileName[nLen] = 0;
         }
      }
      return true;
   }
   return false;
}

bool CMaterial::SaveFile(HANDLE hFile)
{
   if (hFile != INVALID_HANDLE_VALUE)
   {
      // Texturparameter
      int nLen = 0;
      DWORD dwRead;
      ::WriteFile(hFile, m_pfAmbient            , sizeof(float)*4 , &dwRead, NULL);
      ::WriteFile(hFile, m_pfDiffuse            , sizeof(float)*4 , &dwRead, NULL);
      ::WriteFile(hFile, m_pfSpecular           , sizeof(float)*4 , &dwRead, NULL);
      ::WriteFile(hFile, m_pfEmision            , sizeof(float)*4 , &dwRead, NULL);
      ::WriteFile(hFile, &m_fShininess          , sizeof(float)   , &dwRead, NULL);
      ::WriteFile(hFile, &m_bBorder             , sizeof(BOOL)    , &dwRead, NULL);
      ::WriteFile(hFile, &m_fsScaleFactor       , sizeof(float)   , &dwRead, NULL);
      ::WriteFile(hFile, &m_ftScaleFactor       , sizeof(float)   , &dwRead, NULL);
      ::WriteFile(hFile, &m_bGenerateCoordinates, sizeof(BOOL)    , &dwRead, NULL);
      ::WriteFile(hFile, &m_cBorder             , sizeof(COLORREF), &dwRead, NULL);

      if (m_bSaveBmpAsFile)                                    // Texturdaten aus der BMP-Datei lesen lassen
      {
         nLen = strlen(m_pszFileName);
         ::WriteFile(hFile, &nLen, sizeof(int), &dwRead, NULL);
         ::WriteFile(hFile, m_pszFileName, nLen, &dwRead, NULL);
      }
      else                                                     // Texturdaten selbst abspeichern
      {
         nLen = -1;
         ::WriteFile(hFile, &nLen     , sizeof(int), &dwRead, NULL);
         ::WriteFile(hFile, m_pfAverageImageColor, sizeof(float)*3, &dwRead, NULL);
         ::WriteFile(hFile, &m_nWidth , sizeof(int), &dwRead, NULL);
         ::WriteFile(hFile, &m_nHeight, sizeof(int), &dwRead, NULL);
         nLen = m_nWidth * m_nHeight*3;
         if (nLen > 0)
         {
            ::WriteFile(hFile, m_pTexImage, nLen, &dwRead, NULL);
         }
         if (m_pszFileName) nLen = strlen(m_pszFileName);      // BMP-Dateinamen zur Identifikation
         else               nLen = 0;
         ::WriteFile(hFile, &nLen, sizeof(int), &dwRead, NULL);
         ::WriteFile(hFile, m_pszFileName, nLen, &dwRead, NULL);
      }
      return true;
   }
   return false;
}
