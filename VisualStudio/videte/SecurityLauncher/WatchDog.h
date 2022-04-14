/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WatchDog.h $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/WatchDog.h $
// CHECKIN:		$Date: 8.06.06 14:55 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 8.06.06 14:53 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ifndef SETBIT 
#define SETBIT(w,b)    ((WORD)((WORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((WORD)((WORD)(w) & ~(1L << (b))))
#endif

#define WD_COCO		1
#define WD_MICO		2
#define WD_JACOB	3
#define WD_SAVIC	4
#define WD_TASHA	5
#define WD_Q		6
#define WD_UNKNOWN	0xff

#define COCO_WD_REL_OUT_OFFSET			0x03
#define COCO_PTC_COUNT0_OFFSET			0x04
#define COCO_PTC_COUNT1_OFFSET			0x05
#define COCO_PTC_COUNT2_OFFSET			0x06
#define COCO_PTC_CONTROL_OFFSET			0x07
#define RELAY3_BIT						0x03
#define WATCHDOG_BIT					0x04
#define SPARE_5							0x06
#define SPARE_6							0x07
#define IO_BASE_SIZE					255

#define MICO_WD_REL_OUT_OFFSET			0x14
#define MICO_PTC_COUNT0_OFFSET			0x1C
#define MICO_PTC_COUNT1_OFFSET			0x1D
#define MICO_PTC_COUNT2_OFFSET			0x1E
#define MICO_PTC_CONTROL_OFFSET			0x1F


// Piezo Flags
#define PF_OFF				0x0000
#define PF_ERROR_REBOOT		0x0001
#define PF_CREATE_DIRECTORY	0x0002
#define PF_APP_NOT_RUNNING	0x0004
#define PF_CRT_ERROR		0x0008
#define PF_WKRT_ERROR		0x0010
#define PF_STM_MANUAL		0x0020
#define PF_NO_VIDEOSIGNAL	0x0040

class CWatchDog
{
	// construction, destruction
public:
	CWatchDog();
	~CWatchDog();
	
public:
	// operations
	BOOL Init(int nCardType, WORD wTime, BOOL bUsePiezo);
	BOOL Reset(WORD wTime);
	BOOL PiezoSetFlag(DWORD dwPiezoFlag);
	BOOL PiezoClearFlag(DWORD dwPiezoFlag);

	BOOL HardwareReboot();
	BOOL Enable();
	BOOL Disable();

	BOOL IsMico()  {return (m_nCardType == WD_MICO);}
	BOOL IsJaCob() {return (m_nCardType == WD_JACOB);}
	BOOL IsCoco()  {return (m_nCardType == WD_COCO);}
	BOOL IsSaVic()  {return (m_nCardType == WD_SAVIC);}
	BOOL IsTasha()  {return (m_nCardType == WD_TASHA);}
	BOOL IsQ() { return (m_nCardType == WD_Q);}

	// data
private:
	BOOL m_bTraceWatchDogTrigger;
	WORD m_wIOBase;
	int  m_nCardType;
	BOOL m_bEnabled;
	BOOL m_bInit;
	BOOL m_bUsePiezo;

	WORD m_wPtcCount0Offset;
	WORD m_wPtcCount1Offset;
	WORD m_wPtcCount2Offset;
	WORD m_wPtcControlOffset;
	WORD m_wWdRelOutOffset;

	DWORD	m_dwPiezoFlags;
	HANDLE  m_hWatchDogTriggerEvent;
	HANDLE  m_hPiezo;

};

#endif // __WATCHDOG_H__