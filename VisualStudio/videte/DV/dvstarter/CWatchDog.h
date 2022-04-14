// CWatchDog.h: interface for the CWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CWATCHDOG_H__6B2452F6_AC6F_11D3_8E06_004005A11E32__INCLUDED_)
#define AFX_CWATCHDOG_H__6B2452F6_AC6F_11D3_8E06_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Piezo Flags
#define PF_OFF				0x0000
#define PF_ERROR_REBOOT		0x0001
#define PF_CREATE_DIRECTORY	0x0002
#define PF_APP_NOT_RUNNING	0x0004
#define PF_CRT_ERROR		0x0008
#define PF_WKRT_ERROR		0x0010
#define PF_STM_MANUAL		0x0020
#define PF_NO_VIDEOSIGNAL	0x0040
#define PF_TEMPERATURE		0x0080
#define PF_ARCHIVE_FULL		0x0100
#define PF_CANTSTORE		0x0200
#define PF_SMART_ALARM		0x0400
#define PF_FAN_ALARM		0x0800
#define PF_HDD_FAIL			0x1000


class CWatchDog  
{
	// construction/destruction
public:
	CWatchDog();
	virtual ~CWatchDog();

	// operations
public:	
	void Disable();
	void Trigger(int nTime);


private:
	HANDLE	m_hJaCobEventWD;
	HANDLE	m_hSaVicEventWD;
	HANDLE  m_hTashaEventWD;
	HANDLE  m_hQEventWD;
};

#endif // !defined(AFX_CWATCHDOG_H__6B2452F6_AC6F_11D3_8E06_004005A11E32__INCLUDED_)
