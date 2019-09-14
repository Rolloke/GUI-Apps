// ----------------------------------------------------------------------
//   Blackfin BF537
//
//   Funktion zur Bedienung des SPI-Ports des BF537
// ----------------------------------------------------------------------
//
//   Copyright (c) 2001-2011 ELAC-Nautik GmbH.
//
//   WEB		:	http://www.elac-nautik.com
//   Support	:	support@elac-nautik.com
//
//
//   Filename	:	BF1_SPI.c
//   Autor		:	M. Sieger
//
// ----------------------------------------------------------------------

#include <cdefBF537.h>
#include "BF1_locals.h"
#include "BF1_SPI.h"



// ----------------------------------------------------------------------

void SPI_SendSingleWord (unsigned short data)
{
	volatile unsigned short dummy;
	
	*pSPI_TDBR = data;
	dummy = *pSPI_RDBR;
	ssync();
	while ((*pSPI_STAT & RXS) != RXS) ;	// warten auf Transferende
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
/*! \brief	Initialisierung der SPI-Schnittstelle
 *  
 *  alle SSEL auf 1 setzen
 *  Baudrate auf 16.7 MHz
 */

void SPI_Init (void)
{
	// enable SSELx und alle SSEL auf 1 setzen
	*pSPI_FLG = 0xFF00;		// alle SSEL auf 1
				
//	*pSPI_BAUD = 2;			// Baudrate = 25.0 MHz
	*pSPI_BAUD = 3;			// Baudrate = 16.7 MHz
}
