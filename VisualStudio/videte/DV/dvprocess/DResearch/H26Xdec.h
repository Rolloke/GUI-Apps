#ifndef __H26X_DEC_H__
#define __H26X_DEC_H__
/*
 * H.263+, H.261 Decoders
 *
 * (C) 1996-2000 DResearch GmbH
 *
 * h26x_dec.h
 *
 * $Header: /DV/DVProcess/DResearch/H26Xdec.h 1     15.02.01 11:59 Uwe $
 */

#include "codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * compiling as DLL
 */
#ifdef __DLL
#ifdef __H26X_EXPORT
#define H26X_EXPORT	__declspec(dllexport)
#else
#define H26X_EXPORT	__declspec(dllimport)
#endif
#endif

#ifndef H26X_EXPORT
#define H26X_EXPORT
#endif	

/*
 * define a generic handle type
 */
typedef void *DEC_HANDLE;

/* 
 * definition of the possible errorcodes
 *
 * Note, that H26XD_ERR_HAVE_LAYER is not an error but an indication that a 
 * valid GOB/Slice header was parsed
 */
#define H26XD_ERR_OK			(0)
#define H26XD_ERR_NO_PSC		(-1)
#define H26XD_ERR_EOS			(-2)
#define H26XD_ERR_UNSUPPORTED	(-3)
#define H26XD_ERR_BAD_FRAME		(-4)
#define H26XD_ERR_EOF			(-5)
#define H26XD_ERR_PTYPE			(-6)
#define H26XD_ERR_HAVE_LAYER	(-7)

/* parser errorcode refinement */
#define PARSER_ERR_OK			(0x00000000)
#define PARSER_ERR_NO_PSC		(0x00000001)
#define PARSER_ERR_HDR			(0x00000002)
#define PARSER_ERR_IMB			(0x00000004)
#define PARSER_ERR_PMB			(0x00000008)
#define PARSER_ERR_GROUP		(0x00000010)
#define PARSER_ERR_GQUANT		(0x00000020)
#define PARSER_ERR_INTERNAL		(0x00000040)
#define PARSER_ERR_EOF			(0x00000080)
#define PARSER_ERR_MBA			(0x00000100)
#define PARSER_ERR_MBNUM		(0x00000200)
#define PARSER_ERR_MTYPE		(0x00000400)
#define PARSER_ERR_MQUANT		(0x00000800)
#define PARSER_ERR_MVD			(0x00001000)
#define PARSER_ERR_CBP			(0x00002000)
#define PARSER_ERR_IB			(0x00004000)
#define PARSER_ERR_PB			(0x00008000)
#define PARSER_ERR_MVD_RANGE	(0x00010000)
#define PARSER_ERR_MCBPC		(0x00020000)
#define PARSER_ERR_4V			(0x00040000)
#define PARSER_ERR_MCBPC_INTRA	(0x00080000)
#define PARSER_ERR_CBPY			(0x00100000)
#define PARSER_ERR_SAC_FIFO		(0x00200000)
#define PARSER_ERR_STREAM		(0x00400000)
#define PARSER_ERR_SKIP4GOB		(0x00800000)
#define PARSER_ERR_QUANT_RANGE	(0x01000000)
#define PARSER_ERR_ANNEX_COMB	(0x02000000)
#define PARSER_ERR_REF_FRAME	(0x04000000)
#define PARSER_ERR_MBYTE		(0x08000000)
#define PARSER_ERR_SLICE_MODE	(0x01000000)
#define PARSER_ERR_SSBI			(0x02000000)
#define PARSER_ERR_SWI			(0x04000000)
#define PARSER_ERR_SEP			(0x08000000)

typedef int (*GetFrameFunc)( DEC_HANDLE s, FRAME **ret_frame, FRAME *last_frame );
typedef int (*SkipFrameFunc)( DEC_HANDLE s, FRAME **ret_frame, FRAME *last_frame );
typedef BOOL (*TermDecoderFunc)( DEC_HANDLE s );
typedef int (*DropBitsFunc)( DEC_HANDLE s);

/*
 * supported Stream types, currently H.261 and H.263
 */
#define STREAM_UNKNOWN		0
#define STREAM_H261			1
#define STREAM_H263			3
#define STREAM_MJPG			4

/*
 * Purpose:
 *		use this function to determine the stream type and find the next 
 *		startcode. It is possible to force one format, in that case search for
 *		this kind of startcode only
 *
 * Parameters:
 *		GetByte:		function pointer to a BYTE delivery function
 *		pContext:		context parameter for teh GetByte function
 *		fmt:			find startcode of this format
 *
 * Returns:
 *		STREAM_H261		H.261 format detected
 *		STREAM_H263		H.263 format detected
 *		STREAM_MJPG		DResearch Motion JPEG format detected
 *		STREAM_UNKNOWN	no known startcode was found
 */
H26X_EXPORT int StreamType( GETBYTE_CB GetByte, void *pContext, int fmt );

/*
 * this function generates the error report
 */
H26X_EXPORT void err_report(int err_code, char *buf, int maxlen);

#ifdef __cplusplus
}
#endif

#endif
