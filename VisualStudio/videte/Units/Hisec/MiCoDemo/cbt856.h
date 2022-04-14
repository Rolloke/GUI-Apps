#ifndef __CBT856_H__ 
#define __CBT856_H__
#include "ci2c.h"

#define SIZE 			255
#define BT856_MAX_REG   0xde

class CIo;

class CBT856 : public CI2C
{
	public:
		CBT856(WORD wIOBase);
		// Konstruktor

		BOOL Init(const CString &sFileName, const CString &sSection);
		// Initialisiert das Videobackend BT856
		// sFilename = Name der INI-Datei
		// sSection  = Abschnit in der die Daten zur Initialisierung de BT856 stehen 

		BOOL IsValid();
		// Liefert TRUE, wenn das objekt korrekt angelegt werden konnte
		
		WORD SetBeVideoFormat(WORD wBeVideoFormat);
		// Setzt Das Videoformat des Backends:
		// (MICO_PAL_CCIR, MICO_NTSC_CCIR, MICO_PAL_SQUARE, MICO_NTSC_SQUARE)

		WORD GetBeVideoFormat();
		// Liefert das mit 'SetBeVideoFormat()' zuletzt eingestellte Videoformat.

	private:
		BOOL CheckIt();
		// Liefert TRUE, wenn die VendorID des BT856 stimmt.

		void CBT856Reset();
		// Führt einen Hardwarereset des BT856 durch

		BOOL BT856Out(BYTE bySubAddress, BYTE byData);
		// Schreibt ein Byte in ein bestimmtes Register des BT856

		BYTE BT856In(BYTE bySubAddress);
		// Liest ein Byte aus einem bestimmten Register des BT856

		BYTE m_byBT856Reg[BT856_MAX_REG+1];		
		// Latch der Register des BT856, da diese leider nicnt readback-fähig sind

		WORD m_wBeVideoFormat;		// Eingestelltes Videoformat
		CIo  m_Io;					// I/O-Zugriffs Objekt
		WORD m_wIOBase;				// Basisadresse des Boards
		BOOL m_bOk;					// TRUE -> Objekt ok

		CCriticalSection m_csBT856;	// Synchronisationsobjekt
};
		
#endif // __CBT856_H__


