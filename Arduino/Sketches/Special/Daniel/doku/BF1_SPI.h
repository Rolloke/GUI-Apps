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
//   Filename	:	BF1_SPI.h
//   Autor		:	M. Sieger
//
// ----------------------------------------------------------------------

#ifndef _BF1_SPI_H
#define _BF1_SPI_H

void SPI_SendSingleWord (unsigned short data);
void SPI_Init 			(void);

#endif
