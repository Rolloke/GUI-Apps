/******************************************************************************/
/* Projekt: CMEGRA                                                            */
/* Datei:   CMEGRA.H                                                          */
/* Zweck:   Headerdatei für die Aplikationen. Sie enthält die exportierte     */
/*          Klasse CMegra, sowie alle wichtigen Bezeichner.                   */
/******************************************************************************/
#ifndef __CMEGRA_H__
#define __CMEGRA_H__
#include "wk_msg.h"
#include "win16_32.h"
#include "cmymutex.h"

/*** Minimal & Maximalwerte für Get/SetAdjustFactor ***/
#define DISPWINX_MIN            0x0000
#define DISPWINX_MAX            0x03ff
#define DISPWINY_MIN            0x0000
#define DISPWINY_MAX            0x03ff
#define DISPADDRX_MIN           0x0000
#define DISPADDRX_MAX           0x03ff
#define DISPADDRY_MIN           0x0000
#define DISPADDRY_MAX           0x03ff

/*** Maximalwerte für HREFOUT-Delay, VIDSEL-Delay, COLOURKEY-Delay, EXTERN-KEY-Delay ***/
#define HREFOUTDELAY_MIN        0x0001
#define HREFOUTDELAY_MAX        0x000e
#define VIDEOSELDELAY_MIN       0x0000
#define VIDEOSELDELAY_MAX       0x00ff
#define COLOURKEYDELAY_MIN      0x0000
#define COLOURKEYDELAY_MAX      0x001f
#define SCLKBOOST_MIN           0x0000
#define SCLKBOOST_MAX           0x007f

/*** Minimal & Maximalwerte für Contrast, Brightness, Saturation, Hue, GainColor, KeyColor ***/
#define CONTRAST_MIN            -256
#define CONTRAST_MAX            +255
#define BRIGHTNESS_MIN          -256
#define BRIGHTNESS_MAX          +255
#define SATURATION_MIN          -128
#define SATURATION_MAX          +127
#define HUE_MIN                 -128
#define HUE_MAX                 +127
#define COLORKEY_MIN             0
#define COLORKEY_MAX            +255
#define HICOLORKEY_MIN			 0
#define HICOLORKEY_MAX          +32767

/*** Minimal & Maximalwerte für den PLL-Teiler ***/
#define PLLDIV_MIN				0x012c
#define PLLDIV_MAX				0x0400

/*** Maximalwerte für das Acquisitionfenster ***/
#define ACQXRANGE               0x0400
#define ACQYRANGE               0x0400
#define ACQWRANGE               0x0400
#define ACQHRANGE               0x0400

/*** Maximale Anzahl von registrierten Fenstern ***/
#define WIN_MAX					0x0010

/*** Mögliche Typparameter für RegisterWindow ***/
#define REGISTER_WINDOW			0x0001
#define UNREGISTER_WINDOW		0x0002
				
/*** Mögliche Parameter beim Öffnen der Dialoge ***/
#define GERMAN					0x0001
#define ENGLISH					0x0002

/*** Mögliche Rückgabeparameter von GetVGAMode() ***/
#define PSEUDOCOLOR				0x0001
#define HICOLOR					0x0002

/*** Mögliche Typparameter für Read/WriteVLUT ***/
#define VLUT_LUM                0x0001
#define VLUT_CHROM              0x0002

/*** Mögliche Typparameter für Get/SetUMASize ***/
#define UMA_16K                 0x0001
#define UMA_32K                 0x0002
#define UMA_64K                 0x0003
#define UMA_1M                  0x0004

/*** Mögliche Parameter für SetVLUTMode ***/
#define VLUT_IN_USE             0x0001
#define VLUT_BYPASS             0x0002

/*** Mögliche Typparameter für SetVRAMAccessMode ***/
#define MANUELL_MODE            0x0001
#define AUTO_MODE               0x0002

/*** Mögliche Zugriffsarten auf das VRAM ***/
#define MEM_ACCESS              0x0001
#define IO_ACCESS               0x0002

/*** Mögliche Parameter für Get/SetAdjustFactor ***/
#define ADJ_NULL                0x0000
#define ADJ_DISPWINX            0x0001
#define ADJ_DISPWINY            0x0002
#define ADJ_HREFOUTDELAY        0x0005
#define ADJ_VIDEOSELDELAY       0x0006
#define ADJ_COLOURKEYDELAY      0x0007
#define ADJ_SCLKBOOST           0x0008
#define ADJ_COUNT               0x0009

/*** Mögliche Parameter für SetLuminance und GetLuminance ***/
#define BRIGHTNESS              0x0000
#define SATURATION              0x0001
#define CONTRAST                0x0002

/*** Mögliche Parameter für SetFormat ***/
#define FMT_PAL                 0x0000
#define FMT_NTSC                0x0001

/*** Mögliche Parameter für SetInputSource (Mit SRC_CODEC kann NICHT auf Codecbetrieb ***/
/*** geschaltet werden, hierzu dient die Funktion SetInputType(...) ***/
#define SRC_FBAS1               0x0000
#define SRC_FBAS2               0x0001
#define SRC_FBAS3               0x0002
#define SRC_FBAS4               0x0003
#define SRC_SVHS1               0x0004
#define SRC_SVHS2               0x0005
#define SRC_CODEC				0x0006
#define SRC_MAX                 0x0007
#define SRC_CURRENT             0x00ff

/*** Mögliche Parameter für SetInputType ***/
#define INPUT_VIDEO				0x0000
#define INPUT_CODEC				0x0001

/*** Mögliche Parameter für SetInputType ***/
#define TYPE_CAM                0x0000
#define TYPE_REC                0x0001

/*** Mögliche Parameter für SetDecoderModus ***/
#define ENC_DEFAULT             0x0000
#define ENC_CIF                 0x0001
#define ENC_QCIF                0x0002
#define ENC_SIF                 0x0003
#define ENC_QSIF                0x0004
#define ENC_USER                0x0005

/*** Fehlermeldungen (Funktion GetVDPIError) ***/
#define VDE_GETERROR            0x0001       // Liefere letzten Fehler
#define VDE_CLEARERROR          0x0002       // Setzt den internen Fehlerstatus auf VDE_OK
#define VDE_OK                  0x0003       // Kein Fehler
#define VDE_PARAM               0x0004       // Unbekannter Parameter
#define VDE_RANGE               0x0005       // Parameter außerhalb gültiger Grenzen
#define VDE_SOURCE              0x0006       // Nicht existierende Eingangsquelle
#define VDE_BASE                0x0007       // Falsche Basisadresse des VMCplus
#define VDE_VMC                 0x0008       // Zugriff auf VMCplus nicht möglich
#define VDE_VERSION             0x0009       // Falsche Versionsnummer des VMCplus             
#define VDE_MEMLOW				0x000a       // Weniger als 4 MByte Speicher verfügbar
#define VDE_COLORS				0x000b		 // Falsche Farbzahl   0
#define VDE_NOCODEC				0x000c		 // Kein Codec installiert

/*** Mögliche Typ-Parameter für SetStartAddress/GetStartAddress ***/
#define READ_ODD                0x0000       // Startadresse read odd 
#define READ_EVEN               0x0001       // Startadresse read even
#define WRITE_ODD               0x0002       // Startadresse write odd
#define WRITE_EVEN              0x0003       // Startadresse write even

/*** Mögliche Typ-Parameter für GetMemLineInc ***/
#define READ_INC  		        0x0000       // Line-Increment read
#define WRITE_ODD_INC           0x0001       // Line-Increment write odd
#define WRITE_EVEN_INC          0x0002       // Line-Increment write even

/*** Mögliche Typ-Parameter für SetScale/GetScale ***/
#define HORZ_SCALE_ODD          0x0000       // Horizontale Skalierung odd-field
#define HORZ_SCALE_EVEN         0x0001       // Horizontale Skalierung even-field
#define VERT_SCALE_ODD          0x0002       // Vertikale Skalierung odd-field
#define VERT_SCALE_EVEN         0x0003       // Vertikale Skalierung even-field

/*** Mögliche Typ-Parameter für SetZoom/GetZoom ***/
#define HORZ_ZOOM               0x0000       // Horizontaler Zoom
#define VERT_ZOOM               0x0001       // Vertikaler Zoom

/*** Mögliche Typ-Parameter für SetWindow/GetWindow ***/
#define ACQ_ODD                 0x0000       // Acquisitionwindow odd
#define ACQ_EVEN                0x0001       // Acquisitionwindow even
#define DISPLAY                 0x0002       // Displaywindow
#define OUTPUT					0x0003       // Ausgabefenster

/*** Mögliche Parameter für SetDisplayMode ***/
#define DISP_DVS                0x0001       // Update display register with DVS
#define DISP_DHS                0x0002       // Update display register with DHS
#define DISP_NONINTERLACED      0x0004       // Display set to noninterlaced
#define DISP_INTERLACED         0x0008       // Display set to interlaced
#define DISP_512BIT             0x0010       // Set VRAM serial register to 512 Bit
#define DISP_256BIT             0x0020       // Set VRAM serial register to 256 Bit

/*** Mögliche Parameter für SetAcqMode ***/
#define ACQ_CPU_VRAM            0x0001       // CPU access: select VRAM
#define ACQ_CPU_DRAM            0x0002       // CPU access: select DRAM
#define ACQ_WRITE_IN_VRAM       0x0004       // Video data: write into VRAM
#define ACQ_WRITE_IN_DRAM       0x0008       // Video data: write into DRAM
#define ACQ_NORMAL_WRITE        0x0010       // VRAM write: normal write
#define ACQ_USE_WRITEMASK       0x0020       // VRAM: use writemask
#define ACQ_UPDATE_WITH_VS      0x0040       // Update VdAcq register with VS
#define ACQ_UPDATE_WITH_HREF    0x0080       // Update VdAcq register with HREF
#define ACQ_DISABLE_ODDFIELD    0x0100       // Video data disable writing on odd field
#define ACQ_ENABLE_ODDFIELD     0x0200       // Video data enable writing on odd field
#define ACQ_DISABLE_EVENFIELD   0x0400       // Video data disable writing on even field
#define ACQ_ENABLE_EVENFIELD    0x0800       // Video data enablewriting on even field

/*** Mögliche Parameter für SetCKeyMode ***/
#define OUT_DELAY               0x0001       // Out delay für das Colour keying
#define DISABLE_COLKEY          0x0002       // Disable Colour keying
#define ENABLE_PSEUDOCOLKEY     0x0004       // Enable pseudo colour keying
#define ENABLE_HICOLKEY         0x0008       // Enable HiColour keying
#define DISABLE_CHROMAKEY       0x0010       // Disable chroma keying
#define ENABLE_CHROMAKEY        0x0020       // Enable chroma keying

/*** Mögliche Parameter für SetFilterMode ***/
#define C_FILTER_IN_USE         0x0001       // Chrominancefilter in use               
#define C_FILTER_BYPASS         0x0002       // direct bypass
#define DIRECT_UPSAMPLING       0x0004       // direct upsampling without sampling 
#define Y_FILTER_BYPASS         0x0008       // Y decimation filter bypass 
#define Y_FILTER_IN_USE         0x0010       // Y decimation filter
             
/*** Mögliche Parameter für SetFilter ***/
#define HORZ_FILTER1            0x0001
#define HORZ_FILTER2            0x0002
#define HORZ_FILTER3            0x0003
#define HORZ_FILTER4            0x0004
#define HORZ_FILTER5            0x0005
#define HORZ_FILTER6            0x0006
#define HORZ_FILTER7            0x0007
#define HORZ_DELAY              0x0008
#define HORZ_BYPASS             0x0009
#define VERT_FILTER1            0x000a
#define VERT_FILTER2            0x000b
#define VERT_DELAY              0x000c
#define VERT_BYPASS             0x000d

/*** Mögliche Parameter für GetFilter ***/
#define HORIZONTAL              0x0001
#define VERTICAL                0x0002

/*** Mögliche Parameter für SetMatrixMode ***/
#define MATRIX_IN_USE           0x0001       // Colour space matrix mode in use
#define MATRIX_BYPASS           0x0002       // Colour space matrix mode in bypass

// Mögliche Parameter der Funktion Bitmap();                               
#define DIBTRUECOLOR    		0x0001 // DEST_DIB24 in Convert.h
#define DIB8BIT					0x0002 // DEST_DIB8	 in Convert.h

#define	WM_CHANGED_SOURCE			MEGRA_MSGBASE	 + 0x0001
#define	WM_CHANGED_INPUTTYPE        MEGRA_MSGBASE	 + 0x0002
#define	WM_CHANGED_VIDEOTYPE        MEGRA_MSGBASE	 + 0x0003
#define	WM_CHANGED_VIDEOFORMAT      MEGRA_MSGBASE	 + 0x0004
#define	WM_CHANGED_DISPLAYWINDOW    MEGRA_MSGBASE	 + 0x0005
#define	WM_CHANGED_OUTPUTWINDOW     MEGRA_MSGBASE	 + 0x0006
#define	WM_CHANGED_ACQWINDOW        MEGRA_MSGBASE	 + 0x0007
#define	WM_CHANGED_FREEZE           MEGRA_MSGBASE	 + 0x0008
#define	WM_CHANGED_ZOOM             MEGRA_MSGBASE	 + 0x0009
#define	WM_CHANGED_COLORKEY         MEGRA_MSGBASE	 + 0x000a
#define	WM_CHANGED_BRIGHTNESS       MEGRA_MSGBASE	 + 0x000b
#define	WM_CHANGED_CONTRAST         MEGRA_MSGBASE	 + 0x000c
#define	WM_CHANGED_SATURATION       MEGRA_MSGBASE	 + 0x000d
#define	WM_CHANGED_HUE              MEGRA_MSGBASE	 + 0x000e
#define	WM_CHANGED_JUST             MEGRA_MSGBASE	 + 0x000f
#define WM_CHANGED_FILTER			MEGRA_MSGBASE	 + 0x0010

#define MEGRA_INI					"MegraV2.ini"

/////////////////////////////////////////////////////////////////////////////
// CMegra window
class __export CMegra
{

// Construction
public:
	CMegra();
	//     Initialisiert die Karte, sofern noch nicht geschehen, anhand der
	//     Inidatei MEGRA.INI. Dabei werden folgende Punkte durchgeführt 
	//			- I/O-Basisadresse setzen (VMCplus, MERGE-DAC, Kontrollregister) 
	//			- Initialisierung der Register des VMCplus                      
	//			- Initialisierung der Register der I2C-Bausteine                
	//			- Initialisierung der Register des MERGE-DACs 
	//			- Initialisiert das Kontrollregister                    
	//			- Sync-polatität ermitteln
	//			- Videoquelle setzen                                    
	//			- Laden des Videoformats (PAL, Secam, NTSC)
	//			- Laden des Acquisitionfensters (Größe des Videobildes)         
	//			- Laden der Delayzeiten, Justierungen
	//			- Laden der Farb-, Helligkeits- und Kontrasteinstellungen     
	//			- Laden des Videotypes (Kamera, Recorder)                                
	//		Die letzten 2 Punkte sind für jede der Eingangsquellen
	//		(FBAS1...FBAS8, SVHS, CODEC) getrennt einstellbar.
	//		Die Delayzeiten und die Bildjustierungen, sind 
	//		für 16 unterschiedliche Bildschirmmodi einstellbar, wobei
	//		der Bildschirmmodus anhand der aktuellen Bildschirmauflösung
	//		erkannt wird. Die Größe des Acquisitionsfenster ist für jedes 
	//		der drei Videoformate (PAL, SECAM, NTSC) getrennt einstellbar.

	CMegra(HWND hWnd);
	// Dieser Konstruktor kann verwendet werden, wenn zum Zeitpunkt des Anlegens eines
	// CMegra-Objektes schon ein Fensterhandle zur Verfügung steht,
	
	BOOL RegisterWindow(HWND hWnd);			
	// Registriert ein Fenster bei der MEGRA.DLL. Ein solches Fenster erhält Nachrichten
	// über etwaige Änderungen der Einstellungen.
	// hWnd ist ein Fensterhandle, an die die MEGRA.DLL seine Nachrichten schicken soll.
    // Abgemeldet wird ein Fenster im Destruktor von CMegra.
    
	BOOL IsValid();
	// Liefert TRUE, wenn die Megra korrekt initialisiert wurde, ansonsten FALSE.

	WORD GetMegraVersion(void);
	// Liefert die Versionsnummer der MEGRA.DLL (101 steht z.B. für Version 1.01)                    

	WORD GetMegraError(WORD wNewError);                                    
	//	Zweck:     Liefert den Fehlercode des zuletzt aufgetretenen Fehlers.            
	//	Parameter: wNewError = VDE_GETERROR   -> Liefert die Fehlerkennung des letzten Fehlers.                               
	//	                       VDE_CLEARERROR -> Löscht den Fehlerstatus                
	//	Rückgabe:  VDE_PARAM   -> Unbekannter Parameter                                
	//	           VDE_RANGE   -> Parameter liegt außerhalb gültiger Grenzen           
	//	           VDE_SOURCE  -> Unbekannte Eingangsquelle
	//	           VDE_BASE    -> Falsche Basisadresse des VMCplus
	//	           VDE_VMC     -> Zugriff auf VMCplus nicht möglich
	//	           VDE_OK      -> Kein Fehler entdeckt                                 
	//	           VDE_MEMLOW  -> Weniger als 4 MByte vorhanden
	//	           VDE_VERSION -> Falsche Kontrollerversionsnummer
	//	           VDE_COLORS  -> Falche Farbtiefe (!= 8Bit, != 15Bit != 16Bit)		
 
	    	
	WORD SetInputType(WORD wType);
	// Wählt den Quellentyp aus: INPUT_CODEC oder INPUT_VIDEO, Zurückgeliefert wird der
	// bisherige Quellentyp

	WORD GetInputType();
	// Liefert den zur Zeit gültigen Quellentyp: INPUT_CODEC oder INPUT_VIDEO
	
	WORD SetVideoFormat(WORD wFormat);
	// Setzt das Videoformat (PAL, SECAM, NTSC)
	// Zurückgeliefert, wird das bisherige Videoformat
	
	WORD GetVideoFormat();
	// Liefert das Videoformat (PAL, SECAM, NTSC)
	
	WORD SetVideoType(WORD wType);                                
	// Setzt den Videotyp der aktuellen Quelle
	// wType   = TYPE_CAM -> Camera, TYPE_REC -> Rekorder
   	// Zurückgeliefert, wird der bisherige Videotyp

	WORD GetVideoType(WORD wSource);
	// Liefert für die gewünschte Quelle (SRC_CURRENT = aktuelle Quelle) 
	// die Art der Eingangsquelle ( TYPE_CAM -> Kamera, TYPE_REC -> Recorder)
		
	WORD SetInputSource(WORD wNewInput);                  
	// Wählt die Eingangsquelle aus: SRC_FBAS1...SRC_FBAS8, SRC_SVHS
	// Zurückgeliefert, wird die bisherige Quelle

	WORD GetInputSource();                  
	// Liefert die z.Z. verwendete Eingangsquelle: SRC_FBAS1...SRC_FBAS8, SRC_SVHS

	void SetDisplayWindow(const LPRECT lpRect);
	// Setzt die größe des Displaywindows

	void GetDisplayWindow(LPRECT lpRect);
	// Liefert die Größe des Displaywindows

	void SetOutputWindow(const LPRECT lpRect);
	// Setzt die größe des Outputwindows

	void GetOutputWindow(LPRECT lpRect);
	// Liefert die Größe des Outputwindows

	void SetAcqWindow(const LPRECT lpRect);
	// Setzt die größe des Acquisitionsfensters

	void GetAcqWindow(LPRECT lpRect);
	// Liefert die Größe des Acquisitionfensters
    
	void Freeze(BOOL bFreeze);
	// Friert das Bild ein (TRUE), oder taut es wieder auf (FALSE)

    WORD SetZoom(WORD wType, WORD wZoom);
    // Setzt den horizontalen oder vertikalen Zoomfaktor
    // wType=HORZ_ZOOM, VERT_ZOOM
    // wZoom =1, 2, 4, 8
	// Zurückgeliefert wird der aktuelle Zoomfaktor.
	
    WORD GetZoom(WORD wType);
    // Liefert den horizontalen oder vertikalen Zoomfaktor
    // wType=HORZ_ZOOM, VERT_ZOOM
	// Rückgabe = 1, 2, 4, 8
	                                                   
	void SaveBitmap(UINT nDestFormat);
	// Speichert das Outputfenster als DIB ab.
	
	BOOL GetBitmap(UINT nDestFormat, LPCRECT lpRect, HGLOBAL &hBitmap, DWORD &dwSize);
	// Liefert Adresse und Länge der ausgelesenen Bitmap

	void SaveConfig();
	// Speichert die aktuellen Einstellungen unter MEGRA.INI ab

	COLORREF SetColorKey(WORD wCol);                           
	// Setzt die Keyfarbe und liefert die zuvor eingestellte Keyfarbe zurück 	
	
	COLORREF GetColorKey();
	// Liefert die aktuell eingestellte Keyfarbe zurück	                                                                        

	void SetBrightness(int nBrightness);

	void SetContrast(int nContrast);
	
	void SetSaturation(int nSaturation);

	void Scroll(WORD wX, WORD  wY); 
	// Zweck: Dient beim Zoomen zum verschieben des Bildes.
	// wX = Anzahl der Pixel um die das Bild nach links geschoben wird.
	// wY = Anzahl der Pixel um die das Bild nach oben geschoben wird.
	// Anmerkung: Horizontal kann nur in 32 Pixelschritten verschoben werden:
	// Scroll sollte nurverwendet werden, wenn der horizontale und vertikale
	// Zoom 2 ist.
	
	BOOL ActivatePosDialog(HWND hWnd);
	// Öffnet einen Dialog zur Positionierung des eingeblendeten Videobildes
	// hWnd  = Fensterhandle des Parentfensters, ist dies NULL, so wird das Fensterhandle
	//		   verwendet, welches beim Anlegen der Klasse angegeben wurde.
		                                                                        
	BOOL ActivateColorDialog(HWND hWnd);
	// Öffnet einen Dialog zur Einstellung von Kontrast, Helligkeit, Farbsättigung und Hue
	// hWnd  = Fensterhandle des Parentfensters, ist dies NULL, so wird das Fensterhandle
	//		   verwendet, welches beim Anlegen der Klasse angegeben wurde.

	BOOL ActivateDelayDialog(HWND hWnd);
	// Öffnet einen Dialog zur Einstellung von diversen Delays auf dem Megraboard
	// hWnd  = Fensterhandle des Parentfensters, ist dies NULL, so wird das Fensterhandle
	//		   verwendet, welches beim Anlegen der Klasse angegeben wurde.

	BOOL ActivateAcquisitionDialog(HWND hWnd);
	// Öffnet einen Dialog zur Einstellung des Acquisitionfensters
	// hWnd  = Fensterhandle des Parentfensters, ist dies NULL, so wird das Fensterhandle
	//		   verwendet, welches beim Anlegen der Klasse angegeben wurde.

	BOOL ActivatePllDialog(HWND hWnd);
	// Öffnet einen Dialog zur Einstellung des Pll-Synthesizers
	// hWnd  = Fensterhandle des Parentfensters, ist dies NULL, so wird das Fensterhandle
	//		   verwendet, welches beim Anlegen der Klasse angegeben wurde.

	virtual ~CMegra();
	// Zerstört das CMegra-Objekt und meldet ein bei der MEGRA.DLL angemeldetes
	// Fenster wieder ab.

private:
	HWND	 m_hWnd;        		// Fensterhandle des registrierten Fensters, ansonsten NULL
	BOOL	 m_bMegraOk;			// TRUE = Megra korrekt initialisiert.
	CMyMutex m_cmMegra;
};
            
/////////////////////////////////////////////////////////////////////////////
//Export-Funktionen speziell für den Debugger                
WORD   		__export ReadFromI2C(WORD wSlave, WORD wSubAddress);
WORD   		__export WriteToI2C(WORD wSlave, WORD wSubAddress, WORD wValue);
WORD   		__export ReadFromMergeDAC(WORD wReg, WORD wIndex);
VOID   		__export WriteToMergeDAC(WORD wReg, WORD wIndex, WORD wValue);
WORD   		__export ReadFromControl(VOID);
VOID   		__export WriteToControl(WORD wValue);
VOID   		__export ReadVCMemory(LPRECT Quad, WORD huge* lpBuffer);
VOID   	 	__export WriteVCMemory(LPRECT Quad, WORD huge* lpBuffer);
BOOL        __export  SetVMCBase(WORD wVMCBase);

#endif // __CMEGRA_H__
