#ifndef __H263PDEC_H__
#define __H263PDEC_H__
/*
 * H.263+ Decoder
 *
 * (C) 1996-2000 DResearch GmbH
 *
 * $Header: /DV/DVProcess/DResearch/h263pdec.h 1     15.02.01 11:59 Uwe $
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "codec.h"
	
/*
 * compiling as DLL
 */
#ifdef __DLL
#ifdef __H263P_EXPORT
#define H263P_EXPORT	__declspec(dllexport)
#else
#define H263P_EXPORT	__declspec(dllimport)
#endif
#endif

#ifndef H263P_EXPORT
#define H263P_EXPORT
#endif

/* 
 * PSUPP decoder callbacks
 */
typedef void (*PSUPP_NEW_FRAME_CB)(void *context);
typedef void (*PSUPP_DECODER_CB)(BYTE psupp, void *context);

/*
 * Purpose:
 *		this is the main H.263+ parser
 *
 * Parameters:
 *		s:				decoder status
 *		curr_frame:		after call this pointer holds the parsed frame
 *		last_frame:		pointer to the previous frame, may be NULL
 *
 * Returns:
 *		H26XD_ERR_EOS:			an End-Of-Stream was seen
 *		H26XD_ERR_UNSUPPORTED:	the stream resolution or ANNEX option is unsupported
 *		H26XD_ERR_EOF:			an EOF has occured during read, the may may be damaged
 *		H26XD_ERR_NO_PSC:		no Picture start code was found, nothing was parsed
 *		H26XD_ERR_BAD_FRAME:	the curr_frame structure was not correctly initialised
 *		res >= 0				the frame was parsed upto the next startcode, res bits 
 *								are still in the bit buffer, 
 *
 * Remarks:
 *		curr_frame->has_error	hold an error bitmask that declared which errors occured
 *								during parsing
 */
H263P_EXPORT int H263P_GetFrame( DEC_STATUS *s, FRAME **ret_frame, FRAME *last_frame );

/*
 * Purpose:
 *		initialise the H.263+ decoder
 *
 * Parameters:
 *		p:				frame pool to be used
 *		GetByte:		the GetByte function
 *		pContext:		context parameter for the GetByte function
 *
 * Returns:
 *		decoder status on success, NULL else
 */
H263P_EXPORT DEC_STATUS *H263P_InitDecoder(FRAME_POOL *p, GETBYTE_CB GetByte, void *pContext);

/*
 * Purpose:
 *		Sets the layer callback function
 *
 * Parameters:
 *		s:				decoder status
 *		layer_cb:		the layer callback function
 *		context:		context parameter
 *
 * Remarks:
 *		The layer callback is called, whenever a frame of a different layer is decoded
 */
H263P_EXPORT void H263P_SetCallback(DEC_STATUS *s, LAYER_CB layer_cb, void *context);

/*
 * define the error recovery modes, currently only COPY_SLICE mode is available
 */
#define ERROR_RECOVERY_COPY_SLICE	1

/*
 * Purpose:
 *		sets the error recovery mechanism
 *
 * Parameters:
 *		s:			decoder status
 *		mode:		error recovery mode
 *
 * Returns:
 *		old mode
 */
H263P_EXPORT int H263P_SetErrorRecoveryMode(DEC_STATUS *s, int mode);

/*
 * Purpose:
 *		terminates the H.263+ decoder
 *
 * Parameters:
 *		s:		decoder status
 *
 * Returns:
 *		TRUE on success
 */
H263P_EXPORT BOOL H263P_TermDecoder(DEC_STATUS *s);

/*
 * Purpose:
 *		drops the lookahead
 *
 * Parameters:
 *		s:		decoder status
 *
 * Returns:
 *		how many bytes were destroyed
 */
H263P_EXPORT int H263P_DropBits(DEC_STATUS *s);

/*
 * Purpose:
 *		returns the error mask of a frame
 *
 * Parameters:
 *		f:		frame
 *
 * Returns:
 *		bitmask of PARSER_ERR_*
 */
H263P_EXPORT int H263P_GetFrameError(FRAME *f);

/*
 * Purpose:
 *		returns the frame type of a frame
 *
 * Parameters:
 *		f:		frame
 *
 * Returns:
 *		one of FRAME_TYPE_* or 0 if unknown
 */
H263P_EXPORT int H263P_GetFrameType(FRAME *f);

#ifdef MMX_ENABLED
/*
 * Purpose:
 *		enabled/disable MMX-functions
 *
 * Parameters:
 *		mmx_enabled:		TRUE iff MMX-function can be enabled
 */
H263P_EXPORT void H263P_MMX_Enable(BOOL mmx_enabled);
#endif

/*
 * Purpose:
 *		allows to change the default PSUPP decoder
 *
 * Parameters:
 *		s:					decoder status
 *		psupp_new_frame:	called, whenever a new frame starts
 *		psupp_decoder:		called, whenever a new byte must be send to the decoder
 *		context:			context parameter for both functions
 *
 * Remarks:
 *		call with (s, NULL, NULL, NULL) restores the default (H.263v2) PSUPP decoder
 */
H263P_EXPORT void H263P_SetPSUPPDecoder(
	DEC_STATUS *s, PSUPP_NEW_FRAME_CB psupp_new_frame, PSUPP_DECODER_CB psupp_decoder, void *context);

/*
 * Purpose:
 *		visualize motion vectors and/or block types for visual stream debugging 
 *
 * Parameters:
 *		pFrame:		frame to draw on
 *		dFrame:		frame that holds the data
 *		ip_debug:	colorize pFrame blocks according to block type
 *		draw_mv:	draw little "Motion Vectors" lines on a frame
 */
H263P_EXPORT void H263P_FrameDrawMV( FRAME *pFrame, FRAME *dFrame, BOOL draw_mv, BOOL ip_debug );

/*
 * Purpose:
 *		returns the current decoder version and build
 *
 * Parameters:
 *		version:		pointer to a version_t that will be filled
 */
H263P_EXPORT void H263P_GetVersionInfo(version_t *version);

#ifdef __cplusplus
}
#endif

#endif
