// Cara3D1.h: Schnittstelle für die Klasse CEts3DGLDll.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA3D1_H__88440B20_CFFA_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_CARA3D1_H__88440B20_CFFA_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Ets3dGLRegKeys.h"
#include "EtsAppWndMsg.h"

#define CETS3DGL_CLASSNAME     "ETS3DGL_CLASS"
#define CETSGLPARENT_CLASSNAME "ETSGL_PARENTCLASS"
#define OLD_WINDOW_PROC        "OldWindowProc"

#define CONTROL_TIMER_EVENT    129457             // Timer
#define USE_DIRECT_INPUT        0

#define RAD10(A)   ((A)*1.74532925199e-3)    // Umrechnung von GRAD*10 in RAD 
#define RAD(A)     ((A)*1.74532925199e-2)    // Umrechnung von GRAD in RAD 
#define GRAD(A)    ((A)*57.2957795131)       // Umrechnung von RAD in GRAD 
#define INV_256(A) ((A)*0.00390625f)

// Flags für Modi
#define ETS3D_CM_RGB                      0x00000001           // RGB-Mode
#define ETS3D_CM_4096                     0x00000002           // 4096 Farben
#define ETS3D_CM_256                      0x00000004           // 256 Farben
#define ETS3D_CM_CLEAR                    0x00000007           // Löschflags

#define ETS3D_DC_DIRECT                   0x00000030           // Direkt in einen DC Rendern
#define ETS3D_DC_DIRECT_1                 0x00000010           // Direkt in einen DC Rendern
#define ETS3D_DC_DIRECT_2                 0x00000020           // Direkt in einen DC Rendern (double buffer)
#define ETS3D_DC_DIB                      0x000000c0           // Rendern in eine DIB-Section
#define ETS3D_DC_DIB_1                    0x00000040           // Rendern in eine DIB-Section
#define ETS3D_DC_DIB_2                    0x00000080           // Rendern in zwei DIB-Section (double buffer)
#define ETS3D_DC_CLEAR                    0x000000f0           // Löschflags

#define ETS3D_PR_ORTHOGONAL               0x00000100           // orthogonale Projektion
#define ETS3D_PR_PERSPECTIVE              0x00000200           // perspektivische Projektion definiert durch Öffnungswinkel alpha
#define ETS3D_PR_FRUSTUM                  0x00000400           // perspektivische Projektion definiert durch Darstellungsvolumen
#define ETS3D_PR_LOOK_AT                  0x00000800           // LookAt Perspektive: kombinierbar mit jeweils einem der anderen Flags
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
#define ETS3D_THREADWND_NOCHILD           0x00400000           // Threadwindow ist kein Child
#define ETS3D_DESTROY_AT_DLL_TERMINATION  0x00800000           // erst beenden, wenn die Dll aus dem Process entfernt wird
#define ETS3D_KEYBOARD_INPUT              0x01000000           // Keybord Abfrage
#define ETS3D_DIRECTX_JOYSTIC             0x02000000           // DirectX Joystic Abfrage
#define ETS3D_POST_OPENGL_STATE           0x30000000           // Sende Nachricht über (nicht) initialisiert zurück
#define ETS3D_OPENGL_STATE_SHIFT          28                   // Shift um das Parent-Window zu bestimmen: n Stufen höher

// Flags für States
#define OPEN_GL_INITIALIZED               0x00000001           // OpenGL Initialisiert
#define OPEN_GL_INIT_FAILED               0x00000002           // nicht Initialisiert
#define CS_DIB_INITIALIZED                0x00000004           // Kritische Sektion für DibSection
#define CS_EXT_INITIALIZED                0x00000008           // Kritische Sektion für externe Daten
#define CLEAR_INIT                        0x0000000f           // Löschflags

#define INVALID_SIZE                      0x00000010           // Durch die Funktion OnSize verändert
#define INVALID_VIEW                      0x00000020           // Durch die Funktion OnPaint verändert
#define INVALID_VOLUME                    0x00000040           // Wird fast nur am Anfang gesetzt
#define INVALID_LISTS                     0x00000080           // eine oder mehrere der Listen sind ungültig
#define INVALID_COMMANDS                  0x00000100           // eine oder mehrere der Commands sind ungültig
#define INVALID_BKGND                     0x00000200           // Hintergrundfarbe ist ungültig
#define DO_NOT_VALIDATE                   0x00000800           // Nicht Validieren
#define CLEAR_INVALID                     0x000007f0           // Löschflags

#define RENDERING_NOW                     0x00001000           // Die Bilddaten werden gerendert
#define TRANSFORMING_NOW                  0x00002000           // Punkte werden Transformiert
#define COPY_FRAME_BMP_NOW                0x00004000           // Der Framebuffer wird kopiert
#define COPY_FRAME_DC_NOW                 0x00008000           // Der Framebuffer wird kopiert
#define CLEAR_NOW                         0x0000f000           // Löschflags

#define TIME_CHANGED                      0x00010000           // Updatezeit wurde verändert
#define MODE_CHANGED                      0x00020000           // Modi wurden verändert
#define CLEAR_CHANGED                     0x000f0000           // Löschflags

#define RENDERING_SUSPENDED               0x00100000           // Es darf nicht gerendert werden
#define BMP_FORMAT_AVAILABLE              0x00200000           // Bitmap-Rendern möglich
#define CRITICAL_SECTION_ATTACHED         0x00400000           // Externe Critical Section gesetzt
#define HARDWARE_ACCELERATION_AVAILABLE   0x00800000           // Hardware beschleunigung möglich
#define IS_INSIDE_MENU_LOOP               0x01000000           // Fenster befindet sich in der Menüschleife
#define DELETE_DC                         0x02000000           // DC muß Deleted werden, sonst Released
#define NO_WM_DESTROY                     0x04000000           // Fenster muß nicht mehr zerstört werden

#define INVALID_LIST                      0x00000011           // Invalidieren der Listen
#define INVALID_COMMAND                   0x00000022           // Invalidieren der Listen
#define RENDER_LIST                       0x00000031           // Rendere Liste
#define DONT_RENDER_LIST                  0x00000041           // Rendere Liste nicht
#define RENDER_COMMAND                    0x00000052           // Rendere Command
#define DONT_RENDER_COMMAND               0x00000062           // Rendere Command nicht

// weitere Flags
#define ETS3D_WIRE_FRAME                  0x00010000
#define ETS3D_SOLID_FRAME                 0x00020000
#define ETS3D_SPHERE                      0x00100000
#define ETS3D_CUBE                        0x00200000

#define ETS3D_CD_CHANGEMODE               10
#define ETS3D_CD_INITIALIZED              11
#define ETS3D_CD_SZENE_READY              12
#define ETS3D_CD_COPY_FRAME_BMP           13

#define PFD_DRAW_TO_BITMAP_DONTCARE       0x10000000           // PixelFormat Erweiterung

#define MAX_TRIANGLES 8

#define GWL_PARENTWINDOW   4
#define GWL_THREADWINDOWID 8
#define THREADWINDOWID     0x48375936

class  CDibSection;
class  CVector;
class  CEts3DGLDll;

typedef void (*LISTFUNCTION)(CEts3DGLDll*, int, int);
typedef void (*FANFUNCTION)(int, CVector *, void *);

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

struct RenderList
{
   bool         bInvalid,           // Liste ungültig, muß neu gerendert werden
                bRender;            // Liste darf gerendert werden
   LISTFUNCTION pfnRender;          // Renderfunktion
   LISTFUNCTION pfnDirect;          // Renderfunktion für Befehl nach Aufruf der Liste
   int          nListID, nRange;    // Listen ID und Bereich der dazugehörenden Listen
   struct RenderList* pNext;        // Zeiger auf nächste Liste
};

#ifdef _MULTI_3DWINDOWS
struct PropertyWindowFunc
{
   long pfnWindowFunc;
   long nCount;
   CEts3DGLDll *p3DGL[1];
};
#else
struct PropertyWindowFunc
{
   long pfnWindowFunc;
   CEts3DGLDll *p3DGL;
};
#endif

class CAutoCriticalSection
{
public:
   CAutoCriticalSection(CRITICAL_SECTION * pCS);
   ~CAutoCriticalSection();
private:
   CRITICAL_SECTION *m_pCS;
};

class CEts3DGLDll
{
public:
   CEts3DGLDll() {};
   virtual ~CEts3DGLDll()                            = 0;
   virtual void      InitGLLists()                   = 0;
   virtual void      DestroyGLLists()                = 0;
   virtual void      OnRenderSzene()                 = 0;
   virtual LRESULT   OnTimer(long, BOOL)             = 0;
   virtual bool      OnMessage(UINT, WPARAM, LPARAM) = 0;
   virtual LRESULT   OnCommand(WORD, WORD, HWND)     = 0;
   virtual void      AutoDelete()                    = 0;
   virtual void      OnDrawToRenderDC(HDC, int)      = 0;

// Konstruktion - Destruction
	HWND    Create(LPCTSTR lpWindowName, DWORD dwStyle, RECT *prect, HWND hwndParent);
// Datenzugriff
   HWND GetParent();
   HWND GetTopLevelParent();

private:
// Fenstergröße - Zeichnen
   LRESULT OnSizeTH(UINT nType, int cx, int cy);
	LRESULT OnPaintTH(PAINTSTRUCT&);
	LRESULT OnPaint(PAINTSTRUCT&);
// Listen
   static RenderList* CreateRenderListR(RenderList *, int, LISTFUNCTION,int);
	void        OnValidateLists();
	void        OnValidateCommands();
   static void ValidateR(RenderList *, void*);
	static bool SetListR(RenderList*, int, UINT);
	static void RenderSceneR(RenderList*, void*);
	void        DestroyCallLists();
// Steuerung des Threads
	bool     StartRenderThread(HWND);
   LRESULT  OnTimerDll(long, TIMERPROC*);
   BOOL     KillTimer(UINT&);
   BOOL     PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
   LRESULT  ClassWindowFunc(HWND, UINT, WPARAM, LPARAM);
 	bool     AttachThread(HWND);
   static   LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
// OpenGL : Threadfunktionen
   bool     InitGL();
   int      FindPixelFormat(HDC, PIXELFORMATDESCRIPTOR &);
	int      SetDCPixelFormat(HDC, DWORD, bool);
	void     DestroyGL();
	void     OnSetViewPort();
	void     OnSetViewVolume();
	void     OnRender();
   void     ClearBuffer();
	void     OnTransformPoints(WPARAM, CVector *);
	void     OnCopyFrameBuffer(WPARAM, LPARAM);
	void     AttachThreadStates();
	void     DetachThreadStates();
	static unsigned __stdcall OpenGLThread(void*);
   static LRESULT CALLBACK ThreadWindowFunc(HWND, UINT, WPARAM, LPARAM);
	static void RenderSingleDib(CEts3DGLDll*);
	static void RenderDoubleDib(CEts3DGLDll*);
	static void RenderSingleDirect(CEts3DGLDll*);
	static void RenderDoubleDirect(CEts3DGLDll*);
// Zustände
   bool     IsRendering()    {return (m_nStates & RENDERING_NOW)       ? true : false;};
   bool     IsSuspended()    {return (m_nStates & RENDERING_SUSPENDED) ? true : false;}; 
   bool     IsInitialized()  {return (m_nStates & OPEN_GL_INITIALIZED) ? true : false;};
   bool     DoNotValidate()  {return (m_nStates & DO_NOT_VALIDATE)     ? true : false;};
// Helfer
	DWORD       WaitForThreadEvent();
	LRESULT     OnCommandDll(WORD, WORD, HWND);
	void        ChangeModes(DWORD);
	static void MsgBoxGlError();
	static void ErrorMessage(char*);
	static BOOL CALLBACK FindWindow(HWND hwnd, LPARAM lParam);
	static HWND FindTopLevelParent(HWND);
// DirectX
#if USE_DIRECT_INPUT
	static void DirectXInit(HWND);
	static void DirectXTerminate();
	static void DirectXInput(HWND);
	static void DirectXError(HRESULT);
#endif
public:
// Schnittstellen
	void        KillRenderThread();
	bool        CreateRenderListDll(int nID, LISTFUNCTION,int,bool);
	void        OnRenderSzeneDll();
	bool        SetListDll(int, UINT);
	void        TransformPointsDll(CVector*, int, bool);
   bool        DestroyAtDllTermination();
	HWND        DetachThread();
   LRESULT     OnDestroy();
   static void InitRegistryKeys();
	static void UnregisterClass();

private:
   HWND              m_hWndThread;     // Windowhandle des OpenGL-Threads
   HDC               m_hDC;            // Gerätekontexthandle
   HPALETTE          m_hPalette;       // Farbpalette
   HANDLE            m_hThread;        // Thread-Handle
   HANDLE            m_hEvent;         // Eventhandle für den Thread
   HGLRC             m_hGL_RC;         // Renderkontext
   DWORD             m_nThreadID;      // ThreadID
   DWORD             m_nModes;         // Modi für CEts3DGLDll
   volatile DWORD    m_nStates;        // Zustände für CEts3DGLDll
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
   ETS3DGL_Bits      m_Bits;           // Bits für Color, Depth, Stencil

public:
   static HINSTANCE    gm_hInstance;
   static CEts3DGLDll *gm_pFirstObjectInstance;
   static bool         gm_bInitRegistryValues;
private:
	void PostOpenGLState(WPARAM, LPARAM);
   static ATOM       gm_ETS3D_Class;
   static ATOM       gm_ETS3DParentClass;
   static bool       gm_bBugFixMoveWindow;
   static bool       gm_bBugFixBufferSize;
   static int        gm_nBugFixBuffer;
   static bool       gm_bNoGraphic;

#ifdef _DEBUG
   static LPDIRECTINPUT       gm_pdi;
   static LPDIRECTINPUTDEVICE gm_pKeyboard;
#endif
};

#endif // !defined(AFX_CARA3D1_H__88440B20_CFFA_11D2_9E4E_0000B458D891__INCLUDED_)
