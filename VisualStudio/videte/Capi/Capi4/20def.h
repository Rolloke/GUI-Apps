#ifndef __20DEF_H
#define __20DEF_H
/*-----------------------------------------------------------+-------
**                                                           |  ITK
**   SYSTEM   ixEins ISDN-PC-Software CAPI-library           +-------
**
**-------------------------------------------------------------------
**
**   $Workfile:: 20def.h                                    $
**   $Revision:: 7                                          $
**       $Date:: 12/08/05 11:08a                            $
**     $Author:: Rolf.kary-ehlers                           $
**
**
**   ITK
**   Emil-Figge-Strasse 80
**   D-44227 Dortmund
**   Telefon 0231/9747-0
**
**   DESCRIPTION
**     Constants and type definitions for applications using the
**     COMMON-ISDN-API Version 1.1, Version 2.0 or CAPIplus.
**
**-------------------------------------------------------------------
**
** $Log:: /Project/Capi/capi4/20def.h                    $ *
 * 
 * 7     12/08/05 11:08a Rolf.kary-ehlers
 * 
 * 6     23.04.01 14:12 Georg
 * 
 * 5     5.03.01 12:10 Georg
 * 
 * 4     11.09.96 16:22 Hedu
 * 
 * 3     6.09.96 9:08 Georg
 * 
 * 2     4.09.96 21:30 Georg
   
      Rev 1.0   08 Jun 1995 07:58:00   ELLIGER
   Cisco_V1.02_and_basic-SW_V2.42
 *
 *
**-------------------------------------------------------------------*/
//#include "null.h"
// Davon werden nur die drei Defines mehr benötigt
typedef unsigned char           UBYTE;          /*  8 bit unsigned integer */
typedef unsigned short          UWORD;          /* 16 bit unsigned integer */
typedef unsigned long           ULWORD;         /* 32 bit unsigned integer */

#define CAPI_INT_NO             0xf1


#define CAPI20_REGISTER         0x1401
#define CAPI20_RELEASE          0x1402
#define CAPI20_PUT_MESSAGE      0x1403
#define CAPI20_GET_MESSAGE      0x1404
#define CAPI20_SET_SIGNAL       0x1405
#define CAPI20_DEINSTALL        0x1406
#define CAPI20_GET_MANUFACTURER 0x14F0
#define CAPI20_GET_VERSION      0x14F1
#define CAPI20_GET_SERIAL       0x14F2


#define CAPI20_IDENT_LEN               5
#define CAPI20_IDENT_OFFS              6

#define NO_MESS_AVAIL        0x1006

/*----------------*/
/* common defines */
/*----------------*/

#define CAPI20_ALERT               0x01
#define CAPI20_CONNECT             0x02
#define CAPI20_CONNECT_ACTIVE      0x03
#define CAPI20_DISCONNECT          0x04
#define CAPI20_LISTEN              0x05
#define CAPI20_INFO                0x08

/*----------------*/
/* layer2 defines */
/*----------------*/

#define CAPI20_B_PROTOCOL          0x41

/*----------------*/
/* layer3 defines */
/*----------------*/

#define CAPI20_FACILITY            0x80
#define CAPI20_CONNECT_B3          0x82
#define CAPI20_CONNECT_B3_ACT      0x83
#define CAPI20_DISCONNECT_B3       0x84
#define CAPI20_DATA_B3             0x86
#define CAPI20_RESET_B3            0x87
#define CAPI20_CONNECT_B3_T90      0x88

/*-------------------------------*/
/* manufacturer specific defines */
/*-------------------------------*/

#define CAPI20_MANUFACTURER        0xFF


/*------------------------------*/
/* general coding of subcommand */
/*------------------------------*/

#define REQ         0x80
#define CONF        0x81
#define IND         0x82
#define RESP		0x83

/*-------------------------------*/
/* General coding of command     */
/*-------------------------------*/
#define ALERT						0x01
#define CONNECT         			0x02    /* commands */
#define CONNECT_ACTIVE      		0x03
#define DISCONNECT      			0x04
#define LISTEN          			0x05
#define GET_PARAMS      			0x06
#define INFO                    	0x08
#define SELECT_B_PROTOCOL			0x41  
#define FACILITY					0x80
#define LISTEN_B3       			0x81
#define CONNECT_B3      			0x82
#define CONNECT_B3_ACTIVE   		0x83
#define DISCONNECT_B3       		0x84
#define DATA_B3         			0x86
#define RESET_B3					0x87 
#define CONNECT_B3_T90_ACTIVE		0x88
#define DO_DISCONNECT_STUFF			0x89
#define MANUFACTURER        		0xFF

/********************************* CAPI commands and subcommands together *******/
#define CAPI20_ALERT_REQ            0x8001
#define CAPI20_ALERT_CONF           0x8101
#define CAPI20_CONNECT_REQ          0x8002
#define CAPI20_CONNECT_CONF         0x8102
#define CAPI20_CONNECT_IND          0x8202
#define CAPI20_CONNECT_RESP         0x8302
#define CAPI20_CONNECT_ACTIVE_IND   0x8203
#define CAPI20_CONNECT_ACTIVE_RESP  0x8303
#define CAPI20_DISCONNECT_REQ       0x8004
#define CAPI20_DISCONNECT_CONF      0x8104
#define CAPI20_DISCONNECT_IND       0x8204
#define CAPI20_DISCONNECT_RESP      0x8304
#define CAPI20_LISTEN_REQ           0x8005
#define CAPI20_LISTEN_CONF          0x8105
#define CAPI20_INFO_REQ             0x8008
#define CAPI20_INFO_CONF            0x8108
#define CAPI20_INFO_IND             0x8208
#define CAPI20_INFO_RESP            0x8308
#define CAPI20_B_PROTOCOL_REQ       0x8041
#define CAPI20_B_PROTOCOL_CONF      0x8141
#define CAPI20_FACILITY_REQ         0x8080
#define CAPI20_FACILITY_CONF        0x8180
#define CAPI20_CONNECT_B3_REQ       0x8082
#define CAPI20_CONNECT_B3_CONF      0x8182
#define CAPI20_CONNECT_B3_IND       0x8282
#define CAPI20_CONNECT_B3_RESP      0x8382
#define CAPI20_CONNECT_B3_ACT_IND   0x8283
#define CAPI20_CONNECT_B3_ACT_RESP  0x8383
#define CAPI20_DISCONNECT_B3_REQ    0x8084
#define CAPI20_DISCONNECT_B3_CONF   0x8184
#define CAPI20_DISCONNECT_B3_IND    0x8284
#define CAPI20_DISCONNECT_B3_RESP   0x8384
#define CAPI20_DATA_B3_REQ          0x8086
#define CAPI20_DATA_B3_CONF         0x8186
#define CAPI20_DATA_B3_IND          0x8286
#define CAPI20_DATA_B3_RESP         0x8386
#define CAPI20_RESET_B3_REQ         0x8087
#define CAPI20_RESET_B3_CONF        0x8187
#define CAPI20_RESET_B3_IND         0x8287
#define CAPI20_RESET_B3_RESP        0x8387
#define CAPI20_CONNECT_B3_T90_IND   0x8088
#define CAPI20_CONNECT_B3_T90_RESP  0x8188
#define CAPI20_MANUFACTURER_REQ     0x80FF
#define CAPI20_MANUFACTURER_CONF    0x81FF
#define CAPI20_MANUFACTURER_IND     0x82FF
#define CAPI20_MANUFACTURER_RESP    0x83FF


/* ---------------------------------------------------- */
/*   CAPI 2.0 CIP Mask bits                             */
/* ---------------------------------------------------- */
#define CAPI20_CIP_MASK_ANY                0x0001UL
#define CAPI20_CIP_MASK_SPEECH             0x0002UL
#define CAPI20_CIP_MASK_UNREST_DIGITAL     0x0004UL
#define CAPI20_CIP_MASK_REST_DIGITAL       0x0008UL
#define CAPI20_CIP_MASK_56KBIT             0x0100UL
#define CAPI20_CIP_MASK_DTMF               0x0200UL
#define CAPI20_CIP_MASK_X25                0x0080UL
#define CAPI20_CIP_MASK_RESERVED           0x0000UL



/*-----------------------------------------------
            CIP values
-----------------------------------------------*/
#define CAPI20_CIP_NOT_PREDEFINED          0
#define CAPI20_CIP_SPEECH                  1
#define CAPI20_CIP_UNREST_DIGITAL          2
#define CAPI20_CIP_REST_DIGITAL            3
#define CAPI20_CIP_AUDIO31                 4
#define CAPI20_CIP_AUDIO7                  5
#define CAPI20_CIP_VIDEO                   6
#define CAPI20_CIP_X25                     7
#define CAPI20_CIP_DATA56KBIT              8
#define CAPI20_CIP_UNREST_DIGITAL_TONES    9
#define CAPI20_CIP_TELEPHONY              16
#define CAPI20_CIP_FAX_GROUP3             17
#define CAPI20_CIP_FAX_GROUP4             18
#define CAPI20_CIP_MIXED_MODE             19
#define CAPI20_CIP_TELETEX_PROCESS        20
#define CAPI20_CIP_TELETEX_BASIC          21
#define CAPI20_CIP_VIDEOTEX               22
#define CAPI20_CIP_TELEX                  23
#define CAPI20_CIP_MSG_HANDL_X400         24
#define CAPI20_CIP_OSI_APPL_X200          25
#define CAPI20_CIP_TELEPHONY_7KHZ         26
#define CAPI20_CIP_VIDEO_TELEPHONY_1      27
#define CAPI20_CIP_VIDEO_TELEPHONY_2      28


/* ---------------------------------------------------- */
/*   CAPI 2.0 Reject values                             */
/* ---------------------------------------------------- */
#define CAPI20_REJECT_ACCEPT               0
#define CAPI20_REJECT_IGNORE               1
#define CAPI20_REJECT_CLEAR                2
#define CAPI20_REJECT_BUSY                 3
#define CAPI20_REJECT_NOCHANNEL            4
#define CAPI20_REJECT_FACILITY             5
#define CAPI20_REJECT_CHANNEL              6
#define CAPI20_REJECT_DESTINATION          7
#define CAPI20_REJECT_OUTOFORDER           8

/*-----------------------------------------------
            B1 protocols
-----------------------------------------------*/
#define CAPI20_B1_HDLC                     0
#define CAPI20_B1_TRANSPARENT              1
#define CAPI20_B1_V110_SYNC                2
#define CAPI20_B1_V110_ASYNC               3
#define CAPI20_B1_T30                      4
#define CAPI20_B1_HDLC_INV                 5
#define CAPI20_B1_56KBIT_TRANSPARENT       6
#define CAPI20_B1_56KBIT_HDLC              7


/*-----------------------------------------------
            B2 protocols
-----------------------------------------------*/
#define CAPI20_B2_X75                      0
#define CAPI20_B2_TRANSPARENT              1
#define CAPI20_B2_SDLC                     2
#define CAPI20_B2_Q921                     3
#define CAPI20_B2_T30                      4
#define CAPI20_B2_PPP                      5
#define CAPI20_B2_TRANSPARENT_ERRORS       6


/*-----------------------------------------------
            B3 protocols
-----------------------------------------------*/
#define CAPI20_B3_TRANSPARENT              0
#define CAPI20_B3_T90                      1
#define CAPI20_B3_X25DTE                   2
#define CAPI20_B3_X25DCE                   3
#define CAPI20_B3_T30                      4


/*-----------------------------------------------
            INFO values (error codes)
----------------------------------------------*/
#define CAPI20_INFO_ILLEGAL_APPL           0x1101
#define CAPI20_INFO_ILLEGAL_COMMAND        0x1102
#define CAPI20_INFO_SEND_QUEUE_FULL        0x1103
#define CAPI20_INFO_RCV_QUEUE_EMPTY        0x1104
#define CAPI20_INFO_RCV_QUEUE_OVERFLOW     0x1105
#define CAPI20_INFO_INTERNAL_BUSY          0x1107
#define CAPI20_INFO_RESOURCE_ERROR         0x1108


/*-----------------------------------------------
            Flags in DATA_B3 mesages
----------------------------------------------*/
#define CAPI20_FLAG_QUALIFIER              0x0001
#define CAPI20_FLAG_MORE_DATA              0x0002
#define CAPI20_FLAG_DELIVERY_CONF          0x0004
#define CAPI20_FLAG_EXPEDITED_DATA         0x0008
#define CAPI20_FLAG_FRAMING_ERROR          0x8000


/*-----------------------------------------------
            Info_Mask in LISTEN_REQ
----------------------------------------------*/
#define CAPI20_INFOMASK_CAUSE              0x0001
#define CAPI20_INFOMASK_TIME               0x0002
#define CAPI20_INFOMASK_DISPLAY            0x0004
#define CAPI20_INFOMASK_USERUSER           0x0008
#define CAPI20_INFOMASK_PROGRESS           0x0010
#define CAPI20_INFOMASK_FACILITY           0x0020
#define CAPI20_INFOMASK_CHARGING           0x0040


/*-----------------------------------------------
            Info_Number in INFO_IND
----------------------------------------------*/
#define CAPI20_INFO_CHARGE_UNITS           0x8000
#define CAPI20_INFO_CHARGE_CURRENCY        0x8001


/*----------------------------------------------*/
/* definition of the ConnectMask bits           */
/*----------------------------------------------*/
#define C_MASK_NO_CONF      0x01        /* no db3 confirm */

/*----------------------------------------------*/
/* definition of the Reject Codes               */
/*----------------------------------------------*/
#define CONNECT_ACCEPT      0x00
#define CONNECT_IGNORE      0x01               /* exchanged IGNORE and REJECT due to spec 6.10.94 F.H. */
#define CONNECT_REJECT      0x02


/*----------------------------------------------*/
/* definition of info codes (1TR6)              */
/*----------------------------------------------*/
#define INFO_CAUSE             0x0008
#define INFO_CALL_STATE        0x0018
#define INFO_DISPLAY           0x0024
#define INFO_DEST_ADDRESS      0x0070
#define INFO_USER_USER         0x007e
#define INFO_CHARGING          0x0602
#define INFO_DATE              0x0603
#define INFO_STATE_CALLED_PART 0x0607   


/*----------------------------------------------*/
/* definition of B2 protocol types              */
/*----------------------------------------------*/
#define B2_X75_SLP              0x01
#define B2_HDLC_TRANSPARENT     0x02
#define B2_BITTRANSPARENT       0x03
#define B2_SNA_SDLC             0x04
#define B2_X75_BTX              0x05
#define B2_T30_FAX              0x06
#define B2_V110                 0x08
#define B2_BITTRANS_SEND_ONLY   0x0b
#define B2_MODEM                0x0c
#define B2_PPP                  0x0d


/*----------------------------------------------*/
/* definition of B3 protocol types              */
/*----------------------------------------------*/
#define B3_T70_NL               0x01
#define B3_ISO_8208             0x02
#define B3_T90                  0x03
#define B3_TRANSPARENT          0x04
#define B3_T30_FAX              0x05

/*----------------------------------------------*/
/* definition of compression mode               */
/*----------------------------------------------*/
#define COMP_WANTED             0x00
#define COMP_FORCED             0x01

/*----------------------------------------------*/
/* definition of compression select             */
/*----------------------------------------------*/
#define COMP_NONE               0x00
#define COMP_V42bis             0x01
#define COMP_JPEC               0x02
/*----------------------------------------------*/
/* definition of channel mode                   */
/*----------------------------------------------*/
#define CHANNEL_ADD             1
#define CHANNEL_DEL             2

/* -------------------------------------------- */
/* definition of FAX select                     */
/* -------------------------------------------- */
#define FAX_FORM_SFF            0
#define FAX_FORM_PLAIN          1
#define FAX_FORM_PCX            2
#define FAX_FORM_DCX            3
#define FAX_FORM_TIFF           4
#define FAX_FORM_ASCII          5
#define FAX_FORM_ANSI           6
#define FAX_FORM_BINARY         7

#define FAX_RES_STANDARD        0
#define FAX_RES_FINE            1

#define FAX_SPEED_MAX           0

/* --------------------------------------------- */
/* adapter parameter types                       */
/* --------------------------------------------- */
#define ADAPPARMS_COMPRESSION   0x00
#define ADAPPARMS_LICENSE       0x01
#define ADAPPARMS_LINE          0x02
#define ADAPPARMS_SECURITY      0x03
#define ADAPPARMS_HARDWARE      0x04
#define ADAPPARMS_SEC_DEFAULT   0x05
#define ADAPPARMS_SOFTWARE      0x06
#define ADAPPARMS_SPID          0x07

/* --------------------------------------------- */
/* D-Channel protocols (ADAPTER_PARAMS)          */
/* --------------------------------------------- */
#define DCHANNEL_1TR6           0x00
#define DCHANNEL_DSS1           0x01
#define DCHANNEL_VN2            0x02
#define DCHANNEL_VN3            0x03
#define DCHANNEL_NORWAY         0x08



/* --------------------------------------------- */
/* definition of software types (ADAPTER_PARAMS) */
/* --------------------------------------------- */
#define SWTYPE_MASK_BASIC        0x0001
#define SWTYPE_MASK_OCTO         0x0002
#define SWTYPE_MASK_PRIMARY      0x0004
#define SWTYPE_MASK_SNMP         0x0020
#define SWTYPE_MASK_L3PKT        0x0040
#define SWTYPE_MASK_MLP          0x0080
#define SWTYPE_MASK_FAX          0x0100
#define SWTYPE_MASK_MODEM        0x0200
#define SWTYPE_MASK_MVIP         0x0400
#define SWTYPE_MASK_SAS          0x0800


/* ---------------------------------------------------- */
/* definition of hardware type symbols (ADAPTER_PARAMS) */
/* ---------------------------------------------------- */
#define HWTYPE_MASK_DEBUGPORT    0x01
#define HWTYPE_PRIM_VOICE        0x01
#define HWTYPE_PRIM_DATA         0x02
#define HWTYPE_PRIM_POWER        0x03


/* ---------------------------------------------------- */
/* definition of debug type symbols (DEBUG Messages)    */
/* ---------------------------------------------------- */
#define DEBUG_DCHANNEL           0x00
#define DEBUG_ASCII              0x01



/* --------------------------------------------- */
/* definition of some important CAPI error codes */
/* --------------------------------------------- */
#define CAPIERR_WRONG_CONTR      0x2001
#define CAPIERR_NO_MVIP_SUP      0xF400
#define CAPIERR_SAS_MISSING      0xF000
#define CAPIERR_ENCRALGO_NOT_SUP 0xF001
#define CAPIERR_HASHALGO_NOT_SUP 0xF002
#define CAPIERR_PEER_NO_SAS      0xF010
#define CAPIERR_OWN_SAS_ERROR    0xF011
#define CAPIERR_OWN_AUTH         0xF012
#define CAPIERR_OWN_ID_REJECT    0xF013
#define CAPIERR_OWN_DATA_ERR     0xF014
#define CAPIERR_OWN_ALGO_ERR     0xF015
#define CAPIERR_PEER_AUTH        0xF022
#define CAPIERR_PEER_ID_REJECT   0xF023
#define CAPIERR_PEER_DATA_ERR    0xF024
#define CAPIERR_PEER_ALGO_ERR    0xF025



/* -------------------------------------------- */
/* definition of 1TR6 causes                    */
/* -------------------------------------------- */
#define CAUSE_NULL                      0
#define CAUSE_INVALID_CRN               1
#define CAUSE_BEARER_SVC_NOT_IMPL       3
#define CAUSE_CALL_IDENT_NOT_EXIST      7
#define CAUSE_CALL_IDENT_IN_USE         8
#define CAUSE_NO_CHANNEL_AVAILABLE      10
#define CAUSE_REQUESTED_FAC_NOT_IMPL    16
#define CAUSE_REQUESTED_FAC_NOT_SUB     17
#define CAUSE_OUTGOING_CALLS_BARRED     32
#define CAUSE_USER_ACCESS_BUSY          33
#define CAUSE_NEGATIV_GBG_VERGLEICH     34
#define CAUSE_NON_EXISTENT_CUG          35
#define CAUSE_KOMMUN_SPV_VERBOTEN       37
#define CAUSE_REV_CHARG_NOT_ALLOW_ORG   48
#define CAUSE_REV_CHARG_NOT_ALLOW_DES   49
#define CAUSE_REVERSE_CHARGING_REJ      50
#define CAUSE_DEST_NOT_OBTAINABLE       53
#define CAUSE_NUMBER_CHANGED            56
#define CAUSE_OUT_OF_ORDER              57
#define CAUSE_NO_USER_RESPONDING        58
#define CAUSE_USER_BUSY                 59
#define CAUSE_INCOMING_CALLS_BARRED     61
#define CAUSE_CALL_REJ                  62
#define CAUSE_NETWORK_CONGESTION        89
#define CAUSE_REMOTE_USER_INIT          90
#define CAUSE_LOCAL_PROCEDURE_ERROR    112
#define CAUSE_REMOTE_PROCEDURE_ERROR   113
#define CAUSE_USER_SUSPENDED           114
#define CAUSE_REMOTE_USER_RESUMED      115
#define CAUSE_USER_INFO_DISCARDED      127
/*user defined causes*/
#define CAUSE_MANDATORY_MISSING        128
#define CAUSE_BAD_INFOELEM             129
#define CAUSE_BAD_INFOELEM_CONTENS     130
#define CAUSE_INFOELEM_TOSHORT         131
#define CAUSE_ID_CHANNEL_NOT_EXIST     132
#define CAUSE_INVALID_DEST_ADDRS       133
#define CAUSE_CIRCUIT_OUT_OF_ORDER     134
#define CAUSE_MSG_BAD_IN_CALL_STATE    135
#define CAUSE_REJECTED_EVENT           136
#define CAUSE_LOCAL_PROCEDURE_TIMEOUT  137
#define CAUSE_TEI_REMOVED              138

#define CAPI20_HDRLEN					8
#define MAXINT 							0x7FFF

/*----------------------------------------------*/
/* definition of important structures           */
/*----------------------------------------------*/
typedef struct
{
   UBYTE   byLength;
   UWORD   wRate;            /* transmission rate */
   UWORD   wBitsPerChar;     /* bits per character */
   UWORD   wParity;
   UWORD   wStopBits;
} B1_CONFIG;


typedef struct
{
   UBYTE   byLength;
   UBYTE   byLink_add_a;      /* link address a */
   UBYTE   byLink_add_b;      /* link address b */
   UBYTE   byModulo_mode;     /* Modulo mode (B2 packet numbering */
   UBYTE   byWindow_size;     /* number of unacknowledged B2 packets */
   UBYTE   byXid_length;      /* Length of XID-Block (normally 0) */
} B2_CONFIG;


typedef struct
{
   	UBYTE   byLength;
	UWORD   lic;
	UWORD   hic;
	UWORD   ltc;
	UWORD   htc;
	UWORD   loc;
	UWORD   hoc;
	UWORD   modulo_mode;
    UWORD 	window_size;
} B3_CONFIG;
typedef B3_CONFIG FAR* LPB3_CONFIG;

typedef struct {   
	BYTE		byLength;
	WORD		wB1Protocol;
	WORD		wB2Protocol;
	WORD		wB3Protocol;
	char		B[sizeof(B1_CONFIG)+sizeof(B2_CONFIG)+sizeof(B3_CONFIG)]; 
} B_PROTOCOL;
typedef B_PROTOCOL FAR* LPB_PROTOCOL;

typedef struct { 
	BYTE		bySize;
    BYTE       	byType;
    char        szNumber[32];
} CALLED_PARTY_NUM;
typedef CALLED_PARTY_NUM FAR* LPCALLED_PARTY_NUM;
	
typedef struct {
	BYTE		bySize;
    BYTE       	byType;
    BYTE       	byPsi;
    char        szNumber[32];
} CALLING_PARTY_NUM;
typedef CALLING_PARTY_NUM FAR* LPCALLING_PARTY_NUM;
	
typedef struct {
	BYTE		bySize;
    UWORD       wType;
    char*       pSubaddress;
} CALLED_PARTY_SUBADR;
	
typedef struct {
	BYTE		bySize;
    UWORD       wType;
    char*       pSubaddress;
} CALLING_PARTY_SUBADR;
	
/*----------------------------------------------*/
/* definition of message bodies                 */
/*----------------------------------------------*/


typedef struct
{
   ULWORD      dwPlci ;            /* network connection ident. */
   UBYTE       addInfoLen;       /* length of additional information */
   UBYTE       addInfo[100];
} CAPI20_ALERT_REQ_BODY;

typedef struct
{
   ULWORD      dwPlci ;            /* network connection ident. */
   UWORD       info ;            /* return code */
} CAPI20_ALERT_CONF_BODY;

/*-------------------------------------------------------------------------*/

typedef struct
{
   UWORD  dwNcci ;            /* layer 3 identification */
} CAPI20_RESET_REQ_BODY;

typedef struct
{
   UWORD  dwNcci ;            /* layer 3 identification */
   UWORD  info;             /* return code */
} CAPI20_RESET_CONF_BODY;

typedef struct
{
   UWORD  dwNcci ;            /* network connection ident. */
} CAPI20_RESET_IND_BODY;

typedef struct
{
   UWORD  dwNcci ;            /* layer 3 identification */
} CAPI20_RESET_RESP_BODY;

/*-------------------------------------------------------------------------*/


typedef struct
{
   ULWORD   dwController ;      /* controller identification */
   UWORD    wCipValue;
   UBYTE    rest[100] ;       /* rest of the message */
} CAPI20_CON_REQ_BODY;

typedef struct
{
   ULWORD   dwPlci;             /* physical link connection identifier */
   UWORD    info;             /* return code */
} CAPI20_CON_CONF_BODY;

typedef struct
{
   ULWORD   dwPlci;             /* physical link connection identifier */
   UWORD    cipValue;
   UBYTE    rest[100] ;       /* rest of the message */
} CAPI20_CON_IND_BODY;

typedef struct
{
   ULWORD   dwPlci ;            /* physical connection ident. */
   UWORD    reject ;          /* accept or reject call */
   UBYTE    rest[100] ;       /* rest of the message */
} CAPI20_CON_RESP_BODY;


/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD  dwPlci ;            /* physical connection ident. */
   UBYTE   conNumberLen;     /* length of connected address */
   UBYTE   conNumber[100];   /* connected address */
} CAPI20_ACT_IND_BODY;

typedef struct
{
   ULWORD  dwPlci ;            /* physical connection ident. */
} CAPI20_ACT_RESP_BODY;

/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD  dwPlci ;            /* physical connection ident. */
   UBYTE   addInfoLen;       /* length of additional information */
   UBYTE   addInfo[100];     /* additional information */
} CAPI20_DISC_REQ_BODY;

typedef struct
{
   ULWORD  dwPlci ;            /* physical connection ident. */
   UWORD   info ;            /* return code */
} CAPI20_DISC_CONF_BODY;

typedef struct
{
   ULWORD  dwPlci ;            /* physical connection ident. */
   UWORD   wReason;           /* disconnection cause */
} CAPI20_DISC_IND_BODY;

typedef struct
{
    ULWORD  dwPlci ;            /* physical connection ident. */
} CAPI20_DISC_RESP_BODY;

/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD dwController;       /* controller identification */
   ULWORD dwInfomask;         /* info mask */
   ULWORD dwCipMask;
   ULWORD dwCipMask2;
   UBYTE  callingParty[100];
} CAPI20_LIS_REQ_BODY;

typedef struct
{
   ULWORD controller ;       /* controller identification */
   UWORD  info ;             /* return code */
} CAPI20_LIS_CONF_BODY;


/*-------------------------------------------------------------------------*/


typedef struct
{
   ULWORD   dwPlci ;            /* physical connection identifier */
   UBYTE    calledPartyLen;
   UBYTE    calledParty[100];
} CAPI20_INFO_REQ_BODY;

typedef struct
{
   ULWORD   dwPlci ;             /* physical connection identifier */
   UWORD    info ;             /* return code */
} CAPI20_INFO_CONF_BODY;

typedef struct
{
   ULWORD   dwPlci ;             /* physical connection identifier */
   UWORD    info_no ;          /* info number  */
   UBYTE    len ;              /* length of following structure */
   UBYTE    info_string[50];   /* information octetts depending on W-Element */
} CAPI20_INFO_IND_BODY;

typedef struct
{
   ULWORD   dwPlci ;             /* physical connection identifier */
} CAPI20_INFO_RESP_BODY;

/*-------------------------------------------------------------------------*/


typedef struct
{
   ULWORD     dwPlci ;            /* physical connection identifier */
   UBYTE      bLen;             /* length of b protocol struct */
   UWORD      b1Prot;
   UWORD      b2Prot;
   UWORD      b3Prot;
   UBYTE      b1B2B3Structs[100];
} CAPI20_SEL_B_REQ_BODY;

typedef struct
{
   ULWORD     dwPlci ;             /* physical connection identifier */
   UWORD      info ;             /* return code */
} CAPI20_SEL_B_CONF_BODY;


/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD     dwPlci ;            /* physical connection identifier */
   UBYTE      byNcpilen ;         /* length of ncpi structure */
   UBYTE      ncpi[100];
} CAPI20_CON_B3_REQ_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
   UWORD      info ;            /* return code */
} CAPI20_CON_B3_CONF_BODY;

typedef struct
{
   ULWORD     dwNcci;             /* network control connection identifier */
   UBYTE      ncpilen ;         /* length of ncpi structure */
   UBYTE      ncpi[100];
} CAPI20_CON_B3_IND_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* physical connection ident. */
   UWORD      wReject ;          /* accept or reject call */
   UBYTE      byNcpilen     ;     /* length of network control protocol information */
   UBYTE      ncpi[100];
} CAPI20_CON_B3_RESP_BODY;

/*-------------------------------------------------------------------------*/


typedef struct
{
   ULWORD     dwNcci ;         /* network control connection identifier */
   UBYTE      ncpilen ;      /* length of network control protocol information */
   UBYTE      ncpi[100];     /* network control protocol information */
} CAPI20_CON_B3_ACT_IND_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
} CAPI20_CON_B3_ACT_RESP_BODY;

/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
   UBYTE      byNcpilen ;         /* length of ncpi structure */
   UBYTE      ncpi[100];        /* length of ncpi structure */
} CAPI20_DISC_B3_REQ_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
   UWORD      wInfo ;            /* return code */
} CAPI20_DISC_B3_CONF_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
   UWORD      wReason;           /* cause for disconnect */
   UBYTE      byNcpilen ;         /* length of ncpi structure */
   UBYTE      ncpi[100];        /* length of ncpi structure */
} CAPI20_DISC_B3_IND_BODY;

typedef struct
{
   ULWORD     dwNcci ;            /* network control connection identifier */
} CAPI20_DISC_B3_RESP_BODY;

/*-------------------------------------------------------------------------*/

typedef struct
{
   ULWORD      	dwNcci ;          /* network connection ident. */
   BYTE FAR*	ptr ;         /* pointer to data to be transmitted */
   UWORD       	length ; 	     /* length of listen b3 params */
   UWORD      	number ;     /* datablock number */
   UWORD       	flags ;
} CAPI20_DATA_B3_REQ_BODY;

typedef struct
{
   ULWORD      dwNcci ;            /* network connection ident. */
   UWORD       number ;     /* datablock number */
   UWORD       info ;            /* return code */
} CAPI20_DATA_B3_CONF_BODY;

typedef struct
{
   ULWORD       dwNcci ;            /* network connection ident. */
   BYTE FAR*	ptr ;    /* pointer to received data */
   UWORD       	data_length ;     /* size of received B3 data block */
   UWORD       	number ;     /* datablock number */
   UWORD       	flags ;
} CAPI20_DATA_B3_IND_BODY;

typedef struct
{
   ULWORD      dwNcci ;            /* network connection ident. */
   UWORD       wNumber ;     /* datablock number */
} CAPI20_DATA_B3_RESP_BODY;







/*---------------------------------------------------------
            Definition of message type
--------------------------------------------------------- */
typedef struct
{
	UWORD  wTotal_length ;    /* total length of message */
	UWORD  wApplid ;         /* application id */
	UBYTE  command ;         /* api command */
	UBYTE  subcommand ;      /* api subcommand */
  	UWORD  wMessagenumber ;   /* unique messagenumber */

  	union 
  	{
		CAPI20_RESET_REQ_BODY       resetReq;
		CAPI20_RESET_CONF_BODY      resetConf;
		CAPI20_RESET_IND_BODY       resetInd;
		CAPI20_RESET_RESP_BODY      resetResp;
		CAPI20_CON_REQ_BODY         conReq;
		CAPI20_CON_CONF_BODY        conConf;
		CAPI20_CON_IND_BODY         conInd;
		CAPI20_CON_RESP_BODY        conResp;
		CAPI20_ACT_IND_BODY         actInd;
		CAPI20_ACT_RESP_BODY        actResp;
		CAPI20_DISC_REQ_BODY        discReq;
		CAPI20_DISC_CONF_BODY       discConf;
		CAPI20_DISC_IND_BODY        discInd;
		CAPI20_DISC_RESP_BODY       discResp;
		CAPI20_LIS_REQ_BODY         lisReq;
		CAPI20_LIS_CONF_BODY        lisConf;
		CAPI20_INFO_REQ_BODY        infoReq;
		CAPI20_INFO_CONF_BODY       infoConf;
		CAPI20_INFO_IND_BODY        infoInd;
		CAPI20_INFO_RESP_BODY       infoResp;
		CAPI20_SEL_B_REQ_BODY       selBReq;
		CAPI20_SEL_B_CONF_BODY      selBConf;
		CAPI20_CON_B3_REQ_BODY      conB3Req;
		CAPI20_CON_B3_CONF_BODY     conB3Conf;
		CAPI20_CON_B3_IND_BODY      conB3Ind;
		CAPI20_CON_B3_RESP_BODY     conB3Resp;
		CAPI20_CON_B3_ACT_IND_BODY  conB3ActInd;
		CAPI20_CON_B3_ACT_RESP_BODY conB3ActResp;
		CAPI20_DISC_B3_REQ_BODY     discB3Req;
		CAPI20_DISC_B3_CONF_BODY    discB3Conf;
		CAPI20_DISC_B3_IND_BODY     discB3Ind;
		CAPI20_DISC_B3_RESP_BODY    discB3Resp;
		CAPI20_DATA_B3_REQ_BODY     dataB3Req;
		CAPI20_DATA_B3_CONF_BODY    dataB3Conf;
		CAPI20_DATA_B3_IND_BODY     dataB3Ind;
		CAPI20_DATA_B3_RESP_BODY    dataB3Resp;
		CAPI20_ALERT_REQ_BODY       alertReq;
		CAPI20_ALERT_CONF_BODY      alertConf;
		char                        c[200];
    }  body;
} CAPI20_MSG, FAR * LPCAPI20_MSG;

/*-----------------------------------------------*/
/* queue definitions, incl. receive data partion */
/*-----------------------------------------------*/

//#define     MAX_UPMSG_LEN   180 	/* max. length of userpart msg        	*/
#define     MY_N_MSG        1024	/* # of CIA msgs                      	*/
#define     MY_N_XBUF       4		/* Wenn >5 kann CapiQ overflow zeigen.	*/
#define     MY_N_B3_CON 	2   	/* # of layer 3 connections       		*/
#define     MY_BLK_LEN      ((WORD)2048) 	/* Länge der Datenblöcke              	*/
//#define     MY_DPART_LEN     (MY_N_B3_CON * MY_N_XBUF * MY_BLK_LEN) 	// +2 für steuerbits
//#define     MY_Q_LEN      	((DWORD)(MAX_UPMSG_LEN*MY_N_MSG) + (DWORD)MY_DPART_LEN)

#endif /* __20DEF_H */





