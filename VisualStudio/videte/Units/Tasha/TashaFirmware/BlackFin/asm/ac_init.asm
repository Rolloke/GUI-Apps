/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Audio codec test program. Audio Codec, SPORT0 and DMA initializations.
 *
 * $Log$
 *
 ******************************************************************************/

#include    <def21535.h>
#include    "zet.h"
#include    "m21535.h"
#include    "ac_init.h"
#include    "gpio.h"

//..............................................................................

// AC-97 registers

#define	    REGS_RESET		    0x0000
#define	    MASTER_VOLUME	    0x0200
#define	    HEAD_VOLUME		    0x0400
#define	    MASTER_VOLUME_MONO	    0x0600  
#define	    RESERVED_REG_2	    0x0800
#define	    PC_BEEP_Volume	    0x0A00
#define	    PHONE_Volume	    0x0C00
#define	    MIC_Volume		    0x0E00
#define	    LINE_IN_Volume	    0x1000
#define	    CD_Volume		    0x1200
#define	    VIDEO_Volume	    0x1400
#define	    AUX_Volume		    0x1600
#define	    PCM_OUT_Volume	    0x1800
#define	    RECORD_SELECT	    0x1A00
#define	    RECORD_GAIN		    0x1C00
#define	    RESERVED_REG_3	    0x1E00
#define	    GENERAL_PURPOSE	    0x2000
#define	    THREE_D_CONTROL_REG	    0x2200
#define	    RESERVED_REG_4	    0x2400
#define	    POWERDOWN_CTRL_STAT	    0x2600
#define	    EXTEND_AUDIO_ID	    0x2800
#define	    EXTEND_AUDIO_CTL	    0x2A00
#define	    SAMPLE_RATE_GENERATE_1  0x2C00
#define	    SAMPLE_RATE_GENERATE_0  0x3200
#define	    JACK_SENSE		    0x7200
#define	    SERIAL_CONFIGURATION    0x7400
#define	    MISC_CONTROL_BITS	    0x7600
#define	    VENDOR_ID_1		    0x7C00
#define	    VENDOR_ID_2		    0x7E00

//..............................................................................

// Define time constants

// (not yet bound to core clock frequency but you rather have to change here for 
// now)

#define TC_1MS_300MHZ	300000
#define TC_2US_300MHZ	600
#define TC_3S_300MHZ	900000000


//..............................................................................

#define	CODEC_REG_INIT_LENGTH	50

//..............................................................................

#ifdef _EZKITLITE_			// [ _EZKITLITE_

#define bACRes		9

#define kACResSet	ZET(bACRes,1)     	
#define kACResReset	ZET(bACRes,0)     	

#else					// ] [ !_EZKITLITE
#if defined(_EAGLE_35_) || defined(_HAWK_35_) // [ _EAGLE_35_ || _HAWK_35_

#define bACRes		15

#define kACResSet	ZET(bACRes,1)     	
#define kACResReset	ZET(bACRes,0)     	

#else					// ] [ !(_EAGLE_35_ || _HAWK_35_)
#error "Board not defined"
#endif					// ] _EAGLE_35_
#endif					// ] _EZKITLITE_

//------------------------------------------------------------------------------

	.section    data1;

	.global	codecReset;	      
	.global	initSPORT0;
	.global	initSPORT0DMA;
	.global	initCodec;
	.global rx_buf;
	.global tx_buf;
	.global rxStatus;
 
//..............................................................................

	// receive buffer 

	.align 2;
	.byte2	rx_buf[16];	    

	// transmit buffer

	// Note that initialized tx_buf is set up to switch to SLOT16 mode 
	// by setting bit in SERIAL_CONFIGURATION register. Note as well that
	// slots are 1*16+12*20=256 bits before that has happened.
	// After being configured to SLOT16 mode bits are 16*16=256 bits.

	// Last 4 bits of address word in non-16 bit mode 
	// (SERIAL_CONFIGURATION+9) should be zero according to spec. but are
	// 0x9 ??

	// Data word initially (20-bits) contain 0x90000 which sets bits
	// SLOT16 and master codec reg. mask bit.

	.align 2;
	.byte2	tx_buf[16] =	
	kVldFrm|kVldCmd|kVldAddr, SERIAL_CONFIGURATION, 0x9900,0x0000, 
			0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x0000, 0x0000, 0x0000, 
			0x0000, 0x0000, 0x0000, 0x0000;

//..............................................................................

	.align 2;
	.byte2 initCodecRegs[CODEC_REG_INIT_LENGTH] =
	REGS_RESET,		0xFFFF, // Writing any value to this reg. 
					// performs a register reset
	MASTER_VOLUME,		0x0000,	// Mute off master volume, 0 dB gain on 
					// L/R channels
	HEAD_VOLUME,	    	0x0000,	// Mute off headphone volume, 6 dB gain
	MASTER_VOLUME_MONO,	0x8000, // Mute on mono master volume
	PC_BEEP_Volume,	    	0x8000, // Mute on PC beep volume
	PHONE_Volume,	    	0x8008, // Mute on phone volume, 0 dB gain
	MIC_Volume,		0x8008, // Mute on MIC volume, 0 dB gain
	LINE_IN_Volume,	    	0x8808, // Mute on LINE in vol.,0 dB gain on L/R
	CD_Volume,		0x8808, // Mute on CD volume, 0 dB gain on L/R
	VIDEO_Volume,	    	0x8808, // Mute on video volume,0 dB gain on L/R
	AUX_Volume,		0x8808, // Mute on aux volume,0 dB gain on L/R
	PCM_OUT_Volume,	    	0x0808,	// Mute off, 0 dB gain 
#ifndef MIC_IN
	RECORD_SELECT,	    	0x0404,	// Selected LINE in L/R as Record Select 
#else
	RECORD_SELECT,	    	0x0000,	// Selected MIC1 as Record Select 
#endif
	RECORD_GAIN,	    	0x0000,	// Mute off, 0 dB record gain 
	GENERAL_PURPOSE,	0x0000,	// Codec loopback off,MIC1 selected,
					// Mono output selected: Mix
					// PHAT stereo off,pre-3D
	THREE_D_CONTROL_REG,    0x0000, // 3D depth: 0%
	POWERDOWN_CTRL_STAT,    0x0000, // Status reg. to see powered-down
					// subsections
	EXTEND_AUDIO_ID,	0x0001, // Read-only reg. support for variable
					// rate audio
	EXTEND_AUDIO_CTL,	0x0001, // Enable support for variable rate audio
	SAMPLE_RATE_GENERATE_1, 0x1F40,	// DAC Sample Rate 8.000 KHz
	SAMPLE_RATE_GENERATE_0, 0x1F40, // ADC Sample Rate 8.000 KHz
	JACK_SENSE,		0x0000, // No sense interrupts enabled
	MISC_CONTROL_BITS,	0x0404, // DAC:SR1 selected,ADC:SR0 selected
	VENDOR_ID_1,	    	0x4144, // Read-only string
	VENDOR_ID_2,	    	0x5360; // Read-only string

	.align 2;
	.byte2 codecInitResults[CODEC_REG_INIT_LENGTH];	

	.align 2;
	.byte2 rxStatus = 0x0000;	// RX status word 

	.align 2;
	.byte2 imaskCopy;

//------------------------------------------------------------------------------
// AD1885 Hardware Reset
// Toggle reset line to assert and deassert AD1885 reset			     
//------------------------------------------------------------------------------

	.section	program;

codecReset:

	P0.L = FIO_DIR & 0xFFFF;
	P0.H = (FIO_DIR >> 16) & 0xFFFF;
	R0 = (kLEDSet+kACResSet)(z);	// set output pins
    	W[P0] = R0.L; ssync;

	P1.L = FIO_FLAG_S & 0xFFFF;		
	P1.H = (FIO_FLAG_S >> 16) & 0xFFFF;
	R0.L = kLEDSet+kACResSet;	// turn LEDs on,AC not yet in reset
					// (inverse logic)
	W[P1] = R0.L; ssync;

	p5.l = TC_1MS_300MHZ & 0xffff;	// time constant to see lights blink 
	p5.h = (TC_1MS_300MHZ >> 16) & 0xffff;

	LOOP blink_LEDs LC0 = P5;	// duration ca 1 ms for a 300 MHz DSP 

LOOP_BEGIN blink_LEDs;
	nop;
LOOP_END blink_LEDs;

	P1.L = FIO_FLAG_C & 0xFFFF;	    
	P1.H = (FIO_FLAG_C >> 16) & 0xFFFF;
    	R0.L = kLEDSet+kACResSet;	// assert AD1885 /RESET (active low) 
	W[P1] = R0.L; ssync;

	p5.l = TC_2US_300MHZ & 0xffff;	
	p5.h = (TC_2US_300MHZ >> 16) & 0xffff;	
					// AD1885 /Reset Active low for ca 
					// 2 usec (needs 1 us)
					// assuming 300 Mhz DSP Core

    	LOOP hold_reset LC0 = P5;	

LOOP_BEGIN hold_reset;
	nop;
LOOP_END hold_reset;

	P2.L = FIO_FLAG_S & 0xFFFF;
	P2.H = (FIO_FLAG_S >> 16) & 0xFFFF;
	R0.L = kLEDMix+kACResSet;	// De-assert AD1885 /RESET and turn on 
					// (~)half of the LEDs
	W[P2] = R0.L; ssync;
	
	p5.l = TC_3S_300MHZ & 0xffff;	
	p5.h = (TC_3S_300MHZ >> 16) & 0xffff;	

	// use another 3 seconds to allow codec to recover from reset
	// 162.8 ns needed before outputting BITCLK according to datasheet??

	LOOP reset_recovery_delay LC0 = P5;

LOOP_BEGIN reset_recovery_delay;
	nop;
LOOP_END reset_recovery_delay;

    	rts;
    
//------------------------------------------------------------------------------
// SPORT0 initialization
//------------------------------------------------------------------------------

initSPORT0:

	P0.H = (IMASK >> 16) & 0xFFFF;
	P0.L = IMASK & 0xFFFF;
	R2.L = W[P0];
	R0.L = EVT_IVG8;			// mask bit for IVG8
	R0 = R2 | R0;			
	W[P0] = R0.L;				// unmask IVG8
	csync;
	CLI R0;					// Disable interrupts

	i0.l = imaskCopy;
	i0.h = imaskCopy;
	p0 = i0;
	w[p0] = r0.l;				// store IMASK state 

//..............................................................................

	P0.H = (SPORT0_TX_CONFIG >> 16) & 0xFFFF;
	P0.L = SPORT0_TX_CONFIG & 0xFFFF;
	R1.L = 0x0000;
	W[P0] = R1.L; ssync;

	P0.L = SPORT0_RX_CONFIG &0xFFFF;
	W[P0] = R1.L; ssync;

	P0.L = SPORT0_MCMC1 & 0xFFFF;
	W[P0] = R1.L; ssync;		
    
    	P0.L = SPORT0_IRQSTAT_RX & 0xFFFF;
	R1.L = kIRQSTAT_RX;
    	W[P0] = R1.L; ssync;			// bits are cleared by writing
						// 1s to this register
    	P0.L = SPORT0_IRQSTAT_TX & 0xFFFF; 	
	R1.L = kIRQSTAT_TX;
    	W[P0] = R1.L;  ssync;			// bits are cleared by writing
						// 1s to this register
	// Set SPORT0 frame sync divisor rx

    	P0.L = SPORT0_RFSDIV & 0xFFFF;	    	// AC'97 48Khz Frame Sync with 
					    	// 12.288Mhz Input Clock 
    	R1.L = 0x00FF;
    	W[P0] = R1.L; ssync;

	// Set SPORT0 frame sync divisor tx

    	P0.L = SPORT0_TFSDIV & 0xFFFF;	    	// AC'97 48Khz Frame Sync with 
					    	// 12.288Mhz Input Clock 
    	R1.L = 0x00FF;
    	W[P0] = R1.L; ssync;

    	P0.L = SPORT0_RSCLKDIV & 0xFFFF;	// AC'97 48Khz Frame Sync with 
					    	// 12.288Mhz Input Clock??? 
    	R1.L = 0x0005;
    	W[P0] = R1.L; ssync;

//..............................................................................

	P0.L = SPORT0_RX_CONFIG & 0xFFFF;
	R1 = kRX_CFG;
	W[P0] = R1; ssync;

	P0.L = SPORT0_TX_CONFIG & 0xFFFF;
	R1 = kTX_CFG;
	W[P0] = R1; ssync;

//..............................................................................

	// Enable MCM Transmit and Receive Channels 

	P0.L = SPORT0_MTCS0 & 0xFFFF;	   	// Select 16 Channels for TX 
	R1.L = 0xFFFF;				// out of possibly 128				    
	W[P0++] = R1; ssync;
	
	R1.L = 0x0000;				
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
						// Select 16 Channels for RX
	R1.L = 0xFFFF;				// out of possibly 128				    
	W[P0++] = R1; ssync;

	R1.L = 0x0000;			
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
	W[P0++] = R1; ssync;
    
//..............................................................................

	// Set MCM Configuration Reg 1 

	P0.L = SPORT0_MCMC1 & 0xFFFF;
	R1.L = kMCMC1;
	W[P0] = R1.L; ssync;

	// Set MCM Configuration Reg 2 

	P0.L = SPORT0_MCMC2 & 0xFFFF;
	R1.L = kMCMC2;
	W[P0] = R1.L; ssync;

//..............................................................................

	// Unmask SPORT0 Rx DMA interrupt

	// Check revision of silicon 
	
	P0.L = chipId & 0xffff;
	P0.H = chipId >> 16;
	p1.l = SIC_IMASK & 0xffff;
	p1.h = SIC_IMASK >> 16;
	R3 = [P0];
	R6 = [p1];
	r2 = 0x1c04(z);		// length 4, position 28
				// make a macro out of this some other
				// time
	r4 = extract(r3,r2.l); 	// extract revision number
	cc = az;

	// If revision is 1.0 or higher, a "1" in 
	// SIC_IMASK will enable system interrupts
	// otherwise a "0" enable system interrupts 

	if cc jump rev0;

	bitset(r6,bSPORT0RxDMAInt);

	// Unmask the SPORT0 Rx DMA Interrupt 

	[ P1 ] = R6;
	rts;
rev0:
	bitclr(r6,bSPORT0RxDMAInt);

	// Unmask the SPORT0 Rx DMA Interrupt 

	[ P1 ] = R6;    
    	rts;

//------------------------------------------------------------------------------
// SPORT0 DMA initialization
//------------------------------------------------------------------------------

initSPORT0DMA: 

	// SPORT0 DMA AUTOBUFFER TX 

	P0.L = SPORT0_CONFIG_DMA_TX & 0xFFFF;		
	r3.l = kTxDAUTO;
	W[P0] = R3.L; ssync;		// set autobuffer mode
    
	P0.L = SPORT0_START_ADDR_LO_TX & 0xFFFF;
	p1.h = tx_buf;
	p1.L = tx_buf;		    	// SPORT0 TX DMA Internal Memory Address 
	r3 = p1;
	W[P0] = r3.L; ssync;

	P0.L = SPORT0_START_ADDR_HI_TX & 0xFFFF;
	W[P0] = r3.H; ssync;
	
	P0.L = SPORT0_COUNT_TX & 0xFFFF;
	R3 = LENGTH(tx_buf);	    	// SPORT0 TX DMA Internal Memory Count 
	W[P0] = R3; ssync;

//..............................................................................

	// SPORT0 DMA AUTOBUFFER RX 

	P0.L = SPORT0_CONFIG_DMA_RX & 0xFFFF;		
	bitset(r3,bRxDAUTO);
	W[P0] = R3.L;   ssync;		// set autobuffer mode
    
	P0.L = SPORT0_START_ADDR_LO_RX & 0xFFFF;
	p1.L = rx_buf;			// SPORT0 RX DMA Internal Memory Address
	p1.h = rx_buf;
	r3 = p1;
	W[P0] = r3.L; ssync;
	
	P0.L = SPORT0_START_ADDR_HI_RX & 0xFFFF;
	W[P0] = r3.H; ssync;
	
	P0.L = SPORT0_COUNT_RX & 0xFFFF;
	R3.L = LENGTH(rx_buf);	    	// SPORT0 RX DMA Internal Memory Count 
	W[P0] = R3.L; ssync;

//..............................................................................

	// ENABLE SPORT0 DMA and DIRECTION IN DMA CONFIGURATION REGISTER 

	P0.L = SPORT0_CONFIG_DMA_TX & 0xFFFF;
	R3.L = kTX_DMA_CFG;		
	W[P0] = R3.L; ssync;		// Enable and configure TX DMA 
	
	P0.L = SPORT0_CONFIG_DMA_RX & 0xFFFF;
	R3.L = kRX_DMA_CFG;		
	W[P0] = R3.L; ssync;		// Enable and configure RX DMA
	
	RTS;

//------------------------------------------------------------------------------
// AD1885 codec initialization
//------------------------------------------------------------------------------

initCodec:

	P0.L = SPORT0_RX_CONFIG & 0xFFFF;	
	R3.L = W[P0];
	bitset(r3,bRxRSPEN);		// Enable SPORT0 RX 
	W[P0] = R3.L;  ssync;

	P0.L = SPORT0_TX_CONFIG & 0xFFFF;	
	R3.L = W[P0];
	bitset(r3,bTxTSPEN);		// Enable SPORT0 TX 
	W[P0] = R3.L;  ssync;

	i0.l = imaskCopy;
	i0.h = imaskCopy;
	p0 = i0;
	r0.l = w[p0];			// restore IMASK state 

	STI R0;			    	// Enable Interrupts 
    	ssync; nop; nop;

	// idle and ssync will put the processor
	// in "idle state". The processor remains here until a peripheral or
	// external device such as a SPORT or the RTC generates an interrupt
	// that requires servicing.

    	idle; ssync;    		// Wait a few RX interrupts to ensure 
    	idle; ssync;    		// SPORT is up and running 

	r3 = 0(z);
	bitset(r3,bCodecRdy);

	R2.h = 0x0000;
	R2.l = 0x0000;

	P0.l = rx_buf;
	p0.h = rx_buf;

waitCodecReady:
	idle; ssync;
	R4.L = W[P0];	    // poll codec ready bit from AD1885 Tag - SLOT '0' 
	R4.h = 0x0000;
	R2 = R3 & R4;
	CC = AZ;
	if CC jump waitCodecReady;
	
//..............................................................................

#ifdef _EAGLE_35_               // [ _EAGLE_35_
	R5.L = kLEDMix5;	// turn on more LEDs indicating
				// codec-DSP communication established  
#else				// ] [ !_EAGLE_35_
#if defined(_EZKITLITE_) || defined(_HAWK_35_) // [ _EZKITLITE_ || _HAWK_35_
	R5.L = kLEDMix3;	// turn on another LED indicating
				// codec-DSP communication established  
#else				// ] [ !(_EZKITLITE_ || _HAWK_35_)
#error "Board not defined"
#endif             		// ] _EZKITLITE_ ||  _HAWK_35_
#endif				// ] _EAGLE_35_
	P0.L = FIO_FLAG_S & 0xFFFF;
	p0.h = (FIO_FLAG_S >> 16) & 0xFFFF;
	W[P0] = R5.L;

//..............................................................................

	// Write Various registers of AD1885 to configure it - read these 
	// same registers back to ensure they are properly configured 

	P0.l = initCodecRegs;
	P0.h = initCodecRegs;
	i3 = p0;			
	l3 = 0;

	P5 = CODEC_REG_INIT_LENGTH/2;	// sending 2 words each time 
	R6.L = kVldFrm|kVldCmd|kVldAddr;
	p0.h = tx_buf;
	p0.l = tx_buf;
	p0 += oTagPhase;
	nop;
	nop;
	nop;

	// Set 'Valid Slot' bits in TAG phase for SLOTS '0', '1' , '2' 

	W[P0] = R6.L;	

	LOOP Codec_Init LC0 = P5;

LOOP_BEGIN Codec_Init;
	P0.l = initCodecRegs;
	P0.h = initCodecRegs;
	nop;
	nop;
	nop;
	R6.L = W[i3++];		// Fetch next AD1885 register address 
	p1.l = tx_buf;
	p1.h = tx_buf;
	p1 += oCmdAddrSlot;
	nop;
	nop;
	W[P1] = R6; // put fetched codec register address into TX SLOT '1' 
	R6.L = W[i3++];		// Fetch register data contents 
	p1 += -oCmdAddrSlot;
	p1 += oCmdDataSlot;
	nop;
	nop;
	nop;
	W[P1] = R6; /* ...put fetched codec register data into TX SLOT '2' */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	idle; ssync; 
	idle; ssync;	/* ...wait for 2 TDM frames... */
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
LOOP_END Codec_Init;

//..............................................................................

	// Verify AD1885 register writes 

	p0.l = initCodecRegs;
	p0.h = initCodecRegs;
	i3 = p0;
	l3 = 0;
	p0.l = codecInitResults;
	p0.h = codecInitResults;
	i2 = p0;
	l2 = 0;
	
	P5 = CODEC_REG_INIT_LENGTH/2;	// store 2 words each time 
	R6.L = kVldFrm|kVldCmd;
	p0.l = tx_buf;
	p0.h = tx_buf;
	p0 += oTagPhase;			
	nop;
	nop;
	nop;

	// Set 'Valid Slot' bits in TAG phase for SLOTS '0', '1' 
	W[P0] = R6.L;	

	LOOP ad1885_register_status LC0 = P5;

LOOP_BEGIN ad1885_register_status;

	R4.L = W[i3++];		// get address
	i3 += 2;		// skip data
	R3.L = kAddrRead;	// signal read request
	R4 = R4 | R3;

	p1.l = tx_buf;
	p1.h = tx_buf;
	p1 += oCmdAddrSlot;
	nop;
	nop;
	nop;
	W[P1] = R4.L;	    // TX value out of Command address time slot 
	idle; ssync;					    

	// Wait for some AC'97 frames to go by, latency in getting data from 
	// AD1885. 

	// Note that we are continuously requesting the data for a specific address
	// until we change the contents of tx_buf.
	// This means we just need to wait enough frames for the codec to respond 
	// and then the data will always be there.

	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	idle; ssync;
	p0.l = rx_buf;
	p0.h = rx_buf;
	p0 += oStatusAddrSlot;
	nop;
	nop;
	nop;
	R4 = W[P0];
	w[i2++] = R4.L;		// Store address to results buffer 
	p0 += -oStatusAddrSlot;
	p0 += oStatusDataSlot;
	R4 = W[P0];	
	w[i2++] = R4.L;	    	// Store data to results buffer 
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
	nop;
LOOP_END ad1885_register_status;
	
	rts;		    



