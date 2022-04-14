/* File : SETCLK.H   Translated 10/91 */
/* using P2C Version 2.06 TP by LAUER & WALLWITZ D-6200 Wiesbaden */

#define VERS "1.3"      /* Versions-Nummer fÅr Installationsmeldung */
#define DEFAULT_COM 1   /* Voreinstellung fÅr COM-Port */
#define DEFAULT_ZEIT 0  /* Voreinstellung Zeitverschiebung */
#define WAIT_SEC 3      /* Wartezeit in Sekunden fÅr EmpfÑngertest */
#define ATOMZEIT_MASK 0x8000
#define MESZ_MASK 0x4000
#define INSTALL_MASK 0x1000     /* Konstanten fÅr Meldungen */
#define VERS_TEXT "SETCLK Version "
#define INSTALL_OK " fÅr \"Expert mouseCLOCK\" aktiv auf COM"
#define EMPFAENGER_NICHT_DA "\"Expert mouseCLOCK\" nicht gefunden auf COM"
#define ERROR_COM_PORT_UG "UngÅltiger COM-Port: "
#define ERROR_COM_PORT_NG "Schnittstelle nicht gefunden : COM"
#define ERROR_ZEIT "UnzulÑssiger Zeitverschiebungsfaktor: "
#define TAB "        "
#define VOREINST "Voreinstellung: "
#define READ1 "Aufruf des Treibers fÅr die \"Expert mouseClock\" :"
#define READ2 "EXPCLK [COM_Nr] [Zeitversch.] [Umstell. SZ-WZ]"
#define READ3 "   [COM_Nr]           : Nummer des COM-Ports  {1..4}"
#define READ4 "   [Zeitversch.]      : Zeitverschiebung in Stunden  {-13..+13}"
#define READ5 "   [Umstell. SZ-WZ]   : Automatische Sommerzeit-Umstellung {an/aus}"
#define AN "an"
#define ABBRUCH "Programm abgebrochen - Zeit wurde nicht gesetzt !"
typedef USHORT DUMMY5[5];
typedef struct {
  USHORT jahr,mon,tag,std,min,sec,sec100;
} DATUMZEIT;
extern DATUMZEIT dcfdata;
extern SHORT hilf,faktor;
extern BYTE nr,inbit;
extern BOOL atom_zeit,active,count,sommerzeit,zeit_ok,datum_ok,mesz;
typedef USHORT BIT59[4];
extern BIT59 time1,time2;
extern USHORT time_diff0,time_diff1,time_diff1a,time_diff2,time_diff3;
extern BYTE zaehler;
extern USHORT origseg,origofs;
typedef enum {NO,ODD,X1,EVEN,X2,STUCK1,X3,STUCK0,BREAK} PARITY; /* xn sind Dummies */
typedef enum {COM1,COM2,COM3,COM4} SERIAL;
typedef USHORT T_COMADRESSES[4];/* BIOS COMx adresses */

extern T_COMADRESSES (*comadresses);
#define A_MSK 0x0FF0    /* COM address mask, high and low nibble masked off */
#define COMTX 0x08      /* Transmitter Register */
#define COMRX 0x08      /* Receiver Register */
#define COMDLL 0x08     /* Baud Rate Divisor Low Byte */
#define COMDLH 0x09     /* Baud Rate Divisor High Byte */
#define COMIE 0x09      /* Interrupt Enable */
#define COMIID 0x0A     /* Interrupt Identification */
#define COMLC 0x0B      /* Line Control Register */
#define COMMC 0x0C      /* Modem Control Register */
#define COMLS 0x0D      /* Line Status Register */
#define COMMS 0x0E      /* Modem Status Register */
#define COMTREADY 0x20  /* Mask for Transmitter Ready */
#define COMRREADY 0x01  /* Mask for Receiver Ready */
#define NULL_1 '\0'
#define SOH '\x01'
#define STX '\x02'
#define ETX '\x03'
#define EOT '\x04'
#define ENQ '\x05'
#define ACK '\x06'
#define FF '\f'
#define NAK '\x15'
#define ETB '\x17'
#define CR '\r'
#define LF '\n'
