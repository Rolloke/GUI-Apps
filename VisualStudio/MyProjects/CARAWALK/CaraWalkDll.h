// CaraWalkDll.h: Schnittstelle für die Klasse CCaraWalkDll.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARAWALKDLL_H__573CA68A_DDD9_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_CARAWALKDLL_H__573CA68A_DDD9_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ETS3DGL.h"
#include "CVector.h"
#include "RoomDscr.h"
#include "BoxDscr.h"

// Makrodefinitionen
#define WALK_STEP                0.005       // [m]
#define CARA_TYPE                0x41524143
#define CARAWALK_VERSION         110

#define ANGLE_FACTOR               10     // Winkel in 10-tel Gradschritten (int)
#define MAX_ANGLE_PHI            3600     // 360°
#define MAX_ANGLE_XI             3600     // 360°
#define MAX_ANGLE_THETA           800     //  80°

#define AMBIENT_LIGHT            19       // Umgebungslicht einstellung
#define LIST_LIGHT               20       // Listen für Licht (Anzahl Implementationsabhängig)
#define NO_OF_LIGHTS              1       // Anzahl der Lichter

#define PRE_COMMAND              10       // Liste für Commands
#define LIST_ROOM               100       // Raumlisten: Die erste ist die Basisliste, die weiteren sind 
                                          // für je ein Material (Texturen)
#define MAX_ROOMLISTS           899       // maximale Anzahl der Raumlisten (unterschiedliche Texturen)

#define LIST_BOX               1000       // Basisliste für Boxen enthält Listen für unterschiedliche Boxen
                                          // an evtl. mehrere Positionen transformiert und rotiert
#define LIST_SHADOW            1500       // Basisliste für Shatten
#define MAX_BOXLISTS            499       // maximale Anzahl der unterschiedlichen Boxen,
                                          // weitere Listen ab 2000
#define LIST_SCALE             2500       // Liste für die Ausgabe einer Skala
#define LIST_FONT_BASE         3000       // Liste für die Zeichenausgabe
#define LIST_FONT_OFFSET          0
#define LIST_FONT_SIZE          128

#define BOXPOS_X                  4.0     // Position der Box im Box-View
#define BOXPOS_Y                  1.5
#define BOXPOS_Z                  4.0
#define LIGHT_DISTANCE            2.9     // Abstand des Lichtes

#define VIEWPOS_X                 4.0     // Position des Beobachters im Box-View
#define VIEWPOS_Y                 1.5
#define VIEWPOS_Z                 8.5

#define ROOMVIEW                  1       // Nutzung der Klasse zur Darstellung eines Raumes
#define BOXVIEW                   2       // Nutzung der Klasse zur Darstellung der Box
#define SCREENSAVER               3       // Nutzung der Klasse eine Screensavers

#define SHOW_SCALE            0x00000002  // Flag für Anzeige der Größe der Box
#define SHOW_TWICE            0x00000004  // Flag für Anzeige zweier Boxen

#define READ_FILE_ERROR   1
#define ALLOC_ERROR       2
#define FILE_HEADER_ERROR 3
#define WRITE_FILE_ERROR  4

#define BOX_SOUND_DATA        "BSD"
#define BOX_GEOMETRICAL_DATA  "BGD"
#define BOX_MATERIAL_DATA     "BMD"

#define BOX_DATA_FILE_EXT     ".BDD"
#define BOX_DATA_FILE_DIR     "LS_BOX"

#define ROOM_DATA_FILE        "TurnBox.cfg"

#define BACK_SLASH_STR        "\\"
#define BACK_SLASH_SIGN       '\\'

struct BoxDataRed;

class CBoxPropertiesDlg;

class CCaraWalkDll : public CEts3DGL
{
public:
   // public virtual Functions
   virtual ~CCaraWalkDll();
   virtual void      InitGLLists();
   virtual void      DestroyGLLists();
   virtual void      OnRenderSzene();
   virtual LRESULT   OnTimer(long, BOOL);
   virtual bool      OnMessage(UINT, WPARAM, LPARAM);
   virtual LRESULT   OnCommand(WORD, WORD, HWND);
   virtual void      AutoDelete();
   // Constructors
	CCaraWalkDll();
   CCaraWalkDll(CCaraWalkDll *);
// public Functions
// DataExchange
	bool     OnInitTurnBoxData(HANDLE);          // Raum
	bool     OnSetRoomData(void*);               // Raum
	bool     OnSetBoxData(WPARAM, char *);       // Box
	void     OnSetAngle(int, CARAWALK_BoxPos *); // Box

	CMaterial *GetMaterial(char*pszFile, bool bNew);
	CMaterial *GetMaterialFromIndex(int&, CMaterial *pM=NULL);

// public static Functions
   // Heap
	static void  IncrementHeapInstances();
   static void *Alloc(DWORD nSize);
   static void  Free(void*);
   static HANDLE GetHeapHandle();
	static void  HeapDestroy();
	static int   MessageBoxC(long, long , UINT);

   // Helfer
	bool         GetRoomObj();
	void         PressParentDlgButton(int nID, int nTimes);
   HWND         GetParentDlgItem(int);
	LRESULT      SendMessageToParent(UINT, WPARAM,LPARAM);
	LRESULT      PostMessageToParent(UINT, WPARAM,LPARAM);
	void         InitParentMsgWnd(HWND hWnd = NULL);
	void         InitGLSettings();
   static void  GetBmpFile(char*szBmp, BITMAPINFO **ppBmi);
	static void  ReadFileFH(HANDLE, void*, DWORD, CFileHeader*pFH=NULL);
	static void  WriteFileFH(HANDLE, void*, DWORD, CFileHeader*pFH=NULL);
	static bool  IsTriangleNull(CVector &,CVector &,CVector &);
   static       BoxDataRed* GetBoxDB(int &);
	static int   GetBoxMaterialNo(int, BoxDataRed*, char*);
	static void  GetCaraDirectory(int, char*);
	static void  SetBoxPath(char *szFilePath, char *szName);

// public Members
   bool        m_bLButtonDown;
   bool        m_bRButtonDown;
   bool        m_bSetCursor;
	int         m_nViewMode;
// public static Members
   static bool gm_bColorIndex;

private:
   // private Functions
   void  InitCaraWalk();
// Commands
   // Edit
   void  OnEditCopyGrafic();
   // View
   void  OnViewDetail(WORD);
   void  OnViewBoxes(WORD);
   void  OnViewLight(WORD);
   void  OnViewTexture(WORD);
   // Options
   void  OnOptionsBox();
   void  OnOptionsLight();
   // DlgBox Notification
   void  OnChangeLight(WORD, HWND);

   void  OnWalk(WORD, WORD, HWND);
// Messages
	void  OnContextMenu(int, int, HWND);
   void  OnLButtonDown(POINT, UINT);
   void  OnRButtonDown(POINT, UINT);
   void  OnMouseMove(  POINT, UINT);
   void  OnLButtonUp(  POINT, UINT);
   void  OnRButtonUp(  POINT, UINT);
   bool  OnSetCursor(HWND hwndContain, WORD nHittest, WORD wMouseMsg);
   bool  OnHelp(HELPINFO *);
// Cursorsteuerung
   void  SetWalkCursor(bool bCheckPos, bool bResetWaitCursor = false);
   void  CatchCursorPos();
// Menuesteuerung
	void  Check3DControlItem(UINT nIDCommand, UINT nCommand);
// Animation für Bewegungen
	void  SetTurnBoxLight();
   void  SetScreenSaverLight();
   bool  TestWalkStep(CVector*, CVector*, int);
	void  SetEmilsLight();
   void  SetOtherLights(int nL);
// Helferfunktionen
   CMaterial* GetBoxMaterial(int);
	void     DeleteBoxData();
	void     FreeBoxName();

// private static Functions
   // Commandfunktionen OpenGL
   static void LightCommandList(CCaraWalkDll *, int, int);
   // Listenfunktionen OpenGL
	static void LightPositionList(CCaraWalkDll *, int, int);
	static void AmbientLight(CCaraWalkDll *pC, int nID, int nRange);
	static void ScaleList(CCaraWalkDll*pC, int nID, int nRange);
	static void ShadowList(CCaraWalkDll* pC, int nID, int nRange);
	static void LoudspeakerList(CCaraWalkDll *pC, int nID, int nRange);
	static void RoomList(CCaraWalkDll*pC, int nID, int nRange);
	static void RoomListEx(CCaraWalkDll*pC, int nID, int nRange);
	static void PreCommands(CCaraWalkDll *pC, int nID, int nRange);

   static bool ReadBoxHeader(HANDLE hFile, CFileHeader *pfh);

   // private Members
   ETS3DGL_LookAt m_Lap;           // Position und Richtung des Betrachters
   COLORREF    m_cAmbientLight;    // globales Umgebungslicht
   int         m_nTheta, m_nPhi;   // Lichtwinkel
   bool        m_bInitBoxLists;    // Initialisiere Boxenlisten
   bool        m_bInitShadowLists; // Initialisiere Schattenlisten
   float       m_fScaleFactor;     // Scalierungsfaktor für Boxendarstellung
   CVector     m_vScale;           // Scalierungsvector
   int         m_nNoOfLights;      // Anzahl der Lampen
   CGlLight   *m_pLight;           // Beleuchtungsdaten
   CRoomObj   *m_pRoom;            // Raumdaten
   CEtsList    m_Boxes;            // Boxendaten
   bool        m_bTexture;         // Textur (ein, aus) (true, false)
   bool        m_bLight;           // Licht (ein, aus) (true, false)
   bool        m_bDrawBoxes;       // Boxen zeichen
   int         m_nDetail;          // Aufteilungsgrad für Beleuchtung
   bool        m_bLapInvalid;      // Look At Parameter ungültig
   bool        m_bShowScale;       // Skala anzeigen
   bool        m_bGhostMode;       // durch Wände gehen
   bool        m_bFlyMode;         // fliegen
   int         m_nTexFlags;
	double      m_dDistanceFactor;  // Abstandsfaktor für die Material/Chassis Polygone
   char       *m_pszNewBox;
   WPARAM      m_NewBoxwParam;
   HWND        m_hParentMsgWnd;

   CBoxPropertiesDlg*m_pBoxProp;

   // private static Members
   static HANDLE   gm_hHeap;           // eigener Heap
   static int      gm_nHeapInstances;  // Heap-Instanzenzähler
public:
   static bool     gm_bBugFixNormalVector;
   static bool     gm_bBugFixLightTexture;
   static float    gm_fPolygonFactor;
   static int      gm_nPolygonOffset;
   static bool     gm_bTextureObject;
   static bool     gm_bShowShadows;
   static char     gm_szCaraDir[_MAX_PATH];
protected:
	void UpdateBoxLists();
};

#endif // !defined(AFX_CARAWALKDLL_H__573CA68A_DDD9_11D2_9E4E_0000B458D891__INCLUDED_)
