//  getblk.cpp, DCT block decoding for tmndecode (H.263 decoder)

#include "stdafx.h"

#include "H263Decoder.h"

#include "tables.h"

#define INDICES
#include "indices.h"

#define SACTABLES
#include "sactbls.h"

void CH263Decoder::getblock(int comp,int mode)
{
	int val, i, j, sign;
	unsigned int code;
	VLCtab *tab;
	short *bp;
	int run, last, level, QP;
	short *qval;
	
	bp = block[comp];   
	
	/* decode AC coefficients */
	for (i=(mode==0); ; i++) 
	{
		code = m_Bits.showbits(12);
		if (code>=512)
			tab = &DCT3Dtab0[(code>>5)-16];
		else if (code>=128)
			tab = &DCT3Dtab1[(code>>2)-32];
		else if (code>=8)
			tab = &DCT3Dtab2[(code>>0)-8];
		else 
		{
			if (!m_bQuiet)
			{
				TRACE("invalid Huffman code in getblock()\n");
			}
			fault = 1;
			return;
		}
		
		m_Bits.flushbits(tab->len);
		
		run = (tab->val >> 4) & 255;
		level = tab->val & 15;
		last = (tab->val >> 12) & 1;
		
		if (trace) {
			TRACE(" (");
			printbits(code,12,tab->len);
		}
		
		if (tab->val==ESCAPE) { /* escape */
			if (trace) {
				TRACE(" ");
				printbits(m_Bits.showbits(1),1,1);
			}
			last = m_Bits.getbits1();
			if (trace) {
				TRACE(" ");
				printbits(m_Bits.showbits(6),6,6);
			}
			i += run = m_Bits.getbits(6);
			if (trace) {
				TRACE(" ");
				printbits(m_Bits.showbits(8),8,8);
			}
			level = m_Bits.getbits(8);
			
			if ((sign = (level>=128)))
				val = 256 - level;
			else 
				val = level;
		}
		else {
			i+= run;
			val = level;
			sign = m_Bits.getbits1();//(1);
			if (trace)
				TRACE("%d",sign);
		}
		
		if (i >= 64)
		{
			if (!m_bQuiet)
			{
				TRACE("DCT coeff index (i) out of bounds\n");
			}
			fault = 1;
			return;
		}
		
		if (trace)
			TRACE("): %d/%d\n",run,sign ? -val : val);
		
		
		j = zig_zag_scan[i];
		qval = &bp[j];
		if (comp >= 6)
			QP = mmax (1, mmin( 31, ( bquant_tab[bquant] * quant ) >> 2 ));
		else 
			QP = quant;
		
		/* TMN3 dequantization */
		if ((QP % 2) == 1)
			*qval = ( sign ? -(QP * (2* val+1))  : QP * (2* val+1) );
		else
			*qval = ( sign ? -(QP * (2* val+1)-1): QP * (2* val+1)-1 );
		
		if (last) { /* That's it */
			if (trace)
				TRACE("last\n");
			return;
		}
	}
}



/*********************************************************************
*
*        Name:        get_sac_block
*
*	Description:	Decodes blocks of Arithmetic Encoded DCT Coeffs.
*        and performs Run Length Decoding and Coefficient        
*        Dequantisation.
*
*	Input:        Picture block type and number.
*
*	Returns:	Nothing.
*
*	Side Effects:	
*
*	Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
*
*********************************************************************/


void CH263Decoder::get_sac_block(int comp, int ptype)
{
	int position=0;
	int TCOEF_index, symbol_word;
	int last=0, QP, i, j;
	short *qval, *bp;
	RunCoef DCTcoef;
	
	bp = block[comp];
	
	i = (ptype==0);
	
	while (!last) {	/* while there are DCT coefficients remaining */
		position++;	/* coefficient counter relates to Coeff. model */
		TCOEF_index = DecodeTCoef(position, !ptype);
		
		if (TCOEF_index == ESCAPE_INDEX) {        /* ESCAPE code encountered */
			DCTcoef = Decode_Escape_Char(!ptype, &last);
			if (trace)
				TRACE("ESC: ");
		}
		else {
			symbol_word = tcoeftab[TCOEF_index];
			
			DCTcoef = vlc_word_decode(symbol_word,&last);
		}
		
		if (trace) {
			TRACE("val: %d, run: %d, sign: %d, last: %d\n", 
				DCTcoef.val, DCTcoef.run, DCTcoef.sign, last);
		}
		
		i += DCTcoef.run;
		
		j = zig_zag_scan[i];
		
		qval = &bp[j];
		
		i++;
		
		if (comp >= 6)
			QP = mmax (1, mmin( 31, ( bquant_tab[bquant] * quant ) >> 2 ));
		else 
			QP = quant;
		
		if ((QP % 2) == 1)
			*qval = ( (DCTcoef.sign) ? -(QP * (2* (DCTcoef.val)+1))  : 
        QP * (2* (DCTcoef.val)+1) );
		else
			*qval = ( (DCTcoef.sign) ? -(QP * (2* (DCTcoef.val)+1)-1): 
        QP * (2* (DCTcoef.val)+1)-1 );
        
	}	
	return;
}

/*********************************************************************
*
*        Name:        vlc_word_decode
*
*	Description:	Fills Decoder FIFO after a fixed word length
*        string has been detected.
*
*	Input:        Symbol to be decoded, last data flag.
*
*	Returns:	Decoded Symbol via the structure DCTcoeff.
*
*	Side Effects:	Updates last flag.
*
*	Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
*
*********************************************************************/

RunCoef CH263Decoder::vlc_word_decode(int symbol_word, int *last)
{
	int sign_index;
	RunCoef DCTcoef;
	
	*last = (symbol_word >> 12) & 01;
	
	DCTcoef.run = (symbol_word >> 4) & 255; 
	
	DCTcoef.val = (symbol_word) & 15;
	
	sign_index = decode_a_symbol(cumf_SIGN);	
	
	DCTcoef.sign = signtab[sign_index];
	
	return (DCTcoef);
} 

/*********************************************************************
*
*        Name:        Decode_Escape_Char
*
*	Description:	Decodes all components for a Symbol when an 
*        ESCAPE character has been detected.
*
*	Input:        Picture Type and last data flag.
*
*	Returns:	Decoded Symbol via the structure DCTcoeff.
*
*	Side Effects:	Modifies last data flag.
*
*	Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
*
*********************************************************************/

RunCoef CH263Decoder::Decode_Escape_Char(int intra, int *last)
{
	int last_index, run, run_index, level, level_index;
	RunCoef DCTcoef;
	
	if (intra) {
		last_index = decode_a_symbol(cumf_LAST_intra);
		*last = last_intratab[last_index];
	}
	else {
		last_index = decode_a_symbol(cumf_LAST);
		*last = lasttab[last_index];
	}
	
	if (intra) 
		run_index = decode_a_symbol(cumf_RUN_intra);
	else
		run_index = decode_a_symbol(cumf_RUN);
	
	run = runtab[run_index];
	
	/*$if (mrun) run|=64;$*/
	
	DCTcoef.run = run;
	
	if (intra)
		level_index = decode_a_symbol(cumf_LEVEL_intra);
	else
		level_index = decode_a_symbol(cumf_LEVEL);
	
	if (trace)
		TRACE("level_idx: %d ",level_index);
	
	level = leveltab[level_index];
	
	if (level >128) 
		level -=256;
	
	if (level < 0) {
		DCTcoef.sign = 1;
		DCTcoef.val = abs(level);
	}
	
	else {	
		DCTcoef.sign = 0;
		DCTcoef.val = level;
	}
	
	return (DCTcoef);
	
}
/*********************************************************************
*
*        Name:        DecodeTCoef
*
*	Description:	Decodes a.c DCT Coefficients using the        
*        relevant arithmetic decoding model.
*
*	Input:        DCT Coeff count and Picture Type.	
*
*	Returns:	Index to LUT
*
*	Side Effects:	None
*
*	Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
*
*********************************************************************/

int CH263Decoder::DecodeTCoef(int position, int intra)
{
	int index;
	
	switch (position) {
	case 1:
		{
			if (intra) 
				index = decode_a_symbol(cumf_TCOEF1_intra);
			else 
				index = decode_a_symbol(cumf_TCOEF1); 
			break; 
		}
	case 2:
		{
			if (intra) 
				index = decode_a_symbol(cumf_TCOEF2_intra);
			else
				index = decode_a_symbol(cumf_TCOEF2);
			break; 
		}
	case 3:
		{
			if (intra) 
				index = decode_a_symbol(cumf_TCOEF3_intra);
			else
				index = decode_a_symbol(cumf_TCOEF3);
			break; 
		}
	default: 
		{
			if (intra) 
				index = decode_a_symbol(cumf_TCOEFr_intra);
			else
				index = decode_a_symbol(cumf_TCOEFr);
			break; 
		}
	}
	
	return (index);
}
