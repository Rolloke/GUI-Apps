#ifndef __FRAME_TEXT_H__
#define __FRAME_TEXT_H__
/*
 * frame text functions
 *
 * (C) 2000 DResearch GmbH
 *
 * frame_text.h
 *
 * $Header: /DV/DVProcess/DResearch/frame_text.h 1     15.02.01 11:59 Uwe $
 */
#include "codec.h"

typedef enum FILL_COLOR {
	C_RED     =	0,
	C_YELLOW  = 1,
	C_GREEN   = 2,
	C_CYAN    = 3,
	C_BLUE    = 4,
	C_MAGENTA = 5,
	C_BLACK   = 6,
	C_WHITE   = 7,
	C_GRAY    = 8
} FILL_COLOR;

/*
 * Purpose:
 *		set foreground and background color
 *
 * Parameters:
 *		fg:		foreground color
 *		bg:		background color
 *
 * Remarks:
 *		use -1 for transparent background
 */
void FramePrintSetColor( int fg, int bg );

/*
 * Purpose: 
 *		print string using 8x8 pixel chars 
 *
 * Parameter: 
 *		frame:			frame to print on
 *		x, y:			pixel position in frame
 *		lpszFormat,...: printf style parameter
 *
 * Remarks:
 *		used with variable no. of arguments
 */
void __cdecl FramePrint( FRAME *curr_frame, int x, int y, char *lpszFormat, ... );

/*
 * Purpose:
 *		fill a 8 x8 pixel block with color
 *
 * Parameters:
 *		frame:	frame to draw on
 *		x, y:	pixel position in frame
 *		color:	color [0..255]
 */
void FrameFillBlock( FRAME *curr_frame, int x, int y, int color );

/*
 *	Purpose:
 *		fill block x,y with color 
 *
 *	Parameters:
 *		curr_frame:	frame of destination block
 *		x:			x index of block 
 *		y:			y index of block
 *		color:		fill color
 */
void FrameCFillBlock( FRAME *curr_frame, int x, int y, FILL_COLOR color );

/*
 * Purpose:
 *		fill a pixel with color
 *
 * Parameters:
 *		frame:	frame to draw on
 *		x, y:	pixel position in frame
 *		color:	color [0..255]
 *
 * Remarks:
 *		clip to the visible area
 */
void FrameDrawPoint( FRAME *frame, int x, int y, int color );

/*
 *	Purpose:
 *		draw colored point into frame (2x2 Y Points)
 *
 *	Parameters:
 *		frame:	frame to draw point
 *		x:		x index of point (in pixel)
 *		y:		y index of point (in pixel)
 *		cy: 	fill color Y
 *		cu: 	fill color U
 *		cv: 	fill color V
 */
void FrameDrawCPoint( FRAME *frame, int x, int y, int cy, int cu, int cv );

/*
 * Purpose:
 *		xor a pixel with 0xFF
 *
 * Parameters:
 *		frame:	frame to draw on
 *		x, y:	pixel position in frame
 *
 * Remarks:
 *		clip to the visible area
 */
void FrameDrawPointXor( FRAME *frame, int x, int y );

/*
 * Purpose:
 *		draw a line from [x0,y0] to [x1,y1] with color
 *
 * Parameters:
 *		frame:	frame to draw on
 *		x0, y0:	pixel position start of line in frame
 *		x1, y1:	pixel position start of line in frame
 *		color:	color [0..255]
 *
 *	Remarks:
 *		implements Bresenham algorithm
 */
void FrameDrawLine( FRAME *frame, int x0, int y0, int x1, int y1, int color );

/*
 * Purpose:
 *		draw a line from [x0,y0] to [x1,y1] by Xor'ing with 0xFF
 *
 * Parameters:
 *		frame:	frame to draw on
 *		x0, y0:	pixel position start of line in frame
 *		x1, y1:	pixel position start of line in frame
 *		dotted: true, iff dotted line	
 *
 *	Remarks:
 *		implements Bresenham algorithm
 */
void FrameDrawLineXor( FRAME *frame, int x0, int y0, int x1, int y1, int dotted );

/*
 *	Purpose:
 *		draw colored line with linewidth=2 into frame
 *
 *	Parameters:
 *		frame:	frame to draw point
 *		x0:		x index of line startpoint (in pixel)
 *		y0:		y index of line startpoint (in pixel)
 *		x1:		x index of line endpoint (in pixel)
 *		y1:		y index of line endpoint (in pixel)
 *		cy:	    fill color Y
 *		cu:	    fill color U
 *		cv:	    fill color V
 */
void FrameDrawCLine( FRAME *frame, int x0, int y0, int x1, int y1, int cy, int cu, int cv );

/*
 * Purpose:
 *		Draw one Block-Frame around a Block or MB
 *
 * Parameters:
 *		pFrame:		frame to draw into
 *		x, y:		upper left coordinates
 *		type:		either GRID_MB for Macroblock or GRID_BL for Block
 */
void FrameDrawBlock( FRAME *pFrame, int x, int y, int type );

/*
 * Purpose:
 *		Draw a Grid into a frame
 *
 * Parameters:
 *		pFrame:		frame to draw into
 *		dFrame:		frame to take the GOB data
 *		type:		either GRID_MB for Macroblock or GRID_BL for Block
 */
void FrameDrawGrid( FRAME *pFrame, FRAME *dFrame, int type );

#endif /* __FRAME_TEXT_H__ */

