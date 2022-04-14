#ifndef __ENCODER_H__
#define __ENCODER_H__
/*
 * H.263+ Encoder NT
 *
 * (C) 1997, 1999, 2000 DResearch GmbH
 *
 * Encoder Highlevel functions
 *
 * $Header: /DV/DVProcess/DResearch/encoder.h 1     15.02.01 11:59 Uwe $
 */
#include "codec.h"
#include "framehlp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* possible Enc_Init error codes */
#define ERR_NEED_I			(1)
#define ERR_OK				(0)
#define ERR_MEM				(-1)
#define ERR_FOPEN			(-2)

/* possible Enc_CheckOptions error codes */
#define ERR_PLUS_RES		(-3)
#define ERR_PLUS_ANNEX		(-4)
#define ERR_PLUS_FEAT		(-5)
#define ERR_ANNEX_E_S		(-6)
#define ERR_ANNEX_E_T		(-7)
#define ERR_ANNEX_D_E_PLUS	(-8)
#define ERR_ANNEX_R_P		(-9)
#define ERR_DEMO_UNSUPP		(-10)
#define ERR_CPM_DISABLED	(-11)
#define ERR_PARAM			(-12)
#define ERR_STATIC			(-13)
#define ERR_CONFIG_TO_LATE	(-14)
#define ERR_NO_INIT			(-15)
#define ERR_RTP_MODE		(-16)
#define ERR_RTP_MODE_REQ	(-17)
#define ERR_RTP_PLUS_MODE	(-18)

/* special meaning: CPM mode disabled */
#define NO_CPM_STREAM	(-1)

/* Motion vector search modes */
#define MVS_STEP		'F'
#define MVS_SNOWFLAKE	'S'

/*
 * Purpose:
 *		opens a new encoder instance
 *
 * Returns:
 *		encoder status, NULL iff out-of-memory 
 *
 * Remarks:
 *		set default ptype values = no ANNEX is used, quantization 13, range 1..31
 */
ENC_STATUS *Enc_Open(void);

/*
 * Purpose:
 *		initialise the encoder
 *
 * Parameters:
 *		stat:				encoder status
 *		w, h:				width and height of initial stream
 *		PutByteProc:		PutByte callback
 *		context:			context for the PutByte callback
 *		framefile_name:		ascii name of the file containing the uncompressed frames
 *
 * Returns:
 *		ERR_OK				success
 *		ERR_MEM				memory allocation of the frame pool failed
 *		ERR_FOPEN			one of the files couldn't be opened
 */
int Enc_Init(ENC_STATUS *enc, int w, int h, PUTBYTE_CB PutByteProc, void *context, char *framefile_name);

/*
 * Purpose:
 *		switches the encoder between 4:2:2 and 4:2:0 buffer mode
 *
 * Parameters:
 *		enc:				encoder status
 *		use_422:			TRUE, iff 4:2:2 mode should be used
 *
 * Returns:
 *		ERR_OK				success
 *		ERR_STATIC			switch to static mode failed because running in STATIC mode
 *
 * Remarks:
 *		Note that the encoder runs always in 4:2:0 mode because this is the only supported
 *		mode in H.263v2. However, the FRAME_POOL may be allocated in 4:2:2 allowing to
 *		grab 4:2:2 data directly into frames
 *
 *		Note: the mode must be changed before switching to static frame allocation
 */
int Enc_SetFramePoolMode(ENC_STATUS *enc, int use_422);

/*
 * Purpose:
 *		switches the Encoder to static memory allocation
 *
 * Parameters:
 *		enc:				encoder status
 *		max_w, max_h:		maximum width and height of frames to be encoded
 *
 * Returns:
 *		ERR_OK				success
 *		ERR_STATIC			switch to static mode failed
 *
 * Remarks:
 *		should be called immediately after Enc_Init()
 */
int Enc_SetStatic(ENC_STATUS *enc, int max_w, int max_h);

/* 
 * Purpose:
 *		asynchron stop of the encoder loop
 *
 * Parameters:
 *		enc:	encoder status
 *
 * Remarks:
 *		may be called from Ctrl-C handler 
 */
void Enc_Stop(ENC_STATUS *enc);

/* 
 * Purpose:
 *		terminate all allocated data for encoder and close all files
 *
 * Parameters:
 *		enc:	encoder status
 */
void Enc_Term(ENC_STATUS *enc);

/* 
 * Purpose:
 *		this function implements the encoder loop 
 *
 * Parameter:
 *		enc:		encoder status
 *		max:		numbers of frames to encode
 *
 * Returns:
 *		ERR_OK		success
 *		ERR_MEM		out of frame memory
 */
int Enc_Loop(ENC_STATUS *enc, unsigned max);

/* 
 * Purpose:
 *		report the Encoder error code 
 *
 * Parameters:
 *		buf:		pointer to a char buffer
 *		len:		maximal len of this buffer
 *		err:		error code
 */
void Enc_Error(char *buf, int len, int err);

/* 
 * Purpose:
 *		Encode a H.263+ I-Frame 
 *
 * Parameters:
 *		enc:			encoder status
 *		s:				current stream
 *		target_bits:	target bit count for this frame
 * 
 * Returns:
 *		number of bits spent
 */
int Enc_IFrame(ENC_STATUS *enc, STREAM *s, int target_bits);

/* 
 * Purpose:
 *		Encode a H.263+ P-Frame 
 *
 * Parameters:
 *		enc:			encoder status
 *		s:				current stream
 *		target_bits:	target bit count for this frame
 * 
 * Returns:
 *		number of bits spent
 */
int Enc_PFrame(ENC_STATUS *enc, STREAM *s, int target_bits);

/* 
 * Purpose:
 *		Encode a H.263+ EI-Frame 
 *
 * Parameters:
 *		enc:			encoder status
 *		e:				current enhancement stream
 *		target_bits:	target bit count for this frame
 *
 * Returns:
 *		number of bits spent
 */
int Enc_EIFrame(ENC_STATUS *enc, STREAM *e, int target_bits);

/*
 * Purpose:
 *		change the resolution of a stream and all associated enhancement streams
 *
 * Parameters:
 *		enc:	encoder status
 *		s:		stream
 *		w:		width
 *		h:		height
 *
 * Returns:
 *		TRUE on success, FALSE if resolution is out of contrains
 */
BOOL Enc_ChangeStreamRes(ENC_STATUS *enc, STREAM *s, int w, int h);

/* define some often used cases */
#define CLK_FREQ(Hz)	(1800 / (Hz))
#define CLK_25HZ		CLK_FREQ(25)
#define CLK_50HZ		CLK_FREQ(50)
#define CLK_60HZ		CLK_FREQ(60)

/*
 * the clock conversion factor definition: only these two are allowed
 */
#define CLK_CONV_FACTOR_1000	0
#define CLK_CONV_FACTOR_1001	1

/*
 * Purpose:
 *		change the clock frequency of a stream and all associated enhancement streams
 * 
 * Parameters:
 *		enc:		encoder status
 *		s:			stream
 *		divisor:	clk divisor [0 < divisor < 128], 0 == default 29.97Hz
 *		conv_code:	clock conversion code, either CLK_CONV_FACTOR_1000 or CLK_CONV_FACTOR_1001
 *
 * Returns:
 *		TRUE if successful, FALSE on parameter error
 *
 * Remarks:
 *		The clock frequency is calculated as
 *			1800000 / (divisor * conv_code)
 */
BOOL Enc_ChangeStreamClk(ENC_STATUS *enc, STREAM *s, int divisor, int conv_code);

/*
 * Purpose:
 *		switch the encoder to Anvanced Rate Control (TMN-9)
 *
 * Parameters:
 *		enc:		encoder status
 */
void Enc_AdvRateCtrl(ENC_STATUS *enc);

/*
 * Purpose:
 *		switch the encoder to Alternative Rate Control (TMN-9)
 *
 * Parameters:
 *		enc:		encoder status
 */
void Enc_AltRateCtrl(ENC_STATUS *enc);

/*
 * Purpose:
 *		enable/disable the Rounting TYPE toggling
 *
 * Parameters:
 *		flag:		TRUE, if toggling should be enabled
 *
 * Returns:
 *		old toggling mode
 *
 * Remarks:
 *		Toggling is enabled by default, disabling is against the H.263 standard
 */
BOOL Enc_EnableRTYPE(ENC_STATUS *enc, BOOL flag);

/*
 * Purpose:
 *		set the Reference Picture for the next encoded frame in the given stream
 *
 * Parameters:
 *		enc:		encoder status
 *		s:			stream
 *		ref_frame:	frame selected for Reference Picture
 *
 * Returns:
 *		TRUE on success, FALSE iff Reference Picture Selection mode is NOT enabled
 */
BOOL Enc_SetReferencePicture(ENC_STATUS *enc, STREAM *s, FRAME *ref_frame);

/*
 * Purpose:
 *		restrict the encoder's quantization interval
 *
 * Parameters:
 *		enc:		encoder status
 *		pq_min:		minimal quantizer
 *		pq_max:		maximal quantizer
 *
 * Remarks:
 *		the normal interval is [1..31]
 *		these values are used in advanced rate control mode too
 */
BOOL Enc_SetQuantInterval(ENC_STATUS *enc, int pq_min, int pq_max);

/*
 * Purpose:
 *		set the default quantizer for INTRA and INTER frames
 *
 * Parameters:
 *		enc:		encoder status
 *		i_quant:	default INTRA quantizer
 *		p_quant:	default INTER quantizer
 *
 * Remarks:
 *		both values are clipped to [1..31]
 *		these values are ignored if a bitrate control is activated
 */
void Enc_SetDefaultQuant(ENC_STATUS *enc, int i_quant, int p_quant);

/*
 * Purpose:
 *		set the desired bitrate for a bitrate control
 *
 * Parameters:
 *		enc:		encoder status
 *		bitrate:	desired bitrate
 */
void Enc_SetBitrate(ENC_STATUS *enc, int bitrate);

/*
 * Purpose:
 *		set the desired frame skiprate
 *
 * Parameters:
 *		enc:		encoder status
 *		skiprate:	desired skiprate
 */
void Enc_SetTargetSkipRate(ENC_STATUS *enc, int skiprate);

/*
 * Purpose:
 *		set the source frame skiprate
 *
 * Parameters:
 *		enc:		encoder status
 *		skiprate:	desired skiprate
 *
 * Remarks:
 *		only needed if reading from framefile
 */
void Enc_SetSourceSkipRate(ENC_STATUS *enc, int skiprate);

/*
 * Purpose:
 *		sets the INTRA-frame interval
 *
 * Parameter:
 *		enc:		encoder status
 *		val:		interval value
 *
 * Remarks:
 *		after every val frames, the encoder inserts an INTRA-frame
 */
void Enc_SetIntraInterval(ENC_STATUS *enc, int val);

/*
 * Purpose:
 *		sets the motion vector search range
 *
 * Parameter:
 *		enc:		encoder status
 *		range:		motion vector serach range in halfpixel units
 *
 * Remarks:
 *		depending on the current setup, the motion vector restrictions may be even more
 *		restricted
 */
void Enc_SetMotionVectorSearchRange(ENC_STATUS *enc, int range);

/*
 * Purpose:
 *		select one motion search algorithm
 *
 * Parameters:
 *		mode:	either MVS_STEP or MVS_SNOWFLAKE
 */
void Enc_SetMotionVectorSearchMode( int mode );

/*
 * Purpose:
 *		sets the GOB/Slice-Header Synchronization point interval
 *
 * Parameter:
 *		enc:		encoder status
 *		val:		insert a GOB/Slice Synchronization point at every val GOB-border
 *
 * Remarks:
 *		in the current implementation, Slices take always complete GOB's
 */
void Enc_SetSyncPointInterval(ENC_STATUS *enc, int val);

/*
 * Purpose:
 *		returns the current stream
 *
 * Parameter:
 *		enc:		encoder status
 */
STREAM *Enc_GetCurrentStream(ENC_STATUS *enc);

/*
 * Purpose:
 *		returns the current stream ID
 *
 * Parameter:
 *		enc:		encoder status
 */
int Enc_GetCurrentStreamID(ENC_STATUS *enc);

/*
 * Purpose:
 *		switch the current stream
 *
 * Parameter:
 *		enc:			encoder status
 *		next_stream:	switch to this stream
 *
 * Returns:
 *		ERR_ code
 */
int Enc_SwitchStream(ENC_STATUS *enc, int next_stream);

/*
 * Purpose:
 *		allocate an unique BIT-BUFFER for a stream
 *
 * Parameters:
 *		enc:			encoder status
 *		stream:			stream id
 *		PutByteProc:	PutByte callback
 *		context:		context for the PutByte callback
 *
 * Returns:
 *		ERR_OK on success
 */
int Enc_AllocateStreamBuffer(ENC_STATUS *enc, int stream, PUTBYTE_CB PutByteProc, void *context);

/*
 * Purpose:
 *		deallocate an unique BIT-BUFFER for a stream and switches back to the default BIT-BUFFER
 *
 * Parameters:
 *		enc:		encoder status
 *		stream:		stream id
 *
 * Returns:
 *		ERR_OK on success
 */
int Enc_DeallocateStreamBuffer(ENC_STATUS *enc, int stream);

/*
 * definition of PLUSMODE selection
 */
#define AUTO_PLUSMODE	0
#define FORCE_PLUSMODE	1
#define FORBID_PLUSMODE	2

/* 
 * Purpose:
 *		check, if all selected options work together and configure encoder
 *
 * Parameter:
 *		enc:		encoder status
 *		w, h:		width and heigth of the stream
 *		freq:		possible custom picture clock frequency in Hz
 *		plus_mode:	wheater or not PLUSMODE should be used
 *
 * Returns:
 *		ERR_OK or ERR_* may set optional error msg
 */
int Enc_ConfigureOptions(ENC_STATUS *enc, int w, int h, int freq, int plus_mode, char **err_msg);

/*
 * Purpose:
 *		enforce an I-Frame on this stream
 *
 * Parameters:
 *		enc:		encoder status
 *		stream:		stream_id or -1 for all streams
 */
int Enc_ForceIntraUpdate(ENC_STATUS *enc, int stream);

/* 
 * Purpose:
 *		parse the annex string and set the Annexes
 *
 * Parameters:
 *		enc:	encoder status
 *		s:		string holding the requested annexes, eg "DEF"
 *
 * Returns:
 *		NULL on success, else the first unsupported Annex (description)
 */
char *Enc_ParseAnnex(ENC_STATUS *enc, char *s );

/*
 * Purpose:
 *		set the frame-completion callback
 *
 * Parameter:
 *		enc:		encoder status
 *		cb:			new frame callback or NULL for disable the callback
 *		context:	context parameter
 *
 * Returns:
 *		the old callback function
 *
 * Remarks:
 *		the frame callback is called by Enc_Loop(), whenever a frame was fully encoded
 */
FRAME_CB Enc_SetFrameCB(ENC_STATUS *enc, FRAME_CB cb, void *context);

/*
 * Purpose:
 *		set the grabber callback
 *
 * Parameter:
 *		enc:		encoder status
 *		cb:			new grabber callback or NULL for disable the callback
 *		context:	context parameter
 *
 * Returns:
 *		the old callback function
 *
 * Remarks:
 *		the grabber callback is called by Enc_Loop(), whenever a frame must be read from the VideoIn
 */
GRABBER_CB Enc_SetGrabberCB(ENC_STATUS *enc, GRABBER_CB cb, void *context);

/*
 * Purpose:
 *		set the grabber init callback
 *
 * Parameter:
 *		enc:		encoder status
 *		cb:			new grabber init callback or NULL for disable the callback
 *		context:	context parameter
 *
 * Returns:
 *		the old callback function
 *
 * Remarks:
 *		the grabber init callback is called by Enc_Loop(), whenever a video parameter is about to change
 */
GRAB_INIT_CB Enc_SetGrabInitCB(ENC_STATUS *enc, GRAB_INIT_CB cb, void *context);

/*
 * Purpose: 
 *		get timing reference
 *
 * Parameters:
 *		s:		current stream
 *
 * Returns:
 *		the current time reference for this stream
 *
 * Remarks:
 *		the time reference depends on the streams clock freq		
 */
DWORD Enc_GetTR(STREAM *s);

/*
 * Purpose:
 *		enable the Fast INTER-frame encoder
 *
 * Parameters:
 *		enc:		encoder status 
 *		flag:		TRUE, iff the Fast INTER-frame encoder should be enabled
 *
 * Remarks:
 *		Note, that the fast INTER-frame encoder has several restrictions
 */
void Enc_EnableFastINTER(ENC_STATUS *enc, BOOL flag);

/*
 * Purpose:
 *		retrieve the frame pool handle of an encoder
 *
 * Parameters:
 *		enc:		encoder status 
 *
 * Returns:
 *		frame pool handle
 */
FRAME_POOL *Enc_GetFramePool(ENC_STATUS *enc);

/*
 * Purpose:
 *		set a new threshold value for the NOT-CODED block decision
 *
 * Parameters:
 *		enc:		encoder status 
 *		threshold:	new threshold value
 *
 * Returns:
 *		old threshold value
 */
int Enc_SetNotCodedSADThreshold(ENC_STATUS *enc, int threshold);

/*
 * Purpose:
 *		sets the maximal frame skip value for the rate control
 *
 * Parameters:
 *		enc:			encoder status 
 *		max_frame_skip:	new maximum frame skip
 *
 * Returns:
 *		old threshold value
 */
int Enc_MaxFrameSkip(ENC_STATUS *enc, int max_frame_skip);

/* 
 * Purpose:
 *		get a testframe created by the encoder instead of grabbing one
 *
 * Parameters:
 *		s:			current stream
 *		f:			current frame
 *		time_ref:	time reference for this frame
 *
 * Returns:
 *		created test frame
 */
FRAME *Enc_GetTestFrame( STREAM *s, FRAME *f, unsigned time_ref );

/*
 * Purpose:
 *		set/reset the "force INTRA frame when resolution changes" flag
 *
 * Parameters:
 *		enc:			encoder status 
 *		flag:			new flag value
 *
 * Returns:
 *		old value
 */
BOOL Enc_ForceIntraOnResChange(ENC_STATUS *enc, BOOL flag);

/*
 * Purpose:
 *		returns the current encoder version and build
 *
 * Parameters:
 *		version:		pointer to a version_t that will be filled
 */
void Enc_GetVersionInfo(version_t *version);

/*
 * Purpose:
 *		returns a bitmask of supported annexes, bit0 == Annex A, bit1 == Annex B etc.
 */
DWORD Enc_GetSupportedAnnexes(void);

/*
 * Purpose:
 *		set the PEI load callback
 *
 * Parameter:
 *		enc:		encoder status
 *		cb:			new PEI load callback or NULL for restoring the default (Annex L compatible) callback
 *		context:	context parameter
 *
 * Returns:
 *		the old callback function
 *
 * Remarks:
 *		the PEI load callback is called by Enc_Loop(), whenever a new frame is encoded
 */
PEI_LOAD_CB Enc_SetPEIloadCB(ENC_STATUS *enc, PEI_LOAD_CB cb, void *context);

/*
 * Purpose:
 *		set the maximum age of an INTER block
 *
 * Parameter:
 *		enc:		encoder status
 *		max_age:	maximum age of an INTER block
 *
 * Remarks:
 *		every block of a frame is updated as an INTRA block at least every
 *		max_age frame
 */
void Enc_SetMaxAge(ENC_STATUS *enc, unsigned max_age);

/* RTP layer flags */
#define RTP_LAYER_SYNC_PSC			1
#define RTP_LAYER_SYNC_GOB			2
#define RTP_LAYER_SYNC_EOS			4

/*
 * Purpose:
 *		enables the RTP layer
 *
 * Parameters:
 *		enc:			encoder status
 *		rtp_flags:		RTP_LAYER_* flags
 *		packet_size:	RTP packet size
 *		sendfunc:		RTP packet send function
 *		context:		send function context
 *
 * Returns:
 *		ERR_CONFIG_TO_LATE:		called after Enc_Init
 *
 * Remarks:
 *		must be called before Enc_Init
 */
int Enc_EnableRTPlayer(ENC_STATUS *enc, int rtp_flags, int packet_size, RTP_SEND_CB sendfunc, void *context);

#ifdef __cplusplus
}
#endif

#endif
