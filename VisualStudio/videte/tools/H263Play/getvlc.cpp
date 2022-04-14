// getvlc.cpp, variable length decoding for tmndecode (H.263 decoder)


#include "stdafx.h"
#include "H263Decoder.h"
#include "tables.h"

int CH263Decoder::getTMNMV()
{
	int code;
	
	if (trace)
		TRACE("motion_code (");
	
	if (m_Bits.getbits1())
	{
		if (trace)
			TRACE("1): 0\n");
		return 0;
	}
	
	if ((code = m_Bits.showbits(12))>=512)
	{
		code = (code>>8) - 2;
		m_Bits.flushbits(TMNMVtab0[code].len);
		
		if (trace)
		{
			TRACE("0");
			printbits(code+2,4,TMNMVtab0[code].len);
			TRACE("): %d\n", TMNMVtab0[code].val);
		}
		
		return TMNMVtab0[code].val;
	}
	
	if (code>=128)
	{
		code = (code>>2) -32;
		m_Bits.flushbits(TMNMVtab1[code].len);
		
		if (trace)
		{
			TRACE("0");
			printbits(code+32,10,TMNMVtab1[code].len);
			TRACE("): %d\n",TMNMVtab1[code].val);
		}
		
		return TMNMVtab1[code].val;
	}
	
	if ((code-=5)<0)
	{
		if (!m_bQuiet)
		{
			TRACE("Invalid motion_vector code\n");
		}
		fault=1;
		return 0;
	}
	
	m_Bits.flushbits(TMNMVtab2[code].len);
	
	if (trace)
	{
		TRACE("0");
		printbits(code+5,12,TMNMVtab2[code].len);
		TRACE("): %d\n",TMNMVtab2[code].val);
	}
	
	return TMNMVtab2[code].val;
}


int CH263Decoder::getMCBPC()
{
	int code;
	
	if (trace)
		TRACE("MCBPC (");
	
	code = m_Bits.showbits(9);
	
	if (code == 1) {
		/* macroblock stuffing */
		if (trace)
			TRACE("000000001): stuffing\n");
		m_Bits.flushbits(9);
		return 255;
	}
	
	if (code == 0) {
		if (!m_bQuiet)
		{
			fprintf(stderr,"Invalid MCBPC code\n");
		}
		fault = 1;
		return 0;
	}
    
	if (code>=256)
	{
		m_Bits.flushbits(1);
		if (trace)
			TRACE("1): %d\n",0);
		return 0;
	}
    
	m_Bits.flushbits(MCBPCtab[code].len);
	
	if (trace)
	{
		printbits(code,9,MCBPCtab[code].len);
		TRACE("): %d\n",MCBPCtab[code].val);
	}
	
	return MCBPCtab[code].val;
}

int CH263Decoder::getMODB()
{
	int code;
	int MODB;
	
	if (trace)
		TRACE("MODB (");
	
	code = m_Bits.showbits(2);
	
	if (code < 2) {
		if (trace)
			TRACE("0): MODB = 0\n");
		MODB = 0;
		m_Bits.flushbits(1);
	}
	else if (code == 2) {
		if (trace)
			TRACE("10): MODB = 1\n");
		MODB = 1;
		m_Bits.flushbits(2);
	}
	else { /* code == 3 */
		if (trace)
			TRACE("11): MODB = 2\n");
		MODB = 2;
		m_Bits.flushbits(2);
	}
	return MODB;
}


int CH263Decoder::getMCBPCintra()
{
	int code;
	
	if (trace)
		TRACE("MCBPCintra (");
	
	code = m_Bits.showbits(9);
	
	if (code == 1) {
		/* macroblock stuffing */
		if (trace)
			TRACE("000000001): stuffing\n");
		m_Bits.flushbits(9);
		return 255;
	}
	
	if (code < 8) {
		if (!m_bQuiet) 
		{
			TRACE("Invalid MCBPCintra code\n");
		}
		fault = 1;
		return 0;
	}
	
	code >>= 3;
    
	if (code>=32)
	{
		m_Bits.flushbits(1);
		if (trace)
			TRACE("1): %d\n",3);
		return 3;
	}
	
	m_Bits.flushbits(MCBPCtabintra[code].len);
	
	if (trace)
	{
		printbits(code,6,MCBPCtabintra[code].len);
		TRACE("): %d\n",MCBPCtabintra[code].val);
	}
	
	return MCBPCtabintra[code].val;
}

int CH263Decoder::getCBPY()
{
	int code;
	
	if (trace)
		TRACE("CBPY (");
	
	code = m_Bits.showbits(6);
	if (code < 2) {
		if (!m_bQuiet)
		{
			TRACE("Invalid CBPY code\n");
		}
		fault = 1;
		return -1;
	}
    
	if (code>=48)
	{
		m_Bits.flushbits(2);
		if (trace)
			TRACE("11): %d\n",0);
		return 0;
	}
	
	m_Bits.flushbits(CBPYtab[code].len);
	
	if (trace)
	{
		printbits(code,6,CBPYtab[code].len);
		TRACE("): %d\n",CBPYtab[code].val);
	}
	
	return CBPYtab[code].val;
}

