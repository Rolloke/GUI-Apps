/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: SavicReg.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/SavicReg.h $
// CHECKIN:		$Date: 13.12.02 11:37 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 12.12.02 16:21 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __SAVICREG_H__
#define __SAVICREG_H__

#define PCF8563_SLAVE_ADR		0xA2

#define PCF8582_SLAVE_ADR		0xA0

// Registerdiffinition des PCF8563
#define PCF8563_CONTROL_STATUS1		0x00
#define PCF8563_CONTROL_STATUS2		0x01
#define PCF8563_SECONDS				0x02
#define PCF8563_MINUTES				0x03
#define PCF8563_HOURS				0x04
#define PCF8563_DAYS				0x05
#define PCF8563_WEEKDAYS			0x06
#define PCF8563_MONTH_CENTURY		0x07
#define PCF8563_YEARS				0x08
#define PCF8563_MINUTE_ALARM		0x09
#define PCF8563_HOUR_ALARM			0x0A
#define PCF8563_DAY_ALARM			0x0B
#define PCF8563_WEEKDAY_ALAEM		0x0C
#define PCF8563_CLKOUT_FREQUENCY	0x0D
#define PCF8563_TIMER_CONTROL		0x0E
#define PCF8563_TIMER_COUNTDOWN		0x0F

// Bitdefinitionen des PCF8563
#define PCF8563_CTRL1_TEST1_BIT		0x07
#define PCF8563_CTRL1_STOP_BIT		0x05
#define PCF8563_CTRL1_TESTC_BIT		0x03

#define	PCF8563_CTRL2_TITP_BIT		0x04
#define PCF8563_CTRL2_AF_BIT		0x03
#define PCF8563_CTRL2_TF_BIT		0x02
#define PCF8563_CTRL2_AIE_BIT		0x01
#define PCF8563_CTRL2_TIE_BIT		0x00

#define PCF8563_ALARM_AE_MIN_BIT		0x07
#define PCF8563_ALARM_AE_HOUR_BIT		0x07
#define PCF8563_ALARM_AE_DAY_BIT		0x07
#define PCF8563_ALARM_AE_WEEKDAY_BIT	0x07

#define PCF8563_CTKOUT_FE_BIT		0x07
#define PCF8563_CTKOUT_FD1_BIT		0x01
#define PCF8563_CTKOUT_FD0_BIT		0x00

#define PCF8563_TICTRL_TE_BIT		0x07
#define PCF8563_TICTRL_TD1_BIT		0x01
#define PCF8563_TICTRL_TD0_BIT		0x00

#define PCF8563_CENTURY_BIT			0x07

#define PCF8563_VL_BIT				0x07


#define BT878_MIN_REG  	0x000
#define BT878_MAX_REG  	0x200

// Registerdiffinition des BT878
#define BT878_DSTATUS					0x000
#define BT878_IFORM						0x004
#define BT878_TDEC						0x008

#define BT878_E_CROP					0x00C
#define BT878_O_CROP					0x08C

#define BT878_E_VDELAY_LO				0x090
#define BT878_O_VDELAY_LO				0x010

#define BT878_E_VACTIVE_LO				0x014
#define BT878_O_VACTIVE_LO				0x094

#define BT878_E_HDELAY_LO				0x018
#define BT878_O_HDELAY_LO				0x098

#define BT878_E_HACTIVE_LO				0x01C
#define BT878_O_HACTIVE_LO				0x09C

#define BT878_E_HSCALE_HI				0x020
#define BT878_O_HSCALE_HI				0x0A0

#define BT878_E_HSCALE_LO				0x024
#define BT878_O_HSCALE_LO				0x0A4

#define BT878_BRIGHT					0x028

#define BT878_E_CONTROL					0x02C
#define BT878_O_CONTROL					0x0AC

#define BT878_CONTRAST_LO				0x030

#define BT878_SAT_U_LO					0x034
#define BT878_SAT_V_LO					0x038

#define BT878_HUE						0x03C

#define BT878_E_SCLOOP					0x040
#define BT878_O_SCLOOP					0x0C0

#define BT878_WC_UP						0x044

#define BT878_OFORM						0x048

#define BT878_E_VSCALE_HI				0x04C
#define BT878_O_VSCALE_HI				0x0CC

#define BT878_E_VSCALE_LO				0x050
#define BT878_O_VSCALE_LO				0x0D0

#define BT868_ARESET					0x05B

#define BT878_ADELAY					0x060
#define BT878_BDELAY					0x064
#define BT878_ADC						0x068

#define BT878_E_VTC						0x06C
#define BT878_O_VTC						0x0EC

#define BT878_SRESET					0x07C

#define BT878_WC_DN						0x078
#define BT878_TGLB						0x080
#define BT878_TGCTRL					0x084

#define BT878_VTOTAL_LO					0x0B0
#define BT878_VTOTAL_HI					0x0B4

#define BT878_COLOR_FMT					0x0D4
#define BT878_COLOR_CTL					0x0D8

#define BT878_CAP_CTL					0x0DC

#define BT878_VBI_PACK_SIZE				0x0E0
#define BT878_VBI_PACK_DEL				0x0E4
#define BT878_FCAP						0x0E8

#define BT878_PLL_F_LO					0x0F0
#define BT878_PLL_F_HI					0x0F4
#define BT878_PLL_XCI					0x0F8
#define BT878_DVSIF						0x0FC

#define BT878_INT_STAT					0x100
#define BT878_INT_MASK					0x104
#define BT878_GPIO_DMA_CTL				0x10C
#define BT878_I2C_DATA_CTL				0x110

#define BT878_RISC_STRT_ADD				0x114
#define BT878_GPIO_OUT_EN				0x118
#define BT878_RISC_COUNT				0x120
#define BT878_GPIO_DATA_CTL				0x200

// Bitdefinitionen des BT829 Control-Registers
#define BT878_CONTROL_SAT_V_MSB_BIT		0x00
#define BT878_CONTROL_SAT_U_MSB_BIT		0x01
#define BT878_CONTROL_CON_MSB_BIT		0x02
#define BT878_CONTROL_INTERP_BIT		0x03
#define BT878_CONTROL_CBSENSE_BIT		0x04
#define BT878_CONTROL_LDEC_BIT			0x05
#define BT878_CONTROL_COMP_BIT			0x06
#define BT878_CONTROL_LNOTCH_BIT		0x07

// Bitdefinitionen des BT878 DSTATUS-Registers
#define	BT878_DSTATUS_COF_BIT			0x00
#define	BT878_DSTATUS_LOF_BIT			0x01
#define	BT878_DSTATUS_PLOCK_BIT			0x02
#define	BT878_DSTATUS_RESERVED_BIT		0x03
#define	BT878_DSTATUS_NUML_BIT			0x04
#define	BT878_DSTATUS_FIELD_BIT			0x05
#define	BT878_DSTATUS_HLOC_BIT			0x06
#define BT878_DSTATUS_PRES_BIT			0x07

// Definitionen des BT878 IFORM-Registers
#define BT878_IFORM_FORMAT_AUTO_BIT			(0x00)
#define BT878_IFORM_FORMAT_NTSC_M_BIT		(0x01)
#define BT878_IFORM_FORMAT_NTSC_BIT			(0x02)
#define BT878_IFORM_FORMAT_PAL_BDGHI_BIT	(0x03)
#define BT878_IFORM_FORMAT_PAL_M_BIT		(0x04)
#define BT878_IFORM_FORMAT_PAL_N_BIT		(0x05)
#define BT878_IFORM_FORMAT_SECAM_BIT		(0x06)
#define BT878_IFORM_FORMAT_PAL_N_COM_BIT	(0x07)

// Bitdefinitionen des BT878_I2C_DATA_CTL-Registers
#define BT878_I2CSDA_BIT				0x00
#define BT878_I2CSCL_BIT				0x01

// Bitdefinitionen des BT878 ADC Interface-Registers
#define BT878_ADC_C_SLEEP_BIT			0x01
#define BT878_ADC_Y_SLEEP_BIT			0x02
#define BT878_ADC_AGC_EN_BIT			0x04

// Bitdefinitionen des BT878 INST_STAT-Registers
#define BT878_INST_STAT_FMTCHG_BIT		0x00
#define BT878_INST_STAT_VSYNC_BIT		0x01
#define BT878_INST_STAT_HSYNC_BIT		0x02
#define BT878_INST_STAT_OFLOW_BIT		0x03
#define BT878_INST_STAT_HLOCK_BIT		0x04
#define BT878_INST_STAT_VPRES_BIT		0x05
#define BT878_INST_STAT_I2CDONE_BIT		0x08
#define BT878_INST_STAT_GPINT_BIT		0x09
#define BT878_INST_STAT_RISCI_BIT		0x0B
#define BT878_INST_STAT_FBUS_BIT		0x0C
#define BT878_INST_STAT_FTRGT_BIT		0x0D
#define BT878_INST_STAT_FDSR_BIT		0x0E
#define BT878_INST_STAT_PPERR_BIT		0x0F
#define BT878_INST_STAT_RIPERR_BIT		0x10
#define BT878_INST_STAT_PABORT_BIT		0x11
#define BT878_INST_STAT_OCERR_BIT		0x12
#define BT878_INST_STAT_SCERR_BIT		0x13
#define BT878_INST_STAT_FIELD_BIT		0x18
#define BT878_INST_STAT_RISC_EN_BIT		0x1B

// GPIO-Bitbelegung
#define BT878_GPIO_ALARM_IN0_BIT		0x00
#define BT878_GPIO_ALARM_IN1_BIT		0x01
#define BT878_GPIO_ALARM_IN2_BIT		0x02
#define BT878_GPIO_ALARM_IN3_BIT		0x03
#define BT878_GPIO_RELAY_BIT			0x04
#define BT878_GPIO_POWERSWITCH_BIT		0x05
#define BT878_GPIO_POWEROFF_BIT			0x06
#define BT878_GPIO_RESET_BIT			0x07
#define BT878_GPIO_RESETSWITCH_BIT		0x08

#define BT878_GPIO_LED1_BIT				0x09
#define BT878_GPIO_LED2_BIT				0x0a
#define BT878_GPIO_VS_ACTIVE_BIT		0x0b
#define BT878_GPIO_CLEAR_LATCH_BIT		0x0c
#define BT878_GPIO_ALARM_IN4_BIT		0x0d
#define BT878_GPIO_VS_MUX_A0_BIT		0x0e
#define BT878_GPIO_VS_MUX_A1_BIT		0x0f
#define BT878_GPIO_AA1_MUX_A0_BIT		0x10
#define BT878_GPIO_AA1_MUX_A1_BIT		0x11
#define BT878_GPIO_AA2_MUX_A0_BIT		0x12
#define BT878_GPIO_AA2_MUX_A1_BIT		0x13
#define BT878_GPIO_WATCHDOG_ACTIVE_BIT	0x14





// BT878-Color Formate
#define BT878_INST_STAT_
#define BT878_RGB32				0x00
#define BT878_RGB24				0x01
#define BT878_RGB16				0x02
#define BT878_RGB15				0x03
#define BT878_YUV2				0x04
#define BT878_BTYUV				0x05
#define BT878_Y8				0x06
#define BT878_RGB8				0x07
#define BT878_YUV12				0x08
#define BT878_YUV9				0x09

// Bitdefinitionen des BT878 BT878_GPIO_DMA_CTL-Registers
#define BT878_GPIO_DMA_CTL_FIFO_ENABLE_BIT		0x00
#define BT878_GPIO_DMA_CTL_RISC_ENABLE_BIT		0x01
#define BT878_GPIO_DMA_CTL_GPIOMODE0_BIT		0x0B
#define BT878_GPIO_DMA_CTL_GPIOMODE1_BIT		0x0C

/* Bt848 RISC commands */

/* only for the SYNC RISC command */
#define BT878_FIFO_STATUS_FM1  0x06
#define BT878_FIFO_STATUS_FM3  0x0e
#define BT878_FIFO_STATUS_SOL  0x02
#define BT878_FIFO_STATUS_EOL4 0x01
#define BT878_FIFO_STATUS_EOL3 0x0d
#define BT878_FIFO_STATUS_EOL2 0x09
#define BT878_FIFO_STATUS_EOL1 0x05
#define BT878_FIFO_STATUS_VRE  0x04
#define BT878_FIFO_STATUS_VRO  0x0c
#define BT878_FIFO_STATUS_PXV  0x00

#define BT878_RISC_RESYNC      (1<<15)

/* WRITE and SKIP */
/* disable which bytes of each DWORD */
#define BT878_RISC_BYTE0       (1<<12)
#define BT878_RISC_BYTE1       (1<<13)
#define BT878_RISC_BYTE2       (1<<14)
#define BT878_RISC_BYTE3       (1<<15)
#define BT878_RISC_BYTE_ALL    (0x0f<<12)
#define BT878_RISC_BYTE_NONE   0
/* cause RISCI */
#define BT878_RISC_IRQ         (1<<24)
/* RISC command is last one in this line */
#define BT878_RISC_EOL         (1<<26)
/* RISC command is first one in this line */
#define BT878_RISC_SOL         (1<<27)

#define BT878_RISC_WRITE       (0x01<<28)
#define BT878_RISC_SKIP        (0x02<<28)
#define BT878_RISC_WRITEC      (0x05<<28)
#define BT878_RISC_JUMP        (0x07<<28)
#define BT878_RISC_SYNC        (0x08<<28)

#define BT878_RISC_WRITE123    (0x09<<28)
#define BT878_RISC_SKIP123     (0x0a<<28)
#define BT878_RISC_WRITE1S23   (0x0b<<28)

/************************************************************************/
/********* Register Definitionen des SaVic-Boards *********************/
/************************************************************************/


#endif  // __SAVICREG_H__
