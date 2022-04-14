/********************************************************************/
/* Definitionen Header												*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __DEF_DEFINED
	#define __DEF_DEFINED
	


#define H_SIZE 	240
#define V_SIZE	136
#define CHANNEL 3
#define SLAVE 7

#define FRAMEADDRESS 0x00000000 //0x65400  //0x00000000

#define SCLK		135000000		// 135 MHz	
#define CCLK 		540000000    	// 540 MHZ
#define TIMER0FREQ  5 			 	// 10  Hz
//FIO FALG Register 
#define SDA  PF8	//Daten an PF8  
#define SCK  PF7	//Clock an PF7 
#define CS   PF6

//FIO DIR Register
#define IN   CS|SCK 		//SDA IN  ,CS 
#define OUT  CS|SCK|SDA 	//SDA OUT ,CS 

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

// SPORT0 word length
#define SLEN_32	0x001f
#define SLEN_16	0x000f
#define SLEN_8	0x0007

// SPI 
#define SPIDMARX 	0x0002
#define SPIDMATX    0x0003
#define SPISIZE_16  0x0100
#define SPISIZE_8	0x0000

//DMA Discriptor
#define DESCRIPTOR_SIZE 22 
#define SPI_DESCRIPTOR_SIZE 5 

//Bildabmessungen
#define COL 720	// Spalten   (x)
#define ROW 576 // Zeilen 	 (y)

// 1 Halbbild = 720 * 288 * 2(YC)  
#define FRAME COL * ROW 

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
#define SLAVE_ADDRESS 	0x88
#define FE_CONTRAST		0x08  // [0..255] 
#define FE_SATURATION	0x09  // [0..255] 	[-42dB,0,+6dB]
#define FE_BRIGHT		0x0A  // [-128..127][ -3db,0,+3dB]
#define FE_HUE			0x0B  // [-128..127][ -90°,0,+90°] 		

#endif //__DEF_DEFINED
