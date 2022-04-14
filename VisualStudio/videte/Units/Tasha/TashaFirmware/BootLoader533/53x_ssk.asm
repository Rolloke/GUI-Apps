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
.section test;
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


///////////////////////////////////////////////////////////////////////////////////////////////
// The following sets up MemDMA
///////////////////////////////////////////////////////////////////////////////////////////////
FDMA:
	[--SP] = RETS;	
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
	RETS = [SP++];
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
//	R5.L = 0x1001;						//Deposit 1 bit at bit position = 16
//	R5 = DEPOSIT(R0, R5);				//After this instruction R5.L = R0.L and bit0 of R5.H will be retreived
	R5 = R0;

	RETS = [SP++];
	RTS;


///////////////////////////////////////////////////////////////////////////////////////////////
// The following sets up SPI DMA
///////////////////////////////////////////////////////////////////////////////////////////////
SPI_DMA:
	[--SP] = RETS;
	CC = R2 == 0x0;
	IF CC JUMP DMA_COMPLETE;			//If the COUNT = 0, skip the DMA
	
	[P1+OFFSET_(DMA5_START_ADDR)] = R1;	//Set Destination Base Address
	W[P1+OFFSET_(DMA5_X_COUNT)] = R2;	//Set Destination Count
	W[P1+OFFSET_(DMA5_CONFIG)] = R4;	//Set Destination DMAConfig = DMA Enable, Memory Write, 1-D DMA, Flow - Stop, IOC 

	R0 = R4 >> 16;
	W[P1+OFFSET_(SPI_CTL)] = R0;		//enable SPI, dma rx mode, 8bit data
	CALL SET_FLAG;		//***
	IDLE;								//Wait for DMA to Complete
	R0 = 0x1;
	W[P1+OFFSET_(DMA5_IRQ_STATUS)] = R0;//Write 1 to clear DMA interrupt
	
DMA_COMPLETE:
	RETS = [SP++];	
	RTS;

/************************SPI SLAVE BOOT***********************************************************************************************/
SPI_SLAVE_BOOT:
	p4.h = UPPER_(FIO_DIR);			//Set LED at PF5 and toggle PF1
	p4.l = LOWER_(FIO_DIR);
	r7 = PF5|PF1(z);
	w[p4] = r7;
	
	p4.h = UPPER_(FIO_FLAG_C);
	p4.l = LOWER_(FIO_FLAG_C);
	r7 = PF5|PF1(z);
	w[p4] = r7;


	R7 = 0x1;							//Indicates to the SPI_DMA routine not to disable DMA after DMA is complete for SPI Slave Boot
	R4.H = 0x4402;						//SPI Control Register Value:
	CALL WAIT;							//SPI enable, Slave Mode, CPOL = 0, CPHA = 1, DMA receive mode (TIMOD = 10)

SSB_GRAB_HEADER:
	CALL DMA_HEADER;	
	P0 = R1;							//P0 = Destination Address

SSB_CHECK_IGNORE_BIT:
	CC = BITTST(R5,4);
	IF !CC JUMP SSB_CHECK_ZERO_FILL_BIT;
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
//	R0 = 0x01E0;
//	R0 = R0 & R5;
//	R0 = R0 >> 5;						//Bits 8:5 of the FLAG header is the PF number
	R3.L = 0x1;
//	R6.L = LSHIFT R3.L BY R0.L;
	R6 = PF1(z);
	R0 = W[P1+OFFSET_(FIO_DIR)](Z);
	R0 = R0 | R6;
	W[P1+OFFSET_(FIO_DIR)] = R0;		//R6 holds the Flag #
	W[P1+OFFSET_(FIO_FLAG_S)] = R6;		//Set the PF low initially
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
	r6 = PF5|PF1(z);
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


WAIT:
	/********************************************************************************************************************************/
	//wait here
		p4.h = 0x0000;
		p4.l = 0x0200;
		lsetup(wait, wait) lc0=p4; 	// Warteschleife
			wait: nop;
	      	
	/********************************************************************************************************************************/
RTS;

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




	
