// images.h

#ifndef _IMAGES_H
#define _IMAGES_H

#define _IMAGE_HEIGHT	14
#define _IMAGE_WIDTH	20

// Vision: Camera and relays
#define _IMAGE_OFF				0x00
#define _IMAGE_OK				0x01
#define _IMAGE_ALARM			0x02
#define _IMAGE_ERROR			0x03
#define _IMAGE_OPEN				0x04
#define _IMAGE_CLOSE			0x05
#define _IMAGE_CAMERA			0x06
#define _IMAGE_PTZ				0x07
// Both: Hosts
#define _IMAGE_HOSTBASE			0x08
#define _IMAGE_COLOR1			0x08
#define _IMAGE_COLOR2			0x09
#define _IMAGE_COLOR3			0x0A
#define _IMAGE_COLOR4			0x0B
#define _IMAGE_COLOR5			0x0C
#define _IMAGE_COLOR6			0x0D
#define _IMAGE_NR_HOST_COLORS	/**/0x06 // different host colours
// Vision invalid inputs
#define _IMAGE_OK_GRAY			0x0E
#define _IMAGE_ALARM_GRAY		0x0F
// Vision Audio
#define _IMAGE_AUDIO_PLAY		0x10
#define _IMAGE_AUDIO_REC		0x11
#define _IMAGE_AUDIO_PLAY_DEF	0x12
#define _IMAGE_AUDIO_REC_DEF	0x13
#define _IMAGE_AUDIO_PLAY_OFF	0x14
#define _IMAGE_AUDIO_REC_OFF	0x15
// Recherche
#define _IMAGE_ARCH_RING		0x16
#define _IMAGE_ARCH_ALARM		0x17
#define _IMAGE_ARCH_PRE_ALARM	0x18
#define _IMAGE_ARCH_BACKUP		0x19
#define _IMAGE_ARCH_SUM			0x1A
#define _IMAGE_ARCH_SEARCH		0x1B
#define _IMAGE_ARCH_SUSPECT		0x1C
#define _IMAGE_ARCH_ALARMLIST	0x1D
// Both
#define _IMAGE_INVALID			0xFF

#endif