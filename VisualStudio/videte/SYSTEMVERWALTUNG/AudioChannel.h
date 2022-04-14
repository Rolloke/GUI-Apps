/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: AudioChannel.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/AudioChannel.h $
// CHECKIN:		$Date: 12.01.04 15:23 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 12.01.04 15:06 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __AUDIO_CHANNEL_H_
#define __AUDIO_CHANNEL_H_

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Definitionen für Input Struktur           
//

//////////////////////////////////////////////////////////////////////
class CAudioChannel
{
	// construction / destruction
public:
	CAudioChannel(LPCTSTR sChannel, int nChannel, int nStereoChannel, int nUnit, LPCTSTR sDeviceName, bool bInput);
	~CAudioChannel();
	const CAudioChannel& operator=(const CAudioChannel& inp);
	
	// access:
public:
	inline const bool       IsStereo()         const { return m_bStereo;};
	inline const bool       IsInput()          const { return m_bInput;};
	inline const bool       IsDefault()        const { return m_bDefault;};
	inline const int        GetStereoChannel() const { return m_nStereoChannel;};
	inline const int        GetChannel()       const { return m_nChannel;};
	inline const int        GetUnit()          const { return m_nUnit;};

	const CString&	GetName(int nC);
	bool     IsActive(int nC);
	bool     IsFixed(int nC);
	bool     IsDisabled(int nC);

	// settings:
public:
	inline void SetStereoChannel(int nStereoChannel) { m_nStereoChannel = nStereoChannel;};
	inline void SetDefault(bool bDefault)            { m_bDefault       = bDefault;};
	
	void SetName(int nC, const CString &sName);
	void SetActive(int nC, bool bActive);

	// Implementation
public:
	void Load(CWK_Profile &);
	void Save(CWK_Profile &);


public:
	CString  m_sChannel;
	CString  m_sDeviceName;

private:
	int      m_nUnit;
	int      m_nChannel;
	int      m_nStereoChannel;

	CString	m_sNameLeft;
	CString	m_sNameRight;
	bool     m_bDefault;
	bool     m_bStereo;
	bool		m_bInput;
	BOOL     m_bActive;
};
//////////////////////////////////////////////////////////////////////
typedef CAudioChannel* CAudioChannelPtr;
WK_PTR_ARRAY(CAudioChannelArray,CAudioChannelPtr);

#endif // __AUDIO_CHANNEL_H_
