#ifndef __FRAMEDIB_H__
#define __FRAMEDIB_H__
/*
 * DResearch Colorspace converter for H.26X
 *
 * (C) 1996, 1997 DResearch GmbH
 *
 * $Header: /DV/DVProcess/DResearch/framedib.h 1     15.02.01 11:59 Uwe $
 */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "drbastyp.h"

#ifdef __cplusplus 
extern "C" {
#endif

#if defined(DJGPP) || defined(__TCS__)
/*
 * GCC dosn't know about BITMAPINFOHEADER, so we define it here
 */

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD  biSize;
	DWORD  biWidth;
	DWORD  biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	DWORD  biXPelsPerMeter;
	DWORD  biYPelsPerMeter;
	DWORD  biClrUsed;
	DWORD  biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagRGBTRIPLE {
	BYTE    rgbtBlue;
	BYTE    rgbtGreen;
	BYTE    rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

#endif

/*
 * define YUV colorspace descriptors
 */
typedef struct {
	DWORD		fourcc;
	void		(*Frame2DX)( BYTE* lpBits, long pitch, 
							 BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, 
							 int field );
	DWORD		value;
} YUV_CONVERT;

/*
 * define RGB colorspace descriptors
 */
typedef struct {
	const char	*rgb_name;
	unsigned	RGBBitCount;
	unsigned	RBitMask;
	unsigned	GBitMask;
	unsigned	BBitMask;

	void		(*Frame2DX)( BYTE* lpBits, long pitch, 
							 BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, 
							 int field );
	DWORD		value;
} RGB_CONVERT;

/* 
 * Constants for supported color spaces
 * Note, that Recommendation ITU-R BT.470-2 and SMPTE 170M both uses 
 * the same coefficients!
 */
#define COLORSPACE_CCIR_601_STRECHED	0
#define COLORSPACE_CCIR_601				1
#define COLORSPACE_ITU_R_BT_709			2
#define COLORSPACE_FCC					3
#define COLORSPACE_ITU_R_BT_470_2		4 
#define COLORSPACE_SMPTE_170M			5
#define COLORSPACE_SMPTE_240M			6

int GetColorSpaceMode(void);
int SetColorSpaceMode( int color_space );

/* Constants for the ColorDepth */
#define COLOR_UNKNOWN		-1
#define COLOR_PALETTE_GREY	0
#define COLOR_PALETTE_256	1
#define COLOR_15BIT			2
#define COLOR_16BIT			3
#define COLOR_24BIT			4
#define COLOR_32BIT			5

int ColorDepth(void);

#ifndef NO_PALETTE
void SetPalette( int min_x, int max_x, int min_y, int max_y );
#endif

#define CSC_FRAME			0
#define CSC_TOP_FIELD		1
#define CSC_BOTTOM_FIELD	2

/*
 * if we have enabled MMX code, we must use function pointers
 * to the colorspace converters 
 */
#ifdef MMX_ENABLED

#define MMX_STATIC static

/*
 * Purpose:
 *		initialise the colorspace converters
 *
 * Parameters:
 *		use_MMX:		use MMX functions if available on cpu
 *		fast_16bit:		use the fast (but lower quanlity) 16bit NON-MMX routines
 */
void InitFrame2DIB(int use_MMX, int fast_16bit);

/* colorspace converters */
extern void (*Frame2DX15   )( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX16   )( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX24   )( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX32   )( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX_YUY2)( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX_UYVY)( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX_YVYU)( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX_YV12)( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
extern void (*Frame2DX_I420)( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );

#else
/*
 * no MMX support, use only the normal colorspace converters
 */
#define MMX_STATIC			static

#define N_Frame2DX15		Frame2DX15
#define N_Frame2DX16		Frame2DX16
#define N_Frame2DX24		Frame2DX24
#define N_Frame2DX32		Frame2DX32
#define N_Frame2DX_YUY2		Frame2DX_YUY2
#define N_Frame2DX_UYVY		Frame2DX_UYVY
#define N_Frame2DX_YVYU		Frame2DX_YVYU
#define N_Frame2DX_YV12		Frame2DX_YV12
#define N_Frame2DX_I420		Frame2DX_I420

/* DirectX Surface converter */
void Frame2DX15   ( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX16   ( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX24   ( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX32   ( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX_YUY2( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX_UYVY( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX_YVYU( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX_YV12( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );
void Frame2DX_I420( BYTE* lpBits, long pitch, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int field );

#endif

/* enumerator functions */
int EnumerateYUV_converter(YUV_CONVERT *yuv_convert, int *yuv_count);
int EnumerateRGB_converter(RGB_CONVERT *rgb_convert, int *rgb_count);

/* colorspace converter for DIB's */
void Frame2DIB8Grey( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB8    ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB8X2  ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB15   ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB16   ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB24   ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );
void Frame2DIB32   ( LPBITMAPINFO lpBMI, BYTE* lpBits, BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive );

BOOL DumpFrameBMP( BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive, const char *name, int nr );
BOOL DumpFramePGM( BYTE *Y, BYTE *U, BYTE *V, int W, int H, int line_inc, int progressive, const char *name, int nr );

#ifdef __cplusplus 
}
#endif

#endif
