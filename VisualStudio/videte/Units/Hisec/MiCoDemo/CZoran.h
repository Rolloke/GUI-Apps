#ifndef __CZORAN_H__ 
#define __CZORAN_H__

#include <afxmt.h>
#include "zr050.h"
#include "zoranio.h"

typedef HANDLE (WINAPI *OPENVXDH) (HANDLE);

class Z050_Coder;
class ZoranIo;

class CZoran
{
	public:
		CZoran(const CString &sAppIniName);
		// Konstruktor
		// sAppIniName name der INI-Datei in dem die Initialisierungswerte stehen.

		~CZoran();
		// Destruktoer

		BOOL IsValid();
		// Liefert TRUE, wenn das Objekt gültig ist

		BOOL Open(const CONFIG &Conf);
		// Alloziert Speicher für die Jpeg und Config Struktur und initialisiert
		// diese anhand der micounit.ini Datei.
		// Der MiCo.vxd wird geladen und geöffnet.
		
		BOOL Close();
		// Gibt allozierten Speicher wieder frei, und entlädt den MiCo.vxd

		void SetInputSource(WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byPortY, BYTE byPortC, DWORD dwProzessID);
		// Schaltet auf eine andere Videoquelle
		// wFrontend	= Nummer des Frondends (MICO_FE0, MICO_FE1)
		// wExtCard		= Nummer des Boars (MICO_EXTCARD0...MICO_EXTCARD3)
		// byPortY		= Physikalischer Eingang des Y-Signals (0...7)
		// byPortC		= Physikalischer Eingang des C-Signals (0...7)
		// dwProzessID	= Eindeutige ProzeßID. Diese ID wird jeden Bild angeheftet.

		void SetFeVideoFormat(WORD wFormat);
		// Setzt das Frontend VideoFormat
		// wFormat = MICO_PAL_SQUARE, MICO_NTSC_SQUARE, MICO_PAL_CCIR, MICO_NTSC_CCIR

		void SetBeVideoFormat(WORD wFormat);
		// Setzt das Backend VideoFormat
		// wFormat = MICO_PAL_SQUARE, MICO_NTSC_SQUARE, MICO_PAL_CCIR, MICO_NTSC_CCIR

		BOOL StartEncoding();
		// Startet den Encoder

		BOOL StopEncoding();
		// Stopt den Encoder

		BOOL StartDecoding();
		// Startet den Decoder

		BOOL StopDecoding();
		// Stopt den Decoder

		int  GetEncoderState();
		// Liefert den aktuellen Status des Encoders (MICO_ENCODER_ON, MICO_ENCODER_OFF)

		int	 GetDecoderState();
		// Liefert den aktuellen Status des Decoders (MICO_DECODER_ON, MICO_DECODER_OFF)

		void SetFormat(WORD wFormat);
		// Setzt die Auflösung des Encoderbildes
		// wFormat = MICO_ENCODER_LOW, MICO_ENCODER_HIGH

		void SetFramerate(WORD wFramerate);
		// Setzt die Framerate (Wird nicht unterstützt)
		
		void SetBPF(DWORD wBPF);
		// Setzt die Bytes pro Field (MICO_MIN_BPF... MICO_MAX_BPF)

		DWORD WriteData(LPCSTR pData, DWORD dwDataLen);
		// Überträgt ein Jpeg-Bild an den Decoder
		// pData		= Pointer auf die Bilddaten
		// dwDataLen	= Größe des Jpegbildes in Bytes.
		// Rückgabe: Anzahl der übertragenen Bytes.

		JPEG* ReadData(BOOL &bContinue);
		// Liest Bilddaten aus dem SharedMemory und stellt diese in Form einer JPEG-Struktur
		// zur Verfügung. Die Jpegstruktur enthält neben den eigendlichen JPeg-Bilddaten z.B.
		// noch die Kammeranummer.
		// Die Funktion kehrt erst nach erhalt eines Bildes zurück, spätestens aber nach einer
		// Sekunde.

		DWORD WaitForFrameEvent();
		// Realisiert eine Semaphore zur Kommunikatiopn mit dem MiCo.VxD und
		// ersetzt das damit das 'WaitForSingleObject'.

	private:
		BOOL Reset();
		void EnableBrdIRQ();
		void DisableBrdIRQ();
		void SetIRQNumber(WORD wIRQ);
		void ChipSelectVideoInAnalogOut(WORD wFrontEnd, WORD wExtCard, BYTE byData);
		BYTE ChipSelectVideoInAnalogIn(WORD wFrontEnd, WORD wExtCard);
		void ChipSelectVideoInDigitalOut(BYTE byData);
		BYTE ChipSelectVideoInDigitalIn();
		
		void ZR36055InitForEncoding();
		void ZR36050InitForEncoding();
		void ZR36055InitForDecoding();
		void ZR36050InitForDecoding();

		void WriteToZR36050(WORD wAdr, BYTE byValue);
		BYTE ReadFromZR36050(WORD wAdr);
		void TraceEncoderQueue();
		
		Z050_Coder *m_pzCoder;
		ZoranIo	   *m_pZIo;

		CString	m_sAppIniName;
		HANDLE	m_hCVxD;
		BOOL	m_bOK;
		CIo		m_IO;
		BYTE	m_byMiCoID;
		int		m_nEncoderState;
		int		m_nDecoderState;
		CONFIG	*m_pConf;
		DWORD	m_dwDecodedFrames;		
		WORD	m_wFormat;				// Bildauflösung
		WORD	m_wFramerate;			// Frames pro Sekunde
		WORD	m_wBPF;					// Bytes pro Frame
		WORD	m_wMinLen;				// Mindestgröße eines Frames in Bytes
		WORD	m_wMaxLen;				// Maximalgröße eines Frames in Bytes
		float	m_fMinJpegLen;			// Minimale Jpeggröße in % zur Sollgröße
		float	m_fMaxJpegLen;			// Maximale Jpeggröße in % zur Sollgröße
		WORD	m_wReadIndex;			// Leseindex auf Encoderqueue
		WORD	m_wFeVideoFormat;
		WORD	m_wFeHStart[4];
		WORD	m_wFeHEnd[4];
		WORD	m_wFeHTotal[4];
		WORD	m_wFeVStart[4];
		WORD	m_wFeVEnd[4];
		WORD	m_wFeHSStart[4];
		WORD	m_wFeHSEnd[4];
		WORD	m_wFeVTotal[4];
		WORD	m_wFeBlankStart[4];
		WORD	m_wFeNMCU[4];

		WORD	m_wBeVideoFormat;
		WORD	m_wBeHStart;
		WORD	m_wBeHEnd;
		WORD	m_wBeHTotal;
		WORD	m_wBeVStart;
		WORD	m_wBeVEnd;
		WORD	m_wBeHSStart;
		WORD	m_wBeHSEnd;
		WORD	m_wBeVTotal;
		WORD	m_wBeNMCU;
		WORD	m_wBeBlankStart;

		WORD	m_wYBackgnd;
		WORD	m_wUVBackgnd;

		DWORD	m_dwTCVData;
		DWORD	m_dwTCVNet;
		WORD	m_wSF;

		DWORD	m_dwEncoderTimeout;	// in ms
		HANDLE	m_hEvent;
};
#endif // __CZORAN_H__