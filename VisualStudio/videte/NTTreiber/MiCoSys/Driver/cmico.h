////////////////////////////////////////////////////////////////////////////
//	Project:	MiCoISA
//
//	File:		cmico.h 
//
//	Description:		
//
//	Start:		30.11.95
//
//	Last change:01.12.95
//
//	Author:		Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////
   
#ifndef __CMICO_H__
#define __CMICO_H__


#define MICO_VERSION    0x0001
#define WM_MICO_ALARM				MICO_MSGBASE + 0
#define WM_MICO_VXD_COMPRESSED			MICO_MSGBASE + 1
#define WM_MICO_VXD_UNCOMPRESSED		MICO_MSGBASE + 2
#define WM_MICO_VXDTESTMESSAGE			MICO_MSGBASE + 3
#define WM_MICO_VXD_LOST_FIELD			MICO_MSGBASE + 4
#define WM_MICO_VXD_UNVALID_FIELD		MICO_MSGBASE + 5
#define WM_MICO_VXD_UNVALID_BUFFER		MICO_MSGBASE + 6

// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	MICO_SOURCE0				0
#define	MICO_SOURCE1				1
#define	MICO_SOURCE2				2
#define	MICO_SOURCE3				3
#define	MICO_SOURCE4				4
#define	MICO_SOURCE5				5
#define	MICO_SOURCE6				6
#define	MICO_SOURCE7				7
#define MICO_MAX_SOURCE				MICO_SOURCE7
#define MICO_SOURCE_NO				255

// Erweiterungskarten  (Müssen von 0 an aufsteigend nummeriert sein!!)
#define MICO_EXTCARD0				0
#define MICO_EXTCARD1				1
#define MICO_EXTCARD2				2
#define MICO_EXTCARD3				3
#define MICO_EXTCARD_MEGRA			4
#define MICO_MAX_EXTCARD			MICO_EXTCARD_MEGRA
#define MICO_EXTCARD_NO				255

#define MICO_MAX_CAM	MICO_MAX_EXTCARD * (MICO_MAX_SOURCE + 1)

// Mico Frontends
#define MICO_FE0					0
#define MICO_FE1					1
#define MICO_MAX_FE					2

// Fields
#define EVEN_FIELD					0
#define ODD_FIELD					1

// Filtertyp
#define MICO_NOTCH_FILTER_ON		1
#define MICO_NOTCH_FILTER_OFF		2
#define MICO_NOTCH_FILTER_AUTO		3

// Art der Eingangsquelle
#define MICO_FBAS					0
#define MICO_SVHS					1


// VideoFrontEnd-Filter
#define MICO_LNOTCH					1
#define MICO_LDEC					2

// Wertebereich der Helligkeit
#define MICO_MIN_BRIGHTNESS			0
#define MICO_MAX_BRIGHTNESS			255

// Wertebereich des Kontrastes
#define MICO_MIN_CONTRAST			0
#define MICO_MAX_CONTRAST			511

// Wertebereich des Farbsättigung
#define MICO_MIN_SATURATION			0
#define MICO_MAX_SATURATION			511

// Wertebereich des Hue
#define MICO_MIN_HUE				0
#define MICO_MAX_HUE				255

// Wertebereich der horizontalen Skalierung
#define MICO_MIN_HSCALE				0
#define MICO_MAX_HSCALE				65535

// Wertebereich der vertikalen Skalierung
#define MICO_MIN_VSCALE				0
#define MICO_MAX_VSCALE				65535

// Wertebereich der BytesPerFrame
#define MICO_MIN_BPF		5000
#define MICO_MAX_BPF		48000

// Wertebereich für MPI
#define MICO_MIN_FPS				1
#define MICO_MAX_FPS				25


// Bytes pro Frame für die 5 in PicDef.h definierten Kompressionsfaktoren
#define MICO_BPF_1					40000
#define MICO_BPF_2					30000
#define MICO_BPF_3					20000
#define MICO_BPF_4					10000
#define MICO_BPF_5					5000

// Mögliche Transferparameter
#define MICO_DECODER_DATA			1
#define MICO_ENCODER_DATA			2

// Encoder und Decoder Status
#define MICO_ENCODER_ON		1
#define MICO_ENCODER_OFF	2
#define MICO_DECODER_ON		3
#define MICO_DECODER_OFF	4

#define MAX_JPEG_BUFFER		1
#define JPEG_BUFFER_SIZE	65535

// Fehlermeldungen
#define MICO_ERR_NO_ERRROR				 0
#define MICO_ERR_WRONG_FE_ID			 1
#define MICO_ERR_WRONG_SOURCE			 2
#define MICO_ERR_WRONG_EXT_CARD			 3
#define MICO_ERR_WRONG_SOURCE_TYPE		 4
#define MICO_ERR_WRONG_FE_VIDEO_FORMAT   5
#define MICO_ERR_WRONG_BE_VIDEO_FORMAT   6
#define MICO_ERR_WRONG_BE_VIDEO_TYPE	 7
#define MICO_ERR_PARM_OUT_OF_RANGE		 8	
#define MICO_ERR_UNKNOWN_PARAMETER  	 9
#define MICO_ERR_WRONG_FILTER_TYPE		 10
#define MICO_ERR_WRONG_ENCODER_FORMAT	 11
#define MICO_ERR_WRONG_ENCODER_FRAMERATE 12
#define MICO_ERR_WRONG_ENCODER_BPF		 13
#define MICO_ERR_WRONG_AVP_PROZESS		 14
#define MICO_ERROR						0xffff

// Filtertyp
#define MICO_NOTCH_FILTER_ON		1
#define MICO_NOTCH_FILTER_OFF		2
#define MICO_NOTCH_FILTER_AUTO		3


#endif // __CMICO_H__

