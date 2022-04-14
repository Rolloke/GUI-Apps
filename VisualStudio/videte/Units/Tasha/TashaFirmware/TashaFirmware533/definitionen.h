/********************************************************************/
/* Definitionen Header												*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __DEF_DEFINED
	#define __DEF_DEFINED
	
#define FRAMEADDRESS 0x000000


#define CORE_CLK 		594000000     // 594 MHZ (22x27MHz)
#define SCLK			148000000	  // 148 MHz (594MHz/4)	

#define TIMER0FREQ  2 			 	// 2  Hz
//FIO FALG Register 
#define SDA  PF8	// Daten an PF8  
#define SCK  PF7	// Clock an PF7 
#define CS   PF6	// FrontEnd Reset

//FIO DIR Register
#define IN   CS|SCK 		//SDA IN  ,CS 
#define OUT  CS|SCK|SDA 	//SDA OUT ,CS 

//DMA Discriptor
#define DESCRIPTOR_SIZE 22 

//DMA Controler 
#define DEN 	0x0001
#define MEMWR  	0x0002
#define SIZE_08 0x0000
#define SIZE_16 0x0004
#define SIZE_32 0x0008	
#define DMA2    0x0010
#define BUFFC   0x0020
#define I2D		0x0040
#define IEN     0x0080
#define FLEXA	0x0000
#define FLOW_A  0x1000
#define FLOW_DA 0x4000
#define FLOW_DS 0x6000
#define FLOW_DL 0x7000

//Bildabmessungen
#define COL 720	// Spalten   (x)
#define ROW 576 // Zeilen 	 (y)
#define ROWNTSC 480 //Zeilen

// 1 Halbbild = 720 * 288 * 2(YC)  
#define FRAME COL * ROW 
#define FRAMENTSC COL * ROWNTSC

// Header Offset x/y Koordinaten
#define OFF 0

//Blockgröße 8*8
#define BLOCK 8

//Alarm Schwellwert Differenzbild
#define THRESHOLD 20		

//Alarm Schwellwert adaptive Maske
#define THRESHOLD1 20		

//abaptive Maske Modify
#define MOD 6

//adaptive Maske nach ... Frames
#define MASKDELAY 0

//LSB 
#define LSB 0x0001


// I2C FrontEnd
#define SLAVE_ADDRESS 	0x4A  // 0x88 AD 
#define FE_CONTRAST		0x0B  // [0..71..127] 
#define FE_SATURATION	0x0C  // [0..71..127] 	
#define FE_BRIGHT		0x0A  // [0..128..255]
#define FE_HUE			0x0D  // [-128..127][ -180°,0,+178,6°] 
#define FE_STATUS		0x1F  // Interlace|Lock H/Lock V|PAL/NTSC|Lumi MAX|Lumi MIN|White Peak|Slow Time/Macrovision|Right Standard/Ready|
							  //      7       	 6	        5        4         3        2          	 1                  	  0
#define STAT_READY		0x01
#define STAT_NTSC		0x20
#endif //__DEF_DEFINED
