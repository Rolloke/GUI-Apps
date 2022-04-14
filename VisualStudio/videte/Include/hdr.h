#ifndef _INC_HDR
#define _INC_HDR    /* #defined if hdr.h has been included */

///////////////////////////////////////////////////////////////////////////
// hdr.h
// Inhält strukturen die verschiedene Headers definieren.
// Inhält ID's für den Datentransfer.                                    
// 
// Ein Datapaket sieht folgendermaßen aus:
//
//	 <-------------------Capiheader----------------------> <--Extra Hdr--> <--Data-->
//	|ID|walid|number|service|extra hdr. length|data length|extra hdr. data|...data...|

#include "stdlib.h"	// für _MAX_PATH

///////////////////////////////////////////////////////////////////////////
// Header ID's                      
#define	ID_CAPI				123		// Für datatransfer Capi intern.
#define ID_CIPC				124		// Data für CIPC_Pipe
#define ID_CIPC_COMP		125		// Data für CIPC_Pipe komprimiert
#define ID_CIPC_COMP_SEG	126		// Data für CIPC_Pipe komprimiert und portioniert

#define HDRSIZE				6		// Header Größe in Bytes.

// OOPS NEED count CAPI standard fields too?
#define	MY_DATA_LEN		(2048-HDRSIZE-8) 	// Max länge der Daten.

///////////////////////////////////////////////////////////////////////////
// DEFAULTHEADER
// Das ist der eigentliche Capiheader die mit allem Datenpaketen gesendet wird
typedef struct
{
	BYTE m_byID;		// ID des Datenblocks.  Sagt wohin die Daten gehen.
	BYTE	byChannelNr;	// Daten sind Valid oder nicht.
	BYTE	byNr;		// Nummer für Channelbundeling. Nur Capi intern
	BYTE	byService;	// Service der Daten
	WORD	wDataLen;	// die Länge der Daten die nach dem Header kommen
	//
} DEFHDR;       
typedef DEFHDR FAR* LPDEFHDR;

///////////////////////////////////////////////////////////////////////////
// CAPIHDR

// wService defines:
#define	CAPI_CHECKCONNECTION	2
#define CAPI_CONNECTIONOK		3

// byService
// CAVEAT byService and old  byType are mixed in one byte
// the type uses the highest 4 bit
// that implies there can be only 16 services at all
#define CIPC_DATA_SMALL		1
#define CIPC_INVALID_SERVICE 2
#define CIPC_REQ			3
#define CIPC_CONF			4
#define CIPC_REMOVE			5
#define CIPC_FETCH_RESULT	6	// NEW 04.03.97 HEDU
// NEW 280397
#define CIPC_DATA_LARGE_START		7
#define CIPC_DATA_LARGE				8
#define CIPC_DATA_LARGE_END			9


#define CIPC_MOREHDRLEN		0
#define CIPC_HDRLEN			(HDRSIZE)
#define CIPC_MAX_DATA		(MY_DATA_LEN)

typedef struct
{
	BYTE	m_byID;		// ID des Datenblocks, sagt wohin die Daten gehen
	BYTE	byChannelNr;	// Daten sind Valid oder nicht
	BYTE	byNr;		// Nummer für Channelbundling
	BYTE	byService;	// Service der Daten
	WORD	wDataLen;
	// -------------- Weitere Hdr. Daten ------------------
	// DROPPED DWORD	dwRestLen;	// Die restliche Länge der CIPC-Daten
	// DROPPED BYTE	byType;		// Typ der CIPC-Daten (z.Z. INPUT, OUTPUT)
} CIPCHDR;
typedef CIPCHDR FAR* LPCIPCHDR;


#endif	// _INC_HDR              
