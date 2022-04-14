/********************************************************************/
/*  This file contains 3 sections:									*/
/*																	*/
/*	1) A Pre-Init Section - this section saves off all the			*/
/*		registers of the DSP.										*/
/*																	*/
/*	2) A Init Code Section - this section is the customer			*/
/*		initialization code which can be modified by the customer.	*/
/*		As an example, an SDRAM initialization code is supplied.	*/
/*																	*/
/*	3) A Post-Init Section - this section restores all the			*/
/*		register from the stack.									*/
/*																	*/
/*	Customers should not modify the Pre-Init and Post-Init Sections.*/
/*	The Init Code Section can be modified for application use.		*/
/********************************************************************/

#include <defBF532.h>

.global _start;
.section program;
_start:
/*******Pre-Init Section*********************************************/
/*******DO NOT MODIFY************************************************/
	[--SP] = ASTAT;			//Save Regs onto stack
	[--SP] = RETS;
	[--SP] = (r7:0);
	[--SP] = (p5:0);
	[--SP] = I0;
	[--SP] = I1;
	[--SP] = I2;
	[--SP] = I3;
	[--SP] = B0;
	[--SP] = B1;
	[--SP] = B2;
	[--SP] = B3;
	[--SP] = M0;
	[--SP] = M1;
	[--SP] = M2;
	[--SP] = M3;
	[--SP] = L0;
	[--SP] = L1;
	[--SP] = L2;
	[--SP] = L3;
/********************************************************************/

/*******Init Code Section********************************************/

/**Definierter Start************/
	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers
	
	p1.h = hi(VR_CTL);	// Laden der oberen 16 Bit des Volrage Registers
	p1.l = lo(VR_CTL);	// Laden der unteren 16 Bit des Voltage Registers
	
	p2.h = hi(PLL_DIV);	// Laden der oberen 16 Bit des PPL_DIV Registers
	p2.l = lo(PLL_DIV);	// Laden der unteren 16 Bit des PPL_DIV Registers

	R0.L=0x0004;		// DIV auf /4 stellen
	W[P2]=R0;			// schreiben in PLL_DIV Register
	R1.L=0x00DB;		// auf ca. 1.25V
	W[P1]=R1;			// schreiben in VR_CTL Register
	R3.L=0x2C00; 		// VCO Faktor auf 22x 27MHz (=594MHz)
//	R3.L=0x2A00; 		// VCO Faktor auf 21x 27MHz (=567MHz)
	
	W[P0]=R3;			// durch schreiben in PLL_CTL Register

/*******SDRAM Setup************/
	Setup_SDRAM:
	P0.L = EBIU_SDRRC & 0xFFFF;
	P0.H = (EBIU_SDRRC >> 16) & 0xFFFF;		//SDRAM Refresh Rate Control Register
	R0 = 0x08B3(Z);					
	W[P0] = R0;							
	SSYNC;	

	P0.L = EBIU_SDBCTL & 0xFFFF;			
	P0.H = (EBIU_SDBCTL >> 16) & 0xFFFF;	//SDRAM Memory Bank Control Register
	R0 = 0x0011(Z);
	[P0] = R0;
	SSYNC;		

	P0.L = EBIU_SDGCTL & 0xFFFF;			
	P0.H = (EBIU_SDGCTL >> 16) & 0xFFFF;	//SDRAM Memory Global Control Register
//	R0.H = 0x0491;
//	R0.L = 0x1909;
	R0.H = 0x0091;
	R0.L = 0x998D;
	[P0] = R0;
	SSYNC;	

/******************************/

	
/********************************************************************/

/*******Post-Init Section********************************************/
/*******DO NOT MODIFY************************************************/
	L3 = [SP++];
	L2 = [SP++];	
	L1 = [SP++];
	L0 = [SP++];
	M3 = [SP++];
	M2 = [SP++];
	M1 = [SP++];
	M0 = [SP++];
	B3 = [SP++];
	B2 = [SP++];
	B1 = [SP++];
	B0 = [SP++];
	I3 = [SP++];
	I2 = [SP++];
	I1 = [SP++];
	I0 = [SP++];
	(p5:0) = [SP++];  		//Restore Regs from Stack
	(r7:0) = [SP++];
	RETS = [SP++];
	ASTAT = [SP++];
/********************************************************************/

	RTS;



