/////////////////////////////////////////////////////////////////////////////
//	Project:	MiCoISA
//
//	File:		a3micoio.h
//
//	Description:
//
//	Start:		30.11.95
//
//	Last change:01.12.95
//
//	Author:		Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MICOIO_H__
#define __MICOIO_H__

#define SETBIT(w,b)    ((WORD)((WORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((WORD)((WORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((WORD)(w) & (1L << (b)) ? TRUE : FALSE))

// Definitionen des I2C-Bus-Kontrollers PCF8584
#define	I2C_OWN_ADDRESS_REG		0x00			// Own address is only accessible right
#define	I2C_CONTROL_REG			0x01			// Control Register offset
#define	I2C_STATUS_REG			0x01			// Status Register
#define	I2C_CLOCK_REG			0x02			// I2C Clock Control register
#define	I2C_INT_REG				0x03			// Interrupt Vector Register
#define	I2C_DATA_REG			0x00			// Data	Register usable	after init

#define	I2C_SERIAL_CLOCK_FIELD	0x03			// 2 bit field for serial clock
#define	I2C_SERIAL_CLOCK_90KHZ	0x00			// 90kHz serial	clock
#define	I2C_SERIAL_CLOCK_45KHZ	0x01			// 45kHz Serial	clock
#define	I2C_SERIAL_CLOCK_11KHZ	0x02			// 11kHz Serial	Clock
#define	I2C_SERIAL_CLOCK_1500HZ	0x03			// 1.5kHz Serial Clock

#define	I2C_CLOCK_FIELD			0x1A			// PCD8584 Input Clock
#define	I2C_CLOCK_3MHZ			0x00			// 3MHz	clock attached
#define	I2C_CLOCK_4_43MHZ		0x10			// 4.43MHz clock attached
#define	I2C_CLOCK_6MHZ			0x14			// 6MHz	clock attached
#define	I2C_CLOCK_8MHZ			0x18			// 8MHZ	clock attached
#define	I2C_CLOCK_12MHZ			0x1C			// 12MHZ clock attached

#define	I2C_ESO_BIT				0x40			// Enable Serial Output	bit
#define	I2C_SERIAL_BUS_ON		0x40			// Serial Bus ON
#define	I2C_SERIAL_BUS_OFF		0x00			// SERIAL BUS OFF
#define	I2C_ES1_BIT				0x20			// Extended select bit 1
#define	I2C_ES2_BIT				0x10			// Extended select bit 2
#define	I2C_INT_ENB_BIT			0x08			// Interrupt Enable bit
#define	I2C_START_BIT			0x04			// Start Bit
#define	I2C_STOP_BIT			0x02			// Stop	Bit
#define	I2C_ACK_BIT				0x01			// Int Ack Control Bit

#define	I2C_STS_BIT				0x20			// Slave Stop Detected
#define	I2C_BER_BIT				0x10			// Bus Error Detected
#define	I2C_LRB_BIT				0x08			// Last	Received Bit
#define	I2C_AAS_BIT				0x04			// Addressed as	Slave Bit
#define	I2C_LAB_BIT				0x02			// Lost	Arbitration Bit
#define	I2C_BB_N				0x01			// Bus Busy Not	Bit 
												// 1 = Not Busy
												// 0 = Busy
#define	I2C_PIN_BIT				0x80			// Pending Interrupt Not
												// 1 = No Pending Interrupt
												// 0 = No Interrupt Pending, i.e
												//     the command is completed.
#define	I2C_START_CMD			0x04			// Start Command
#define	I2C_REPEAT_START_CMD	0x04			// Repeat Start	Command
#define	I2C_STOP_CMD			0x02			// Stop	Command
#define	I2C_CHAIN_CMD			0x01			// Data	Chaining Command
#define	I2C_NOP_CMD				0x00			// No Op command

// Bitdefinitionen des BT829 Control-Registers
#define SAT_V_MSB_BIT			0x00
#define SAT_U_MSB_BIT			0x01
#define CON_MSB_BIT				0x02
#define INTERP_BIT				0x03
#define CBSENSE_BIT				0x04
#define LDEC_BIT				0x05
#define COMP_BIT				0x06
#define LNOTCH_BIT				0x07

// Bitdefinitionen des BT829 ADC Interface-Registers
#define C_ADC_BIT				0x01
#define Y_ADC_BIT				0x02

// Slaveadressen des Videodecoders und Videoencoders
#define BT829_SLAVE_ADR			0x8A
#define BT856_SLAVE_ADR			0x88

// Index zur Registerauswahl für die 4 Videomodi
#define NTSC_MODE				0x00
#define PAL_MODE				0x01
#define NTSC_CIF_MODE			0x02
#define PAL_CIF_MODE			0x03

// Registerdiffinition des BT829
#define BT829_STATUS			0x00
#define BT829_IFORM				0x01
#define BT829_TDEC				0x02
#define BT829_CROP				0x03
#define BT829_VDELAY_LO			0x04
#define BT829_VACTIVE_LO		0x05
#define BT829_HDELAY_LO			0x06
#define BT829_HACTIVE_LO		0x07
#define BT829_HSCALE_HI			0x08
#define BT829_HSCALE_LO			0x09
#define BT829_BRIGHT			0x0a
#define BT829_CONTROL			0x0b
#define BT829_CONTRAST_LO		0x0c
#define BT829_SAT_U_LO			0x0d
#define BT829_SAT_V_LO			0x0e
#define BT829_HUE				0x0f
#define BT829_OFORM				0x12
#define BT829_VSCALE_HI			0x13
#define BT829_VSCALE_LO			0x14
#define BT829_TEST				0x15
#define BT829_VPOLE				0x16
#define BT829_IDCODE			0x17
#define BT829_ADELAY			0x18
#define BT829_BDELAY			0x19
#define BT829_ADC				0x1a
#define BT829_SRESET			0x1f

// Registerdefinition des BT856
#define BT856_GETID				0x00 // Dieses Register gibt es nicht, es wird nur zu internem Zweck verwendet
#define BT856_RESERVED			0xc0 // 1. Register des BT856
#define BT856_CCDA				0xce
#define BT856_CCDB				0xd0
#define BT856_CCDC				0xd2
#define BT856_CCDD				0xd4
#define BT856_HSYNC_BEG			0xd6
#define BT856_HSYNC_END			0xd8
#define BT856_HSYNC_TIMING		0xda
#define BT856_MODE_CONTROL		0xdc
#define BT856_CAP_MODE_CONTROL	0xde


#define ZR_CB_DATA_OFFSET			0x00
#define ZR_ADDRESS_OFFSET			0x02
#define ZR_DATA_OFFSET				0x04
#define ZR_STATUS0_OFFSET			0x05
#define ZR_LEN0_OFFSET				0x06
#define ZR_LEN1_OFFSET				0x07
#define ZR_COM2_OFFSET				0x08
#define ZR_COM0_OFFSET				0x09
#define ZR_COM1_OFFSET				0x0a
#define ZR_FCNT_OFFSET				0x0b
#define ZR_SB_DATA_OFFSET			0x0c
#define ZR_SIZE_OFFSET				0x0e

// Bitdefinitionen des Status0-Registers
#define STATUS0_PG0_BIT				0x00
#define STATUS0_PG1_BIT				0x01
#define STATUS0_RDY_BIT				0x03
#define STATUS0_SBA_BIT				0x04
#define STATUS0_FNV_BIT				0x05

// Bitdefinitionen des COM1-Registers
#define COM0_PCG0_BIT				0x00
#define COM0_PCG1_BIT				0x01
#define COM0_STILL_BIT				0x02
#define COM0_EXP_BIT				0x03
#define COM0_VID_422_BIT			0x04
#define COM0_GRESET_BIT				0x05
#define COM0_FFI_BIT				0x06
#define COM0_FAST_BIT				0x07


// Bitdefinitionen des COM1-Registers
#define COM1_ACTIVE_BIT				0x00
#define COM1_CB_SIZE_BIT			0x01
#define COM1_VER_DEC_BIT			0x02
#define COM1_HOR_DEC_BIT			0x03
#define COM1_SNC_SRC_BIT			0x04
#define COM1_CLK_EN_BIT				0x05
#define COM1_INT_EN_BIT				0x06
#define COM1_VID_FMT_BIT			0x07

// Bitdefinitionen des COM2-Registers
#define COM2_ID0_BIT				0x00
#define COM2_ID1_BIT				0x01
#define COM2_ID2_BIT				0x02
#define COM2_ID3_BIT				0x03
#define COM2_ID4_BIT				0x04
#define COM2_ID5_BIT				0x05
#define COM2_VID_FILT_BIT			0x06
#define COM2_STRP_SIZE_BIT			0x07

#define RESET_OFFSET				0x10
#define PARAM_OFFSET				0x10
#define CSVINDN_OFFSET				0x11
#define CSVINAN1_OFFSET				0x12
#define CSVINAN2_OFFSET				0x13
#define WATCHDOG_OFFSET				0x14
#define RELAY_OFFSET				0x14
#define ALARM_CUR_STATUS_OFFSET		0x15
#define ALARM_EDGE_SEL_OFFSET		0x15
#define ALARM_STATUS_OFFSET			0x16
#define ALARM_ACK_OFFSET			0x16
#define MICO_ID_OFFSET				0x17
#define MICO_IER_OFFSET				0x18
#define MICO_IRR_OFFSET				0x19
#define MICO_IRQ_EMU_OFFSET			0x19
#define PCF8584_OFFSET	  			0x1a
#define PTC_COUNT0_OFFSET			0x1c
#define PTC_COUNT1_OFFSET			0x1d
#define PTC_COUNT2_OFFSET			0x1e
#define PTC_CONTROL_OFFSET			0x1f

// Bitdefinitionen des CSVDIN-Latches (0x11)
#define SELECT_VD_BIT			0x00
#define SELECT_I2C_BIT			0x01
#define SELECT_MEGRA_BIT		0x02
#define SELECT_SPARE2_BIT		0x03
#define SELECT_SKPNVS_BIT		0x04
#define SELECT_SKPSVS_BIT		0x05
#define SELECT_SPARE3_BIT		0x06
#define SELECT_SPARE4_BIT		0x07

// Bitdefinitionen des Reset und Parameter Register (0x10)
#define PARAM_RESET_BT829_BIT	0x00
#define PARAM_RESET_VIF_BIT		0x01
#define PARAM_RESET_BT856_BIT	0x02
#define PARAM_RESET_PCF8584_BIT 0x03
#define PARAM_RESET_ZR			0x04
#define PARAM_SQUARE_BIT		0x05
#define PARAM_SQUARE2_BIT		0x06
#define PARAM_SPARE1_BIT		0x07

// Bitdefininitionen des Spare/Relay/Watchdog-Registers	(0x14)
#define RELAY0_BIT				0x00
#define RELAY1_BIT				0x01
#define RELAY2_BIT				0x02
#define RELAY3_BIT				0x03
#define WATCHDOG_ENABLE_BIT		0x04
#define RGBOUT_BIT				0x05
#define SPARE5_BIT				0x06
#define SPARE6_BIT				0x07

/********* INTERRUPT ENABLE REGISTER BITS *******************************/
#define ZR_IE_BIT			    0x03
#define VI_IE_BIT		        0x04
#define VID_AES_BIT				0x05
#define PULLDPWN_BIT			0x06

/********* INTERRUPT REQUEST REGISTER BITS ******************************/
#define ZR_IR_BIT				0x00
#define ZR_EMU_BIT				0x00
#define VI_IR_BIT				0x01
#define VI_EMU_BIT				0x01
#define FIELD_BIT			    0x02
#define VSYNC_BIT				0x03

// Definitionen des CoVi's
#define ALARM_CUR_STATUS_COVI_OFFSET 0x00
#define ALARM_EDGE_SEL_COVI_OFFSET	 0x00
#define ALARM_STATUS_COVI_OFFSET	 0x01
#define ALARM_ACK_COVI_OFFSET		 0x01
#define CSVINAN_COVI_OFFSET			 0x02
#define RELAY_COVI_OFFSET			 0x03

#endif  // __MICOIO_H__
