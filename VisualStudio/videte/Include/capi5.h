/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: capi5.h $
// ARCHIVE:		$Archive: /Project/Include/capi5.h $
// CHECKIN:		$Date: 23.01.97 13:51 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 7.01.97 13:06 $
// BY AUTHOR:	$Author: Carsten $
// $Nokeywords:$

#ifndef __CAPI5_H__
#define __CAPI5_H__

#include "AbstractCapi.h"
////////////////////////////////////////////////////////////////////////////
// Konstanten für die ListenRequest Funktion
#define LISTEN_REJECTALL			"R"
#define LISTEN_ALL					"A"		// Gilt nur für CAPI 2.0

////////////////////////////////////////////////////////////////////////////
// Diese Struktur wird bei der Submessage C_INCOMMING benutzt
typedef struct
{
	char szCallingParty[32];	// Telefonnumer der Gegenstelle
	char szCalledParty[32];		// Telefonnumer der Station, die die Gegenstation anrufen möchte
} CALLINFO;
typedef CALLINFO FAR* LPCALLINFO;

////////////////////////////////////////////////////////////////////////////
// Capi prioritäten für data.  (Funktion: SendData, Param: iPrior)
#define PRIOR_HIGH	1	// Daten werden mit hoechster Prioritaet gesendet
#define PRIOR_MID	2 	// Mittlere Priorität für Daten
#define PRIOR_LOW	3   // Niedrige Priorität für Daten

enum CapiConnectionState {
	CCS_INVALID_CONNECTION	=	0,
	CCS_DISCONNECTED		=	1,
	CCS_CALLING				=	2,
	CCS_CONNECTED			=	3,
	CCS_DISCONNECTING		=	4
};	


////////////////////////////////////////////////////////////////////////////
// Forward declaration
class CCapiIntern;
class CCapiClient;
class CCapiConnectionManager;

////////////////////////////////////////////////////////////////////////////
class CCapi
{
	CCapiConnectionManager*	m_pConnectionManager;
public:
	CCapi();

	BOOL		Create(CAbstractCapiClient* pClient, const CString &sOwnNumber, DWORD dwConnectionTimeout=30000);

	CapiHandle	Open(const CString &sAdress, WORD wBCannelCount=0);
	// Verbindung aufbauen
	// szAdress:		Telefonnummer, max. 31 Zeichen
	// wBCannelCount:	Max Anzahl der B-Kanäle für diese Verbindung, wenn 0 dann die maimal möglichen
	// Return			Handle auf Connection oder INVALID_CAPI_HANDLE

	void		Close(CapiHandle ch, WORD wBChannel=0);
	// Die Verbindung con CapiHandle abbauen
	// Wenn wBCannel==0 oder wBCannel==GetActChannelCount() ganze Verbingung trennen
	// sonst nur wBCannel abbauen

	int			GetActChannelCount(CapiHandle ch);
	// Liefert die aktuelle Anzahl der B-Kanäle für die Verbindung ch
	
	BOOL		OpenNextChannel(CapiHandle ch);

	BOOL		AnswerCall(CapiHandle ch, BOOL bAccept, WORD wBChannel=0);

	CapiConnectionState	GetState(CapiHandle ch);

	BOOL		SendData(CapiHandle ch, const BYTE* pData, DWORD dwLen, BOOL bEndOfQueue=TRUE);

	CString		GetVersion();
	// Liefert die Verson des CAPI-Treibers

	CString		GetManufacturer();
	// Liefert den Manufacturer des CAPI-Treibers
	
	int			GetNumConnections();
	int			GetNumController();
	int			GetMaxChannels();

	~CCapi();
};

////////////////////////////////////////////////////////////////////////////
#endif // __CAPI5_H__
