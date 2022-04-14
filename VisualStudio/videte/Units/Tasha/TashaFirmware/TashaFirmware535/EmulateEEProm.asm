/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: TashaFirmware535.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/TashaFirmware535.cpp $
// CHECKIN:		$Date: 19.01.04 11:18 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 19.01.04 11:18 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <defBF535.h>

// Adresse des Alteraregisters für den SPI Slave select.
#define ALT_REG_SLAVE_SELECT	0x20000006


#define OFFSET_( x ) ((x) & 0x0000FFFF)  
#define UPPER_( x ) (((x) >> 16) & 0x0000FFFF)  
#define LOWER_( x ) ((x) & 0x0000FFFF)  
#define SYSMMR_BASE 0xFFC00000
#define TIMEOUT  1000000								// Entspricht etwa 12takte/300Mhz = 40ms
#define TIMEOUT2 100000

.section program;

.global _Emulate24BitEEProm;
.global _FirmwareTransfer;

// R0: Adresse des Quellbuffers.
// R1: Anzahl der Datenbytes im Quellbuffer.
_Emulate24BitEEProm:
	[--SP] = (P5:0);
	[--SP] = (R7:0);

	P3.H 	= UPPER_(SYSMMR_BASE);					// P3 Points to the beginning of SYSTEM MMR Space
	P3.L 	= LOWER_(SYSMMR_BASE);					//    "
	P0 		= R0;									// SourceBufferAddr		
	P1 		= R0;									// SourceBufferBaseAddr
	P2 		= R1;									// Länge in Bytes		
	P2		= P2+P1;								// Endaddresse	
	R2 		= 0;		 							// bValidAddress = FALSE;
	R3 		= 0xff(Z);								// Konstante
	R4		= 0;
 	R5.H 	= UPPER_(TIMEOUT2);						// Timeout timer laden
 	R5.L 	= LOWER_(TIMEOUT2);						//  "
	
	begin_loop:	    																					
		R0 = R3;
		cc = BITTST(R2,0);
		IF !cc JUMP NO_VALID_ADDR;					// Schon eine Adresse erhalten?
			ssync; 
			R0 = B[P0++] (Z);						// dann sende das gewünschte Byte
		NO_VALID_ADDR:
		    W[P3+OFFSET_(SPI1_TDBR)] = R0;			// ansonsten 0xff senden

	     	R4.H = UPPER_(TIMEOUT);					// Timeout timer laden
	     	R4.L = LOWER_(TIMEOUT);					//  "
		RD_ST1:
			R4 += -1;								// Timeout timer decrementieren
			cc = R4;								// Timeout?
			IF !cc JUMP TIME_OUT;					// Ja, dann raus hier
			R0 = W[P3+OFFSET_(SPI1_ST)] (Z);		// Warte bis Date empfangen wurde.
	   		cc = bittst(R0,5);						//  "
     	IF !cc JUMP RD_ST1;							//  "
		ssync; 
    
		R0 = B[P3+OFFSET_(SPI1_RDBR)] (Z);			// Read Command?
     	cc = R0 == 0x03;							//  "
     	IF !cc JUMP NO_READ_COMAND;					//  "        
			ssync; 

        	//---- Hi Byte Address -----
	     	R4.H = UPPER_(TIMEOUT);					// Timeout timer laden
	     	R4.L = LOWER_(TIMEOUT);					//  "
			W[P3+OFFSET_(SPI1_TDBR)] = R3;			//  Dumme '0xff' senden
			RD_ST2:
				R4 += -1;							// Timeout timer decrementieren
				cc = R4;							// Timeout?
				IF !cc JUMP TIME_OUT;				// Ja, dann raus hier
				R0 = W[P3+OFFSET_(SPI1_ST)] (Z);	// Warte bis Date empfangen wurde.
		   		cc = bittst(R0,5);					//  "
	    	IF !cc JUMP RD_ST2;						//  "
			ssync; 
	    
	    	R1 = B[P3+OFFSET_(SPI1_RDBR)] (Z);		// Higher Byte Address lesen
       		R1 = R1 << 16;      
               		
        	//---- Mid Byte Address -----
	     	R4.H = UPPER_(TIMEOUT);					// Timeout timer laden
	     	R4.L = LOWER_(TIMEOUT);					//  "
			W[P3+OFFSET_(SPI1_TDBR)] = R3;			//  Dumme '0xff' senden
			RD_ST3:
				R4 += -1;							// Timeout timer decrementieren
				cc = R4;							// Timeout?
				IF !cc JUMP TIME_OUT;				// Ja, dann raus hier
				R0 = W[P3+OFFSET_(SPI1_ST)] (Z);	// Warte bis Date empfangen wurde.
		   		cc = bittst(R0,5);					//  "
	    	IF !cc JUMP RD_ST3;						//  "
			ssync; 
    
	    	R0 = B[P3+OFFSET_(SPI1_RDBR)] (Z);		// Higher Byte Address lesen
       		R0 = R0 << 8;							//  "
	     	R1 = R1 | R0;							//  "

	     	//---- Lo Byte Address -----
	     	R4.H = UPPER_(TIMEOUT);					// Timeout timer laden
	     	R4.L = LOWER_(TIMEOUT);					//  "
			W[P3+OFFSET_(SPI1_TDBR)] = R3;			//  Dumme '0xff' senden
	     	RD_ST4:
				R4 += -1;							// Timeout timer decrementieren
				cc = R4;							// Timeout?
				IF !cc JUMP TIME_OUT;				// Ja, dann raus hier
				R0 = W[P3+OFFSET_(SPI1_ST)] (Z);	// Warte bis Date empfangen wurde.
		   		cc = bittst(R0,5);					//  "
	     	IF !cc JUMP RD_ST4;	 					//  "
			ssync; 
	     	
			// ---- Complete 24 Bit Address fetched -----
	     	R0 = B[P3+OFFSET_(SPI1_RDBR)] (Z);		// Lower Byte Address lesen
			R1 = R1 | R0;							// 24 Bit Adresse komplett
			P0 = R1;								// Adresspointer auf die gewünschte Stelle setzen
			P0 = P0 + P1;							//    "
			R2 = 1;									// bValidAdress = TRUE
 		NO_READ_COMAND: 
 		
 		R5 += -1; 
 		CC = R5 == 0;
 		IF CC JUMP TIME_OUT;
		CC = P0 < P2;
	IF CC JUMP begin_loop;	 
	
	(R7:0)	= [SP++];								// Stack aufräumen
	(P5:0) 	= [SP++];								//  "
	R0 = 1;											// return True
	rts;
	
TIME_OUT:
	(R7:0)	= [SP++];								// Stack aufräumen
	(P5:0) 	= [SP++];								//  "
	R0 = 0;											// return false
	rts;

_Emulate24BitEEProm.end:

// R0: Adresse des Quellbuffers.
// R1: Anzahl der Datenbytes im Quellbuffer.
_FirmwareTransfer:
	[--SP] = (P5:0);
	[--SP] = (R7:0);

	P3.H 	= UPPER_(SYSMMR_BASE);					// P3 Points to the beginning of SYSTEM MMR Space
	P3.L 	= LOWER_(SYSMMR_BASE);					//    "
	P0 		= R0;									// SourceBufferAddr		
	P1 		= R0;									// SourceBufferBaseAddr
	P2 		= R1;									// Länge in Bytes		
	P2		= P2+P1;								// Endaddresse	
 
	begin_loop_2:	    																					
		R0 = B[P0++] (Z);							// dann sende das gewünschte Byte
	    W[P3+OFFSET_(SPI1_TDBR)] = R0;				//  "
		RD_ST1_2:
			R0 = W[P3+OFFSET_(SPI1_ST)] (Z);		// Warte bis Date empfangen wurde.
	   		cc = bittst(R0,5);						//  "
     	IF !cc JUMP RD_ST1_2;						//  "
		ssync; 
		R0 = B[P3+OFFSET_(SPI1_RDBR)] (Z);			// Dummy read
 		
		CC = P0 < P2;								
	IF CC JUMP begin_loop_2;	 
	
	(R7:0)	= [SP++];								// Stack aufräumen
	(P5:0) 	= [SP++];								//  "
	R0 = 1;											// return True
	rts;
_FirmwareTransfer.end:



