/********************************************************************/
/*                                                                  */
/*                     Library ETS3DGL                              */
/*                                                                  */
/*        Verwaltungsklasse für 3D-Darstellung mit OpenGL           */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARA3D.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//
#include "stdafx.h"

#include "Ets3dGL.h"
#include "DibSection.h"
#include "CVector.h"

bool     g_bDoReport = false;
#define  ETS_DEBUGNAME "ETS3DGL"
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"

//typedef CVector Vector;

extern "C" 
{
   void  Destructor(       CEts3DGLDll*);    // immer Ordinalzahl 1
   HWND  DLL_Create(       CEts3DGLDll*, LPCTSTR, DWORD, RECT *, HWND);
   void  OnRenderSzene(    CEts3DGLDll*);
   bool  CreateRenderList( CEts3DGLDll*, UINT, void (*pfn)(CEts3DGLDll *, int, int), int, bool);
   bool  SetList(          CEts3DGLDll*, UINT, UINT);
   void  TransformPoints(  CEts3DGLDll*, CVector *, int, bool);
   void  Draw(DWORD, float);
   void  DrawTriangleFan(CVector*, CVector*, CVector*, ETS3DGL_Fan*);
   void  SetFirstInstance(CEts3DGLDll*);
   void  Destroy(CEts3DGLDll*);
}

struct VectorPointer
{
   CVector  ppv[MAX_TRIANGLES][3];        // gespeicherte Vektoren
   int      nCount;                       // Anzahl der gespeicherten Dreiecke
   int      nEbene;                       // Rekursionsebene 
   double   dStep;                        // Schrittweite
   void   (*pFn)(int, CVector *, void *); // Zeiger auf Funktion für die erzeugten Punkte
   void    *pParam;                       // Zeiger auf Parameter [optional] der Funktion
};


/********************************************************************/
/* Windowfunktionen für das Cara3D-Fenster                          */
/********************************************************************/
HWND  CaraCreate(CEts3DGLDll *, LPCTSTR, LPCTSTR, DWORD, RECT *, HWND);

/********************************************************************/
/* OpenGL Funktionen für die Cara3D-Klasse                           */
/********************************************************************/
void __fastcall glcSphereTriangle(CVector*, CVector*, CVector*, int, float, DWORD);
void __fastcall ExtractFan(VectorPointer&);
int  __fastcall FindFan(VectorPointer&, int&);
void __fastcall glcTriangleFan(CVector*, CVector*, CVector*, VectorPointer&);
void DrawSphere(DWORD, float);
void DrawCube(  DWORD, float);
void DrawFan(int, CVector *, void *);

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD  dwReason, LPVOID lpReserved)
{
   BOOL bReturn = false;
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:                                 // Einbinden der DLL in den Adressraum des Prozesses
      {
         DisableThreadLibraryCalls(hInstance);
         CEts3DGLDll::gm_hInstance = hInstance;                // Instance-Handle global speichern
         bReturn = true;
      } break;
      case DLL_PROCESS_DETACH:                                 // Dll-Resourcen wieder freigeben
      {
         CEts3DGLDll::UnregisterClass();
      }
   }
   return bReturn;
}
void InitRegistryValues()
{
   CEts3DGLDll::gm_bInitRegistryValues = false;
   HKEY hKey;
   char szRegKey[_MAX_PATH];                             // Debug report abfragen
   wsprintf(szRegKey, "%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY);
   if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;
      if (RegQueryValueEx(hKey, REPORT_KEY, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if ((dwType== REG_DWORD)&&(dwValue!=0))
         {
            g_bDoReport = true;
            ::DeleteFile("Ets3DGL.dbg");
         }
      }
      RegCloseKey(hKey);
   }
   CEts3DGLDll::InitRegistryKeys();
}
/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void  SetFirstInstance(CEts3DGLDll*pFirst)
{
   if (CEts3DGLDll::gm_bInitRegistryValues) InitRegistryValues();
   if (CEts3DGLDll::gm_pFirstObjectInstance == NULL)
      CEts3DGLDll::gm_pFirstObjectInstance = pFirst;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void Destructor(CEts3DGLDll*p)
{
   if (CEts3DGLDll::gm_pFirstObjectInstance)
   {
      CEts3DGLDll::gm_pFirstObjectInstance->KillRenderThread();
	  CEts3DGLDll::gm_pFirstObjectInstance = NULL;
   }
   ETSDEBUGEND;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void  Destroy(CEts3DGLDll*pThis)
{
   HWND hwndParent = pThis->GetParent();
//   pThis->OnDestroy();         // muß vielleicht immer aufgerufen werden
   if (hwndParent)
   {
      pThis->OnDestroy();
      ::SendMessage(hwndParent, WM_REMOVE_PROPERTY, (WPARAM)pThis, 0);
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND DLL_Create(CEts3DGLDll *pCEts3DGL, LPCTSTR lpWindowName, DWORD dwStyle, RECT *prect, HWND hwndParent)
{
   if (CEts3DGLDll::gm_bInitRegistryValues) InitRegistryValues();

   ASSERT(pCEts3DGL!=NULL);
   ASSERT(::IsWindow(hwndParent));
   HWND hwndReturn = pCEts3DGL->Create(lpWindowName, dwStyle, prect, hwndParent);
   return hwndReturn;
}

/******************************************************************************
* Name       : Draw                                                           *
* Zweck      : Zeichnen von 3D-Objekten (Würfel, Kugel)                       *
* Aufruf     : Draw(mode, fRadius);                                           *
* Parameter  : keine                                                          *
* mode    (E): Flags für die Objekte                                 (DWORD)  *
* fRadius (E): Größe des Objektes                                    (float)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void Draw(DWORD mode, float fRadius)
{
   if (mode&ETS3D_SPHERE)
   {
      DrawSphere(mode, fRadius);
   }
   else if (mode&ETS3D_CUBE)
   {
      DrawCube(  mode, fRadius);
   }
}

/******************************************************************************
* Name       : DrawSphere                                                     *
* Zweck      : Zeichnen einer Kugel aus Dreiecken, mit rekursiver Aufteilung  *
*              bis zu einer vorgebbaren Rekursionstiefe.                      *
* Aufruf     : DrawSphere(mode, fRadius);                                     *
* Parameter  :                                                                *
* mode    (E): LOWORD : Rekursionstiefe                              (DWORD)  *
*              HIWORD : Modi: Drahtmodell, Oberfläche                         *
* fRadius (E): Radius der Kugel                                      (float)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void DrawSphere(DWORD mode, float fRadius)
{
#define X .525731112119133606
#define Z .850650808352039932

   int    i, nDepth = mode & 0xffff;
   static CVector vdata[12] = {CVector(-X,0.0,Z),CVector(X,0.0,Z ),CVector(-X,0.0,-Z),CVector(X,0.0,-Z ),
		                         CVector(0.0,Z,X ),CVector(0.0,Z,-X),CVector(0.0,-Z,X ),CVector(0.0,-Z,-X),
                               CVector(Z,X,0.0 ),CVector(-Z,X,0.0),CVector(Z,-X,0.0 ),CVector(-Z,-X,0.0)};

   static GLint tindices[20][3] = {{0,4,1 },{0,9,4 },{9,5,4 },{4,5,8 },{4,8,1 },
                                   {8,10,1},{8,3,10},{5,3,8 },{5,2,3 },{2,7,3 },
                                   {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6 },
                                   {6,1,10},{9,0,11},{9,11,2},{9,2,5 },{7,2,11}};

   if (mode & ETS3D_SOLID_FRAME)
      glBegin(GL_TRIANGLES);
   for (i=0;i<20;i++)
   {
      glcSphereTriangle(&vdata[tindices[i][0]], &vdata[tindices[i][1]], &vdata[tindices[i][2]], nDepth, fRadius, mode);
   }
   if (mode & ETS3D_SOLID_FRAME)
      glEnd();

#undef X
#undef Z
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void __fastcall glcSphereTriangle(CVector *v1, CVector *v2, CVector *v3, int depth, float fRadius, DWORD mode)
{
   CVector v12,v23,v31;
 
   if (depth==0)
   {
      CVector vr;
      if (mode & ETS3D_WIRE_FRAME)
         glBegin(GL_LINE_LOOP);

      if (mode & ETS3D_SOLID_FRAME)
         glNormal3dv((double*)v3);
      vr = (*v3) * (double)fRadius;
      glVertex3dv((double*)&vr);

      if (mode & ETS3D_SOLID_FRAME)
         glNormal3dv((double*)v2);
      vr = (*v2) * (double)fRadius;
      glVertex3dv((double*)&vr);

      if (mode & ETS3D_SOLID_FRAME)
         glNormal3dv((double*)v1);
      vr = (*v1) * (double)fRadius;
      glVertex3dv((double*)&vr);

      if (mode & ETS3D_WIRE_FRAME)
         glEnd();
      return;
   }  
   v12 = Norm(*v1 + *v2);
   v23 = Norm(*v2 + *v3);
   v31 = Norm(*v3 + *v1);

   glcSphereTriangle(v1  , &v12, &v31, depth-1, fRadius, mode);
   glcSphereTriangle(v2  , &v23, &v12, depth-1, fRadius, mode);
   glcSphereTriangle(v3  , &v31, &v23, depth-1, fRadius, mode);
   glcSphereTriangle(&v12, &v23, &v31, depth-1, fRadius, mode);
}

/******************************************************************************
* Name       : DrawCube                                                       *
* Zweck      : Zeichnen eines Würfels mit der Kantenlänge fHeight             *
* Aufruf     : DrawCube(mode, fHeight);                                       *
* Parameter  :                                                                *
* mode    (E): LOWORD : Rekursionstiefe                              (DWORD)  *
*              HIWORD : Modi: Drahtmodell, Oberfläche                         *
* fHeight (E): Kantenlänge des Würfels                               (float)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void DrawCube(DWORD mode, float fHeight)
{
   int    i, nDepth = mode & 0xffff;
   float fHalf = fHeight * 0.5f;
   CVector pV[] = 
   { 
      CVector(-fHalf,  fHalf,  fHalf),
      CVector( fHalf,  fHalf,  fHalf),
      CVector( fHalf, -fHalf,  fHalf),
      CVector(-fHalf, -fHalf,  fHalf),
      CVector(-fHalf,  fHalf, -fHalf),
      CVector( fHalf,  fHalf, -fHalf),
      CVector( fHalf, -fHalf, -fHalf),
      CVector(-fHalf, -fHalf, -fHalf)
   };
   int pn[] = { 3,2,1,0, 4,0,1,5, 4,5,6,7, 2,3,7,6, 1,2,6,5, 0,4,7,3 };
   if (mode & ETS3D_WIRE_FRAME)
   {
      glBegin(GL_LINE_LOOP);
      for (i=0; i< 4; i++)  glVertex3dv((double*)&pV[pn[i]]);
      glEnd();
      glBegin(GL_LINE_LOOP);
      for (i=8; i<12; i++)  glVertex3dv((double*)&pV[pn[i]]);
      glEnd();
      glBegin(GL_LINES);
      for (i=0; i<4; i++)
      {
         glVertex3dv((double*)&pV[pn[i]]);
         glVertex3dv((double*)&pV[pn[11-i]]);
      }
      glEnd();
   }
   else if (mode & ETS3D_SOLID_FRAME)
   {
      if (nDepth == 0)
      {
         glBegin(GL_QUADS);
         for (i=0; i<24; i++) glVertex3dv((double*)&pV[pn[i]]);
         glEnd();
      }
      else
      {
         VectorPointer vp;
         ZeroMemory(&vp, sizeof(VectorPointer));
         vp.pFn   = DrawFan;
         vp.dStep = fHeight / (float)nDepth;
         for (i=0; i<6; i++)
         {
            glcTriangleFan(&pV[i*4  ], &pV[i*4+1], &pV[i*4+2], vp);
            glcTriangleFan(&pV[i*4+1], &pV[i*4+2], &pV[i*4+3], vp);
         }
      }
   }
}


/******************************************************************************
* Name       : DrawFan                                                        *
* Definition : DrawFan(int, CVector *, void *);                               *
* Zweck      : Ausgabe von Vertexes für OpenGL für den Dreieck-Fan.           *
* Aufruf     : Als Callback-Funktion in der Funktion glcTriangleFan(..);      *
* Parameter  :                                                                *
* nCount (E) : Anzahl der übergebenen Punkte                       (int)      *
* pV     (E) : Punkte als CVector-Array                            (CVector*) *
* pParam (E) : Parameter für die Funktion                          (void*)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void DrawFan(int nCount, CVector *pV, void *pParam)
{
   int i;
   if (nCount == 3) glBegin(GL_TRIANGLES);
   else             glBegin(GL_TRIANGLE_FAN);

   for (i=0; i<nCount; i++)
   {
      glVertex3dv((double*)&pV[i]);
   }

   glEnd();
}


/******************************************************************************
* Name       : FindFan                                                        *
* Definition : void FindFan(int, VectorPointer&)                              *
* Zweck      : Suche von gleichen Punkten zwischen Dreiecken, Ermittlung der  *
*              Anzahl der zusammenhängenden Dreiecke mit einem gemeinsamen    *
*              Drehpunkt.                                                     *
* Aufruf     : FindFan(int nCount, VectorPointer& pvLS);                      *
* Parameter  :                                                                *
* pvLS  (EA) : Struktur für die Dreiecksuche                (VectorPointer&)  *
*   ppv       : gespeicherte Vektoren                       (CVector[][])     *
*   nCount    : Anzahl der gespeicherten Dreiecke           (int)             *
*   dStep     : Schrittweite                                (double)          *
*   nEbene    : Rekursionsebene                             (int)             *
*   pFn       : Zeiger auf Funktion für die Bearbeitung der erzeugten Punkte  *
*               (void(*)(int, CVector *, void *)                              *
*   pParam    : Zeiger auf Parameter der Funktion           (void*)           *
* nFanP0 (A) : Drehpunkt des Fans                           (int&)            *
* Funktionswert : Anzahl der Dreiecke mit gleichem Drehpunkt(int)             *
******************************************************************************/
int __fastcall FindFan(VectorPointer& pvLS, int &nFanP0)
{
   int   nTR, i, j, pnEqual[3];
   int   nOldMax   = 0,
         nOldFanP0 = -1,
         nMaxEqual = pvLS.nCount;

   nFanP0 = 0;
   while ((nOldMax != nMaxEqual) || (nOldFanP0 != nFanP0))     // alte und neue Anzahl gleich ?
   {
      pnEqual[0] = 0;
      pnEqual[1] = 0;
      pnEqual[2] = 0;
      nOldMax    = nMaxEqual;                                  // alte Dreieckanzahl merken
      nOldFanP0  = nFanP0;                                     // alten Drehpunkt merken
      for (nTR=0; nTR<nMaxEqual; nTR++)                        // nach gleichen Punkten suchen
      {
         for (i=0; i<3; i++)
         {
            for (j=0; j<3; j++)
            {
               if (pvLS.ppv[0][i] == pvLS.ppv[nTR][j])
               {
                  pnEqual[i]++;
               }
            }
         }
      }
      // die größe Anzahl hat der Drehpunkt
                                  nMaxEqual = pnEqual[0], nFanP0 = 0;
      if (pnEqual[1] > nMaxEqual) nMaxEqual = pnEqual[1], nFanP0 = 1;
      if (pnEqual[2] > nMaxEqual) nMaxEqual = pnEqual[2], nFanP0 = 2;
   }
   return nMaxEqual;
}

/******************************************************************************
* Name       : ExtractFan                                                     *
* Definition : ExtractFan(VectorPointer&);                                    *
* Zweck      : Extrahierung eines Fans aus aufeinanderfolgenden Dreiecken.    *
* Aufruf     : ExtractFan(dStep, pv1, pv2, pv3);                              *
* Parameter  :                                                                *
* pvLS  (EA) : Struktur für die Dreieckextrahierung         (VectorPointer&)  *
*   ppv       : gespeicherte Vektoren                       (CVector[][])     *
*   nCount    : Anzahl der gespeicherten Dreiecke           (int)             *
*   dStep     : Schrittweite                                (double)          *
*   nEbene    : Rekursionsebene                             (int)             *
*   pFn       : Zeiger auf Funktion für die Bearbeitung der erzeugten Punkte  *
*               (void(*)(int, CVector *, void *)                              *
*   pParam    : Zeiger auf Parameter der Funktion           (void*)           *
* Funktionswert : keiner                                                      *
******************************************************************************/
void __fastcall ExtractFan(VectorPointer& pvLS)
{
   bool bEqual;
   int  i, nMaxEqual, j, k, nFanP1, nFanP0;

   nMaxEqual = FindFan(pvLS, nFanP0);                          // ermittle Dreieckanzahl des Fans

   if (nMaxEqual > 1)                                          // mindestens 2 Dreiecke ergeben einen Fan
   {
      CVector pvPoints[MAX_TRIANGLES+2];                       // Fan besteht aus i-Dreiecken mit i+2 Punkten
      pvPoints[0] = pvLS.ppv[0][nFanP0];                       // Punkt 0 = Drehpunkt des Fans !!

      for (i=1; i<nMaxEqual; i++)                              // Schleife über die Anzahl der Dreiecke
      {
         bEqual = false;
         for (j=0; j<3; j++)                                   // Schleife über die Punkte des (i-1). Dreiecks
         {
            if (pvPoints[0] == pvLS.ppv[i-1][j]) continue;     // den Drehpunkt des Fans nicht nehmen
            for (k=0; k<3; k++)                                // Schleife über die Punkte des i. Dreiecks
            {
               if (pvPoints[0]     ==pvLS.ppv[i][k]) continue; // den Drehpunkt des Fans nicht nehmen
               if (pvLS.ppv[i-1][j]==pvLS.ppv[i][k])           // gleicher Punkt gefunden
               {
                  pvPoints[i+1] = pvLS.ppv[i-1][j];            // Punkt i+2 einfügen
                  bEqual = true;                               // und merken, sonst Fehler
                  break;                                       // Schleife unterbrechen
               }
            }
            if ((k==3) && (i==1))                              // ungleicher Punkt und erstes Dreieck
            {
               pvPoints[i] = pvLS.ppv[i-1][j];                 // Punkt 1 einfügen und
               nFanP1 = j;                                     // für die Drehrichtung merken
            }
         }
         if (!bEqual) {nMaxEqual = i; break;}                  // Fehler: Schleife unterbrechen
      }
      for (j=0; j<3; j++)                                      // Schleife über die Punkte des letzten Dreiecks
      {
         if (pvLS.ppv[nMaxEqual-1][j] == pvPoints[0]) continue;// den Drehpunkt des Fans nicht nehmen
         if (pvLS.ppv[nMaxEqual-1][j] != pvPoints[nMaxEqual])  // der vorletzte Punkt muß ungleich
            pvPoints[nMaxEqual+1] = pvLS.ppv[nMaxEqual-1][j];  // dem letzten sein
      }

      if (((nFanP0 == 0) && (nFanP1 == 1)) || ((nFanP0 == 1) && (nFanP1 == 2)) || ((nFanP0 == 2) && (nFanP1 == 0)))
      {                                                        // Der Index muß aufsteigend sein
         pvLS.pFn(nMaxEqual+2, pvPoints, pvLS.pParam);         // Fans an die Callback-Funktion übergeben
      }
      else                                                     // Drehsinn des Fans tauschen
      {
         CVector pTemp[MAX_TRIANGLES+2];
         pTemp[0] = pvPoints[0];
         for (i=1; i<nMaxEqual+2; i++) pTemp[nMaxEqual-i+2] = pvPoints[i];
         pvLS.pFn(nMaxEqual+2, pTemp, pvLS.pParam);            // Fans an die Callback-Funktion übergeben
      }
   }
   else                                                        // kein Fan extrahiert 
   {
      nMaxEqual = 1;                                           // das erste Dreieck
      pvLS.pFn(3, pvLS.ppv[0], pvLS.pParam);                   // an die Callback-Funktion übergeben
   }
   pvLS.nCount -= nMaxEqual;                                   // Anzahl der gespeicherten Dreiecke verringern
   for (i=0; i<pvLS.nCount; i++)                               // Punkte im Suchfeld umspeichern
   {
      pvLS.ppv[i][0] = pvLS.ppv[i+nMaxEqual][0];
      pvLS.ppv[i][1] = pvLS.ppv[i+nMaxEqual][1];
      pvLS.ppv[i][2] = pvLS.ppv[i+nMaxEqual][2];
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void DrawTriangleFan(CVector *pv1, CVector *pv2, CVector *pv3, ETS3DGL_Fan* pvLS)
{
   VectorPointer VP;
   ZeroMemory(&VP, sizeof(VectorPointer));
   VP.dStep  = pvLS->dStep;
   if (pvLS->pFn != NULL) VP.pFn = pvLS->pFn;
   else                   VP.pFn = DrawFan;
   VP.pParam = pvLS->pParam;
   glcTriangleFan(pv1, pv2, pv3, VP);
}

/******************************************************************************
* Name       : glcTriangleFan                                                 *
* Definition : glcTriangleFan(double, CVector *, CVector *, CVector *)        *
* Zweck      : Darstellung eines Dreieckelements mit Rekursiver Aufteilung in *
*              kleinere Dreiecke.                                             *
* Aufruf     : glcTriangleFan(dStep, pv1, pv2, pv3)                           *
* Parameter  :                                                                *
* pv1, pv2, pv3   (E) : Eckpunkte des Dreiecks                       (CVector)*
* pvLS   (E) : Struktur für die Dreieckextrahierung         (VectorPointer&)  *
*   ppv       : gespeicherte Vektoren                       (CVector[][])     *
*   ppnEqual  : Suchfeld für gleiche Punkte                 (int [][])        *
*   nCount    : Anzahl der gespeicherten Dreiecke           (int)             *
*   dStep     : Schrittweite                                (double)          *
*   nEbene    : Rekursionsebene                             (int)             *
*   pFn       : Zeiger auf Funktion für die Bearbeitung der erzeugten Punkte  *
*               (void(*)(int, CVector *, void *)                              *
*   pParam    : Zeiger auf Parameter der Funktion           (void*)           *
* Funktionswert : keiner                                                      *
******************************************************************************/
void __fastcall glcTriangleFan(CVector *pv1, CVector *pv2, CVector *pv3, VectorPointer& pvLS)
{
   ASSERT(((*pv1) != (*pv2)) && ((*pv2) != (*pv3)) && ((*pv1) != (*pv3)));
   pvLS.nEbene++;                                              // Rekusionsebene erhöhen
   double dMax;
   int    nSeite;
   CVector s1,s2,s3;

   if(pvLS.dStep > 0)                                          // Teilung noch nötig ?
   {  // Berechnungsblock
      double l1,l2,l3;

      s1 = (*pv3) - (*pv2);                                    // Strecken berechnen
      s2 = (*pv1) - (*pv3);
      s3 = (*pv2) - (*pv1);

      l1 = Betrag(s1);                                         // Betrag der Strecken
      l2 = Betrag(s2);
      l3 = Betrag(s3);

      if (l1>l2)                                               // längste Seite ermitteln
      {
         if (l1>l3) dMax = l1, nSeite=1;
         else       dMax = l3, nSeite=3;
      }
      else
      {
         if (l2>l3) dMax = l2, nSeite=2;
         else       dMax = l3, nSeite=3;
      }
   }
   else
   {
      ASSERT(pvLS.dStep == 0.0);
      dMax       = -1.0;
      pvLS.dStep = 0.0;
   }
   if (dMax<pvLS.dStep)                                        // Teilung noch nötig ?
   {
      if ((pvLS.nCount < MAX_TRIANGLES) && (dMax > 0))         // Zwischenspeichern der Dreiecke
      {
         pvLS.ppv[pvLS.nCount][0] = *pv1;
         pvLS.ppv[pvLS.nCount][1] = *pv2;
         pvLS.ppv[pvLS.nCount][2] = *pv3;
         pvLS.nCount++;
         if (pvLS.nCount == MAX_TRIANGLES)                     // maximale Anzahl erreicht
         {
            ExtractFan(pvLS);                                  // Fan extrahieren
         }
      }
   }
   else                                                        // sonst weiter aufteilen
   {
      CVector v;
      switch(nSeite)                                           // Teile die längste Seite
      {
         case 1:                                               // s1 = *pv3 - *pv2;
            v = *pv2+0.5*s1;
            if ((pvLS.nEbene & 0x01) == 1)                     // ungerade Ebene
            {
               glcTriangleFan(pv1, pv2, &v , pvLS);
               glcTriangleFan(pv1, &v , pv3, pvLS);
            }
            else
            {
               glcTriangleFan(pv1, &v , pv3, pvLS);
               glcTriangleFan(pv1, pv2, &v , pvLS);
            }

            break;

         case 2:                                               // s2 = *pv1 - *pv3;
            v = *pv3+0.5*s2;
            if ((pvLS.nEbene & 0x01) == 0)
            {
               glcTriangleFan(pv1, pv2, &v , pvLS);
               glcTriangleFan(&v , pv2, pv3, pvLS);
            }
            else
            {
               glcTriangleFan(&v , pv2, pv3, pvLS);
               glcTriangleFan(pv1, pv2, &v , pvLS);
            }
            break;
         case 3:                                               // s3 = *pv2 - *pv1;
            v = *pv1+0.5*s3;
            if ((pvLS.nEbene & 0x01) == 1)
            {
               glcTriangleFan(pv1,  &v, pv3, pvLS);
               glcTriangleFan(&v , pv2, pv3, pvLS);
            }
            else
            {
               glcTriangleFan(&v , pv2, pv3, pvLS);
               glcTriangleFan(pv1,  &v, pv3, pvLS);
            }
            break;
      }
   }
   pvLS.nEbene--;                                              // Rekursionsebene verringern
   if (pvLS.nEbene == 0)                                       // Oberste Ebene erreicht ?
   {
      while (pvLS.nCount > 0)                                  // aus gesammelten Dreiecken 
      {
         ExtractFan(pvLS);                                     // restliche Fans extrahieren
      }
   }
}

// exportierte Klassenfunktionen der Klasse CEts3DGLDll
bool CreateRenderList(CEts3DGLDll *p3DGL, int nID, void (*pfn)(CEts3DGLDll *, int, int), int nRange, bool bCommand)
{
   return p3DGL->CreateRenderListDll(nID, pfn, nRange, bCommand);
}

void OnRenderSzene(CEts3DGLDll *p3DGL)
{
   p3DGL->OnRenderSzeneDll();
}

bool SetList(CEts3DGLDll *p3DGL, UINT nID, UINT nParam)
{
   return p3DGL->SetListDll(nID, nParam);
}

void TransformPoints(CEts3DGLDll *p3DGL, CVector *pv, int nPoints, bool bDir)
{
   p3DGL->TransformPointsDll(pv, nPoints, bDir);
}
// Ende der exportierten Klassenfunktionen

CEts3DGLDll *CEts3DGLDll::gm_pFirstObjectInstance = NULL;

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::ErrorMessage(char *pszText)
{
   ::MessageBox(NULL, pszText, "ETS3DGL-Error", MB_OK);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
BOOL CEts3DGLDll::PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (m_nThreadID)
      return ::PostThreadMessage(m_nThreadID, nMsg, wParam, lParam);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
bool CEts3DGLDll::DestroyAtDllTermination()
{
   if ((gm_pFirstObjectInstance != NULL) && ((m_nModes & ETS3D_DESTROY_AT_DLL_TERMINATION)==ETS3D_DESTROY_AT_DLL_TERMINATION))
      return true;
   return false;
}

/******************************************************************************
* Name       : DetachThread                                                   *
* Definition : HWND DetachThread();                                           *
* Zweck      : Initialisierten OpenGL-Thread von der Renderklasse lösen und   *
*              Parameter in der ersten DLL-Bindungs-Instanz speichern. Die    *
*              OpenGL-Parameter werden im DLL-Destruktor wieder freigegeben.  *
* Aufruf     : DetachThread();                                                *
* Parameter  : keine                                                          *
* Rückgabe   : Altes Parentwindow                                 (HWND)      *
******************************************************************************/
HWND CEts3DGLDll::DetachThread()
{
   HWND hWndParent = NULL;
   try
   {
      DWORD dwExitCode;                                        // Thread parameter retten
      if (gm_pFirstObjectInstance == NULL)       throw (int)1; // eine erste Instanz muß existieren
      if (gm_pFirstObjectInstance->m_hWndThread) throw (int)2; // wurde schon ein Thread gespeichert
      if (::GetCurrentThreadId() == m_nThreadID) throw (int)3; // Diese Funktion darf nur aus dem Hauptthread aufgerufen werden

      KillTimer(m_nRenderTimer);                               // Parentwindow Timer beenden
      if (!m_hThread)                            throw (int)4; // der Thread muß existieren
      if (!m_nThreadID)                          throw (int)5; //
      if (!GetExitCodeThread(m_hThread, &dwExitCode))throw (int)6;//
      if (dwExitCode != STILL_ACTIVE)            throw (int)7; // und aktiv sein

      ::ShowWindow(m_hWndThread, SW_HIDE);                     // Fenster verstecken

      gm_pFirstObjectInstance->m_hWndThread = m_hWndThread;    // Windowhandle des OpenGL-Threads
      gm_pFirstObjectInstance->m_hThread    = m_hThread;       // Thread-Handle
      gm_pFirstObjectInstance->m_hEvent     = m_hEvent;        // Eventhandle für den Thread
      gm_pFirstObjectInstance->m_nThreadID  = m_nThreadID;     // ThreadID
      gm_pFirstObjectInstance->m_nModes     = m_nModes;        // Die Modi werden zum Vergleich mit dem nächsten

      gm_pFirstObjectInstance->m_hDC        = m_hDC;           // Gerätekontexthandle
      gm_pFirstObjectInstance->m_hPalette   = m_hPalette;      // Farbpalette
      gm_pFirstObjectInstance->m_hGL_RC     = m_hGL_RC;        // Renderkontext
      gm_pFirstObjectInstance->m_pDib       = m_pDib;          // DibSection zum Rendern

      if (!::ResetEvent(m_hEvent))               throw (int)8; // Synchronisierungs Event zurücksetzen
      if (!PostThreadMessage(WM_DETACH_THREAD_STATES,0,0))     // Parameter löschen und
                                                 throw (int)9; // Klassenzeiger tauschen
      if (WaitForThreadEvent() != WAIT_OBJECT_0) throw (int)11;// auf Synchronisierungs Event warten

      hWndParent = SetParent(m_hWndThread, NULL);              // Elternfenster entfernen
      if (!hWndParent)                           throw (int)12;
      long lStyle = ::GetWindowLong(m_hWndThread, GWL_STYLE);  // ohne Parent Fenster
      lStyle |= WS_POPUP;                                      // wird Popup Fenster
      lStyle &= ~WS_CHILD;                                     // kein Child mehr
      ::SetWindowLong(m_hWndThread, GWL_STYLE, lStyle);        // muß der Fensterstil geändert werden

      m_hWndThread = NULL;                                     // Nach dem Austasch der Klassenzeiger
      m_hThread    = NULL;                                     // können diese Parameter auf 0 gestzt werden
      m_hEvent     = NULL;
      m_nThreadID  = NULL;

      m_hDC        = NULL;
      m_hPalette   = NULL;
      m_hGL_RC     = NULL;
      m_pDib       = NULL;
   }
   catch (int nError)
   {
      REPORT("DetachThread-Error: %d, %x", nError, GetLastError());
   }
   return hWndParent;
}

/******************************************************************************
* Name       : AttachThread                                                   *
* Definition : bool AttachThread(HWND);                                       *
* Zweck      : Dem initialisierten OpenGL-Thread die Parameter der neuen      *
*              Renderklasse und eine neues Parentwindow zuweisen. Render-     *
*              fenster wieder anzeigen. Die gespeicherten Parameter der ersten*
*              DLL-Bindungsinstanz werden der neuen Klasse zugewiesen         *
* Aufruf     : AttachThread(hwndParent);                                      *
* Parameter  : Neues Parentwindow                                     (HWND)  *
******************************************************************************/
bool CEts3DGLDll::AttachThread(HWND hwndParent)
{
   try
   {
      DWORD dwExitCode;                                        // Threadparameter wiederholen
      if (gm_pFirstObjectInstance == NULL)       throw (int)1; // Nur wenn es schon eine Instanz gibt
      if (gm_pFirstObjectInstance->m_hThread == NULL) throw (int)2; // Wenn kein Thread detached wurde
      if ((gm_pFirstObjectInstance->m_nModes&(ETS3D_CM_CLEAR|ETS3D_DC_CLEAR)) != 
                                   (m_nModes&(ETS3D_CM_CLEAR|ETS3D_DC_CLEAR)))
                                                 throw (int)3; // Color und Render-Modi müssen gleich sein
      // gesicherte Handles / Daten
      m_nStates |= DO_NOT_VALIDATE;                            // nicht validieren !
      m_nThreadID  = gm_pFirstObjectInstance->m_nThreadID;     // ThreadID
      m_hEvent     = gm_pFirstObjectInstance->m_hEvent;        // Eventhandle für den Thread
      m_hThread    = gm_pFirstObjectInstance->m_hThread;       // Thread-Handle
      m_hWndThread = gm_pFirstObjectInstance->m_hWndThread;    // Windowhandle des OpenGL-Threads
      ASSERT(::IsWindow(m_hWndThread));

      m_pDib       = gm_pFirstObjectInstance->m_pDib;       // DibSection zum Rendern
      m_hGL_RC     = gm_pFirstObjectInstance->m_hGL_RC;     // Renderkontext
      m_hPalette   = gm_pFirstObjectInstance->m_hPalette;   // Farbpalette
      m_hDC        = gm_pFirstObjectInstance->m_hDC;        // Device Context

      if (!GetExitCodeThread(m_hThread, &dwExitCode)) throw (int)6;
      if (dwExitCode != STILL_ACTIVE)                 throw (int)7;

      long lStyle = ::GetWindowLong(m_hWndThread, GWL_STYLE);  // mit Parentfenster
      lStyle |= WS_CHILD;                                      // wird Child Fenster
      lStyle &= ~WS_POPUP;                                     // Kein Popup mehr
      ::SetWindowLong(m_hWndThread, GWL_STYLE, lStyle);        // muß der Fensterstil geändert werden
      HWND hwnd = SetParent(m_hWndThread, hwndParent);         // Neues Elternfenster setzen

      if (hwnd == NULL)                          throw (int)13;

      if (!::ResetEvent(m_hEvent))               throw (int)8; // Synchronisierungs Event zurücksetzen
      if (PostThreadMessage(WM_ATTACH_THREAD_STATES, (WPARAM)this, 0)==0)// Parameter initialisieren,
                                                 throw (int)10;// Klassenzeiger tauschen
      if (WaitForThreadEvent() != WAIT_OBJECT_0) throw (int)11;// auf Synchronisierungs Event warten

      gm_pFirstObjectInstance->m_nThreadID  = NULL;            // nach dem Austausch der Klassenzeiger
      gm_pFirstObjectInstance->m_hEvent     = NULL;            // können die Parameter auf 0 gesetzt werden
      gm_pFirstObjectInstance->m_hThread    = NULL;
      gm_pFirstObjectInstance->m_hWndThread = NULL;

      gm_pFirstObjectInstance->m_pDib       = NULL;
      gm_pFirstObjectInstance->m_hGL_RC     = NULL;
      gm_pFirstObjectInstance->m_hPalette   = NULL;
      gm_pFirstObjectInstance->m_hDC        = NULL;

      ::ShowWindow(m_hWndThread, SW_SHOW);                     // Threadfenster anzeigen 

      PostOpenGLState(1, 0);                                   // OpenGL Initialisiert

      if (!(m_nModes & ETS3D_NO_TIMER))                        // Timer initialisieren
      {
         m_nRenderTimer = ::SetTimer(hwndParent, CONTROL_TIMER_EVENT, m_nTime, NULL);
         if (m_nRenderTimer == 0)                throw (int)12;
      }

      PostThreadMessage(WM_THREAD_SIZE, 0, 0);

      return true;
   }
   catch (int nError)
   {
      if (nError != 2)
      {
         m_nModes &= ~ETS3D_DESTROY_AT_DLL_TERMINATION;        // Flag löschen
         REPORT("Error AttachThread : %d, %x", nError, GetLastError());
      }
   }
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND CEts3DGLDll::FindTopLevelParent(HWND hwndChild)
{
   HWND hwndParent = hwndChild;
   do
   {
      hwndChild  = hwndParent;
      hwndParent = ::GetParent(hwndChild);
   } while (hwndParent);
   return hwndChild;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CAutoCriticalSection::CAutoCriticalSection(CRITICAL_SECTION * pCS)
{
   m_pCS = pCS;
   if (m_pCS)
   {
      EnterCriticalSection(m_pCS);
//      REPORT("EnterCriticalSection %x", m_pCS);
   }
};

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
CAutoCriticalSection::~CAutoCriticalSection()
{
   if (m_pCS)
   {
//      REPORT("LeaveCriticalSection %x", m_pCS);
      LeaveCriticalSection(m_pCS);
   }
};

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::MsgBoxGlError()
{
   for (;;)
   {
      GLenum nError = glGetError();
      if (nError != GL_NO_ERROR)
      {
         char text[256];
         switch(nError)
         {
            case GL_INVALID_ENUM:      strcpy(text, "An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag.");
            case GL_INVALID_VALUE:     strcpy(text, "A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag.");
            case GL_INVALID_OPERATION: strcpy(text, "The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag.");
            case GL_STACK_OVERFLOW:    strcpy(text, "This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag.");
            case GL_STACK_UNDERFLOW:   strcpy(text, "This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag.");
            case GL_OUT_OF_MEMORY:     strcpy(text, "There is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded.");
         }
   #ifdef _DEBUG
         ErrorMessage(text);
   #else
         REPORT("Error : %s", text);
   #endif
      }
      else break;
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::InitRegistryKeys()
{
   HKEY hKey;
   char szRegKey[_MAX_PATH];
   wsprintf(szRegKey, "%s\\%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY, REGKEY_BUGFIX);
   if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;
      if (RegQueryValueEx(hKey, REGKEY_BUGFIX_BUFFERSIZE, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if (dwType== REG_DWORD) gm_bBugFixBufferSize = (dwValue!=0) ? true : false;
      }
      if (RegQueryValueEx(hKey, REGKEY_BUGFIX_BUFFER, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if (dwType== REG_DWORD) gm_nBugFixBuffer = dwValue;
      }
      if (RegQueryValueEx(hKey, REGKEY_BUGFIX_MOVEWINDOW, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if (dwType== REG_DWORD) gm_bBugFixMoveWindow = (dwValue!=0) ? true : false;
      }
      if (RegQueryValueEx(hKey, REGKEY_BUGFIX_NOGRAPHIC, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if (dwType== REG_DWORD) gm_bNoGraphic = (dwValue!=0) ? true : false;
      }
      RegCloseKey(hKey);
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::UnregisterClass()
{
   if (gm_ETS3D_Class)
   {
      ::UnregisterClass((LPCTSTR)gm_ETS3D_Class, gm_hInstance);
      gm_ETS3D_Class = NULL;
   }
   if (gm_ETS3DParentClass)
   {
      ::UnregisterClass((LPCTSTR)gm_ETS3DParentClass, gm_hInstance);
      gm_ETS3DParentClass = NULL;
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND CEts3DGLDll::GetParent()
{
   if (m_hWndThread)
   {
      return ::GetParent(m_hWndThread);
   }
   return NULL;
}

HWND CEts3DGLDll::GetTopLevelParent()
{
   if (m_hWndThread)
   {
      if (::GetWindowLong(m_hWndThread, GWL_THREADWINDOWID) != THREADWINDOWID)
      {
         HWND hWndTop = FindTopLevelParent(m_hWndThread);
         if (hWndTop)
         {
            ::SetWindowLong(m_hWndThread, GWL_PARENTWINDOW, (long)hWndTop);   // TopLevelParent speichern
            ::SetWindowLong(m_hWndThread, GWL_THREADWINDOWID, THREADWINDOWID);// Identifikation des ThreadWindows
            return hWndTop;
         }
      }
      else
         return (HWND) ::GetWindowLong(m_hWndThread, GWL_PARENTWINDOW);
   }
   return NULL;
}

void CEts3DGLDll::PostOpenGLState(WPARAM wParam, LPARAM lParam)
{
   if (m_nModes & ETS3D_POST_OPENGL_STATE)
   {
      int i, n = (m_nModes & ETS3D_POST_OPENGL_STATE) >> ETS3D_OPENGL_STATE_SHIFT;
      HWND hwnd = GetParent();
      if (hwnd)
      {
         for (i=0; i<n; i++)
         {
            hwnd = ::GetParent(hwnd);
            if (hwnd == NULL) break;
            if (i == n-1)
            {
               ::PostMessage(hwnd, WM_OPENGL_STATE, wParam, lParam);
            }
         }
      }
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
BOOL CALLBACK CEts3DGLDll::FindWindow(HWND hwnd, LPARAM lParam)
{
   HWND *pwnd = (HWND *) lParam;
   if (hwnd == pwnd[0])
   {
      pwnd[1] = hwnd;
      return false;
   }
   return true;
}

#ifndef _MULTI_3DWINDOWS
/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
HWND CEts3DGLDll::Create(LPCTSTR lpWindowName, DWORD dwStyle, RECT *prect, HWND hwndParent)
{
   if (gm_bNoGraphic) return NULL;
   if (::IsWindow(hwndParent))
   {
      HWND hwndThreadParent = NULL;
      try
      {
         if (dwStyle & WS_CHILD)
         {
            if (!gm_ETS3DParentClass)                          // noch nicht registriert ?
            {
               WNDCLASSEX	 wcl;
               ZeroMemory(&wcl, sizeof(WNDCLASSEX));
               // Fensterklasse definieren
               wcl.cbSize        = sizeof(WNDCLASSEX);         // Größe von WNDCLASSEX
               wcl.hInstance     = gm_hInstance;               // Handle zu dieser Instance
               wcl.lpszClassName = CETSGLPARENT_CLASSNAME;     // Name der Fensterklasse (Programmfenster)
               wcl.lpfnWndProc   = CEts3DGLDll::WindowFunc;    // Fensterfunktion
               wcl.style         = CS_HREDRAW|CS_VREDRAW;      // Fensterstil
               wcl.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursorstil
               wcl.hbrBackground	= NULL;                       // weißer Hintergrund
               wcl.cbWndExtra    = 4;
               gm_ETS3DParentClass = ::RegisterClassEx(&wcl);  // Fensterklasse registrieren
            }

            dwStyle &= ~(WS_CLIPCHILDREN|WS_CLIPSIBLINGS);     // dieser Stil darf nicht gesetzt werden
            HWND hwndFind[2] = {m_hWndThread, NULL};           // Suchen nach dem Childwindow
            ::EnumChildWindows(hwndParent, FindWindow, (LPARAM)hwndFind);
            if (hwndFind[1] == NULL)                           // Wenn es noch nicht Childwindow war
            {
               hwndThreadParent = CreateWindowEx(
                  0,                                           // EX-Style
                  (LPCTSTR)gm_ETS3DParentClass,                // Name der Fensterklasse (Programmfenster)
                  lpWindowName,                                // Name des Fensters
                  dwStyle,                                     // Fensterstil
                  prect->left, prect->top,                     // y-Koordinate, x-Koordinate
                  prect->right-prect->left,                    // Fensterbreite
                  prect->bottom-prect->top,                    // Fensterhöhe
                  hwndParent,                                  // übergeordnetes Fenster für das ParentWindow
                  NULL,                                        // kein Menu
                  gm_hInstance,                                // Handle auf diese Programminstance
                  this);                                       // zusätzliche Argumente
               if (!hwndThreadParent) throw (int)1;            // nicht erzeugt : Fehler
               m_hWndThread = hwndThreadParent;
            }
            hwndParent = hwndThreadParent;
            m_nStates |= ETS3D_EXTRA_CHILDWND;
         }
         else
         {
            PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwndParent, OLD_WINDOW_PROC);
            if (!pnWF)
            {
               pnWF = (PropertyWindowFunc*) malloc(sizeof(PropertyWindowFunc));
               if (!pnWF) throw (int)2;
               long nID = ::GetWindowLong(hwndParent, GWL_ID);
               if (nID == 0) ::SetWindowLong(hwndParent, GWL_ID, 20395);
               pnWF->pfnWindowFunc = ::SetWindowLong(hwndParent, GWL_WNDPROC, (long)CEts3DGLDll::WindowFunc);
               pnWF->p3DGL = this;
               ::SetProp(hwndParent, OLD_WINDOW_PROC, (HANDLE)pnWF);
            }
            m_nStates &= ~ETS3D_EXTRA_CHILDWND;
         }

         if (m_pcsExt == NULL)
         {
            m_pcsExt = new CRITICAL_SECTION;
            if (m_pcsExt)
            {
               InitializeCriticalSection(m_pcsExt);            // Kritische Sektion Externe Daten initisalisieren
               m_nStates |= CS_EXT_INITIALIZED;
            }
            else throw (int)3;
         }

         InitializeCriticalSection(&m_csDisplay);              // Kritische Sektion für Darstellung initisalisieren
         m_nStates |= CS_DIB_INITIALIZED;

         if (DestroyAtDllTermination() &&                      // wenn der Renderkontext erst am Schluß zerstört werden soll
             (gm_pFirstObjectInstance->m_hWndThread != NULL) &&// und er schon vorhanden ist
             AttachThread(hwndParent))                         // Parentfenster dem Thread zuordnen;
            return hwndParent;

         if (!StartRenderThread(hwndParent)) throw (int)4;

         return hwndParent;
      }
      catch (int nError)
      {
         REPORT("CreateError : %d", nError);
         KillRenderThread();
         if (hwndThreadParent) DestroyWindow(hwndThreadParent);
         return NULL;
      }
   }
   return NULL;
}
#else
HWND CEts3DGLDll::Create(LPCTSTR lpWindowName, DWORD dwStyle, RECT *prect, HWND hwndParent)
{
   // Dise ist die aktuelle !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   if (::IsWindow(hwndParent))
   {
      HWND hwndThreadParent = NULL;
      try
      {
         if (dwStyle & WS_CHILD)
         {
            if (!gm_ETS3DParentClass)
            {
               WNDCLASSEX	 wcl;
               ZeroMemory(&wcl, sizeof(WNDCLASSEX));
               // Fensterklasse definieren
               wcl.cbSize        = sizeof(WNDCLASSEX);               // Größe von WNDCLASSEX
               wcl.hInstance     = gm_hInstance;                     // Handle zu dieser Instance
               wcl.lpszClassName = CETSGLPARENT_CLASSNAME;           // Name der Fensterklasse (Programmfenster)
               wcl.lpfnWndProc   = CEts3DGLDll::WindowFunc;          // Fensterfunktion
               wcl.style         = CS_HREDRAW|CS_VREDRAW;            // Fensterstil
               wcl.hCursor       = LoadCursor(NULL, IDC_ARROW);      // Cursorstil
               wcl.hbrBackground	= NULL;                             // weißer Hintergrund
               wcl.cbWndExtra    = 4;
               // Fensterklasse registrieren
               gm_ETS3DParentClass = ::RegisterClassEx(&wcl);
            }

            HWND hwndFind[2] = {m_hWndThread, NULL};
            ::EnumChildWindows(hwndParent, FindWindow, (LPARAM)hwndFind);
            if (hwndFind[1] == NULL) 
            {
               hwndThreadParent = CreateWindowEx(
                  0,                                           // EX-Style
                  (LPCTSTR)gm_ETS3DParentClass,                // Name der Fensterklasse (Programmfenster)
                  "GLParentWindow",                            // Name des Fensters
                  dwStyle,                                     // Fensterstil
                  prect->left, prect->top,                     // y-Koordinate, x-Koordinate
                  prect->right-prect->left,                    // Fensterbreite
                  prect->bottom-prect->top,                    // Fensterhöhe
                  hwndParent,                                  // übergeordnetes Fenster für das ParentWindow
                  NULL,                                        // kein Menu
                  gm_hInstance,                                // Handle auf diese Programminstance
                  this);                                       // zusätzliche Argumente
               if (!hwndThreadParent) throw (int)1;
               hwndParent = hwndThreadParent;
            }

            m_nStates |= ETS3D_EXTRA_CHILDWND;
         }
         else
         {
            PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwndParent, OLD_WINDOW_PROC);
            if (!pnWF)
            {
               pnWF = (PropertyWindowFunc*) malloc(sizeof(PropertyWindowFunc));
               if (!pnWF) throw (int)2;
               long nID = ::GetWindowLong(hwndParent, GWL_ID);
               if (nID == 0) ::SetWindowLong(hwndParent, GWL_ID, 20395);
               pnWF->pfnWindowFunc = ::SetWindowLong(hwndParent, GWL_WNDPROC, (long)CEts3DGLDll::WindowFunc);
               pnWF->p3DGL[0]      = this;
               pnWF->nCount        = 1;
               ::SetProp(hwndParent, OLD_WINDOW_PROC, (HANDLE)pnWF);
            }
            else
            {
               pnWF = (PropertyWindowFunc*) realloc(pnWF, sizeof(PropertyWindowFunc)+sizeof(CEts3DGLDll*)*pnWF->nCount);
               pnWF->p3DGL[pnWF->nCount++] = this;
               ::SetProp(hwndParent, OLD_WINDOW_PROC, (HANDLE)pnWF);
            }
            m_nStates &= ~ETS3D_EXTRA_CHILDWND;
         }

         RECT rcClient;
         if (gm_bBugFixBufferSize)                                   // BugFix für Rage 128 GL und Windows 2000 ICD
         {                                                           // Rendercontext muß bei voller Bildschirmgröße erstellt werden
            ::GetClientRect(hwnd, &rcClient);
            ::MoveWindow(hwnd, 0, 0, ::GetSystemMetrics(SM_CXSCREEN)-1, ::GetSystemMetrics(SM_CYSCREEN)-1, false);
         }

         if (m_pcsExt == NULL)
         {
            m_pcsExt = new CRITICAL_SECTION;
            if (m_pcsExt)
            {
               InitializeCriticalSection(m_pcsExt);                  // Kritische Sektion Externe Daten initisalisieren
               m_nStates |= CS_EXT_INITIALIZED;
            }
            else throw (int)3;
         }

         InitializeCriticalSection(&m_csDisplay);                     // Kritische Sektion für Darstellung initisalisieren
         m_nStates |= CS_DIB_INITIALIZED;

         if (!StartRenderThread(hwndParent)) throw (int)4;
         if (gm_bBugFixBufferSize)                                // Die Fenstergröße wiederherstellen
         {
            ::PostMessage(GetParent(hwnd), WM_SIZE, SIZE_RESTORED, MAKELONG(rcClient.right, rcClient.bottom));
         }
         return hwndParent;
      }
      catch (int nError)
      {
         REPORT("CreateError : %d", nError);
         KillRenderThread();
         if (hwndThreadParent) DestroyWindow(hwndThreadParent);
         return NULL;
      }
   }
   return NULL;
}
#endif
