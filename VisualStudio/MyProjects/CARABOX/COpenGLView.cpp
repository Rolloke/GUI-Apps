// Cara3DGL.cpp: Implementierung der Klasse COpenGLView.
//
//////////////////////////////////////////////////////////////////////

#include "COpenGLView.h"
#include "BoxPropertySheet.h"
#include "Debug.h"
#include "Cara3DTr.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "resource.h"

#include <GL/gl.h>            // Open-Gl Headerdateien
#include <GL/glu.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern COLORREF  g_rgbPickColor;
extern HINSTANCE g_hInstance;
extern int       g_nUserMode;

#define ASSERT_GL_ERROR   ASSERT(glGetError()   == GL_NO_ERROR )
#define ASSERT_LAST_ERROR ASSERT(GetLastError() == 0)

#define REG_PERSPECTIVE    "Perspective"
#define REG_LIGHT_PHI      "LightPhi"
#define REG_LIGHT_THETA    "LightTheta"
#define REG_LIGHT_DIST     "LightDist"
#define REG_LIGHT_INTENS   "LightIntens"
#define REG_LIGHT_AMBIENT  "LightAmbient"
#define REG_LIGHT_SPECULAR "LightSpecular"
#define REG_SPOT_EXP       "SpotExp"
#define REG_SPOT_COF       "SpotCutOff"
#define REG_LIGHTING       "Lighting"
#define REG_ALPHA_BLEND    "bAlphaBlend"
#define REG_ALPHA_BLEND_F  "fAlphaBlend"
#define REG_SCALE_X        "ScaleX"
#define REG_SCALE_Y        "ScaleY"
#define REG_SCALE_Z        "ScaleZ"
#define REG_COL_BOX        "ColBox"
#define REG_COL_CHASSIS    "ColChassis"
#define REG_COL_SCALE      "ColScale"
#define REG_COL_BKGND      "ColBkGnd"
#define REG_COL_SPECULAR   "ColSpecMat"
#define REG_MAT_SHININES   "MatShinines"
#define REG_DETAIL         "Detail"


COpenGLView::COpenGLView()
{
   InitGLView();
   HKEY hKey = NULL;
   CGlLight light;
   // Registryparameter (extern einstellbar) GL-Setup
   m_nPolygonOffset   = 1;
   m_fPolygonFactor   = 0.1f;
   char szRegKey[_MAX_PATH];
   wsprintf(szRegKey, "%s\\%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY, REGKEY_BUGFIX);
   if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
   {
//      m_bBugFixNormalVector = GetRegDWord(hKey, REGKEY_BUGFIX_NORMALVECTOR, gm_bBugFixNormalVector) ? true : false;
//      m_bBugFixLightTexture = GetRegDWord(hKey, REGKEY_BUGFIX_LIGHTTEXTURE, gm_bBugFixLightTexture) ? true : false;
      m_nPolygonOffset      = GetRegDWord(hKey, REGKEY_BUGFIX_POLYGON_OFFSET, m_nPolygonOffset);
      m_fPolygonFactor      = (float)(0.00001 * GetRegDWord(hKey, REGKEY_BUGFIX_DISTANCE_FACTOR, 5000));
   }
   if (hKey) EtsRegCloseKey(hKey);

   m_Bits.nGeneric = CARAWALK_GENERIC_DEFAULT;
   m_Bits.nGenAccel= CARAWALK_GENERIC_ACC_DEFAULT;
   m_Bits.nColor   = CARAWALK_COLOR_BITS_DEFAULT;
   m_Bits.nDepth   = CARAWALK_DEPTH_BITS_DEFAULT;
   m_Bits.nStencil = CARAWALK_STENCIL_BITS_DEFAULT;
   m_Bits.nAlpha   = CARAWALK_ALPHA_BITS_DEFAULT;
   m_Bits.nAccum   = CARAWALK_ACCUM_BITS_DEFAULT;
   DWORD dwAlpha   = CARAWALK_PERSPECTIVE_ANGLE_DEFAULT;
   SetGlobalRegParam(CARAWALK_REGKEY, NULL, NULL, false);
   if (OpenRegistry(NULL, KEY_READ, &hKey, CARAWALK_GL_SETTINGS)) 
   {  // hInstance = NULL, dann wird die Version des aufrufenden Modules ermittelt
      // d.h. Version 2.1 von CARAWALK !
      m_Bits.nGeneric = GetRegDWord(hKey, CARAWALK_GENERIC     , m_Bits.nGeneric);
      m_Bits.nGenAccel= GetRegDWord(hKey, CARAWALK_GENERIC_ACC , m_Bits.nGenAccel);
      m_Bits.nColor   = GetRegDWord(hKey, CARAWALK_COLOR_BITS  , m_Bits.nColor);
      m_Bits.nDepth   = GetRegDWord(hKey, CARAWALK_DEPTH_BITS  , m_Bits.nDepth);
      m_Bits.nStencil = GetRegDWord(hKey, CARAWALK_STENCIL_BITS, m_Bits.nStencil);
      m_Bits.nAlpha   = GetRegDWord(hKey, CARAWALK_ALPHA_BITS  , m_Bits.nAlpha);
      m_Bits.nAccum   = GetRegDWord(hKey, CARAWALK_ACCUM_BITS  , m_Bits.nAccum);
      SetRefreshTime(   GetRegDWord(hKey, CARAWALK_REFRESH_TIME, CARAWALK_REFRESH_TIME_DEFAULT));
      dwAlpha         = GetRegDWord(hKey, CARAWALK_PERSPECTIVE_ANGLE, dwAlpha);
      SetAlpha((double) dwAlpha);
   
      if (m_Bits.nColor   > 32) m_Bits.nColor   = CARAWALK_COLOR_BITS_DEFAULT;
      if (m_Bits.nDepth   > 32) m_Bits.nDepth   = CARAWALK_DEPTH_BITS_DEFAULT;
      if (m_Bits.nStencil > 32) m_Bits.nStencil = CARAWALK_STENCIL_BITS_DEFAULT;
      if (m_Bits.nAlpha   > 32) m_Bits.nAlpha   = CARAWALK_ALPHA_BITS_DEFAULT;
      if (m_Bits.nAccum   > 32) m_Bits.nAccum   = CARAWALK_ACCUM_BITS_DEFAULT;

      DWORD dwDirect  = GetRegDWord(hKey, REGKEY_DEFAULT_GLDIRECT, CARAWALK_GLDIRECT_DEFAULT);
      DWORD dwBuffer  = GetRegDWord(hKey, REGKEY_DEFAULT_GLDOUBLE, CARAWALK_GLDOUBLE_DEFAULT);
      DWORD dwInit    = GetRegDWord(hKey, CARAWALK_INIT_3D_ALWAYS, CARAWALK_INIT_3D_ALWAYS_DEFAULT);
      DWORD dwFlags   = 0;
      if (dwDirect)
      {
         if (dwBuffer) dwFlags |= ETS3D_DC_DIRECT_2;
         else          dwFlags |= ETS3D_DC_DIRECT_1;
      }
      else
      {
         if (dwBuffer) dwFlags |= ETS3D_DC_DIB_2;
         else          dwFlags |= ETS3D_DC_DIB_1;
      }
      SetModes(dwFlags, ETS3D_DC_CLEAR);
      if (!dwInit) SetModes(ETS3D_DESTROY_AT_DLL_TERMINATION,0);
   }
   else
   {
      SetRefreshTime(42);
      SetAlpha((double) dwAlpha);
   }
   if (hKey) EtsRegCloseKey(hKey);

   m_bPerspective     = false;

   m_dPhiLight        =  0.0;
   m_dThetaLight      =  0.0;
   m_dDistanceLight   = 10.0;
   m_dSpotCutExponent = 20;
   m_dSpotCutOff      = 120;
   
   light.SetDiffuseColor(0.7f, 0.7f, 0.7f, 0);// Lichtintensität !

   m_bLighting   = true;
   light.SetAmbientColor(0.2f, 0.2f, 0.2f, 0);// Umgebungslicht

   m_Light.SetSpecularColor(0.5f, 0.5f, 0.5f, 0);// Glanzeffekt

   m_nAlphaBlend = 2;                         // Blendtyp 
   m_dAlphaBlend = 60;                        // Transparenz 60 %
   m_dShinines   = 50;                        // Glanzeffekt 50 %

   m_nResolution = 2;                         // Auflösung
   m_bShowBackSide = false;

   m_nScaleModeX     = MAKELONG(SCALE_CENTER,SCALE_CENTER);
   m_nScaleModeY     = MAKELONG(SCALE_CENTER,SCALE_CENTER);
   m_nScaleModeZ     = MAKELONG(SCALE_CENTER,SCALE_CENTER);
   m_nScaleModePhi   = MAKELONG(SCALE_CENTER,SCALE_CENTER);
   m_nScaleModeTheta = MAKELONG(SCALE_CENTER,SCALE_CENTER);

   m_cBox        = RGB(  0,  0,255);
   m_cChassis    = RGB(128,128,128);
   m_cScale      = RGB(  0,255,  0);
   m_cSpecular   = RGB(127,127,127);

   // Registryparameter (intern einstellbar)
   SetGlobalRegParam(CARABOX_REGKEY, NULL, NULL, false);
   if (OpenRegistry(g_hInstance, KEY_READ, &hKey, CARAWALK_GL_SETTINGS))
   {  // der Instanzhandle der DLL liefert die Version für den Boxeneditor,
      // um die richtigen Parameter bei Versionssprung zu erhalten

      COLORREF cColor;
//    m_bPerspective = (GetRegDWord(hKey, REG_PERSPECTIVE, m_bPerspective) != 0) ? true : false;

      m_dPhiLight      = 0.1 * GetRegDWord(hKey, REG_LIGHT_PHI  , (int)(10*m_dPhiLight));
      m_dThetaLight    = 0.1 * GetRegDWord(hKey, REG_LIGHT_THETA, (int)(10*m_dThetaLight));
      m_dDistanceLight = 0.1 * GetRegDWord(hKey, REG_LIGHT_DIST , (int)(10*m_dDistanceLight));

      m_nResolution = GetRegDWord(hKey, REG_DETAIL , m_nResolution);
      if (m_nResolution < 1) m_nResolution = 1;
      if (m_nResolution > 5) m_nResolution = 5;

      cColor = CGlLight::FloatsToColor(light.m_pfDiffuseColor);
      cColor = GetRegDWord(hKey, REG_LIGHT_INTENS, cColor);
      light.SetDiffuseColor(cColor);

      cColor = CGlLight::FloatsToColor(light.m_pfAmbientColor);
      cColor = GetRegDWord(hKey, REG_LIGHT_AMBIENT, cColor);
      light.SetAmbientColor(cColor);

      cColor = CGlLight::FloatsToColor(m_Light.m_pfSpecularColor);
      cColor = GetRegDWord(hKey, REG_LIGHT_SPECULAR, cColor);
      m_Light.SetSpecularColor(cColor);

      cColor = GetRegDWord(hKey, REG_COL_BKGND, RGB(255,255,255));
      SetBkColor(cColor);

      m_dSpotCutExponent = 0.1 * GetRegDWord(hKey, REG_SPOT_EXP, (int)(10*m_dSpotCutExponent));
      m_dSpotCutOff      = 0.1 * GetRegDWord(hKey, REG_SPOT_COF, (int)(10*m_dSpotCutOff));
      m_bLighting = (GetRegDWord(hKey, REG_LIGHTING, m_bLighting) != 0) ? true : false;

      m_nAlphaBlend = GetRegDWord(hKey, REG_ALPHA_BLEND  , m_nAlphaBlend);
      m_dAlphaBlend = GetRegDWord(hKey, REG_ALPHA_BLEND_F, (int)m_dAlphaBlend);

//      m_nScaleModeX = GetRegDWord(hKey, REG_SCALE_X, m_nScaleModeX);
//      m_nScaleModeY = GetRegDWord(hKey, REG_SCALE_Y, m_nScaleModeY);
//      m_nScaleModeZ = GetRegDWord(hKey, REG_SCALE_Z, m_nScaleModeZ);

      m_cBox      = GetRegDWord(hKey, REG_COL_BOX     , m_cBox);
      m_cChassis  = GetRegDWord(hKey, REG_COL_CHASSIS , m_cChassis);
      m_cScale    = GetRegDWord(hKey, REG_COL_SCALE   , m_cScale);
      m_cSpecular = GetRegDWord(hKey, REG_COL_SPECULAR, m_cSpecular);
      m_dShinines = GetRegDWord(hKey, REG_MAT_SHININES, (int)(m_dShinines*1.28)) / 1.28;
   }
   if (hKey) EtsRegCloseKey(hKey);

   m_dLightIntens[0] = light.m_pfDiffuseColor[0]*100;
   m_dLightIntens[1] = light.m_pfDiffuseColor[1]*100;
   m_dLightIntens[2] = light.m_pfDiffuseColor[2]*100;
   m_AmbientColor[0] = light.m_pfAmbientColor[0];
   m_AmbientColor[1] = light.m_pfAmbientColor[1];
   m_AmbientColor[2] = light.m_pfAmbientColor[2];

   m_dZoom        = 1;
   m_dVolumeXY    = 1;
   m_dVolumeZ     = 20;
   SetProjection();

   m_pPropertySheet = NULL;

#ifdef UNIT_METER
   m_vTranslation  = CVector(0.0, 0.0, -2.0);
#else
   m_vTranslation  = CVector(0.0, 0.0, -2000.0);
#endif
   m_vScale        = CVector(1.0, 1.0,  1.0);
   m_vAnchor       = CVector(0.0, 0.0,  0.0);
   m_dPhi          = 0.0;
   m_dTheta        = 0.0;
   m_dXi           = 0.0;

   m_Light.SetAmbientColor((float)(m_dLightIntens[0]*0.01), (float)(m_dLightIntens[1]*0.01), (float)(m_dLightIntens[2]*0.01), 1);
   m_Light.SetDiffuseColor(m_Light.m_pfAmbientColor[0], m_Light.m_pfAmbientColor[1], m_Light.m_pfAmbientColor[2], 1.0);
   m_Light.m_nLight = GL_LIGHT0;
   m_nTransMode     = 0;
   m_nScale         = 0;
   m_bChanged[0] = m_bChanged[1] = m_bChanged[2] = false;
}

COpenGLView::~COpenGLView()
{
//   Destroy();
   HKEY hKey = NULL;
   CGlLight light;
   if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, CARAWALK_GL_SETTINGS))
   {
      COLORREF cColor;
//    SetRegDWord(hKey, REG_PERSPECTIVE, m_bPerspective);
      SetRegDWord(hKey, REG_LIGHT_PHI  , (int)(10*m_dPhiLight));
      SetRegDWord(hKey, REG_LIGHT_THETA, (int)(10*m_dThetaLight));
      SetRegDWord(hKey, REG_LIGHT_DIST , (int)(10*m_dDistanceLight));

      light.m_pfDiffuseColor[0] = (float)(m_dLightIntens[0]*0.01);
      light.m_pfDiffuseColor[1] = (float)(m_dLightIntens[1]*0.01);
      light.m_pfDiffuseColor[2] = (float)(m_dLightIntens[2]*0.01);
      cColor = CGlLight::FloatsToColor(light.m_pfDiffuseColor);
      SetRegDWord(hKey, REG_LIGHT_INTENS, cColor);

      light.m_pfAmbientColor[0] = (float)m_AmbientColor[0];
      light.m_pfAmbientColor[1] = (float)m_AmbientColor[1];
      light.m_pfAmbientColor[2] = (float)m_AmbientColor[2];
      cColor = CGlLight::FloatsToColor(light.m_pfAmbientColor);
      SetRegDWord(hKey, REG_LIGHT_AMBIENT, cColor);

      cColor = CGlLight::FloatsToColor(m_Light.m_pfSpecularColor);
      SetRegDWord(hKey, REG_LIGHT_SPECULAR, cColor);

      cColor = GetBkColor();
      SetRegDWord(hKey, REG_COL_BKGND, cColor);

      SetRegDWord(hKey, REG_SPOT_EXP, (int)(10*m_dSpotCutExponent));
      SetRegDWord(hKey, REG_SPOT_COF, (int)(10*m_dSpotCutOff));
      SetRegDWord(hKey, REG_LIGHTING, m_bLighting);

      SetRegDWord(hKey, REG_ALPHA_BLEND, m_nAlphaBlend);
      SetRegDWord(hKey, REG_ALPHA_BLEND_F, (int)m_dAlphaBlend);

//      SetRegDWord(hKey, REG_SCALE_X, m_nScaleModeX);
//      SetRegDWord(hKey, REG_SCALE_Y, m_nScaleModeY);
//      SetRegDWord(hKey, REG_SCALE_Z, m_nScaleModeZ);

      SetRegDWord(hKey, REG_DETAIL  , m_nResolution);

      SetRegDWord(hKey, REG_COL_BOX     , m_cBox);
      SetRegDWord(hKey, REG_COL_CHASSIS , m_cChassis);
      SetRegDWord(hKey, REG_COL_SCALE   , m_cScale);
      SetRegDWord(hKey, REG_COL_SPECULAR, m_cSpecular);
      SetRegDWord(hKey, REG_MAT_SHININES, (int)(m_dShinines*1.28));
   }
   if (hKey) EtsRegCloseKey(hKey);
}

COpenGLView::COpenGLView(COpenGLView*pGL) : CEts3DGL(pGL)
{
   ASSERT(FALSE); // erst Objekt Initialisieren !
   InitGLView();
}

/******************************************************************************
* Definition : void InitGLView();
* Zweck      : Initialisiert die Parameter für die OpenGL-Darstellung.
*              Hierfür werden die Basisparameter für ETS3DGL in der Registry
*              berücksichtigt, sowie CARABOX eigene Registryparameter
******************************************************************************/
void COpenGLView::InitGLView()
{
}

/******************************************************************************
* Definition : void SetProjection();
* Zweck      : Initialisieren des Darstellungsvolumens und der Projektion
******************************************************************************/
void COpenGLView::SetProjection()
{
   ETS3DGL_Volume Vol;
   Vol.Left   = (float)-m_dVolumeXY;
   Vol.Top    = (float) m_dVolumeXY;
   Vol.Right  = (float) m_dVolumeXY;
   Vol.Bottom = (float)-m_dVolumeXY;
   Vol.Near   = (float)(1.0 / m_dVolumeZ);
   if (Vol.Near > 0.01) Vol.Near = 0.01f; // maximal 10 mm
   Vol.Far    = (float) m_dVolumeZ;
   SetVolume(Vol);
   if (m_bPerspective)
   {
      SetModes(ETS3D_PR_PERSPECTIVE, ETS3D_PR_CLEAR);
   }
   else
   {
      SetModes(ETS3D_PR_ORTHOGONAL, ETS3D_PR_CLEAR);
   }
}

/******************************************************************************
* Definition : void InitGLLists();
* Zweck      : Initialisierung der Renderlistenfunktionen
******************************************************************************/
void COpenGLView::InitGLLists()
{
   int  i;
   ASSERT_LAST_ERROR;

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   SetClearBits(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
// Commands
   CreateCommand(PRE_CMD   , (LISTFUNCTION)COpenGLView::PreCommand  , 1);
   CreateCommand(LIST_LIGHT, (LISTFUNCTION)COpenGLView::LightCommand, 1);

// Listen
   CreateList(LIST_LIGHT  , (LISTFUNCTION)COpenGLView::ListLight  , 1);
   CreateList(LIST_MATRIX , (LISTFUNCTION)COpenGLView::ListMatrix , 1);
   CreateList(LIST_SCALE  , (LISTFUNCTION)COpenGLView::ListScale , 1);
   CreateCabinetList(0);
   CCabinet*pCbase = m_pPropertySheet->GetCabinet(BASE_CABINET);
   for (i=1; i<=pCbase->m_Cabinets.GetCounter(); i++)
   {
      CreateCabinetList(i);
   }
   CreateList(LIST_CHASSIS    , (LISTFUNCTION)COpenGLView::ListChassis, 1);
   if (g_nUserMode & USERMODE_USER_REF)
      CreateList(LIST_USER_REF_PT, (LISTFUNCTION)COpenGLView::ListUserRefPt, 1);

   PerformList(   LIST_LIGHT, m_bLighting);
   PerformCommand(LIST_LIGHT, m_bLighting);
   ::PostMessage(::GetParent(GetHWND()), WM_COMMAND, IDC_3D_BTN_UPDATE, (LPARAM)GetHWND());

   m_bChanged[0] = m_bChanged[1] = m_bChanged[2] = true;
   InvalidateTransMode(true, TRANS_TURNLIGHT);
   m_bChanged[0] = m_bChanged[1] = true;
   InvalidateTransMode(true, TRANS_LIGHT_SPOT);

   ASSERT_GL_ERROR;
   ASSERT_LAST_ERROR;
}

/******************************************************************************
* Definition : bool COpenGLView::CreateCabinetList(int)
* Zweck      : Initialisierung der Gehäuselistenfunktion
* Parameter (EA): nCab : Gehäuseindex
******************************************************************************/
bool COpenGLView::CreateCabinetList(int nCab)
{
   return CreateList(LIST_CABINET+nCab, (LISTFUNCTION)COpenGLView::ListCabinet, 1);
}

/******************************************************************************
* Definition : void AutoDelete();
* Zweck      : Aufruf des Destruktors
* Aufruf     : Nach dem zerstören des Renderkontextes
******************************************************************************/
void COpenGLView::AutoDelete()
{
   if (GetModes() & ETS3D_DETACH_FOREIGN_DATA) DetachData();
   CEts3DGL::AutoDelete();
}

/******************************************************************************
* Definition : void DetachData();
* Zweck      : Entkoppeln von Daten von einem Temporären COpenGLView-Objekt
* Aufruf     : Nach dem zerstören des Renderkontextes
******************************************************************************/
void COpenGLView::DetachData()
{

}

/******************************************************************************
* Definition : void DestroyGLLists();
* Zweck      : Löschen von Global angelegten Renderlisten, die nicht durch die
*              RenderListen verwaltet werden.
* Aufruf     : Beim Zerstören des Renderkontextes
******************************************************************************/
void COpenGLView::DestroyGLLists()
{
   return;
}

/******************************************************************************
* Definition : void COpenGLView::OnRenderSzene();
* Zweck      : Rendern einer 3D-Szene
* Aufruf     : Wenn Listen ungültig geworden sind
******************************************************************************/
void COpenGLView::OnRenderSzene()
{
   glLoadIdentity();
   CEts3DGL::OnRenderSzene();
}

/******************************************************************************
* Definition : LRESULT OnTimer(long, BOOL)
* Zweck      : RenderUpdatetimer
* Aufruf     : Zyklisch durch den Timer des Renderfensters
* Parameter (E): nTimerID: ID des Timers                              (long)
*           (E): bValidate: Validieren des Renderkontextes
******************************************************************************/
LRESULT COpenGLView::OnTimer(long /*nTimerID*/, BOOL /*bValidate*/)
{
   return 0;
}

/******************************************************************************
* Definition : bool OnMessage(UINT, WPARAM, LPARAM);
* Zweck      : Messagefunktion des Renderfensters.
*              Achtung das Renderfenster läuft in einem separaten Thread.
* Aufruf     : vom Renderfenster
* Parameter (EA): siehe Windows Message.
* Funktionswert : Nachricht bearbeitet (true, false)              (bool)
******************************************************************************/
bool COpenGLView::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (GetThreadID() == ::GetCurrentThreadId())
   {
      switch (nMsg)
      {
//         case WM_MOUSEWHEEL:
         case WM_MOUSEMOVE:
         case WM_LBUTTONUP:
         case WM_RBUTTONUP:
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_LBUTTONDBLCLK:
         case WM_RBUTTONDBLCLK:
         {
            ::PostMessage(GetParent(GetHWND()), nMsg, wParam, lParam);
            return true;
         }
      }
   }
   return false;
}

/******************************************************************************
* Definition : LRESULT OnCommand(WORD, WORD, HWND);
* Zweck      : Bearbeitung der WM_COMMAND an das Renderfenster.
* Aufruf     : vom Renderfenster
* Parameter (EA): siehe Windows WM_COMMAND
* Funktionswert : siehe Windows WM_COMMAND
******************************************************************************/
LRESULT COpenGLView::OnCommand(WORD /*nID*/, WORD /*nNotifyCode*/, HWND /*hwndFrom*/)
{
//   switch(wID)
//   {
//   }
   return 1;
}

/******************************************************************************
* Definition : void COpenGLView::PreCommand(COpenGLView *, int, int);
* Zweck      : Aufruf von OpenGL-State Fkt. beim Begin des Renderns
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der Commandliste                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::PreCommand(COpenGLView *pC, int /*nListID*/, int /*nRange*/)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;
   float pfCol[3];
   int nShinines = (int)(pC->m_dShinines*1.28);

   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, pC->m_AmbientColor);

   if (pC->m_bLighting)
   {
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_NORMALIZE);
   }
/*
   glDisable(GL_NORMALIZE);
   glDisable(GL_TEXTURE_1D);
   glDisable(GL_TEXTURE_2D);
*/ 

   glShadeModel(GL_SMOOTH);

   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

   glMateriali(GL_FRONT, GL_SHININESS, nShinines);
   if (nShinines)
   {
      glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)CGlLight::ColorToFloat(pC->m_cSpecular, pfCol));
   }
   else
   {
      glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)CGlLight::ColorToFloat(RGB(0,0,0), pfCol));
   }

   if (pC->m_nAlphaBlend&ALPHA_BLENDING)
   {
      glEnable(GL_BLEND);
      glDisable(GL_CULL_FACE);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
//      glBlendFunc(GL_DST_COLOR, GL_ONE);                 // geht garnicht
//      glBlendFunc(GL_ONE, GL_SRC_COLOR);                 // kein Blendeffekt
//      glBlendFunc(GL_DST_COLOR, GL_ZERO);                // kein Blendeffekt
//      glBlendFunc(GL_ZERO, GL_SRC_COLOR);                // kein Blendeffekt
//      glBlendFunc(GL_DST_ALPHA, GL_SRC_COLOR);           // kein Blendeffekt
//      glBlendFunc(GL_DST_COLOR, GL_DST_ALPHA);           // kein Blendeffekt
//      glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_ALPHA); // geht garnicht
      switch (LOWORD(pC->m_nAlphaBlend))
      {
         case 1:glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA); break;          // schon besser 1
         case 0:
         case 2:glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);break; // schon besser 2 !
         case 3:glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);          break; // schon besser 3
         case 4:glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_COLOR);break; // schon besser 4
         case 5:glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_COLOR);break; // schon besser 5
         case 6:glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);break; // geht so lala
         case 7:glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);          break; // schon besser 3
      }
   }
   else
   {
      glEnable(GL_CULL_FACE);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
      glDisable(GL_BLEND);
   }

   ASSERT_GL_ERROR;
}

/******************************************************************************
* Definition : void LightCommand(COpenGLView *, int, int);
* Zweck      : Aufruf von OpenGL-State Fkt. für die Beleuchtung
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der Commandliste                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::LightCommand(COpenGLView *pC, int nListID, int /*nRange*/)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;

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

/******************************************************************************
* Definition : void ListLight(COpenGLView *pC, int, int);
* Zweck      : Rendern der Liste für die Beleuchtung
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der RenderListe                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::ListLight(COpenGLView *pC, int nListID, int nRange)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;

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

/******************************************************************************
* Definition : void ListMatrix(COpenGLView *, int, int);
* Zweck      : Rendern der Liste für die Transformation des 3D-Objektes
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der RenderListe                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::ListMatrix(COpenGLView *pC, int nListID, int nRange)
{
   ASSERT(pC != NULL);

   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
#ifdef UNIT_METER
   CVector vTran = pC->m_vTranslation + pC->m_vAnchor;
#else
   CVector vTran = (pC->m_vTranslation*UNIT_QUOTIENT) + pC->m_vAnchor;
#endif
      glTranslated(Vx(vTran), Vy(vTran), Vz(vTran));
      glRotated(pC->m_dTheta, 1.0, 0.0, 0.0);
      glRotated(pC->m_dPhi  , 0.0, 1.0, 0.0);
      glScaled(     Vx(pC->m_vScale) ,  Vy(pC->m_vScale) ,  Vz(pC->m_vScale));
      glTranslated(-Vx(pC->m_vAnchor), -Vy(pC->m_vAnchor), -Vz(pC->m_vAnchor));
      glFlush();
   glEndList();

   ASSERT_GL_ERROR;
}

/******************************************************************************
* Definition : void ListScale(COpenGLView *pV, int nListID, int nRange);
* Zweck      : Rendern der Liste für die Maßlinien
*              3D-Objektes
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der RenderListe                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::ListScale(COpenGLView *pV, int nListID, int nRange)
{
   ASSERT(pV != NULL);
   glDeleteLists(nListID, nRange);
   if (pV->m_pPropertySheet->m_nCurrentCab)
   {
      int nCab = pV->m_pPropertySheet->m_nCurrentCab;
      CCabinet *pC1    = pV->m_pPropertySheet->GetCabinet(pV->m_nScale);
      CCabinet *pC2    = pV->m_pPropertySheet->GetCurrentCab();
      CCabinet *pCbase = pV->m_pPropertySheet->GetCabinet(BASE_CABINET);
      CVector    v1, v2;
      PointFloat vF1, vF2;
      float fTemp = (float)(pV->m_dVolumeXY * 0.5);
      float fColor[3];
      CGlLight::ColorToFloat(pV->m_cScale, fColor);

      glNewList(nListID, GL_COMPILE);
      if (pV->m_bLighting) glDisable(GL_LIGHTING);
      glColor3fv((float*)fColor);

      if (pV->m_nTransMode == TRANS_MOVE)
      {
         if (pV->m_nScaleModeX & SHOW_SCALE)
         {
            if (pV->m_nScaleModeX & SCALE_DIR_R) fTemp = -fTemp;
            v1 = CVector(0,0,0);
            v2 = CVector(0,0,0);
            if (LOWORD(pV->m_nScaleModeX)&SCALE_SIDE) pC1->GetYShift(LOWORD(pV->m_nScaleModeX), &v1);
            if (HIWORD(pV->m_nScaleModeX)&SCALE_SIDE) pC2->GetYShift(HIWORD(pV->m_nScaleModeX), &v2);
            v1.SetZ() = 0;
            v2.SetZ() = 0;

            glBegin(GL_LINE_STRIP);
            vF1 = pCbase->VectorToBDDpoint(v1, 0.001, pV->m_nScale);
            vF2 = pCbase->VectorToBDDpoint(v2, 0.001, nCab);
            glVertex3f(vF1.y, vF1.z, vF1.x);
            glVertex3f(vF1.y, vF1.z, fTemp);
            glVertex3f(vF2.y, vF2.z, fTemp);
            glVertex3f(vF2.y, vF2.z, vF2.x);
            glEnd();
         }

         if (pV->m_nScaleModeY & SHOW_SCALE)
         {
            if (pV->m_nScaleModeY & SCALE_DIR_R) fTemp = -fTemp;
            v1 = CVector(0,0,0);
            v2 = CVector(0,0,0);

            if (HIWORD(pV->m_nScaleModeY) & SCALE_SIDE1) v2.SetZ() =  Vz(pC2->GetShift());
            if (LOWORD(pV->m_nScaleModeY) & SCALE_SIDE1) v1.SetZ() =  Vz(pC1->GetShift());
            if (HIWORD(pV->m_nScaleModeY) & SCALE_SIDE2) v2.SetZ() = -Vz(pC2->GetShift());
            if (LOWORD(pV->m_nScaleModeY) & SCALE_SIDE2) v1.SetZ() = -Vz(pC1->GetShift());

            glBegin(GL_LINE_STRIP);
            vF1 = pCbase->VectorToBDDpoint(v1, 0.001, pV->m_nScale);
            vF2 = pCbase->VectorToBDDpoint(v2, 0.001, nCab);
            glVertex3f(vF1.y, vF1.z, vF1.x);
            glVertex3f(fTemp, vF1.z, 0);
            glVertex3f(fTemp, vF2.z, 0);
            glVertex3f(vF2.y, vF2.z, vF2.x);
            glEnd();
         }

         if (pV->m_nScaleModeZ & SHOW_SCALE)
         {
            if (pV->m_nScaleModeZ & SCALE_DIR_R) fTemp = -fTemp;
            v1 = CVector(0,0,0);
            v2 = CVector(0,0,0);
            if (LOWORD(pV->m_nScaleModeZ)&SCALE_SIDE) pC1->GetXShift(LOWORD(pV->m_nScaleModeZ), &v1);
            if (HIWORD(pV->m_nScaleModeZ)&SCALE_SIDE) pC2->GetXShift(HIWORD(pV->m_nScaleModeZ), &v2);
            v1.SetZ() = 0;
            v2.SetZ() = 0;

            glBegin(GL_LINE_STRIP);
            vF1 = pCbase->VectorToBDDpoint(v1, 0.001, pV->m_nScale);
            vF2 = pCbase->VectorToBDDpoint(v2, 0.001, nCab);
            glVertex3f(vF1.y, vF1.z, vF1.x);
            glVertex3f(fTemp, vF1.z, vF1.x);
            glVertex3f(fTemp, vF2.z, vF2.x);
            glVertex3f(vF2.y, vF2.z, vF2.x);
            glEnd();
         }
      }
      else if (pV->m_nTransMode == TRANS_TURN)
      {
         if (pV->m_nScaleModePhi & SHOW_SCALE)
         {
            CVector vNorm;
            int nWall = 0;
            v1 = CVector(0,0,0);
            if (LOWORD(pV->m_nScaleModePhi)&SCALE_SIDE1 ) nWall = CARABOX_CABWALL_FRONT;
            if (LOWORD(pV->m_nScaleModePhi)&SCALE_SIDE1B) nWall = CARABOX_CABWALL_BACK;
            if (LOWORD(pV->m_nScaleModePhi)&SCALE_SIDE2B) nWall = CARABOX_CABWALL_LEFT;
            if (LOWORD(pV->m_nScaleModePhi)&SCALE_SIDE2 ) nWall = CARABOX_CABWALL_RIGHT;
            if (nWall)
            {
               pCbase->GetCabinetWall(MAKELONG(nWall, pV->m_nScale), NULL, &vNorm);
               vNorm = vNorm*fTemp;
               vF1 = pCbase->VectorToBDDpoint(v1      , 0.001, MAKELONG(pV->m_nScale, NO_ROTATION));
               glBegin(GL_LINES);
               glVertex3f(vF1.y, vF1.z, vF1.x);
               glVertex3d(vF1.y+Vy(vNorm), vF1.z+Vz(vNorm), vF1.x+Vx(vNorm));
               glEnd();
            }
            v1 = CVector(0,0,0);
            if (HIWORD(pV->m_nScaleModePhi)&SCALE_SIDE1 ) nWall = CARABOX_CABWALL_FRONT;
            if (HIWORD(pV->m_nScaleModePhi)&SCALE_SIDE1B) nWall = CARABOX_CABWALL_BACK;
            if (HIWORD(pV->m_nScaleModePhi)&SCALE_SIDE2B) nWall = CARABOX_CABWALL_LEFT;
            if (HIWORD(pV->m_nScaleModePhi)&SCALE_SIDE2 ) nWall = CARABOX_CABWALL_RIGHT;
            if (nWall)
            {
               pCbase->GetCabinetWall(MAKELONG(nWall, nCab), NULL, &vNorm);
               vNorm = vNorm*fTemp;
               vF1 = pCbase->VectorToBDDpoint(v1      , 0.001, MAKELONG(nCab, NO_ROTATION));
               glBegin(GL_LINES);
               glVertex3f(vF1.y, vF1.z, vF1.x);
               glVertex3d(vF1.y+Vy(vNorm), vF1.z+Vz(vNorm), vF1.x+Vx(vNorm));
               glEnd();
            }
         }
      }
      if (pV->m_bLighting) glEnable(GL_LIGHTING);
      glEndList();
   }
      
   ASSERT_GL_ERROR;
}

/******************************************************************************
* Definition : void ListCabinet(COpenGLView *, int, int);
* Zweck      : Rendern der Listen für die Gehäusesegmente
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der RenderListe                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::ListCabinet(COpenGLView *pC, int nListID, int nRange)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;

   glDeleteLists(nListID, nRange);
   if (pC->m_pPropertySheet->GetCabinet(nListID-LIST_CABINET))
   {
      glNewList(nListID, GL_COMPILE);
      if (pC->m_nAlphaBlend & ALPHA_DRAW_DIR)
      {
         int nCab = nListID-LIST_CABINET;
         nCab = (pC->m_pPropertySheet->m_Cabinet.m_Cabinets.GetCounter()) - nCab;
         if (!pC->DrawCabinet3D(nCab))
         {
            pC->PerformList(nListID, false);
         }
      }
      else
      {
         if (!pC->DrawCabinet3D(nListID-LIST_CABINET))
         {
            pC->PerformList(nListID, false);
         }
      }
      glFlush();
      glEndList();
   }
}

/******************************************************************************
* Definition : void ListChassis(COpenGLView *, int, int);
* Zweck      : Rendern der Liste für die Chassis
* Parameter (E): pC: COpenGLView-Objekt                       (COpenGLView*)
*           (E): nListID: ID in der RenderListe                       (int)
*           (E): nRange: Bereich der ID´s
******************************************************************************/
void COpenGLView::ListChassis(COpenGLView *pC, int nListID, int nRange)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;
   DWORD dwCount = 0;
   pC->m_pPropertySheet->m_ChassisData.ProcessWithObjects(COpenGLView::CountValidChassis, (WPARAM)&dwCount, 0);
   glDeleteLists(nListID, nRange);
   if (dwCount)
   {
      glNewList(nListID, GL_COMPILE);
         pC->m_pPropertySheet->m_ChassisData.ProcessWithObjects(COpenGLView::DrawChassis3D, (WPARAM)pC, 0);
         glFlush();
      glEndList();
   }
}

void COpenGLView::ListUserRefPt(COpenGLView *pC, int nListID, int nRange)
{
   ASSERT(pC != NULL);
   ASSERT_LAST_ERROR;
   float fColor[3];
   CGlLight::ColorToFloat(g_rgbPickColor, fColor);
   float fMat[4] = {fColor[0], fColor[1], fColor[2], 1};
   CGlLight::MultnFloat(3, fMat, 0.5);

   glDeleteLists(nListID, nRange);
   glNewList(nListID, GL_COMPILE);
      glPushMatrix();
      glColor3fv((float*)fColor);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fMat);
      PointFloat vF = pC->m_pPropertySheet->m_Cabinet.VectorToBDDpoint(pC->m_pPropertySheet->m_Cabinet.vUserRef, 0.001, 0);
      glTranslated(vF.y, vF.z, vF.x);
      CEts3DGL::Draw(ETS3D_CUBE|ETS3D_SOLID_FRAME, 0.02f);
      glPopMatrix();
   glEndList();
}

/******************************************************************************
* Definition : void OnDrawToRenderDC(HDC, int);
* Zweck      : direktes Zeichen in das Renderfenster
* Aufruf     : nach dem Rendervorgang
* Parameter (EA): hdc: Gerätekontext für die Zeichenausgabe
******************************************************************************/
void COpenGLView::OnDrawToRenderDC(HDC /*hdc*/, int /*nOff*/)
{
//   ::TextOut(hdc, 20, 20, "Dödel", 5);
   ASSERT_LAST_ERROR;
}

/******************************************************************************
* Definition : bool TextOut(HDC, double, double, double, UINT, int, char *);
* Zweck      : Ausgabe von Text in das Renderfenster
* Parameter (EA): hdc: Gerätekontext für die Zeichenausgabe             (HDC)
*           (E): x, y, z: Koordinaten für den Text
*           (E): nAlign : Ausrichtung des Textes ::SetTextAlign(..)     (UINT)
*           (E): nOff: Offset für die Zeichenposition (NT, Win9X)       (int)
*           (E): pszText: Text für die Ausgabe                          (char*)
* Funktionswert :
******************************************************************************/
bool COpenGLView::TextOut(HDC hdc, double x, double y, double z, UINT nAlign, int nOff, char *pszText)
{
   unsigned char bVal = 0;
   if (!pszText) return false;

   ASSERT_LAST_ERROR;

   glRasterPos3d(x, y, z);
   glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &bVal);
   if (bVal)
   {
      double pos[4];
      glGetDoublev(GL_CURRENT_RASTER_POSITION, pos);
      ::SetTextAlign(hdc, nAlign&0xffffffdf);
/*
      if ((nAlign & TA_VCENTER) == TA_VCENTER)
      {
         SIZE size;
         ::GetTextExtentPoint32(hdc, pszText, strlen(pszText), &size);
         pos[1] -=  size.cy >> 1;
      }
*/
      ::TextOut(hdc, (int)pos[0], nOff-(int)pos[1], pszText, strlen(pszText));
      return true;
   }
   else return false;
}


CGlLight* COpenGLView::GetLight(int)
{
   return &m_Light;
}

/*************************************************************************************
* Definition : static int CountValidChassis(void *, WPARAM, LPARAM);
* Zweck      : prüft, ob gültige Chassis vorhanden sind
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p: ChassisData-Objekt (ChassisData*)                         (void*)
*           (EA): wParam: (int*)                                              (WPARAM)
* Funktionswert : (0) : Abbruch der Iteration                                 (int)
*                 (1) : kein Abbruch
**************************************************************************************/
int COpenGLView::CountValidChassis(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH    = (ChassisData*)p;
   if (pCH->nCabWall)
   {
      (*((DWORD*)wParam))++;
      return lParam;
   }
   return 1;
}

/******************************************************************************
* Definition : int DrawChassis3D(void *, WPARAM, LPARAM);
* Zweck      : Zeichnen eines Chassis in 3D
* Aufruf     : durch CEtsList::ProcessWithObjects(..)
* Parameter (E): p : Chassisdateobjekt (ChassisData*)                (void*)
*           (E): wParam: COpenGLView-Objekt (COpenGLView*)           (wParam)
* Funktionswert : (0) : Abbruch der Iteration                        (int)
*                 (1) : kein Abbruch
******************************************************************************/
int COpenGLView::DrawChassis3D(void *p, WPARAM wParam, LPARAM)
{
   const int nNoOfPt   = 36; // maximale Anzahl (runde Chassis)
   ChassisData *pCH    = (ChassisData*)p;
   COpenGLView *pGL    = (COpenGLView*)wParam;
   CCabinet    *pCbase = pGL->m_pPropertySheet->GetCabinet(BASE_CABINET);
   int          j, nPoints, nCab = HIWORD(pCH->nCabWall);
   GLenum       nFace = 0;
   double       dCenterX, dCenterY, dCenterLine, dPhi, dDist;
   CVector      vCalePos, pvPt[nNoOfPt];
   CVector2D    vrcWall[NO_OF_RECT_POINTS];
   PointFloat   vF;
   ETS3DGL_Fan  Fan;
   float        fColor[4];
   bool bLighting = pGL->m_bLighting;
   if (pCH->nCabWall == 0) return 1;
   if (pCH->IsHidden())    return 1;

   if (pCH->dwFlagsCH & CHASSIS_3D_POS_INVALID)
   {
      pCbase->CalcChassisPos3D(pCH);
   }

   Fan.dStep    = pGL->m_dVolumeXY * 400 / (1<<pGL->m_nResolution);
   Fan.pFn      = DrawBoxFan;
   Fan.pParam   = pGL->m_pPropertySheet->GetCabinet(pCH->GetCabinet());

   pCbase->GetCabinetWall(pCH->nCabWall, vrcWall);
   dCenterLine = pCbase->GetXCenterLine(vrcWall);
   CalcInverseTran();

   dCenterX = dCenterLine+pCH->dDistFromCenter;
   dCenterY = pCH->dHeightOnCabWall;

   CGlLight::ColorToFloat(pGL->m_cChassis, (float*)fColor);
   if (pGL->m_nAlphaBlend&ALPHA_BLENDING)
   {
      nFace = GL_FRONT_AND_BACK;
      fColor[3] = (float)(pGL->m_dAlphaBlend*0.01);                 // Alpha Blending
   }
   else
   {
      nFace = GL_FRONT;
      fColor[3] = 1.0f;
   }
   glColor3fv((float*)fColor);

   if (bLighting)
   {
      float pMat[4] = {fColor[0], fColor[1], fColor[2], fColor[3]};
      glMaterialfv(nFace, GL_AMBIENT_AND_DIFFUSE, pMat);
      pCbase->GetCabinetWall(pCH->nCabWall, NULL, &vCalePos);
      vF = pCbase->VectorToBDDpoint(vCalePos, 1.0, 0);
      glNormal3f(vF.y, vF.z, vF.x);
   }

   nPoints = 0;
   if (pCH->IsRect())
   {
      nPoints = 4;            // 4 Punkte für Rechteckchassis
   }
   else
   {
      if (pCH->IsSphere())
      {
         glPushMatrix();
         vF = pCbase->VectorToBDDpoint(pCH->vPosChassis, 0.001, MAKELONG(nCab, NO_TRANSLATION));
         glTranslatef(vF.y, vF.z, vF.x);
         pGL->Draw(ETS3D_SPHERE|ETS3D_SOLID_FRAME|pGL->m_nResolution, (float)(pCH->dEffDiameter*0.0005));
         glPopMatrix();
         return 1;
      }
      else nPoints = nNoOfPt;// runde Chassis, aber keine Kugeln
   }

   if (pGL->m_nPolygonOffset) 
   {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(pGL->m_fPolygonFactor, (float)(-pGL->m_nPolygonOffset));
      dDist = 0;
   }
   else
   {
      dDist = pGL->m_dVolumeXY * pGL->m_fPolygonFactor * 35;
   }

   if (!bLighting)
   {
      if (nPoints == 4) glBegin(GL_QUADS);
      else              glBegin(GL_POLYGON);
   }

   for(j=0; j<nPoints; j++)
   {
      if (nPoints == 4)   // Rechteckchassis
      {
         switch(j)
         {
            case 0:
            pvPt[j].SetX() = dCenterX + 0.5*pCH->dEffBreite;
            pvPt[j].SetY() = dCenterY - 0.5*pCH->dEffHoehe ;
            break;
            case 1:
            pvPt[j].SetX() = dCenterX + 0.5*pCH->dEffBreite;
            pvPt[j].SetY() = dCenterY + 0.5*pCH->dEffHoehe ;
            break;
            case 2:
            pvPt[j].SetX() = dCenterX - 0.5*pCH->dEffBreite;
            pvPt[j].SetY() = dCenterY + 0.5*pCH->dEffHoehe ;
            break;
            case 3:
            pvPt[j].SetX() = dCenterX - 0.5*pCH->dEffBreite;
            pvPt[j].SetY() = dCenterY - 0.5*pCH->dEffHoehe ;
            break;
         }
      }
      else                                               // rundes Chassis
      {
         dPhi = ((10*j)*M_PI_D_180);                     // Winkel Phi im Bogenmaß
         pvPt[j].SetX() = dCenterX + 0.5*pCH->dEffDiameter*cos(dPhi);
         pvPt[j].SetY() = dCenterY + 0.5*pCH->dEffDiameter*sin(dPhi);
      }
      pvPt[j].SetZ() = dDist;                           // Z ist 0 !!
      ReTransformVector(pvPt[j]);
      if (!bLighting) 
      {
         vF = pCbase->VectorToBDDpoint(pvPt[j], 0.001, nCab);
         glVertex3f(vF.y, vF.z, vF.x);
      }
   }

   if (bLighting)
   {
      if (pvPt != NULL)
      {
         if (nPoints==4)
         {
            DrawTriangleFan(&pvPt[0], &pvPt[1], &pvPt[2], &Fan);
            DrawTriangleFan(&pvPt[2], &pvPt[3], &pvPt[0], &Fan);
         }
         else
         {
            CVector vCenter(dCenterX, dCenterY, dDist);
            ReTransformVector(vCenter);
            nPoints -= 1;
            for(j=0; j<nPoints; j++)
            {
               DrawTriangleFan(&pvPt[j], &pvPt[j+1], &vCenter, &Fan);
            }
            DrawTriangleFan(&pvPt[nPoints], &pvPt[0], &vCenter, &Fan);
         }
      }
   }
   else glEnd();
  
   if (pGL->m_nPolygonOffset) glDisable(GL_POLYGON_OFFSET_FILL);

   return 1;
}

/******************************************************************************
* Definition : bool DrawCabinet3D(int);
* Zweck      : Zeichnen eines Gehäusesegmentes in 3D
* Parameter (E): nCab : Gehäusesegmentnummer
* Funktionswert : Segment gezeichnet                                 (bool)
******************************************************************************/
bool COpenGLView::DrawCabinet3D(int nCab)
{
   int         i, j, nWall;
   CVector     vP[NO_OF_RECT_POINTS];
   CVector     vNorm = CVector(1,0,0);
   CVector    *pvNorm = NULL;
   float       fColor[4];
   GLenum      nFace = 0;
   PointFloat  vF;
   ETS3DGL_Fan Fan;
   CCabinet *pC     = m_pPropertySheet->GetCabinet(nCab);
   CCabinet *pCbase = m_pPropertySheet->GetCabinet(BASE_CABINET);
   if (pC == NULL) return false;
   Fan.dStep    = m_dVolumeXY * 400 / (1<<m_nResolution);
   Fan.pFn      = DrawBoxFan;
   Fan.pParam   = m_pPropertySheet->GetCabinet(BASE_CABINET);
   
   if (nCab != 0)
   {
      if (nCab == m_pPropertySheet->m_nCurrentCab)
      {
         CGlLight::ColorToFloat(g_rgbPickColor, fColor);
      }
      else if (m_pPropertySheet->m_nCurrentCab != 0)
      {
         CGlLight::ColorToFloat(m_cBox, fColor);
         if      (fColor[0] == 0) fColor[0] += (0.1f*nCab);
         else if (fColor[1] == 0) fColor[1] += (0.1f*nCab);
         else if (fColor[2] == 0) fColor[2] += (0.1f*nCab);
         else CGlLight::MultnFloat(3, fColor, (0.05f*nCab));
         if (fColor[0] > 1) fColor[0] = 1;
         if (fColor[1] > 1) fColor[0] = 1;
         if (fColor[2] > 1) fColor[0] = 1;
      }
      else
      {
         CGlLight::ColorToFloat(m_cBox, fColor);
      }
   }
   else
   {
      CGlLight::ColorToFloat(m_cBox, fColor);
   }

   if (m_nAlphaBlend&ALPHA_BLENDING)
   {
      nFace = GL_FRONT_AND_BACK;
      fColor[3] = (float)(m_dAlphaBlend*0.01);                // Alpha Blending
   }
   else
   {
      nFace = GL_FRONT;
      fColor[3] = 1.0f;
   }

   glColor4fv((float*)fColor);
   if (m_bLighting) pvNorm = &vNorm;
   else             glBegin(GL_QUADS);

   for (i=0; i<NO_OF_WALLS; i++)
   {
      nWall = i+1;
      if (pC->IsWallEnabled(nWall))
      {
         pCbase->GetCabinetWall(MAKELONG(nWall, nCab), NULL, pvNorm, vP);
         if (m_bLighting)
         {
            float fMat[4] = {fColor[0], fColor[1], fColor[2], fColor[3]};
            CGlLight::MultnFloat(3, fMat, 0.5);
            glMaterialfv(nFace, GL_AMBIENT_AND_DIFFUSE, fMat);
            vF = pCbase->VectorToBDDpoint(vNorm, 1.0, 0);
            glNormal3f(vF.y, vF.z, vF.x);
            DrawTriangleFan(&vP[0], &vP[1], &vP[2], &Fan);
            DrawTriangleFan(&vP[2], &vP[3], &vP[0], &Fan);
         }
         else
         {
            for (j=0; j<NO_OF_RECT_POINTS; j++)
            {
               vF = pCbase->VectorToBDDpoint(vP[j], 0.001, 0);
               glVertex3f(vF.y, vF.z, vF.x);
            }
         }
      }
   }
   if (!m_bLighting) glEnd();
   return true;
}

/******************************************************************************
* Definition : void DrawBoxFan(int, CVector *, void *);
* Zweck      : Zeichnen von Dreieck-Fans
* Aufruf     : durch DrawTrianglefan(..)
* Parameter (E): nCount: (3): Zeiche Dreieck                            (int)
*                        (>3): Zeichne Dreieck-Fan
*           (E) pV: Array mit Eckpunktkoordinaten                  (CVector*)
*           (E) pParam: Basisgehäuseobjekt (CCabinet*)                (void*)
******************************************************************************/
void COpenGLView::DrawBoxFan(int nCount, CVector *pV, void *pParam)
{
   int i;
   CCabinet*pC = (CCabinet*) pParam;
   PointFloat vF;

   if (nCount == 3) glBegin(GL_TRIANGLES);
   else             glBegin(GL_TRIANGLE_FAN);

   for (i=0; i<nCount; i++)
   {
      vF = pC->VectorToBDDpoint(pV[i], 0.001, 0);
      glVertex3f(vF.y, vF.z, vF.x);
   }

   glEnd();
}

/******************************************************************************
* Definition : void InvalidateTransMode(bool)
* Zweck      : Updaten und prüfen der Parameter, Invalidieren der Listen
* Parameter (E): bReset: Löschen der Flags (bChanged)                (bool)
******************************************************************************/
void COpenGLView::InvalidateTransMode(bool bReset, int nMode)
{
   if ((nMode == -1) || (nMode == -2)) nMode = m_nTransMode;
   switch (nMode)
   {
      case TRANS_MOVE: case TRANS_TURN:
      if (!m_pPropertySheet->IsSectionChangeable())
      {
         if (m_dPhi > 360) m_dPhi -= 360;
         if (m_dPhi <   0) m_dPhi += 360;
         InvalidateList(LIST_MATRIX);
      }
      else
      {
         double dTemp;
         CCabinet *pC1 = m_pPropertySheet->GetCabinet(m_nScale);
         CCabinet *pC2 = m_pPropertySheet->GetCurrentCab();
         if (nMode==TRANS_MOVE)
         {
            if (m_bChanged[0])
            {
#ifdef UNIT_METER
               dTemp = m_dTranCabX * UNIT_FACTOR;
#else
               dTemp = m_dTranCabX;
#endif
               if (LOWORD(m_nScaleModeX)&SCALE_SIDE) dTemp += pC1->GetYShift(LOWORD(m_nScaleModeX));
               if (HIWORD(m_nScaleModeX)&SCALE_SIDE) dTemp -= pC2->GetYShift(HIWORD(m_nScaleModeX));
               if (!pC1->IsBaseCabinet()) dTemp += Vy(pC1->vPosRef);
               pC2->vPosRef.SetY() = dTemp;
            }
            if (m_bChanged[1])
            {
#ifdef UNIT_METER
               dTemp = m_dTranCabY * UNIT_FACTOR;
#else
               dTemp = m_dTranCabY;
#endif
               if (LOWORD(m_nScaleModeY)&SCALE_SIDE1) dTemp += Vz(pC1->GetShift());
               if (HIWORD(m_nScaleModeY)&SCALE_SIDE1) dTemp -= Vz(pC2->GetShift());
               if (LOWORD(m_nScaleModeY)&SCALE_SIDE2) dTemp -= Vz(pC1->GetShift());
               if (HIWORD(m_nScaleModeY)&SCALE_SIDE2) dTemp += Vz(pC2->GetShift());
               if (!pC1->IsBaseCabinet()) dTemp += Vz(pC1->vPosRef);
               pC2->vPosRef.SetZ() = dTemp;
            }
            if (m_bChanged[2])
            {
#ifdef UNIT_METER
               dTemp = m_dTranCabZ * UNIT_FACTOR;
#else
               dTemp = m_dTranCabZ;
#endif
               if (LOWORD(m_nScaleModeZ)&SCALE_SIDE) dTemp += pC1->GetXShift(LOWORD(m_nScaleModeZ));
               if (HIWORD(m_nScaleModeZ)&SCALE_SIDE) dTemp -= pC2->GetXShift(HIWORD(m_nScaleModeZ));
               if (!pC1->IsBaseCabinet()) dTemp += Vx(pC1->vPosRef);
               pC2->vPosRef.SetX() = dTemp;
            }
         }
         else
         {
            if (m_bChanged[2]) pC2->vUserRef.SetX() = m_dXiCab   *M_PI_D_180;
            if (m_bChanged[1]) pC2->vUserRef.SetY() = m_dThetaCab*M_PI_D_180;
            if (m_bChanged[0])
            {
               if (m_dPhiCab > 360) m_dPhiCab -= 360;
               if (m_dPhiCab <   0) m_dPhiCab += 360;
               if (GetAnglePhiDiff(dTemp))
               {
//                  m_dPhiCab = (Vz(pC2->vUserRef) - dTemp) / M_PI_D_180;
                  pC2->vUserRef.SetZ() = (m_dPhiCab * M_PI_D_180) - dTemp;
               }
               else
               {
                  pC2->vUserRef.SetZ() = m_dPhiCab * M_PI_D_180;
               }
            }
            if (m_nScaleModePhi & SHOW_SCALE)
               InvalidateList(LIST_SCALE);
         }
         if (m_bChanged[0] || m_bChanged[1] || m_bChanged[2])
         {
            pC2->SetTransformation();
            InvalidateList(LIST_CABINET+m_pPropertySheet->m_nCurrentCab);
            InvalidateList(LIST_CHASSIS);
            if ((m_nScaleModeX & SHOW_SCALE) || (m_nScaleModeY & SHOW_SCALE) || (m_nScaleModeZ & SHOW_SCALE))
               InvalidateList(LIST_SCALE);
         }
      } break;
      case TRANS_ZOOM:
      {
         if (m_bChanged[0])
         {
            m_vScale = CVector(m_dZoom, m_dZoom, m_dZoom);
            InvalidateList(LIST_MATRIX);
         }
         if (m_bChanged[1] || m_bChanged[2])
         {
            SetProjection();
         }

      } break;
      case TRANS_TURNLIGHT:
      {
         double cosTheta;
         CVector vLight, vTemp;
         if (m_bChanged[0])
         {
            if (m_dPhiLight > 360) m_dPhiLight -= 360;
            if (m_dPhiLight <   0) m_dPhiLight += 360;
         }
         cosTheta  = cos(m_dThetaLight * M_PI_D_180);
         vLight    = CVector(cosTheta * sin(m_dPhiLight   * M_PI_D_180),
                                        sin(m_dThetaLight * M_PI_D_180),
                             cosTheta * cos(m_dPhiLight   * M_PI_D_180));
         m_Light.SetDirection(-vLight);
         if (m_dDistanceLight > 0.0)
         {
            vLight *= m_dDistanceLight;
            vLight *= (Vx(m_vScale) + Vz(m_vScale)) * 0.5;
#ifdef UNIT_METER
            vTemp   = m_vAnchor + m_vTranslation + vLight;
#else
            vTemp   = m_vAnchor + (m_vTranslation*UNIT_QUOTIENT) + vLight;
#endif
            m_Light.SetPosition(vTemp, 1.0f);
            TRACE("Lightposition: %.2f,%.2f,%.2f\n", Vx(vTemp), Vy(vTemp), Vz(vTemp));
            TRACE("Direction    : %.2f,%.2f,%.2f\n", -Vx(vLight), -Vy(vLight), -Vz(vLight));
         }
         else
         {
            m_Light.SetPosition(vLight, 0.0f);
         }
         InvalidateList(LIST_LIGHT); 
      } break;
      case TRANS_LIGHT_INTENS:
         m_Light.SetAmbientColor((float)(m_dLightIntens[0]*0.01), (float)(m_dLightIntens[1]*0.01), (float)(m_dLightIntens[2]*0.01), 1.0);
         m_Light.SetDiffuseColor(m_Light.m_pfAmbientColor[0], m_Light.m_pfAmbientColor[1], m_Light.m_pfAmbientColor[2], 1.0);
         InvalidateCommand(LIST_LIGHT);
         InvalidateCommand(PRE_CMD);
         break;
      case TRANS_LIGHT_SPOT:
         if (m_bChanged[0] || m_bChanged[1])
         {
            m_Light.m_fSpotCutOff   = (float)(m_dSpotCutOff      * 0.5);
            m_Light.m_fSpotExponent = (float)(m_dSpotCutExponent * 1.28);
            InvalidateCommand(LIST_LIGHT);
         }
         if (m_bChanged[2])
         {
            InvalidateCommand(PRE_CMD);
            InvalidateAllCabinets();
            InvalidateList(LIST_CHASSIS);
         }
         break;
      case TRANS_SETTINGS:
         if (m_bChanged[0])
         {
            InvalidateCommand(PRE_CMD);
            InvalidateAllCabinets();
            InvalidateList(LIST_CHASSIS);
         }
         if (m_bChanged[1]) SetProjection();
         if (m_bChanged[2]) InvalidateCommand(PRE_CMD);
         break;
   }
   if (bReset) Reset();
}

void COpenGLView::Reset()
{
   m_bChanged[0] = m_bChanged[1] = m_bChanged[2] = false;
}

/******************************************************************************
* Definition : void OnChangeCabinet()
* Zweck      : Updaten der abhängigen Parameter, wenn das Gehäusesegment
*              geändert wird.
******************************************************************************/
void COpenGLView::OnChangeCabinet()
{
   if (m_pPropertySheet->m_nCurrentCab)
   {
      if (g_nUserMode & USERMODE_USER_REF) PerformList(LIST_USER_REF_PT, false);
      if (m_pPropertySheet->m_nCurrentCab == m_nScale)
      {
         m_nScale = 0;
      }
      CCabinet *pC1    = m_pPropertySheet->GetCabinet(m_nScale);
      CCabinet *pC2    = m_pPropertySheet->GetCurrentCab();
//      CCabinet *pCbase = m_pPropertySheet->GetCabinet(BASE_CABINET);
      PerformList(LIST_SCALE, true);
      if (pC1 && pC2)                                                  // Transformationsparameter updaten 
      {
         double dTemp;

         dTemp = Vy(pC2->vPosRef);
         if (!pC1->IsBaseCabinet()) dTemp -= Vy(pC1->vPosRef);
         if (LOWORD(m_nScaleModeX)&SCALE_SIDE) dTemp -= pC1->GetYShift(LOWORD(m_nScaleModeX));
         if (HIWORD(m_nScaleModeX)&SCALE_SIDE) dTemp += pC2->GetYShift(HIWORD(m_nScaleModeX));
#ifdef UNIT_METER
         m_dTranCabX = dTemp * UNIT_QUOTIENT;
#else
         m_dTranCabX = dTemp;
#endif

         dTemp = Vz(pC2->vPosRef);
         if (!pC1->IsBaseCabinet()) dTemp -= Vz(pC1->vPosRef);
         if (LOWORD(m_nScaleModeY)&SCALE_SIDE1) dTemp -= Vz(pC1->GetShift());
         if (HIWORD(m_nScaleModeY)&SCALE_SIDE1) dTemp += Vz(pC2->GetShift());
         if (LOWORD(m_nScaleModeY)&SCALE_SIDE2) dTemp += Vz(pC1->GetShift());
         if (HIWORD(m_nScaleModeY)&SCALE_SIDE2) dTemp -= Vz(pC2->GetShift());
#ifdef UNIT_METER
         m_dTranCabY = dTemp * UNIT_QUOTIENT;
#else
         m_dTranCabY = dTemp;
#endif

         dTemp = Vx(pC2->vPosRef);
         if (!pC1->IsBaseCabinet()) dTemp -= Vx(pC1->vPosRef);
         if (LOWORD(m_nScaleModeZ)&SCALE_SIDE) dTemp -= pC1->GetXShift(LOWORD(m_nScaleModeZ));
         if (HIWORD(m_nScaleModeZ)&SCALE_SIDE) dTemp += pC2->GetXShift(HIWORD(m_nScaleModeZ));
#ifdef UNIT_METER
         m_dTranCabZ = dTemp * UNIT_QUOTIENT;
#else
         m_dTranCabZ = dTemp;
#endif
         m_dXiCab    = Vx(pC2->vUserRef) / M_PI_D_180;
         m_dThetaCab = Vy(pC2->vUserRef) / M_PI_D_180;

         if (GetAnglePhiDiff(dTemp))
         {
            m_dPhiCab = (Vz(pC2->vUserRef) + dTemp) / M_PI_D_180;
         }
         else
         {
            m_dPhiCab = Vz(pC2->vUserRef) / M_PI_D_180;
         }
      }
   }
   else
   {
      if (g_nUserMode & USERMODE_USER_REF)
      {
         if (m_pPropertySheet->m_Cabinet.vUserRef != CVector())
            PerformList(LIST_USER_REF_PT, true);
         else
            PerformList(LIST_USER_REF_PT, false);
      }
      PerformList(LIST_SCALE, false);
   }
}

/******************************************************************************
* Definition : bool GetAnglePhiDiff(double &);
* Zweck      : Ermitteln des Differenzwinkels Phi zwischen zwei Gehäusesgment-
*              schallwänden.
* Parameter (EA): dDiff: Differenzwinkel Phi
* Funktionswert : Winkel ermittelt
******************************************************************************/
bool COpenGLView::GetAnglePhiDiff(double &dDiff)
{
   CCabinet *pCbase = m_pPropertySheet->GetCabinet(BASE_CABINET);
   int nWall1 = 0, nWall2 = 0;

   if (LOWORD(m_nScaleModePhi)&SCALE_SIDE1 ) nWall1 = CARABOX_CABWALL_FRONT;
   if (LOWORD(m_nScaleModePhi)&SCALE_SIDE1B) nWall1 = CARABOX_CABWALL_BACK;
   if (LOWORD(m_nScaleModePhi)&SCALE_SIDE2B) nWall1 = CARABOX_CABWALL_LEFT;
   if (LOWORD(m_nScaleModePhi)&SCALE_SIDE2 ) nWall1 = CARABOX_CABWALL_RIGHT;

   if (HIWORD(m_nScaleModePhi)&SCALE_SIDE1 ) nWall2 = CARABOX_CABWALL_FRONT;
   if (HIWORD(m_nScaleModePhi)&SCALE_SIDE1B) nWall2 = CARABOX_CABWALL_BACK;
   if (HIWORD(m_nScaleModePhi)&SCALE_SIDE2B) nWall2 = CARABOX_CABWALL_LEFT;
   if (HIWORD(m_nScaleModePhi)&SCALE_SIDE2 ) nWall2 = CARABOX_CABWALL_RIGHT;
   if (nWall1 && nWall2)
   {
      CVector vNorm1, vNorm2;
      int *n;
      CCabinet *pC1 = pCbase->GetCabinet(m_nScale);
      CCabinet *pC2 = pCbase->GetCabinet(m_pPropertySheet->m_nCurrentCab);

      n = (int*)CCabinet::gm_nWallPoints[nWall1-1];
      vNorm1 = Norm(Product(pC1->GetCabPoint(n[0]) - pC1->GetCabPoint(n[1]), pC1->GetCabPoint(n[1]) - pC1->GetCabPoint(n[2])));
      n = (int*)CCabinet::gm_nWallPoints[nWall2-1];
      vNorm2 = Norm(Product(pC2->GetCabPoint(n[0]) - pC2->GetCabPoint(n[1]), pC2->GetCabPoint(n[1]) - pC2->GetCabPoint(n[2])));

      double dTemp1 = atan2(Vy(vNorm1), Vx(vNorm1));
      double dTemp2 = atan2(Vy(vNorm2), Vx(vNorm2));
      dDiff = dTemp2 - dTemp1;
      return true;
   }
   return false;
}

void COpenGLView::InvalidateAllLists()
{
   InvalidateCommand(PRE_CMD);
   InvalidateCommand(LIST_LIGHT);
   InvalidateList(LIST_LIGHT);
   InvalidateList(LIST_MATRIX);
   InvalidateList(LIST_SCALE);
   InvalidateAllCabinets();
   InvalidateList(LIST_CHASSIS);
}

void COpenGLView::InvalidateAllCabinets()
{
   for (int i=0; i<=m_pPropertySheet->m_Cabinet.m_Cabinets.GetCounter(); i++)
      InvalidateList(LIST_CABINET+i);
}

void COpenGLView::SetScalesOff(bool bMove)
{
   if (bMove)
   {
      m_nScaleModeX   &= ~SHOW_SCALE;
      m_nScaleModeY   &= ~SHOW_SCALE;
      m_nScaleModeZ   &= ~SHOW_SCALE;
   }
   else
   {
      m_nScaleModePhi   &= ~SHOW_SCALE;
      m_nScaleModeTheta &= ~SHOW_SCALE;
   }
}

bool COpenGLView::IsScaleActive(bool bMove)
{
   if (bMove)
   {
      if ((m_nScaleModeX & SHOW_SCALE) || (m_nScaleModeY & SHOW_SCALE) || (m_nScaleModeZ & SHOW_SCALE))
         return true;
   }
   else
   {
      if ((m_nScaleModePhi & SHOW_SCALE) || (m_nScaleModeTheta & SHOW_SCALE))
         return true;
   }
   return false;
}

void COpenGLView::MoveBox(int x, int y, int z)
{
#ifdef UNIT_METER
   ((double*)&m_vTranslation)[0] += 0.001*(x);
   ((double*)&m_vTranslation)[1] -= 0.001*(y);
   ((double*)&m_vTranslation)[2] += 0.001*(z);
#else
   ((double*)&m_vTranslation)[0] += (double)(x);
   ((double*)&m_vTranslation)[1] -= (double)(y);
   ((double*)&m_vTranslation)[2] += (double)(z);
#endif
   InvalidateList(LIST_MATRIX);
}

void COpenGLView::TurnBox(int np, int nt)
{
   m_dPhi   += 0.5*np;
   m_dTheta += 0.5*nt;
   InvalidateList(LIST_MATRIX);
}
