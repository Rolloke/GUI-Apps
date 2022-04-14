/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CAPI4.H $
// ARCHIVE:		$Archive: /Project/Include/CAPI4.H $
// CHECKIN:		$Date: 15.12.05 10:42 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 15.12.05 10:42 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CAPI4_H
#define CAPI4_H


class CAbstractCapiClient;	// forward declaration

////////////////////////////////////////////////////////////////////////////
// Konstanten fuer Create funktion
#define PROT_NOHANDSAKE				0		// Konstante für wMyProtocol in Create Funktion (default)
// DROPPED UNUSED #define PROT_HANDSAKE				1 		// Konstante für wMyProtocol in Create Funktion
#define DEFAULT_B_CHANNELS			2 		// Konstante für wMaxBCh in Create Funktion (default)
#define DEFAULT_X_BUFFERS			4		// Konstante für wXBuffers in Create Funktion(default)
#define MAX_MOVING_BLOCKS			10      // Konstante für wMyMaxMovingBlocks in Create Funktion (default)

////////////////////////////////////////////////////////////////////////////
// Konstanten für die ListenRequest Funktion
#define LISTEN_REJECTALL			_T("R")
#define LISTEN_ALL					_T("A")		// Gilt nur für CAPI 2.0

////////////////////////////////////////////////////////////////////////////
// Diese Struktur wird bei der Submessage C_INCOMMING benutzt
typedef struct
{
	char szCallingParty[32];	// Telefonnumer der Gegenstelle
	char szCalledParty[32];		// Telefonnumer der Station, die die Gegenstation anrufen möchte
} CALLINFO;
typedef CALLINFO FAR* LPCALLINFO;

////////////////////////////////////////////////////////////////////////////
// Submessages für uMsgCommand
// 	message:		uMsgCommand
// 	wParam:		C_...

#define C_INCOMMING					10
// Wird gesendet bei Incomming Call (CONNECT_IND). Wurde ein Listen Request auf LISTEN_ALL
// ausgesetzt, muß der Anruf hier angenommen oder zurueckgewiesen werden
// lParam:	Pointer auf eine CALLINFO Struktur. (siehe oben)
// return:	FALSE 	Anruf wird zurückgewiesen,
//			TRUE  	Anruf wird angenommen
// ACHTUNG:	Diese Message kommt nicht unter folgenden Bedingungen:
//			- Ein Listen wird als LISTEN_REJECTALL ausgesetzt
//			- Telefonnummer stimmt nicht mit der Telefonnummer,
//			  die mit ListenRequest ausgesetzt wurde, überein

#define C_OUTGOING					20
// Wird gesendet, wenn Netz Outgoing Call bestaetigt hat (CONNECT_CONF)
// lParam:	LPSTR auf die Telefonnumer der Gegenstation
// return:	wird nicht ausgewertet

#define C_REJECTING					30
// Wird gesendet wenn Incomming call zurueckgewiesen wird
// lParam:	Pointer auf eine CALLINFO Struktur. (siehe oben)
// return:	wird nicht ausgewertet

#define C_OUTGOINGOK				50
// Wird gesendet, wenn eine aktive vollstaendige Verbindung bestehet (CONNECT_B3_ACTIVE_RESP)
// lParam:	LPSTR auf die Telefonnumer der Gegenstation
// return:	wird nicht ausgewertet

#define C_INCOMMINGOK				40
// Wird gesendet, wenn eine passive vollstaendige Verbindung bestehet (CONNECT_B3_ACTIVE_RESP)
// lParam:	LPSTR auf die Telefonnumer der Gegenstation
// return:	wird nicht ausgewertet

#define C_HANGUP					60
// Wird gesendet, wenn Capi4 keine weitere Verbindungen hat
// lParam:	0L
// return:	wird nicht ausgewertet

#define C_CHANNEL_HANGUP			61
// Wird gesendet, wenn eine Verbindung (B-Kanal) vollstaendig beendet ist (DISCONNECT_RESP)
// lParam:	LPSTR auf die Telefonnummer der Gegenstation
// return:	wird nicht ausgewertet

#define C_NODISCONNECT				80
// ERROR, Capi kann eine Verbindung nicht abbauen
// lParam:	LPSTR auf die Telefonnumer der Gegenstation
// return:	wird nicht ausgewertet

#define C_CHECKPASSWORD				90
// Wenn Passwortaustausch vorhanden ist (SetPasswort Funktion),
// kann das Passwort fuer diesen B-Kanal hier von der Applikation untersucht werden
// lParam:	LPSTR auf das Passwort
// return:	TRUE	Passwort ist OK
// 			FALSE	Passwort ist falsch -> die Verbindung wird abgebaut

#define C_PASSWORDFALSE				100
// Das Passwort einer Gegenstation ist falsch -> Verbindung wird abgebaut (B-Kanal)
// lParam:	0L
// return:	wird nicht ausgewertet

#define C_INFO						110
// Capi hat einen Fehler entdeckt oder liefert bestimmte Informationen vom Netz, siehe INF_... Message
// lParam:	LOWORD = INF_... Message
// return:	wird nicht ausgewertet

#define C_NR_CONNECT				120
// Wird jedesmal gesendet, wenn sich die Anzahl der benutzten B-Kanaele aendert
// lParam:	LOWORD = Aktuelle Anzahl der B-Kanaele
// return:	wird nicht ausgewertet

#define C_DATA						130
// Capi hat Data bekommen
// lParam:	LPBYTE auf die Daten.  Der Pointer wird sofort nach Return ungueltig
// return:	wird nicht ausgewertet

#define C_UNKNOWNDATA				131
// Capi hat Data bekommen mit ungueltigem Header
// lParam:	LPBYTE auf die Daten.  Der Pointer wird sofort nach Return ungueltig
// return:	wird nicht ausgewertet

#define C_LISTENREQUEST				140
// lParam:	0:		Listen ist ohne Fehler abgesetzt worden.  (LISTEN_CONF)
//			<>0:	Fehler ist aufgetreten, siehe INF_ Message
// return:	wird nicht ausgewertet

#define C_USER_BUSY					150
// Gegenstation ist Busy und kann den Anruf nicht annehmen
// lParam:	0L

#define C_CALL_REJECTED				160
// Gegenstation ist Busy und hat den Anruf rejected
// lParam: 	0L

////////////////////////////////////////////////////////////////////////////
// Capi Submessages für C_INFO Message Stehen in LOWORD von lParam
// CAPI 2.0
#define INF_TOO_MANY_APPL					0x1001
#define INF_BLOCK_SIZE_TOO_SMALL			0x1002
#define INF_BUF_EXCEEDS_64K					0x1003
#define INF_MSG_BUF_TOO_SMALL				0x1004
#define INF_MAX_NR_CONN_NOSUPPORT			0x1005
#define INF_INTERNAL_BUSY_1					0x1007
#define INF_NO_MEMORY_1						0x1008
#define INF_CAPI_NOT_INSTALLED_1			0x1009
#define INF_EXT_EQU_NOSUPPORT_1				0x100A
#define INF_EXT_EQU_ONLY_SUPPORT_1			0x100B

#define INF_ILL_APPL_NR						0x1101
#define INF_ILL_CMD_OR_MSGLEN				0x1102
#define INF_Q_FULL							0x1103
#define INF_Q_EMPTY							0x1104
#define INF_Q_OVERFLOW						0x1105
#define INF_UNKNOWN_NOTF_PARAM				0x1106
#define INF_INTERNAL_BUSY_2					0x1107
#define INF_NO_MEMORY_2						0x1108
#define INF_CAPI_NOT_INSTALLED_2			0x1109
#define INF_EXT_EQU_NOSUPPORT_2				0x110A
#define INF_EXT_EQU_ONLY_SUPPORT_2			0x110B
		
#define INF_20_MESSAGE_NOT_SUPPORTED		0x2001
#define INF_ILLEGAL_CONTROLLER_PLCI			0x2002
#define INF_OUT_OF_PLCI						0x2003
#define INF_OUT_OF_NCCI						0x2004
#define INF_OUT_OF_LISTEN					0x2005
#define INF_OUT_OF_FAX						0x2006
#define INF_ILLEGAL_MSG_PARAMS				0x2007

#define INF_B1_PROTOCOL_NOT_SUPPORTED		0x3001
#define INF_B2_PROTOCOL_NOT_SUPPORTED		0x3002
#define INF_B3_PROTOCOL_NOT_SUPPORTED		0x3003
#define INF_B1_PARAMS_NOT_SUPPORTED			0x3004
#define INF_B2_PARAMS_NOT_SUPPORTED			0x3005
#define INF_B3_PARAMS_NOT_SUPPORTED			0x3006
#define INF_B_PROTOCOL_COMBINATION_NOT_SUPPORTED	0x3007
#define INF_NCPI_NOT_SUPPORTED				0x3008
#define INF_CIP_VALUE_UNKNOWN				0x3009
#define INF_FLAGS_NOT_SUPPORTED				0x300A
#define INF_FACILITY_NOT_SUPPORTED			0x300B
#define INF_DATA_LENGTGH_NOT_SUPPORTED		0x300C
#define INF_RESET_PROCEDURE_NOT_SUPPORTED	0x300D

#define INF_PROTOCOL_ERROR_LAYER_1			0x3301
#define INF_PROTOCOL_ERROR_LAYER_2			0x3302
#define INF_PROTOCOL_ERROR_LAYER_3			0x3303
#define INF_ANOTHER_APPLICATION_GOT_THE_CALL	0x3304

#define INF_20_NORMAL_CALL_CLEARING			0x3410
#define INF_20_USER_BUSY					0x3411
#define INF_20_NO_USER_RESPONDING			0x3412
#define INF_20_NO_ANSWER_FROM_USER			0x3413
#define INF_20_CALL_REJECTED				0x3415
#define INF_20_NUMBER_CHANGED				0x3416
#define INF_20_NON_SELECTED_USER_CLEARING	0x341A
#define INF_20_DESTINATION_OUT_OF_ORDER		0x341B
#define INF_20_INVALID_NUMBER_FORMAT		0x341C
#define INF_20_FACILITY_REJECTED			0x341D
#define INF_20_RESPONSE_TO_STATUS_ENQUIRY	0x341E
#define INF_20_NORMAL_UNSPECIFIED			0x341F
#define INF_20_NO_CHANNEL_AVAILABLE			0x3422
#define INF_20_NETWORK_OUT_OF_ORDER			0x3426
#define INF_20_TEMPORARY_FAILURE			0x3429

////////////////////////////////////////////////////////////////////////////
// CAPI 1.1
#define INF_ILL_APPL_ID				0x1002
#define INF_Q_OVFL					0x1007
#define INF_ILL_CNTL				0x2001
#define INF_ILL_PLCI				0x2002
#define INF_ILL_NCCI				0x2003
#define INF_B_MASK					0x3101
#define INF_INFO_MASK				0x3102
#define INF_TO_MANY_LISTEN_REQ		0x3202
#define INF_B2PROT_CHG				0x3207
#define INF_B3PROT_NOT_SUPP			0x3208
#define INF_DLEN_NOT_SUPP			0x320D
#define INF_NO_PLCI_FREE			0x320E
#define INF_NO_NCCI_FREE			0x320F
#define INF_NONETWORKREAKTION		0x3301
#define INF_ERR_CONNECTING_D2		0x3302
#define INF_B_L2_SHUTDN				0x3309
#define INF_B_L1_SHUTDN				0x3308
#define INF_NO_USER_RESP			0x34BA
#define INF_USER_BUSY				0x34BB
#define INF_REMOTE_USER_INITIATED	0x34DA
#define INF_NO_CH_AVALIABLE			0x348A
#define INF_DEST_NOT_OBTAINABLE		0x34B5
#define INF_OUT_OF_ORDER			0x34B9
#define INF_CALL_REJECTED			0x34BE
#define INF_LOCAL_PROCEDURE_ERROR	0x34F0
#define INF_REMOTE_PROCEDURE_ERROR	0x34F1
#define INF_CONN_NOT_POSSIBLE		0x34A2
#define INF_NETWORK_CONGESTION		0x34D9

////////////////////////////////////////////////////////////////////////////
// Capi prioritäten für data.  (Funktion: SendData, Param: iPrior)
#define PRIOR_HIGH	1	// Daten werden mit hoechster Prioritaet gesendet
						// ACHTUNG: wird auch intern von CAPI benutzt
#define PRIOR_MID	2 	// Mittlere Priorität für Daten
#define PRIOR_LOW	3   // Niedrige Priorität für Daten

class CCp;

////////////////////////////////////////////////////////////////////////////
class CCapi
{
	CCp*	m_pC;
public:
	CCapi();
	int	 	Create(	HWND hWnd,
					UINT uMsgCommand,
					WORD wMaxBCh=DEFAULT_B_CHANNELS,
					WORD wXBuffers=DEFAULT_X_BUFFERS,
					WORD wMyProtocol=PROT_NOHANDSAKE,
					WORD wMyMaxMovingBlocks=MAX_MOVING_BLOCKS,
					BOOL bChannelBundeling=TRUE);
	int	 	NewCreate(	CAbstractCapiClient *pWrapper,
					HWND hWnd,
					UINT uMsgCommand,
					WORD wMaxBCh=DEFAULT_B_CHANNELS,
					WORD wXBuffers=DEFAULT_X_BUFFERS,
					WORD wMyProtocol=PROT_NOHANDSAKE,
					WORD wMyMaxMovingBlocks=MAX_MOVING_BLOCKS,
					BOOL bChannelBundeling=TRUE,
					BOOL bClientWantsRawData=FALSE
					);
	// hWnd:				Handle auf das Empfangfenster
	// uMsgCommand:			Die Message die an das Empfangsfenster gesendet werden
	// wMaxBCh:				Maximale Anzahl der B-Kanäle die das System zur Verfügung hat
	// wXBuffers:			Max. gleichzeitig offene B3-Datablöcke
	// wMyProtocol:			Capi4 Protocol
	// wMyMaxMovingBlocks:	Max B3-Databloecke die gleichzeitig unterwegs sein durfen
	// bChannelBundeling:	Kanaele werden gebundelt
	// bClientWantsRawData	non-iST ISDN protcol for box systems
	// return:	0:	alles OK
	//			1:	Kein Dosspeicher für Capi-Message
	//			2:	Kein Dosspeicher für DosQ 1
	//			3:	Kein Dosspeicher für DosQ 2
	//			4:	Kein Dosspeicher für DosQ 3
	//			5:	Kein Dosspeicher für Capi-PutMessage
	//			6:	Kein Dosspeicher für Textbuffer
	//			8:	Kein Speicher für WaitQ
	//			9:	Kein Speicher für Connect-Control Unit
	//			10:	RegisterClass failed
	//			-1:

	void EnableRawData(BOOL bEnable);

	void 	SetPasswort(LPCTSTR szId);	// ACHTUNG!	Muss aufgerufen werden ! WICHTIG!
	//
	// szId:	Pointer auf das Passwort für diese Applikation
	// 			Wenn szId=NULL, werden keine Passwörter benutzt

	BOOL 	Open(LPCTSTR sAddress, WORD wBCh=DEFAULT_B_CHANNELS);
	// Verbindung aufbauen
	// szAddress:	Telefonnummer, max. 31 Zeichen
	// wBCh:		Max Anzahl der B-Kanäle für diese Verbindung

	void 	Close(BOOL bCloseOnlyOne=FALSE);
	// Alle Verbindungen Abbauen

	// OOPS not found void 	Close(LPSTR szIdNr);
	// Verbindung Abbauen zur Gegenstation mit Telefonnummer szIdNr oder Passwort szIdNr
	// Diese Funktion ist nicht Aktiv

	UINT 	Reset(BOOL bShutdown=FALSE);
	// Capi4 Reset. Alle Daten die Capi noch nicht gesendet hat gehen verloren
	// Alle Verbindungen werden abgebaut

	int Poll();	// returns the number of open requests

	BOOL 	ListenRequest(LPCTSTR pOwnNumber, BYTE byController=1, DWORD dwServiceMask=0x0000020, DWORD dwInfoMask=0);
	// pOwnNumber:		Pointer auf eingene Rufnummer oder
	// max. 31 Zeichen	LISTEN_REJECTALL:	Alle Anrufe werden rejected
	//					LISTEN_ALL:			Alle Anrufe werden angenommen

	BOOL 	CapiActive();
	// return:	TRUE, Capi4 hat aktive Verbindung, FALSE: keine Verbindung vorhanden

	BOOL 	SendData(LPBYTE lpHdr, LPBYTE lpData=NULL, int iPrior=PRIOR_LOW);
	// Daten an Capi4 senden
	// lpHdr:	Inhält infos über herkunft, länge usw. der Daten
	// lpData:	Pointer auf die Daten
	// iPrior:	Priorität der Daten

	BOOL	SendRawData(LPBYTE lpData, WORD wLen);
	// Daten ohne Header, direkt an Capi senden. Die Daten werden mit PRIOR_HIGH gesendet
	// lpData:	Pointer auf die Daten. (ACHTUNG Capi 1.1: lpData muß ein DOS-Pointer sein)
	// wLen:	Die Länge der Daten
	// return:	FALSE: Daten konnten nicht gesendet werden, sonst TRUE

	LPCTSTR GetVersion();
	// Liefert die Verson des CAPI-Treibers

	LPCTSTR GetManufacturer();
	// Liefert den Manufacturer des CAPI-Treibers

	LPCTSTR GetCapi4Version();
	// Liefert den Manufacturer und die Verson des CAPI4-Modul

	int GetNumOpenRequests() const;

	void	SetConnectionTimeout(DWORD dwTimeout);
	// Setzt die Zeit bis eine Verbindung die keine reaktion zeigt abgebaut wird

	~CCapi();
};


extern BOOL LoadDll();
extern void UnloadDll();
extern CCapi*	pCapi;
////////////////////////////////////////////////////////////////////////////
#endif // CAPI4_H
