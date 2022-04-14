// YUVToRGB.cpp: implementation of the CYUVToRGB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "h263play.h"
#include "YUVToRGB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CYUVToRGB::CYUVToRGB()
{
	long int crv,cbu,cgu,cgv;
	int i;   
	
	crv = 104597; cbu = 132201;  /* fra matrise i global.h */
	cgu = 25675;  cgv = 53279;
	
	for (i = 0; i < 256; i++) 
	{
		m_crv_tab[i] = (i-128) * crv;
		m_cbu_tab[i] = (i-128) * cbu;
		m_cgu_tab[i] = (i-128) * cgu;
		m_cgv_tab[i] = (i-128) * cgv;
		m_tab_76309[i] = 76309*(i-16);
	}

	/* clip table */
	if (!(m_clp=(unsigned char *)malloc(1024)))
		TRACE("malloc failed\n");
	
	m_clp += 384;
	
	for (i=-384; i<640; i++)
		m_clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
}
//////////////////////////////////////////////////////////////////////
CYUVToRGB::~CYUVToRGB()
{
	if (m_clp)
	{
		m_clp -= 384;
		free(m_clp);
	}
}
//////////////////////////////////////////////////////////////////////
void CYUVToRGB::ConvertYUVtoRGB(BYTE *srcY, BYTE *srcU, BYTE *srcV, 
								 BYTE *dstRGB,  int width,  int height)
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
	
	d1 = dstRGB;
	d1 += width*height*3 - width*3;
	d2 = d1 - width*3;
	
	py1 = srcY; 
	pu = srcU; 
	pv = srcV;
	py2 = py1 + width;
	
	id1 = (unsigned int *)d1;
	id2 = (unsigned int *)d2;
	
	for (j = 0; j < height; j += 2) { 
		/* line j + 0 */
		for (i = 0; i < width; i += 4) {
			u = *pu++;
			v = *pv++;
			c11 = m_crv_tab[v];
			c21 = m_cgu_tab[u];
			c31 = m_cgv_tab[v];
			c41 = m_cbu_tab[u];
			u = *pu++;
			v = *pv++;
			c12 = m_crv_tab[v];
			c22 = m_cgu_tab[u];
			c32 = m_cgv_tab[v];
			c42 = m_cbu_tab[u];
			
			y11 = m_tab_76309[*py1++]; /* (255/219)*65536 */
			y12 = m_tab_76309[*py1++];
			y13 = m_tab_76309[*py1++]; /* (255/219)*65536 */
			y14 = m_tab_76309[*py1++];
			
			y21 = m_tab_76309[*py2++];
			y22 = m_tab_76309[*py2++];
			y23 = m_tab_76309[*py2++];
			y24 = m_tab_76309[*py2++];
			
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
