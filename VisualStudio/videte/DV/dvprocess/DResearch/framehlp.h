#ifndef __FRAMEHLP_H__
#define __FRAMEHLP_H__
/*
 * H.263+ Codec NT
 *
 * (C) 2000 DResearch GmbH
 *
 * frame helper function
 *
 * $Header: /DV/DVProcess/DResearch/framehlp.h 1     15.02.01 11:59 Uwe $
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "codec.h"

/*
 * supported frame types, must be != 0
 */
#define FRAME_MODE_H261		1			/* H.261 */
#define FRAME_MODE_H262		2			/* MPEG-II */
#define FRAME_MODE_H263		3			/* H.263+ */
#define FRAME_MODE_MPG4		4			/* MPEG-4 */
#define FRAME_MODE_RAW		5			/* RAW frame */
#define FRAME_MODE_JPG		8			/* JPEG */

#define	FRAME_MODE_MASK		0xFF

/*
 * additional flags
 */
#define FRAME_MODE_420		0x00000000		/* default mode: planar YUV 4:2:0 */
#define FRAME_MODE_422		0x80000000		/* planar YUV 4:2:2 mode */


/* the PICT struct describes a frame */
typedef struct _PICT {
	BYTE			*Y;				/* pointer in the visible Y-Plane */
	BYTE			*U;				/* pointer in the visible U-Plane */
	BYTE			*V;				/* pointer in the visible V-Plane */
	int				line_skip;		/* line skip (aka pitch) */
	int				w;				/* selected window width in pixels */
	int				h;				/* selected window height in pixels */
	int				stream;			/* stream number or NO_CPM_STREAM */
	unsigned		nr;				/* frame number */
	unsigned		tr;				/* time ref */
	unsigned		mean_quant;		/* mean quantizer in 1% units */
	void			*user_data;		/* free for the user */
} PICT;

/*
 * Purpose:
 *		create a frame pool
 *
 * Parameters:
 *		max_frames:		maximal number of allocated frames
 *
 * Returns:
 *		FRAME_POOL on success, NULL on low memory
 */
FRAME_POOL *Hlp_CreatePool(int max_frames);

/* 
 * Purpose:
 *		terminate a frame pool handler
 *
 * Parameters:
 *		p:		FRAME_POOL
 *
 * Remarks:
 *		this will destroy the entire frame pool
 */
void Hlp_TermPool(FRAME_POOL *p);

/*
 * Purpose:
 *		converts a FRAME into a PICT
 *
 * Parameters:
 *		f:		FRAME
 *
 * Returns:
 *		PICT
 */
PICT *Hlp_Frame2Pict(FRAME *f);

/*
 * Purpose:
 *		converts a PICT into a FRAME
 *
 * Parameters:
 *		p:		PICT
 *
 * Returns:
 *		FRAME
 */
FRAME *Hlp_Pict2Frame(PICT *p);

/*
 * Purpose:
 *		allocate a free picture from a pool
 *
 * Parameters:
 *		p:		FRAME_POOL
 *		w, h:	width and height of the picture to be allocated
 *		mode:	FRAME_MODE_*
 *
 * Returns:
 *		a new PICT structure if memory is avilable
 */
PICT *Hlp_GetPict(FRAME_POOL *p, int w, int h, int mode);

/*
 * Purpose:
 *		release a unused pict into a pool
 *
 * Parameters:
 *		p:			FRAME_POOL
 *		pict:		PICT
 */
void Hlp_PutPict(FRAME_POOL *p, PICT *pict);

/*
 * Purpose:
 *		increase a frames reference count
 *
 * Parameters:
 *		pict:		PICT
 */
void Hlp_PictAddRef(PICT *pict);

#ifdef __TCS__
/*
 * Purpose:
 *		invalidate data cache for a frame (CPU must reload the data of the frame)
 *
 *	Parameters:
 *		f:	frame to invalidate
 */
void Hlp_InvalidatePict(PICT *pict);

/*
 * Purpose:
 *		cause a copy back of the frame data
 *
 *	Parameters:
 *		f:	frame to copy back
 */
void Hlp_CopybackPict(PICT *pict);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEHLP_H__ */
