// carawalkdll.cpp: Implementierung der Klasse CCaraWalkDll.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CaraWalkDll.h"
#include "FileHeader.h"
#include "CEtsDlg.h"
#include "DllError.h"
#include "resource.h"
#include "Cara3DTr.h"
#include "LightDlg.h"
#include "BoxPsDlg.h"
#include "DibSection.h"
#include "CCaraMat.h"
#include "LsBoxDb_cfg.h"
#include "CEtsHelp.h"
#include "ETS3DGLRegKeys.h"
#include "CustErr.h"
#include "EtsRegistry.h"
#include "EtsAppWndMsg.h"
#include "BoxProperties.h"

#include <mmsystem.h>         // Sound

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern HINSTANCE        g_hInstance;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

// Boxdialog
// DoDataExchange(...)
#define CARAWALK_SHOW_SCALE                0x0001
#define CARAWALK_SHOW_TWICE                0x0002
#define CARAWALK_SET_DISTANCE_VECTOR       0x0002
#define CARAWALK_SHOW_URP_AND_SCALE        0x0004

#define CARAWALK_NO_OF_LIGHTS          "NoOfLights"
#define CARAWALK_LIGHT                 "Light"
#define CARAWALK_BOX                   "Box"
#define CARAWALK_ROOM                  "Room"
#define CARAWALK_SCREENSAVER           "Screen"
#define CARAWALK_AMBIENT_LIGHT         "Ambient"
#define CARAWALK_CUSTOM                "Custom"
#define CARAWALK_COLORS                "Colors"
#define CARAWALK_LIGHT_ON              "LightOn"
#define CARAWALK_TEX_ON                "TexOn"
#define CARAWALK_BOX_ON                "BoxOn"
#define CARAWALK_DETAIL                "Detail"
#define CARAWALK_TEX_FLAG              "TexFlag"
#define CARAWALK_THETA                 "Theta"
#define CARAWALK_PHI                   "Phi"

#define CARA_TYPE                0x41524143


#define TEX_MIN_LIN 0x0001
#define TEX_MAG_LIN 0x0002

bool     CCaraWalkDll::gm_bColorIndex         = false;
// HANDLE   CCaraWalkDll::gm_hHeap               = NULL;
int      CCaraWalkDll::gm_nHeapInstances      = 0;
bool     CCaraWalkDll::gm_bBugFixNormalVector = BUGFIX_NORMALVECTOR_DEFAULT;
bool     CCaraWalkDll::gm_bBugFixLightTexture = BUGFIX_LIGHTTEXTURE_DEFAULT;
int      CCaraWalkDll::gm_nPolygonOffset      = BUGFIX_POLYGON_OFFSET_DEFAULT;
float    CCaraWalkDll::gm_fPolygonFactor      = BUGFIX_DISTANCE_FACTOR_DEFAULT;
bool     CCaraWalkDll::gm_bTextureObject      = false;
bool     CCaraWalkDll::gm_bShowShadows        = CARAWALK_BOX_SHADOWS_DEFAULT;
char     CCaraWalkDll::gm_szCaraDir[_MAX_PATH]= "";

extern HANDLE g_hHeap;

struct BoxDataRed
{
   char     szName[BOX_NAME_LENGTH];
   WORD     nMaterial;
};

// Konstruktor ohne Argumente
CCaraWalkDll::CCaraWalkDll()
{
   BEGIN("CCaraWalkDll()");

   InitCaraWalk();                           // Initialisierung der Carawalk Parameter

   m_bDrawBoxes        = true;
   m_bInitBoxLists     = false;
   m_bInitShadowLists  = false;
   m_bLight            = true;
   m_bTexture          = false;
   m_cAmbientLight     = RGB( 52, 52, 52);
   m_fScaleFactor      = 1.0f;
   m_nDetail           = 2;
   m_nViewMode         = ROOMVIEW;
   m_nNoOfLights       = 0;
   m_nTheta            = -200;
   m_nPhi              = 2400;
   m_pLight            = NULL;
   m_pRoom             = NULL;
   m_bShowScale        = false;
   m_nTexFlags         = TEX_MIN_LIN|TEX_MAG_LIN;
   m_bSetCursor        = true;
   ZeroMemory(&m_vScale, sizeof(CVector));
   m_pszNewBox         = NULL;

}

// Konstruktor mit Argumenten
CCaraWalkDll::CCaraWalkDll(CCaraWalkDll *pGL) : CEts3DGL(pGL)
{
   BEGIN("CCaraWalkDll(pGL)");

   InitCaraWalk();                           // Initialisierung durch ein "Eltern"objekt

   m_bDrawBoxes        = pGL->m_bDrawBoxes;
   m_bInitBoxLists     = true;
   m_bInitShadowLists  = true;
   m_bLight            = pGL->m_bLight;
   m_Boxes.ADDHead_Splitted(&pGL->m_Boxes);
   m_bTexture          = pGL->m_bTexture;
   m_cAmbientLight     = pGL->m_cAmbientLight;
   m_fScaleFactor      = pGL->m_fScaleFactor;
   m_nDetail           = pGL->m_nDetail;
   m_nViewMode         = pGL->m_nViewMode;
   m_nNoOfLights       = pGL->m_nNoOfLights;
   m_nPhi              = pGL->m_nPhi;
   m_nTheta            = pGL->m_nTheta;
   m_pLight            = pGL->m_pLight;
   m_pRoom             = pGL->m_pRoom;
   m_vScale            = pGL->m_vScale;
   m_bShowScale        = pGL->m_bShowScale;
}

CCaraWalkDll::~CCaraWalkDll()
{
   BEGIN("~CCaraWalkDll()");
   Destroy();
   if (m_pLight)   delete[] m_pLight;
   if (m_pRoom)    delete m_pRoom;
   if (m_pBoxProp) delete m_pBoxProp;
   DeleteBoxData();
   FreeBoxName();
}

/******************************************************************************
* Name       : InitCaraWalk                                                   *
* Definition : void InitCaraWalk();                                           *
* Zweck      : Initialisierung der Daten in den Konstruktoren                 *
* Aufruf     : InitCaraWalk();                                                *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::InitCaraWalk()
{
   BEGIN("InitCaraWalk");
   m_Boxes.SetDestroyMode(true);
   m_Boxes.SetDeleteFunction(CBoxObj::DeleteFunction);
   m_bLButtonDown  = false;
   m_bRButtonDown  = false;
   m_bLapInvalid   = false;
   m_bGhostMode    = false;
   m_bFlyMode      = false;
   m_hParentMsgWnd = NULL;
   m_pBoxProp      = NULL;
   SetRefreshTime(CARAWALK_REFRESH_TIME_DEFAULT);
   InitGLSettings();
}
void CCaraWalkDll::InitGLSettings()
{
   try
   {
      HKEY hKey = NULL;
      m_Bits.nGeneric = CARAWALK_GENERIC_DEFAULT;
      m_Bits.nGenAccel= CARAWALK_GENERIC_ACC_DEFAULT;
      m_Bits.nColor   = CARAWALK_COLOR_BITS_DEFAULT;
      m_Bits.nDepth   = CARAWALK_DEPTH_BITS_DEFAULT;
      m_Bits.nStencil = CARAWALK_STENCIL_BITS_DEFAULT;
      m_Bits.nAlpha   = CARAWALK_ALPHA_BITS_DEFAULT;
      m_Bits.nAccum   = CARAWALK_ACCUM_BITS_DEFAULT;
      DWORD dwAlpha   = CARAWALK_PERSPECTIVE_ANGLE_DEFAULT;

      if (OpenRegistry(g_hInstance, KEY_READ, &hKey, CARAWALK_GL_SETTINGS))
      {
         m_Bits.nGeneric = GetRegDWord(hKey, CARAWALK_GENERIC     , m_Bits.nGeneric);
         m_Bits.nGenAccel= GetRegDWord(hKey, CARAWALK_GENERIC_ACC , m_Bits.nGenAccel);
         m_Bits.nColor   = GetRegDWord(hKey, CARAWALK_COLOR_BITS  , m_Bits.nColor);
         m_Bits.nDepth   = GetRegDWord(hKey, CARAWALK_DEPTH_BITS  , m_Bits.nDepth);
         m_Bits.nStencil = GetRegDWord(hKey, CARAWALK_STENCIL_BITS, m_Bits.nStencil);
         m_Bits.nAlpha   = GetRegDWord(hKey, CARAWALK_ALPHA_BITS  , m_Bits.nAlpha);
         m_Bits.nAccum   = GetRegDWord(hKey, CARAWALK_ACCUM_BITS  , m_Bits.nAccum);
         SetRefreshTime(   GetRegDWord(hKey, CARAWALK_REFRESH_TIME, CARAWALK_REFRESH_TIME_DEFAULT));
         dwAlpha         = GetRegDWord(hKey, CARAWALK_PERSPECTIVE_ANGLE, dwAlpha);
         if (dwAlpha == 0)
         {
            SetModes(ETS3D_PR_ORTHOGONAL, ETS3D_PR_PERSPECTIVE);
         }
         SetAlpha((double) dwAlpha);
      
         if (m_Bits.nColor   > 32) m_Bits.nColor   = CARAWALK_COLOR_BITS_DEFAULT;
         if (m_Bits.nDepth   > 32) m_Bits.nDepth   = CARAWALK_DEPTH_BITS_DEFAULT;
         if (m_Bits.nStencil > 32) m_Bits.nStencil = CARAWALK_STENCIL_BITS_DEFAULT;
         if (m_Bits.nAlpha   > 32) m_Bits.nAlpha   = CARAWALK_ALPHA_BITS_DEFAULT;
         if (m_Bits.nAccum   > 32) m_Bits.nAccum   = CARAWALK_ACCUM_BITS_DEFAULT;
         gm_bShowShadows = GetRegDWord(hKey, CARAWALK_BOX_SHADOWS, gm_bShowShadows) ? true : false;
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

         DWORD dwAmbient  = GetRegDWord(hKey, CARAWALK_TEX_AMBIENT, CARAWALK_TEX_AMBIENT_DEFAULT);
         CGlLight::ColorToFloat(dwAmbient, CMaterial::gm_pfAmbient);
         DWORD dwDiffuse  = GetRegDWord(hKey, CARAWALK_TEX_DIFFUSE, CARAWALK_TEX_DIFFUSE_DEFAULT);
         CGlLight::ColorToFloat(dwDiffuse, CMaterial::gm_pfDiffuse);
         DWORD dwSpecular = GetRegDWord(hKey, CARAWALK_TEX_SPECULAR, CARAWALK_TEX_SPECULAR_DEFAULT);
         CGlLight::ColorToFloat(dwSpecular, CMaterial::gm_pfSpecular);

         DWORD dwTemp  = GetRegDWord(hKey, CARAWALK_TEX_AVG_B, CARAWALK_TEX_AVG_B_DEFAULT);
         CMaterial::gm_fb = 0.001f * dwTemp;
         dwTemp  = GetRegDWord(hKey, CARAWALK_TEX_AVG_M, CARAWALK_TEX_AVG_M_DEFAULT);
         CMaterial::gm_fm = 0.001f * (int)dwTemp;
         dwTemp  = GetRegDWord(hKey, CARAWALK_TEX_AMBIENT_FACTOR, CARAWALK_TEX_AMBIENTF_DEFAULT);
         CMaterial::gm_fAmbient = 0.001f * dwTemp;

         EtsRegCloseKey(hKey);
      }
      else
      {
         SetRefreshTime(42);
         SetAlpha((double) dwAlpha);
      }

      char szRegKey[_MAX_PATH];
      wsprintf(szRegKey, "%s\\%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY, REGKEY_BUGFIX);
      if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
      {
         gm_bBugFixNormalVector = GetRegDWord(hKey, REGKEY_BUGFIX_NORMALVECTOR, gm_bBugFixNormalVector) ? true : false;
         gm_bBugFixLightTexture = GetRegDWord(hKey, REGKEY_BUGFIX_LIGHTTEXTURE, gm_bBugFixLightTexture) ? true : false;
         gm_nPolygonOffset      = GetRegDWord(hKey, REGKEY_BUGFIX_POLYGON_OFFSET, gm_nPolygonOffset);
         gm_fPolygonFactor      = (float)(0.00001 * GetRegDWord(hKey, REGKEY_BUGFIX_DISTANCE_FACTOR, 5000));

         EtsRegCloseKey(hKey);
      }
   }
   catch (DWORD dwError)
   {
      REPORT("InitGLSettings %08x", dwError);
   }
}

/******************************************************************************
* Name       : AutoDelete                                                     *
* Definition : virtual void AutoDelete();                                     *
* Zweck      : Wird aufgerufen wenn sich das Objekt automatisch selbst löscht *
*              Die Funktion AutoDelete der Basisklasse muß am Ende der        *
*              Funktion aufgerufen werden.                                    *
* Aufruf     : durch die Windowfunktion der OpenGL Basisklasse                *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::AutoDelete()
{
   BEGIN("AutoDelete");
   if (GetModes() & ETS3D_DETACH_FOREIGN_DATA)  // Fremde daten entkoppeln
   {
      m_pRoom  = NULL;
      m_pLight = NULL;
      m_Boxes.SetDestroyMode(false);
      m_Boxes.Destroy();
   }
   CEts3DGL::AutoDelete();
}

/******************************************************************************
* Name       : DeleteBoxData                                                  *
* Definition : void DeleteBoxData();                                          *
* Zweck      : Löschen der Boxendaten aller Boxen in der Liste                *
* Aufruf     : DeleteBoxData();                                               *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::DeleteBoxData()
{
   if (m_Boxes.GetCounter() > 0)
   {
      CAutoCriticalSection acs(*this);
      m_Boxes.Destroy();
   }
}

/******************************************************************************
* Name       : InitGLLists                                                    *
* Definition : virtual void InitGLLists();                                    *
* Zweck      : Initialisierung der OpenGL Parameter für die 3D-Darstellung am *
*              Anfang des Renderthreads.                                      *
* Aufruf     : Am Anfang des OpenGL-Render-Threads nach der Initialisierung   *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::InitGLLists()
{
   BEGIN("InitGLLists");
   SetClearBits(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   if (m_pLight == NULL)
   {
      bool   bRegistry = false;
      HKEY hKey = NULL;
      char szSubKey[64];

      switch (m_nViewMode)
      {
         case BOXVIEW:     strcpy(szSubKey, CARAWALK_BOX); break;
         case ROOMVIEW:    strcpy(szSubKey, CARAWALK_ROOM);break;
         case SCREENSAVER: strcpy(szSubKey, CARAWALK_SCREENSAVER);break;
      }
      CreateList(PRE_COMMAND, (LISTFUNCTION)CCaraWalkDll::PreCommands, 1);
      if (OpenRegistry(g_hInstance, KEY_READ, &hKey, szSubKey))
      {
         DWORD nCount = GetRegDWord(hKey, CARAWALK_NO_OF_LIGHTS, 0);
         if ((nCount > 0) && (nCount < 9))                       // Anzahl prüfen
         {
            m_pLight = new CGlLight[nCount];
            m_nNoOfLights = nCount;
            for (DWORD i=0; i<nCount; i++)                       // Beleuchtungslisten
            {
               wsprintf(szSubKey, "%s%d", CARAWALK_LIGHT, i);
               DWORD dwSize = sizeof(CGlLight);
               if (GetRegBinary(hKey, szSubKey, &m_pLight[i], dwSize))
               {
                  CreateList(   LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightPositionList, 1);
                  CreateCommand(LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightCommandList , 1);
               }
               else
               {
                  delete[] m_pLight;
                  m_pLight      = NULL;
                  m_nNoOfLights = 0;
                  break;
               }
            }
         }
         if (m_pLight)
         {
            bRegistry = true;                                      // Lichtdaten aus cfg-Datei
            m_cAmbientLight =  GetRegDWord(hKey, CARAWALK_AMBIENT_LIGHT, m_cAmbientLight);
            m_nDetail       =  GetRegDWord(hKey, CARAWALK_DETAIL       , m_nDetail);
            m_bLight        = (GetRegDWord(hKey, CARAWALK_LIGHT_ON     , 1) != 0) ? true : false;
            m_bTexture      = (GetRegDWord(hKey, CARAWALK_TEX_ON       , 0) != 0) ? true : false;
            m_bDrawBoxes    = (GetRegDWord(hKey, CARAWALK_BOX_ON       , 1) != 0) ? true : false;
            m_nTexFlags     =  GetRegDWord(hKey, CARAWALK_TEX_FLAG     ,  m_nTexFlags);
            if (m_nViewMode == SCREENSAVER)
            {
               m_nTheta        =  GetRegDWord(hKey, CARAWALK_THETA, 0);
               m_nPhi          =  GetRegDWord(hKey, CARAWALK_PHI  , 0);
            }
         }
         EtsRegCloseKey(hKey);
      }
      if (OpenRegistry(g_hInstance, KEY_READ, &hKey, CARAWALK_CUSTOM))
      {
         DWORD dwSize = sizeof(COLORREF)*NUM_CUSTOMCOLORS;
         GetRegBinary(hKey, CARAWALK_COLORS, CEtsDialog::gm_CustomColors, dwSize);
         EtsRegCloseKey(hKey);
      }
      if (!bRegistry)                                          // wenn keine Registryeinträge vorhanden sind
      {
         m_bLight = true;
         if (m_pLight == NULL) m_pLight = new CGlLight[NO_OF_LIGHTS];
         m_nNoOfLights = NO_OF_LIGHTS;
         for (int i=0; i<NO_OF_LIGHTS; i++)                    // Beleuchtungslisten erstellen
         {
            CreateList(   LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightPositionList, 1);
            CreateCommand(LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightCommandList , 1);
            m_pLight[i].m_pfPosition[3] = 1.0f;
            m_pLight[i].m_nLight = GL_LIGHT0 + i;
         }
      }
   }
   else
   {
      for (int i=0; i<m_nNoOfLights; i++)                       // Beleuchtungslisten
      {
         CreateList(   LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightPositionList, 1);
         CreateCommand(LIST_LIGHT+i, (LISTFUNCTION)CCaraWalkDll::LightCommandList , 1);
      }
   }

   if (GetModes() & ETS3D_CM_RGB)                              // Texturen nur im RGB-Modus
   {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   }
   else m_bTexture = false;

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
/*
   GL_ARB_multitexture
   GL_EXT_abgr
   GL_EXT_bgra
   GL_EXT_clip_volume_hint
   GL_EXT_compiled_vertex_array
   GL_EXT_fog_coord
   GL_EXT_packed_pixels
   GL_EXT_point_parameters
   GL_EXT_secondary_color
   GL_EXT_separate_specular_color
   GL_EXT_stencil_wrap
   GL_EXT_texture_env_add
   GL_EXT_texture_env_combine
   GL_EXT_texture_object
   GL_EXT_vertex_array
   GL_EXT_vertex_weighting
   GL_KTX_buffer_region
   GL_NV_fog_distance
   GL_NV_texgen_reflection
   GL_NV_texture_env_combine4
   GL_WIN_swap_hint
   WGL_EXT_swap_control

*/
   const GLubyte *pText = glGetString(GL_EXTENSIONS);
   if (pText)
   {
      REPORT("%s", pText);
      if (strstr((const char*)pText, "GL_EXT_texture_object"))
      {
         gm_bTextureObject = true;
      }
//      char *pstr = strstr((const char*)pText, "EXT_polygon_offset");
   }

   gm_bTextureObject = false; // !! Dies geht noch nicht so wie ich es will !!

   VERIFY(CreateCommand(AMBIENT_LIGHT, (LISTFUNCTION)CCaraWalkDll::AmbientLight, 1));
   if (gm_bTextureObject)
      VERIFY(CreateList(LIST_ROOM, (LISTFUNCTION)CCaraWalkDll::RoomListEx, MAX_ROOMLISTS));
   else
      VERIFY(CreateList(LIST_ROOM, (LISTFUNCTION)CCaraWalkDll::RoomList  , MAX_ROOMLISTS));

   VERIFY(CreateList(LIST_BOX, (LISTFUNCTION)CCaraWalkDll::LoudspeakerList, MAX_BOXLISTS));

   for (int i=0; i<m_nNoOfLights; i++)
   {
      PerformList(LIST_LIGHT+i, m_bLight);
      PerformCommand(LIST_LIGHT+i, m_bLight);
   }
   PerformCommand(AMBIENT_LIGHT, m_bLight);
   PerformList(LIST_BOX, m_bDrawBoxes);

   if (m_nViewMode == BOXVIEW)
   {
      char szFileName[_MAX_PATH];

      if (gm_bShowShadows)
         VERIFY(CreateList(LIST_SHADOW, (LISTFUNCTION)CCaraWalkDll::ShadowList, MAX_ROOMLISTS));
      VERIFY(CreateList(LIST_SCALE , (LISTFUNCTION)CCaraWalkDll::ScaleList , 1));
      PerformList(LIST_SCALE , false);
      glEnable(GL_NORMALIZE);

      GetCaraDirectory(_MAX_PATH, szFileName);
      strncat(szFileName, ROOM_DATA_FILE, _MAX_PATH);
      HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      OnInitTurnBoxData(hFile);
      CloseHandle(hFile);
   }
   else if ((m_nViewMode == ROOMVIEW) || (m_nViewMode == SCREENSAVER))
   {
      CBoxObj::SetSpecularMatComp(0, 0, 0);
      int i, nCount = IDM_VIEW_DETAIL_VERYHIGH-IDM_VIEW_DETAIL_VERYLOW;
      for (i=0; i<=nCount; i++)
      {
         Check3DControlItem(i+IDM_VIEW_DETAIL_VERYLOW, (m_nDetail == i) ? RADIOCHECKED : UNCHECKED);
      }
      Check3DControlItem(IDM_VIEW_LIGHT, m_bLight ? CHECKED : UNCHECKED);
      UINT                                nMode = UNCHECKED;
      if ((GetModes() & ETS3D_CM_RGB)==0) nMode = DISABLE_ITEM;
      else if (m_bTexture)                nMode = CHECKED;
      Check3DControlItem(IDM_VIEW_TEXTURE, nMode);
      Check3DControlItem(IDM_VIEW_BOXES  , m_bDrawBoxes ? CHECKED : UNCHECKED);
      Check3DControlItem(IDM_VIEW_TEX_MAG_LINEAR, ((m_nTexFlags&TEX_MAG_LIN)!=0) ? CHECKED : UNCHECKED);
      Check3DControlItem(IDM_VIEW_TEX_MIN_LINEAR, ((m_nTexFlags&TEX_MIN_LIN)!=0) ? CHECKED : UNCHECKED);

      if (m_nViewMode == SCREENSAVER)
      {
         float fMaxDimension = 20;
         SetVolume(-fMaxDimension, fMaxDimension, fMaxDimension, -fMaxDimension, 0.4f / fMaxDimension, fMaxDimension);
         m_Lap.dEyex  = 0;
         m_Lap.dEyey  = 0;
         m_Lap.dEyez  = 0;
         m_Lap.nPhi   = 0;
         m_Lap.nXi    = 0;
         m_Lap.nTheta = 0;
         glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
         m_pLight[0].SetSpecularColor(RGB(100, 100, 100));
         m_dDistanceFactor = (double)gm_fPolygonFactor * fMaxDimension / (double)m_Bits.nDepth;
         SetScreenSaverLight();
      }
   }
}

/******************************************************************************
* Name       : DestroyGLLists                                                 *                                                
* Definition : virtual void DestroyGLLists();                                 *
* Zweck      : Löschen der OpenGL-Listen und Speichern der Einstellungen in   *
*              der Registry.                                                  *
* Aufruf     : Am Ende des OpenGL-Render-Threads.                             *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::DestroyGLLists()
{
   BEGIN("DestroyGLLists");
   if (glIsList(LIST_FONT_BASE)) 
      glDeleteLists(LIST_FONT_BASE, LIST_FONT_SIZE);

   if (m_pRoom) m_pRoom->DeleteData();

   HKEY hKey = NULL;
   char szSubKey[64];
   switch (m_nViewMode)
   {
      case BOXVIEW:     strcpy(szSubKey, CARAWALK_BOX); break;
      case ROOMVIEW:    strcpy(szSubKey, CARAWALK_ROOM);break;
      case SCREENSAVER: strcpy(szSubKey, CARAWALK_SCREENSAVER);break;
   }
   if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, szSubKey))
   {
      SetRegDWord(hKey, CARAWALK_NO_OF_LIGHTS, m_nNoOfLights);
      for (int i=0; i<m_nNoOfLights; i++)                       // Beleuchtungslisten
      {
         wsprintf(szSubKey, "%s%d", CARAWALK_LIGHT, i);
         SetRegBinary(hKey, szSubKey, (BYTE*)&m_pLight[i], sizeof(CGlLight));
      }
      SetRegDWord(hKey, CARAWALK_AMBIENT_LIGHT, m_cAmbientLight);
      SetRegDWord(hKey, CARAWALK_DETAIL       , m_nDetail);
      //SetRegDWord(hKey, CARAWALK_LIGHT_ON     , m_bLight);
      SetRegDWord(hKey, CARAWALK_TEX_ON       , m_bTexture);
      SetRegDWord(hKey, CARAWALK_BOX_ON       , m_bDrawBoxes);
      SetRegDWord(hKey, CARAWALK_TEX_FLAG     , m_nTexFlags);
      if (m_nViewMode == SCREENSAVER)
      {
         SetRegDWord(hKey, CARAWALK_THETA, m_nTheta);
         SetRegDWord(hKey, CARAWALK_PHI  , m_nPhi);
      }
      EtsRegCloseKey(hKey);
   }

   if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, CARAWALK_CUSTOM))
   {
      RegSetValueEx(hKey, CARAWALK_COLORS, 0, REG_BINARY, (BYTE*)CEtsDialog::gm_CustomColors, sizeof(COLORREF)*NUM_CUSTOMCOLORS);
      EtsRegCloseKey(hKey);
   }
}

/******************************************************************************
* Name       : OnRenderSzene                                                  *
* Definition : virtual void OnRenderSzene()                                   *
* Zweck      : Rendern der 3D-Szene. Die Funktion CEts3DGL::OnRenderSzene()   *
*              ruft die erstellten OpenGL-Listen in der Erstellungsreihenfolge*
*              auf.                                                           *
* Aufruf     : durch die Thread-Nachricht WM_RENDER_SZENE                     *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnRenderSzene()
{
   BEGIN("OnRenderSzene");
   glLoadIdentity();
   CEts3DGL::OnRenderSzene();
   PostMessage(WM_RESET_CURSOR_AFTER_RENDERING, 0, 0);
}

/******************************************************************************
* Name       : OnTimer                                                        * 
* Definition : virtual LRESULT OnTimer(long, BOOL);                           *
* Zweck      : Verarbeitung der Timernachricht für die Steuerung einer        *
*              Animation bzw. Update von ungültigen Listen und Variablen      *
* Aufruf     : durch WM_TIMER mit dem CONTROL_TIMER_EVENT.                    *
* Parameter  :                                                                *
* nID         (E): Event Nr. des Timers                           (long)      *
* bValidate3D (E): Zeiger auf Timerfunktion                       (BOOL)      *
* Funktionswert : 0                                               (LRESULT)   *
******************************************************************************/
LRESULT CCaraWalkDll::OnTimer(long /*nID*/, BOOL bValidate3D)
{
   if (bValidate3D && m_bLapInvalid)
   {
//      REPORT("LookAt and Emil");
      if (m_nViewMode == ROOMVIEW)
      {
         SetEmilsLight();
         SetLookAt(m_Lap);
      }
      else if (m_nViewMode == BOXVIEW)
      {
         SetTurnBoxLight();
         SetLookAt(m_Lap);
      }
      else // if (m_nViewMode == SCREENSAVER)
      {
         SetScreenSaverLight();
      }

      m_bLapInvalid = false;
   }
   if (m_pszNewBox && IsInitialized())
   {
      if (OnSetBoxData(m_NewBoxwParam, m_pszNewBox))
      {
         FreeBoxName();
      }
   }
   return 0;
}

/******************************************************************************
* Name       : OnMessage                                                      *
* Definition : virtual bool OnMessage(UINT, WPARAM, LPARAM);                  *
* Zweck      : Verarbeitung der Message, die an das OpenGL-Fensters gesendet  *
*              werden.                                                        *
* Aufruf     : Durch die Nachrichtenschleife des OpenGL-Fensters.             *
* Parameter  :                                                                *
* nMsg    (E): ID der Nachricht                                      (UINT)   *
* wParam  (E): wParameter der Nachricht                              (WPARAM) *
* lParam  (E): lParameter der Nachricht                              (LPARAM) *
* Funktionswert : true, wenn die Nachricht verarbeitet wurde und nicht mehr   *
*              weiter verarbeitet werden soll (z.B.: durch DefWindowProc()).  *
******************************************************************************/
bool CCaraWalkDll::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
//   BEGIN("OnMessage");
   switch (nMsg)
   {
      case WM_MOUSEMOVE:
      {  
         POINT ptMouse = {LOWORD(lParam), HIWORD(lParam)};
         OnMouseMove(ptMouse, (UINT) wParam);
      } break;
      case WM_RESET_CURSOR_AFTER_RENDERING:
      {
         BEGIN("WM_RESET_CURSOR_AFTER_RENDERING");
         SetWalkCursor(false, true);
      } break;
      case WM_CONTEXTMENU:
      {
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnContextMenu(ptMouse.x, ptMouse.y, (HWND)wParam); 
      } break;
      case WM_LBUTTONDOWN:
      {  
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnLButtonDown(ptMouse, (UINT) wParam);
      } break;
      case WM_RBUTTONDOWN:
      {  
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnRButtonDown(ptMouse, (UINT) wParam);
      } break;
      case WM_LBUTTONUP:
      {  
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnLButtonUp(ptMouse, (UINT) wParam);
      } break;
      case WM_RBUTTONUP:
      {  
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnRButtonUp(ptMouse, (UINT) wParam);
      } break;
      case WM_CHAR:
      {
         WORD wCd = 0;
         WORD wNf = 2;
         if (lParam & MK_SHIFT)   wNf *= 2;
         if (lParam & MK_CONTROL) wParam += 1000;
         switch (wParam)
         {
            case   38: wCd = IDM_WALK_FORWARD; break;
            case   40: wCd = IDM_WALK_BACKWARD; break;
            case   37: wCd = IDM_WALK_TURNLEFT;wNf *= 2; break;
            case   39: wCd = IDM_WALK_TURNRIGHT;wNf *= 2; break;
            case   33: wCd = IDM_WALK_LOOKUP; break;
            case   34: wCd = IDM_WALK_LOOKDOWN; break;
            case 1038: wCd = IDM_WALK_FORWARD_CTRL; break;
            case 1040: wCd = IDM_WALK_BACKWARD_CTRL; break;
            case 1037: wCd = IDM_WALK_TURNLEFT_CTRL;wNf *= 2; break;
            case 1039: wCd = IDM_WALK_TURNRIGHT_CTRL;wNf *= 2; break;
            case 1033: wCd = IDM_WALK_LOOKUP_CTRL; break;
            case 1034: wCd = IDM_WALK_LOOKDOWN_CTRL; break;
         }
         if (wCd) OnWalk(wCd , wNf, NULL);
      } break;
      case WM_KEYDOWN:
         REPORT("WM_KEYDOWN");
         break;
      case WM_KEYUP:
         REPORT("WM_KEYUP");
         break;
      case WM_HELP: return OnHelp((HELPINFO *) lParam);
      case WM_SETCURSOR: 
         return OnSetCursor((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
   }
   return false;
}

/******************************************************************************
* Name       : OnCommand                                                      *
* Definition : virtual LRESULT OnCommand(WORD, WORD, HWND);                   *
* Zweck      : Verarbeitung der Nachricht WM_COMMAND.                         *
* Aufruf     : Durch die Nachrichtenschleife des OpenGL-Fensters.             *
* Parameter  :                                                                *
* nIDCommand(E) : ID des Commands                                     (WORD)  *
* nNotifyCode(E): Notifikations Code des Commands                     (WORD)  *
* hwndFrom(E)   : Handle des Fensters, das die Nachricht gesendet hat (HWND)  *
* Funktionswert : 1, wenn die Nachricht verarbeitet wurde und nicht  (LRESULT)*
*              an die Funktion OnCommand() der Basisklasse weitergeleitet     *
*              werden soll.                                                   *
******************************************************************************/
LRESULT CCaraWalkDll::OnCommand(WORD nIDCommand, WORD nNotifyCode, HWND hwndFrom)
{
   if ((nIDCommand >= IDM_WALK_FORWARD) && (nIDCommand <= IDM_WALK_GEAR_RIGHT_CTRL))
   {
      OnWalk(nIDCommand, nNotifyCode, hwndFrom);
   }
   else if ((nIDCommand >= IDM_VIEW_DETAIL) && (nIDCommand <= IDM_VIEW_DETAIL_VERYHIGH))
   {
      OnViewDetail(nIDCommand);                          // Auflösung einstellen
   }
   else
   {
      switch (nIDCommand)
      {
         case IDM_VIEW_TEXTURE:  OnViewTexture(nIDCommand); break;// Textur an/ausschalten
         case IDM_VIEW_LIGHT:    OnViewLight(nIDCommand);   break;// Licht an/ausschalten
         case IDM_VIEW_BOXES:    OnViewBoxes(nIDCommand);   break;// Boxen ein/ausblenden
         case IDM_OPTIONS_LIGHT: OnOptionsLight();          break;// Dialog für Lichteinstellungen
         case IDM_OPTIONS_BOX:   OnOptionsBox();            break;// Dialog für Boxeneinstellungen
         case IDD_LIGHTPARAM:    OnChangeLight(nNotifyCode, hwndFrom); break;// Nachricht vom Dialog für Lichteinstellungen
         case IDM_SELECTALL:     OnEditCopyGrafic();        break;// 
         case IDD_BOX_PROPERTIES:          // Nachrichten von Dialogen für Boxeneinstellungen
            m_bInitBoxLists = true;
         case IDD_BOX_POSITION: 
            InvalidateList(LIST_BOX);
            break;
         case IDM_OPTIONS_GHOSTMODE: 
         {
            m_bGhostMode = !m_bGhostMode;
         } break;
         case IDM_OPTIONS_FLYMODE:
         {
            m_bFlyMode = !m_bFlyMode;
         } break;
         case IDM_VIEW_TEX_MIN_LINEAR:
         {
            if (m_nTexFlags & TEX_MIN_LIN) m_nTexFlags &= ~TEX_MIN_LIN;
            else                           m_nTexFlags |=  TEX_MIN_LIN;
            InvalidateList(PRE_COMMAND);
            Check3DControlItem(nIDCommand, ((m_nTexFlags&TEX_MIN_LIN)!=0) ? CHECKED : UNCHECKED);
         } break;
         case IDM_VIEW_TEX_MAG_LINEAR:
         {
            if (m_nTexFlags & TEX_MAG_LIN) m_nTexFlags &= ~TEX_MAG_LIN;
            else                           m_nTexFlags |=  TEX_MAG_LIN;
            InvalidateList(PRE_COMMAND);
            Check3DControlItem(nIDCommand, ((m_nTexFlags&TEX_MAG_LIN)!=0) ? CHECKED : UNCHECKED);
         } break;
      }
   }
   return 1;
}

// Listenfunktionen
/******************************************************************************
* Name       : LightPositionList                                              *
* Definition : static void LightPositionList(CCaraWalkDll *, int, int);       *
* Zweck      : Erzeugen der Lichtpositionsliste.                              *
* Aufruf     : Durch die Funktion ValidateRenderLists                         *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID der OpenGL-Liste.                            (int)          *
* nRange  (E): Länge des Bereichs der dazugehörenden Listen.   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::LightPositionList(CCaraWalkDll *pC, int nID, int nRange)
{
   BEGIN("LightPositionList");
   glDeleteLists(nID, nRange);
   
   if (gm_bBugFixLightTexture)                                 // Bugfix für Sci-Tech GL-Direct ICD:
      glDisable(GL_TEXTURE_2D);                                // Sonst wird hier die Lichtliste nicht neu erstellt wird.

   if (pC->m_pLight != NULL)
   {
      int nL = nID - LIST_LIGHT;
      if (nL < pC->m_nNoOfLights)
      {
         glNewList(nID, GL_COMPILE);
         glLightfv(pC->m_pLight[nL].m_nLight, GL_POSITION      , pC->m_pLight[nL].m_pfPosition);
         glLightfv(pC->m_pLight[nL].m_nLight, GL_SPOT_DIRECTION, pC->m_pLight[nL].m_pfDirection);
         glEndList();
      }
   }
}

/******************************************************************************
* Name       : LightCommandList                                               *
* Definition : static void LightCommandList(CCaraWalkDll *, int, int);        *
* Zweck      : Setzen des OpenGL-Commands für das Licht.                      *
* Aufruf     : Durch die Funktion ValidateCommands                            *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID des OpenGL-Commands.                         (int)          *
* nRange  (E): Nicht benutzt                                   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::LightCommandList(CCaraWalkDll *pC, int nID, int /*nRange*/)
{
   BEGIN("LightCommandList");
   if (pC->m_pLight != NULL)
   {
      int nL = nID - LIST_LIGHT;
      if (nL < pC->m_nNoOfLights)
      {
         if (pC->m_pLight[nL].m_bEnabled)
         {
            glEnable(pC->m_pLight[nL].m_nLight);
            glLightfv(pC->m_pLight[nL].m_nLight, GL_AMBIENT              , pC->m_pLight[nL].m_pfAmbientColor);
            glLightfv(pC->m_pLight[nL].m_nLight, GL_DIFFUSE              , pC->m_pLight[nL].m_pfDiffuseColor);
            glLightfv(pC->m_pLight[nL].m_nLight, GL_SPECULAR             , pC->m_pLight[nL].m_pfSpecularColor);
            glLightf( pC->m_pLight[nL].m_nLight, GL_SPOT_CUTOFF          , pC->m_pLight[nL].m_fSpotCutOff);
            glLightf( pC->m_pLight[nL].m_nLight, GL_SPOT_EXPONENT        , pC->m_pLight[nL].m_fSpotExponent);

            glLightf( pC->m_pLight[nL].m_nLight, GL_CONSTANT_ATTENUATION , pC->m_pLight[nL].m_fAttenuation[0]);
            glLightf( pC->m_pLight[nL].m_nLight, GL_LINEAR_ATTENUATION   , pC->m_pLight[nL].m_fAttenuation[1]);
            glLightf( pC->m_pLight[nL].m_nLight, GL_QUADRATIC_ATTENUATION, pC->m_pLight[nL].m_fAttenuation[2]);
         }
         else
         {
            glDisable(pC->m_pLight[nL].m_nLight);
         }
      }
   }
}

/******************************************************************************
* Name       : AmbientLight                                                   *
* Definition : static void AmbientLight(CCaraWalkDll *, int, int);            *
* Zweck      : setzen des Umgebungslichtes.                                   *
* Aufruf     : Durch die Funktion ValidateCommands                            *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID des OpenGL-Commands.                         (int)          *
* nRange  (E): Nicht benutzt                                   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::AmbientLight(CCaraWalkDll *pC, int /*nID*/, int /*nRange*/)
{
   float fAmbient[4];
   fAmbient[3] = 1.0;
   CGlLight::ColorToFloat(pC->m_cAmbientLight, fAmbient);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fAmbient);
}


/******************************************************************************
* Name       : RoomList                                                       *
* Definition : static void RoomList(CCaraWalkDll *, int, int);                *
* Zweck      : Erzeugen der Raumliste.                                        
* Aufruf     : Durch die Funktion ValidateRenderLists                         *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID der OpenGL-Liste.                            (int)          *
* nRange  (E): Länge des Bereichs der dazugehörenden Listen.   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::RoomList(CCaraWalkDll *pC, int nID, int nRange)
{
   BEGIN("RoomList");
   glDeleteLists(nID, nRange);
   if (pC->m_pRoom)
   {
      HPALETTE hPal = NULL;
      if ((pC->GetModes()&ETS3D_CM_RGB)==0)
         hPal = pC->GetHPALETTE();
/*
      double dBias, dRange, dScale;
      int    nBits, nClear; 
      glGetDoublev(GL_DEPTH_BIAS, &dBias);
      glGetIntegerv(GL_DEPTH_BITS, &nBits);
      glGetIntegerv(GL_DEPTH_CLEAR_VALUE, &nClear);
      glGetDoublev(GL_DEPTH_RANGE, &dRange);
      glGetDoublev(GL_DEPTH_SCALE, &dScale);
*/
      pC->m_pRoom->DrawRoom(nID, pC->m_bLight, pC->m_bTexture, pC->m_nDetail, hPal, pC->m_dDistanceFactor);
   }
}

void CCaraWalkDll::RoomListEx(CCaraWalkDll *pC, int nID, int nRange)
{
   BEGIN("RoomList");
   glDeleteLists(nID, nRange);
   if (pC->m_pRoom)
   {
      HPALETTE hPal = NULL;
      if ((pC->GetModes()&ETS3D_CM_RGB)==0)
         hPal = pC->GetHPALETTE();

//      pC->m_pRoom->DrawRoomEx(nID, pC->m_bLight, pC->m_bTexture, pC->m_nDetail, hPal, pC->m_dDistanceFactor);
   }
}
/******************************************************************************
* Name       : LoudspeakerList                                                *
* Definition : static void LoudspeakerList(CCaraWalkDll *, int, int);         *
* Zweck      : Erzeugen der Lautsprecherliste.                                *
* Aufruf     : Durch die Funktion ValidateRenderLists                         *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID der OpenGL-Liste.                            (int)          *
* nRange  (E): Länge des Bereichs der dazugehörenden Listen.   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::LoudspeakerList(CCaraWalkDll *pC, int nID, int /*nRange*/)
{
   BEGIN("LoudspeakerList");
   glDeleteLists(nID, 1);
   int i, j;
   CBoxObj *pBox;
   if (pC->m_bInitBoxLists)
   { 
      HPALETTE hPal = NULL;
      if ((pC->GetModes()&ETS3D_CM_RGB)==0)
         hPal = pC->GetHPALETTE();

      double dDist = pC->m_dDistanceFactor;
      if (pC->m_fScaleFactor != 1.0) dDist /= pC->m_fScaleFactor;

      for (pBox=(CBoxObj*)pC->m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) pC->m_Boxes.GetNext())
      {
         i = pC->m_Boxes.GetActualPosition();
         ASSERT((i>=0) && (i<pC->m_Boxes.GetCounter()));
         if (pBox->GetNoOfPositions() > 0)
         {
            glDeleteLists(nID+i+1, 1);
            glNewList(nID+i+1, GL_COMPILE);
            pBox->DrawBox(pC->m_bTexture, pC->m_bLight, pC->m_nDetail, hPal, false, dDist);
            glEndList();
         }
      }
      pC->m_bInitBoxLists = false;
      glFlush();
   }

   glNewList(nID, GL_COMPILE);    // Basisliste erzeugen
//   glDisable(GL_TEXTURE_2D);
   if (pC->m_bLight) glEnable(GL_LIGHTING);
   else              glDisable(GL_LIGHTING);
   for (pBox=(CBoxObj*)pC->m_Boxes.GetFirst(); pBox!=NULL; pBox = (CBoxObj*) pC->m_Boxes.GetNext())
   {
      i = pC->m_Boxes.GetActualPosition();
      ASSERT((i>=0) && (i<pC->m_Boxes.GetCounter()));
      for (j=0; j<pBox->GetNoOfPositions(); j++)
      {
         glPushMatrix();
         pBox->MoveToPosition(j);
         glCallList(LIST_BOX+i+1);
         pBox->DrawSelection(pC->m_bLight, j);
         glPopMatrix();
      }
   }
   glEndList();
}

/******************************************************************************
* Name       : ShadowList                                                     *
* Definition : static void ShadowList(CCaraWalkDll *, int, int);              *
* Zweck      : Erzeugen der Schattenlisten.                                   *
* Aufruf     : Durch die Funktion ValidateRenderLists                         *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID der OpenGL-Liste.                            (int)          *
* nRange  (E): Länge des Bereichs der dazugehörenden Listen.   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::ShadowList(CCaraWalkDll *pC, int nID, int /*nRange*/)
{
   BEGIN("ShadowList");
   glDeleteLists(nID, 1);
   int  i, j, nWall;
   CBoxObj *pBox;
   if (pC->m_bInitShadowLists)
   {  
      glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
      for (pBox=(CBoxObj*)pC->m_Boxes.GetFirst(); pBox != NULL; pBox = (CBoxObj*) pC->m_Boxes.GetNext())
      {
         i = pC->m_Boxes.GetActualPosition();
         ASSERT((i>=0) && (i<pC->m_Boxes.GetCounter()));
         glDeleteLists(nID+1+i, 1);
         if (pBox->GetNoOfPositions()>0)
         {
            glNewList(nID+1+i, GL_COMPILE);
            pBox->DrawBox(false, false, pC->m_nDetail, NULL, true, 0.001);
            glEndList();
         }
      }
      pC->m_bInitShadowLists = false;
      glFlush();
   }

   if ((pC->m_pRoom != NULL) && pC->m_pLight[0].m_bEnabled) // wenn Raumdaten (Wände) vorhanden sind
   {                                                        // und wenn die Lichtquelle an ist : 
      glNewList(nID, GL_COMPILE);                           // fällt auch Schatten
      if (pC->m_bTexture) glDisable(GL_TEXTURE_2D);
      if (pC->m_bLight)   glDisable(GL_LIGHTING);
      glDisable(GL_CULL_FACE);
      if (pC->m_pLight[0].m_bEnabled)
      {                                                  
         HPALETTE hPal = NULL;
         if ((pC->GetModes()&ETS3D_CM_RGB)==0)
            hPal = pC->GetHPALETTE();
//       glEnable(GL_BLEND);
//       glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
         if (hPal) glIndexi(GetNearestPaletteIndex(hPal, RGB(25,25,25)));
         else      glColor4f(0.1f, 0.1f, 0.1f, 1.f);
//       else      glColor4f(0.1f, 0.1f, 0.1f, 0.6f);
         for (nWall=0; nWall<pC->m_pRoom->GetNoOfWalls(); nWall++)
         {
            if (!pC->m_pRoom->IsValidMatrix(nWall)) continue;
            for (pBox = (CBoxObj*) pC->m_Boxes.GetFirst(); pBox != NULL; pBox = (CBoxObj*) pC->m_Boxes.GetNext())
            {
               i = pC->m_Boxes.GetActualPosition();
               ASSERT((i>=0) && (i<pC->m_Boxes.GetCounter()));
               for (j=0; j<pBox->GetNoOfPositions(); j++)
               {
                  glPushMatrix();
                  glMultMatrixf(pC->m_pRoom->GetWallMatrix(nWall));
                  pBox->MoveToPosition(j);
                  glCallList(nID+i+1);
                  glPopMatrix();
               }
            }
         }
//         glDisable(GL_BLEND);
      }
      glEnable(GL_CULL_FACE);
      if (pC->m_bTexture) glEnable(GL_TEXTURE_2D);
      if (pC->m_bLight)   glEnable(GL_LIGHTING);
      glEndList();
   }
}

/******************************************************************************
* Name       : ScaleList                                                      *
* Definition : static void ScaleList(CCaraWalkDll *, int, int);               *
* Zweck      : Erzeugen der Liste für die Skala.                              *
* Aufruf     : Durch die Funktion ValidateRenderLists                         *
* pC      (E): this-Zeiger der Klasse CCaraWalkDll.            (CCaraWalkDll*)*
* nID     (E): ID der OpenGL-Liste.                            (int)          *
* nRange  (E): Länge des Bereichs der dazugehörenden Listen.   (int)          *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::ScaleList(CCaraWalkDll *pC, int nID, int nRange)
{
   BEGIN("ScaleList");
   glDeleteLists(nID, nRange);

   if (pC->m_bShowScale && (!glIsList(LIST_FONT_BASE)))
   {
      HDC hDC = pC->GetHDC();
      if (hDC != NULL)
      {
         ::SelectObject(hDC, GetStockObject(SYSTEM_FONT));
         if (!wglUseFontBitmaps(hDC, LIST_FONT_OFFSET, LIST_FONT_SIZE, LIST_FONT_BASE))
         {
            REPORT("wglUseFontBmp-Error %d", GetLastError());
         }
      }
   }

   CBoxObj *pBox = (CBoxObj*)pC->m_Boxes.GetFirst();
   if (pBox)
   {
      CVector vPoint = pBox->GetMYRP();
      vPoint.SetY() = Vy(pC->m_vScale) * (double)pC->m_fScaleFactor;
//      REPORT("Punkt %f, %f, %f", Vx(vPoint),Vy(vPoint),Vz(vPoint));
      glNewList(nID, GL_COMPILE);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);

      HPALETTE hPal = NULL;
      if ((pC->GetModes()&ETS3D_CM_RGB)==0)
         hPal = pC->GetHPALETTE();
      if (hPal) glIndexi(GetNearestPaletteIndex(hPal, RGB(255, 0, 0)));
      else      glColor3f(1.0, 0, 0);

      glBegin(GL_LINE_STRIP);
      glVertex3dv((double*)&vPoint);
      if (Vz(vPoint) < 5)
         vPoint.SetX() = 2.5;
      else
         vPoint.SetX() = 3.0;
      glVertex3dv((double*)&vPoint);
      vPoint.SetY() = 0;
      glVertex3dv((double*)&vPoint);
      glEnd();

      if (glIsList(LIST_FONT_BASE))
      {
         char text[128];
         GLboolean bVal;
         wsprintf(text, "%d mm", (int)(1000.0*Vy(pC->m_vScale)+0.5));

         vPoint.SetY() = Vy(pC->m_vScale) * (double)pC->m_fScaleFactor;
         glRasterPos3d(Vx(vPoint), Vy(vPoint)*1.01, Vz(vPoint));
         glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &bVal);
         if (bVal)
         {
            glListBase(LIST_FONT_BASE);
            glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
            glListBase(0);
         }
      }

      glEndList();
   }
}

void CCaraWalkDll::GetCaraDirectory(int nMax, char *szFilePath)
{
   if (gm_szCaraDir[0] == 0)
   {
      GetCurrentDirectory(_MAX_PATH, gm_szCaraDir);
      strupr(gm_szCaraDir);
      char *str = strstr(gm_szCaraDir, BOX_DATA_FILE_DIR);
      if (str)
      {
         str[0] = 0;
         MessageBoxC(IDS_ERROR_LS_BOX_FOLDER, IDS_ERROR_MSG_BOX, MB_OK|MB_ICONSTOP);
      }
      int nLen = strlen(gm_szCaraDir);
      if (gm_szCaraDir[nLen-1] != BACK_SLASH_SIGN)
      {
         gm_szCaraDir[nLen++] = BACK_SLASH_SIGN;
         gm_szCaraDir[nLen]   = 0;
      }
      strcat(gm_szCaraDir, ROOM_DATA_FILE);
      HANDLE hFile = CreateFile(gm_szCaraDir, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      if (hFile != INVALID_HANDLE_VALUE)
      {
         CloseHandle(hFile);
         gm_szCaraDir[nLen] = 0;
      }
      else  // Fehler
      {
         MessageBoxC(IDS_ERROR_WRONG_FOLDER, IDS_ERROR_MSG_BOX, MB_OK|MB_ICONSTOP);
      }
   }
   strncpy(szFilePath, gm_szCaraDir, nMax);
}

/******************************************************************************
* Name       : SetBoxPath                                                     *
* Definition : void SetBoxPath(char *, char *);                               *
* Zweck      : Erzeugen des Pfades für die Lautsprecherdaten                  *
* Aufruf     : SetBoxPath(szFilePath, szName);                                *
* Parameter  :                                                                *
* szFilePath(E): Verzeichnispfad                                     (char*)  *
* szName    (E): Baxendateiname                                      (char*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::SetBoxPath(char *szFilePath, char *szName)
{
   if (strstr(szName, BACK_SLASH_STR) != NULL)
   {
      strcpy(szFilePath, szName);
      return;
   }
   GetCaraDirectory(_MAX_PATH, szFilePath);
   if (szFilePath[lstrlen(szFilePath)-1] != BACK_SLASH_SIGN)
      strcat(szFilePath, BACK_SLASH_STR);
   strupr(szFilePath);
   strcat(szFilePath, BOX_DATA_FILE_DIR);
   strcat(szFilePath, BACK_SLASH_STR);
   strcat(szFilePath, szName);
   if (strstr(szName, BOX_DATA_FILE_EXT) == NULL) strcat(szFilePath, BOX_DATA_FILE_EXT);
   REPORT("%s", szFilePath);
}

/******************************************************************************
* Name       : ReadBoxHeader                                                  *
* Definition : bool ReadBoxHeader(HANDLE, CFileHeader *);                     *
* Zweck      : Lesen des Headers aus dem Boxendatensatz und Verifizierung des *
*              Datensatzes.                                                   *
* Aufruf     : ReadBoxHeader(hFile, pfh);                                     *
* Parameter  :                                                                *
* hFile   (E): Dateinhandle des Datensatzes                     (HANDLE)      *
* pfh     (E): Zeiger auf den FileHeader zur Verifizierung.     (CFileHeader) *
* Funktionswert : (true, false) Datensatz gültig.               (bool)        *
******************************************************************************/
bool CCaraWalkDll::ReadBoxHeader(HANDLE hFile, CFileHeader *pfh)
{
   bool bReturn = false;
   if (hFile != INVALID_HANDLE_VALUE)           // der erste gültige Datensatz
   {
      DWORD           dwBytesRead;
      ReadFile(hFile, pfh, sizeof(CFileHeader), &dwBytesRead, NULL);
      ASSERT(sizeof(CFileHeader) == dwBytesRead);
      if (pfh->IsType(BOX_SOUND_DATA))
      {
         SetFilePointer(hFile, pfh->GetLength(), NULL, FILE_CURRENT);
         ReadFile(hFile, pfh, sizeof(CFileHeader), &dwBytesRead, NULL);
         ASSERT(sizeof(CFileHeader) == dwBytesRead);
         if (pfh->IsType(BOX_GEOMETRICAL_DATA)) bReturn = true;
      }
   }
   return bReturn;
}

/******************************************************************************
* Name       : OnInitTurnBoxData                                              *
* Definition : bool OnInitTurnBoxData(HANDLE);                                *
* Zweck      : Initialisierung des Raumes und weiterer Parameter zur Anzeige  *
*              von Lautsprecherboxen.                                         *
* Aufruf     : OnInitTurnBoxData(hRoomFile);                                  *
* Parameter  :                                                                *
* hRoomFile(E): Dateihandle für die Raumdaten                        (HANDLE) *
* Funktionswert : Daten Initialisiert (true, false)                   (bool)  *
******************************************************************************/
bool CCaraWalkDll::OnInitTurnBoxData(HANDLE hRoomFile)
{
   BEGIN("OnInitTurnBoxData");
   try
   {
      if (hRoomFile == INVALID_HANDLE_VALUE)
         throw (DWORD)1000;//ERR_INVALID_HANDLE;
      CAutoCriticalSection acs(*this);
      if (!GetRoomObj()) throw (DWORD) 2;

      float fMaxDimension = 12;
      if (m_pRoom->Read(hRoomFile))
      {
         fMaxDimension = (float) (m_pRoom->GetMaxDimension()*1.05);// Transformationsparameter setzen
         if (fMaxDimension < 12) fMaxDimension = 12;
//         int ab = 2; // die Bit´s für die Z-Verschiebung
//         m_dDistanceFactor = pow(2, ab) *(fMaxDimension / (pow(2,m_Bits.nDepth)));
         m_dDistanceFactor = (double)gm_fPolygonFactor * fMaxDimension / (double)m_Bits.nDepth;
      }
      else
      {
         m_pRoom->DeleteData();
      }

      SetVolume(-fMaxDimension, fMaxDimension, fMaxDimension, -fMaxDimension, 0.4f / fMaxDimension, fMaxDimension);
      m_Lap.dEyex  = VIEWPOS_X;
      m_Lap.dEyey  = VIEWPOS_Y;
      m_Lap.dEyez  = VIEWPOS_Z;
      m_Lap.nPhi   = 1800;
      m_Lap.nTheta = 0;
      m_Lap.nXi    = 0;
      SetTurnBoxLight();           // Lichtparameter setzen
      SetLookAt(m_Lap);
   }
   catch (DWORD nError)
   {
      SetLastError(nError);
      return false;
   }
   return true;
}

/******************************************************************************
* Name       : OnSetRoomData                                                  *
* Definition : bool OnSetRoomData(void *);                                    *
* Zweck      : Initialisierung der Raumdaten aus einem übergebenen Datensatz  *
*              für den Raumwanderer.                                          *
* Aufruf     : OnSetRoomData(pData);                                          *
* Parameter  :                                                                *
* pData   (E): Zeiger auf den Datensatz                              (void*)  *
* Funktionswert : Datensatz wurde gesetzt (true, false)              (bool)   *
******************************************************************************/
bool CCaraWalkDll::OnSetRoomData(void *pData)
{
   BEGIN("OnSetRoomData");
   if (!IsInitialized()) return false;
   if (pData == NULL)                                          // sind keine Daten vorhanden
   {
      if (m_pRoom)
      {
         CAutoCriticalSection acs(*this);
         m_pRoom->DeleteData();                                // Raumdaten löschen
      }
   }
   else
   {
      long nDataPtr = 0;
      HANDLE hFile = INVALID_HANDLE_VALUE;
      BoxDataRed *pBoxData = NULL;
      try                                                      // Raumdaten lesen
      {
         CAutoCriticalSection acs(*this);

         if (!GetRoomObj()) throw (DWORD) 2;

         nDataPtr = m_pRoom->SetRoomData(pData);               // Raumdaten lesen

         if (nDataPtr == 0)      throw (DWORD)CARAWALK_ROOMDATA_ERROR;

         if ((GetModes() & ETS3D_CM_RGB) == 0)                 // Colorindex Modus
         {
            gm_bColorIndex = true;
            if (m_pRoom->SetPaletteAndIndexes(GetHPALETTE()))  // Palettenfarben setzen
            {
               ::RealizePalette(GetHDC());
            }
         }

         float fMaxDimension = (float) (m_pRoom->GetMaxDimension()*1.05);// Transformationsparameter setzen
//         ETS3DGL_LookAt Lap = {-1, -1, -1, 0, 0, 0};
         if (fMaxDimension < 10) fMaxDimension = 10;
         m_Lap.dEyex  = -1;
         m_Lap.dEyey  = -1;
         m_Lap.dEyez  = -1;
         m_Lap.nTheta = 0;
         m_Lap.nXi    = 0;
         m_Lap.nPhi   = 0;
         // Formel für Abstandsfaktor

//         int ab = 2; // die Bit´s für die Z-Verschiebung
//         m_dDistanceFactor = pow(2, ab) *(fMaxDimension / (pow(2,m_Bits.nDepth)));
         m_dDistanceFactor = (double)gm_fPolygonFactor * fMaxDimension / (double)m_Bits.nDepth;

/*
#ifdef _DEBUG
         CVector ltn, rbf;
         m_pRoom->GetRoomDimensions(ltn, rbf);
         *((CVector*)&m_Lap.dEyex) = (ltn + rbf) * 0.5;
#endif
         */       
         CVector vLBN, vRTF;
         m_pRoom->GetRoomDimensions(vLBN, vRTF);
         SetVolume((float)Vx(vLBN), (float)Vy(vRTF), (float)Vx(vRTF), (float)Vy(vLBN), (float)(0.4/fMaxDimension), fMaxDimension);
//         SetVolume(-fMaxDimension, fMaxDimension, fMaxDimension, -fMaxDimension, 0.4f / fMaxDimension, fMaxDimension);
         m_bLapInvalid = true;
      }
      catch (DWORD nError)
      {
         REPORT("Error at Postion %d", nDataPtr);
         SetDllError(nError);
         return false;
      }

      m_Lap.nPhi = 10 * SendMessageToParent(WM_CHECKPOS, WALKER_STARTPOS, (LPARAM)&m_Lap.dEyex);

      CBoxObj *pBox = NULL;
      try                                                      // Boxendaten lesen
      {
         int nBoxes = m_pRoom->GetNoOfBoxes();                 // 
         if (nBoxes)                                           // sind Boxen vorhanden
         {
            int             i;
            char            szName[_MAX_FNAME], szFilePath[_MAX_PATH];
            CARAWALK_BoxPos bp;
            CFileHeader     fh;
            unsigned long   crc;
            int             nBoxesInDB = 0;
            pBoxData = GetBoxDB(nBoxesInDB);

            CAutoCriticalSection acs(*this);
            if (m_Boxes.GetCounter() > 0)                      // Boxenpositionen löschen
            {
               for (pBox = (CBoxObj*)m_Boxes.GetFirst(); pBox != NULL; pBox = (CBoxObj*)m_Boxes.GetNext())
                  pBox->DeleteAllPositions();
            }

            for (i=0; i<nBoxes; i++)                           // Boxen einlesen
            {
               ReadData(pData, szName, _MAX_FNAME, nDataPtr);
               _strupr(szName);
               for (pBox = (CBoxObj*)m_Boxes.GetFirst(); pBox != NULL; pBox = (CBoxObj*)m_Boxes.GetNext())
               {
                  if (strcmp(szName, pBox->GetFileName()) == 0)// Box gefunden
                  {
                     ReadData(pData, &bp, sizeof(CARAWALK_BoxPos), nDataPtr);
                     CRoomObj::CaraToGL((CVector*) bp.pdPos);

                     pBox->AddPosition(bp);                    // Position hinzufügen
                     break;
                  }
               }
               if (pBox == NULL)                               // keine gleiche gefunden
               {
                  SetBoxPath(szFilePath, szName);              // Boxendaten einlesen
                  hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
                  if (!ReadBoxHeader(hFile, &fh))
                     throw (DWORD)CARAWALK_BOXDATA_ERROR;
                  crc = fh.GetChecksum();                      // Checksumme speichern
                  fh.CalcChecksum();                           // neu initialisieren
                  pBox = new CBoxObj;
                  pBox->SetFileName(szName);
                  if (!pBox->ReadType(hFile, &fh)) throw (DWORD)CARAWALK_BOXDATA_ERROR;
                  if (!pBox->ReadData(hFile, &fh)) throw (DWORD)CARAWALK_BOXDATA_ERROR;
                  if (!fh.IsValid(crc))            throw (DWORD)CARAWALK_BOXDATA_ERROR;

                  DWORD dwBytesRead = 0;
                  ::ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                  if ((dwBytesRead == sizeof(CFileHeader)) && fh.IsType(BOX_MATERIAL_DATA))
                     pBox->ReadMaterial(hFile, &fh, this);

                  if (pBoxData && (pBox->GetMaterial(BOX_MAIN_MATERIAL) == NULL))
                  {
                     int nMatNo = GetBoxMaterialNo(nBoxesInDB, pBoxData, szName);
                     if (nMatNo) pBox->SetMaterial(GetBoxMaterial(nMatNo));
                  }

                  ReadData(pData, &bp, sizeof(CARAWALK_BoxPos), nDataPtr);
                  CRoomObj::CaraToGL((CVector*) bp.pdPos);
                  pBox->AddPosition(bp);

                  m_Boxes.ADDHead(pBox);

                  CloseHandle(hFile);                          // File Schließen
                  hFile = INVALID_HANDLE_VALUE;
               }
               pBox = NULL;
            }
            if (m_Boxes.GetCounter() > 0)                   // ersten Datensatz initialisieren
            {
               for (pBox = (CBoxObj*)m_Boxes.GetFirst(); pBox != NULL; pBox = (CBoxObj*)m_Boxes.GetNext())
               {
                  if (pBox->GetNoOfPositions() == 0)
                  {
                     m_Boxes.Delete(pBox);
                     delete pBox;
                  }
               }
            }
            m_bInitBoxLists    = true;
            InvalidateList(LIST_BOX);

//          InvalidateList(LIST_SHADOW);
//          m_bInitShadowLists = true;
         }
         InvalidateList(LIST_ROOM);
         InvalidateList(LIST_LIGHT);
      }
      catch (DWORD nError)
      {
         if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
         if (pBox)   delete pBox;
         REPORT("Error at File-postion %d", nDataPtr);
         SetDllError(nError);
         return false;
      }
      if (pBoxData) delete pBoxData;
   }
   return true;
}


/******************************************************************************
* Name       : OnSetBoxData                                                   *
* Definition : bool OnSetBoxData(WPARAM, char *);                             *
* Zweck      : Initialisieren der Boxendaten für den BoxView                  *
* Aufruf     : OnSetBoxData(wParam, pszBoxName);                              *
* Parameter  :                                                                *
* wParam    (E): Darstellungsparameter                               (WPARAM) *
* pszBoxName(E): Name der Boxendatei                                 (char*)  *
* Funktionswert : Boxendaten gesetzt (true, false)                   (bool)   *
******************************************************************************/
bool CCaraWalkDll::OnSetBoxData(WPARAM wParam, char *pszBoxName)
{
   BEGIN("OnSetBoxData");
   if (!IsInitialized())
   {
      if (pszBoxName)
      {
         FreeBoxName();
         m_pszNewBox    = _strdup(pszBoxName);
         m_NewBoxwParam = wParam;
      }
      return false;
   }

   CBoxObj *pBox = (CBoxObj*) m_Boxes.GetFirst();
   bool bReturn  = false;

   WORD wMaterialNo = HIWORD(wParam);
   if (pszBoxName)
   {
      _strupr(pszBoxName);
      if (pBox != NULL)                                           // ist eine Box vorhanden und soll
      {                                                           // eine neue gesetzt werden
         char * pszOldBoxName = (char*) pBox->GetFileName();      // vergleichen der alten und neuen Box
         if (strcmp(pszBoxName, pszOldBoxName) == 0)              // true zurückgeben wenn´s dieselbe Box ist
         {
            if ((wMaterialNo != 0xffff) && (wMaterialNo != pBox->GetMaterialNo()))
            {
               if (wMaterialNo == 0) pBox->SetMaterial(NULL);
               else                  pBox->SetMaterial(GetBoxMaterial(wMaterialNo));
               m_bInitBoxLists = true;
               InvalidateList(LIST_BOX);
            }
            return true;                                          // und nicht neu übergeben
         }
      }
   }

   if (IsRendering()) return false;                            // wenn gerade gerendert wird
                                                               // Daten nicht übergeben und
   HANDLE          hFile = INVALID_HANDLE_VALUE;
   char            szFilePath[_MAX_PATH];
   CFileHeader     fh;
   unsigned long   crc;
   try
   {
      if (pszBoxName == NULL)                                     // wird NULL als BoxName übergeben
      {
         if (pBox != NULL)
         {
            DeleteBoxData();
            UpdateBoxLists();
         }
         return true;
      }
      if (m_nViewMode != SCREENSAVER)
      {
         DeleteBoxData();
      }
      pBox = NULL;                                                // Zeiger auf NULL, da alle Boxen gelöscht sind

      SetBoxPath(szFilePath, pszBoxName);                         // Boxen Datei-Pfad holen
      hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      if (ReadBoxHeader(hFile, &fh))                              // Header lesen
      {
         crc = fh.GetChecksum();                                  // Checksumme zwischenspeichern
         fh.CalcChecksum();                                       // Checksumme zurücksetzen
         pBox = new CBoxObj;
         if (pBox)
         {
            pBox->SetFileName(pszBoxName);                        // Namen setzen und  Daten lesen
            if (pBox->ReadType(hFile, &fh) && pBox->ReadData(hFile, &fh) && fh.IsValid(crc))
            {
               DWORD dwBytesRead = 0;
               ::ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
               if ((dwBytesRead == sizeof(CFileHeader)) && fh.IsType(BOX_MATERIAL_DATA))
                  pBox->ReadMaterial(hFile, &fh, this);
               if (pBox->GetMaterial(BOX_MAIN_MATERIAL))
                  wMaterialNo = 0;
               else if (0xffff == wMaterialNo)
               {
                  int      nBoxesInDB;
                  BoxDataRed *pBoxData = GetBoxDB(nBoxesInDB);
                  if (pBoxData)
                  {
                     int nMatNo = GetBoxMaterialNo(nBoxesInDB, pBoxData, pszBoxName);
                     REPORT("Material Nr.: %d", nMatNo);
                     if (nMatNo) pBox->SetMaterial(GetBoxMaterial(nMatNo));
                     delete[] pBoxData;
                  }
               }
               else if (wMaterialNo)
               {
                  pBox->SetMaterial(GetBoxMaterial(wMaterialNo));
               }

               if (m_nViewMode == SCREENSAVER)                 // beim Screensaver wird die Box nicht skaliert
               {
                  Array44 m;

                  CARAWALK_BoxPos bp = {0,0,0,0,0,3};          // Anfangsposition 0,0,3 im ViewVolume
                  pBox->AddPosition(bp);
                  LoadIdentity();
                  GetMatrix(m);
                  pBox->SetMatrix(0, (double*)&m);

                  pBox->m_bShowURP = false;                    // kein Referenzpunkt
                  m_bShowScale     = false;                    // keine Skala
                  m_fScaleFactor   = 1.0;

                  CAutoCriticalSection acs(*this);
                  m_Boxes.ADDHead(pBox);
                  PerformList(LIST_BOX   , true);
                  m_bInitBoxLists    = true;
                  InvalidateList(LIST_BOX);
               }
               else                                            // im Boxview haben sie eine normierte Größe
               {
                  Array44 m;
                  CVector vMin, vMax;
//                  double  dHeight = pBox->GetHeight();
                  pBox->GetMinMaxDimension(vMin, vMax);        // Ausdehnung der Box ermitteln
                  double dMaxDim = Betrag(vMax-vMin);
                  double dScale  = 3.0 / dMaxDim;              // Scalierungsfaktor berechnen
                  if (wParam & CARAWALK_SHOW_URP_AND_SCALE)
                  {
                     m_vScale         = vMax - vMin;
                     pBox->m_bShowURP = true;
                     m_bShowScale     = true;
                  }
                  else if (wParam & CARAWALK_SHOW_SCALE)
                  {
                     m_vScale         = vMax - vMin;
                     pBox->m_bShowURP = (wParam & CARAWALK_SHOW_URP_AND_SCALE) ? true : false;
                     m_bShowScale     = true;
                  }
                  else
                  {
                     pBox->m_bShowURP = true;
                     m_bShowScale     = false;
                  }

                  m_fScaleFactor = (float)dScale;

                  LoadIdentity();                                    // Scalierungsmatrix berechnen
                  Translate(CVector(BOXPOS_X, -Vy(vMin)*dScale, BOXPOS_Z));
                  Scale(CVector(dScale, dScale, dScale));
                  GetMatrix(m);

                  CARAWALK_BoxPos bp = {0,0,0,0,0,0};
                  pBox->AddPosition(bp);
                  pBox->SetMatrix(0, (double*)&m);
                  if (wParam & CARAWALK_SHOW_TWICE)                  // Positionen und Transformatonen setzen
                  {
                     pBox->AddPosition(bp);
                     pBox->SetMatrix(1, (double*)&m);
                  }

                  CAutoCriticalSection acs(*this);

                  m_Boxes.ADDHead(pBox);
                  PerformList(LIST_SCALE , m_bShowScale);
                  PerformList(LIST_BOX   , true);
                  PerformList(LIST_SHADOW, true);
                  m_bInitShadowLists = true;                            // listen neu erzeugen
                  m_bInitBoxLists    = true;
                  InvalidateList(LIST_SHADOW);
                  InvalidateList(LIST_BOX);
                  InvalidateList(LIST_SCALE);
               }

               bReturn = true;
            }
         }
      }
      else
      {
         UpdateBoxLists();
      }
   }
   catch (DWORD dwError)
   {
      REPORT("OnSetBoxData Error %08x", dwError);
   }

   if ((!bReturn) && (pBox != NULL))
   {
      delete pBox;
   }

   if (hFile != INVALID_HANDLE_VALUE)
   {
      CloseHandle(hFile);
   }
   strupr(szFilePath);
   if (strstr(szFilePath, ".TMP") != NULL)
   {
      ::DeleteFile(szFilePath);
   }

   return bReturn;
}

/******************************************************************************
* Name       : OnSetAngle                                                     *
* Definition : void OnSetAngle(int, CARAWALK_BoxPos *);                       *
* Zweck      : Setzen des Drehwinkels bzw. eines Abstandsvectors für die      *
*              Box(en) im Raum.                                               *
* Aufruf     : OnSetAngle(nFlags, pBoxPos);                                   *
* Parameter  :                                                                *
* nFlags  (E): Was zu setzen ist (Winkel, Abstand)                      (int) *
* pBoxPos (E): Zeiger auf Boxenpositionsdaten              (CARAWALK_BoxPos*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnSetAngle(int nFlags, CARAWALK_BoxPos *pBoxPos)
{
   BEGIN("OnSetAngle");

   CBoxObj *pBox = (CBoxObj*) m_Boxes.GetFirst();
   if (pBox)// ist eine Box vorhanden
   {
      CVector         vMin, vMax;
      Array44         m;
      double          dScale;
      int             i;                                       // soll der Abstandsvektor zweier Boxen gesetzt werden
      CAutoCriticalSection acs(*this);
      if (m_nViewMode == SCREENSAVER)
      {
         while (nFlags>0)
         {
            pBox = (CBoxObj*) m_Boxes.GetNext();
            if (pBox==NULL) return;
            nFlags--;
         }

         pBox->SetPosition(0, *pBoxPos);
         InvalidateList(LIST_BOX);
         return;
      }
      else if ((nFlags & CARAWALK_SET_DISTANCE_VECTOR) && (pBox->GetNoOfPositions()==2))
      {
         LoadIdentity();
         GetMatrix(m);
         double *pd = (double*) pBoxPos->pdPos;
         CVector vBoxDistance = CVector(pd[0], pd[1], pd[2]);
         CRoomObj::CaraToGL(&vBoxDistance);
         double pdF[2] = {0.5, -0.5};
         for (i=0; i<pBox->GetNoOfPositions(); i++)            // Abstandsvektor setzen
         {
            pBox->SetMatrix(i, (double*)&m);                   // Einheitsmatrix setzen : Skalierung = 1.0
            pBox->SetPosition(i, vBoxDistance*pdF[i], 0, 0, 0);
            pBox->CalcMinMaxDimension(i, &vMin, &vMax, (i==0) ? true : false);
         }
         dScale = 3.0 / Betrag(vMax-vMin);                     // neuen Skalierungsfaktor berechen
         m_fScaleFactor = (float)dScale;

         LoadIdentity();                                       // neue Matrix berechnen
         Translate(CVector(BOXPOS_X, -Vy(vMin)*dScale, BOXPOS_Z));
         Scale(CVector(dScale, dScale, dScale));
         Rotate_y(RAD( pBoxPos->nPhi));
         Rotate_x(RAD(-pBoxPos->nTheta));
         Rotate_z(RAD( pBoxPos->nPsi));
         GetMatrix(m);
         pBox->SetMatrix(0, (double*)&m);
         pBox->SetMatrix(1, (double*)&m);
      }
      else
      {
         dScale = m_fScaleFactor;
         LoadIdentity();                                       // neue Matrix berechnen
         Translate(CVector(BOXPOS_X, 0, BOXPOS_Z));
         Scale(CVector(dScale,dScale,dScale));
         Rotate_y(RAD( pBoxPos->nPhi));
         Rotate_x(RAD(-pBoxPos->nTheta));
         Rotate_z(RAD( pBoxPos->nPsi));
         GetMatrix(m);

         for (i=0; i<pBox->GetNoOfPositions(); i++)            // neue Abmessungen berechnen
         {
            pBox->SetMatrix(i, (double*)&m);
            pBox->CalcMinMaxDimension(i, &vMin, &vMax, (i==0) ? true : false, dScale);
         }

         if (m_bShowScale) m_vScale = (vMax-vMin)/dScale;      // Boxabmessungen speichern

         LoadIdentity();                                       // Box auf die richtige Höhe verschieben
         Translate(CVector(0, -Vy(vMin), 0));
         MultMatrix(m);
         GetMatrix(m);
         for (i=0; i<pBox->GetNoOfPositions(); i++)
         {
            pBox->SetMatrix(i, (double*)&m);
         }
      }
      if (m_bShowScale) InvalidateList(LIST_SCALE);
      InvalidateList(LIST_SHADOW);
      InvalidateList(LIST_BOX);
   }
}

/******************************************************************************
* Name       : OnContextMenu                                                  *
* Definition : void OnContextMenu(int, int, HWND);                            *
* Zweck      : Verarbeitung der Nachricht WM_CONTEXTMENU                      *
* Aufruf     : OnContextMenu(nX, nY, hwnd);                                   *
* Parameter  :                                                                *
* nX, nY  (E): Koordinaten des Mauscursors.                                   *
* hwnd    (E): Fenster, daß die Nachricht gesendet hat.                       *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnContextMenu(int nX, int nY, HWND hwnd)
{
   if (m_nViewMode == BOXVIEW)
   {
      HMENU hMenu   = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_DETAIL));
      HMENU context = GetSubMenu(hMenu, 0);
      CheckMenuItem(context, IDM_VIEW_TEXTURE, MF_BYCOMMAND|((m_bTexture) ? MF_CHECKED : MF_UNCHECKED));
      if ((GetModes() & ETS3D_CM_RGB)==0) EnableMenuItem(context, IDM_VIEW_TEXTURE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
      HMENU detail  = GetSubMenu(context, 0);
      int nCmd = GetMenuItemCount(detail);
      CheckMenuRadioItem(detail, 0, nCmd, m_nDetail, MF_BYPOSITION);
      SendMessageToParent(WM_INITMENU, (WPARAM)context, 0);
      TrackPopupMenu(context, TPM_CENTERALIGN, nX, nY, 0, hwnd, NULL);
      DestroyMenu(hMenu);
   }
}


/******************************************************************************
* Name       : SetWalkCursor                                                  *
* Definition : void SetWalkCursor(bool);                                      *
* Zweck      : Setzen des Cursors abhängig vom Zustand des Programmes         *
* Aufruf     : SetWalkCursor(bCheckPos);                                      *
* Parameter  :                                                                *
* bCheckPos (E): prüfen ob der Cursor innerhalb des Clientbereiches ist.      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::SetWalkCursor(bool bCheckPos, bool bResetWaitCursor)
{
   if (bCheckPos)
   {
      RECT  rcClient;
      POINT ptCursor;
      GetWindowRect(GetHWND(), &rcClient);
      GetCursorPos(&ptCursor);
      if (!PtInRect(&rcClient, ptCursor)) return;
   }

   HCURSOR hOld, hNew;
   if      (m_bLButtonDown||m_bRButtonDown)                      hNew = NULL;
   else if (!bResetWaitCursor && IsRendering()) hNew = LoadCursor(NULL, IDC_WAIT);
   else                                         hNew = LoadCursor(NULL, IDC_ARROW);

   hOld = SetCursor(hNew);
   if (hOld != hNew) REPORT("Cursor Changed");
}

/******************************************************************************
* Name       : OnLButtonDown                                                  *
* Definition : void OnLButtonDown(POINT, UINT);                               *
* Zweck      : Verarbeitung der Nachricht WM_LBUTTONDOWN                      *
* Aufruf     : OnLButtonDown(ptMouse, nFlags);                                *
* Parameter  :                                                                *
* ptMouse (E): Position des Mauszeigers                             (POINT)   *
* nFlags  (E): Buttonflags.                                         (UINT)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnLButtonDown(POINT /*ptMouse*/, UINT /*nFlags*/)
{
   HWND hwnd       = GetHWND();
   HWND hwndParent = GetParent(hwnd);
   SetCapture(hwnd);
   CatchCursorPos();
   if (IsWindow(hwndParent)) ::SendMessage(hwndParent, WM_SETSTATUSBAR_TEXT, IDS_MOVING_MOUSE, NULL);
   m_bLButtonDown = true;
   SetWalkCursor(true);
}

/******************************************************************************
* Name       : OnRButtonDown                                                  *
* Definition : void OnRButtonDown(POINT, UINT);                               *
* Zweck      : Verarbeitung der Nachricht WM_RBUTTONDOWN                      *
* Aufruf     : OnRButtonDown(ptMouse, nFlags);                                *
* Parameter  :                                                                *
* ptMouse (E): Position des Mauszeigers                             (POINT)   *
* nFlags  (E): Buttonflags.                                         (UINT)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnRButtonDown(POINT /*ptMouse*/, UINT /*nFlags*/)
{
   HWND hwnd       = GetHWND();
   HWND hwndParent = GetParent(hwnd);
   SetCapture(hwnd);
   CatchCursorPos();
   if (IsWindow(hwndParent)) ::SendMessage(hwndParent, WM_SETSTATUSBAR_TEXT, IDS_LOOKING_MOUSE, NULL);
   m_bRButtonDown = true;
   SetWalkCursor(true);
}

/******************************************************************************
* Name       : OnMouseMove                                                    *
* Definition : void OnMouseMove(POINT, UINT);                                 *
* Zweck      : Verarbeitung der Nachricht WM_MOUSEMOVE                        *
* Aufruf     : OnMouseMove(ptMouse, nFlags);                                  *
* Parameter  :                                                                *
* ptMouse (E): Position des Mauszeigers                             (POINT)   *
* nFlags  (E): Buttonflags.                                         (UINT)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnMouseMove(POINT ptMouse, UINT nFlags)
{
   POINT ptDiff;
   HWND hwnd       = GetHWND();
 
   if (m_bSetCursor)
   {
      m_bSetCursor = false;
      return;
   }
 
   if ((nFlags & (MK_LBUTTON|MK_RBUTTON)) == 0)
      return;

   RECT   rcClient;
   GetClientRect(hwnd, &rcClient);
   POINT  ptCenter = {(rcClient.left+rcClient.right ) >> 1,
                      (rcClient.top +rcClient.bottom) >> 1}; 

   ptDiff.x = ptCenter.x - ptMouse.x;
   ptDiff.y = ptCenter.y - ptMouse.y;

   m_bSetCursor = true;
   ClientToScreen(hwnd, &ptCenter);
   SetCursorPos(ptCenter.x, ptCenter.y);

   if (nFlags & MK_LBUTTON)
   {
      if (m_nViewMode == BOXVIEW)
      {
         if (nFlags & MK_CONTROL)
         {
            if      (ptDiff.y > 0)
            {
               PressParentDlgButton(IDC_THETAUP, ptDiff.y);
            }
            else if (ptDiff.y < 0)
            {
               PressParentDlgButton(IDC_THETADOWN, -ptDiff.y);
            }
            if      (ptDiff.x > 0)
            {
               PressParentDlgButton(IDC_PHIRIGHT, ptDiff.x);
            }
            else if (ptDiff.x < 0)
            {
               PressParentDlgButton(IDC_PHILEFT, -ptDiff.x);
            }
         }
         else
         {
            int nTheta = m_nTheta,
                nPhi   = m_nPhi;
            m_nTheta += ptDiff.y * 2;
            m_nPhi   += ptDiff.x * 2;
            if (m_nTheta > -100) m_nTheta = -100;
            if (m_nTheta < -700) m_nTheta = -700;
            if (m_nPhi   < 1000) m_nPhi   = 1000;
            if (m_nPhi   > 2600) m_nPhi   = 2600;
            if ((nTheta |= m_nTheta) || (nPhi |= m_nPhi))
               SetTurnBoxLight();
         }
      }
      else if (m_nViewMode == ROOMVIEW)
      {
         if (ptDiff.x)
         {
            int maxAngle = 360*ANGLE_FACTOR;                      // Bereich 0° - 360°
            m_Lap.nPhi += ptDiff.x * 2;
            if ((ptDiff.x > 0) && (m_Lap.nPhi >= maxAngle)) m_Lap.nPhi -= maxAngle;
            if ((ptDiff.x < 0) && (m_Lap.nPhi <=        0)) m_Lap.nPhi += maxAngle;
            m_bLapInvalid = true;
         }
         if (ptDiff.y)
         {
            if (nFlags & MK_CONTROL) ptDiff.y *= 2;
            double dY = 0;
            if (m_bFlyMode) dY = sin(RAD10(m_Lap.nTheta));

            CVector vDirection(sin(RAD10(m_Lap.nPhi)), dY, cos(RAD10(m_Lap.nPhi))); // Touch-Test
            if (TestWalkStep((CVector*)&m_Lap.dEyex, &vDirection, ptDiff.y*2)) m_bLapInvalid = true;
            else PlaySound("Autsch.wav", NULL, SND_FILENAME|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
         }
      }
      else //  if (m_nViewMode == SCREENSAVER)
      {
         CBoxObj *pBox = (CBoxObj*) m_Boxes.GetFirst();
         if (pBox && (pBox->GetSelectedPolygon() != -1))
         {
            CBoxPosition *pPos = pBox->GetpPosition(0);
            pPos->m_nPsi = 0;
            int nTheta = pPos->m_nTheta,
                nPhi   = pPos->m_nPhi;
            pPos->m_nTheta  -= ptDiff.y * 2;
            pPos->m_nPhi    -= ptDiff.x * 2;
            if ((nTheta |= pPos->m_nTheta) || (nPhi |= pPos->m_nPhi))
            {
               m_bInitBoxLists = true;
               InvalidateList(LIST_BOX);
               char szText[64];
               wsprintf(szText, "%d, %d", pPos->m_nTheta, pPos->m_nPhi);
               SendMessageToParent(WM_SETSTATUSBAR_TEXT, 0, (LPARAM)szText);
            }
            return;
         }// ist eine Box vorhanden
         int nTheta = m_nTheta,
             nPhi   = m_nPhi;
         m_nTheta  += ptDiff.y * 2;
         m_nPhi    -= ptDiff.x * 2;
         if (m_nTheta >  250) m_nTheta =  250;
         if (m_nTheta < -250) m_nTheta = -250;
         if (m_nPhi   >  400) m_nPhi   =  400;
         if (m_nPhi   < -400) m_nPhi   = -400;
         if ((nTheta |= m_nTheta) || (nPhi |= m_nPhi))
         {
            SetScreenSaverLight();
            char szText[64];
            wsprintf(szText, "%d, %d", m_nTheta/10, m_nPhi/10);
            SendMessageToParent(WM_SETSTATUSBAR_TEXT, 0, (LPARAM)szText);
         }
      }
   }

   if ((nFlags & MK_RBUTTON) && (m_nViewMode == ROOMVIEW))
   {
      if (ptDiff.x)
      {
         int maxAngle = 360*ANGLE_FACTOR;                      // Bereich 0° - 360°
         m_Lap.nPhi += ptDiff.x * 2;
         if ((ptDiff.x > 0) && (m_Lap.nPhi >= maxAngle)) m_Lap.nPhi -= maxAngle;
         if ((ptDiff.x < 0) && (m_Lap.nPhi <=        0)) m_Lap.nPhi += maxAngle;
         m_bLapInvalid = true;
      }
      if (ptDiff.y)
      {
         int maxAngle = 80*ANGLE_FACTOR;                     // Bereich ± 80°
         m_Lap.nTheta += ptDiff.y;
         if ((ptDiff.y > 0) && (m_Lap.nTheta >  maxAngle)) m_Lap.nTheta =  maxAngle;
         if ((ptDiff.y < 0) && (m_Lap.nTheta < -maxAngle)) m_Lap.nTheta = -maxAngle;
         m_bLapInvalid = true;
      }
   }
}

/******************************************************************************
* Name       : OnLButtonUp                                                    *
* Definition : void OnLButtonUp(POINT, UINT);                                 *
* Zweck      : Verarbeitung der Nachricht WM_LBUTTONUP                        *
* Aufruf     : OnLButtonUp(ptMouse, nFlags);                                  *
* Parameter  :                                                                *
* ptMouse (E): Position des Mauszeigers                             (POINT)   *
* nFlags  (E): Buttonflags.                                         (UINT)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnLButtonUp(POINT /*ptMouse*/, UINT /*nFlags*/)
{
   HWND hwnd       = GetHWND();
   HWND hwndParent = GetParent(hwnd);
   if (GetCapture() == hwnd) ReleaseCapture();
   ClipCursor(NULL);
   if (IsWindow(hwndParent)) ::SendMessage(hwndParent, WM_SETSTATUSBAR_TEXT, 0, NULL);
   m_bLButtonDown = false;
   SetWalkCursor(true);
}

/******************************************************************************
* Name       : OnRButtonUp                                                    *
* Definition : void OnRButtonUp(POINT, UINT);                                 *
* Zweck      : Verarbeitung der Nachricht WM_RBUTTONUP                        *
* Aufruf     : OnRButtonUp(ptMouse, nFlags);                                  *
* Parameter  :                                                                *
* ptMouse (E): Position des Mauszeigers                             (POINT)   *
* nFlags  (E): Buttonflags.                                         (UINT)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::OnRButtonUp(POINT /*ptMouse*/, UINT /*nFlags*/)
{
   HWND hwnd       = GetHWND();
   HWND hwndParent = GetParent(hwnd);
   if (GetCapture() == hwnd) ReleaseCapture();
   ClipCursor(NULL);
   if (IsWindow(hwndParent)) ::SendMessage(hwndParent, WM_SETSTATUSBAR_TEXT, 0, NULL);
   m_bRButtonDown = false;
   SetWalkCursor(true);
}

/******************************************************************************
* Name       : OnHelp                                                         *
* Definition : bool OnHelp(HELPINFO *);                                       *
* Zweck      : Verarbeitung der Nachricht WM_HELP.                            *
* Aufruf     : OnHelp(pHI);                                                   *
* Parameter  :                                                                *
* pHI     (E): Zeiger auf HelpInfo-Struktur                       (HELPINFO*) *
* Funktionswert : Nachricht verarbeitet (true, false)                         *
******************************************************************************/
bool CCaraWalkDll::OnHelp(HELPINFO *pHI)
{
   if (pHI->iContextType == HELPINFO_WINDOW)
   {
      char text[1024];
      OnLButtonUp(pHI->MousePos,MK_LBUTTON);
      if (LoadString(g_hInstance, IDS_HELP_STRING, text, 1024))
         CEtsHelp::CreateContextWnd(text, pHI->MousePos.x, pHI->MousePos.y);
      return true;
   }
   
   return false;
}

/******************************************************************************
* Name       : OnSetCursor                                                    *
* Definition : void OnSetCursor(HWND, WORD, WORD);                            *
* Zweck      : Verarbeitung der Nachricht WM_SETCURSOR.                       *
* Aufruf     : OnSetCursor(hwndContain, nHittest, wMouseMsg);                 *
* Parameter  :                                                                *
* hwndContain (E): Fenster des Cursors                                  (HWND)*
* nHittest    (E): Info über den Ort des Cursors innerhalb des Fensters (WORD)*
* wMouseMsg   (E): Id der Mausnachricht                                 (WORD)*
* Funktionswert : keiner                                                      *
******************************************************************************/
bool CCaraWalkDll::OnSetCursor(HWND /*hwndContain*/, WORD /*nHittest*/, WORD /*wMouseMsg*/)
{
   SetWalkCursor(false);
   return true;
}

/******************************************************************************
* Name       : CatchCursorPos                                                 *
* Definition : CatchCursorPos()                                               *
* Zweck      : Fangen der Mausposition in der Mitte des Fensters              *
* Aufruf     : CatchCursorPos();                                              *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::CatchCursorPos()
{
   HWND hwnd       = GetHWND();
   RECT   rcClient;
   GetClientRect(hwnd, &rcClient);
   POINT  ptConv = {(rcClient.left+rcClient.right ) >> 1,
                    (rcClient.top +rcClient.bottom) >> 1}; 
   ClientToScreen(hwnd, &ptConv);
   m_bSetCursor = true;
   SetCursorPos(ptConv.x, ptConv.y);
   rcClient.left   = ptConv.x - 40;
   rcClient.right  = ptConv.x + 40;
   rcClient.top    = ptConv.y - 40;
   rcClient.bottom = ptConv.y + 40;
   ClipCursor(&rcClient);
}

// Commands
void CCaraWalkDll::OnViewTexture(WORD nIDCommand)
{
   { // CriticalSection
      CAutoCriticalSection acs(*this);
      m_bTexture = !m_bTexture;
      InvalidateList(LIST_ROOM);
      m_bInitBoxLists = true;
      InvalidateList(LIST_BOX);
      m_bInitShadowLists = true;
      InvalidateList(LIST_SHADOW);
   }
   Check3DControlItem(nIDCommand, m_bTexture ? CHECKED : UNCHECKED);
}

void CCaraWalkDll::OnViewLight(WORD nIDCommand)
{
   { // CriticalSection
      CAutoCriticalSection acs(*this);
      m_bLight = !m_bLight;

      for (int i=0; i<m_nNoOfLights; i++)
      {
         InvalidateList(LIST_LIGHT+i);
         InvalidateCommand(LIST_LIGHT+i);
         PerformList(LIST_LIGHT+i, m_bLight);
         PerformCommand(LIST_LIGHT+i, m_bLight);
      }

      PerformCommand(AMBIENT_LIGHT, m_bLight);

      InvalidateList(LIST_ROOM);

      m_bInitBoxLists = true;
      InvalidateList(LIST_BOX);
   }

   Check3DControlItem(nIDCommand, m_bLight ? CHECKED : UNCHECKED);
}

void CCaraWalkDll::OnViewBoxes(WORD nIDCommand)
{
   if (m_nViewMode == ROOMVIEW)
   {
      { // CriticalSection
         CAutoCriticalSection acs(*this);
         m_bDrawBoxes = !m_bDrawBoxes;
         PerformList(LIST_BOX, m_bDrawBoxes);
//       PerformList(LIST_SHADOW, m_bDrawBoxes && m_bLight); // Testweise
      }

      Check3DControlItem(nIDCommand, m_bDrawBoxes ? CHECKED : UNCHECKED);
   }
}

void CCaraWalkDll::OnViewDetail(WORD nIDCommand)
{
   int nDetail;
   int i, nCount = IDM_VIEW_DETAIL_VERYHIGH-IDM_VIEW_DETAIL_VERYLOW;
   if (nIDCommand == IDM_VIEW_DETAIL)
   {
      nDetail = m_nDetail;
      nDetail++;
      if (nDetail > nCount)
         nDetail = 0;
   }
   else
   {
      nDetail = nIDCommand - IDM_VIEW_DETAIL_VERYLOW;
   }
   if (m_bLight && (nDetail >= 0) && (nDetail < 10) && (nDetail != m_nDetail))
   {
      { // CriticalSection
         CAutoCriticalSection acs(*this);
         m_nDetail = nDetail;
         InvalidateList(LIST_ROOM);
         m_bInitBoxLists    = true;
         m_bInitShadowLists = true;
         InvalidateList(LIST_BOX);
         InvalidateList(LIST_SHADOW);
      }

      for (i=0; i<=nCount; i++)
      {
         Check3DControlItem(i+IDM_VIEW_DETAIL_VERYLOW, (nDetail == i) ? RADIOCHECKED : UNCHECKED);
      }
   }
}

void CCaraWalkDll::OnOptionsLight()
{
   CLightdlg dlg(g_hInstance, IDD_LIGHTPARAM, GetHWND());
   dlg.SetLightParam(m_pLight);
   dlg.SetAmbientColor(m_cAmbientLight);
   dlg.DoModal();
   InvalidateView();
}


void CCaraWalkDll::OnOptionsBox()
{
   if (m_Boxes.GetCounter() > 0)
   {
      if (m_nViewMode == ROOMVIEW)
      {
         const ETS3DGL_LookAt *pLAP = GetLookAt();
         CBoxPositionDlg dlg(g_hInstance, IDD_BOX_POSITION, GetHWND());
         CBoxObj *pBox;
         for (pBox=(CBoxObj*)m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) m_Boxes.GetNext())
         {
            dlg.SetpBox(pBox);
         }

         CVector vDirection(sin(RAD10(pLAP->nPhi  )),
                            sin(RAD10(pLAP->nTheta)),
                            cos(RAD10(pLAP->nPhi  )) * cos(RAD10(pLAP->nTheta)));
         CVector *pEye  = (CVector*)&pLAP->dEyex;
         dlg.CatchBox(pEye, &vDirection);
         dlg.DoModal();
         InvalidateView();
      }
      else if (m_nViewMode == SCREENSAVER)
      {
         if (m_pBoxProp == NULL)
         {
            m_pBoxProp = new CBoxPropertiesDlg(g_hInstance, IDD_BOX_PROPERTIES, GetHWND());
            m_pBoxProp->m_pCaraWalk = this;
         }
         if (m_pBoxProp)
         {
            CBoxObj *pBox;
            m_pBoxProp->SetpBox(NULL);
            for (pBox=(CBoxObj*)m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) m_Boxes.GetNext())
            {
               m_pBoxProp->SetpBox(pBox);
            }
            m_pBoxProp->Create();
            InvalidateView();
         }
      }
   }
}

void CCaraWalkDll::OnEditCopyGrafic()
{
   HWND hwnd       = GetHWND();
   BEGIN("OnEditCopyGrafic");
//   const SIZE szView = GetViewSize();
//   RECT       rcView = {0, 0, szView.cx, szView.cy};
   REPORT("From Framebuffer");
   HGLOBAL hGl = NULL;
   ::SendMessage(hwnd, WM_COPY_FRAME_BUFFER, COPY_TO_HGLOBAL, (LPARAM)&hGl);
   if (hGl && OpenClipboard(hwnd) && EmptyClipboard())
   {
      if (SetClipboardData(CF_DIB, hGl) == NULL)
         GlobalFree(hGl);
      CloseClipboard();
   }
}

void CCaraWalkDll::OnChangeLight(WORD nNotifyCode, HWND hwndFrom)
{
   if (nNotifyCode == 0xffff)                         // Umgebungslicht
   {
      m_cAmbientLight = (COLORREF)hwndFrom;
      InvalidateCommand(AMBIENT_LIGHT);
   }
   else
   {
      if (nNotifyCode == 0)
         PerformList(LIST_SHADOW, m_bLight && m_pLight[0].m_bEnabled);
      if (/*(nNotifyCode>=0) && */(nNotifyCode<m_nNoOfLights))
      {
         PerformList(LIST_LIGHT+nNotifyCode, m_bLight && m_pLight[nNotifyCode].m_bEnabled);
         InvalidateCommand(LIST_LIGHT+nNotifyCode);
         m_bLapInvalid = true;
      }
   }
}

void CCaraWalkDll::OnWalk(WORD nIDCommand, WORD nNotifyCode, HWND /*hwndFrom*/)
{
   int nStepCount = 0;
   switch (nIDCommand)                                         // Laufen
   {
      case IDM_WALK_BACKWARD_CTRL:  nStepCount = -10; break;
      case IDM_WALK_BACKWARD:       nStepCount =  -5; break;
      case IDM_WALK_FORWARD_CTRL:   nStepCount =  10; break;
      case IDM_WALK_FORWARD:        nStepCount =   5; break;
   }
   if (nStepCount)
   {
      if (nNotifyCode > 1) nStepCount *= nNotifyCode;
      double dY = 0;
      if (m_bFlyMode) dY = sin(RAD10(m_Lap.nTheta));
      CVector vDirection(sin(RAD10(m_Lap.nPhi)), dY, cos(RAD10(m_Lap.nPhi)));// Touch-Test
      if (TestWalkStep((CVector*)&m_Lap.dEyex, &vDirection, nStepCount)) m_bLapInvalid = true;
      else
      {
         PlaySound("Autsch.wav", NULL, SND_FILENAME|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
         return;
      }
   }
   else
   {
      switch (nIDCommand)                                      // Drehen
      {
         case IDM_WALK_TURNRIGHT_CTRL: nStepCount = -10; break;
         case IDM_WALK_TURNRIGHT:      nStepCount =  -5; break;
         case IDM_WALK_TURNLEFT_CTRL:  nStepCount =  10; break;
         case IDM_WALK_TURNLEFT:       nStepCount =   5; break;
      }
      if (nStepCount)
      {
         if (nNotifyCode > 1) nStepCount *= nNotifyCode;
         m_Lap.nPhi += nStepCount;
         if ((nStepCount > 0) && (m_Lap.nPhi >= MAX_ANGLE_PHI)) m_Lap.nPhi -= MAX_ANGLE_PHI;
         if ((nStepCount < 0) && (m_Lap.nPhi <=             0)) m_Lap.nPhi += MAX_ANGLE_PHI;
         m_bLapInvalid = true;
      }
      else
      {
         switch (nIDCommand)                                   // Nach oben oder unten
         {                                                     // gucken
            case IDM_WALK_LOOKUP_CTRL:   nStepCount =  10; break;
            case IDM_WALK_LOOKUP:        nStepCount =   5; break;
            case IDM_WALK_LOOKDOWN_CTRL: nStepCount = -10; break;
            case IDM_WALK_LOOKDOWN:      nStepCount =  -5; break;
         }
         if (nStepCount)
         {
            if (nNotifyCode > 1) nStepCount *= nNotifyCode;
            m_Lap.nTheta += nStepCount;
            if ((nStepCount > 0) && (m_Lap.nTheta >  MAX_ANGLE_THETA)) m_Lap.nTheta =  MAX_ANGLE_THETA;
            if ((nStepCount < 0) && (m_Lap.nTheta < -MAX_ANGLE_THETA)) m_Lap.nTheta = -MAX_ANGLE_THETA;
            m_bLapInvalid = true;
         }
         else
         {
            switch (nIDCommand)                                // Kippen
            {
               case IDM_WALK_GEAR_RIGHT_CTRL: nStepCount = -10; break;
               case IDM_WALK_GEAR_RIGHT:      nStepCount =  -5; break;
               case IDM_WALK_GEAR_LEFT_CTRL:  nStepCount =  10; break;
               case IDM_WALK_GEAR_LEFT:       nStepCount =   5; break;
            }
            if (nStepCount)
            {
               m_Lap.nXi += nStepCount;
               if ((nStepCount > 0) && (m_Lap.nXi >= MAX_ANGLE_XI)) m_Lap.nXi -= MAX_ANGLE_XI;
               if ((nStepCount < 0) && (m_Lap.nXi <= MAX_ANGLE_XI)) m_Lap.nXi += MAX_ANGLE_XI;
               m_bLapInvalid = true;
            }
            else return;
         }
      }
   }   
}


/******************************************************************************
* Name       : SetEmilsLight                                                  *
* Definition : SetEmilsLight();                                               *
* Zweck      : Berechnet die Licht-Parameter für Emil mit der Taschenlampe    *
* Aufruf     : SetEmilsLight();                                               *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::SetEmilsLight()
{
   BEGIN("SetEmilsLight");
   if (m_pLight)
   {
      CVector vDir(sin(RAD10(m_Lap.nPhi)), sin(RAD10(m_Lap.nTheta)), cos(RAD10(m_Lap.nPhi)));
      vDir = Norm(vDir);
      CAutoCriticalSection acs(*this);
      m_pLight[0].SetDirection(vDir);
      if (m_pLight[0].m_pfPosition[3] == 0.0)                  // Lichquelle als Paralleles Licht :
      {                                                        // Die Parameter entsprechen dann einer Richtung
         m_pLight[0].SetPosition(-vDir, 0);
      }
      else                                                     // Lichtquelle als Strahler :
      {                                                        // Die Parameter entsprechen dann einer Richtung
                                                               // und einer Position
         m_pLight[0].SetPosition((*((CVector*)&m_Lap.dEyex)) - vDir, 1.0); 
      }
      InvalidateList(LIST_LIGHT);
      InvalidateCommand(LIST_LIGHT);
   }
}

/******************************************************************************
* Name       : SetTurnBoxLight                                                *
* Definition : SetTurnBoxLight();                                             *
* Zweck      : Berechnet die Lichtparameter für die Beleuchtung der Box       *
* Aufruf     : SetTurnBoxLight();                                             *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::SetTurnBoxLight()
{
   BEGIN("SetTurnBoxLight");
   if (m_pLight)
   {
      double sinTheta = sin(RAD10(m_nTheta));
      CVector vDir(sin(RAD10(m_nPhi))*(1-fabs(sinTheta)), sinTheta, cos(RAD10(m_nPhi))*(1-fabs(sinTheta)));
      CVector vCenter(BOXPOS_X, BOXPOS_Y, BOXPOS_Z);
      vDir = Norm(vDir);

      { // CriticalSection
         CAutoCriticalSection acs(*this);
         m_pLight[0].SetDirection(vDir);
         if (m_pLight[0].m_pfPosition[3] == 0.0)               // Lichquelle als Paralleles Licht :
         {                                                     // Die Parameter entsprechen dann einer Richtung
            m_pLight[0].SetPosition(-vDir, 0);
         }
         else                                                  // Lichtquelle als Strahler :
         {                                                     // Die Parameter entsprechen dann einer Richtung
            CVector vPos = vCenter - vDir * LIGHT_DISTANCE;    // und einer Position
            m_pLight[0].SetPosition(vPos, m_pLight[0].m_pfPosition[3]);
         }
      }

      if (m_pRoom && m_pRoom->GetNoOfWalls())
      {
         if (m_bLight && m_pLight[0].m_bEnabled)               // nur bei Licht
         {
            CAutoCriticalSection acs(*this);
            m_pRoom->CalculateShadowMatrixes(&m_pLight[0]);    // Schattenlisten neu erzeugen
            InvalidateList(LIST_SHADOW);
         }
      }
      InvalidateList(LIST_LIGHT);
   }
}

void CCaraWalkDll::SetScreenSaverLight()
{
   BEGIN("SetScreenSaverLight");
   if (m_pLight)
   {
      double sinTheta = sin(RAD10(m_nTheta));
      CVector vDir;
      CVector vPos(0,0,0);                                    // und einer Position
      vDir.SetX() = sin(RAD10(m_nPhi))*(1-fabs(sinTheta));
      vDir.SetY() = sinTheta;
      if (GetModes() & ETS3D_PR_LOOK_AT)
      {
         vDir.SetZ() =   cos(RAD10(m_nPhi))*(1-fabs(sinTheta));
         vPos.SetZ() = -10;
      }
      else
      {
         vDir.SetZ() =  -cos(RAD10(m_nPhi))*(1-fabs(sinTheta));
         vPos.SetZ() =  10;
      }

      vDir = Norm(vDir);
  
      { // CriticalSection
         CAutoCriticalSection acs(*this);

         m_pLight[0].SetDirection(vDir);
         if (m_pLight[0].m_pfPosition[3] == 0.0)               // Lichquelle als Paralleles Licht :
         {                                                     // Die Parameter entsprechen dann einer Richtung
            m_pLight[0].SetPosition(-vDir, 0);
         }
         else                                                  // Lichtquelle als Strahler :
         {                                                     // Die Parameter entsprechen dann einer Richtung
            m_pLight[0].SetPosition(vPos, m_pLight[0].m_pfPosition[3]);
         }
      }

      InvalidateList(LIST_LIGHT);
   }
}


/******************************************************************************
* Name       : TestWalkStep                                                   *
* Definition : bool TestWalkStep(CVector *, CVector *, int);                  *
* Zweck      : Überprüft, ob und wie weit man in die Richtung gehen kann.     *
* Aufruf     : TestWalkStep(pvEye, pvDir, nStepCount);                        *
* Parameter  :                                                                *
* pvEye     (E): Ausgangsposition                                  (CVector*) *
* pvDir     (E): Richtungsvektor                                   (CVector*) *
* nStepCount(E): Anzahl der Schritte in dieser Richtung              (int)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
bool CCaraWalkDll::TestWalkStep(CVector *pvEye, CVector *pvDir, int nStepCount)
{
   double   dY    = 0;
   CVector  pvTest[2];
   pvTest[0] = (*pvEye) + (*pvDir) * (WALK_STEP * (double)nStepCount);
   pvTest[1] = (*pvEye);

   if (m_bGhostMode)                                  // kein Test
   {
      (*pvEye) = pvTest[0];
      return true;
   }

   if (m_bFlyMode) dY = pvTest[1].SetY() = Vy(pvTest[0]);
   if (SendMessageToParent(WM_CHECKPOS, WALKER_POSITION, (LPARAM)pvTest))
   {
      if (m_bFlyMode) ((double*)&pvTest[0])[1] = dY;
      (*pvEye) = pvTest[0];
      return true;
   }
   return false;
}


/******************************************************************************
* Name       : GetBmpFile                                                     *
* Definition : static void GetBmpFile(char *, BITMAPINFO **);                 *
* Zweck      : Laden einer Bitmapdatei.                                       *
* Aufruf     : GetBmpFile(szBmp, ppBmi);                                      *
* Parameter  :                                                                *
* szBmp   (E): Name der Bitmap                                 (char*)        *
* ppBmi   (E): Zeiger auf BITMAPINFO Strukturzeiger            (BITMAPINFO**) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::GetBmpFile(char *szBmp, BITMAPINFO **ppBmi)
{
   HANDLE hFile = CreateFile(szBmp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      CDibSection Dib;
      if (Dib.LoadFromFile(hFile))
      {
         BITMAPINFO *pBmi    = (BITMAPINFO*) Dib.GetBitmapInfo();
         BITMAPINFO *pNewBmi = NULL;
         int nWidth , nHeight, i, j, k = 0;
     
         nWidth = Dib.GetWidth();
         for (i=-1; nWidth!=0; nWidth>>=1, i++);
         nWidth  = 1 << i;

         nHeight = Dib.GetHeight();
         for (i=-1; nHeight!=0; nHeight>>=1, i++);
         nHeight = 1 << i;

         pNewBmi = (BITMAPINFO*) Alloc(sizeof(BITMAPINFO) + nWidth*nHeight*sizeof(COLORREF));
         if (pNewBmi)
         {
            memcpy(pNewBmi, pBmi, sizeof(BITMAPINFO));
            COLORREF *pColors = (COLORREF*) &pNewBmi->bmiColors[0];

            for (i=0; i<nHeight; i++)
               for (j=0; j<nWidth; j++)
                  pColors[k++] = Dib.GetPixel(j, i);

            pNewBmi->bmiHeader.biWidth  = nWidth;
            pNewBmi->bmiHeader.biHeight = nHeight;
            *ppBmi = pNewBmi;
         }
      }
      CloseHandle(hFile);
   }
}




/******************************************************************************
* Name       : GetHeapHandle                                                  *
* Definition : static HANDLE GetHeapHandle();                                 *
* Zweck      : liefert Zugriff auf den Heaphandle bzw. erzeugt den Heap.      *
* Aufruf     : GetHeapHandle();                                               *
* Parameter  :                                                                *
* Funktionswert : Heap-Handle                                                 *
******************************************************************************/
HANDLE CCaraWalkDll::GetHeapHandle()
{
   ASSERT(g_hHeap != NULL);

   return g_hHeap;                              // Ist er NULL, so ist er NULL !
}

/******************************************************************************
* Name       : Alloc                                                          *
* Definition : static void * Alloc(DWORD);                                    *
* Zweck      : Allozieren eines Speicherbereiches                             *
* Aufruf     : Alloc(nSize);                                                  *
* Parameter  :                                                                *
* nSize   (E): Größe des Speicherbereiches in BYTE                   (DWORD)  *
* Funktionswert : Zeiger auf Speicherbereich                         (void*)  *                                                     *
******************************************************************************/
void * CCaraWalkDll::Alloc(DWORD nSize)
{
   if (nSize <= 0) return NULL;

   ASSERT(g_hHeap != NULL);

//   if (g_hHeap == NULL) throw (DWORD)CARAWALK_ALLOCATION_ERROR;

   void *ptr = HeapAlloc(g_hHeap, 0, nSize);

   if (ptr) InterlockedIncrement((long*)&gm_nHeapInstances);
   else     throw (DWORD)CARAWALK_ALLOCATION_ERROR;

   return ptr;
}

/******************************************************************************
* Name       : Free                                                           *
* Definition : static void Free(void *);                                      *
* Zweck      : Freigabe des Speicherbereiches                                 *
* Aufruf     : void Free(p);                                                  *
* Parameter  :                                                                *
* p      (E) : Zeiger auf den Speicherbereich                        (void*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::Free(void *p)
{
   if (p)                                        // keine NULL-Pointer verwenden !
   {
      ASSERT(g_hHeap != NULL);

      if( InterlockedDecrement((long*)&gm_nHeapInstances) < 0)
      {
#ifdef _DEBUG
         ASSERT(false);
#else
         MessageBoxC(IDS_ERROR_HEAP, IDS_ERROR_MSG_BOX, MB_OK);
#endif
      }

//      if(g_hHeap) // nicht nötig, da DLL nicht geladen wenn kein g_hHeap !
//      {
         if (!HeapFree(g_hHeap, 0, p)) 
            throw (DWORD)CARAWALK_DESTRUCTION_ERROR;
//      }else throw (DWORD)CARAWALK_DESTRUCTION_ERROR;
   }
}

/******************************************************************************
* Name       : IncrementHeapInstances                                         *
* Definition : static void IncrementHeapInstances();                          *
* Zweck      : erhöhen des Instanzzählers, wenn Ausserhalb der Funktion Alloc *
*              Speicher angefordert wird.                                     *
* Aufruf     : IncrementHeapInstances();                                      *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::IncrementHeapInstances()
{
   InterlockedIncrement((long*)&gm_nHeapInstances);
}

void CCaraWalkDll::HeapDestroy()
{
//   if (g_hHeap)
//   {
      if (gm_nHeapInstances != 0)
      {
         char str[256];
         wsprintf(str, "Heap-Error %d", gm_nHeapInstances);
         MessageBoxC((long)str, IDS_ERROR_MSG_BOX, MB_OK|MBU_TEXTSTRING);
         SetDllError(CARAWALK_HEAP_ERROR);
      }
      ::HeapDestroy(g_hHeap);
      g_hHeap = NULL;
//   }
}

/******************************************************************************
* Name       : Check3DControlItem                                             *
* Definition : void Check3DControlItem(UINT, UINT);                           *
* Zweck      : Setzen der Zustände der Toolbarbuttons.                        *
* Aufruf     : Check3DControlItem(nIDCommand, nCommand);                      *
* Parameter  :                                                                *
* nIDCommand (E): ID des Toolbar-Buttons                             (UINT)   *
* nCommand   (E): Zustand der gesetzt werden soll                    (UINT)   *
*               (UNCHECKED, CHECKED, RADIOCHECKED, DISABLE_ITEM, ENABLE_ITEM) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWalkDll::Check3DControlItem(UINT nIDCommand, UINT nCommand)
{
   PostMessageToParent(WM_3DCHECK, nIDCommand, nCommand);
}

void CCaraWalkDll::ReadFileFH(HANDLE hFile, void *pData, DWORD dwSize, CFileHeader *pFH)
{
   DWORD dwBytesRead = 0;
   if (!ReadFile(hFile, pData, dwSize, &dwBytesRead, NULL)) throw (DWORD)READ_FILE_ERROR;
   if (dwBytesRead != dwSize)                               throw (DWORD)READ_FILE_ERROR;
   if (pFH) pFH->CalcChecksum(pData, dwSize);
}

void CCaraWalkDll::WriteFileFH(HANDLE hFile, void *pData, DWORD dwSize, CFileHeader *pFH)
{
   DWORD dwBytesRead = 0;
   if (!WriteFile(hFile, pData, dwSize, &dwBytesRead, NULL)) throw (DWORD)WRITE_FILE_ERROR;
   if (dwBytesRead != dwSize)                                throw (DWORD)WRITE_FILE_ERROR;
   if (pFH) pFH->CalcChecksum(pData, dwSize);
}

bool CCaraWalkDll::IsTriangleNull(CVector &p1, CVector &p2, CVector &p3)
{
   return (Quadbetrag(Product(p2-p1, p3-p1)) <= 1e-12) ? true : false;
}

void CCaraWalkDll::PreCommands(CCaraWalkDll *pC, int /*nID*/, int /*nRange*/)
{
   if (pC)
   {
      if (pC->m_nTexFlags & TEX_MAG_LIN)
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      else
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      if (pC->m_nTexFlags & TEX_MIN_LIN)
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      else
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   }
}

/******************************************************************************
* Name       : GetBoxMaterial                                                 *
* Definition : GetBoxMaterial(char *);                                        *
* Zweck      : Laden des Boxenmaterials.                                      *
* Aufruf     : GetBoxMaterial(pszBoxName);                                    *
* Parameter  :                                                                *
* pszBoxName (E): Name der Box                                    (char*)     *
* Funktionswert : Zeiger auf Material                             (CMaterial*)*
******************************************************************************/
CMaterial* CCaraWalkDll::GetBoxMaterial(int nMat)
{
   if (GetRoomObj()) return m_pRoom->GetMaterial(nMat);
   return NULL;
}

CMaterial * CCaraWalkDll::GetMaterial(char *pszFile, bool bNew)
{
   if (GetRoomObj()) return m_pRoom->GetMaterial(pszFile, bNew);
   return NULL;
}

CMaterial* CCaraWalkDll::GetMaterialFromIndex(int& nIndex, CMaterial*pM)
{
   if (GetRoomObj()) return m_pRoom->GetMaterialFromIndex(nIndex, pM);
   return NULL;
}

bool CCaraWalkDll::GetRoomObj()
{
   if (m_pRoom  == NULL)                                 // Raumobjekt nicht vorhanden
   {
      m_pRoom = new CRoomObj;                           // erstellen
   }
   if (m_pRoom) return true;
   return false;
}

/******************************************************************************
* Name       : GetBoxDB                                                       *
* Definition : GetBoxDB(int &);                                               *
* Zweck      : Laden der Boxendatenbank.                                      *
* Aufruf     : GetBoxMaterial(nBoxes);                                        *
* Parameter  :                                                                *
* nBoxes  (A): Anzahl der Boxen                                   (int&)      *
* Funktionswert : Zeiger auf Boxendaten                        (BoxDataRed*)  *
******************************************************************************/
BoxDataRed* CCaraWalkDll::GetBoxDB(int &nBoxes)
{
   BoxDataRed *pBoxData = NULL;
   int      i, nType, nVersion, nLength;
   WORD     nFiles;
   char     szFilePath[_MAX_PATH], *ptr;
   HANDLE   hFile = INVALID_HANDLE_VALUE;

   SetBoxPath(szFilePath, "LsBoxDb");              // Boxendaten einlesen
   ptr = strstr(szFilePath, BOX_DATA_FILE_EXT);
   try
   {
      if (ptr== NULL) throw (DWORD)1;
      strcpy(ptr, ".cfg");
      hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      if (hFile == INVALID_HANDLE_VALUE) throw (DWORD)2;
      CCaraWalkDll::ReadFileFH(hFile, &nType, sizeof(int));
      CCaraWalkDll::ReadFileFH(hFile, &nVersion, sizeof(int));
      if (nType != CARA_TYPE) throw (DWORD)3;
      if (nVersion != 110)    throw (DWORD)4;

      CCaraWalkDll::ReadFileFH(hFile, &nBoxes, sizeof(int));
      if (nBoxes > 0) pBoxData = new BoxDataRed[nBoxes];
      if (!pBoxData) throw (DWORD)5;
      for (i=0; i<nBoxes; i++)
      {
         ZeroMemory(&pBoxData[i], sizeof(BoxDataRed));

         CCaraWalkDll::ReadFileFH(hFile, &nLength, sizeof(long));
         CCaraWalkDll::ReadFileFH(hFile, pBoxData[i].szName, nLength);

         CCaraWalkDll::ReadFileFH(hFile, &nLength, sizeof(long));
         ::SetFilePointer(hFile, nLength, NULL, FILE_CURRENT);

         ::SetFilePointer(hFile, sizeof(WORD), NULL, FILE_CURRENT);
         CCaraWalkDll::ReadFileFH(hFile, &nFiles, sizeof(WORD));
         CCaraWalkDll::ReadFileFH(hFile, &pBoxData[i].nMaterial, sizeof(WORD));

         if (nFiles>0)
         {
            nLength = nFiles*BDD_NAME_LENGTH*sizeof(char);
            ::SetFilePointer(hFile, nLength, NULL, FILE_CURRENT);
         }
      }
   }
   catch (DWORD dwError)
   {
      REPORT("GetBoxDB-Error %d, %x", dwError, GetLastError());
      if (pBoxData)
      {
         delete[] pBoxData;
         pBoxData = NULL;
         nBoxes   = 0;
      }
   }
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);

   return pBoxData;
}

/******************************************************************************
* Name       : GetBoxMaterialNo                                               *
* Definition : GetBoxMaterialNo(int, BoxDataRed, char*);                         *
* Zweck      : Sucht die Materialnummer der Box aus der Boxendatenbank        *
* Aufruf     : GetBoxMaterial(nBoxes, pBD, szBoxName);                        *
* Parameter  :                                                                *
* nBoxes    (E): Anzahl der Boxen                                 (int)       *
* pBD       (E): Zeiger auf Boxendaten                            (BoxDataRed*)  *
* szBoxName (E): Name der Box                                     (char*)     *
* Funktionswert : Materialnummer                                  (int)       *
******************************************************************************/
int CCaraWalkDll::GetBoxMaterialNo(int nBoxes, BoxDataRed *pBD, char *szBoxName)
{
   int i;
   char *ptr = strstr(szBoxName, BOX_DATA_FILE_EXT);
   if (ptr) ptr[0] = 0;
   for (i=0; i<nBoxes; i++)
   {
      if (strcmp(pBD[i].szName, szBoxName)==0)
      {
         return pBD[i].nMaterial;
      }
   }
   return 0;
}

void CCaraWalkDll::FreeBoxName()
{
   if (m_pszNewBox)
   {
      free(m_pszNewBox);
      m_pszNewBox    = NULL;
      m_NewBoxwParam = 0;
   }
}

void CCaraWalkDll::InitParentMsgWnd(HWND hWnd)
{
   if (m_nViewMode == BOXVIEW)
   {
      if (!hWnd) hWnd = GetHWND();
      m_hParentMsgWnd = hWnd;
      while (m_hParentMsgWnd)
      {
         m_hParentMsgWnd = ::GetParent(m_hParentMsgWnd);
         long lStyle = ::GetWindowLong(m_hParentMsgWnd, GWL_STYLE);
         if (lStyle&WS_DLGFRAME)
            return;
      }
   }
   else
   {
      if (!hWnd)
      {
         hWnd = GetHWND();
         m_hParentMsgWnd = ::GetParent(hWnd);
      }
      m_hParentMsgWnd = hWnd;
      if (!::IsWindow(m_hParentMsgWnd))
      {
         m_hParentMsgWnd = NULL;
      }
   }
}

void CCaraWalkDll::UpdateBoxLists()
{
   CAutoCriticalSection acs(*this);
   PerformList(LIST_BOX   , false);
   PerformList(LIST_SHADOW, false);
   PerformList(LIST_SCALE , false);
   InvalidateList(LIST_BOX);
   InvalidateList(LIST_SHADOW);
   InvalidateList(LIST_SCALE);
   m_bInitBoxLists    = true;
   m_bInitShadowLists = true;
}


LRESULT CCaraWalkDll::SendMessageToParent(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (!m_hParentMsgWnd) InitParentMsgWnd();
   ASSERT(IsWindow(m_hParentMsgWnd));
   return ::SendMessage(m_hParentMsgWnd, nMsg, wParam, lParam);
}

LRESULT CCaraWalkDll::PostMessageToParent(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (!m_hParentMsgWnd) InitParentMsgWnd();
   ASSERT(IsWindow(m_hParentMsgWnd));
   return ::PostMessage(m_hParentMsgWnd, nMsg, wParam, lParam);
}

HWND CCaraWalkDll::GetParentDlgItem(int nID)
{
   if (!m_hParentMsgWnd) InitParentMsgWnd();
   ASSERT(IsWindow(m_hParentMsgWnd));
   return ::GetDlgItem(m_hParentMsgWnd, nID);
}

void CCaraWalkDll::PressParentDlgButton(int nID, int nTimes)
{
   int  i;
   HWND hwnd = GetParentDlgItem(nID);
   LONG lState = ::SendMessage(hwnd, BM_GETSTATE, 0, 0);
   ::SendMessage(hwnd, BM_SETSTATE, lState|BST_PUSHED, 0);
   for (i=0; i<nTimes; i++)
      SendMessageToParent(WM_TIMER, 0, 0);
   ::SendMessage(hwnd, BM_SETSTATE, lState, 0);
}

int CCaraWalkDll::MessageBoxC(long lT, long lH, UINT uP)
{
   CEtsDialog dlg(g_hInstance, 0, 0);
   return dlg.MessageBox(lT, lH, uP);
}
