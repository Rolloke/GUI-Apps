// yuvrgb24.c, colour space conversion for tmndecode (H.263 decoder)

#include "stdafx.h"
#include "H263Decoder.h"

#include "tables.h"

void CH263Decoder::init_dither_tab()
{
	long int crv,cbu,cgu,cgv;
	int i;   
	
	crv = 104597; cbu = 132201;  /* fra matrise i global.h */
	cgu = 25675;  cgv = 53279;
	
	for (i = 0; i < 256; i++) {
		crv_tab[i] = (i-128) * crv;
		cbu_tab[i] = (i-128) * cbu;
		cgu_tab[i] = (i-128) * cgu;
		cgv_tab[i] = (i-128) * cgv;
		tab_76309[i] = 76309*(i-16);
	}
}




/**********************************************************************
*
*	Name:	         ConvertYUVtoRGB	
*	Description:     Converts YUV image to RGB (packed mode)
*	
*	Input:	         pointer to source luma, Cr, Cb, destination,
*                       image width and height
*	Returns:       
*	Side effects:
*
*	Date: 951208	Author: Karl.Lillevold@nta.no
*
***********************************************************************/



void CH263Decoder::ConvertYUVtoRGB(unsigned char* src0, 
					 unsigned char* src1,
					 unsigned char* src2,
					 unsigned char* dst_ori,
					 int width, int height)
{       
	int y11,y21;
	int y12,y22;
	int y13,y23;
	int y14,y24;
	int u,v; 
	int i,j;
	int c11, c21, c31, c41;
	int c12, c22, c32, c42;
	unsigned int DW;
	unsigned int *id1, *id2;
	unsigned char *py1,*py2,*pu,*pv;
	unsigned char *d1, *d2;
	int foo = (9 * width)>>2;
	
	d1 = dst_ori;
	d1 += width*height*3 - width*3;
	d2 = d1 - width*3;
	
	py1 = src0; pu = src1; pv = src2;
	py2 = py1 + width;
	
	id1 = (unsigned int *)d1;
	id2 = (unsigned int *)d2;
	
	for (j = 0; j < height; j += 2) { 
		/* line j + 0 */
		for (i = 0; i < width; i += 4) {
			u = *pu++;
			v = *pv++;
			c11 = crv_tab[v];
			c21 = cgu_tab[u];
			c31 = cgv_tab[v];
			c41 = cbu_tab[u];
			u = *pu++;
			v = *pv++;
			c12 = crv_tab[v];
			c22 = cgu_tab[u];
			c32 = cgv_tab[v];
			c42 = cbu_tab[u];
			
			y11 = tab_76309[*py1++]; /* (255/219)*65536 */
			y12 = tab_76309[*py1++];
			y13 = tab_76309[*py1++]; /* (255/219)*65536 */
			y14 = tab_76309[*py1++];
			
			y21 = tab_76309[*py2++];
			y22 = tab_76309[*py2++];
			y23 = tab_76309[*py2++];
			y24 = tab_76309[*py2++];
			
			/* RGBR*/
			DW = ((m_clp[(y11 + c41)>>16])) |
				((m_clp[(y11 - c21 - c31)>>16])<<8) |
				((m_clp[(y11 + c11)>>16])<<16) |  
				((m_clp[(y12 + c41)>>16])<<24);
			*id1++ = DW;
			
			/* GBRG*/
			DW = ((m_clp[(y12 - c21 - c31)>>16])) |
				((m_clp[(y12 + c11)>>16])<<8) |  
				((m_clp[(y13 + c42)>>16])<<16) |
				((m_clp[(y13 - c22 - c32)>>16])<<24);
			*id1++ = DW;
			
			/* BRGB*/
			DW = ((m_clp[(y13 + c12)>>16])) |  
				((m_clp[(y14 + c42)>>16])<<8) |
				((m_clp[(y14 - c22 - c32)>>16])<<16) |
				((m_clp[(y14 + c12)>>16])<<24);  
			*id1++ = DW;
			
			/* RGBR*/
			DW = ((m_clp[(y21 + c41)>>16])) |
				((m_clp[(y21 - c21 - c31)>>16])<<8) |
				((m_clp[(y21 + c11)>>16])<<16) |  
				((m_clp[(y22 + c41)>>16])<<24);
			*id2++ = DW;
			
			/* GBRG*/
			DW = ((m_clp[(y22 - c21 - c31)>>16])) |
				((m_clp[(y22 + c11)>>16])<<8) |  
				((m_clp[(y23 + c42)>>16])<<16) |
				((m_clp[(y23 - c22 - c32)>>16])<<24);
			*id2++ = DW;
			
			/* BRGB*/
			DW = ((m_clp[(y23 + c12)>>16])) |  
				((m_clp[(y24 + c42)>>16])<<8) |
				((m_clp[(y24 - c22 - c32)>>16])<<16) |
				((m_clp[(y24 + c12)>>16])<<24);  
			*id2++ = DW;
		}
		id1 -= foo;
		id2 -= foo;
		py1 += width;
		py2 += width;
	}           
}  

