#ifndef _INC_CAPIEXDF
#define _INC_CAPIEXDF
/*****************************************************************************/
/*                                       */
/*    Projekt:      COMMON-IDSN-API (CAPI)                   */
/*                                       */
/*    File:         CAPIEXDF.H                           */
/*                                       */
/*    Autor:        Manfred Jung                         */
/*                                       */
/*    Erstellt:     29.05.1990                           */
/*                                       */
/*    Copyright:    Sto//mann Gmbh, Hamburg                  */
/*                                                   */
/*    Funktionen:   External definitions for CAPI (message format, function  */
/*          codes, commands and subcommands .. )             */
/*                                       */
/*   >>>>>> NOTE: BYTE alignment is required for compilation !!!  <<<<   */
/*                                       */
/*****************************************************************************/


typedef     void (far *f_type)();
typedef     int (*drv_type)();


#define 	FIRST_B_CHANNEL 	0x81
#define 	SECOND_B_CHANNEL    0x82
#define 	NEXT_B_CHANNEL	    0x83       

/*#define     API_REGISTER        0x01
#define     API_RELEASE     0x02
#define     API_PUT_MESSAGE     0x03
#define     API_GET_MESSAGE     0x04
#define     API_SET_SIGNAL      0x05

#define     API_GET_MANUFACTURER    0xf0
#define     API_GET_VERSION     0xf1
#define     API_GET_SERIAL_NUMBER   0xf2

#define     API_MANUFACTURER    0xff
*/


        /* CAPI commands and subcommands */

#define RESET_B3                0x01
#define CONNECT         0x02    /* commands */
#define CONNECT_ACTIVE      0x03
#define DISCONNECT      0x04
#define LISTEN          0x05
#define GET_PARAMS      0x06
#define INFO                    0x07

#define SELECT_B2_PROTOCOL  0x40

#define SELECT_B3_PROTOCOL  0x80
#define LISTEN_B3       0x81
#define CONNECT_B3      0x82
#define CONNECT_B3_ACTIVE   0x83
#define DISCONNECT_B3       0x84
#define DATA_B3         0x86

#define HANDSET         0x87

#define MANUFACTURER        0xff

#define REQ     0x00    /* subcommands, types */
#define CONF        0x01
#define IND     0x02
#define RESP        0x03

        /*+++ definitions for HANDSET_IND +++*/

#define HANDSET_OFF_HOOK    '+'
#define HANDSET_ON_HOOK     '-'

        /*+++ definitions for MANUFACTURER_IND +++*/

#define MNF_BOARD_RESET     0xff

typedef union {             /* parms. for IND */
    BYTE    controller;
} MNF_I_PARM;

typedef MNF_I_PARM  MNF_R_PARM; /* parms. for RESP */


        /*+++ message types +++*/

#define     REQ_TYPE(m) (m->subcmd == REQ)
#define     CONF_TYPE(m)    (m->subcmd == CONF)
#define     IND_TYPE(m) (m->subcmd == IND)
#define     RESP_TYPE(m)    (m->subcmd == RESP)


        /*+++ service indicators and add. info. octets +++*/

#define     SIN_TEL         0x01    /* telephone        */
#define     SIN_A_B         0x02    /* a/b services     */
#define     SIN_X21         0x03    /* X.21 services    */
#define     SIN_FAX4        0x04    /* Telefax group 4  */
#define     SIN_BTX         0x05
#define     SIN_DATA64K     0x07    /* file transfer    */
#define     SIN_X25         0x08    /* X.25 services    */
#define     SIN_TTX64       0x09    /* Teletex 64       */
#define     SIN_MIXMODE     0x0a    /* Mixed Mode       */
#define     SIN_TELMTC      0x0d    /* Telematic        */
#define     SIN_GRPHTEL     0x0e    /* Graphic telephone    */
#define     SIN_BTX_NEW     0x0f    /* BTX new standard */
#define     SIN_BILDTEL     0x10    /* Bildtelefon      */

#define     AIO_TEL         0x01
#define     AIO_A_B         0x02    /* data via modem   */
#define     AIO_X21         0x04    /* UC 4         */
#define     AIO_FAX4        0x00
#define     AIO_BTX         0x00
#define     AIO_DATA64K     0x00
#define     AIO_X25         0x06    /* 19,2 KB      */
#define     AIO_TTX64       0x00
#define     AIO_MIXMODE     0x00
#define     AIO_TELMTC      0x00
#define     AIO_GRPHTEL     0x00
#define     AIO_BTX_NEW     0x00
#define     AIO_BILDTEL     0x03



    /*+++       layer 2 definitions              +++*/
    /*+++ data link protocol descriptions and identifiers (DLPD) +++*/

typedef struct {        /* XID */
    BYTE    len;        /* parameter length */
} XID;
typedef XID far *   XIDP;

typedef struct {        /* layer 2 description  */
    BYTE    len;        /* parameter length */
    WORD    blksize;    /* block size       */
    BYTE    link_addr_a;
    BYTE    link_addr_b;
    BYTE    modulo;     /* frame numbering  */
    BYTE    wdwsize;    /* window size      */
    XID xid;
} B2D;
typedef B2D far *   B2DP;

typedef B2D X75;        /* X.75/LAP-B       */
typedef X75 far *   X75P;


typedef union {
    BYTE    len;        /* length       */
    B2D b2d;
    X75 x75;
} DLPD;
typedef DLPD far *  DLPDP;


        /* B2 protocol IDs */

#define     B2_X75_SLP  0x01        /* X.75 SLP ..      */
#define     B2_TRANSP   0x02        /* transparent HDLC with bit */
                        /* stuffing, CRC ..      */
#define     B2_BIT_TRANSP   0x03        /* bit transparent  */
#define     B2_SNA_SDLC 0x04        /* SDLC         */
#define     B2_X75_BTX  0x05        /* X.75 BTX ..      */


        /*+++       layer 3 definitions       +++*/
        /*+++ network control protocol information (NCPI) +++*/


typedef union {
    BYTE        len;        /* length       */
} NCPI;
typedef NCPI far *  NCPIP;


        /*+++ network control protocol description (NCPD) +++*/

typedef struct {        /* layer 3 description  */
    BYTE    len;        /* parameter length */
    WORD    lic;        /* least incoming channel */
    WORD    hic;
    WORD    ltc;
    WORD    htc;
    WORD    loc;
    WORD    hoc;
    BYTE    modulo;     /* frame numbering  */
} B3D;
typedef B3D far *   B3DP;

typedef union {
    BYTE    len;        /* length       */
    B3D b3d;
} NCPD;
typedef NCPD far *  NCPDP;


        /* B3 protocol IDs */

#define     B3_T70_NL   0x01        /* T70 NL       */
#define     B3_ISO_8208 0x02        /* ISO 8208 DTE/DTE */
#define     B3_T90_APP_II   0x03        /* T90, appendix II */
#define     B3_TRANSP   0x04        /* transparent      */



        /*++++*+++++++++++++++++++++++++++*/
        /*+++ CAPI message definitions +++*/
        /*++++*+++++++++++++++++++++++++++*/

#define MAX_NUM_LEN 22  /* maximum length of dest./orig. address */

typedef struct {            /* destination address  */
    BYTE    len;            /* length       */
    BYTE    type;           /* type of number   */
    BYTE    num[ MAX_NUM_LEN ]; /* ISDN number      */
} DST_ADDR;
typedef DST_ADDR    far * DST_ADDRP;

typedef DST_ADDR    CLR_ADDR;   /* caller address   */
typedef CLR_ADDR    far * CLR_ADDRP;

typedef DST_ADDR    CON_ADDR;   /* connected address    */
typedef CON_ADDR    far * CON_ADDRP;


typedef struct {
    WORD ncci;
    } P_RESET_B3_REQ;
typedef struct {
    WORD ncci;
    WORD info;
    } P_RESET_B3_CONF;

typedef struct {            /* parameters for cmd CONNECT/REQ */
    BYTE        controller;
    BYTE        b_channel;
    DWORD       info_mask;
    BYTE        srv;
    BYTE        aio;
    BYTE        eaz;
    DST_ADDR    dst_addr;
} P_CON_REQ;
typedef P_CON_REQ   far * P_CON_REQP;

typedef struct {            /* parameters for cmd CONNECT/CONF */
    WORD    plci;
    WORD    info;
} P_CON_CONF;
typedef P_CON_CONF  far * P_CON_CONFP;

typedef struct {            /* parms. for cmd CONNECT_ACTIVE/IND */
    WORD        plci;
    CON_ADDR    con_addr;
} P_CONACT_IND;
typedef P_CONACT_IND    far * P_CONACT_INDP;

typedef struct {            /* parms. for cmd CONNECT_ACTIVE/RESP */
    WORD    plci;
} P_CONACT_RESP;
typedef P_CONACT_RESP   far * P_CONACT_RESPP;

typedef struct {            /* parameters for cmd CONNECT/IND */
    WORD        plci;
    BYTE        controller;
    BYTE        req_srv;
    BYTE        req_aio;
    BYTE        req_eaz;
    CLR_ADDR    clr_addr;
} P_CON_IND;
typedef P_CON_IND   far * P_CON_INDP;

typedef struct {            /* parameters for cmd CONNECT/RESP */
    WORD    plci;
    BYTE    reject;
} P_CON_RESP;
typedef P_CON_RESP  far * P_CON_RESPP;

typedef struct {            /* parameters for cmd DISCONNECT/REQ */
    WORD    plci;
    BYTE    cause;
} P_DCON_REQ;
typedef P_DCON_REQ  far * P_DCON_REQP;

typedef P_CON_CONF  P_DCON_CONF;    /* parameters for cmd DISCONNECT/CONF */
typedef P_DCON_CONF far * P_DCON_CONFP;

typedef P_DCON_CONF P_DCON_IND; /* parameters for cmd DISCONNECT/IND */
typedef P_DCON_IND  far * P_DCON_INDP;

typedef P_CONACT_RESP   P_DCON_RESP;    /* parameters for cmd DISCONNECT/RESP */
typedef P_DCON_RESP far * P_DCON_RESPP;

typedef struct {            /* parameters for cmd LISTEN/REQ */
    BYTE        controller;
    DWORD       info_mask;
    WORD        srv_eaz_mask;
    WORD        srv_si_mask;
} P_LISTEN_REQ;
typedef P_LISTEN_REQ    far * P_LISTEN_REQP;

#define BIT_SI_TEL  2       /* bit srv "teleph." in srv_si_mask */

typedef struct {            /* parameters for cmd LISTEN/CONF */
    BYTE        controller;
    WORD        info;
} P_LISTEN_CONF;
typedef P_LISTEN_CONF   far * P_LISTEN_CONFP;


typedef struct {            /* parameters for cmd CONNECT_B3/REQ */
    WORD    plci;
    NCPI    ncpi;
} P_CONB3_REQ;
typedef P_CONB3_REQ far * P_CONB3_REQP;

typedef struct {            /* parameters for cmd CONNECT_B3/CONF */
    WORD    plci;
    WORD    ncci;
    WORD    info;
} P_CONB3_CONF;
typedef P_CONB3_CONF    far * P_CONB3_CONFP;

typedef struct {            /* parms. for CONNECT_B3_ACTIVE/IND */
    WORD    ncci;
    NCPI    ncpi;
} P_CONACTB3_IND;
typedef P_CONACTB3_IND  far * P_CONACTB3_INDP;

typedef struct {            /* parms. for CONNECT_B3_ACTIVE/RESP */
    WORD    ncci;
} P_CONACTB3_RESP;
typedef P_CONACTB3_RESP far * P_CONACTB3_RESPP;

typedef struct {            /* parms. for CONNECT_B3/IND */
    WORD    ncci;
    WORD    plci;
    NCPI    ncpi;
} P_CONB3_IND;
typedef P_CONB3_IND far * P_CONB3_INDP;

typedef struct {            /* parms. for CONNECT_B3/RESP */
    WORD    ncci;
    BYTE    reject;
    NCPI    ncpi;
} P_CONB3_RESP;
typedef P_CONB3_RESP    far * P_CONB3_RESPP;

typedef struct {            /* parms. for LISTEN_B3/REQ */
    WORD    plci;
} P_LISTENB3_REQ;
typedef P_LISTENB3_REQ  far * P_LISTENB3_REQP;

typedef struct {            /* parms. for cmd LISTEN_B3/CONF */
    WORD    plci;
    WORD    info;
} P_LISTENB3_CONF;
typedef P_LISTENB3_CONF far * P_LISTENB3_CONFP;

typedef struct {            /* parms. for cmd DISCONNECT_B3/REQ */
    WORD    ncci;
    NCPI    ncpi;
} P_DCONB3_REQ;
typedef P_DCONB3_REQ    far * P_DCONB3_REQP;

typedef struct {            /* parms. for cmd DISCONNECT_B3/CONF */
    WORD    ncci;
    WORD    info;
} P_DCONB3_CONF;
typedef P_DCONB3_CONF   far * P_DCONB3_CONFP;

typedef struct {            /* parms. for cmd DISCONNECT_B3/IND */
    WORD    ncci;
    WORD    info;
    NCPI    ncpi;
} P_DCONB3_IND;
typedef P_DCONB3_IND    far * P_DCONB3_INDP;

typedef struct {            /* parms. for cmd DISCONNECT_B3/RESP */
    WORD    ncci;
} P_DCONB3_RESP;
typedef P_DCONB3_RESP   far * P_DCONB3_RESPP;

typedef struct {            /* parms. for cmd DATA_B3/REQ */
    WORD    ncci;
    WORD    lng;
    BYTE    far *data;
    BYTE    number;
    WORD    flags;
} P_DATAB3_REQ;
typedef P_DATAB3_REQ    far * P_DATAB3_REQP;

typedef struct {            /* parms. for cmd DATA_B3/CONF */
    WORD    ncci;
    BYTE    number;
    WORD    info;
} P_DATAB3_CONF;
typedef P_DATAB3_CONF   far * P_DATAB3_CONFP;

typedef P_DATAB3_REQ    P_DATAB3_IND;   /* parms. for cmd DATA_B3/REQ */
typedef P_DATAB3_IND    far * P_DATAB3_INDP;

typedef struct {            /* parms. for cmd DATA_B3/RESP */
    WORD    ncci;
    BYTE    number;
} P_DATAB3_RESP;
typedef P_DATAB3_RESP   far * P_DATAB3_RESPP;


#define DATA_B3_Q_BIT   0x01    /* qualifier bit in DATA_B3 msgs    */
#define DATA_B3_M_BIT   0x02    /* more-data  "     "       */
#define DATA_B3_D_BIT   0x04    /* delivery confirmation  "     */


typedef struct {            /* parms. for cmd GET_PARAMS/REQ */
    WORD    plci;
} P_GPARMS_REQ;
typedef P_GPARMS_REQ    far * P_GPARMS_REQP;

#define CHARGING_INFO 0x0602

typedef struct {
        WORD plci;
        DWORD info_mask;
} P_INFO_REQ;

typedef struct {
        WORD plci;
        WORD info;
} P_INFO_CONF;

typedef struct {
        WORD plci;
        WORD info_nbr;
        struct
        {
         BYTE len;
         char data[100];
        } ie;
} P_INFO_IND;

typedef struct {
        WORD plci;
} P_INFO_RESP;


typedef struct {            /* parms. for cmd GET_PARAMS/CONF */
    WORD        plci;
    BYTE        controller;
    BYTE        b_channel;
    WORD        info;
    BYTE        b3_link_cnt;
    BYTE        srv;
    BYTE        srv_aio;
    BYTE        srv_eaz;
    CON_ADDR    con_addr;
} P_GPARMS_CONF;
typedef P_GPARMS_CONF   far * P_GPARMS_CONFP;

typedef struct {            /* parms. for cmd SELECT_B2_PROT/REQ */
    WORD    plci;
    BYTE    protocol;
    DLPD    dlpd;
} P_B2PROT_REQ;
typedef P_B2PROT_REQ    far * P_B2PROT_REQP;

typedef struct {            /* parms. for cmd SELECT_B2_PROT/CONF */
    WORD    plci;
    WORD    info;
} P_B2PROT_CONF;
typedef P_B2PROT_CONF   far * P_B2PROT_CONFP;

typedef struct {            /* parms. for cmd SELECT_B3_PROT/REQ */
    WORD    plci;
    BYTE    protocol;
    NCPD    ncpd;
} P_B3PROT_REQ;
typedef P_B3PROT_REQ    far * P_B3PROT_REQP;

typedef struct {            /* parms. for cmd SELECT_B3_PROT/CONF */
    WORD    plci;
    WORD    info;
} P_B3PROT_CONF;
typedef P_B3PROT_CONF   far * P_B3PROT_CONFP;

typedef struct {            /* parms. for cmd HANDSET/IND */
    WORD    plci;
    BYTE    controller;
    BYTE    status;
} P_HANDSET_IND;
typedef P_HANDSET_IND   far * P_HANDSET_INDP;

typedef struct {            /* parms. for cmd HANDSET/RESP */
    WORD    plci;
} P_HANDSET_RESP;
typedef P_HANDSET_RESP  far * P_HANDSET_RESPP;

typedef struct {            /* parms. for cmd MANUFACTURER/IND */
    BYTE        code;
    MNF_I_PARM  mip;        /* parms. depending on code    */
} P_MNF_IND;
typedef P_MNF_IND   far * P_MNF_INDP;

typedef struct {            /* parms. for cmd MANUFACTURER/RESP */
    BYTE        code;
    MNF_R_PARM  mrp;        /* parms. depending on code    */
} P_MNF_RESP;
typedef P_MNF_RESP  far * P_MNF_RESPP;


typedef union {  /* parameter overlay       */
    P_RESET_B3_REQ  reset_b3_req;
    P_RESET_B3_REQ  reset_b3_resp;
    P_RESET_B3_REQ  reset_b3_ind;
    P_RESET_B3_CONF  reset_b3_conf;
    P_CON_REQ   con_req;
    P_CON_CONF  con_conf;
    P_CON_IND   con_ind;
    P_CON_RESP  con_resp;
    P_CONACT_IND    conact_ind;
    P_CONACT_RESP   conact_resp;
    P_DCON_REQ  dcon_req;
    P_DCON_CONF dcon_conf;
    P_DCON_IND  dcon_ind;
    P_DCON_RESP dcon_resp;
    P_LISTEN_REQ    listen_req;
    P_LISTEN_CONF   listen_conf;
    P_CONB3_REQ conb3_req;
    P_CONB3_CONF    conb3_conf;
    P_CONACTB3_IND  conactb3_ind;
    P_CONACTB3_RESP conactb3_resp;
    P_LISTENB3_REQ  listenb3_req;
    P_LISTENB3_CONF listenb3_conf;
    P_CONB3_IND conb3_ind;
    P_CONB3_RESP    conb3_resp;
    P_DATAB3_REQ    datab3_req;
    P_DATAB3_CONF   datab3_conf;
    P_DATAB3_IND    datab3_ind;
    P_DATAB3_RESP   datab3_resp;
    P_DCONB3_REQ    dconb3_req;
    P_DCONB3_CONF   dconb3_conf;
    P_DCONB3_IND    dconb3_ind;
    P_DCONB3_RESP   dconb3_resp;
    P_GPARMS_REQ    gparms_req;
    P_GPARMS_CONF   gparms_conf;
    P_B2PROT_REQ    b2prot_req;
    P_B2PROT_CONF   b2prot_conf;
    P_B3PROT_REQ    b3prot_req;
    P_B3PROT_CONF   b3prot_conf;
    P_HANDSET_IND   handset_ind;
    P_HANDSET_RESP  handset_resp;
    P_MNF_IND   mnf_ind;
    P_MNF_RESP  mnf_resp;
        P_INFO_REQ      info_req;
        P_INFO_CONF     info_conf;
        P_INFO_IND      info_ind;
        P_INFO_RESP     info_resp;
} PARMS;

        /*+++ CAPI message +++*/

typedef struct {
    WORD    len;            /* total msg length in bytes    */
    WORD    appl_id;        /* application id       */
    BYTE    cmd;            /* command          */
    BYTE    subcmd;         /* subcommand           */
    WORD    msg_nbr;        /* message number       */
    PARMS   p;          /* parameters depending on cmd  */
} CIAMSG;
typedef CIAMSG  far *CIAMSGP;

#define CIA_MSGLEN  sizeof( CIAMSG )
#define CIA_HDRLEN  (CIA_MSGLEN - sizeof( PARMS ))


        /*+++ SI MASKs +++*/

#define DUE_64KBIT 0x0080


         /*+++ EAZ MASKs +++*/

#define EAZ_ALL         0x01fe		// EAZ 1,2,3,4,5,6,7,8
#define EAZ_GLOBAL      0x0001
#define EAZ_1           0x0002
#define EAZ_2           0x0004
#define EAZ_3           0x0008
#define EAZ_4           0x0010
#define EAZ_5           0x0020
#define EAZ_6       	0x0040
#define EAZ_7       	0x0080
#define EAZ_8       	0x0100
#define EAZ_9           0x0200


/*+++ queue definitions, incl. receive data partion +++*/
#define     MAX_UPMSG_LEN   180 /* max. length of userpart msg        */
#define     MY_N_MSG        100	/* # of CIA msgs                      */
#define     MY_N_XBUF       4	// Wenn >5 kann CapiQ overflow zeigen.
#define     MY_N_B3_CON 	2   /* # of layer 3 connections       */
#define     MY_BLK_LEN      2046 /*    L„nge der Datenblöcke              */
#define     MY_DPART_LEN     (MY_N_B3_CON * MY_N_XBUF * (MY_BLK_LEN+2)) // +2 für steuerbits
#define     MY_Q_LEN      	((DWORD)(MAX_UPMSG_LEN*MY_N_MSG) + (DWORD)MY_DPART_LEN)

#define MAXINT 0x7FFF

#endif

