// idctref.cpp, inverse DCT, double precision, for tmndecode (H.263 decoder)

#include "stdafx.h"
#include "H263Decoder.h"

/*  Perform IEEE 1180 reference (64-bit floating point, separable 8x1
*  direct matrix multiply) Inverse Discrete Cosine Transform
*/


/* Here we use math.h to generate constants.  Compiler results may
vary a little */

#include <math.h>

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif


/* initialize DCT coefficient matrix */

void CH263Decoder::init_idctref()
{
	int freq, time;
	double scale;
	
	for (freq=0; freq < 8; freq++)
	{
		scale = (freq == 0) ? sqrt(0.125) : 0.5;
		for (time=0; time<8; time++)
			c[freq][time] = scale*cos((PI/8.0)*freq*(time + 0.5));
	}
}

/* perform IDCT matrix multiply for 8x8 coefficient block */

void CH263Decoder::idctref(short *block)
{
	int i, j, k, v;
	double partial_product;
	double tmp[64];
	
	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
		{
			partial_product = 0.0;
			
			for (k=0; k<8; k++)
				partial_product+= c[k][j]*block[8*i+k];
			
			tmp[8*i+j] = partial_product;
		}
		
		/* Transpose operation is integrated into address mapping by switching 
		loop order of i and j */
		
		for (j=0; j<8; j++)
			for (i=0; i<8; i++)
			{
				partial_product = 0.0;
				
				for (k=0; k<8; k++)
					partial_product+= c[k][i]*tmp[8*k+j];
				
				v = (int)floor(partial_product+0.5);
				block[8*i+j] = (v<-256) ? -256 : ((v>255) ? 255 : v);
			}
}
