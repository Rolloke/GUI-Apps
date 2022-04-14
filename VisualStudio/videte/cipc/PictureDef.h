/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: PictureDef.h $
// ARCHIVE:		$Archive: /Project/CIPC/PictureDef.h $
// CHECKIN:		$Date: 6.06.06 9:55 $
// VERSION:		$Revision: 52 $
// LAST CHANGE:	$Modtime: 6.06.06 9:53 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef PICTURE_DEF_H
#define PICTURE_DEF_H

// 
//  CIPCSavePictureType | type of picture
enum CIPCSavePictureType 
{
	// I-frame with following P-frames
	SPT_GOP_PICTURE = 0,
	// P-frame 
	SPT_DIFF_PICTURE = 1,
	// I-frame or JPEG
	SPT_FULL_PICTURE = 2
};
#define SPT2TCHAR(x) ((x == SPT_FULL_PICTURE)?_T("I"):(x == SPT_GOP_PICTURE)?_T("G"):_T("P"))
/* 
 Resolution | picture resolution depending on compression
algorithm.
*/
enum Resolution
{	// Videonorm:					Pal,		NTSC
	// invalid resolution
	RESOLUTION_NONE	= -1,
	// half field, RESOLUTION_HIGH
	RESOLUTION_2CIF	= 0, //			704x288,	720x240
	// CIF, RESOLUTION_MID
	RESOLUTION_CIF	= 1, //			352x288,	360x240
	// quarter cif, RESOLUTION_LOW
	RESOLUTION_QCIF	= 2, //			176x144,	180x120
	// full frame, very high
	RESOLUTION_4CIF	= 3, //			704x576,	720x480
	// VGA					
	RESOLUTION_VGA	= 4, //					640x480
	// QVGA, SIF (Standard Interchange Format)				
	RESOLUTION_QVGA  = 5, //				320x240
	// QSIF
	RESOLUTION_QSIF	= 6, //					160x120
	// XVGA
	RESOLUTION_XVGA	= 7, //					1024x768
	// 4VGA
	RESOLUTION_4VGA	= 8, //					1280x960
	// HDTV
	RESOLUTION_HDTV	= 9, //					1920x1080
	// not a resolution value
	RESOLUTION_LAST = 10,
};

AFX_EXT_CLASS const _TCHAR *NameOfEnum(Resolution r);
AFX_EXT_CLASS CString		ShortNameOfEnum(Resolution r);
AFX_EXT_CLASS Resolution	EnumOfName(LPCTSTR psz);
AFX_EXT_CLASS SIZE          SizeOfEnum(Resolution r, BOOL bNTSC=FALSE);
AFX_EXT_CLASS Resolution    EnumOfSize(SIZE sz);

// CAVEAT values _X = X-1 used in NameOfEnum
/*
 Compression | picture compression rate
*/
enum Compression
{
	// invalid compression
	COMPRESSION_NONE	= -1,
	// compression 1 H.263 2048KBit/s 25fps
	COMPRESSION_1		= 0,	
	// compression 2 H.263 2048KBit/s	12fps
	COMPRESSION_2		= 1,	
	// compression 3 H.263 1536KBit/s	25fps 
	COMPRESSION_3		= 2,	
	// compression 4 H.263 1536KBit/s	12fps
	COMPRESSION_4		= 3,	
	// compression 5 H.263 1024KBit/s	25fps
	COMPRESSION_5		= 4,	 
	// compression 6 H.263 1024KBit/s	12fps
	COMPRESSION_6		= 5,	 
	// compression 7 H.263 512KBit/s	25fps
	COMPRESSION_7		= 6,	 
	// compression 8 H.263 512KBit/s	12fps
	COMPRESSION_8		= 7,	 
	// compression 9 H.263 512KBit/s	6fps
	COMPRESSION_9		= 8,	 
	// compression 10 H.263 512KBit/s	3fps
	COMPRESSION_10		= 9,	 
	// compression 11 H.263 512KBit/s	1fps
	COMPRESSION_11		= 10,	
	// compression 12 H.263 256KBit/s	25fps
	COMPRESSION_12		= 11,	
	// compression 13 H.263 256KBit/s	12fps
	COMPRESSION_13		= 12,	
	// compression 14 H.263 256KBit/s	6fps
	COMPRESSION_14		= 13,	 
	// compression 15 H.263 256Bit/s	3fps
	COMPRESSION_15		= 14,	 
	// compression 16 H.263 256Bit/s	1fps
	COMPRESSION_16		= 15,	 
	// compression 17 H.263 128KBit/s	25fps
	COMPRESSION_17		= 16,	
	// compression 18 H.263 128KBit/s	12fps
	COMPRESSION_18		= 17,	 
	// compression 19 H.263 128KBit/s	6fps 
	COMPRESSION_19		= 18,	 
	// compression 20 H.263 128KBit/s	3fps
	COMPRESSION_20		= 19,	
	// compression 21 H.263 128KBit/s	1fps
	COMPRESSION_21		= 20,	
	// compression 22 H.263 64KBit/s	25fps
	COMPRESSION_22		= 21,	
	// compression 23 H.263 64Bit/s	12fps
	COMPRESSION_23		= 22,	
	// compression 24 H.263 64Bit/s	6fps
	COMPRESSION_24		= 23,	
	// compression 25 H.263 64Bit/s	3fps
	COMPRESSION_25		= 24,	 
	// compression 26 H.263 64Bit/s	1fps
	COMPRESSION_26		= 25	 
};
AFX_EXT_CLASS const _TCHAR *NameOfEnum(Compression c);
/*
 CompressionType | the compression algorithm
*/
enum CompressionType
{
	// invalid CompressionType
	COMPRESSION_UNKNOWN		= -1,
	// H.263
	COMPRESSION_H263		= 4,
	// Color JPEG
	COMPRESSION_JPEG		= 5,
	// RGB 24 Bit DIB
	COMPRESSION_RGB_24		= 6,
	// YUV 422 DIB
	COMPRESSION_YUV_422		= 7,
	// Presence propriatary format
	COMPRESSION_PRESENCE	= 8,
	// MPEG4
	COMPRESSION_MPEG4		= 9,
	// YUV 420 DIB
	COMPRESSION_YUV_420		= 10,
};

AFX_EXT_CLASS const _TCHAR *NameOfEnum(CompressionType ct);
// just to read old 3. db files
// Absolute Bildtypen als Wert
#define	H263_HIGH		(4)	// idip! CoCo
#define	H263_LOW		(5)
#define	COLORJPEG_HIGH	(6)	// idip! Mico
#define	COLORJPEG_MID	(7)
#define	COLORJPEG_LOW	(8)
#define	PRESENCE_HIGH	(10)// pt-Box (Presence Technology)
#define	PRESENCE_LOW	(11)


#endif // PICTURE_DEF_H
