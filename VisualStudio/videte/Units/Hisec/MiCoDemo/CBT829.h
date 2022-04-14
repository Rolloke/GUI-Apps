#ifndef __CBT829_H__ 
#define __CBT829_H__
#include "ci2c.h"

#define SIZE 			255
#define BT829_MAX_REG  	40
class Cio;

class CBT829 : public CI2C
{
	public:
		CBT829(WORD wIOBase, WORD wFeID);
		// Konstruktor
		// wIOBase = Basisadresse des Boards
		// wFeID   = Nummer des Frontends (MICO_FE0, MICO_FE1)

		BOOL Init(const CString &sFileName, const CString &sSection);
		// Initialisiert das Videofrontend
		// sFilename = Name der INI-Datei
		// sSection  = Name des Abschnitts in dem die Initialisierungswerte des BT829 stehen

		BOOL IsValid();
		// Liefert TRUE, wenn das Objekt korrekt angelegt werden konnte

		BOOL SetFeVideoFormat(WORD wFeVideoFormat);
		// Setzt das Videoformat des BT829
		// wFeVideoFormat = MICO_NTSC_SQUARE, MICO_PAL_SQUARE, MICO_NTSC_CCIR, MICO_PAL_CCIR
		// Rückgabe: Bisheriges FrontEnd-Videoformat
		//			 Im Fehlerfall FALSE

		BOOL SetFeSourceType(WORD wFeSourceType, WORD wFeFilter);
		// Setzt den Sourcetype des BT829
		// wFeSourceType = MICO_FBAS, MICO_SVHS
		// wFilter		 = MICO_NOTCH_FILTER_AUTO, MICO_NOTCH_FILTER_ON, MICO_NOTCH_FILTER_OFF
		// Rückgabe: Im Fehlerfall FALSE
		// Note:	 Wenn wFeFilter auf MICO_NOTCH_FILTER_AUTO steht, so werden die Filter
		//			 im FBAS-Betrieb eingeschaltet und im SVHS-Betrieb ausgeschaltet.

		
		BOOL SetFeBrightness(WORD wFeBrightness);
		// Setzt die Helligkeit
		// wBrightness	 = Helligkeitseinstellung (MICO_MIN_BRIGHTNESS...MICO_MAX_BRIGHTNESS)
		// Rückgabe: Im Fehlerfall FALSE

		BOOL SetFeContrast(WORD wFeContrast);
		// Setzt die Kontrasteinstellung
		// wFeContrast = Kontrasteinstellung (MICO_MIN_CONTRAST... MICO_MAX_CONTRAST)
		// Rückgabe: Im Fehlerfall FALSE
		
		BOOL SetFeSaturation(WORD wFeSaturation);
		// Setzt die Farbeinstellung
		// wFeSaturation = Farbeinstellung (MICO_MIN_SATURATION... MICO_MAX_SATURATION)
		// Rückgabe: Im Fehlerfall FALSE

		BOOL SetFeHue(WORD wFeHue);
		// Setzt die HUE-einstellung
		// wFeHue = HUE-einstellung (MICO_MIN_HUE... MICO_MAX_HUE)
		// Rückgabe: Im Fehlerfall FALSE

		WORD SetFeHScale(WORD wHScale);
		// Setzt die horizontale Skalierung im BT829
		// wHScale = horizontaler Skalierungswert (MICO_MIN_HSCALE...MICO_MAX_HSCALE)
		// Rückgabe bisherige Skalierung

		WORD SetFeVScale(WORD wVScale);
		// Setzt die vertikale Skalierung im BT829
		// wVScale = horizontaler Skalierungswert (MICO_MIN_VSCALE...MICO_MAX_VSCALE)
		// Rückgabe: Bisherige Skalierung

		WORD SetFeFilter(WORD wFilter);
		// Aktiviert bzw. deaktiviert die beiden Filter des BT829
		// wFilter = MICO_LNOTCH_ON, MICO_LNOTCH_OFF
		//			 MICO_LDEC_ON, MICO_LDEC_OFF
		// Rückgabe: Bisherige Filtereinstellung.

		WORD GetFeVideoFormat();
		// Liefert das FrontEnd-Videoformat (MICO_NTSC_SQUARE, MICO_PAL_SQUARE, MICO_NTSC_CCIR, MICO_PAL_CCIR) 

		WORD GetFeSourceType();
		// Liefert den SourceType (MICO_FBAS, MICO_SVHS)
		
		WORD GetFeBrightness();
		// Liefert die Helligkeitseinstellung
		// Der Wert liegt innerhalb der Grenzen (MICO_MIN_BRIGHTNESS...MICO_MAX_BRIGHTNESS)
		
		WORD GetFeContrast();
		// Liefert die Kontrasteinstellung
		// Der Wert liegt innerhalb der Grenzen (MICO_MIN_CONTRAST...MICO_MAX_CONTRAST)

		WORD GetFeSaturation();
		// Liefert die Farbsättigung
		// Der Wert liegt innerhalb der Grenzen (MICO_MIN_SATURATION...MICO_MAX_SATURATION)
		
		WORD GetFeHue();
		// Liefert den 'Hue'
		// Der Wert liegt innerhalb der Grenzen (MICO_MIN_HUE...MICO_MAX_HUE)
		
		WORD GetFeHScale();
		// Liefert die aktuelle horizontale Skalierung des BT829
		
		WORD GetFeVScale();
		// Liefert die aktuelle vertikale Skalierung des BT829
		
		WORD GetFeFilter();
		// Liefert die augenblickliche Filtereinstellung des BT829
		
		BOOL IsVideoPresent();
		// Liefert TRUE, wenn der BT829 das Videosignal für gültig hält

	private:
		void CBT829Reset();
		// Führt einen Hardwarereset des BT829 durch

		BOOL CheckIt();
		// Führt einen Schreib/Lesetest auf die Register des BT829 durch

		BOOL BT829Out(BYTE bySubAddress, BYTE byData);
		// Schreibt einen 8Bit Wert in ein Register des BT829

		BYTE BT829In(BYTE bySubAddress);
		// Liest einen 8Bit Wert aus einem Register des BT829

		BYTE m_byBT829Reg[BT829_MAX_REG][4];		
		// Enthält die Initialisierungswerte des BT829 für die 4 Videoformate
		// Die Initialisierungswerte stammen aus der INI-Datei

		static int 	m_nBT829InstCnt;		// Instanzenzähler
		
		static CCriticalSection m_csBT829;	// Synchronisationsobjekt

		CIo  m_Io;							// I/O-Zugriffs Objekt
		WORD m_wIOBase;						// Basisadresse des Boards
		WORD m_wFeID;						// Nummer des Frontends (MICO_FE0, MICO_FE1)
		WORD m_wFeVideoFormat;				// Aktuelles Videoformat
		WORD m_wFeSourceType;				// Aktueller Sourcetype
		WORD m_wFeBrightness;				// Aktuelle Helligkeitseinstellung
		WORD m_wFeContrast;					// Aktuelle Kontrasteinstellung
		WORD m_wFeSaturation;				// Aktuelle Farbeinstellung
		WORD m_wFeHue;						// Aktuelle Einstellung des Hue
		WORD m_wFeHScale;					// Aktuelle Einstellung der hor.  Skalierung
		WORD m_wFeVScale;					// Aktuelle Einstellung der vert. Skalierung
		WORD m_wFeFilter;					// QAktuelle Filtereinstellung
		BOOL m_bOk;							// TRUE -> Objekt ok
};
		
#endif // __CBT829_H__


