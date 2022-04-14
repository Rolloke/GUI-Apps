/*******************************************************************************/
/*Edinburgh Boot Rom														****/
/*Modified:																	****/
/*02/18/2003:  	Added code to update source pointer for ignore bit.			****/
/*03/03/2003:	Adjusted comments											****/
/*03/05/2003:	Put in delay for CS Disable Time for SPI Boot				****/
/*03/07/2003:	Corrected external pointer for Ignore block for flash boot	****/
/*05/20/2003:	Put in 8-/16-/24-bit SPI support for BMODE = 11				****/
/********Rev 0.3 enhancements below*********************************************/
/*09/11/2003:	Put in support for full 16-bit flash boot (no zero padding)	****/
/*10/30/2003	Put in support for SPI Slave Mode Boot						****/
/*11/10/2003	Put in support for ATMEL SPI DataFlash						****/
/*																	Hiren Desai*/
/*******************************************************************************/

#include <defBF532.h>

#define SPIFREQ 340 //196KHz
#define OFFSET_( x ) ((x) & 0x0000FFFF)
#define UPPER_( x ) (((x) >> 16) & 0x0000FFFF)
#define LOWER_( x ) ((x) & 0x0000FFFF)

#define SYSMMR_BASE 0xFFC00000
#define COREMMR_BASE 0xFFE00000

/************************TEST****************************************************************************************************/
#define DMA_Destination		0xFF807FF4
/************************TEST****************************************************************************************************/

/************************L1 Memory***********************************************************************************************/
#define L1_DataBA	0xFF800000		//L1 Data Bank A
#define L1_DataBB	0xFF900000		//L1 Data Bank B	
#define L1_Code		0xFFA00000		//L1 Code SRAM
#define L1_Scratch_END 0XFFB00FFC	//L1 Scratch SRAM (end)
#define ASYNC_Bank0 0x20000000		//Async Bank 0
/********************************************************************************************************************************/

#define SYSCFG_VALUE 0x30			//To allow operations up to 600MHz.

.SECTION program;

.global _bootkernel;
_bootkernel:
/************************Code to Stay in Supervisor Mode*************************************************************************/
	R1 = SYSCFG_VALUE;
	SYSCFG = R1;					//To allow operations up to 600MHz.
	


	SP.H = UPPER_(L1_Scratch_END);
	SP.L = LOWER_(L1_Scratch_END);	//Set up supervisor stack in the end of L1 Scratch Memory

	P0.H = UPPER_(COREMMR_BASE);	// P0 Points to the beginning of CORE MMR Space
	P0.L = LOWER_(COREMMR_BASE);

	P1.H = UPPER_(SYSMMR_BASE);		// P1 Points to the beginning of SYSTEM MMR Space
	P1.L = LOWER_(SYSMMR_BASE);

	P3.H = UPPER_(DMA_Destination);	// P3 Points to the DMA Header Destination.
	P3.L = LOWER_(DMA_Destination);

	P2.H = START;
	P2.L = START;
	[P0+OFFSET_(EVT15)] = P2;		//Place the address of start code in IVG15 of EVT

	R1.L = OFFSET_(EVT_IVG15);
	[P0+OFFSET_(IMASK)] = R1;		//Set(enable) IVG15
	RAISE 15;						//Invoke IVG15 interrupt
	P2.L = WAIT_HERE;
	RETI = P2;						//Set Reset Return Address 
	RTI;							//Return from Reset Interrupt
WAIT_HERE:	               			//Wait here till IVG15 interrupt is processed
	JUMP WAIT_HERE;
/********************************************************************************************************************************/

/************************Test BMODE Pins*****************************************************************************************/
START:
	[--SP] = RETI;					//RETI must be pushed to enable interrupts.  Boot code in supervisor mode starts here.
	
	R7 = W[P1+OFFSET_(SYSCR)] (Z);
	R1 = 0x000E;
	R0 = R7 & R1;
	
	R4.L = 0x0083;							//SPI DMAConfig Value or Destination DMAConfig Value
	R4.H = 0x5002;							//SPI Control Register Value: 
											//SPI enable, Master, CPHA = CPOL = 0, DMA receive mode (TIMOD = 10)
	R1 = 0x0;
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R1;	//MDMA Source Modify = 0 for Zero Fill
	R1 += 1;
	W[P1+OFFSET_(DMA5_X_MODIFY)] = R1;		//SPI DMA Modify = 1
	W[P1+OFFSET_(MDMA_D0_X_MODIFY)] = R1;	//MDMA Destination Modify = 1
	R3 = R1;								//Source DMAConfig Value (8-bit words)

	CC = BITTST(R7,4);
	R7 = 0x0;						//Set R7 = 0x0 for SPI Master Mode Boot
	IF CC JUMP SWRESET;				//If Software Reset and Bypass Normal Boot Sequence, Jump to L1 Code SRAM
	
HARD_RESET:							//Test BMODE Pins
	R0 = R0 >> 1;
	CC = R0 == 0x2;
	IF CC JUMP SPI_SLAVE_BOOT;		//BMODE = 010, SPI Slave Boot
	
	JUMP SPI_SLAVE_BOOT; //SPI_BOOT;					//BMODE = 011, Boot from 8-/16-/24-bit SPI Serial ROM or ATMEL SPI DataFlash
/********************************************************************************************************************************/
/************************Flash BOOT**********************************************************************************************/
FLASH_BOOT:
/********************************************************************************************************************************/
	CALL AN_8_OR_16_BIT_PART;

	R0 = P0;							//Set R0 to the start of Async Bank 0 (0x20000000)
	
///////////////////////////////////////////////////////////////////////////////////////////////
// At this point, we know what part we have connected to the Async Bank 0, 8- or 16-bit Memory
// Time to read/parse headers and boot stuff in
///////////////////////////////////////////////////////////////////////////////////////////////
FGRAB_HEADER:
	CALL FDMA_HEADER;
	P0 = R1;							//P0 = Destination Address

FCHECK_IGNORE_BIT:
	CC = BITTST(R5,4);
	IF !CC JUMP FCHECK_INIT_BIT;
	CC = BITTST(R4,16);
	IF CC JUMP UPDATE_ADDR;
	R2 = R2 << 1;						//Multiply count value by 2 so address increments by 2 for 8-bit part (EBIU is 16 bits wide)
UPDATE_ADDR:	
	R0 = R0 + R2;						//Update Source Base Address
	CC = BITTST(R5,3);					//Check Init Bit within FLAG
	IF !CC JUMP FCHECK_LAST_SECTION;	//Ignore this block and check if this is the last one
	CALL(P0);							//If Init Bit is set, Call Init Code
	JUMP FCHECK_LAST_SECTION;			//Check if this is the last block
     
FCHECK_INIT_BIT:
	CC = BITTST(R5,3);
	IF !CC JUMP FCHECK_ZERO_FILL_BIT;
	CALL FDMA;							//DMA Init Code into internal memory
	R0 = [P1+OFFSET_(MDMA_S0_CURR_ADDR)];	//Get Source Base Address
	CALL(P0);							//Call Init Code
	JUMP FGRAB_HEADER;					//Grab the next header (Init Block cannot be the last block within the loader stream)
											//This is for 3.1 and 3.5 support which set the last block bit for Init Blocks

FCHECK_ZERO_FILL_BIT:
	CC = BITTST(R5,0);
	IF !CC JUMP FDO_DMA;				//Zero Fill?
	R0.H = ZERO;	
	R0.L = ZERO;						//Source Base Address = ZERO Location
	R6 = 0x0;
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R6;//Source Modify = 0 for Zero Fill

FDO_DMA:
	CALL FDMA;
	
	R0 = 0x2;
	CC = BITTST(R4,16);
	IF CC R0 = R3;						//R0 = 0x1 (R3 should already contain a 0x1)
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R0;//Source Modify = 2 for 8-bit part OR Source Modify = 1 for 16-bit part
//	CALL AN_8_OR_16_BIT_PART;			//Restore Source Modify to 2 for 8-bit part or 1 for 16-bit part
	
	CC = BITTST(R5,0);
	IF CC R0 = R7;						//If Zero Fill, Restore R0 to Flash Memory
	R7 = [P1+OFFSET_(MDMA_S0_CURR_ADDR)];
	IF !CC R0 = R7; 					//If Not Zero Fill, R0 = Current Source Base Address

FCHECK_LAST_SECTION:	
	CC = BITTST(R5,15);					//Last Section?
	IF !CC JUMP FGRAB_HEADER;			// If not, Jump to grab header
	JUMP BOOT_END;



///////////////////////////////////////////////////////////////////////////////////////////////
// The following sets up MemDMA
///////////////////////////////////////////////////////////////////////////////////////////////
FDMA:
	CC = R2 == 0x0;
	IF CC JUMP FDMA_RTS;					//If the COUNT = 0, skip the DMA
		
	[P1+OFFSET_(MDMA_S0_START_ADDR)] = R0;	//Set Source Base Address
	W[P1+OFFSET_(MDMA_S0_X_COUNT)] = R2;	//Set Source Count
	W[P1+OFFSET_(MDMA_S0_CONFIG)] = R3;		//Set Source:  DMAConfig = DMA Enable, Memory Read,  8-Bit Transfers, 1-D DMA, Flow - Stop
	
	[P1+OFFSET_(MDMA_D0_START_ADDR)] = R1;	//Set Destination Base Address
	W[P1+OFFSET_(MDMA_D0_X_COUNT)] = R2;	//Set Destination Count
	W[P1+OFFSET_(MDMA_D0_CONFIG)] = R4;		//Set Destination DMAConfig = DMA Enable, Memory Write, 8-Bit Transfers, 1-D DMA, Flow - Stop, IOC 
	
	IDLE;									//Wait for DMA to Complete
	
	W[P1+OFFSET_(MDMA_D0_IRQ_STATUS)] = R3;	//Write 1 to clear DMA interrupt (R3 should already contain a 0x1)
	
FDMA_RTS:
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// The following DMAs Header:
// R1 = ADDRESS
// R2 = COUNT
// R5 = FLAG
///////////////////////////////////////////////////////////////////////////////////////////////
FDMA_HEADER:
	[--SP] = RETS;
	R1 = P3;
	R2 = 0xA;
	
	CALL FDMA;
	
	R1 = [P3];
	R2 = [P3+0x4];
	R5 = W[P3+0x8](Z);

	R0 = [P1+OFFSET_(MDMA_S0_CURR_ADDR)];//Get Source Base Address
	R7 = R0;							//Save Source Base Address for zero fill
	
	RETS = [SP++];
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// The following determines whether we have an 8- or 16-bit flash/prom connected.
// The ADDRESS of the DXE count block will be 0xFF800040 or 0xFFA00000 (rev 0.2) for an 8-bit flash or
// 0xFFA00060 for a 16-bit flash.  The 0x40 or 0x60 LSB will be used for UART baud rate detection 
// in the future.  The memories will look like:
//
//					8-bit flash		8-bit flash (for rev 0.2)	16-bit flash
//0x20000000		xx  40			xx  00						00  60 <--- we will be checking this location
//0x20000002		xx  00			xx  00						FF  80
//0x20000004		xx  80			xx  80						cc  cc
//0x20000006		xx  FF			xx  FF						cc  cc                    
//0x20000008		xx  cc			xx  cc						ff  ff
//0x2000000A		xx  cc			xx  cc						??  ??
//0x2000000C		xx  cc			xx  cc						??  ??
//0x2000000E		xx  cc			xx  cc						??  ??
//0x20000010		xx  ff			xx  ff						??  ??
//0x20000012		xx  ff			xx  ff						??  ??
//0x20000014		xx  ??			xx  ??						??  ??
///////////////////////////////////////////////////////////////////////////////////////////////
AN_8_OR_16_BIT_PART:
	P0.H = UPPER_(ASYNC_Bank0);
	P0.L = LOWER_(ASYNC_Bank0);
	R1 = B[P0](Z);						//Read the 1st byte from flash memory
	R2 = 0xF0;
	R1 = R1 & R2;						//Extract bits[7:4] of the first byte
	R2 = 0x60;
	CC = R1 == R2;						//Are bits[7:4] = 6.  If so, it's a 16-bit part

A_16_BIT_PART:
	R4.H = 0x1;							//R4.H = 0x1 for 16-bit part (used to increment pointer by 1 for ignore block)
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R3;//Source Modify = 1 for 16-bit part (R3 should already contain a 0x1)
	IF CC JUMP DONE;

AN_8_BIT_PART:
	R4.H = 0x0;							//R4.H = 0x0 for 8-bit part (used to increment pointer by 2 for ignore block)
	R0 = 0x2;
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R0;//Source Modify = 2 for 8-bit part

DONE:
	RTS;
/************************SPI MASTER BOOT***********************************************************************************************/
SPI_BOOT:								//SPI setup


	
/********************************************************************************************************************************/
//wait here
	p4.h = 0x4;
	p4.l = 0x0000;
	lsetup(wait_start, wait_end) lc0=p4; 	// Warteschleife
		wait_start:
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;	
		wait_end: nop;       	
/********************************************************************************************************************************/

	CALL SETUP_SPI_INTERFACE;
///////////////////////////////////////////////////////////////////////////////////////////////
// At this point, we know what part (8-, 16-, or 24-bit) we have connected to the SPI port
// Time to read/parse headers and boot stuff in
///////////////////////////////////////////////////////////////////////////////////////////////

GRAB_HEADER:
	CALL SPI_SETUP;						//Before setting up SPI DMA, we need to address the SPI memory
	CALL DMA_HEADER;	
	P0 = R1;							//P0 = Destination Address
	CALL DISABLE_SPI;					//Disable SPI Interface	
	R3 += 0xA;							//Increment external address by header

CHECK_IGNORE_BIT:
	CC = BITTST(R5,4);
	IF !CC JUMP CHECK_INIT_BIT;
	R3 = R3 + R2;						//Update Source Base Address
	CC = BITTST(R5,3);					//Check if the INIT bit is set within FLAG
	IF !CC JUMP CHECK_LAST_SECTION;		//Ignore this block and check if this is the last block
	CALL(P0);							//If Init Bit is set, Call Init Code
	JUMP CHECK_LAST_SECTION;			//Check if this is the last block
	
CHECK_INIT_BIT:
	CC = BITTST(R5,3);
	IF !CC JUMP CHECK_ZERO_FILL_BIT;
	CALL SPI_SETUP;						//Before setting up SPI DMA, we need to address the SPI memory	
	CALL SPI_DMA;						//DMA Init Code into internal memory
	CALL DISABLE_SPI;					//Disable SPI Interface	
	R3 = R3 + R2;						//Update Source Base address
	CALL(P0);							//Call Init Code
	JUMP GRAB_HEADER;					//Grab the next header (Init Block cannot be the last block within the loader stream)
											//This is for 3.1 and 3.5 support which set the last block bit for Init Blocks

CHECK_ZERO_FILL_BIT:
	CC = BITTST(R5,0);		
	IF !CC JUMP DO_DMA;					//Zero Fill?
	R0.H = ZERO;
	R0.L = ZERO;
	[--SP] = R3;						//Save R3 because FDMA routine uses R3
	R3 = 0x1;							//R3 = Source DMA Config Value
	CALL FDMA;
	R3 = [SP++];						//Restore R3
	JUMP CHECK_LAST_SECTION;			//Check if this is the last block

DO_DMA:
	CALL SPI_SETUP;						//Before setting up SPI DMA, we need to address the SPI memory	
	CALL SPI_DMA;
	CALL DISABLE_SPI;					//Disable SPI Interface	
	R3 = R3 + R2;						//increment external address by count value	

CHECK_LAST_SECTION:
	CC = BITTST(R5,15);					//Last Section?
	IF CC JUMP BOOT_END;
	JUMP GRAB_HEADER;					//If yes,JUMP to GRAB_HEADER

///////////////////////////////////////////////////////////////////////////////////////////////
// The following sets up the SPI Interface:  Baud rate and sets PF2 (SPISS) as an output
///////////////////////////////////////////////////////////////////////////////////////////////
SETUP_SPI_INTERFACE:
	[--SP] = RETS;						//Save return address onto stack
	
    R1 = SPIFREQ;//0x85;
	W[P1+OFFSET_(SPI_BAUD)] = R1;		//set baud rate register
	
	p4.h = UPPER_(FIO_DIR);			//Set LED at PF5 and toggle PF1
	p4.l = LOWER_(FIO_DIR);
	r7 = PF5|PF1|PF2(z);
	w[p4] = r7;
	
	p4.h = UPPER_(FIO_FLAG_C);
	p4.l = LOWER_(FIO_FLAG_C);
	r7 = PF5(z);
	w[p4] = r7;
	ssync;
	
	p4.h = UPPER_(FIO_FLAG_T);
	p4.l = LOWER_(FIO_FLAG_T);
	r7 = PF1(z);
	w[p4] = r7;
	ssync;
//	R0 = 0x4;
//	W[P1+OFFSET_(FIO_DIR)] = R0;		//set PF2 as an output, this is our CS line
	

///////////////////////////////////////////////////////////////////////////////////////////////
// 8-, 16-, or 24-bit SPI Memory?
// The following routine checks whether there is a 8-/16-/24-bit or an ATMEL SPI Memory connected
// and sets R6 = 0x3 for 8-bit SPI Memory
//			R6 = 0x2 for 16-bit SPI Memory
//			R6 = 0x1 for 24-bit SPI Memory
//			R6 = 0x0 for ATMEL SPI Memory
///////////////////////////////////////////////////////////////////////////////////////////////

PART_SELECT:
	R3 = 0x0;								//Initial address of external memory
	R1 = 0xFF;								//Test value for receive byte
	R6 = 0x3;								//set R6 = 0x3 for 8-bit part
	
	R0 = 0x5001;
	W[P1+OFFSET_(SPI_CTL)] = R0;			//enable SPI, nondma tx mode, 8bit
	
	R0 = 0x4;
	W[P1+OFFSET_(FIO_FLAG_C)] = R0;			//assert CS low
	
	R0 += -1;								//send out control word (R0 = 0x03)
	CALL SINGLE_BYTE_TRANSFER;
	
	R0 = R3;								//send out address byte
	CALL SINGLE_BYTE_TRANSFER;

	R0 = R3;								//send out dummy byte (or address byte if 16-/24-bit SPI Memory)
	CALL SINGLE_BYTE_TRANSFER;
    
EIGHT_BIT_CHECK:
	CC = R0 == R1;
	IF !CC JUMP PART_SELECT_DONE;			//if the byte we receive is NOT equal to 0xFF, it's an 8-bit SPI part

SIXTEEN_BIT_CHECK:
	R0 = R3;								//send out dummy byte (or address byte if 24-bit SPI Memory)
	CALL SINGLE_BYTE_TRANSFER;

	CC = R0 == R1;
	R6 += -1;								//set R6 = 0x2 for 16-bit part
	IF !CC JUMP PART_SELECT_DONE;			//if the byte we receive is NOT equal to 0xFF, it's a 16-bit SPI part

TWENTYFOUR_BIT_CHECK:
	R0 = R3;								//send out dummy byte
	CALL SINGLE_BYTE_TRANSFER;	

	CC = R0 == R1;
	R6 += -1;								//set R6 = 0x1 for 24-bit part
	IF !CC JUMP PART_SELECT_DONE;			//if the byte we receive is NOT equal to 0xFF, it's a 24-bit SPI part

ATMEL_PART:	R6 += -1;						//If all of the tests above fail, we are going to assume we have an ATMEL SPI Memory connected
	CALL READ_STATUS_REGISTER;

PART_SELECT_DONE:
	CALL DISABLE_SPI;
	
	RETS = [SP++];							//Restore return address from stack
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// The following DMAs the 10-byte header into location 0xFF807FE0 and returns the following:
// R1 = Destination Base Address (4 bytes)
// R2 = Count (4 bytes)
// R5 = Flag (2 bytes)
///////////////////////////////////////////////////////////////////////////////////////////////
DMA_HEADER:
	[--SP] = RETS;

	R1 = P3;							//DMA Destination = 0xFF807FF4
	R2 = 0xA;							//DMA Count = 10

	CALL SPI_DMA;

	R1 = [P3];							//Get Destination Base Address 
	R2 = [P3+0x4];						//Get Count
	R0 = W[P3+0x8](Z);					//Get Flag
	R5.L = 0x1001;						//Deposit 1 bit at bit position = 16
	R5 = DEPOSIT(R0, R5);				//After this instruction R5.L = R0.L and bit0 of R5.H will be retreived

	RETS = [SP++];
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// The following sets up SPI DMA
///////////////////////////////////////////////////////////////////////////////////////////////
SPI_DMA:
	CC = R2 == 0x0;
	IF CC JUMP DMA_COMPLETE;			//If the COUNT = 0, skip the DMA
	
	[P1+OFFSET_(DMA5_START_ADDR)] = R1;	//Set Destination Base Address
	W[P1+OFFSET_(DMA5_X_COUNT)] = R2;	//Set Destination Count
	W[P1+OFFSET_(DMA5_CONFIG)] = R4;	//Set Destination DMAConfig = DMA Enable, Memory Write, 1-D DMA, Flow - Stop, IOC 

	R0 = R4 >> 16;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//enable SPI, dma rx mode, 8bit data

	IDLE;								//Wait for DMA to Complete

	R0 = 0x1;
	W[P1+OFFSET_(DMA5_IRQ_STATUS)] = R0;//Write 1 to clear DMA interrupt
	
	CC = BITTST(R7,0);					//R7 = 0x0 for SPI Master Mode Boot; R7 = 0x1 for SPI Slave Mode Boot
	IF CC JUMP DMA_COMPLETE;
	
	W[P1+OFFSET_(DMA5_CONFIG)] = R7;	//Disable DMA only for SPI Master Mode Boot (R7 should already contain a 0x0 if we have come to this point)

DMA_COMPLETE:
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// Routine that addresses the SPI Memory
///////////////////////////////////////////////////////////////////////////////////////////////
SPI_SETUP:
	[--SP] = RETS;						//Save return address onto the stack
	
	I0 = R3;							//save logical address in I0
	
	R0 = 0x5001;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//enable SPI, nondma tx mode, 8bit
	
	R0 = 0x4;
	W[P1+OFFSET_(FIO_FLAG_C)] = R0;		//assert CS (PF2) low
	
	CC = R6 < 1;
	IF !CC JUMP ADDRESS_8_16_24;
	
ATMEL:
	CALL ADDRESS_DECODE;				//convert logical address into a page number and byte number
	R0 = 0xE8;							//control word for ATMEL SPI Memory
	CALL SINGLE_BYTE_TRANSFER;			//send out control word

	JUMP ADDRESS_24;	
	
ADDRESS_8_16_24:
	R0 = 0x3;							//control word for 8-/16-/24-bit SPI Memory
	CALL SINGLE_BYTE_TRANSFER;			//send out control word
	
	CC = R6 < 2;
	IF !CC JUMP ADDRESS_16;
	
ADDRESS_24:
	R0 = R3 >> 16;						//send out address byte
	CALL SINGLE_BYTE_TRANSFER;	
	
ADDRESS_16:
	CC = R6 < 3;
	IF !CC JUMP ADDRESS_8;

	R0 = R3 >> 8;						//send out address byte
	CALL SINGLE_BYTE_TRANSFER;		

ADDRESS_8:
	R0 = R3;							//send out address byte
	CALL SINGLE_BYTE_TRANSFER;

	CC = R6 < 1;
	IF !CC JUMP DONE_ADDRESSING;
		
	P2 = 0x4;							//send out 4 don't care bytes for ATMEL SPI Memory
	LSETUP(ATMEL_Begin, ATMEL_End) LC0=P2;
	ATMEL_Begin:	R0 = R3;			//send out byte
					CALL SINGLE_BYTE_TRANSFER;
	ATMEL_End:		NOP;
	
	R3 = I0;							//restore logical address from I0	

DONE_ADDRESSING:
	R0 = 0x5000;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//set TIMOD = 00 for SPI port
		
	RETS = [SP++];						//Restore return address from the stack
	RTS;



///////////////////////////////////////////////////////////////////////////////////////////////
// SPI Byte Transfer Routine
// The following sends out a byte stored in R0, check's the Status Register for single-word 
// transfer complete, and finally reads a byte from the SPI memory and stores it in R0.
///////////////////////////////////////////////////////////////////////////////////////////////
SINGLE_BYTE_TRANSFER:
	W[P1+OFFSET_(SPI_TDBR)] = R0;		//send out byte
	
RD_ST:		
	R0 = W[P1+OFFSET_(SPI_STAT)] (Z);
	CC = BITTST(R0,5);					//test bit 5 (RXS) of SPIST0 register to see if the RX Data Buffer is full, if 0 do test again
	IF !CC JUMP RD_ST;
	
	R0 = W[P1+OFFSET_(SPI_RDBR)] (Z);	//do read
	
	RTS;

	

///////////////////////////////////////////////////////////////////////////////////////////////
// SPI Disable
// The following routine disables the SPI port and holds it disabled for 500ns
///////////////////////////////////////////////////////////////////////////////////////////////
DISABLE_SPI:
	R0 = 0x0000;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//disable SPI
	R0 = W[P1+OFFSET_(SPI_RDBR)] (Z);	//do dummy read to clear RXS

	R0 += 0x4;
	W[P1+OFFSET_(FIO_FLAG_S)] = R0;		//de-assert CS
	
	P2 = 0x01A4;
	LSETUP(DELAY_LOOP, DELAY_LOOP) LC0=P2;
	DELAY_LOOP: NOP;					//We need a 500ns delay for /CS Disable Time (Calculated for 600MHz SCLK)

	RTS;
	
///////////////////////////////////////////////////////////////////////////////////////////////
// The following routine converts a logical address to a page number and a byte number for the
// ATMEL SPI DataFlash.
// The AT45DB041 has 11 page bits and 9 byte bits (264 bytes/page), R5.H = 0x1
// The AT45DB081 has 12 page bits and 9 byte bits (264 bytes/page), R5.H = 0x1
// The AT45DB161 has 12 page bits and 10 byte bits (528 bytes/page),R5.H = 0x0
///////////////////////////////////////////////////////////////////////////////////////////////
ADDRESS_DECODE:
	R7 = 0x108;							//264 bytes per page for AT45DB041 and AT45DB081
	
	CC = BITTST(R5, 16);
	IF CC JUMP ATMEL_041_081;
	
ATMEL_161:
	R7 = 0x210;							//528 bytes per page for AT45DB161
	
ATMEL_041_081:
	R0 = 0x0;							//R0 holds the page number
	
COMPARE:
	CC = R3 <= R7;						//Is the logical address less than bytes/page?
	IF CC JUMP LESS_THAN_PAGE;
	
ADD_PAGE_SUBTRACT:
	R0 += 1;							//if not, add 1 to page number
	R3 = R3 - R7;						//and subtract bytes/page from logical address
	JUMP COMPARE;						//compare again

LESS_THAN_PAGE:	
	CC = R3 == R7;						//if so, check if we have exactly 1 page
	IF !CC JUMP FINISHED_DECODING;
	R3 = 0x0;							//if exact page, byte number = 0
	R0 += 1;							//and add 1 to page number

FINISHED_DECODING:
	R0 = R0 << 10;						//left shift page number by 10 for AT45DB161
	CC = BITTST(R5,16);
	IF !CC JUMP ADDRESS_161;
	R0 = R0 >> 1;						//left shift page number by 9 for AT45DB041 and AT45DB081
	
ADDRESS_161:
	R3 = R0|R3;							//OR page number and byte number
	
	R7 = 0x0;							//restore R7 to 0x0 for SPI Slave use
	RTS;
	

///////////////////////////////////////////////////////////////////////////////////////////////
// The following routine reads the status register for ATMEL SPI DataFlash.  This is to
// determine which memory we have connected:
// The AT45DB041 has bits 5:2 set to 0111 (set R5.H = 0x1)
// The AT45DB081 has bits 5:2 set to 1001 (set R5.H = 0x1)
// The AT45DB161 has bits 5:2 set to 1011 (set R5.H = 0x0)
///////////////////////////////////////////////////////////////////////////////////////////////
READ_STATUS_REGISTER:
	[--SP] = RETS;						//Save return address onto stack

	CALL DISABLE_SPI;					//disable SPI for new transfer
	
	R0 = 0x5001;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//enable SPI, nondma tx mode, 8bit
	
	R0 = 0x4;
	W[P1+OFFSET_(FIO_FLAG_C)] = R0;		//assert CS low
	
	R0 = 0xD7;							//send out control word to read status register
	CALL SINGLE_BYTE_TRANSFER;
	CALL SINGLE_BYTE_TRANSFER;
	
	R5.H = 0x0;
	R1 = 0x3C;
	R0 = R0 & R1;
	R1 = 0x2C;
	CC = R0 == R1;
	IF CC JUMP STATUS_DONE;
	R5.H = 0x1;
	
STATUS_DONE:
	RETS = [SP++];
	RTS;
	

	
	

/************************SPI SLAVE BOOT***********************************************************************************************/
SPI_SLAVE_BOOT:
	p4.h = UPPER_(FIO_DIR);			//Set LED at PF5 and toggle PF1
	p4.l = LOWER_(FIO_DIR);
	r7 = PF5(z);
	w[p4] = r7;
	
	p4.h = UPPER_(FIO_FLAG_C);
	p4.l = LOWER_(FIO_FLAG_C);
	r7 = PF5(z);
	w[p4] = r7;
	ssync;

	R7 = 0x1;							//Indicates to the SPI_DMA routine not to disable DMA after DMA is complete for SPI Slave Boot
	R4.H = 0x4402;						//SPI Control Register Value:
										//SPI enable, Slave Mode, CPOL = 0, CPHA = 1, DMA receive mode (TIMOD = 10)
SSB_GRAB_HEADER:
	CALL DMA_HEADER;
	P0 = R1;							//P0 = Destination Address

SSB_CHECK_IGNORE_BIT:
	CC = BITTST(R5,4);
	IF !CC JUMP SSB_CHECK_ZERO_FILL_BIT;
	CALL SET_FLAG;						//Set appropriate flag for host feedback
	R0 = 0x0;
	W[P1+OFFSET_(DMA5_X_MODIFY)] = R0;	//Set SPI DMA Modify to 0
	R1.H = 0xFF80;
	R1.L = 0x7FFE;						//Set the SPI DMA Start Address to 0xFF807FFE (2nd to last location of L1 Data Bank A)
	JUMP SSB_DO_DMA;					//Boot in Ignored Block and check if last section (only first location will be written to)

SSB_CHECK_ZERO_FILL_BIT:
	CC = BITTST(R5,0);		
	IF !CC JUMP SSB_DO_DMA;				//Zero Fill?
	CALL TOGGLE_FLAG;					//Set PF high to signal host not to send anymore bytes
	R0.H = ZERO;
	R0.L = ZERO;
	CALL FDMA;							//R3 should already contain a 0x1 for the FDMA routine
	CALL TOGGLE_FLAG;					//Set PF low to signal to host to continue sending bytes
	JUMP SSB_CHECK_LAST_SECTION;

SSB_DO_DMA:
	CALL SPI_DMA;
	W[P1+OFFSET_(DMA5_X_MODIFY)] = R0;	//Reset SPI DMA Modify back to 1 (R0 should already contain a 0x1)
	
SSB_CHECK_INIT_BIT:
	CC = BITTST(R5,3);
	IF !CC JUMP SSB_CHECK_LAST_SECTION;
	CALL TOGGLE_FLAG;					//Set PF high to signal host not to send anymore bytes
	CALL(P0);							//Call Init Code
	CALL TOGGLE_FLAG;					//Set PF low to signal to host to continue sending bytes

SSB_CHECK_LAST_SECTION:
	CC = BITTST(R5,15);					//Last Section?
	IF CC JUMP BOOT_END;
	JUMP SSB_GRAB_HEADER;				//JUMP to GRAB_HEADER if not done

SET_FLAG:
	R0 = 0x01E0;
	R0 = R0 & R5;
	R0 = R0 >> 5;						//Bits 8:5 of the FLAG header is the PF number
//	R3.L = 0x1;
//	R6.L = LSHIFT R3.L BY R0.L;
	R6.L = PF1;
	R0 = W[P1+OFFSET_(FIO_DIR)](Z);
	R0 = R0 | R6;
	W[P1+OFFSET_(FIO_DIR)] = R0;		//R6 holds the Flag #
	W[P1+OFFSET_(FIO_FLAG_C)] = R6;		//Set the PF low initially
	RTS;
	
TOGGLE_FLAG:
	W[P1+OFFSET_(FIO_FLAG_T)] = R6;
	RTS;
	


	
///////////////////////////////////////////////////////////////////////////////////////////////
// We are done booting.  Bit 1 of the FLAG has information on whether we are an ADSP-BF533
// (Bit 1 = 1), which has a starting address of 0xFFA00000 or an ADSP-BF531/32 (Bit 1 = 0),
// which has a starting location of 0xFFA08000.
///////////////////////////////////////////////////////////////////////////////////////////////
BOOT_END:
	
	p4.h = UPPER_(FIO_DIR);				//LED aus manchen
	p4.l = LOWER_(FIO_DIR);
	r6 = PF5(z);
	w[p4] = r6;
	
	p4.h = UPPER_(FIO_FLAG_S);
	p4.l = LOWER_(FIO_FLAG_S);
	r6 = PF5(z);
	w[p4] = r6;
	ssync;
	
	CC = BITTST(R5,1);
	P2.H = UPPER_(L1_Code);
	P2.L = LOWER_(L1_Code);
	IF CC JUMP ADSP_BF533_SPI;			//Check if BF533
	P2.L = 0x8000;						//If BF531/BF532, set LSB address bits to 0x8000
ADSP_BF533_SPI:
	P0.H = UPPER_(EVT1);
	P0.L = LOWER_(EVT1);
	[P0] = P2;							//Store the start of L1 Instruction Memory Address (0xFFA00000 or 0xFFA08000) into EVT1 for Software Reset case
	JUMP(P2);							//JUMP to 0xFFA00000 for BF533 or JUMP to 0xFFA08000 for BF531/BF532




///////////////////////////////////////////////////////////////////////////////////////////////
// The following takes care of a software reset case where the user wants to bypass the normal
// boot sequence and jump to the address stored in EVT1 (RESET Vector)
///////////////////////////////////////////////////////////////////////////////////////////////
SWRESET:
	P0.H = UPPER_(EVT1);				//P0 points to the RESET vector within the Event Vector Table
	P0.L = LOWER_(EVT1);
	P1 = [P0];
	JUMP(P1);							//Jump to the RESET vector



.BYTE ZERO[1] = 0x0;

_bootkernel.end:  NOP;
/////////////////////////////////////////////////////////////