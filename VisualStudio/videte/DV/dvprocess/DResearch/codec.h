#ifndef __CODEC_H__
#define __CODEC_H__
/*
 * H.263+ Codec NT
 *
 * (C) 1997-2000 DResearch GmbH
 *
 * codec datatype definitions
 *
 * $Header: /DV/DVProcess/DResearch/codec.h 1     15.02.01 11:59 Uwe $
 */
#include "drbastyp.h"

/* possible frame types */
#define FRAME_TYPE_I	'i'
#define FRAME_TYPE_P	'p'
#define FRAME_TYPE_PB	'b'
#define FRAME_TYPE_EI	'I'
#define FRAME_TYPE_EP	'P'
#define FRAME_TYPE_B	'B'

/* general definitions */
typedef struct _ENC_STATUS ENC_STATUS;
typedef struct _DEC_STATUS DEC_STATUS;
typedef struct _STREAM STREAM;
typedef struct _FRAME FRAME;
typedef struct _FRAME_POOL FRAME_POOL;

/* version information */
#define VERSION_FLAG_ARCH_X86			1
#define VERSION_FLAG_ARCH_TRIMEDIA		2
#define VERSION_FLAG_EVALUATION_VERSION	0x80000000

typedef struct _version {
	unsigned 	major;
	unsigned	minor;
	unsigned 	build;
	unsigned	flag;
} version_t;


/* the frame callback */
typedef void (*FRAME_CB)(FRAME *curr_frame, FRAME *reco_frame, int frame_type, int bits, void *context);

/* the PutByte function */
typedef void (*PUTBYTE_CB)(BYTE c, void *context);

/* the grabber callback's */
typedef FRAME *(*GRABBER_CB)(STREAM *s, FRAME *f, int skip_frames, void *context);
typedef void (*GRAB_INIT_CB)(int w, int h, int skip_rate_s, int skip_rate_t, void *context);

/* decoder callbycks */
typedef int	(*GETBYTE_CB)(void *context);

/* layer change callback */
typedef FRAME *(*LAYER_CB)(int r_layer, int stream, void *context);

/* define the PEI load callback */
typedef void (*PEI_LOAD_CB)(BYTE **p, int *size, void *context);

/* RTP send packet callback */
typedef struct _RTP_SEND_CB_PARAM {
	unsigned	tr;				/* time reference of the frame this packet belongs to */
	unsigned	user_data;		/* the user data value of the frame this packet belongs to */
} RTP_SEND_CB_PARAM;

typedef void (*RTP_SEND_CB)(BYTE *packet, int size, RTP_SEND_CB_PARAM *param, void *context);

#endif /* __CODEC_H__ */

