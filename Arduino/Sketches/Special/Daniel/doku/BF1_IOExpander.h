// ----------------------------------------------------------------------
//   Blackfin BF537
//
//   Funktion zur Bedienung des IO-Expanders auf der EBE284
// ----------------------------------------------------------------------
//
//   Copyright (c) 2001-2011 ELAC-Nautik GmbH.
//
//   WEB		:	http://www.elac-nautik.com
//   Support	:	support@elac-nautik.com
//
//
//   Filename	:	BF1_IOExpander.h
//   Autor		:	M. Sieger
//
// ----------------------------------------------------------------------

#ifndef _BF1_IOEXP_H
#define _BF1_IOEXP_H

#include "BF1_locals.h"

// NOOP-Kommando für unbenutzten IO-Expander
#define	IOEXP_NOOP				0
#define	IOEXP_TRANSITIONMASK	0x25	// P29 | P26 | P24
#define	IOEXP_ENABLETRANSITION	0x81	// M=1 (Enable Transition) | S=1 (Normal operation)



// Kommandos für die Bedienung der IO-Expander (siehe Datenblatt)
// beim IO-Expander #2 (D1) ist Bit 8 zur Unterscheidung gesetzt
typedef enum {
	// Einzelbits von D2
    IO_Modbus_Adr0	    = 0x24,         // 0x240x
    IO_Modbus_Adr1,			            // 0x250x
    IO_Modbus_Adr2,			            // 0x260x
    IO_Modbus_Adr3,			            // 0x270x
    IO_Modbus_Adr4,			            // 0x280x
    IO_Modbus_Adr5,			            // 0x290x
    IO_Modbus_Adr6,			            // 0x2A0x
    IO_Modbus_Adr7,			            // 0x2B0x
    IO_BF561_installed,	                // 0x2C0x
    IO_Modbus_Baudrate,	                // 0x2D0x
    IO_Freq_ID,	                        // 0x2E0x
    IO_Activation_Freq,		            // 0x2F0x
    IO_PLL_PowerDown,		            // 0x300x
    IO_Modbus_Interface1,               // 0x310x
    IO_Modbus_Protocol,                 // 0x320x
    IO_TxPower_0dB,			            // 0x330x
    IO_TxPower_6dB,		                // 0x340x
    IO_TxPower_12dB,		            // 0x350x
    IO_TxPower_18dB,		            // 0x360x
    IO_Dummylast,			            // 0x370x
    IO_USV_OK,				            // 0x380x
    IO_BF561_Reset,			            // 0x390x
    IO_USV_ERROR,			            // 0x3A0x
    IO_Status_Activated,                // 0x3B0x
    IO_Status_Error,		            // 0x3C0x
    IO_Status_Disable,	                // 0x3D0x
    IO_Status_Ready,		            // 0x3E0x
    IO_PortChange_Output_1,             // 0x3F0x
    
    // Kommandos für D2
    IO_ConfigurationRegister = 0x04,	// 0x04xx = Configuration Register
    IO_ReadTransitionFlags   = 0x58,	// 0x5800 = Port 24..31 Register
    IO_TransitionDetectMask  = 0x06,	// 0x06xx = Transition Detect Mask Register
    
    // 8 Bit-Ports von D2
    IO_Modbus_Adress     = 0x44, 
    
    // Einzelbits von D1
    IO_Enable_TxVoltage  = 0x124,	    // 0x240x
    IO_Enable_Rx1,			            // 0x250x
    IO_Enable_Rx2,			            // 0x260x
    IO_Attenuator,			            // 0x270x
    IO_Enable_Simulator,                // 0x280x
    IO_Enable_Rx1Test,		            // 0x290x
    IO_Enable_Rx2Test,		            // 0x2A0x
    IO_unused_2,					    // 0x2B0x
    IO_Spare2_In,				        // 0x2C0x
    IO_Test1,					        // 0x2D0x
    IO_Test2,					        // 0x2E0x
    IO_Test3,					        // 0x2F0x
    IO_Test4,					        // 0x300x
    IO_Modbus_Interface2,				// 0x310x	
    IO_unused_3,					    // 0x320x
    IO_LED1,					        // 0x330x
    IO_LED2,					        // 0x340x
    IO_LED3,					        // 0x350x
    IO_LED4,					        // 0x360x
    IO_unused_4,				        // 0x370x
    IO_unused_5,				        // 0x380x
    IO_unused_6,				        // 0x390x
    IO_unused_7,				        // 0x3A0x
    IO_unused_8,				        // 0x3B0x
    IO_unused_9,                        // 0x3C0x
    IO_unused_10,				        // 0x3D0x
    IO_unused_11,			            // 0x3E0x
    IO_PortChange_Output_2              // 0x3F0x
} enum_IOExp_Command;    
    
    
unsigned char 	IOExp_Read 						(enum_IOExp_Command cmd);
void 			IOExp_Write 					(enum_IOExp_Command cmd, unsigned char data);
void 			IOExp_Write32 					(unsigned short D1_data, unsigned short D2_data);
void 			IOExp_Init 						(void);
void			IOExp_EnableTransitionDetection	(void);

#endif
