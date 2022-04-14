/*** Prototypen von MEGRA_CF.cpp ***/
BOOL    LoadVideoModi(LPSTR szIniName);
VOID    SaveVideoModi(LPSTR szIniName);
BOOL    ConfigInit(LPSTR INI_Name);
WORD    ConfigSave(LPSTR INI_Name);
BOOL    SetVMCBase(WORD wVMCBase);
BOOL	CheckVMC(void);
VOID    ReleaseAcquisition(HWND hWnd);
BOOL	RegisterWindow(HWND hWnd, WORD wType);

#define CENTRONICS   0x278

/*** Bild und Farbeinstellungen ***/
typedef struct
{
	WORD        wVideoType;        // Kamera, Recorder
    int         nBrightness;       // Helligkeit
    int         nSaturation;       // Farbsättigung
    int         nContrast;         // Kontrast
    int         nHueControl;       // Phase
  } MODE;

typedef MODE *PMODE;

typedef struct
{
	DWORD		dwMagic;		   // Magic Word
	vdDriver	Treiber;
	REGBUFFER	RegBuffer;		   // Zwischenspeicher der Register auf der Megra	
	WORD        wVideoFormat;      // PAL, SECAM, NTSC
    WORD        wSource;           // Eingangsquelle: SRC_FBAS1...SRC_FBAS8, SRC_SVHS
	WORD		wInputType;        // INPUT_VIDEO, INPUT_CODEC
	BOOL		bCodecUsed;	       // TRUE -> Der Codec soll verwendet werden (CodecUsed=YES in MEGRA.INI)
    WORD		wVGABPP;		   // Zahl der Farben (2^n) 
    
    /*** Keyfarben ***/
	WORD		wColorKey;		   // PseudoColorKey bzw. HiColorKey
	WORD		wKeyMask;          // Keymaske
	
	/*** Delays und Offsets ***/
    WORD        wVideoDispWinSkewX;// Offset to add to display window registers (für Videobetrieb)
    WORD        wVideoDispWinSkewY;//   "
    WORD        wCodecDispWinSkewX;// Offset to add to display window registers
    WORD        wCodecDispWinSkewY;//   "
    WORD        wHRefOutDelay;     // HREFOUT Delay                       (VMCplus)
    WORD        wVideoSelDelay;    // EnableVideoSel Delay                (VMCplus)
	WORD        wColourKeyDelay;   // Colourkeying Delay                  (VMCplus)
	WORD        wSclkBoost;        // Number of fast SCLK before zooming  (VMCplus)
	WORD		wPllDiv;		   // PLL-Teiler 
	WORD		wHScaleCorrection; // Skalierungskorrektur bei PLL-Betrieb

    /*** Einleseausschnitte, formatabhängig ***/
	RECT		rcPALAcqWindow;
	RECT		rcNTSCAcqWindow;
	RECT		rcENCODERAcqWindow;
	RECT		rcDECODERAcqWindow;	
	RECT		rcOutputWindow;    // Ausgabefenster

	/*** Zugriffmodi auf das VRAM ***/
	WORD		wVRAMMode;		   // auto oder manuell
	WORD		wManuellMode;      // 0...15 (0 = I/O-Zugriff, 1...15 VRAM-Adresse in MByte)
	WORD		wVRAMAccess;	   // MEM_ACCES / IO_ACCESS
		
    /*** Einstellungen für jede der 9 Eingangsquellen ***/
    MODE		Mode[SRC_MAX];      // Einstellungen für jede Quelle

	HWND		hWnd[WIN_MAX];		// Array mit Registrierten Fensterhandeln.

	/*** Instanzenzähler der DLL ***/
	int			nInstNr;

	DWORD		dwWindowsVersion;	// Windowsversion (WIN95/WinNT...)
}CfgStruct;

