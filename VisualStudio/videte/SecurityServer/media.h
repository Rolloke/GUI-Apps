/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/media.h $
// CHECKIN:		$Date: 16.01.04 12:12 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 16.01.04 12:10 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MEDIA_H_
#define __MEDIA_H_

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Definitionen für Media Struktur           
//
// Status-word(bitmask) der Struktur

class CMediaGroup;
//////////////////////////////////////////////////////////////////////
class CMedia
{
	// construction / destruction
public:
	CMedia(WORD wGroupID, WORD wSubID);
	~CMedia();
	const CMedia& operator=(const CMedia& inp);
	
	// access:
public:
	inline const CSecID		GetID() const;
	inline const CString&	GetName() const;
	inline const CString&	GetType() const;
	
	const bool	IsDefault() const { return m_bIsDefault; } 
	const bool	IsAudio()   const { return m_bIsAudio;   };
	const bool	IsVideo()   const { return m_bIsVideo;   };
	const bool	IsEnabled() const { return m_bIsEnabled; };

	// settings:
public:
	inline void SetName(const CString &sName);
	inline void SetType(const CString &sType);

	// serialization
public:
	BOOL Load(CWK_Profile& wkp);

public:

private:
	CSecID	m_id;			// MediaID
	CString	m_sName;
	CString	m_sType;
	bool     m_bIsDefault;
	bool     m_bIsAudio;
	bool     m_bIsVideo;
	bool     m_bIsEnabled;
};
//////////////////////////////////////////////////////////////////////
const CSecID CMedia::GetID() const
{
	return (m_id);
}
//////////////////////////////////////////////////////////////////////
const CString &CMedia::GetName() const
{
	return (m_sName);
}
//////////////////////////////////////////////////////////////////////
const CString &CMedia::GetType() const
{
	return (m_sType);
}
//////////////////////////////////////////////////////////////////////
inline void CMedia::SetName(const CString &sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
inline void CMedia::SetType(const CString &sType)
{
	m_sType= sType;
}

//////////////////////////////////////////////////////////////////////
typedef CMedia* CMediaPtr;
WK_PTR_ARRAY(CMediaArray,CMediaPtr);
#endif
