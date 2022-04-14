// gethdr.cpp, header decoding for tmndecode (H.263 decoder)

#include "stdafx.h"
#include "H263Decoder.h"

/*
* decode headers from one input stream
* until an End of Sequence or picture start code
* is found
*/

int CH263Decoder::getheader()
{
	unsigned int code, gob;
	
	/* look for startcode */
	startcode();
	code = m_Bits.getbits(PSC_LENGTH);
	gob = m_Bits.getbits(5);
	if (gob == SE_CODE) 
		return 0;
	if (gob == 0) {
		getpicturehdr();
		if (syntax_arith_coding)        /* reset decoder after receiving */
			decoder_reset();	        /* fixed length PSC string */
	}
	return gob + 1;
}


/* align to start of next startcode */

void CH263Decoder::startcode()
{
	/* search for new picture start code */
	while (m_Bits.showbits(PSC_LENGTH)!=1l) 
		m_Bits.flushbits(1);
}

/* decode picture header */

void CH263Decoder::getpicturehdr()
{
	int pei, tmp;
	
	prev_temp_ref = temp_ref;
	temp_ref = m_Bits.getbits(8);
	trd = temp_ref - prev_temp_ref;
	
	if (trd < 0)
		trd += 256;
	
	tmp = m_Bits.getbits1();//(1); /* always "1" */
	if (!tmp)
		if (!m_bQuiet)
		{
			TRACE("warning: spare in picture header should be \"1\"\n");
		}
		tmp = m_Bits.getbits1();//(1); /* always "0" */
		if (tmp)
			if (!m_bQuiet)
			{
				TRACE("warning: H.261 distinction bit should be \"0\"\n");
			}
			tmp = m_Bits.getbits1();//(1); /* split_screen_indicator */
			if (tmp) {
				if (!m_bQuiet)
				{
					TRACE("error: split-screen not supported in this version\n");
				}
				exit (-1);
			}
			tmp = m_Bits.getbits1();//(1); /* document_camera_indicator */
			if (tmp)
				if (!m_bQuiet)
				{
					TRACE("warning: document camera indicator not supported in this version\n");
				}
				
				tmp = m_Bits.getbits1();//(1); /* freeze_picture_release */
				if (tmp)
					if (!m_bQuiet)
					{
						TRACE("warning: frozen picture not supported in this version\n");
					}
					
					source_format = m_Bits.getbits(3);
					pict_type = m_Bits.getbits1();//(1);
					mv_outside_frame = m_Bits.getbits1();//(1);
					long_vectors = (mv_outside_frame ? 1 : 0);
					syntax_arith_coding = m_Bits.getbits1();//(1);
					adv_pred_mode = m_Bits.getbits1();//(1);
					mv_outside_frame = (adv_pred_mode ? 1 : mv_outside_frame);
					pb_frame = m_Bits.getbits1();//(1);
					quant = m_Bits.getbits(5);
					tmp = m_Bits.getbits1();//(1);
					if (tmp) {
						if (!m_bQuiet)
						{
							TRACE("error: CPM not supported in this version\n");
						}
						exit(-1);
					}
					
					if (pb_frame) {
						trb = m_Bits.getbits(3);
						bquant = m_Bits.getbits(2);
					}
					else {
						trb = 0;
					}
		
					pei = m_Bits.getbits1();//(1);
pspare:
					if (pei) {
						/* extra info for possible future backward compatible additions */
						m_Bits.getbits(8);  /* not used */
						pei = m_Bits.getbits1();//(1);
						if (pei) goto pspare; /* keep on reading pspare until pei=0 */
					}
}


