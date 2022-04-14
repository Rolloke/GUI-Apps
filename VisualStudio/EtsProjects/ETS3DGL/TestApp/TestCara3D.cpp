// TestCara3D.cpp: Implementierung der Klasse CTestCara3D.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "TestCara3D.h"
#include "Dibsection.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"


#define LIGHT_COMMAND 1
#define LIST_MATRIX   2
#define LIST_OBJECT   3
#define LIST_LIGHT    4


extern HINSTANCE g_hInstance;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CTestCara3D::CTestCara3D()
{
   BEGIN("CTestCara3D");
   InitTest();
   m_pTexImage = NULL;
   m_nWidth    = 0;
   m_nHeight   = 0;
   m_nPhi      = 0;
   m_bRun      = true;
}
void CTestCara3D::InitTest()
{
   m_vObjectPos            = CVector(0.0, 0.0, 3.5);
   m_Light.m_nLight        = GL_LIGHT0;
   m_Light.m_bEnabled      = true;
   m_Light.SetPosition(-40.0, 0.0, 30.0, 1.0);
   m_Light.m_fSpotCutOff   = 20.0;
   m_Light.m_fSpotExponent = 55.0;
   m_Light.SetAmbientColor( 0.1f, 0.1f, 0.1f, 1.0f);
   m_Light.SetDiffuseColor( 1.0f, 1.0f, 1.0f, 1.0f);
   m_Light.SetSpecularColor(0.4f, 0.4f, 0.4f, 1.0f);
   SetVolume(-3.5f, 3.5f, 3.5f, -3.5f, 0.1f, 10.0f);
   SetAlpha(60.0);
}

CTestCara3D::CTestCara3D(CTestCara3D *pC3D) : CEts3DGL(pC3D)
{
   InitTest();
   m_nPhi = pC3D->m_nPhi;
   m_bRun = false;
   m_pTexImage = pC3D->m_pTexImage;
   m_nWidth    = pC3D->m_nWidth;
   m_nHeight   = pC3D->m_nHeight;
}

void CTestCara3D::LoadBitmap(char *pszFile)
{
   HANDLE hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      REPORT("File opened");
      CDibSection Dib;
      if (Dib.LoadFromFile(hFile))
      {
         if (m_pTexImage)
         {
            delete[] m_pTexImage;
            m_pTexImage = NULL;
         }
         REPORT("File loaded");
         BITMAPINFO *pBmi    = (BITMAPINFO*) Dib.GetBitmapInfo();
         BITMAPINFO *pNewBmi = NULL;
         int nBitCount = pBmi->bmiHeader.biBitCount;
         int i, j, k = 0;
         COLORREF color;
         m_nWidth = Dib.GetWidth();
         for (i=-1; m_nWidth!=0; m_nWidth>>=1, i++);
         m_nWidth  = 1 << i;

         m_nHeight = Dib.GetHeight();
         for (i=-1; m_nHeight!=0; m_nHeight>>=1, i++);
         m_nHeight = 1 << i;
         m_pTexImage = new BYTE[m_nWidth*m_nHeight*3];
         for (i=0; i<m_nHeight; i++)
         {
            for (j=0; j<m_nWidth; j++)
            {  
               color = Dib.GetPixel(j, i);
               m_pTexImage[k++] = GetBValue(color);
               m_pTexImage[k++] = GetGValue(color);
               m_pTexImage[k++] = GetRValue(color);
            }
         }
      }
      CloseHandle(hFile);
   }
}

CTestCara3D::~CTestCara3D()
{
//   BEGIN("~CTestCara3D");
   Destroy();
   if (m_pTexImage) delete[] m_pTexImage;
}

void CTestCara3D::InitGLLists()
{
   BEGIN("InitGLLists");
   DWORD nModes = GetModes();
   if (nModes & ETS3D_CM_RGB)
   {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_2D);

      if (m_pTexImage)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, 3, m_nWidth, m_nHeight,
                      false, GL_RGB, GL_UNSIGNED_BYTE, m_pTexImage);
      }
   }

   // Lichtwerte
   glEnable(GL_LIGHTING);
   if (m_Light.m_bEnabled) glEnable(m_Light.m_nLight);
   glLightfv(m_Light.m_nLight, GL_AMBIENT      , m_Light.m_pfAmbientColor);
   glLightfv(m_Light.m_nLight, GL_DIFFUSE      , m_Light.m_pfDiffuseColor);
   glLightfv(m_Light.m_nLight, GL_SPECULAR     , m_Light.m_pfSpecularColor);
   glLightf( m_Light.m_nLight, GL_SPOT_EXPONENT, m_Light.m_fSpotExponent);
   glLightf( m_Light.m_nLight, GL_SPOT_CUTOFF  , m_Light.m_fSpotCutOff);

   // Materialwerte
   float fColor[] = { 0.5f, 0.6f, 0.5f, 1.0f};
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fColor);
   float fSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f};
   glMaterialfv(GL_FRONT, GL_SPECULAR, fSpecular);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

   SetClearBits(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   CreateList(LIST_LIGHT , (LISTFUNCTION)CTestCara3D::ListLight , 1);
   CreateList(LIST_MATRIX, (LISTFUNCTION)CTestCara3D::ListMatrix, 1);
   CreateList(LIST_OBJECT, (LISTFUNCTION)CTestCara3D::ListObject, 1);

   return;
}

void CTestCara3D::AutoDelete()
{
   if (GetModes() & ETS3D_DETACH_FOREIGN_DATA) DetachData();
   CEts3DGL::AutoDelete();
}

void CTestCara3D::OnDrawToRenderDC(HDC hdc, int nOff)
{
   char text[] = "22.5";//"Dies ist eine Textausgabe in den OpenGL-DC";
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, RGB(255,255,255));
   int x = 300, y = 100;
   TextOut(hdc, x, nOff-y, text, strlen(text));
}

void CTestCara3D::DetachData()
{
   m_pTexImage = NULL;
}

void CTestCara3D::DestroyGLLists()
{
   BEGIN("DestroyGLLists");
   return;
}

void CTestCara3D::OnRenderSzene()
{
   glLoadIdentity();
   CEts3DGL::OnRenderSzene();
}

LRESULT CTestCara3D::OnTimer(long nTimerID, BOOL bValidate3D)
{
   if (m_bRun)
   {
      m_nPhi += 1;
      if (m_nPhi > 359) m_nPhi = 0;
      InvalidateList(LIST_MATRIX);
   }
   return 0;
}

void CTestCara3D::LightCommand(CTestCara3D *pC, int, int)
{
   BEGIN("LightCommand");
}

void CTestCara3D::ListMatrix(CTestCara3D *pC, int nListID, int nRange)
{
   BEGIN("ListMatrix");
   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
   glTranslated(-Vx(pC->m_vObjectPos), -Vy(pC->m_vObjectPos), -Vz(pC->m_vObjectPos));
   glRotated((double)pC->m_nPhi, 0.0, 1.0, 0.0);
   glEndList();
}

void CTestCara3D::ListObject(CTestCara3D *pC, int nListID, int nRange)
{
   BEGIN("ListObject");
   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
   CEts3DGL::Draw(ETS3D_SPHERE|ETS3D_SOLID_FRAME|4, 1.5);
   glEndList();
}

void CTestCara3D::ListLight(CTestCara3D *pC, int nListID, int nRange)
{
   BEGIN("ListLight");
   pC->m_Light.SetDirection(pC->m_vObjectPos - pC->m_Light.GetPostion());
   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
   glLightfv(pC->m_Light.m_nLight, GL_POSITION , pC->m_Light.m_pfPosition);
   glLightfv(pC->m_Light.m_nLight, GL_SPOT_DIRECTION, pC->m_Light.m_pfDirection);
   glEndList();
}

bool CTestCara3D::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_LBUTTONDOWN:
      {
         CVector point((double)HIWORD(lParam), (double)LOWORD(lParam), 0);
         REPORT("Mc(%d, %d)", LOWORD(lParam), HIWORD(lParam));
         TransformPoints(&point, 1, false);
         REPORT("Vc(%.2f, %.2f, %.2f)", Vx(point),Vy(point),Vz(point));
         return true;
      }
      case WM_MOUSEMOVE:
         return true;
      case WM_CONTEXTMENU:
      {
         DWORD nModes = GetModes();
         HMENU hMenu    = ::LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT));
         HMENU hContext = ::GetSubMenu(hMenu, 0);
         int nID = 0;
         if      (nModes & ETS3D_CM_RGB)  nID = ID_CONTEXT_COLORMODES_RGB;
         else if (nModes & ETS3D_CM_4096) nID = ID_CONTEXT_COLORMODES_4096COLORS;
         else if (nModes & ETS3D_CM_256)  nID = ID_CONTEXT_COLORMODES_256COLORS;
         ::CheckMenuRadioItem(GetSubMenu(hContext, 0), ID_CONTEXT_COLORMODES_RGB, ID_CONTEXT_COLORMODES_256COLORS, nID, MF_BYCOMMAND);

         if      (nModes & ETS3D_DC_DIRECT_1) nID = ID_CONTEXT_RENDERMODES_GENERIC;
         else if (nModes & ETS3D_DC_DIRECT_2) nID = ID_CONTEXT_RENDERMODES_GENERICDOUBLEBUFFER;
         else if (nModes & ETS3D_DC_DIB_1)    nID = ID_CONTEXT_RENDERMODES_DIB;
         else if (nModes & ETS3D_DC_DIB_2)    nID = ID_CONTEXT_RENDERMODES_DIBDOUBLEBUFFER;
         ::CheckMenuRadioItem(GetSubMenu(hContext, 1), ID_CONTEXT_RENDERMODES_DIB, ID_CONTEXT_RENDERMODES_GENERICDOUBLEBUFFER, nID, MF_BYCOMMAND);
         if (!IsBmpFormatAvailable())
         {
            EnableMenuItem(GetSubMenu(hContext, 1), ID_CONTEXT_RENDERMODES_DIB, MF_BYCOMMAND|MF_GRAYED);
            EnableMenuItem(GetSubMenu(hContext, 1), ID_CONTEXT_RENDERMODES_DIBDOUBLEBUFFER, MF_BYCOMMAND|MF_GRAYED);
         }
         TrackPopupMenu(hContext, 0, LOWORD(lParam), HIWORD(lParam), 0, GetHWND(), NULL);
         DestroyMenu(hMenu);
      }
      default: break;
   }
   return false;
}

LRESULT CTestCara3D::OnCommand(WORD wID, WORD wNotify, HWND hwndFrom)
{
   UINT nRender = 0, nColorM = 0;
   switch(wID)
   {
      case ID_OPTIONEN_ANIMATION:                      m_bRun = !m_bRun; break;
      case ID_CONTEXT_COLORMODES_RGB:                  nColorM |= ETS3D_CM_RGB;      break;
      case ID_CONTEXT_COLORMODES_4096COLORS:           nColorM |= ETS3D_CM_4096;     break;
      case ID_CONTEXT_COLORMODES_256COLORS:            nColorM |= ETS3D_CM_256;      break;
      case ID_CONTEXT_RENDERMODES_DIB:                 nRender |= ETS3D_DC_DIB_1;    break;
      case ID_CONTEXT_RENDERMODES_DIBDOUBLEBUFFER:     nRender |= ETS3D_DC_DIB_2;    break;
      case ID_CONTEXT_RENDERMODES_GENERIC:             nRender |= ETS3D_DC_DIRECT_1; break;
      case ID_CONTEXT_RENDERMODES_GENERICDOUBLEBUFFER: nRender |= ETS3D_DC_DIRECT_2; break;
      default:  break;
   }
   UINT nModes = GetModes();
   if (nRender && (nRender != (nModes&ETS3D_DC_CLEAR)))
   {
      nModes &= ~ETS3D_DC_CLEAR;
      nModes |= nRender;
      ::PostMessage(GetHWND(), WM_CHANGE_3DMODES, nModes, (LPARAM)this);
   }
   nModes = GetModes();
   if (nColorM && (nColorM != (nModes&ETS3D_CM_CLEAR)))
   {
      UINT nModes = GetModes();
      nModes &= ~ETS3D_CM_CLEAR;
      nModes |= nColorM;
      ::PostMessage(GetHWND(), WM_CHANGE_3DMODES, nModes, (LPARAM)this);
   }
   return 1;
}

DWORD CTestCara3D::GetThreadID()
{
   return 0;//m_nThreadID;
}
