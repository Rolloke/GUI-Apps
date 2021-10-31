/*******************************************************************************
                                 CEts3DGL
  Zweck:
   Basisklasse zur Erzeugung und Steuerung der 3D-Grafikausgabe mit Hilfe von
   OpenGL®. Die Funktionen sind in der ETS3DGL.dll implementiert.
  Header:            Bibliotheken:        DLL´s:
   ----              EtsBind.lib          ETSBIND.dll
   ----              ----                 ETS3DGL.dll
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die ETS3DGL.dll ist eine
   "load on call-Dll". D.h., sie wird erst geladen, wenn sie das erste mal be-
   nötigt wird.
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 11.10.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined(AFX_CARA3D_H__1F601677_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_CARA3D_H__1F601677_CFCA_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EtsAppWndMsg.h"

// Makrofunktionen
#define RAD10(A)((A)*1.74532925199e-3)                         // Umrechnung von GRAD*10 in RAD 
#define RAD(A)  ((A)*1.74532925199e-2)                         // Umrechnung von GRAD in RAD 
#define GRAD(A) ((A)*57.2957795131)                            // Umrechnung von RAD in GRAD 
#define INV_256(A) ((A)*0.00390625f)                           // Teilen durch 256

// Flags für Modi
#define ETS3D_CM_RGB                      0x00000001           // RGB-Mode    (*)
#define ETS3D_CM_4096                     0x00000002           // 4096 Farben
#define ETS3D_CM_256                      0x00000004           // 256 Farben
#define ETS3D_CM_CLEAR                    0x0000000f           // Löschflags

#define ETS3D_DC_DIRECT                   0x00000030           // Direkt in einen DC Rendern
#define ETS3D_DC_DIRECT_1                 0x00000010           // Direkt in einen DC Rendern
#define ETS3D_DC_DIRECT_2                 0x00000020           // Direkt in einen DC Rendern (double buffer) (*)
#define ETS3D_DC_DIB                      0x000000c0           // Rendern in eine DIB-Section
#define ETS3D_DC_DIB_1                    0x00000040           // Rendern in eine DIB-Section
#define ETS3D_DC_DIB_2                    0x00000080           // Rendern in zwei DIB-Section (double buffer)
#define ETS3D_DC_CLEAR                    0x000000f0           // Löschflags

#define ETS3D_PR_ORTHOGONAL               0x00000100           // orthogonale Projektion
#define ETS3D_PR_PERSPECTIVE              0x00000200           // perspektivische Projektion definiert durch Öffnungswinkel alpha (*)
#define ETS3D_PR_FRUSTUM                  0x00000400           // perspektivische Projektion definiert durch Darstellungsvolumen
#define ETS3D_PR_LOOK_AT                  0x00000800           // LookAt Perspektive: kombinierbar mit jeweils einem deranderen Flags
#define ETS3D_PR_CLEAR                    0x00000f00           // Löschflags

#define ETS3D_SIZE_FIXED                  0x00001000           // Festgelegte Größe
#define ETS3D_AUTO_DELETE                 0x00002000           // Automatisches zerstören
#define ETS3D_NO_TIMER                    0x00004000           // Keinen Timer starten
#define ETS3D_DONT_WAIT                   0x00008000           // Nicht auf die Erstellung warten
#define ETS3D_COPY_TO_CLIPBOARD           0x00010000           // Automatisch ins Clipboard kopieren
#define ETS3D_SAVE_AS_BMP_FILE            0x00020000           // Automatisch als BMP-File speichern
#define ETS3D_DETACH_FOREIGN_DATA         0x00040000           // Fremde Daten nicht löschen
#define ETS3D_DOUBLEBUFFER_DONTCARE       0x00080000           // Double-Buffer nicht erforderlich
#define ETS3D_DIBSECTION_DONTCARE         0x00100000           // Statt Dibsection darf auch Window gewählt werden
#define ETS3D_EXTRA_CHILDWND              0x00200000           // Zusätzliches ChildWindow wurde erzeugt
#define ETS3D_THREADWND_NOCHILD           0x00400000           // Zusätzliches ChildWindow wurde erzeugt
#define ETS3D_DESTROY_AT_DLL_TERMINATION  0x00800000           // erst beenden, wenn die Dll aus dem Process entfernt wird
#define ETS3D_KEYBOARD_INPUT              0x01000000           // Keybord Abfrage
#define ETS3D_DIRECTX_JOYSTIC             0x02000000           // DirectX Joystic Abfrage
#define ETS3D_POST_OPENGL_STATE           0x30000000           // Sende Nachricht über (nicht) initialisiert zurück
#define ETS3D_OPENGL_STATE_SHIFT          28                   // Shift um das Parent-Window zu bestimmen: n Stufen höher

// Vordefinierte Modi für die Benutzung der 3D-Klasse
// einmaliges Rendern einer Scene in eine DibSection zum Kopieren oder Drucken etc...
#define ETS3D_DC_COPY                ETS3D_DC_DIB_1|ETS3D_SIZE_FIXED|ETS3D_NO_TIMER
// Kopieren einer Scene als DIB-Format ins Clipboard im Hintergrund mit automatischem Löschen des Objektes
#define ETS3D_CLIPBOARD_INDEPENDENT  ETS3D_DC_COPY|ETS3D_COPY_TO_CLIPBOARD|ETS3D_AUTO_DELETE|ETS3D_DONT_WAIT|ETS3D_DETACH_FOREIGN_DATA
#define ETS3D_BMP_FILE_INDEPENDENT   ETS3D_DC_COPY|ETS3D_SAVE_AS_BMP_FILE|ETS3D_AUTO_DELETE|ETS3D_DONT_WAIT|ETS3D_DETACH_FOREIGN_DATA

// Flags für die Funktion Draw 
#define ETS3D_WIRE_FRAME                  0x00010000           // Zeichne als Drahtmodel
#define ETS3D_SOLID_FRAME                 0x00020000           // Zeichne als feste Hülle
#define ETS3D_SPHERE                      0x00100000           // Zeichne eine Kugel
#define ETS3D_CUBE                        0x00200000           // Zeichen eine Würfel

// OnCommand Messages
#define ETS3D_CD_CHANGEMODE               10
#define ETS3D_CD_INITIALIZED              11                   // für Modus ETS3D_DONT_WAIT
#define ETS3D_CD_SZENE_READY              12                   // für Modus ETS3D_DONT_WAIT

#define PFD_DRAW_TO_BITMAP_DONTCARE       0x10000000           // PixelFormat Erweiterung


// Vordefinitionen
class  CEts3DGL;
class  CDibSection;
class  CVector;
struct RenderList;

// Typdefinitionen
typedef void (*LISTFUNCTION)(CEts3DGL*, int, int);
typedef void (*FANFUNCTION) (int, CVector *, void *);

// Strukturen
struct ETS3DGL_Fan
{
   double      dStep;               // Schrittweite
   FANFUNCTION pFn;                 // Zeiger auf Funktion für die erzeugten Punkte
   void       *pParam;              // Zeiger auf Parameter [optional] der Funktion
};

struct ETS3DGL_Volume               // Struktur für Viewing Volume
{
   float Left,  Top,    Near,       // Ecke 1
         Right, Bottom, Far;        // Ecke 2
};

struct ETS3DGL_LookAt               // Struktur für den LookAt-Modus
{
   double dEyex, dEyey, dEyez;      // Betrachterposition
   int    nTheta;                   // Elevationswinkel
   int    nPhi;                     // Azimuthwinkel
   int    nXi;                      // Kippwinkel
};

struct ETS3DGL_Bits
{
   unsigned long nColor     : 8;      // ColorBits
   unsigned long nDepth     : 8;      // DepthBits
   unsigned long nStencil   : 8;      // StencilBits
   unsigned long nAlpha     : 8;      // AlphaBits
   unsigned long nAccum     : 8;      // AccumulationBits
   unsigned long nGeneric   : 1;      // Generic format
   unsigned long nGenAccel  : 1;      // Generic Accelerated
   unsigned long nSuppGDI   : 1;      // Support GDI
   unsigned long nSuppGL    : 1;      // Support OpenGL
   unsigned long nNeedPal   : 1;      // Needs Palette
   unsigned long nNeedSysPal: 1;      // Needs Systempalette
   unsigned long nStereo    : 1;      // Stereoscopical
   unsigned long nSwapLayBuf: 1;      // Swap Layer Buffer
   unsigned long nSwapCopy  : 1;      // Swap Copy
   unsigned long nSwapExch  : 1;      // Swap Exchange
   unsigned long nDummy1    : 14;     // Dummybits 
};

struct ETS3DGL_Draw
{
   RECT         rcFrame;
   HDC          hDC;
   HENHMETAFILE hMeta;
};

class CGlLight  
{
public:
   CGlLight();
   CGlLight(int);
   void SetPosition(CVector &, float);                // void SetPosition(CVector vPos, float w)
   void SetPosition(float, float, float, float);      // void SetPosition(float x, float y, float z, float w)
   void SetPosition(float*);                          // void SetPosition(float* pPos)
   CVector GetPostion();                              // CVector GetPostion()
   void SetDirection(CVector&);                       // void SetDirection(CVector &vDir)
   void SetDirection(float, float, float);            // void SetDirection(float x, float y, float z)
   void SetDirection(float*);                         // void SetDirection(float *pvDir)
   CVector GetDirection();                            // CVector GetDirection()
   void SetAttenuation(float, float, float);          // void SetAttenuation(float fConstant, float fLinear, float fQuadratic)
   void SetAmbientColor(float, float, float, float);  // void SetAmbientColor(float fR, float fG, float fB, float fA)
   void SetAmbientColor(float*);                      // void SetAmbientColor(float * pfColor)
   void SetAmbientColor(COLORREF);                    // void SetAmbientColor(COLORREF Color)
   COLORREF GetAmbientColor();                        // COLORREF GetAmbientColor()
   void SetDiffuseColor(float, float, float, float);  // void SetDiffuseColor(float fR, float fG, float fB, float fA)
   void SetDiffuseColor(float*);                      // void SetDiffuseColor(float * pfColor)
   void SetDiffuseColor(COLORREF);                    // void SetDiffuseColor(COLORREF Color)
   COLORREF GetDiffuseColor();                        // COLORREF GetDiffuseColor()
   void SetSpecularColor(float, float, float, float); // void SetSpecularColor(float fR, float fG, float fB, float fA)
   void SetSpecularColor(float*);                     // void SetSpecularColor(float * pfColor)
   void SetSpecularColor(COLORREF);                   // void SetSpecularColor(COLORREF Color)
   COLORREF GetSpecularColor();                       // COLORREF GetSpecularColor()

   static BYTE     FloatToByte(float);                // BYTE FloatToByte(float fVal)
   static COLORREF FloatsToColor(float*);             // COLORREF FloatsToColor(float *pfColor)
	static float*   ColorToFloat(COLORREF, float*);    // float* ColorToFloat(COLORREF Color, float* fColor)
   static void     MultnFloat(int, float*, float);    // void  MultnFloat(int nValues, float*pfValues, float fMultiplicator)
public:
   int   m_nLight;
   bool  m_bEnabled;
   float m_pfAmbientColor[4];
   float m_pfDiffuseColor[4];
   float m_pfSpecularColor[4];
   float m_pfPosition[4];
   float m_pfDirection[3];
   float m_fSpotExponent;
   float m_fSpotCutOff;
   float m_fAttenuation[3];
private:
   void  Init();
};

class CAutoCriticalSection
{
public:
   CAutoCriticalSection(CEts3DGL &);
   CAutoCriticalSection(CRITICAL_SECTION *pCS);
   ~CAutoCriticalSection();
private:
   CRITICAL_SECTION *m_pCS;
};

class CEts3DGL
{
   friend class CAutoCriticalSection;
public:     // virtual Memberfunctions
	HWND GetTopLevelParent();
   virtual ~CEts3DGL();
   virtual void      InitGLLists()                   {};
   virtual void      DestroyGLLists()                {};
   virtual void      OnRenderSzene();
   virtual LRESULT   OnTimer(long, BOOL)             {return 0;};
   virtual bool      OnMessage(UINT, WPARAM, LPARAM) {return false;};
   virtual LRESULT   OnCommand(WORD, WORD, HWND)     {return 1;};
   virtual void      AutoDelete();
   virtual void      OnDrawToRenderDC(HDC, int)      {};

   // Memberfunctions
   CEts3DGL();
   CEts3DGL(CEts3DGL *pInit);
   void     Init();
	void     Destructor();
   HWND     Create(LPCTSTR, DWORD, RECT*, HWND);            // pszWndText, dwStyle, rcWnd, hwndParent
   void     Destroy();
   bool     AttachDC(HDC);                                  // hdcInit
   HDC      DetachDC();
   // Setzen der GL-Parameter
   bool     SetModes(DWORD, DWORD);                         // dwSet, dwRemove
   bool     SetSize(SIZE);                                  // szWnd
   void     SetVolume(ETS3DGL_Volume&);
   void     SetVolume(float,float,float,float,float,float); // Left,Top, Right, Bottom, Near, Far
   void     SetAlpha(double);                               // alpha [Degree]
   bool     SetLookAt(ETS3DGL_LookAt&);
   bool     SetRefreshTime(int);                            // nTime [ms]
   void     SetClearBits(UINT);
   void     SetBkColor(COLORREF);
	void     SetValidation(bool);

   // Listen, Erzeugung und Steuerung
   bool     CreateList(int, LISTFUNCTION, int);             // nID, pFunction, nRange
   bool     CreateCommand(int, LISTFUNCTION, int);          // nID, pFunction, nRange
   bool     PerformList(int, bool);                         // nID, (true, false)
   bool     PerformCommand(int, bool);                      // nID, (true, false)
   bool     InvalidateList(int);                            // nID
   bool     InvalidateCommand(int);                         // nID
   void     InvalidateView();
   // Zustandsabfrage
	bool     IsInitialized();
	bool     IsSuspended();
	bool     IsRendering();
   bool     IsBmpFormatAvailable();
	bool     IsValid();
	bool     DoNotValidate();
	bool     IsRenderEvent(long);
   // Windowfunktionen
   LRESULT  SendMessage(UINT, WPARAM, LPARAM);
   BOOL     PostMessage(UINT, WPARAM, LPARAM);
   BOOL     MoveWindow(int, int, int, int, bool);           // left, top, Width, Height, Redraw
   // Koordinatentransformation
   void     TransformPoints(CVector *, int, bool);          // pPoints, nCount, bDir
   // Threadsteuerung und Sicherung
	bool     AttachExternalCS(CRITICAL_SECTION *);
	CRITICAL_SECTION *DetachExternalCS();
   void     ProtectData();
   void     ReleaseData();
   // Zugriff auf Interne Daten
   HWND           GetHWND()         const; 
   HWND           GetThreadHWND()   const {return m_hWndThread;} 
   DWORD          GetThreadID()     const {return m_nThreadID;} 
   HDC            GetHDC()          const {return m_hDC;}
   HPALETTE       GetHPALETTE()     const {return m_hPalette;}
   DWORD          GetModes()        const {return m_nModes;}
   UINT           GetTimerID()      const {return m_nRenderTimer;}
   SIZE           GetViewSize()     const {return m_szView;}
   double         GetAlpha()        const {return m_dAlpha;}
   COLORREF       GetBkColor()      const {return m_cBkGnd;}
   CDibSection*   GetDibSec()       const {return m_pDib;}
   const ETS3DGL_LookAt *GetLookAt()const {return &m_LookAt;}
   const ETS3DGL_Volume *GetVolume()const {return &m_Volume;}

   void ReportGLError();

   // static Memberfunctions
   static void  DrawTriangleFan(CVector*, CVector*, CVector*, ETS3DGL_Fan*);// p1, p2, p3, Fanstruct
   static void  Draw(DWORD, float);                         // dwType, fSize

private:
   HWND              m_hWndThread;     // Windowhandle des OpenGL-Threads
   HDC               m_hDC;            // Gerätekontexthandle
   HPALETTE          m_hPalette;       // Farbpalette
   HANDLE            m_hThread;        // Thread-Handle
   long              m_hEvent;         // Eventhandle zur Threadsyncronisation
   HGLRC             m_hGL_RC;         // Renderkontext
   DWORD             m_nThreadID;      // ThreadID
   DWORD             m_nModes;         // Modi für ETS3D
   volatile DWORD    m_nStates;        // Zustände für CEts3DGL
   UINT              m_nRenderTimer;   // Timer für die Ablaufsteuerung
   UINT              m_nTime;          // Zeit für die Timerwiederholung
   UINT              m_nClearBits;     // Bits für die Funktion glClear()
   SIZE              m_szView;         // Viewgröße
   double            m_dAlpha;         // Öffnungswinkel Alpha
   COLORREF          m_cBkGnd;         // Hintergrundfarbe;
   ETS3DGL_Volume    m_Volume;         // Viewing Volume
   ETS3DGL_LookAt    m_LookAt;         // Betrachterposition für die LookAt Perspektive
   RenderList       *m_pRenderList;    // Zeiger auf Renderlistenstruktur
   RenderList       *m_pCommandList;   // Zeiger auf Renderlistenstruktur
   CDibSection      *m_pDib;           // DibSection zum Rendern
   CRITICAL_SECTION *m_pcsExt;         // Kritische Sektion für externe Daten
   CRITICAL_SECTION  m_csDisplay;      // Kritische Sektion für die Darstellung
protected:
   ETS3DGL_Bits      m_Bits;           // Bits für Color, Depth, Stencil
};

COLORREF GetColorFromIndex(int, float);                        // nIndex, fIntens
int      GetIndexFromColor(COLORREF, float &);                 // color, fIntens
void     InterpolateColors(int, int, COLORREF, COLORREF, COLORREF*);// n1, n2, c1, c2, pColors
void     SetIndexColors(int, int, COLORREF, COLORREF, COLORREF*);   // n1, n2, c1, c2, pColors

#endif // !defined(AFX_CARA3D_H__1F601677_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
