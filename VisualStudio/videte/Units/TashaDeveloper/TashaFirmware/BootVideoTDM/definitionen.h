/********************************************************************/
/* Definitionen Header												*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __DEF_DEFINED
	#define __DEF_DEFINED

#define	B0	0xFF000000
#define	B1	0x00FF0000
#define	B2	0x0000FF00
#define	B3	0x000000FF
#define	W0	0xFF00
#define	W1	0x00FF
	
#define SwapEndian(x) 		(int)((((x)<<8)&B0)|(((x)<<8)&B2)|(((x)>>8)&B1)|(((x)>>8)&B3))
#define SwapEndianSmal(x) 	(short int)((((x)<<8)&W0)|(((x)>>8)&W1))	
//FIO FALG Register 
#define SDA  0x0002 //PF1 
#define SCK  0x0001 //PF2 

//FIO DIR Register
#define IN   0x0005 //SDA IN 
#define OUT  0x0007 //SDA OUT 

//BOOLSCHE Konstanten
#define TRUE    1
#define FALSE   0

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

//DMA Discriptor
#define DESCRIPTOR_SIZE 24 

//Bildabmessungen
#define COL 720	// Spalten   (x)
#define ROW 576 // Zeilen 	 (y)

// 1 Halbbild = 720 * 288 * 2(YC)  
#define FRAME COL * ROW 

// Header Offset x/y Koordinaten
#define OFF 12

//Blockgröße 8*8
#define BLOCK 8

//Alarm Schwellwert Differenzbild
#define THRESHOLD 20		

//Alarm Schwellwert adaptive Maske
#define THRESHOLD1 20		

//abaptive Maske Modify
#define MOD 6
#endif __DEF_DEFINED
